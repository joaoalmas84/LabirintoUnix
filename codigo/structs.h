#ifndef CODIGOFONTE_STRUCTS_H
#define CODIGOFONTE_STRUCTS_H

#include <pthread.h>

typedef struct {
    int flag;
    int pid;
} PEDIDO_PLAYERS;

typedef struct {
    int flag;
    int pid;
} PEDIDO_EXIT;

typedef struct {
    int tipo; // 1 -> player, 2 -> spectator
    int pid;
    char nome[20];
    int pos;
} CLIENT;

typedef struct {
    int pid;
    char nome[50];
} LOGIN;

typedef struct {
    int flag;
    int pid;
    LOGIN l;
} F_LOGIN;

typedef struct {
    char from[50];
    char to[50];
    char content[100];
} MSG;

typedef struct {
    int flag;
    MSG msg;
} F_MSG;

typedef struct {
    int flag;
    int pid;
    MSG msg;
} MSG_REQUEST;

typedef struct {
    int pid;
    MSG msg;
} MSG_RESULT;

typedef struct {
    int flag;
    MSG_RESULT msg_result;
} F_MSG_RESULT;

typedef struct {
    char mapa[640];
    int nBlocks;
    int nPedras;
    //PEDRAS *sendPedras;/*x*/
} LABIRINTO;

typedef struct {
    int nivel;
    LABIRINTO labirinto;
    CLIENT clientData; // informação sobre o cliente em questão
} JOGO;

typedef struct {
    int flag;
    int pid;
    JOGO j;
} F_JOGO;

typedef struct {
    char tab[500];
} TABELA;

typedef struct {
    int flag;
    TABELA t;
} F_TABELA;

typedef struct {
    char str[100];
} ATUALIZA_JPLAYERS;

typedef struct {
    int flag;
    ATUALIZA_JPLAYERS p;
} F_ATUALIZA_JPLAYERS;

typedef struct {
    char str[100];
} NOTIFICACAO;

typedef struct {
    int flag;
    NOTIFICACAO n;
} F_NOTIFICACAO;

typedef struct {
    char str[100];
} ERROR;

typedef struct {
    int flag;
    ERROR error;
} F_ERROR;

typedef struct {
    int position;
} KICK_REQUEST;

typedef struct {
    int flag;
    KICK_REQUEST kickRequest;
} F_KICK_REQUEST;

// Thread Data
typedef struct {
    int fd;
    int fd_main;
    int begin; // muda para 1 quando é introduzido o comando begin no motor
    int continua;
    pthread_mutex_t *ptrinco;
    int nClients;
    CLIENT *clients;
    int nivel; //nivel do jogo atual
    int continuaBots;
    int interval; //intervalo de tempo que o bot demara a enviar coordenadas
    int inscreve; // 0 -> so permite a inscricao como espectador 1 -> permite inscricao como jogador
    int posOcupadas[5]; // posicoes ocupadas pelos jogadores inicialmente
    int numPosOcupadas; //numero de posicoes ocupadas
} TDATA;

#endif //CODIGOFONTE_STRUCTS_H
