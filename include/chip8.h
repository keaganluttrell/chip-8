#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#define MEMORY 4096
#define REGISTER_LEN 16
#define STACK_LEN 16
#define RESOLUTION 64 * 32
#define ROM_START 512
#define ROM_LEN (MEMORY - ROM_START)
#define FONT_SET_LEN 80

extern const uint8_t fontset[FONT_SET_LEN];
typedef enum { FN_SUCCESS = 0, FN_ERROR, FN_LEN } RETURN_CODE;

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

uint8_t chip8_load_font(Chip8 *c8);
uint8_t chip8_load_rom(Chip8 *c8, char *filename);
uint8_t chip8_init(Chip8 *c8);
void chip8_execute(Chip8 *c8);
void chip8_render(Chip8 *c8);

#endif // !CHIP8_H
