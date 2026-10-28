#include <stdlib.h>
#include "cobertura.h"

// Separa os atributos do lado direito das dfs
static int separa_rhs(struct esquema *esquema) {
    int qtde_nova = 0;

    // CPercorre todas as dfs do esquema
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            // Verifica se a letra atual pertence ao lado direito
            if (subconjunto(atributo(letra), direita)) {
                qtde_nova++;  // cria uma df para esse atributo
            }
        }
    }

    // Reserva memória para as novas dfs
    struct dependencia *novas_dfs = malloc(qtde_nova * sizeof(struct dependencia));

    // Verifica falha de alocação
    if (qtde_nova > 0 && novas_dfs == NULL)
        return 0;
    
    int posicao = 0;

    // Percorre as dfs originais para criar as novas
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto esquerda = esquema->dfs[i].esquerda;
        conjunto direita = esquema->dfs[i].direita;

        for (char letra = 'A'; letra <= 'Z'; letra++) {
            conjunto atual = atributo(letra);  // converte a letra atual para a representação em bits

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

/* Remove atributos desnecessários do lado esquerdo */
static int remover_estranhos(struct esquema *esquema) {

    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto esquerda = esquema->dfs[i].esquerda;
        conjunto direita = esquema->dfs[i].direita;

        // Testa cada atributo possível para verificar se pode ser removido
        for (char letra = 'A'; letra <= 'Z'; letra++) {
            conjunto atual = atributo(letra);

            // Se o atributo não está na esquerda, não tem o que remover
            if (!subconjunto(atual, esquerda)) {
                continue;
            }

            // Cria um conjunto sem o atributo que está sendo testado (desliga os bits)
            conjunto reduzida = esquerda & ~atual;

            // Reserva memória para criar uma cópia das dependências
            struct dependencia *copia = malloc(esquema->quantidade * sizeof(struct dependencia));

            if (!copia) 
                return 0;

            // Copia todas as dfs para o novo vetor
            for (int j = 0; j < esquema->quantidade; j++) {
                copia[j] = esquema->dfs[j];
            }

            /* Mantém a dependência original durante o teste */
            struct esquema temporario = *esquema;
            temporario.dfs = copia;

            /* Calcula o fecho sem o atributo testado */
            conjunto resultado = fecho(reduzida, &temporario);

            free(copia);

            // Verifica se o fecho ainda contém todo o lado direito. Se sim, atributo testado pode ser removido
            if (subconjunto(direita, resultado)) {
                esquema->dfs[i].esquerda = reduzida;
                esquerda = reduzida;
            }
        }
    }

    return 1;
}

/* Remove dfs redundantes */
static void remover_redundancias(struct esquema *esquema) {
    int i = 0;  //índice da df que vai ser testada

    while (i < esquema->quantidade) {
        // Guarda a df que vai ser testada
        struct dependencia atual = esquema->dfs[i];

        // Retira temporariamente a df do esquema
        for (int j = i; j < esquema->quantidade - 1; j++) {
            esquema->dfs[j] = esquema->dfs[j + 1];
        }

        esquema->quantidade--;

        // Calcula o fecho do lado esquerdo sem a df testada
        conjunto resultado = fecho(atual.esquerda, esquema);

        // Se o lado direito ainda pode ser determinado, df era redundante e fica removida
        if (subconjunto(atual.direita, resultado)) {
            /* A dependencia era redundante; permanece removida */
            continue;
        }

        // Se a df não era redundante, restaura no vetor
        for (int j = esquema->quantidade; j > i; j--) {
            esquema->dfs[j] = esquema->dfs[j - 1];
        }

        esquema->dfs[i] = atual;
        esquema->quantidade++;

        i++;
    }
}

// Calcula a cobertura mínima das dfs
int cobertura_minima(struct esquema *esquema) {

    // Garante que cada df tenha apenas um atributo no lado direito
    if (!separa_rhs(esquema))
        return 0;
    
    // Garante que não há atributos desnecessários do lado esquerdo
    if (!remover_estranhos(esquema))
        return 0;
    
    // Remove dfs redundantes
    remover_redundancias(esquema);

    return 1;
}