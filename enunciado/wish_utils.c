#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"


void execute_exit(char *args)
{
	exit(0);
}

/*
void execute_cd(char *newpath){
	char *path = strtok_r(newpath, " ", &newpath);
	chdir(path);
}

*/

// Implementación del comando cd para cambiar la ruta del directorio actual,sólo debe recibir 1 argumento que será el cambio a la nnueva ruta
void execute_cd(char *newpath)
{
	printf("Variable newpath %s\n", newpath);
	char *path = strtok_r(newpath, " ", &newpath);
	if (path == NULL)
	{
		printf("Error: el comando cd requiere un argumento\n");
	}
	else
	{
		if (strtok_r(NULL, " ", &newpath) != NULL)
		{
			printf("Error: el comando cd solo acepta un argumento\n");
		}
		else
		{
			if (access(path, F_OK) == 0)
			{
				chdir(path);
				printf("La nueva ruta es %s\n", path);
			}
			else
			{
				printf("Directory does not exist: %s\n", path);
			}
		}
	}
}

// Implementación del comando path que contiene las rutas donde se buscarán los comandos ejecutables en el programa,
// puede recibir cero(Se conserva el path actual) o más argumentos(se actualiza el path actual con el numero de argumentos que se ingresen)
void execute_path(char *newpath, char ***mypath)
{
	char *copypath = strdup(newpath);
	int path_count = 0;
	int i = 0;
	char *path = strtok_r(copypath, " ", &copypath);

	//printf("COMANDOS en path executa: %s\n", newpath);

	// count number of paths to be added
	while (path != NULL)
	{
		path_count++;
		path = strtok_r(copypath, " ", &copypath);
	}
	// Path con cero argumentos
	if (path_count == 0)
	{

		printf("Variable mypath sin modificaciones\n");
	}
	else
	{ // Path con n argumentos
		*mypath = realloc(*mypath, (path_count + 1) * sizeof(char *));

		path = strtok_r(newpath, " ", &newpath);
		i = 0;
		while (path != NULL)
		{
			(*mypath)[i] = strdup(path);
			path = strtok_r(newpath, " ", &newpath);
			i++;
		}
		// set last element of mypath to ""
		(*mypath)[i] = "";
	}
}
