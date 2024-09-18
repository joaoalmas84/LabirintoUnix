#ifndef MOTOR_H
#define MOTOR_H

#include "utils.h"
#include "structs.h"

#define BOT "bot"
#define TAM 3
#define MAX_READ_SIZE 100
#define FIFO_MAIN "fifo_main"
#define FIFO_INSCR "fifo_inscr"

//*************************************************** COMANDOS *********************************************************

void executaComando_M(char *cmd, int index, TDATA *td, pthread_t t); // se retornar != 0 termina programa

void USERS(CLIENT *clients, int nClients); // Comando USERS

void KICK(TDATA *td, char *nome); // Comando KICK (return 0 se o kick for bem sucedido)

void BEGIN(TDATA *td, pthread_t t); // Comando BEGIN

void END(TDATA *td); // Comando END

//*************************************************** MAPAS ************************************************************

F_JOGO mudaMapa(F_JOGO f_jogo);

void mapaNivel1(char *map);

void mapaNivel2(char *map);

void mapaNivel3(char *map);

//*************************************************** CLIENTS **********************************************************

CLIENT* addClient(CLIENT *clients, int *nClients, LOGIN login, int *posOcupadas, int *numPosOcupadas, int inscreve);

CLIENT* updatePosCliente(CLIENT *clients, int nClients, JOGO jogo);

CLIENT* dellClient (CLIENT *clients, int *nClients, int id);

CLIENT* clearClientList(CLIENT* clients, int *nClients);

void shutDownClients(CLIENT *clients, int nClients);

int encontraPidClient(CLIENT *clients, int nClients, char* playerName); // Recebe o nome, se existir retorna o seu PID, se não retorna 0

void encontraNomeClient(CLIENT *clients, int nClients, int pid, char nome[20]); // Recebe o pid de um suposto utilzador, se existir preenche a string nome com o seu nome

int countPlayers(CLIENT *clients, int nClients); // retorna n.º de players

//*************************************************** THREADS **********************************************************

TDATA inicializaThreadData(int fd, int fd_main, pthread_mutex_t *trinco); // Inicialização das variáveis da estrutura td (Thread Data)

//*************************************************** THREAD COMUNICACAO ***********************************************

void *comunicacaoCliente(void *data);

void enviaFlagFifo(const char *fifoCli, int flag);

void enviaFlagFd(int fd, int flag);

void enviaPid(int fd, int pid);

void enviaF_KICK_REQUEST(int fd, int position);

void enviaF_JOGO(const char *fifoCli, const F_JOGO *f_jogo);

void enviaF_MSG_RESULT(const char *fifoCli, const F_MSG_RESULT *f_msgResult);

void enviaF_TABELA(const char *fifoCli, const F_TABELA *f_tabela);

int recebeFlag(int fd);

int recebePid(int fd);

MSG recebeMSG(int fd);

LOGIN recebeLogin(int fd);

JOGO recebeJogo(int fd);

int recebeKickRequest(int fd);

//*************************************************** THREAD TIMEINSCRICAO *********************************************

void *coordenadasBot(void *data);

void *timeInscricao(void *data); // <- Contagem decrescente 15 segundos para os jogadores se inscreverem, se chegar a 0 e não houverem clientes termina o motor

//*************************************************** OUTRAS ***********************************************************

void init_var_ambiente();

void preencheTabela(CLIENT *clients, int nClients, char tab[500]);

void atualiza_jplayers(CLIENT *clients, int nClients);

void notificaClients(CLIENT *clients, int nClients, char *mensagem);

int getFromBot(int coord[], int tam, int interval, char *duration);

F_JOGO preencheF_JOGO(const TDATA td);

int geraPosicao(int *posOcupadas, int *numPosOcupadas);

int posEstaOcupada(int pos, int *posOcupadas, int numPosOcupadas);

int getPosicao(const TDATA *td, int pid);

int verificaClientTipo(const TDATA *td, int pid); // retorna 1 se for player; retorna 0 se for spectator; retorna -1 se o player nao existir

#endif