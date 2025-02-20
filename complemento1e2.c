#include <stdio.h>
#include <string.h>
#define MAX 16

int contadorDeOverflow(char binario1[MAX + 1], char binario2[MAX + 1], char resultado[MAX + 1]) {
    //Se os dois binários têm o mesmo sinal (binario1[0] == binario2[0]),
    //e o sinal do resultado é diferente (resultado[0] != binario1[0]), houve overflow
    if (binario1[0] == binario2[0] && binario1[0] != resultado[0]) {
        return 1; // Overflow
    }
    return 0;
    //e retorna um icremento pro contador de overflow
}

void complemento1(char valor[MAX + 1], char resultado[MAX + 1]) {
    for (int i = 0; i < MAX; i++) {
         if (valor[i] == '1') {
             resultado[i] = '0';
         } else {
             resultado[i] = '1';
         }
     }
     resultado[MAX] = '\0';
}

void complemento2(char complementoUm[MAX + 1], char resultado[MAX + 1], int *carry_count, int *contaOverflow) {
    strcpy(resultado, complementoUm);
    char contOver[MAX + 1] = "0000000000000001";
    int carry = 1;
    int i = MAX - 1;
    *carry_count = 0;

    while (i >= 0 && carry > 0) {
        if (resultado[i] == '1') {
            resultado[i] = '0';
            if(i == 0){
                (*carry_count)++;
            }
        } else {
            resultado[i] = '1';
            carry = 0;
        }
        i--;
    }
    *contaOverflow = contadorDeOverflow(complementoUm, contOver, resultado);
}

int main() {
    char binario[MAX + 1], complementoUm[MAX + 1], complementoDois[MAX + 1];
    int carry_count = 0;
    int contaOverflow = 0;

    scanf("%s", binario);

    complemento1(binario, complementoUm);
    complemento2(complementoUm, complementoDois, &carry_count, &contaOverflow);

    printf("Binario: %s\n", binario);
    printf("Complemento de 1: %s\n", complementoUm);
    printf("Complemento de 2: %s\n", complementoDois);
    printf("Numero de carrys: %d\n", carry_count);
    printf("Overflow: %d\n", contaOverflow);

    return 0;
}