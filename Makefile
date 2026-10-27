CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

ARQUIVOS = main.c fds.c leitura.c cobertura.c chaves.c normalform.c

fdtool: $(ARQUIVOS)
	$(CC) $(CFLAGS) $(ARQUIVOS) -o fdtool

clean:
	rm -f fdtool