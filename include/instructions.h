#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#define INSTRUCTION_SIZE 16
#include "chip8.h"

typedef void (*TableFn)(Chip8 *c8, uint16_t opcode);
extern const TableFn primary_table[INSTRUCTION_SIZE];
extern const TableFn secondary_table_8[INSTRUCTION_SIZE];

#endif // !INSTRUCTIONS_H
