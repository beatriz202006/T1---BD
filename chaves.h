#ifndef CHAVES_H
#define CHAVES_H

#include "fds.h"

/* Encontra todas as chaves candidatas do esquema */
int encontrar_chaves(const struct esquema *esquema, conjunto **chaves, int *qtde);

#endif