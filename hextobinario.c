#include <stdio.h>
#include <string.h>

void ConverterHexaPraBinario(char valor[], char resultado[]) {
    //tabela de conversão de hexadecimal para binario
    char *tabela[16] = {
        "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
    };
    resultado[0] = '\0';
        
    for (int i = 0; valor[i] != '\0'; i++) {
        char c = valor[i];

        if (c >= '0' && c <= '9') {
            strcat(resultado, tabela[c - '0']);  //converte de 0 a 9
        } else if (c >= 'A' && c <= 'F') {
            strcat(resultado, tabela[c - 'A' + 10]);  //converte c para sua letra na tabela ascii e soma com 10
        } else if (c >= 'a' && c <= 'f') {
            strcat(resultado, tabela[c - 'a' + 10]);  //faz a mesma coisa da condição acima pra letras minusculas
        } else {
            printf(" [ERRO] ");
            return;
        }
    }
}

int main() {
    char hexa[4], binario[16];
    scanf("%s", hexa);

    printf("Hexadecimal: %s\n", hexa);
    ConverterHexaPraBinario(hexa, binario);
    printf("Binario: %s\n", binario);

    return 0;
}