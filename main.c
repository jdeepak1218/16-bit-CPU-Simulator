#include<stdio.h>
#include <inttypes.h>
#include "cpu.h"
#include "instruction.h"
#include<time.h>
#include <stdint.h>
extern int assemble(const char *source, uint16_t *output, int max_words);
extern void cpu_run(CPU *cpu);
extern void cpu_init(CPU *cpu);
extern void mem_write16(CPU *cpu,uint16_t address,uint16_t value);
int main()
{
  CPU cpu;
  cpu_init(&cpu);
  const char *program =
      // store array in memory at address 200
      // array = {64, 25, 12, 22, 11}
      "LOAD R0, 200\n"
      "LOAD R1, 64\n"
      "STR R0, R1\n"
      "LOAD R3, 2\n"
      "ADD R0, R3\n"
      "LOAD R1, 25\n"
      "STR R0, R1\n"
      "ADD R0, R3\n"
      "LOAD R1, 12\n"
      "STR R0, R1\n"
      "ADD R0, R3\n"
      "LOAD R1, 22\n"
      "STR R0, R1\n"
      "ADD R0, R3\n"
      "LOAD R1, 11\n"
      "STR R0, R1\n"

      // bubble sort
      // R6 = outer loop counter (5 passes)
      // R7 = inner loop pointer
      "LOAD R6, 0\n"      // outer counter = 0
      "LOAD R4, 5\n"      // outer limit = 5
      "LOAD R3, 2\n"      // step size

      "outer:\n"
      "CMP R6, R4\n"      // outer counter == 5?
      "JZ done\n"
      "LOAD R7, 200\n"    // inner pointer = start of array
      "LOAD R5, 208\n"    // inner limit = 206 (last pair starts at 206)

      "inner:\n"
      "CMP R7, R5\n"      // reached end of inner loop?
      "JZ outernext\n"
      "LDR R0, R7\n"      // R0 = arr[i]
      "LOAD R1, 2\n"
      "ADD R1, R7\n"      // R1 = address of arr[i+1]
      "LDR R2, R1\n"      // R2 = arr[i+1]
      "CMP R0, R2\n"      // arr[i] > arr[i+1]?
      "JN noswap\n"       // if negative no swap needed
      "JZ noswap\n"       // if equal no swap needed
      "STR R7, R2\n"      // arr[i] = arr[i+1]
      "STR R1, R0\n"      // arr[i+1] = arr[i]
      "noswap:\n"
      "ADD R7, R3\n"      // move to next pair
      "JMP inner\n"

      "outernext:\n"
      "LOAD R1, 1\n"
      "ADD R6, R1\n"      // outer counter++
      "JMP outer\n"

      "done:\n"
      // load sorted array back into registers to print
      "LOAD R0, 200\n"
      "LDR R1, R0\n"      // R1 = arr[0]
      "LOAD R2, 2\n"
      "ADD R0, R2\n"
      "LDR R2, R0\n"      // R2 = arr[1]
      "LOAD R3, 2\n"
      "ADD R0, R3\n"
      "LDR R3, R0\n"      // R3 = arr[2]
      "LOAD R4, 2\n"
      "ADD R0, R4\n"
      "LDR R4, R0\n"      // R4 = arr[3]
      "LOAD R5, 2\n"
      "ADD R0, R5\n"
      "LDR R5, R0\n"      // R5 = arr[4]
      "HALT\n";
  uint16_t buffer[32768];
  int words = assemble(program,buffer,32768);
  for(int i = 0 ; i < words ; i++ )
  {
    mem_write16(&cpu,i*2,buffer[i]);
  }
  clock_t start = clock();
  cpu_run(&cpu);
  clock_t end = clock();
  double time_take = ((double)(end - start))/CLOCKS_PER_SEC * 1000000;
  for(int i = 0; i < 8; i++) {
      printf("R%d = %d\n", i, cpu.registers[i]);
  }
  printf("Cycles = %" PRIu64 "\n", cpu.cycles);
  printf("Time taken = %f\n",time_take);
  return 0;
}
