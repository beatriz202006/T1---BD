#include <stdlib.h>
#include "cobertura.h"

// Separa os atributos do lado direito das dependencias
static int separa_rhs(struct esquema *esquema) {
    int qtde_nova = 0;

    // Conta quantas dependencias vao ser criadas
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            if (subconjunto(atributo(letra), direita)) {
                qtde_nova++;
            }
        }
    }

    // Separa memoria para as novas dependencias
    struct dependencia *novas_dfs = malloc(qtde_nova * sizeof(struct dependencia));

    if (qtde_nova > 0 && novas_dfs == NULL)
        return 0;
    
    int posicao = 0;

    // Cria uma dependencia para cada atributo do lado direito
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto esquerda = esquema->dfs[i].esquerda;
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            conjunto atual = atributo(letra);

            if(subconjunto(atual, direita)) {
                novas_dfs[posicao].esquerda = esquerda;
                novas_dfs[posicao].direita = atual;
                posicao++;
            }
        }
    }

    // substitui as dependencias antigas pelas novas
    free(esquema->dfs);
    esquema->dfs = novas_dfs;
    esquema->quantidade = qtde_nova;

    return 1;
}

/* Remove atributos desnecessarios do lado esquerdo */
static int remover_estranhos(struct esquema *esquema) {
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto esquerda = esquema->dfs[i].esquerda;
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            conjunto atual = atributo(letra);

            if (!subconjunto(atual, esquerda)) {
                continue;
            }

            conjunto reduzida = esquerda & ~atual;

            /* Faz uma copia independente das dependencias */
            struct dependencia *copia =
                malloc(esquema->quantidade * sizeof(struct dependencia));

            if (copia == NULL) {
                return 0;
            }

            for (int j = 0; j < esquema->quantidade; j++) {
                copia[j] = esquema->dfs[j];
            }

            /* Mantem a dependencia original durante o teste */
            struct esquema temporario = *esquema;
            temporario.dfs = copia;

            /* Calcula o fecho sem o atributo testado */
            conjunto resultado = fecho(reduzida, &temporario);

            free(copia);

            if (subconjunto(direita, resultado)) {
                esquema->dfs[i].esquerda = reduzida;
                esquerda = reduzida;
            }
        }
    }

    return 1;
}

/* Remove dependencias funcionais redundantes */
static void remover_redundancias(struct esquema *esquema) {
    int i = 0;

    while (i < esquema->quantidade) {
        /* Guarda a dependencia que sera testada */
        struct dependencia atual = esquema->dfs[i];

        /* Retira temporariamente a dependencia do esquema */
        for (int j = i; j < esquema->quantidade - 1; j++) {
            esquema->dfs[j] = esquema->dfs[j + 1];
        }

        esquema->quantidade--;

        /* Calcula o fecho do lado esquerdo sem a dependencia */
        conjunto resultado = fecho(atual.esquerda, esquema);

        /* Verifica se ainda conseguimos determinar o lado direito */
        if (subconjunto(atual.direita, resultado)) {
            /* A dependencia era redundante; permanece removida */
            continue;
        }

        /* A dependencia era necessaria, entao vamos restaura-la */
        for (int j = esquema->quantidade; j > i; j--) {
            esquema->dfs[j] = esquema->dfs[j - 1];
        }

        esquema->dfs[i] = atual;
        esquema->quantidade++;

        i++;
    }
}

// Calcula a cobertura minima  das dependencias funcionais
int cobertura_minima(struct esquema *esquema) {
    if (!separa_rhs(esquema))
        return 0;
    
    if (!remover_estranhos(esquema))
        return 0;
    
    remover_redundancias(esquema);

    return 1;
}