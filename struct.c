#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMORY_SIZE 0x10000  // Ex.: 65.536 posições para 16 bits de endereçamento
#define STACK_START 0x8200   // Endereço inicial da pilha
#define STACK_SIZE  16       // Tamanho da pilha (em bytes ou em palavras, conforme sua definição)

// Estrutura para as flags da CPU
typedef struct {
    uint8_t C;   // Carry
    uint8_t Ov;  // Overflow
    uint8_t Z;   // Zero
    uint8_t S;   // Signal
} Flags;

// Estrutura para representar o estado da CPU
typedef struct {
    uint16_t regs[8];   // Registradores R0 - R7
    uint16_t PC;        // Program Counter
    uint16_t IR;        // Instruction Register
    uint16_t SP;        // Stack Pointer
    Flags flags;        // Flags
    uint16_t memory[MEMORY_SIZE];  // Memória (instruções e dados)
} CPU;


// Função para inicializar a CPU
void initCPU(CPU *cpu) {
    for (int i = 0; i < 8; i++) {
        cpu->regs[i] = 0;
    }
    cpu->PC = 0;
    cpu->IR = 0;
    cpu->SP = STACK_START;
    cpu->flags.C = 0;
    cpu->flags.Ov = 0;
    cpu->flags.Z = 0;
    cpu->flags.S = 0;
    memset(cpu->memory, 0, sizeof(cpu->memory));
}