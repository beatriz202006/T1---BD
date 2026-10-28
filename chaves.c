#include <stdlib.h>
#include "chaves.h"

/* Remove um atributo de um conjunto */
static conjunto remover_atributo(conjunto conjunto_atual, char letra) {
    conjunto resultado = 0;

    for (char atual = 'A'; atual <= 'Z'; atual++) {
        conjunto bit = atributo(atual);

        // Se o atributo pertence ao conjunto original e não é o que vai ser removido, adiciona ao conjunto resultado
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

    conjunto total = esquema->atributos; // conjunto de todos os atributos do esquema
    conjunto atual = total;

    // Percorre todos os subconjuntos possíveis
    while (atual != 0) {
        conjunto resultado = fecho(atual, esquema);

        // Verifica se o fecho contém todos os atributos do esquema
        if (subconjunto(total, resultado)) {

            // Verifica se o conjunto é uma chave mínima
            int min = 1;
            for (char letra = 'A'; letra <= 'Z'; letra++) {
                conjunto atributo_atual = atributo(letra);

                if (subconjunto(atributo_atual, atual)) {
                    conjunto reduzido = remover_atributo(atual, letra);  // cria um conjunto sem o atributo testado
                    conjunto fecho_reduzido = fecho(reduzido, esquema);  // calcula o fecho do conjunto reduzido
                    
                    // Se o conjunto reduzido ainda determina todos os atributos do esquema, o atributo testado é desnecessário
                    if (subconjunto(total, fecho_reduzido)) {
                        min = 0;  // conjunto original não é mínimo
                        break;
                    }
                }
            }

            //Se min for 1, o conjunto é uma chave candidata
            if (min) {

                // Aumenta o espaço reservado para armazenar mais uma chave
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

        // Passa para o próximo subconjunto de atributos
        atual = (atual - 1) & total;
    }

    return 1;
}
