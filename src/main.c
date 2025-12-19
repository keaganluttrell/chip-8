#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
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
typedef void (*SecondaryFn)(Chip8 *c8, uint16_t opcode);

/* SECONDARY FUNCTIONS */
void op_8xy4(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  uint16_t sum = c8->V[x] + c8->V[y];

  if (sum > 255) {
    c8->V[0xF] = 1;
  } else {
    c8->V[0xF] = 0;
  }

  c8->V[x] = (uint8_t)(sum & 0xFF);
}

void op_8xy5(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  uint8_t borrow_flag = (c8->V[x] >= c8->V[y]) ? 1 : 0;
  c8->V[x] = c8->V[x] - c8->V[y];
  c8->V[0xF] = borrow_flag;
}

void op_8xy7(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  uint8_t borrow_flag = (c8->V[y] >= c8->V[x]) ? 1 : 0;
  c8->V[x] = c8->V[y] - c8->V[x];
  c8->V[0xF] = borrow_flag;
}

SecondaryFn secondary_jump_table[INSTRUCTION_SIZE] = {
    [0x4] = op_8xy4,
    [0x5] = op_8xy5,
    [0x7] = op_8xy7,
};

/* PRIMARY FUNCTIONS */
void op_Annn(Chip8 *c8, uint16_t opcode) { c8->I = opcode & 0x0FFF; }

void op_Dxyn(Chip8 *c8, uint16_t opcode) {
  uint8_t x_coord = c8->V[(opcode & 0x0F00) >> 8] % 64;
  uint8_t y_coord = c8->V[(opcode & 0x00F0) >> 4] % 32;
  uint8_t height = opcode & 0x000F;

  c8->V[0xF] = 0;

  for (int row = 0; row < height; row++) {
    uint8_t sprite_byte = c8->memory[c8->I + row];

    for (int col = 0; col < 8; col++) {
      if ((sprite_byte & (0x80 >> col)) != 0) {
        uint16_t screen_x = (x_coord + col) % 64;
        uint16_t screen_y = (y_coord + row) % 32;
        uint16_t index = screen_x + (screen_y * 64);

        if (c8->display[index] == 1) {
          c8->V[0xF] = 1;
        }

        c8->display[index] ^= 1;
      }
    }
  }
}

void op_6xkk(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = opcode & 0x00FF;

  c8->V[x] = kk;
}
void op_8xxx(Chip8 *c8, uint16_t opcode) {
  uint8_t sub_index = opcode & 0x000F;
  if (secondary_jump_table[sub_index] != NULL) {
    secondary_jump_table[sub_index](c8, opcode);
  }
}

PrimaryFn primary_jump_table[INSTRUCTION_SIZE] = {
    [0x6] = op_6xkk,
    [0xA] = op_Annn,
    [0xD] = op_Dxyn,
    [0x8] = op_8xxx,

};

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

void chip8_execute(Chip8 *c8) {
  uint16_t opcode = (c8->memory[c8->pc] << 8) | c8->memory[c8->pc + 1];
  c8->pc += 2;
  uint8_t fn_idx = (opcode & 0xF000) >> 12;

  if (primary_jump_table[fn_idx] != NULL) {
    primary_jump_table[fn_idx](c8, opcode);
  }
}

void chip8_render(Chip8 *c8) {
  printf("\033[H");

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      uint16_t index = x + (y * 64);

      if (c8->display[index]) {
        printf("██");
      } else {
        printf("  ");
      }
    }
    printf("\n");
  }
}

int main() {
  Chip8 c8;
  chip8_init(&c8);
  uint8_t instructions_per_frame = 10;

  while (1) {
    for (int i = 0; i < instructions_per_frame; i++) {
      chip8_execute(&c8);
    }

    chip8_render(&c8);
    usleep(16666);
    c8.delay_counter > 0 ? c8.delay_counter-- : 0;
    c8.sound_counter > 0 ? c8.sound_counter-- : 0;
  }
  return 0;
}
