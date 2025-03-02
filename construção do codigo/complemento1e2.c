#include <stdio.h>
#include <string.h>
// variavel global para os 16 bits da CPU
#define MAX 16

int contadorDeOverflow(char binario1[MAX + 1], char binario2[MAX + 1], char resultado[MAX + 1]) {
    //Se os dois binários tiverem o mesmo sinal,
    //e o sinal do resultado é diferente, quer dizer que teve overflow
    if (binario1[0] == binario2[0] && binario1[0] != resultado[0]) {
        return 1; // Overflow
    }
    return 0;
    //e retorna um icremento pro contador de overflow
}

// Função p calcular o complemente de 1 de um binario
void complemento1(char valor[MAX + 1], char resultado[MAX + 1]) {
    // Sequencia que vai invertendo os bits
    for (int i = 0; i < MAX; i++) {
         if (valor[i] == '1') {
             resultado[i] = '0';
         } else {
             resultado[i] = '1';
         }
     }
     resultado[MAX] = '\0';
}

void complemento2(char complementoUm[MAX + 1], char resultado[MAX + 1], int *cont_carry, int *contaOverflow) {
    
    strcpy(resultado, complementoUm); //copia o c1 para o resultado, onde vamos manipular
    char contOver[MAX + 1] = "0000000000000001";
    int carry = 1;
    int i = MAX - 1;
    *cont_carry = 0;

    // onde acontece a soma "automatica" do bit mais a direita até o da esquerda
    while (i >= 0 && carry > 0) {
        if (resultado[i] == '1') {
            resultado[i] = '0';
            if(i == 0){
                (*cont_carry)++;
            }
        } else {
            resultado[i] = '1';
            carry = 0;
        }
        i--;
    }
    // indica se aconteceu overflow
    *contaOverflow = contadorDeOverflow(complementoUm, contOver, resultado);
}

int main() {
    char binario[MAX + 1], complementoUm[MAX + 1], complementoDois[MAX + 1];
    int cont_carry = 0;
    int contaOverflow = 0;

    scanf("%s", binario); // entrada do binario

    complemento1(binario, complementoUm);// transforma o binario em c1 e armazena em complementoUm
    complemento2(complementoUm, complementoDois, &cont_carry, &contaOverflow); // armazena em complementoDois

    // imprime a situação geral
    printf("Binario: %s\n", binario);
    printf("Complemento de 1: %s\n", complementoUm);
    printf("Complemento de 2: %s\n", complementoDois);
    printf("Numero de carrys: %d\n", cont_carry);
    printf("Overflow: %d\n", contaOverflow);

    return 0;
}