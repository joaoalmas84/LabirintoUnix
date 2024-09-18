#ifndef JOGOUI_H
#define JOGOUI_H

#include "utils.h"
#include "structs.h"

#include <ncurses.h>

//*************************************************** COMANDOS *********************************************************

void executaComando_J(char *cmd, char *playerName, int index, WINDOW *jOutput, int fd);

//*************************************************** MOVIMENTACAO *****************************************************

int verificaMovimento(int position, LABIRINTO labirinto, int ch); // 0 -> negado, 1 -> aceite

JOGO handleMovimento(int ch, JOGO jogo, int *position, char *playerName);

//*************************************************** PREENCHIMENTO DE ESTRUTURAS **************************************

MSG preencheMsg(char *cmd, char *playerName);

F_LOGIN preencheLogin(const char *nome);

F_JOGO preencheF_JOGO(JOGO jogo, F_LOGIN j_login, int position);

//*************************************************** ENVIAR INFORMACAO ************************************************

void enviaFlag(const int fd, int flag, WINDOW *jOutput);

void enviaPid(const int fd, int pid, WINDOW *jOutput);

void enviaPedidoExit(const int fd, WINDOW *jOutput);

void enviaPedidoPlayers(const int fd, WINDOW *jOutput);

void enviaF_Jogo(const int fd, F_JOGO f_jogo, WINDOW *jOutput);

void enviaMSG_Request(const int fd, MSG_REQUEST msgRequest, WINDOW *jOutput);

void enviaF_MSG(const char *fifoCli, F_MSG f_msg, WINDOW *jOutput);

//*************************************************** RECEBER INFORMACAO ***********************************************

int recebeFlag(const int fd, WINDOW *jOutput);

void recebeLixo(const int fd, WINDOW *jOutput);

JOGO recebeJogo(const int fd, WINDOW *jOutput, WINDOW *jInfo, WINDOW *jMapa, int *pos, int *nivelAnt);

void recebeMSG(const int fd, WINDOW *jOutput);

void recebeAtualizaJplayers(const int fd, WINDOW *jplayers, WINDOW *jOutput);

void recebeMsgResult(const int fd, WINDOW *jOutput);

void recebeTabela(const int fd, WINDOW *jOutput);

void recebeNotificacao(const int fd, WINDOW *jOutput);

//*************************************************** OUTRAS ***********************************************************

void desenhaMoldura(int alt, int comp, int linha, int coluna);

#endif