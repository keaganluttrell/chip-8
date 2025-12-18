#include <stdint.h>
#include <stdio.h>
#define MEMORY 4096
#define REGISTER_LEN 16
#define STACK_LEN 16
#define INSTRUCTION_SIZE 16
#define FONT_SET_LEN 80
#define ROM_START 512
#define ROM_LEN (MEMORY - ROM_START)
#define RESOLUTION 64 * 32

typedef enum { FN_SUCCESS = 0, FN_ERROR, FN_LEN } RETURN_CODE;

uint8_t fontset[FONT_SET_LEN] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct Chip8 {
  uint8_t memory[MEMORY];
  uint8_t V[REGISTER_LEN];
  uint8_t stack_pointer;
  uint8_t delay_counter;
  uint8_t sound_counter;
  uint16_t I;
  uint16_t pc;
  uint16_t stack[REGISTER_LEN];
  char display[RESOLUTION];
} Chip8;

typedef void (*PrimaryFn)(Chip8 *c8, uint16_t opcode);
PrimaryFn primary_jump_table[INSTRUCTION_SIZE];

uint8_t chip8_load_font(Chip8 *c8) {
  if (c8 == NULL) {
    printf("Invalid pointer: Chip8\n");
    return FN_ERROR;
  }

  for (uint8_t i = 0; i < FONT_SET_LEN; i++) {
    c8->memory[i] = fontset[i];
  }
  return FN_SUCCESS;
}

uint8_t chip8_load_rom(Chip8 *c8, char *filename) {
  if (c8 == NULL) {
    printf("Invalid pointer: Chip8\n");
    return FN_ERROR;
  }

  FILE *f_ptr = fopen(filename, "rb");
  if (f_ptr == NULL) {
    printf("No file found: %s\n", filename);
    return FN_ERROR;
  }

  fseek(f_ptr, 0, SEEK_END);

  long file_size = ftell(f_ptr);
  if (file_size > ROM_LEN) {
    printf("File %s is too large\n", filename);
    return FN_ERROR;
  }

  fseek(f_ptr, 0, SEEK_SET);

  fread(&c8->memory[ROM_START], sizeof(uint8_t), file_size, f_ptr);

  fclose(f_ptr);
  return FN_SUCCESS;
}

uint8_t chip8_init(Chip8 *c8) {
  if (c8 == NULL) {
    printf("Invalid pointer: Chip8\n");
    return FN_ERROR;
  }

  for (uint16_t i = 0; i < MEMORY; i++) {
    c8->memory[i] = 0;
  }

  for (uint8_t j = 0; j < REGISTER_LEN; j++) {
    c8->V[j] = 0;
  }

  for (uint8_t k = 0; k < STACK_LEN; k++) {
    c8->stack[k] = 0;
  }

  for (int l = 0; l < RESOLUTION; l++) {
    c8->display[l] = 0;
  }

  c8->stack_pointer = 0;
  c8->delay_counter = 0;
  c8->sound_counter = 0;
  c8->pc = 512;
  c8->I = 0;

  uint8_t load_code = chip8_load_font(c8);

  if (load_code != FN_SUCCESS) {
    return FN_ERROR;
  }

  return FN_SUCCESS;
}

int main() {
  uint16_t opcode = (0x000 << 8) | 0x11F;
  printf("Hello World: %d\n", opcode);
  return 0;
}
