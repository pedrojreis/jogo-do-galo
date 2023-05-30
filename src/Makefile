#Comandos
CC=gcc
RM=rm


#Programas
PROGS=jogo_galo


#Quando se faz make
all: ${PROGS}


#Compilar programas
jogo_galo: jogo_galo.c
	${CC} jogo_galo.c -o jogo_galo


#remover todos os programas
clean:
	${RM} ${PROGS}


#Refazer o programa
rebuild: clean ${PROGS}
	@echo programas criados
