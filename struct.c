#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMORY_SIZE 0x10000 
#define STACK_START 0x8200   // Endereço inicial da pilha
#define STACK_SIZE  16       // Tamanho da pilha

// Estrutura para as flags da CPU
typedef struct {
    uint8_t C;   // carry
    uint8_t Ov;  // overflow
    uint8_t Z;   // zero
    uint8_t S;   // signal
} Flags;

// Estrutura para representar o estado da CPU
typedef struct {
    uint16_t regs[8];   // registradores R0 - R7
    uint16_t PC;
    uint16_t IR;
    uint16_t SP;
    Flags flags;
    uint16_t memory[MEMORY_SIZE];  // Memória
} CPU;


// Função para inicializar a CPU
void initCPU(CPU *cpu) {
    for (int i = 0; i < 8; i++) {
        cpu->regs[i] = 0; // zera os registradores
    }
    // inicia tudo zerado
    cpu->PC = 0;
    cpu->IR = 0;
    cpu->SP = STACK_START;
    cpu->flags.C = 0;
    cpu->flags.Ov = 0;
    cpu->flags.Z = 0;
    cpu->flags.S = 0;
    memset(cpu->memory, 0, sizeof(cpu->memory)); // limpa a memoria
}