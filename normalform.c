#include <stdio.h>
#include <stdlib.h>
#include "normalform.h"
#include "chaves.h"

/* Verifica se um conjunto é uma superchave */
static int eh_superchave(conjunto atributos, const struct esquema *esquema) {
    conjunto resultado = fecho(atributos, esquema);

    // Se o fecho contém todos os atributos do esquema, o conjunto é uma superchave
    return subconjunto(esquema->atributos, resultado);
}

/* Verifica se um atributo é primo */
static int eh_primo(char letra, const conjunto *chaves, int qtde) {
    conjunto bit = atributo(letra);

    // Verifica se o atributo pertence a chave candidata atual
    for (int i = 0; i < qtde; i++) {
        if (subconjunto(bit, chaves[i])) {
            return 1;
        }
    }

    return 0;
}

/* Mostra as letras dos atributos que pertencem a um conjunto */
static void mostrar_atributos(conjunto atributos) {
    for (char letra = 'A'; letra <= 'Z'; letra++) {
        conjunto bit = atributo(letra);

        if (subconjunto(bit, atributos)) {
            printf("%c", letra);
        }
    }
}

/* Verifica se o esquema satisfaz a BCNF e a 3FN e mostra as violações */
int verificar_formas_normais(const struct esquema *esquema) {
    conjunto *chaves = NULL;  // ponteiro que armazena as chaves candidatas
    int qtde = 0;

    if (!encontrar_chaves(esquema, &chaves, &qtde)) {
        printf("Erro ao encontrar as chaves candidatas.\n");
        return 0;
    }

    int viola_bcnf = 0;
    int viola_3fn = 0;

    // Percorre as dfs para identificar se existe alguma violação
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto esquerda = esquema->dfs[i].esquerda;
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            conjunto bit = atributo(letra);

            if (!subconjunto(bit, direita)) {
                continue;
            }

            if (subconjunto(bit, esquerda)) {
                continue;
            }

            // Verifica se o lado esquerdo é uma superchave
            int superchave = eh_superchave(esquerda, esquema);

            // Verifica se o atributo da direita é primo
            int primo = eh_primo(letra, chaves, qtde);

            // Se lhs não é determinante, viola bcnf
            if (!superchave) {
                viola_bcnf = 1;
            }

            // Se lhs não é determinante e atributo da direita não é primo, viola 3fn
            if (!superchave && !primo) {
                viola_3fn = 1;
            }
        }
    }

    /* Mostra o resultado geral */
    if (viola_bcnf) {
        printf("BCNF: VIOLATIONS\n");
    } else {
        printf("BCNF: OK\n");
    }

    if (viola_3fn) {
        printf("3NF: VIOLATIONS\n");
    } else {
        printf("3NF: OK\n");
    }

    /* Mostra as dfs que violam as formas normais */
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto esquerda = esquema->dfs[i].esquerda;
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            conjunto bit = atributo(letra);

            if (!subconjunto(bit, direita)) {
                continue;
            }

            if (subconjunto(bit, esquerda)) {
                continue;
            }

            int superchave = eh_superchave(esquerda, esquema);
            int primo = eh_primo(letra, chaves, qtde);

            if (!superchave) {
                printf("VIOLATION BCNF: ");
                mostrar_atributos(esquerda);
                printf("->%c (", letra);
                mostrar_atributos(esquerda);
                printf(" not superkey)\n");
            }

            if (!superchave && !primo) {
                printf("VIOLATION 3NF: ");
                mostrar_atributos(esquerda);
                printf("->%c (%c not prime, ", letra, letra);
                mostrar_atributos(esquerda);
                printf(" not superkey)\n");
            }
        }
    }

    free(chaves);
    return 1;
}
