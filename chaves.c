#include <stdlib.h>
#include "chaves.h"

/* Remove um atributo de um conjunto */
static conjunto remover_atributo(conjunto conjunto_atual, char letra) {
    conjunto resultado = 0;

    for (char atual = 'A'; atual <= 'Z'; atual++) {
        conjunto bit = atributo(atual);

        if (subconjunto(bit, conjunto_atual) && atual != letra) {
            resultado = resultado | bit;
        }
    }

    return resultado;
}

/* Encontra todas as chaves candidatas */
int encontrar_chaves(const struct esquema *esquema, conjunto **chaves, int *qtde) {
    *chaves = NULL;
    *qtde = 0;

    conjunto total = esquema->atributos;
    conjunto atual = total;

    // Percorre os subconjuntos possiveis
    while (atual != 0) {
        conjunto resultado = fecho(atual, esquema);

        // Verifica se o conjunto determina todos os atributos
        if (subconjunto(total, resultado)) {

            // Verifica se o conjunto é uma chave mínima
            int min = 1;
            for (char letra = 'A'; letra <= 'Z'; letra++) {
                conjunto atributo_atual = atributo(letra);

                if (subconjunto(atributo_atual, atual)) {
                    conjunto reduzido = remover_atributo(atual, letra);
                    conjunto fecho_reduzido = fecho(reduzido, esquema);

                    if (subconjunto(total, fecho_reduzido)) {
                        min = 0;
                        break;
                    }
                }
            }

            //Guarda o conjunto se for uma chave candidata
            if (min) {
                conjunto *novo = realloc(*chaves, (*qtde + 1) * sizeof(conjunto));

                if (!novo) {
                    free(*chaves);
                    *chaves = NULL;
                    *qtde = 0;
                    return 0;
                }

                *chaves = novo;
                (*chaves)[*qtde] = atual;
                (*qtde)++;
            }
        }

        // Passa para o proximo subconjunto do total 
        atual = (atual - 1) & total;
    }

    return 1;
}
