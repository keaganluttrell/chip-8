#include "instructions.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const TableFn primary_table[INSTRUCTION_SIZE] = {
    op_0xxx, op_1nnn, op_2nnn, op_3xkk, op_4xkk, op_5xy0, op_6xkk, op_7xkk,
    op_8xxx, op_9xy0, op_Annn, op_Bnnn, op_Cxkk, op_Dxyn, op_Exxx, op_Fxxx,
};

const TableFn secondary_table_8[INSTRUCTION_SIZE] = {
    op_8xy0, op_8xy1, op_8xy2, op_8xy3, op_8xy4, op_8xy5, op_8xy6, op_8xy7, [14] = op_8xyE,
};

/* PRIMARY FUNCTIONS */
void op_00E0(Chip8 *c8) { memset(c8->display, 0, sizeof(c8->display)); }

void op_00EE(Chip8 *c8) {
  c8->stack_pointer--;
  c8->pc = c8->stack[c8->stack_pointer];
}

void op_0xxx(Chip8 *c8, uint16_t opcode) {
  uint16_t last_three = opcode & 0x0FFF;
  if (last_three == 0x00E0) {
    op_00E0(c8);
  } else if (last_three == 0x00EE) {
    op_00EE(c8);
  }
}

void op_1nnn(Chip8 *c8, uint16_t opcode) { c8->pc = opcode & 0x0FFF; }

void op_2nnn(Chip8 *c8, uint16_t opcode) {
  c8->stack_pointer++;
  c8->stack[c8->stack_pointer] = c8->pc;
  c8->pc = opcode & 0x0FFF;
}

void op_3xkk(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = (opcode & 0x00FF);
  if (c8->V[x] == kk) {
    c8->pc += 2;
  }
}

void op_4xkk(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = (opcode & 0x00FF);
  if (c8->V[x] != kk) {
    c8->pc += 2;
  }
}

void op_5xy0(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  if (c8->V[x] == c8->V[y]) {
    c8->pc += 2;
  }
}

void op_6xkk(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = (opcode & 0x00FF);
  c8->V[x] = kk;
}

void op_7xkk(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = (opcode & 0x00FF);
  c8->V[x] += kk;
}

void op_8xxx(Chip8 *c8, uint16_t opcode) {
  uint8_t sub_index = opcode & 0x000F;
  if (secondary_table_8[sub_index] != NULL) {
    secondary_table_8[sub_index](c8, opcode);
  }
}

void op_9xy0(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  if (c8->V[x] != c8->V[y]) {
    c8->pc += 2;
  }
}

void op_Annn(Chip8 *c8, uint16_t opcode) { c8->I = opcode & 0x0FFF; }

void op_Bnnn(Chip8 *c8, uint16_t opcode) {
  uint8_t nnn = (opcode & 0x0FFF);
  c8->pc = c8->V[0] + nnn;
}

void op_Cxkk(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = (opcode & 0x00FF);

  c8->V[x] = (rand() % 256) & kk;
}

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

void op_Ex9E(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t key = c8->V[x];
  if (c8->keypad[key] != 0) {
    c8->pc += 2;
  }
}

void op_ExA1(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t key = c8->V[x];
  if (c8->keypad[key] != 1) {
    c8->pc += 2;
  }
}

void op_Exxx(Chip8 *c8, uint16_t opcode) {
  uint8_t last_byte = opcode & 0x00FF;
  if (last_byte == 0x9E) {
    op_Ex9E(c8, opcode);
  } else if (last_byte == 0xA1) {
    op_ExA1(c8, opcode);
  }
}

void op_Fxxx(Chip8 *c8, uint16_t opcode) {
  uint8_t last_byte = opcode & 0x00FF;

  switch (last_byte) {
  case 0x07:
    op_Fx07(c8, opcode);
    break; // LD Vx, DT
  case 0x0A:
    op_Fx0A(c8, opcode);
    break; // LD Vx, K (Wait for key)
  case 0x15:
    op_Fx15(c8, opcode);
    break; // LD DT, Vx
  case 0x18:
    op_Fx18(c8, opcode);
    break; // LD ST, Vx
  case 0x1E:
    op_Fx1E(c8, opcode);
    break; // ADD I, Vx
  case 0x29:
    op_Fx29(c8, opcode);
    break; // LD F, Vx (Font)
  case 0x33:
    op_Fx33(c8, opcode);
    break; // LD B, Vx (BCD)
  case 0x55:
    op_Fx55(c8, opcode);
    break; // LD [I], Vx (Store)
  case 0x65:
    op_Fx65(c8, opcode);
    break; // LD Vx, [I] (Load)
  default:
    printf("Unknown F opcode: 0x%X\n", opcode);
    break;
  }
}

/* SECONDARY TABLE 8 FUNCTIONS */
void op_8xy0(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  c8->V[x] = c8->V[y];
}

void op_8xy1(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  c8->V[x] = c8->V[x] | c8->V[y];
}

void op_8xy2(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  c8->V[x] = c8->V[x] & c8->V[y];
}

void op_8xy3(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  c8->V[x] = c8->V[x] ^ c8->V[y];
}

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

void op_8xy6(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  c8->V[0xF] = (c8->V[x] & 0x01);
  c8->V[x] >>= 1;
}

void op_8xy7(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  uint8_t borrow_flag = (c8->V[y] >= c8->V[x]) ? 1 : 0;
  c8->V[x] = c8->V[y] - c8->V[x];
  c8->V[0xF] = borrow_flag;
}

void op_8xyE(Chip8 *c8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  c8->V[0xF] = (c8->V[x] & 0x80) >> 7;
  c8->V[x] <<= 1;
}
