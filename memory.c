#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "cpu.h"
void cpu_init(CPU *cpu)
{
  memset(cpu->registers,0,sizeof(cpu->registers));
  memset(cpu->memory,0, sizeof(cpu->memory));
  cpu->pc = 0x0000;
  cpu->sp = 0xFFFF;
  cpu->flags = 0;
  cpu->halted = false;
  cpu->cycles = 0;
}
uint16_t mem_read16(CPU *cpu,uint16_t address)
{
  uint16_t low = cpu->memory[address];
  uint16_t high = cpu->memory[address+1];
  return (high << 8) | low;
}
void mem_write16(CPU *cpu,uint16_t address,uint16_t value)
{
  cpu->memory[address] = (value & 0xFF);
  cpu->memory[address+1] = ((value >> 8) & (0xFF));
}
