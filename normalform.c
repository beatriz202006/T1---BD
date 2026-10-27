#include <stdio.h>
#include <stdlib.h>
#include "normalform.h"
#include "chaves.h"

/* Verifica se um conjunto é uma superchave */
static int eh_superchave(conjunto atributos, const struct esquema *esquema) {
    conjunto resultado = fecho(atributos, esquema);

    return subconjunto(esquema->atributos, resultado);
}

/* Verifica se um atributo pertence a alguma chave candidata */
static int eh_primo(char letra, const conjunto *chaves, int qtde) {
    conjunto bit = atributo(letra);

    for (int i = 0; i < qtde; i++) {
        if (subconjunto(bit, chaves[i])) {
            return 1;
        }
    }

    return 0;
}

/* Mostra os atributos de um conjunto */
static void mostrar_atributos(conjunto atributos) {
    for (char letra = 'A'; letra <= 'Z'; letra++) {
        conjunto bit = atributo(letra);

        if (subconjunto(bit, atributos)) {
            printf("%c", letra);
        }
    }
}

/* Verifica as formas normais do esquema */
int verificar_formas_normais(const struct esquema *esquema) {
    conjunto *chaves = NULL;
    int qtde = 0;

    if (!encontrar_chaves(esquema, &chaves, &qtde)) {
        printf("Erro ao encontrar as chaves candidatas.\n");
        return 0;
    }

    int viola_bcnf = 0;
    int viola_3fn = 0;

    /* Primeira passagem: verifica se existem violacoes */
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
                viola_bcnf = 1;
            }

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

    /* Segunda passagem: mostra as dependencias que violam as formas normais */
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
