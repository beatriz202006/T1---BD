#include <stdio.h>
#include <stdlib.h>
#include "leitura.h"

/* Le as letras e transforma cada uma em um atributo do conjunto */
static conjunto ler_atributos(const char *texto) {
    conjunto resultado = 0;

    while (*texto != '\0') {
        if (*texto >= 'A' && *texto <= 'Z') {
            resultado = resultado | atributo(*texto);
        }

        texto++;
    }

    return resultado;
}

/* Le as dependencias funcionais de uma linha e armazena separadamente */
static int ler_dependencias(char *linha, struct esquema *esquema) {
    int i = 0;

    // Procura o inicio das dependencias, indicado por {
    while (linha[i] != '{' && linha[i] != '\0') {
        i++;
    }

    if (linha[i] == '\0') 
        return 0;
    
    i++;

    // Le cada dependencia ate encontrar }
    while (linha[i] != '}' && linha[i] != '\0') {
        conjunto esquerda = 0;
        conjunto direita = 0;
        int lado = 0;

        // le os atributos da dependencia atual
         while (linha[i] != ',' && linha[i] != ';' && linha[i] != '}' && linha[i] != '\0') {
            if (linha[i] == '-') {
                if (linha[i + 1] != '>') {
                    return 0;
                }

                lado = 1;
                i += 2;
            } else {
                if (linha[i] >= 'A' && linha[i] <= 'Z') {
                    if (lado == 0)
                        esquerda = esquerda | atributo(linha[i]);
                    else
                        direita = direita | atributo(linha[i]);
                } else if (linha[i] != ' ' && linha[i] != '\t' &&
                        linha[i] != '\n' && linha[i] != '\r') {
                    return 0;
                }

                i++;
            }
        }

        // a dependencia precisa ter os dois lados
        if (esquerda == 0 || direita == 0 || lado == 0) 
            return 0;
        
        // aumenta o espaco para guardar mais uma dependencia
        struct dependencia *nova_mem = realloc(esquema->dfs, (esquema->quantidade + 1) * sizeof(struct dependencia));
        if (!nova_mem)
            return 0;
        
        esquema->dfs = nova_mem;

        // guarda a dependencia lida
        esquema->dfs[esquema->quantidade].esquerda = esquerda;
        esquema->dfs[esquema->quantidade].direita = direita;
        esquema->quantidade++;

        if (linha[i] == ',' || linha[i] == ';') 
            i++;
    }

    if (linha[i] != '}')
        return 0;
    
    return 1;
}

/* Le os atributos e as dependencias de um arquivo .fds */
int ler_esquema(const char *nome_arquivo, struct esquema *esquema) {
    FILE *arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return 0;
    }

    char linha[1024];
    int encontrou_atributos = 0;
    int encontrou_dependencias = 0;

    // Inicializa o esquema
    esquema->atributos = 0;
    esquema->dfs = NULL;
    esquema->quantidade = 0;

    // Le o arquivo linha por linha
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        // Procura a linha que comeca com U
        if (linha[0] == 'U') {
            int i = 0;

            // Procura o sinal de igual
            while (linha[i] != '=' && linha[i] != '\0') {
                i++;
            }

            if (linha[i] == '\0') {
                fclose(arquivo);
                liberar_esquema(esquema);
                return 0;
            }

            // Le os atributos que aparecem depois do sinal de igual
            esquema->atributos = ler_atributos(&linha[i + 1]);

            if (esquema->atributos == 0) {
                fclose(arquivo);
                liberar_esquema(esquema);
                return 0;
            }

            encontrou_atributos = 1;
        }

        // Procura a linha que comeca com F
        else if (linha[0] == 'F') {
            if (!ler_dependencias(linha, esquema)) {
                fclose(arquivo);
                liberar_esquema(esquema);
                return 0;
            }

            encontrou_dependencias = 1;
        }
    }

    fclose(arquivo);

    // Verifica se as duas partes foram encontradas
    if (!encontrou_atributos || !encontrou_dependencias) {
        liberar_esquema(esquema);
        return 0;
    }

    // Verifica se as dependencias usam apenas atributos de U
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto usados = esquema->dfs[i].esquerda |
                          esquema->dfs[i].direita;

        if (!subconjunto(usados, esquema->atributos)) {
            liberar_esquema(esquema);
            return 0;
        }
    }

    return 1;
}

/* Libera a memoria usada pelas dependencias */
void liberar_esquema(struct esquema *esquema) {
    free(esquema->dfs);

    esquema->dfs = NULL;
    esquema->quantidade = 0;
    esquema->atributos = 0;
}