#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INPUT 0	 /* entrada para o pipe */
#define OUTPUT 1 /* saida para o pipe */

void table(int M[3][3]);
int winner(int M[3][3]);
int pipe(int[2]);
int verif = 9, counter = 0, win = 0;
int main()
{
	int M[3][3], i, y, x;
	int p2f[2], f2p[2], flag = 0, valx, valy, z, c, k, final;

	if ((pipe(f2p) != -1) && (pipe(p2f) != -1)) /* vai verificar a criacao dos pipes */
	{
		switch (fork()) /* vai criar os processos */
		{
		case -1: /* vai dar erro no fork() */
			fputs("Erro na criacao do processo filho", stderr);
			break;

		case 0: // processo filho que é responsavel pela introdução das coordenadas dos 2 jogadores
		{

			write(f2p[OUTPUT], &flag, sizeof(int)); // salto para o processo pai

			do
			{

				read(p2f[INPUT], &flag, sizeof(int)); // regresso do processo pai ao processo filho para efectuar o pedido de coordenadas aos utilizadores

				c = 1;
				while (c == 1)
				{

					read(p2f[INPUT], &flag, sizeof(int)); // regresso do processo pai ao processo filho para efectuar o pedido de coordenadas ao jogador 1

					printf("JOGADOR 1 -X- introduza as coordenadas: \a\n"); // pergunta ao jogador 1 as coordenadas onde deseja efectuar a sua jogada, o jogador 1 vai ser o simbolo X
					printf("x: ");
					scanf("%d", &valx);
					printf("y: ");
					scanf("%d", &valy);
					printf("\n");

					write(f2p[OUTPUT], &valx, sizeof(int)); // o processo filho envia para o processo pai a coordenada valx (x)  do jogador 1
					write(f2p[OUTPUT], &valy, sizeof(int)); // o processo filho envia para o processo pai a coordenada valy (y)  do jogador 1
					read(p2f[INPUT], &c, sizeof(int));		// o processo filho recebe do processo pai c, se c=0, as coordenadas introduzidas estão correctas e pode sair do while
				}

				read(p2f[INPUT], &final, sizeof(float));

				if (final != verif)
				{
					c = 1;
					while (c == 1)
					{

						read(p2f[INPUT], &flag, sizeof(int)); // regresso do processo pai ao processo filho para efectuar o pedido de coordenadas ao jogador 2

						printf("JOGADOR 2 -O- introduza as coordenadas: \a\n"); // pergunta ao jogador 2 as coordenadas onde deseja efectuar a sua jogada, o jogador 2 vai ser o simbolo O
						printf("x: ");
						scanf("%d", &valx);
						printf("y: ");
						scanf("%d", &valy);
						printf("\n");

						write(f2p[OUTPUT], &valx, sizeof(int)); // o processo filho envia para o processo pai a coordenada valx (x)  do jogador 2
						write(f2p[OUTPUT], &valy, sizeof(int)); // o processo filho envia para o processo pai a coordenada valy (y)  do jogador 2
						read(p2f[INPUT], &c, sizeof(int));		// o processo filho recebe do processo pai c, se c=0, as coordenadas intoduzidas estão correctas e pode sair do while
					}
				}

				read(p2f[INPUT], &final, sizeof(float)); // o processo filho recebe do processo pai final, se final=9,   pode sair do while, pois já houve vencedor ou ocorreu empate

			} while (final != verif);

			close(f2p[INPUT]);	/* fecha o pipe */
			close(f2p[OUTPUT]); /* fecha o pipe */
		}
		break;

		default: // processo pai vai, verifica se a jogada é valida, e verifica se existe vitoria ou empate
		{
			read(f2p[INPUT], &flag, sizeof(int));

			for (y = 0; y < 3; y++)
			{
				for (i = 0; i < 3; i++)
				{
					M[i][y] = 32; //  M[3][3] carregada com o valor 32 (space em ASCII) , para limpar a matriz
				}
			}
			table(M); // função responsavel pelo aspecto gráfico do jogo

			do
			{

				write(p2f[OUTPUT], &flag, sizeof(int)); // salto para o processo filho, para pedir as coordenadas aos jogadores

				z = 1;
				while (z == 1)
				{

					table(M);
					write(p2f[OUTPUT], &flag, sizeof(int)); // salto para o processo filho, para pedir coordenadas ao jogador 1

					read(f2p[INPUT], &x, sizeof(int)); // o processo pai recebe a coordenada x do jogador 1
					read(f2p[INPUT], &y, sizeof(int)); // o processo pai recebe a coordenada y do jogador 1

					if ((x <= 3) && (x >= 1)) // verifica se as coordenadas introduzidas pelo jogador são correctas [1-3]
					{
						if ((y <= 3) && (y >= 1))
						{
							x = x - 1;
							y = y - 1;

							if (M[x][y] != 32) // se a casa escolhida não estiver vazia  entao
							{
								printf("ERROR : A casa escolhida ja esta preenchida\n");
								sleep(1);
								table(M);
								write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (1), para que volte a perguntar novas coordenadas validas ao jogador
							}
							else
							{
								M[x][y] = 88; // X ASCII
								z = 0;
								counter++;
								write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (0), OK, as coordenadas são válidas, pode saltar do ciclo para perguntar ao jogador 2
							}
						}
						else
						{
							printf("ERROR: Coordenadas mal introduzidas (intervalo [1-3])\n");
							sleep(1);
							write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (1), para que volte a perguntar novas coordenadas válidas ao jogador
						}
					}
					else
					{
						printf("ERROR: Coordenadas mal introduzidas (intervalo [1-3])\n");
						sleep(1);
						write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (1), para que volte a perguntar novas coordenadas válidas ao jogador
					}
				}

				table(M);  // representação gráfica da jogada
				winner(M); // função de verificação de vencedor

				//*****************************************************************************************************3
				write(p2f[OUTPUT], &counter, sizeof(int)); // envia ao processo filho o valor do counter, para comunicar ao processo filho se já existe vencedor

				if (counter != verif) // se ainda não existe vencedor, então pergunta as coordenadas ao segundo jogador
				{
					z = 1;
					while (z == 1)
					{

						table(M);
						write(p2f[OUTPUT], &flag, sizeof(int));

						read(f2p[INPUT], &x, sizeof(int));
						read(f2p[INPUT], &y, sizeof(int));

						if ((x <= 3) && (x >= 1))
						{
							if ((y <= 3) && (y >= 1))
							{
								x = x - 1;
								y = y - 1;

								if (M[x][y] != 32)
								{
									printf("ERROR : A casa escolhida ja esta preenchida\n");
									sleep(1);
									table(M);
									write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (1), para que volte a perguntar novas coordenadas válidas ao jogador
								}
								else
								{
									M[x][y] = 79; // O ASCII
									z = 0;
									counter++;
									write(p2f[OUTPUT], &z, sizeof(int)); // envia ao filho (0), OK, as coordenadas são válidas, pode saltar do ciclo
								}
							}
							else
							{
								printf("ERROR: Coordenadas mal introduzidas (intervalo [1-3])\n");
								sleep(1);
								write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (1), para que volte a perguntar novas coordenadas válidas ao jogador
							}
						}
						else
						{
							printf("ERROR: Coordenadas mal introduzidas (intervalo [1-3])\n");
							sleep(1);
							write(p2f[OUTPUT], &z, sizeof(int)); // envia ao processo filho (1), para que volte a perguntar novas coordenadas válidas ao jogador
						}
					}
				}
				table(M);
				winner(M); // verificação do jogador vencedor

				write(p2f[OUTPUT], &counter, sizeof(int)); // envia ao processo filho o valor de counter, se o counter for 9, entao é porque já existe vencedor ou então foi empate

			} while (counter != verif); // salta  já existe vencedor ou então foi empate

			table(M);
			winner(M); // se ouver jogador vencedor o winner escreve no ecrã qual o jogador que ganhou , se não ouver vencedor não escreve nada

			if (win == 0) // se o win for 0 entao não ouve nenhum vencedor logo so pode ser empate, forma 9 jogadas (counter=9) e sem vencedor
				printf("JOGO EMPATADO!!!!! \n\n");

			close(p2f[INPUT]);	/* fecha o pipe */
			close(p2f[OUTPUT]); /* fecha o pipe */
		}
		break;
		}
	}
	else
	{
		perror("Erro na criacao dos pipes");
	}
}

void table(int M[3][3]) // tabela gráfica
{
	system("clear");
	printf("    x   1        2        3        \n");
	printf(" y                             \n");
	printf("            |        |         \n");
	printf(" 1     %c    |   %c    |   %c     \n", M[0][0], M[1][0], M[2][0]);
	printf("            |        |         \n");
	printf("    -------------------------- \n");
	printf("            |        |         \n");
	printf(" 2     %c    |   %c    |   %c     \n", M[0][1], M[1][1], M[2][1]);
	printf("            |        |         \n");
	printf("    -------------------------- \n");
	printf("            |        |         \n");
	printf(" 3     %c    |   %c    |   %c     \n", M[0][2], M[1][2], M[2][2]);
	printf("            |        |         \n");
	printf("\n\n\n\n\n");
}

int winner(int M[3][3]) // verifica se há jogador vencedor
{
	int x, y;

	win = 0; // variavel global, que identifica se houve ou nao vencedor, se 0 então não ouve venvedor, se 1 OK ouve vencedor

	if ((M[0][0] == M[1][0]) && (M[1][0] == M[2][0]) && (M[0][0] != 32))
	{
		win = 1; // venceu
		x = 0;
		y = 0;
	}
	if ((M[0][1] == M[1][1]) && (M[1][1] == M[2][1]) && (M[0][1] != 32))
	{
		win = 1; // venceu
		x = 0;
		y = 1;
	}
	if ((M[0][2] == M[1][2]) && (M[1][2] == M[2][2]) && (M[0][2] != 32))
	{
		win = 1; // venceu
		x = 0;
		y = 2;
	}

	if ((M[0][0] == M[0][1]) && (M[0][1] == M[0][2]) && (M[0][0] != 32))
	{
		win = 1; // venceu
		x = 0;
		y = 0;
	}
	if ((M[1][0] == M[1][1]) && (M[1][1] == M[1][2]) && (M[1][0] != 32))
	{
		win = 1; // venceu
		x = 1;
		y = 0;
	}
	if ((M[2][0] == M[2][1]) && (M[2][1] == M[2][2]) && (M[2][0] != 32))
	{
		win = 1; // venceu
		x = 2;
		y = 0;
	}

	if ((M[0][0] == M[1][1]) && (M[1][1] == M[2][2]) && (M[0][0] != 32))
	{
		win = 1; // venceu
		x = 0;
		y = 0;
	}
	if ((M[2][0] == M[1][1]) && (M[1][1] == M[0][2]) && (M[2][0] != 32))
	{
		win = 1; // venceu
		x = 2;
		y = 0;
	}

	if (win == 1) // venceu alguem
	{
		if (M[x][y] == 88) //  se o caracter que esta na linha vencedor for X entao (88= X ACII)
		{
			printf("O JOGADOR 1 -X- E VENCEDOR\n\n");
			// sleep(2);
			counter = 9;
		}
		else //  senão o caracter que está na linha vencedor é O entao (79= O ACII)
		{
			printf("O JOGADOR 2 -O- E VENCEDOR\n\n");
			// sleep(2);
			counter = 9;
		}
	}
}
