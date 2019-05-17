#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */

void cpu_ram_write(struct cpu *cpu, unsigned char adrs, unsigned char value){
    cpu->ram[adrs] = value;
}


unsigned char cpu_ram_read(struct cpu *cpu, unsigned char adrs) {
    return cpu->ram[adrs];
}


void call(struct cpu *cpu, unsigned char reg_addrs_to_jump)
{
  
    cpu->reg[7]--;
    cpu_ram_write(cpu, cpu->reg[7], cpu->pc + 2);
    cpu->pc = cpu->reg[reg_addrs_to_jump];
}


void ret(struct cpu *cpu)
{
    unsigned char adrs_to_return = cpu_ram_read(cpu, cpu->reg[7]);
    cpu->reg[7]++;
    cpu->pc = adrs_to_return;
}



void push(struct cpu *cpu, unsigned char reg_to_push)
{
    cpu->reg[7]--;
    unsigned char val_to_push = cpu->reg[reg_to_push];
    cpu_ram_write(cpu, cpu->reg[7], val_to_push);
    cpu->pc += 2;
}


void pop(struct cpu *cpu, unsigned char reg_to_pop)
{
    unsigned char sp_address = cpu->reg[7];
    unsigned char val_to_pop = cpu_ram_read(cpu, sp_address);
    
    cpu->reg[reg_to_pop] = val_to_pop;
    cpu->reg[7]++;
    cpu->pc += 2;
}

void cpu_load(struct cpu *cpu, char *load_program)
{
   int address = 0;
    
    
    FILE *fp;
    char line[1024];
    fp = fopen(load_program, "r");
    
    
    
    if (fp == NULL) {
        fprintf(stderr,"comp: error opening file\n");
        exit(2);
    }
    
    
    
    while (fgets(line, 1024, fp) != NULL) {
        char *endptr;
        
        unsigned int val = strtoul(line, &endptr, 2) & 0xFF ;
        
        if (endptr == line) {
            continue;
        }
        cpu_ram_write(cpu, address ++, val) ;
    }
    
    fclose(fp);  
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
    switch (op) {
        case ALU_MUL:
            cpu->reg[regA] = (cpu->reg[regA] * cpu ->reg[regB]) & 0xFF ;
         
        case ALU_ADD:
            cpu->reg[regA] = (cpu->reg[regA] + cpu ->reg[regB]) & 0xFF ;
            break;
          
        case ALU_CMP:
            if (cpu->reg[regA] > cpu->reg[regB]) {
                cpu->flag = CMP_G;
            } else if (cpu->reg[regA] < cpu->reg[regB]) {
               cpu->flag = CMP_L;
            } else {
                cpu->flag = CMP_E;
            }
            break;
    }
}


/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
    int running = 1;
    while (running) {
        unsigned char val = cpu_ram_read(cpu, cpu->pc) ;
        
        unsigned char operandA = cpu_ram_read(cpu, cpu->pc + 1);
        unsigned char operandB = cpu_ram_read(cpu, cpu->pc + 2);
        
        switch (val) {
            case LDI:
                cpu->reg[operandA] = operandB;
                cpu->pc += 3;
                break;
            case PRN:
                printf("%d\n", cpu->reg[operandA]);
                cpu->pc += 2;
                break;
            case MUL:
                alu(cpu, ALU_MUL, operandA, operandB);
                cpu->pc += 3;
                break;
            case ADD:
                alu(cpu, ALU_ADD, operandA, operandB);
                cpu->pc += 3;
                break;
            case CMP:
                alu(cpu, ALU_CMP, operandA, operandB);
                cpu->pc += 3;
                break;
            case PUSH:
                push(cpu, operandA) ;
                break ;
            case POP:
                pop(cpu, operandA) ;
                break ;
            case CALL:
                call(cpu, operandA) ;
                break ;
            case RET:
                ret(cpu);
                break ;
            case JMP:
                cpu->pc = cpu->reg[operandA];
                break;
            case JEQ:
                if ((cpu->flag & CMP_E) == CMP_E) {
                    
                    
                    cpu->pc = cpu->reg[operandA];
                    
                    
                } else {
                    cpu->pc += 2;
                }
                break ;
            case JNE:
                if ((cpu->flag & CMP_E) != CMP_E) {
                    
                    
                    cpu->pc = cpu->reg[operandA];
                    
                    
                } else {
                    cpu->pc += 2;
                }
                break ;
            case HLT:
                running = 0;
                break;
            default:
                printf("Unknown instruction at %d: %d\n", cpu->pc, val);
                exit(1);
        }
    }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
    cpu->pc = 0;
    cpu->reg[7] = 0xF4;
    memset(cpu->ram, 0, sizeof(cpu->ram));
}



