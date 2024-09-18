#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// return 0 -> valido; 1 -> invalido; 2 -> falta argumentos; 3 -> excesso de argumentos;
int validaComando(char* cmd, const int size, const char* cmdArray[], const int nArgCmd[], int *index) {
    int i, j = 0, validCmd = -1;
    char* argsAll;
    char* args[100];
    char* cmdCopy;

    *index = -1; // <- variável para guardar o id do comando

    // Faz uma cópia exata da variável cmd, alocando memória suficiente dependendo da variável "original"
    cmdCopy = strdup(cmd);

    cmdCopy = strtok(cmdCopy, " "); // guarda o nome do comando
    argsAll = strtok(NULL, "\n");

    // verifica se comando principal existe através do array de comandos incicializado
    for (i = 0; i < size; ++i) {
        if (strcmp(cmdArray[i], cmdCopy) == 0) {
            validCmd = 0;// comando correto
            break;
        }
    }

    if (validCmd != 0) {return 1;} // se validCmd for 1 signifa que comando nao existe, logo retorna valor 1

    //verifica quantos argumentos foram introduzidos no comando e guarda na variavel j
    if (argsAll != NULL) {
        j = -1;
        while (args[j] != NULL) {
            j++;
            if (j == 0) {
                args[j] = strtok(argsAll, " ");
            } else if (j == 1 && strcmp(cmdArray[i], "msg") == 0) { // Se for msg
                args[j] = strtok(NULL, "\n");                       // ler o resto da linha
            } else {
                args[j] = strtok(NULL, " ");
            }
        }
    }

    free(cmdCopy); //liberta a memória alocada para a cópia do cmd

    if (nArgCmd[i] > j) {return 2;}
    else if (nArgCmd[i] < j) {return 3;}

    *index = i;
    return 0;
}

char* tolowerString(char* s) {
    char *aux = malloc(sizeof(char)*(strlen(s)));
    int i;
    for (i = 0; s[i] != '\0'; ++i) {
        aux[i] = tolower(s[i]);
    }
    aux[i] = '\0';
    return aux;
}

char* toupperString(char* s) {
    char *aux = malloc(sizeof(char)*(strlen(s)));
    int i;
    for (i = 0; s[i] != '\0'; ++i) {
        aux[i] = toupper(s[i]);
    }
    aux[i] = '\0';
    return aux;
}