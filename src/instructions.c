#include "instructions.h"
#include <stdio.h>

const PrimaryFn primary_jump_table[INSTRUCTION_SIZE] = {
    [0x6] = op_6xkk,
    [0xA] = op_Annn,
    [0xD] = op_Dxyn,
    [0x8] = op_8xxx,

};

const SecondaryFn secondary_jump_table[INSTRUCTION_SIZE] = {
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
