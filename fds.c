#include <stdio.h>
#include <stdlib.h>
#include "fds.h"

// A corresponde ao bit 0, B ao bit 1, ate Z (bit 25)
conjunto atributo(char letra) {
    if (letra < 'A' || letra > 'Z')
        return 0;
    
    return (conjunto)1 << (letra - 'A');
}

int subconjunto(conjunto A, conjunto B) {
    return (A & B) == A;
}


/* Calcula o fecho de X percorrendo as dfs e adicionando os atributos da direita de todos os atributos 
   que ja pertencem ao fecho atual
*/ 
conjunto fecho(conjunto x, const struct esquema *esquema) {
    conjunto resultado = x;
    int mudou;  // indica se algum atributo foi adicionado ao resultado

    // Repete enquanto encontrar novos atributos para adicionar ao fecho
    do {
        mudou = 0;
        
        // percorre todas as dfs do esquema
        for (int i = 0; i < esquema->quantidade; i++) {
            conjunto esquerda = esquema->dfs[i].esquerda;
            conjunto direita = esquema->dfs[i].direita;

            // Verifica se todos os atributos da esquerda estao no fecho atual
            if (subconjunto(esquerda, resultado)) {
                
                // Adiciona os atributos da direita no fecho
                conjunto novo = resultado | direita;  // combina os bits dos dois conjuntos

                if (novo != resultado) {
                    resultado = novo;
                    mudou = 1;
                }
            }
        }
    } while (mudou);

    return resultado;
}
