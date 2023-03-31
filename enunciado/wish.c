/*
	para compilar: gcc -o wish wish.c wish_utils.c -lreadline
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_COMMANDS 1000
#define MAX_SIZE 500
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 30

char history[HISTORY_SIZE][BUFFER_SIZE];
int history_count = 0;
#define DELIMITERS " \t\r\n\a\0"

void execute_commands(char *s, char ***mypath)
{
	char *token;
	int fd;
	char *command_string;
	char str[MAX_SIZE];

	// Obtener el primer argumento del comando
	command_string = strtok_r(s, " ", &s);

	// Comparar el primer argumento con las posibles opciones
	if (strcmp(command_string, "exit") == 0)
	{
		execute_exit(s);
	}
	else if (strcmp(command_string, "cd") == 0)
	{
		execute_cd(s);
	}
	else if (strcmp(command_string, "path") == 0)
	{
		execute_path(s, mypath);
	}
	else
	{
		fd = -1;
		char ***mp_copy = malloc(2 * sizeof(char **)); // Crea una copia de mypath
		memcpy(mp_copy, mypath, 2 * sizeof(char **));
		char ***mp = mp_copy;

		char specificpath[MAX_SIZE];

		while ((strcmp((*mp)[0], "") != 0) && fd != 0)
		{
			strcpy(specificpath, *(mp[0]++));
			strncat(specificpath, command_string, strlen(command_string));
			fd = access(specificpath, X_OK);
		}

		// Si el file descriptor existe, osea la ruta del programa y el programa
		if (fd == 0)
		{
			// Como comprobamos que el programa existe, entonces creamos un proceso hijo
			int subprocess = fork();

			// Error lanzando el subproceso
			if (subprocess < 0)
			{
				printf("Error launching the subprocess");
			}
			else if (subprocess == 0)
			{ // Estoy en el proceso hijo
				// Cuento cuantos argumentos tiene el comando
				int num_args = 0;
				char *s_copy = strdup(s);
				char *token = strtok(s_copy, " ");
				while (token != NULL)
				{
					num_args++;
					token = strtok(NULL, " ");
				}
				// Guardo los argumentos en el vector myargs
				int i = 1;
				char *myargs[num_args + 1];
				myargs[0] = strdup(command_string);

				char *s_copy2 = strdup(s);
				token = strtok(s_copy2, " ");
				while (token != NULL)
				{
					myargs[i] = token;
					token = strtok(NULL, " ");
					i++;
				}
				myargs[i] = NULL;
				i = 0;
				while (myargs[i] != NULL)
				{
					i++;
				}
				execv(specificpath, myargs);
			}
			else
			{ // Estoy en el proceso padre y esperaré a que los hijos terminen
				wait(NULL);
			}
		}
		else
		{ // Si el file descriptor no existe
			printf("Command not found: %s\n", str);
		}
	}
	// free(input_line);
}

int main(int argc, char *argv[])
{

	char str[MAX_SIZE];
	char *command_string;
	char *s;
	int fd;
	char specificpath[MAX_SIZE];
	char *input_line;

	char **mypath = malloc(2 * sizeof(char *));
	mypath[0] = "/bin/";
	mypath[1] = "";

	if (argc == 1)
	{
		// Modo interactivo
		do
		{

			input_line = readline("wish> ");
			if (!input_line)
			{
				// EOF o error
				break;
			}
			if (strlen(input_line) > 0)
			{
				add_history(input_line);

				// Copiar cadena de comando al búfer de historial
				if (history_count < HISTORY_SIZE)
				{
					strcpy(history[history_count++], input_line);
				}
				else
				{
					for (int i = 0; i < HISTORY_SIZE - 1; i++)
					{
						strcpy(history[i], history[i + 1]);
					}
					strcpy(history[HISTORY_SIZE - 1], input_line);
				}
			}

			s = input_line;
			// command_string = strtok_r(s, " ", &s);

			// Ejecuto los comandos
			execute_commands(s, &mypath);

		} while (1);
		free(input_line);
	}
	else if (argc == 2) // Modo batch
	{
		char commands[MAX_COMMANDS][MAX_SIZE];
		int num_commands = 0;

		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL)
		{
			printf("Error opening file\n");
			exit(1);
		}

		// Leer lineas del archivo
		while (fgets(commands[num_commands], MAX_SIZE, fp))
		{
			num_commands++;
		}
		fclose(fp);

		// Ejecutar los comandos en orden
		for (int i = 0; i < num_commands; i++)
		{

			input_line = commands[i];

			input_line[strcspn(input_line, "\n")] = '\0';
			s = input_line;
			// command_string = strtok_r(s, " ", &s);

			// Ejecuto los comandos
			execute_commands(s, &mypath);
		}
	}
	else
	{
		printf("Uso: programa [archivo]\n");
		return 1;
	}

	return 0;
}