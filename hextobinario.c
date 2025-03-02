#include <stdio.h>
#include <string.h>

// função que converte a string hexa para binário
// Ela recebe dois parâmetros: 
// 1- 'valor': a string que tem o número hexadecimal
// 2- 'resultado': a string onde colocaremos o número em binário
void ConverterHexaPraBinario(char valor[], char resultado[]) {
    // fizeomos uma tabela que mapeia cada dígito hexadecimal do 0 ao F(15)
    // para sua representação em 4 bits (em formato de string).
    char *tabela[16] = {
        "0000", "0001", "0010", "0011", 
        "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011", 
        "1100", "1101", "1110", "1111"
    };
    
    // inicia a string de resultado como vazia
    resultado[0] = '\0';
    
    // percorremos cada caractere da string hexa 'valor'
    // até encontrarmos o final da string, que é o nulo
    for (int i = 0; valor[i] != '\0'; i++) {
        // Pegamos o caractere atual.
        char c = valor[i];

        // Se o caractere for um dígito entre '0' e '9'
        // usamos o índice para acessar a parte binária de acordo com a tabela.
        if (c >= '0' && c <= '9') {
            strcat(resultado, tabela[c - '0']);  // add a parte binária no final da string 'resultado'
        }
        // se o caractere for uma letra maiúscula de 'A' a 'F'
        // calculamos o índice (c - 'A' + 10) ja que 'A' representa 10 no sistema hexadecimal
        else if (c >= 'A' && c <= 'F') {
            strcat(resultado, tabela[c - 'A' + 10]);  // concatena a representação binária
        }
        // Se o caractere for uma letra minúscula de 'a' a 'f'
        // fazemos o mesmo(evitar erros de leitura)
        else if (c >= 'a' && c <= 'f') {
            strcat(resultado, tabela[c - 'a' + 10]);
        }
        // Se o caractere não se encaixar em nenhuma das condições,
        // significa que o caractere é inválido
        else {
            printf(" [ERRO] ");
            return;  // Encerra a função sem completar a conversão.
        }
    }
}

int main() {
    // 'hexa' para armazenar a entrada hexadecimal
    // 'binario' para guardar a string resultante da conversão para binário
    char hexa[4], binario[16];

    // leitura do hexa
    scanf("%s", hexa);

    // exibimos o valor hexadecimal lido
    printf("Hexadecimal: %s\n", hexa);

    // Chamamos a função que realiza a conversão para binário
    ConverterHexaPraBinario(hexa, binario);

    // Exibimos o resultado da conversão
    printf("Binario: %s\n", binario);
    
    return 0;
}
