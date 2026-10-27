#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fds.h"
#include "leitura.h"
#include "cobertura.h"
#include "chaves.h"
#include "normalform.h"

int main(int argc, char *argv[]) {
    struct esquema esquema;
    char *nome_arquivo = NULL;
    conjunto x = 0;
    int encontrou_arquivo = 0;
    int encontrou_x = 0;

    // Verifica se o comando foi informado
    if (argc < 2) {
        printf("Uso: ./fdtool closure --fds arquivo.fds --X A\n");
        return 1;
    }

   // Verifica qual operacao foi solicitada
    int comando_fecho = strcmp(argv[1], "closure") == 0;
    int comando_cobertura = strcmp(argv[1], "mincover") == 0;
    int comando_chaves = strcmp(argv[1], "keys") == 0;
    int comando_formas = strcmp(argv[1], "normalform") == 0;


    if (!comando_fecho && !comando_cobertura && !comando_chaves && !comando_formas) {
        printf("Comando desconhecido: %s\n", argv[1]);
        return 1;
    }

    // Percorre os argumentos recebidos
    for (int i = 2; i < argc; i++) {

        // Recebe o nome do arquivo
        if (argv[i][0] == '-' && argv[i][1] == '-' &&
            argv[i][2] == 'f' && argv[i][3] == 'd' &&
            argv[i][4] == 's' && argv[i][5] == '\0') {

            if (i + 1 < argc) {
                nome_arquivo = argv[i + 1];
                encontrou_arquivo = 1;
                i++;
            } else {
                printf("Falta informar o arquivo .fds\n");
                return 1;
            }
        }

        // Recebe os atributos de X
        else if (argv[i][0] == '-' && argv[i][1] == '-' &&
                 argv[i][2] == 'X' && argv[i][3] == '\0') {

            if (i + 1 < argc) {
                char *texto = argv[i + 1];

                while (*texto != '\0') {
                    if (*texto < 'A' || *texto > 'Z') {
                        printf("Atributos invalidos: use letras de A ate Z\n");
                        return 1;
                    }

                    x = x | atributo(*texto);
                    texto++;
                }

                encontrou_x = 1;
                i++;
            } else {
                printf("Falta informar os atributos de X\n");
                return 1;
            }
        }
    }

    // Verifica se os argumentos obrigatorios foram informados
    if (!encontrou_arquivo) {
        printf("Uso: ./fdtool closure --fds arquivo.fds --X A\n");
        printf("Uso: ./fdtool mincover --fds arquivo.fds\n");
        printf("Uso: ./fdtool keys --fds arquivo.fds\n");
        printf("Uso: ./fdtool normalform --fds arquivo.fds\n");
        return 1;
    }

    if (comando_fecho && (!encontrou_x || x == 0)) {
        printf("Uso: ./fdtool closure --fds arquivo.fds --X A\n");
        return 1;
    }

    // Le o esquema do arquivo
    if (!ler_esquema(nome_arquivo, &esquema)) {
        printf("Erro ao ler o arquivo.\n");
        return 1;
    }

    // Executa a cobertura minima
    if (comando_cobertura) {
        if (!cobertura_minima(&esquema)) {
            printf("Erro ao calcular a cobertura minima.\n");
            liberar_esquema(&esquema);
            return 1;
        }

        for (int i = 0; i < esquema.quantidade; i++) {
            conjunto esquerda = esquema.dfs[i].esquerda;
            conjunto direita = esquema.dfs[i].direita;

            // Mostra os atributos do lado esquerdo
            for (char letra = 'A'; letra <= 'Z'; letra++) {
                if (subconjunto(atributo(letra), esquerda)) {
                    printf("%c", letra);
                }
            }

            printf("->");

            // Mostra os atributos do lado direito
            for (char letra = 'A'; letra <= 'Z'; letra++) {
                if (subconjunto(atributo(letra), direita)) {
                    printf("%c", letra);
                }
            }

            printf("\n");
        }

        liberar_esquema(&esquema);
        return 0;
    }

    if (comando_chaves) {
        conjunto *chaves = NULL;
        int qtde = 0;

        if (!encontrar_chaves(&esquema, &chaves, &qtde)) {
            printf("Erro ao encontrar as chaves candidatas.\n");
            liberar_esquema(&esquema);
            return 1;
        }

        for (int i = 0; i < qtde; i++) {
            int primeiro = 1;

            for (char letra = 'A'; letra <= 'Z'; letra++) {
                conjunto bit = atributo(letra);

                if(subconjunto(bit, chaves[i])) {
                    if (!primeiro)
                        printf(", ");
                    printf("%c", letra);
                    primeiro = 0;
                }
            }

            printf("\n");
        }

        if (qtde == 0) 
            printf("Nenhuma chave candidata encontrada.\n");

        free(chaves);
        liberar_esquema(&esquema);
        return 0;
    }

    if  (comando_formas) {
        if (!verificar_formas_normais(&esquema)) {
            liberar_esquema(&esquema);
            return 1;
        }

        liberar_esquema(&esquema);
        return 0;
    }

    // Verifica se X utiliza apenas atributos do universo U
    if (!subconjunto(x, esquema.atributos)) {
        printf("Erro: X contem atributos que nao pertencem a U.\n");
        liberar_esquema(&esquema);
        return 1;
    }

    // Calcula o fecho de X
    conjunto resultado = fecho(x, &esquema);

    // Mostra os atributos do fecho
    for (char letra = 'A'; letra <= 'Z'; letra++) {
        if (subconjunto(atributo(letra), resultado)) {
            printf("%c", letra);
        }
    }

    printf("\n");


    // Libera a memoria utilizada
    liberar_esquema(&esquema);

    return 0;
    
}