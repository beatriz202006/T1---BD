#ifndef FDS__H
#define FDS__H

#include <stdint.h>

/* Representa um conjunto de atributos
   cada atributo de A ate Z representado por um bit */
typedef uint32_t conjunto;

/* Representa uma dependência funcional */
struct dependencia {
    conjunto esquerda;  // LHS
    conjunto direita;   // RHS
};

/* Guarda as informacoes do esquema: atributos, dfs, quantidade de dfs armazenadas */
struct esquema {
    conjunto atributos;
    struct dependencia *dfs;
    int quantidade;
};

/* Retorna a representacao de um atributo */
conjunto atributo(char letra);

/* Verifica se todos os atributos de A tambem estao em B */
int subconjunto(conjunto A, conjunto B);

/* Calcula o fecho de um conjunto de atributos */
conjunto fecho(conjunto x, const struct esquema *esquema);

#endif