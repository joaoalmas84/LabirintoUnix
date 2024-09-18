#ifndef TP_SO_UTILS_H
#define TP_SO_UTILS_H

#define FIFO_MOTOR "server"
#define MAP_SIZE 640
#define N_LIN 17
#define N_COL 40

#define MAXUSERS 5
#define PEDRASMAX 50
#define BLOCKMOVEIS 5

int validaComando(char* cmd, const int size, const char* cmdArray[], const int nArgCmd[], int *index); // return 0 -> valido; 1 -> invalido; 2 -> falta argumentos; 3 -> excesso de argumentos;

char* tolowerString(char* s); // recebe uma string e retorna-a em lowercase

char* toupperString(char* s); // recebe uma string e retorna-a em uppercase

#endif //TP_SO_UTILS_H
