#include <stdio.h>
#include <inttypes.h>
#include "cpu.h"
#include "opcodes.h"
#include "instruction.h"

extern uint16_t alu_execute(CPU *cpu, Opcode op, uint16_t a, uint16_t b);
extern uint16_t mem_read16(CPU *cpu, uint16_t address);
extern void mem_write16(CPU *cpu, uint16_t address, uint16_t value);

void cpu_step(CPU *cpu) {
    if (cpu->halted) return;

    uint16_t instruction = mem_read16(cpu, cpu->pc);

    Opcode opcode = DECODE_OPCODE(instruction);
    uint8_t dst   = DECODE_DST(instruction);
    uint8_t src   = DECODE_SRC(instruction);
    uint8_t imm5  = DECODE_IMM5(instruction);
    uint16_t addr = DECODE_ADDR(instruction);

    cpu->pc += 2;

    switch (opcode) {
        case OP_NOP: break;

        case OP_LOAD:
            cpu->registers[dst] = mem_read16(cpu, cpu->pc);
            cpu->pc += 2;
            break;

        case OP_MOV:
            cpu->registers[dst] = cpu->registers[src];
            break;

        case OP_ADD:
            cpu->registers[dst] = alu_execute(cpu, OP_ADD,
                cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_SUB:
            cpu->registers[dst] = alu_execute(cpu, OP_SUB,
                cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_AND:
            cpu->registers[dst] = alu_execute(cpu, OP_AND,
                cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_OR:
            cpu->registers[dst] = alu_execute(cpu, OP_OR,
                cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_XOR:
            cpu->registers[dst] = alu_execute(cpu, OP_XOR,
                cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_NOT:
            cpu->registers[dst] = alu_execute(cpu, OP_NOT,
                cpu->registers[dst], 0);
            break;

        case OP_SHL:
            cpu->registers[dst] = alu_execute(cpu, OP_SHL,
                cpu->registers[dst], imm5);
            break;

        case OP_SHR:
            cpu->registers[dst] = alu_execute(cpu, OP_SHR,
                cpu->registers[dst], imm5);
            break;

        case OP_CMP:
            alu_execute(cpu, OP_CMP,
                cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_JMP:
            cpu->pc = addr;
            break;

        case OP_JZ:
            if (cpu->flags & FLAG_ZERO) cpu->pc = addr;
            break;

        case OP_JNZ:
            if (!(cpu->flags & FLAG_ZERO)) cpu->pc = addr;
            break;

        case OP_JN:
            if (cpu->flags & FLAG_NEGATIVE) cpu->pc = addr;
            break;

        case OP_LDR:
            cpu->registers[dst] = mem_read16(cpu, cpu->registers[src]);
            break;

        case OP_STR:
            mem_write16(cpu, cpu->registers[dst], cpu->registers[src]);
            break;

        case OP_PUSH:
            cpu->sp -= 2;
            mem_write16(cpu, cpu->sp, cpu->registers[dst]);
            break;

        case OP_POP:
            cpu->registers[dst] = mem_read16(cpu, cpu->sp);
            cpu->sp += 2;
            break;

        case OP_CALL: {
            uint16_t target = mem_read16(cpu, cpu->pc);
            cpu->pc += 2;
            cpu->sp -= 2;
            mem_write16(cpu, cpu->sp, cpu->pc);
            cpu->pc = target;
            break;
        }

        case OP_RET:
            cpu->pc = mem_read16(cpu, cpu->sp);
            cpu->sp += 2;
            break;

        case OP_HALT:
            cpu->halted = true;
            printf("CPU halted after %" PRIu64 " cycles.\n", cpu->cycles);
            break;

        default:
            fprintf(stderr, "UNKNOWN OPCODE 0x%04X at PC 0x%04X\n",
                    instruction, cpu->pc - 2);
            cpu->halted = true;
            break;
    }

    cpu->cycles++;
}

void cpu_run(CPU *cpu) {
    while (!cpu->halted) {
        cpu_step(cpu);
    }
}
