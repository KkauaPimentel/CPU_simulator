#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE   65536
#define STACK_SIZE 16
#define MAX_TRACE  1024
#define STACK_BASE 0x8200

typedef struct {
  int C;
  int Ov;
  int Z;
  int S;
} Flags;

uint16_t memory[MEM_SIZE];
int accessed[MEM_SIZE];
uint16_t reg[8];
uint16_t IR;
uint16_t PC;
uint16_t SP;
Flags flags;
uint16_t stack[STACK_SIZE];
int stack_index = 0;
char trace[MAX_TRACE][128];
int trace_count = 0;

void update_SP() { 
  SP = STACK_BASE - (stack_index * 2);
}

void print_stack() {
  printf("Pilha (endereco : conteudo):\n");
  for (int i = 0; i < STACK_SIZE; i++) {
    uint16_t addr = STACK_BASE - (i * 2);
    printf("%04X : 0x%04X\n", addr, stack[i]);
  }
  printf("\n");
}

void print_registers() {
  printf("Registradores:\n");
  for (int i = 0; i < 8; i++) {
    printf("  R%d: 0x%04X\n", i, reg[i]);
  }
  printf("  PC: 0x%04X\n", PC);
  printf("  SP: 0x%04X\n", SP);
  printf("\n");
}

void print_flags() {
  printf("Flags Finais:\n");
  printf("  C  : %d\n", flags.C);
  printf("  Ov : %d\n", flags.Ov);
  printf("  Z  : %d\n", flags.Z);
  printf("  S  : %d\n", flags.S);
  printf("\n");
}

void print_memory() {
  printf("Memoria de dados:\n");
  for (int i = 0; i < MEM_SIZE; i++) {
    if (accessed[i] == 1) {
      if (i >= (STACK_BASE - (STACK_SIZE * 2)) && i < STACK_BASE)
        continue;
      printf("%04X: 0x%04X\n", i, memory[i]);
    }
  }
  printf("\n");
}

void print_trace() {
  printf("===== INSTRUCOES =====\n");
  if (trace_count > 0)
    printf("Inicio:\n");
  for (int i = 0; i < trace_count; i++) {
    printf("  %s\n", trace[i]);
  }
  printf("\n");
}

void printAll(){
  print_trace();
  print_stack();
  print_registers();
  print_flags();
  print_memory();
}

int sign_extend_8(uint8_t imm) { 
  return (imm & 0x80) ? imm - 0x100 : imm;
}

int sign_extend_9(uint16_t imm) { 
  return (imm & 0x100) ? imm - 0x200 : imm;
}

void decode_instruction(uint16_t instr, char *buf) {
  if (instr == 0x0000) {
    sprintf(buf, "NOP");
    return;
  }
  if (instr == 0xFFFF) {
    sprintf(buf, "HALT");
    return;
  }
  
  uint8_t opcode = (instr >> 11) & 0x1F;
  uint8_t sufixo = instr & 0x3;
  
  switch (opcode) {
    case 0x00:
      switch (sufixo) {
        case 0x1: {
          uint8_t rn = (instr >> 2) & 0x7;  
          sprintf(buf, "PSH R%d", rn);
          break;
        }
        case 0x2: {
          uint8_t rd = (instr >> 8) & 0x7; 
          sprintf(buf, "POP R%d", rd);
          break;
        }
        case 0x3: {
          uint8_t rm = (instr >> 5) & 0x7;
          uint8_t rn = (instr >> 2) & 0x7;
          sprintf(buf, "CMP R%d, R%d", rm, rn);
          break;
        }
        default:
          sprintf(buf, "Instrucao indefinida");
          break;
      }
      break;
    case 0x01: {
      uint16_t imediato = (instr >> 2) & 0x1FF;
      int offset = sign_extend_9(imediato) * 2;
      switch (sufixo) {
        case 0x0:
          sprintf(buf, "JMP %d", offset);
          break;
        case 0x1:
          sprintf(buf, "JEQ %d", offset);
          break;
        case 0x2:
          sprintf(buf, "JLT %d", offset);
          break;
        case 0x3:
          sprintf(buf, "JGT %d", offset);
          break;
        default:
          sprintf(buf, "J??? %d", offset);
          break;
      }
      break;
    }
    case 0x02: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      sprintf(buf, "MOV R%d, R%d", rd, rm);
      break;
    }
    case 0x03: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t imediato = instr & 0xFF;
      sprintf(buf, "MOV R%d, #%d", rd, imediato);
      break;
    }
    case 0x04: {
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "STR [R%d], R%d", rm, rn);
      break;
    }
    case 0x05: {
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t imediato = (((instr >> 8) & 0x7) << 5) | (instr & 0x1F);
      sprintf(buf, "STR [R%d], #%d", rm, imediato);
      break;
    }
    case 0x06: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      sprintf(buf, "LDR R%d, [R%d]", rd, rm);
      break;
    }
    case 0x08: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "ADD R%d, R%d, R%d", rd, rm, rn);
      break;
    }
    case 0x0A: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "SUB R%d, R%d, R%d", rd, rm, rn);
      break;
    }
    case 0x0C: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "MUL R%d, R%d, R%d", rd, rm, rn);
      break;
    }
    case 0x0E: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "AND R%d, R%d, R%d", rd, rm, rn);
      break;
    }
    case 0x10: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "ORR R%d, R%d, R%d", rd, rm, rn);
      break;
    }
    case 0x12: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      sprintf(buf, "NOT R%d, R%d", rd, rm);
      break;
    }
    case 0x14: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t rn = (instr >> 2) & 0x7;
      sprintf(buf, "XOR R%d, R%d, R%d", rd, rm, rn);
      break;
    }
    case 0x16: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t imediato = instr & 0x1F;
      sprintf(buf, "SHR R%d, R%d, #%d", rd, rm, imediato);
      break;
    }
    case 0x18: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      uint8_t imediato = instr & 0x1F;
      sprintf(buf, "SHL R%d, R%d, #%d", rd, rm, imediato);
      break;
    }
    case 0x1A: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      sprintf(buf, "ROR R%d, R%d", rd, rm);
      break;
    }
    case 0x1C: {
      uint8_t rd = (instr >> 8) & 0x7;
      uint8_t rm = (instr >> 5) & 0x7;
      sprintf(buf, "ROL R%d, R%d", rd, rm);
      break;
    }
    case 0x1F: {
      if (instr == 0xFFFF)
        sprintf(buf, "HALT");
      else
        sprintf(buf, "Instrucao indefinida");
      break;
    }
    default:
      sprintf(buf, "Instrucao indefinida");
      break;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Uso: %s <arquivo de entrada>\n", argv[0]);
    return 1;
  }
  FILE *fp = fopen(argv[1], "r");
  if (!fp) {
    perror("Erro ao abrir arquivo");
    return 1;
  }
  for (int i = 0; i < MEM_SIZE; i++) {
    memory[i] = 0;
    accessed[i] = 0;
  }
  char line[256];
  unsigned int addr, value;
  unsigned int min_addr = MEM_SIZE, max_addr = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (strchr(line, ':') != NULL) {
      int ret = sscanf(line, "%x: %x", &addr, &value);
      if (ret != 2) {  
        fprintf(stderr, "Instrucao indefinida: %s", line);
        memory[addr] = 0x8FAA;
        continue; 
      }
      if (addr < MEM_SIZE) {
        memory[addr] = (uint16_t)value;
        if (addr < min_addr)
          min_addr = addr;
        if (addr > max_addr)
          max_addr = addr;
      }
    }
  }
  
  fclose(fp);

  for (int i = 0; i < 8; i++) {
    reg[i] = 0;
  }
  PC = min_addr;
  SP = STACK_BASE; 
  flags.C = flags.Ov = flags.Z = flags.S = 0;
  for (int i = 0; i < STACK_SIZE; i++) {
    stack[i] = 0;
  }
  stack_index = 0;

  int halt = 0;
  while (PC <= max_addr && !halt) {
    uint16_t addr_atual = PC;
    uint16_t instr = memory[PC];
    IR = instr;
    PC += 2;
    char decoded[128];
    decode_instruction(instr, decoded);
    if (trace_count < MAX_TRACE) {
      sprintf(trace[trace_count], "%04X: %s", addr_atual, decoded);
      trace_count++;
    }
    
    uint8_t opcode = (instr >> 11) & 0x1F;
    uint8_t sufixo = instr & 0x3;
    switch (opcode) {
      case 0x00:
        if (instr == 0x0000) {
          printAll();
        } else {
          switch (sufixo) {
            case 0x1: {
              uint8_t rn = (instr >> 2) & 0x7;  
              if (stack_index >= STACK_SIZE) {
                printf("Stack overflow\n");
                halt = 1;
                break;
              }
              stack[stack_index] = reg[rn];
              stack_index++;
              update_SP();
              break;
            }
            case 0x2: {
              if (stack_index <= 0) {
                printf("Stack underflow\n");
                halt = 1;
                break;
              }
              stack_index--;
              update_SP();
              uint8_t rd = (instr >> 8) & 0x7;
              reg[rd] = stack[stack_index];
              break;
            }
            case 0x3: {
              uint8_t rm = (instr >> 5) & 0x7;
              uint8_t rn = (instr >> 2) & 0x7;
              int16_t diff = (int16_t)reg[rm] - (int16_t)reg[rn];
              flags.Z = (diff == 0) ? 1 : 0;
              flags.C = (reg[rm] < reg[rn]) ? 1 : 0;
              flags.S = (diff < 0) ? 1 : 0;
              flags.Ov = 0;
              break;
            }
            default:
              halt = 1;
              break;
          }
        }
        break;
        case 0x01: {
          int offset = sign_extend_9((instr >> 2) & 0x1FF);
          
          switch (sufixo) {
            case 0x0: // JMP
              PC += offset;
              break;
            case 0x1: // JEQ
              if (flags.Z == 1 && flags.S == 0)
                PC += offset;
              break;
            case 0x2: // JLT
              if (flags.Z == 0 && flags.S == 1)
                PC += offset;
              break;
            case 0x3: // JGT
              if (flags.Z == 0 && flags.S == 0)
                PC += offset;
              break;
            default:
              halt = 1;
              break;
          }
          break;
        }
        
        
      case 0x02: {
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        reg[rd] = reg[rm];
        break;
      }
      case 0x03: {
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t imediato = instr & 0xFF;
        reg[rd] = imediato;
        break;
      }
      case 0x04: {
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint16_t addr = reg[rm];
        if (addr < MEM_SIZE) {
          memory[addr] = reg[rn];
          accessed[addr] = 1;
        }
        break;
      }
      case 0x05: {
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t imediato = (((instr >> 8) & 0x7) << 5) | (instr & 0x1F);
        uint16_t addr = reg[rm];
        if (addr < MEM_SIZE) {
          memory[addr] = imediato;
          accessed[addr] = 1;
        }
        break;
      }
      case 0x06: {
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint16_t addr = reg[rm];
        if (addr < MEM_SIZE) {
          reg[rd] = memory[addr];
          accessed[addr] = 1;
        } else {
          reg[rd] = 0;
        }
        break;
      }
      case 0x08: {
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint32_t full = (uint32_t)reg[rm] + (uint32_t)reg[rn];
        flags.C = (full > 0xFFFF) ? 1 : 0;
        uint16_t result = (uint16_t)full;
        if (flags.C)
          result = (~result + 1) & 0xFFFF;
        reg[rd] = result;
        {
          int16_t s_rm = (int16_t)reg[rm];
          int16_t s_rn = (int16_t)reg[rn];
          int16_t s_res = (int16_t)reg[rd];
          flags.Ov = (((s_rm > 0) && (s_rn > 0) && (s_res < 0)) ||
                      ((s_rm < 0) && (s_rn < 0) && (s_res >= 0))) ? 1 : 0;
        }
        flags.Z = (reg[rd] == 0) ? 1 : 0;
        flags.S = ((reg[rd] & 0x8000) != 0) ? 1 : 0;
        break;
      }
      case 0x0A: { //SUB
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint16_t result = reg[rm] - reg[rn];
        reg[rd] = result;
        flags.Z = (result == 0) ? 1 : 0;
        flags.S = ((result & 0x8000) != 0) ? 1 : 0;
        flags.C = (reg[rm] < reg[rn]) ? 1 : 0;
        {
          int16_t s_rm = (int16_t)reg[rm];
          int16_t s_rn = (int16_t)reg[rn];
          int16_t s_res = (int16_t)result;
          flags.Ov = (((s_rm < 0) && (s_rn > 0) && (s_res > 0)) ||
                      ((s_rm >= 0) && (s_rn < 0) && (s_res < 0))) ? 1 : 0;
        }
        break;
      }
      case 0x0C: { //MUL
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint32_t full = (uint32_t)reg[rm] * (uint32_t)reg[rn];
        flags.C = (full > 0xFFFF) ? 1 : 0;
        uint16_t result = (uint16_t)full;
        if (flags.C)
          result = (~result + 1) & 0xFFFF;
        reg[rd] = result;
        flags.Ov = flags.C;
        flags.Z = (reg[rd] == 0) ? 1 : 0;
        flags.S = ((reg[rd] & 0x8000) != 0) ? 1 : 0;
        break;
      }
      case 0x0E: { //AND
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint16_t result = reg[rm] & reg[rn];
        reg[rd] = result;
        flags.Z = (result == 0) ? 1 : 0;
        flags.S = ((result & 0x8000) != 0) ? 1 : 0;
        flags.C = 0;
        flags.Ov = 0;
        break;
      }
      case 0x10: { //ORR
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint16_t result = reg[rm] | reg[rn];
        reg[rd] = result;
        flags.Z = (result == 0) ? 1 : 0;
        flags.S = ((result & 0x8000) != 0) ? 1 : 0;
        flags.C = 0;
        flags.Ov = 0;
        break;
      }
      case 0x12: { //NOT
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint16_t result = ~reg[rm];
        reg[rd] = result;
        flags.Z = (result == 0) ? 1 : 0;
        flags.S = ((result & 0x8000) != 0) ? 1 : 0;
        flags.C = 0;
        flags.Ov = 0;
        break;
      }
      case 0x14: { //XOR
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t rn = (instr >> 2) & 0x7;
        uint16_t result = reg[rm] ^ reg[rn];
        reg[rd] = result;
        flags.Z = (result == 0) ? 1 : 0;
        flags.S = ((result & 0x8000) != 0) ? 1 : 0;
        flags.C = 0;
        flags.Ov = 0;
        break;
      }
      case 0x16: { //SHR
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t imediato = instr & 0x1F;
        uint16_t val = reg[rm];
        uint16_t result = val >> imediato;
        reg[rd] = result;
        flags.Z = (result == 0);
        flags.S = ((result & 0x8000) != 0);
        if (imediato > 0) {
          flags.C = ((val >> (imediato - 1)) & 1);
        } else {
          flags.C = 0;
        }
        flags.Ov = 0;
        break;
      }
      case 0x18: { //SHL
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint8_t imediato = instr & 0x1F;
        uint16_t val = reg[rm];
        uint16_t result = val << imediato;
        reg[rd] = result;
        flags.Z = (result == 0);
        flags.S = ((result & 0x8000) != 0);
        if (imediato > 0) {
          flags.C = ((val >> (16 - imediato)) & 1);
        } else {
          flags.C = 0;
        }
        flags.Ov = 0;
        break;
      }
      case 0x1A: { //ROR
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint16_t val = reg[rm];
        uint8_t last_bit = val & 1;
        uint16_t result = (val >> 1) | (last_bit << 15);
        reg[rd] = result;
        flags.Z = (result == 0);
        flags.S = ((result & 0x8000) != 0);
        flags.C = last_bit;
        flags.Ov = 0;
        break;
      }
      case 0x1C: { //ROL
        uint8_t rd = (instr >> 8) & 0x7;
        uint8_t rm = (instr >> 5) & 0x7;
        uint16_t val = reg[rm];
        uint8_t msb = (val >> 15) & 1;
        uint16_t result = (val << 1) | msb;
        reg[rd] = result;
        flags.Z = (result == 0);
        flags.S = ((result & 0x8000) != 0);
        flags.C = msb;
        flags.Ov = 0;
        break;
      }
      case 0x1F: {
        halt = 1;
        break;
      }
      default: {
        halt = 1;
        break;
      }
    }
  }
  printAll();
  return 0;
}