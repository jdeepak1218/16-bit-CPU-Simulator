#ifndef CPU_H
#define CPU_H
#include<stdint.h>
#include<stdbool.h>
#define NUM_REGISTER 8
#define MEMORY_SIZE 65535
#define FLAG_ZERO (1 << 0)
#define FLAG_NEGATIVE (1 << 1)
#define FLAG_OVERFLOW (1 << 2)
typedef struct{
  uint16_t registers[NUM_REGISTER];
  uint16_t pc;
  uint16_t sp;
  uint16_t flags;
  uint8_t memory[MEMORY_SIZE];
  bool halted;
  uint64_t cycles;
}CPU;
#endif
