#include <stdio.h>
#include <stdlib.h>
#include "leitura.h"

/* Lê as letras e transforma cada uma em um atributo do conjunto */
static conjunto ler_atributos(const char *texto) {
    conjunto resultado = 0;

    while (*texto != '\0') {
        if (*texto >= 'A' && *texto <= 'Z') {
            resultado = resultado | atributo(*texto);  // adiciona o atributo ao conjunto
        }

        texto++;
    }

    return resultado;
}

/* Lê as dfss de uma linha e armazena separadamente */
static int ler_dependencias(char *linha, struct esquema *esquema) {
    int i = 0;

    // Procura o inicio das dependencias, indicado por {
    while (linha[i] != '{' && linha[i] != '\0') {
        i++;
    }

    if (linha[i] == '\0') 
        return 0;
    
    i++;

    // Lê cada dependência até encontrar }
    while (linha[i] != '}' && linha[i] != '\0') {
        conjunto esquerda = 0;
        conjunto direita = 0;
        int lado = 0;

        // lê os atributos da df atual
         while (linha[i] != ',' && linha[i] != ';' && linha[i] != '}' && linha[i] != '\0') {
            if (linha[i] == '-') {
                if (linha[i + 1] != '>') {
                    return 0;
                }

                lado = 1; // a partir daqui os atributos pertencem ao lado direito
                i += 2;   // pula os dois caracteres da seta: ->
            } else {
                if (linha[i] >= 'A' && linha[i] <= 'Z') {
                    if (lado == 0) // antes da seta
                        esquerda = esquerda | atributo(linha[i]);
                    else  // depois da seta
                        direita = direita | atributo(linha[i]);
                } else if (linha[i] != ' ' && linha[i] != '\t' &&
                        linha[i] != '\n' && linha[i] != '\r') {
                    return 0;
                }

                i++;  // avança para o próximo caractere
            }
        }

        // Verifica se a df tem lhs, rhs e uma seta
        if (esquerda == 0 || direita == 0 || lado == 0) 
            return 0;
        
        // Reserva espaço para uma nova df
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

/* Lê os atributos e as dfs de um arquivo .fds */
int ler_esquema(const char *nome_arquivo, struct esquema *esquema) {
    FILE *arquivo = fopen(nome_arquivo, "r");  // abre o arquivo em modo leitura

    if (arquivo == NULL) {  
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return 0;
    }

    char linha[1024];  // buffer que armazena temporariamente cada linha lida
    int encontrou_atributos = 0;
    int encontrou_dependencias = 0;

    // Inicializa o esquema
    esquema->atributos = 0;
    esquema->dfs = NULL;
    esquema->quantidade = 0;

    // Lê cada linha até chegar no final do arquivo
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {

        // Procura a linha que começa com U
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

            // Converte os atributos depois do sinal de igual em um conjunto
            esquema->atributos = ler_atributos(&linha[i + 1]);

            if (esquema->atributos == 0) {
                fclose(arquivo);
                liberar_esquema(esquema);
                return 0;
            }

            encontrou_atributos = 1;
        }

        // Procura a linha que começa com F
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

    // Verifica se as dfs usam apenas atributos de U
    for (int i = 0; i < esquema->quantidade; i++) {
        conjunto usados = esquema->dfs[i].esquerda | esquema->dfs[i].direita;  // Reúne os atributos dos dois lados da df

        if (!subconjunto(usados, esquema->atributos)) {
            liberar_esquema(esquema);
            return 0;
        }
    }

    return 1;
}

/* Libera a memória usada pelas dependências */
void liberar_esquema(struct esquema *esquema) {
    free(esquema->dfs);

    esquema->dfs = NULL;
    esquema->quantidade = 0;
    esquema->atributos = 0;
}