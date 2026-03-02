  #include <stdint.h>
  #include <stdio.h>
  #include "cpu.h"
  #include "opcodes.h"
  void update_flags(CPU *cpu, uint16_t result,uint16_t a,uint16_t b,Opcode op)
  {
    cpu->flags = 0;
    if(result == 0) cpu->flags |= FLAG_ZERO;
    if(result & 0x8000) cpu->flags |= FLAG_NEGATIVE;
    int a_neg = (a & 0x8000);
    int b_neg = (b & 0x8000);
    int res_neg = (result & 0x8000);
    if(op == OP_ADD)
    {
      if(!a_neg && !b_neg && res_neg)
      {
        cpu->flags |= FLAG_OVERFLOW;
      }
    }
    else if(op == OP_CMP || op == OP_SUB)
    {
      if(!a_neg && b_neg && res_neg)
      {
      cpu->flags |= FLAG_OVERFLOW;
      }
      else if(a_neg && !b_neg && !res_neg)
      {
        cpu->flags |= FLAG_OVERFLOW;
      }
    }
  }
  uint16_t alu_execute(CPU *cpu,Opcode op,uint16_t a,uint16_t b)
  {
    uint16_t result = 0;
    switch (op) {
      case OP_ADD : result = a + b; break;
      case OP_AND : result = a&b; break;
      case OP_OR : result = a|b; break;
      case OP_XOR : result = a^b; break;
      case OP_SHR : result = a >> (b & 0xF); break;
      case OP_SHL : result = a << (b & 0xF); break;
      case OP_NOT: result = ~a; break;
      case OP_SUB :
      case OP_CMP : result = a - b; break;
      default:
      fprintf(stderr,"Calculator error: unknown operation 0x%X\n",op);
      return 0;
    }
    update_flags(cpu,result,a,b,op);
    return result;
  }
