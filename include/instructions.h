#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#define INSTRUCTION_SIZE 16
#include "chip8.h"

typedef void (*TableFn)(Chip8 *c8, uint16_t opcode);

extern const TableFn primary_table[INSTRUCTION_SIZE];
extern const TableFn secondary_table_8[INSTRUCTION_SIZE];

/* Primary Table Dispatchers */
void op_0xxx(Chip8 *c8, uint16_t opcode);
void op_1nnn(Chip8 *c8, uint16_t opcode);
void op_2nnn(Chip8 *c8, uint16_t opcode);
void op_3xkk(Chip8 *c8, uint16_t opcode);
void op_4xkk(Chip8 *c8, uint16_t opcode);
void op_5xy0(Chip8 *c8, uint16_t opcode);
void op_6xkk(Chip8 *c8, uint16_t opcode);
void op_7xkk(Chip8 *c8, uint16_t opcode);
void op_8xxx(Chip8 *c8, uint16_t opcode);
void op_9xy0(Chip8 *c8, uint16_t opcode);
void op_Annn(Chip8 *c8, uint16_t opcode);
void op_Bnnn(Chip8 *c8, uint16_t opcode);
void op_Cxkk(Chip8 *c8, uint16_t opcode);
void op_Dxyn(Chip8 *c8, uint16_t opcode);
void op_Exxx(Chip8 *c8, uint16_t opcode);
void op_Fxxx(Chip8 *c8, uint16_t opcode);

/* Secondary Table 8 */
void op_8xy0(Chip8 *c8, uint16_t opcode);
void op_8xy1(Chip8 *c8, uint16_t opcode);
void op_8xy2(Chip8 *c8, uint16_t opcode);
void op_8xy3(Chip8 *c8, uint16_t opcode);
void op_8xy4(Chip8 *c8, uint16_t opcode);
void op_8xy5(Chip8 *c8, uint16_t opcode);
void op_8xy6(Chip8 *c8, uint16_t opcode);
void op_8xy7(Chip8 *c8, uint16_t opcode);
void op_8xyE(Chip8 *c8, uint16_t opcode);

/* Secondary Functions */
void op_00E0(Chip8 *c8);
void op_00EE(Chip8 *c8);

void op_Ex9E(Chip8 *c8, uint16_t opcode);
void op_ExA1(Chip8 *c8, uint16_t opcode);

void op_Fx07(Chip8 *c8, uint16_t opcode);
void op_Fx0A(Chip8 *c8, uint16_t opcode);
void op_Fx15(Chip8 *c8, uint16_t opcode);
void op_Fx18(Chip8 *c8, uint16_t opcode);
void op_Fx1E(Chip8 *c8, uint16_t opcode);
void op_Fx29(Chip8 *c8, uint16_t opcode);
void op_Fx33(Chip8 *c8, uint16_t opcode);
void op_Fx55(Chip8 *c8, uint16_t opcode);
void op_Fx65(Chip8 *c8, uint16_t opcode);

#endif // !INSTRUCTIONS_H
