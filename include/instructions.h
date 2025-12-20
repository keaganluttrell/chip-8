#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#define INSTRUCTION_SIZE 16
#include "chip8.h"

typedef void (*PrimaryFn)(Chip8 *c8, uint16_t opcode);
typedef void (*SecondaryFn)(Chip8 *c8, uint16_t opcode);

extern const PrimaryFn primary_jump_table[INSTRUCTION_SIZE];
extern const SecondaryFn secondary_jump_table[INSTRUCTION_SIZE];

/* Primary Functions */
void op_Annn(Chip8 *c8, uint16_t opcode);
void op_Dxyn(Chip8 *c8, uint16_t opcode);
void op_6xkk(Chip8 *c8, uint16_t opcode);
void op_8xxx(Chip8 *c8, uint16_t opcode);

/* Secondary Functions */
void op_8xy4(Chip8 *c8, uint16_t opcode);
void op_8xy5(Chip8 *c8, uint16_t opcode);
void op_8xy7(Chip8 *c8, uint16_t opcode);
#endif // !INSTRUCTIONS_H
