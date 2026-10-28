# FDTool - Análise de Dependências Funcionais

# 1. Descrição
O programa permite:

- Calcular o fecho de um conjunto de atributos;
- Encontrar a cobertura mínima de um conjunto de dependências funcionais;
- Encontrar todas as chaves candidatas de um esquema relacional;
- Verificar se um esquema está na Forma Normal de Boyce-Codd (BCNF) ou na Terceira Forma Normal (3FN).

# 2. Compilação

Na pasta raiz do projeto, execute:

make

Esse comando utiliza as instruções definidas no `Makefile` para compilar o programa e gerar o executável `fdtool`.

Caso seja necessário remover os arquivos gerados pela compilação, execute:

make clean

Após a compilação, os comandos podem ser executados utilizando `./fdtool`.

# 3. Formato do arquivo de entrada

O programa recebe um arquivo com extensão `.fds`, que deve conter a definição do conjunto universal de atributos `U` e do conjunto de dependências funcionais `F`.

Exemplo de arquivo `exemplo.fds`:

U = {A,B,C,D,E}
F = { A->BC, C->D, BD->E }

Nesse exemplo:

- `U` representa o conjunto de todos os atributos do esquema relacional.
- `F` representa o conjunto de dependências funcionais.
- `A->BC` indica que `A` determina funcionalmente `B` e `C`.
- `BD->E` indica que o conjunto de atributos `BD` determina funcionalmente `E`.

As dependências funcionais podem ser separadas por vírgulas ou pontos e vírgulas.

Os atributos devem ser representados por letras maiúsculas de `A` a `Z`. Todos os atributos utilizados nas dependências devem pertencer ao conjunto `U`.

# 4. Execução

O programa possui quatro comandos principais.

## 4.1. Fecho de atributos

Calcula o fecho de um conjunto de atributos em relação às dependências funcionais do arquivo.

**Sintaxe:**

./fdtool closure --fds arquivo.fds --X ATRIBUTOS

**Exemplo:**

./fdtool closure --fds exemplo.fds --X A

O programa calcula e imprime o fecho de `A`, representado por `A+`.

## 4.2. Cobertura mínima

Calcula e imprime uma cobertura mínima das dependências funcionais do esquema.

**Sintaxe:**

./fdtool mincover --fds arquivo.fds

**Exemplo:**

./fdtool mincover --fds exemplo.fds

O resultado apresenta as dependências da cobertura mínima, uma por linha.

## 4.3. Chaves candidatas

Encontra e imprime todas as chaves candidatas do esquema relacional.

**Sintaxe:**

./fdtool keys --fds arquivo.fds

**Exemplo:**

./fdtool keys --fds exemplo.fds

Cada chave candidata encontrada é apresentada em uma linha.

## 4.4. Formas normais

Verifica se o esquema está na Forma Normal de Boyce-Codd (BCNF) ou na Terceira Forma Normal (3FN).

**Sintaxe:**

./fdtool normalform --fds arquivo.fds

**Exemplo:**

./fdtool normalform --fds exemplo.fds

O programa informa o resultado da verificação de cada forma normal e apresenta as dependências que violam os critérios identificados.

# 5. Exemplos

## Exemplo 1 — Cálculo de fecho

Crie um arquivo chamado `fecho.fds` com o seguinte conteúdo:

U = {A,B,C}
F = {A->B, B->C}

Execute:

./fdtool closure --fds fecho.fds --X A

**Resultado esperado:**

ABC

Isso ocorre porque `A` determina `B`, e `B` determina `C`. Portanto, o fecho de `A` contém os três atributos.

## Exemplo 2 — Encontrando chaves candidatas

Utilize o arquivo `fecho.fds`:

./fdtool keys --fds fecho.fds

**Resultado esperado:**

A

A chave candidata é `A`, pois seu fecho contém todos os atributos do esquema.

## Exemplo 3 — Verificação de formas normais

Crie um arquivo chamado `normalizacao.fds`:

U = {A,B,C}
F = {A->B, B->C}

Execute:

./fdtool normalform --fds normalizacao.fds

**Resultado esperado:**

BCNF: VIOLATIONS
3NF: VIOLATIONS
VIOLATION BCNF: B->C (B not superkey)
VIOLATION 3NF: B->C (C not prime, B not superkey)

Nesse esquema, `A` é a chave candidata. A dependência `B->C` viola a BCNF porque `B` não é superchave. Também viola a 3FN porque `C` não é atributo primo.

# 6. Tratamento de erros

O programa apresenta mensagens de erro em situações como:

- Arquivo de entrada inexistente ou inacessível.
- Arquivo com formato inválido.
- Dependências funcionais com sintaxe incorreta.
- Atributos utilizados nas dependências que não pertencem a `U`.
- Comando desconhecido.
- Argumentos obrigatórios ausentes.
- Falha na alocação de memória durante determinadas operações.

Quando ocorre um erro, o programa encerra a execução com um código de retorno diferente de zero.

# 7. Estrutura do projeto

O projeto está organizado em arquivos C e seus respectivos cabeçalhos, que separam as responsabilidades da implementação.

- `main.c`: recebe os comandos, valida os argumentos e coordena a execução.
- `fds.c` e `fds.h`: implementam operações básicas sobre conjuntos de atributos e o cálculo de fechos.
- `leitura.c` e `leitura.h`: leem e validam os arquivos de entrada.
- `cobertura.c` e `cobertura.h`: implementam o cálculo da cobertura mínima.
- `chaves.c` e `chaves.h`: implementam a busca pelas chaves candidatas.
- `normalform.c` e `normalform.h`: verificam a BCNF e a 3FN.
- `Makefile`: define as instruções de compilação.
- Arquivos `.fds`: contêm exemplos de esquemas e dependências funcionais.
