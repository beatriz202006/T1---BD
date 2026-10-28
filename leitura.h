#ifndef LEITURA_H
#define LEITURA_H

#include "fds.h"

/* Lê o esquema de um arquivo .fds */
int ler_esquema(const char *nome_arquivo, struct esquema *esquema);

/* Libera a memória usada pelas dependências */
void liberar_esquema(struct esquema *esquema);

#endif