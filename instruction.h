#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include<stdint.h>
#include "opcodes.h"
#define DECODE_OPCODE(instr) ((Opcode)(((instr) >> 11) & 0x1F))
#define DECODE_DST(instr) (((instr) >> 8) & 0x7)
#define DECODE_SRC(instr) (((instr) >> 5) & 0x7)
#define DECODE_IMM5(instr) ((instr) & 0x1F)
#define DECODE_ADDR(instr) ((instr)& 0x7FF)
static inline uint16_t ENCODE_REG(Opcode op,uint8_t dest,uint8_t src,uint8_t imm5)
{
  return ((uint16_t)((op & 0x1F) << 11)) | ((uint16_t)((dest & 0x7) << 8)) | ((uint16_t)((src & 0x7) << 5)) | (imm5 & 0x1F);
}
static inline uint16_t ENCODE_JUMP(Opcode op, uint16_t address)
{
  return ((uint16_t)((op & 0x1F) << 11)) | (address & 0x7FF);
}
#endif
