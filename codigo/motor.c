#include "motor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>

//*************************************************** COMANDOS *********************************************************

void executaComando_M(char *cmd, int index, TDATA *td, pthread_t t) {
    char *cmdCopy, *args;
    char nome[50], fifoCli[50], mensagem[100];

    // Faz uma cópia exata da variável cmd, alocando memória suficiente dependendo da variável "original"
    cmdCopy = strdup(cmd);

    cmdCopy = strtok(cmdCopy, " ");
    args = strtok(NULL, "\n");

    switch (index) {
        case 0: // <- (users)
            USERS(td->clients, td->nClients);
            break;
        case 1: // <- (kick)
            KICK(td, args);
            if (countPlayers(td->clients, td->nClients) == 0) {
                printf("\nJogo terminou por falta de jogadores.");
                END(td);
            }
            break;
        case 2: // <- (bots)
            printf("\nComando: %s nao implementado", cmdCopy);
            break;
        case 3: // <- (bmov)
            printf("\nComando: %s nao implementado", cmdCopy);
            break;
        case 4: // <- (rbm)
            printf("\nComando: %s nao implementado", cmdCopy);
            break;
        case 5: // <- (begin)
            td->nClients > 0 ? BEGIN(td, t) : printf("\nImpossível começar o jogo sem jogadores");
            break;
        case 6: // <- (end)
            END(td);
            break;
        default:
            break;
    }

    free(cmdCopy);//liberta a memória alocada para a cópia do cmd
}

void USERS(CLIENT *clients, int nClients) {
    if (nClients == 0) {return;}

    printf("\nClientes\nPID\tNome\tTipo\t\tCoord");
    for (int i = 0; i < nClients; ++i) {
        if (clients[i].tipo == 1) {
            printf("\n%d\t%s\tPlayer\t\t(%d)", clients[i].pid, clients[i].nome, clients[i].pos);
        } else if (clients[i].tipo == 2) {
            printf("\n%d\t%s\tSpectator\t(%d)", clients[i].pid, clients[i].nome, clients[i].pos);
        }
    }
}

void KICK(TDATA *td, char *nome) {
    int pid, position;
    char fifoCli[50], mensagem[100];

    pid = encontraPidClient(td->clients, td->nClients, nome);

    if (pid == 0) {
        printf("\nJogador nao existe.");
        return;
    }


    if (verificaClientTipo(td, pid) == 1) {
        position = getPosicao(td, pid);
        enviaF_KICK_REQUEST(td->fd, position);
    }

    pthread_mutex_lock(td->ptrinco);
    td->clients = dellClient(td->clients, &td->nClients, pid);
    pthread_mutex_unlock(td->ptrinco);

    sprintf(fifoCli, "cli%d", pid);

    enviaFlagFifo(fifoCli, 7);

    if (countPlayers(td->clients, td->nClients) == 0) {return;}

    atualiza_jplayers(td->clients, td->nClients);

    sprintf(mensagem, "%s foi expulso do jogo.", nome);

    notificaClients(td->clients, td->nClients, mensagem);

}

void BEGIN(TDATA *td, pthread_t t) {
    char fifoCli[50];

    pthread_mutex_lock(td->ptrinco);
    td->begin = 1;
    pthread_mutex_unlock(td->ptrinco);

    for (int i = 0; i < td->nClients; ++i) {
        sprintf(fifoCli, "cli%d", td->clients[i].pid);
        enviaFlagFifo(fifoCli, 0);
    }
    notificaClients(td->clients, td->nClients, "Inicio do jogo!");

    if (pthread_join(t, NULL) != 0) {
        perror("Erro");
    }
}

void END(TDATA *td) {

    shutDownClients(td->clients, td->nClients);

    pthread_mutex_lock(td->ptrinco);
    td->clients = clearClientList(td->clients, &td->nClients);
    pthread_mutex_unlock(td->ptrinco);

    enviaFlagFd(td->fd, -1);

    enviaPid(td->fd, getpid());

    enviaFlagFd(td->fd_main, -1);

}

//*************************************************** MAPAS ************************************************************

F_JOGO mudaMapa(F_JOGO f_jogo) {
    F_JOGO aux = f_jogo;

    switch (f_jogo.j.nivel) {
        case 1:
            mapaNivel1(aux.j.labirinto.mapa);
            break;
        case 2:
            mapaNivel2(aux.j.labirinto.mapa);
            break;
        case 3:
            mapaNivel3(aux.j.labirinto.mapa);
            break;
    }

    return aux;
}

void mapaNivel1(char *map) {
    size_t bytesRead;
    char buffer[MAX_READ_SIZE];

    FILE* file = fopen("mapa", "r");

    if (file == NULL) {
        // Não foi possível abrir o arquivo
        perror("\nErro ao abrir o arquivo\n");
        exit(1);
    } else {
        strcpy(map, "");
        while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            strncat(map, buffer, bytesRead);//coloca na var map o que está no buffer com o tamanho de bytes lidos
        }
        fclose(file);
    }
}

void mapaNivel2(char *map) {
    size_t bytesRead;
    char buffer[MAX_READ_SIZE];

    FILE* file = fopen("mapa2", "r");

    if (file == NULL) {
        // Não foi possível abrir o arquivo
        perror("\nErro ao abrir o arquivo\n");
        exit(1);
    } else {
        strcpy(map, "");
        while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            strncat(map, buffer, bytesRead);//coloca na var map o que está no buffer com o tamanho de bytes lidos
        }
        fclose(file);
    }
}

void mapaNivel3(char *map) {
    size_t bytesRead;
    char buffer[MAX_READ_SIZE];

    FILE* file = fopen("mapa3", "r");

    if (file == NULL) {
        // Não foi possível abrir o arquivo
        perror("\nErro ao abrir o arquivo\n");
        exit(1);
    } else {
        strcpy(map, "");
        while((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            strncat(map, buffer, bytesRead);//coloca na var map o que está no buffer com o tamanho de bytes lidos
        }
        fclose(file);
    }

}

//*************************************************** CLIENTS **********************************************************

CLIENT* addClient(CLIENT *clients, int *nClients, LOGIN login, int *posOcupadas, int *numPosOcupadas, int inscreve) {

    for (int i = 0; i < *nClients; ++i) {
        if (clients[i].pid == login.pid) {
            return clients;
        }
    }

    CLIENT *aux = realloc(clients, sizeof(CLIENT)*(*nClients+1));
    if (aux == NULL) {
        printf("\n[ERRO] - Alocação de memória para o novo cliente falhou!\n");
        return NULL;
    }

    clients = aux;

    clients[*nClients].pid = login.pid;
    strcpy(clients[*nClients].nome, login.nome);

    if (*nClients+1 < MAXUSERS && inscreve == 1) {
        clients[*nClients].tipo = 1;
    } else {
        clients[*nClients].tipo = 2;
    }

    //printf("\nNOME ADD %s\n", clients[*nClients].nome);

    clients[*nClients].pos = geraPosicao(posOcupadas, numPosOcupadas);
    //printf("\nNOME ADD %s\n", clients[*nClients].nome);
    //printf("POSICAO GERADA %d", clients[*nClients].pos);

    (*nClients)++;

    return clients;
}

CLIENT* updatePosCliente(CLIENT *clients, int nClients, JOGO jogo) {

    for(int i = 0; i < nClients; i++) {
        if (jogo.clientData.pid == clients[i].pid) {
            clients[i].pos = jogo.clientData.pos;
        }
    }

    return clients;
}

CLIENT* dellClient(CLIENT *clients, int *nClients, int id) {
    CLIENT *aux = NULL;

    if (*nClients == 1) {
        *nClients = 0;
        free(clients);
        return NULL;
    }

    for (int i = 0; i < *nClients; ++i) {
        if (id == clients[i].pid) {
            for (int j = i; j < *nClients - 1; ++j) {
                clients[j] = clients[j + 1];
            }
            break;
        }
    }

    aux = realloc(clients, sizeof(CLIENT)*(*nClients-1));
    if (aux == NULL) {
        printf("\n[ERRO] - Realocação de memória para dellClient falhou!\n");
        return NULL;
    } else {
        (*nClients)--;
        clients = aux;
    }

    return clients;
}

CLIENT* clearClientList(CLIENT* clients, int *nClients) {
    free(clients);
    *nClients = 0;
    return NULL;
}

void shutDownClients(CLIENT *clients, int nClients) {
    int fd_resposta, nbytes;
    char fifoCli[50];

    if (nClients == 0) {return;}

    for (int i = 0; i < nClients; ++i) {
        sprintf(fifoCli, "cli%d", clients[i].pid);

        fd_resposta = open(fifoCli, O_WRONLY);
        if (fd_resposta == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            enviaFlagFifo(fifoCli, 7);
            close(fd_resposta);
        }
    }
}

// Recebe o nome, se existir retorna o seu PID, se não retorna 0
int encontraPidClient(CLIENT *clients, int nClients, char* playerName) {
    if (nClients == 0) {return 0;}
    for (int i = 0; i < nClients; ++i) {
        if (strcmp(clients[i].nome, playerName) == 0) {
            return clients[i].pid;
        }
    }
    return 0;
}

void encontraNomeClient(CLIENT *clients, int nClients, int pid, char nome[20]) {
    for (int i = 0; i < nClients; ++i) {
        if (clients[i].pid == pid) {
            strcpy(nome, clients[i].nome);
            return;
        }
    }
    strcpy(nome, "");
}

int countPlayers(CLIENT *clients, int nClients) {
    int count = 0;
    for (int i = 0; i < nClients; ++i) {
        if (clients[i].tipo == 1) {count++;}
    }
    return count;
}

//*************************************************** THREADS **********************************************************

TDATA inicializaThreadData(int fd, int fd_main, pthread_mutex_t *trinco) {
    TDATA td;

    td.fd = fd;
    td.fd_main = fd_main;
    td.continua = 1;
    td.ptrinco = trinco;
    td.nClients = 0;
    td.clients = NULL;
    td.nivel = 1;
    td.inscreve = 1;
    td.numPosOcupadas = 0;

    return td;
}

//*************************************************** THREAD COMUNICACAO ***********************************************

void *comunicacaoCliente(void *data) {
    TDATA *ptd = (TDATA*)  data;
    int sair = 1, tipo, position;
    char nome[20], mensagem[100];
    int nivelAnt = 1; //nivel antes de receber novos dados de jogoUI

    // variaveis relacionadas com comunicação com o jogoUI
    int fd_resposta, cont = 3;
    char fifoCli[50], n[10];

    // Estruturas que envia
    int flagEnvia;
    F_JOGO f_jogo;
    F_MSG_RESULT f_msgResult;
    F_TABELA f_tabela;

    // Estruturas que recebe
    int flagRecebe;
    int pid;
    LOGIN login;
    JOGO jogo;
    MSG msg;

    // Coloca na estrutura JOGO o nivel atual
    jogo.nivel = ptd->nivel;

    do {
        strcpy(fifoCli, "");

        flagRecebe = recebeFlag(ptd->fd);

        if (flagRecebe == -1) {break;}

        if (flagRecebe != 0 && flagRecebe != 6) {
            pid = recebePid(ptd->fd);
            sprintf(fifoCli, "cli%d", pid);
        }

        switch (flagRecebe) {
            case 0:
                if (ptd->begin == 1) {
                    printf("\nInicio do jogo");
                    for (int i = 0; i < ptd->nClients; ++i) {
                        sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                        enviaFlagFifo(fifoCli, 0);
                    }
                }
                notificaClients(ptd->clients, ptd->nClients, "Inicio do jogo!");
                break;
            case 1: // login
                login = recebeLogin(ptd->fd);

                // se ja existir um jogador com este nome envia flag = -1 para terminar o processo
                if (encontraPidClient(ptd->clients, ptd->nClients, login.nome) != 0) {
                    enviaFlagFifo(fifoCli, -1);
                    break;
                }

                pthread_mutex_lock(ptd->ptrinco);
                ptd->clients = addClient(ptd->clients, &ptd->nClients, login, ptd->posOcupadas, &ptd->numPosOcupadas, ptd->inscreve);
                pthread_mutex_unlock(ptd->ptrinco);

                f_jogo = preencheF_JOGO(*ptd);

                //coloca os "avatares" no mapa se a isncrição for do tipo player
                for (int i = 0; i< ptd->nClients; i++) {
                    if (login.pid == ptd->clients[i].pid) { //envia a posicao do avatar se for o cliente que fez o "pedido"
                        f_jogo.j.clientData.pos = ptd->clients[i].pos;
                    }
                    if (ptd->clients[i].tipo == 1) {
                        f_jogo.j.labirinto.mapa[ptd->clients[i].pos] = tolower(ptd->clients[i].nome[0]);
                    }
                }

                //envia o mapa atualizado para todos os jogadores
                for (int i = 0; i < ptd->nClients; ++i) {
                    sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                    enviaF_JOGO(fifoCli, &f_jogo);
                }

                ptd->inscreve == 1 ? enviaFlagFifo(fifoCli, 1) : enviaFlagFifo(fifoCli, 2);

                atualiza_jplayers(ptd->clients, ptd->nClients);

                break;
            case 2: // JOGO
                char win[20];//guarda o nome do vencedor de  cada nivel

                //guarda na var nivelAnt o nivel atual antes de ler informaçoes do jogoUI
                nivelAnt = jogo.nivel;

                jogo = recebeJogo(ptd->fd);

                //atualiza posicao na estrutura cliente do jogador em questao
                pthread_mutex_lock(ptd->ptrinco);
                ptd->clients = updatePosCliente(ptd->clients, ptd->nClients, jogo);
                pthread_mutex_unlock(ptd->ptrinco);

                // atualiza jogo
                f_jogo.j = jogo;
                f_jogo.pid = pid;

                //verifica se ja chegou ao final do nivel do mapa
                if (f_jogo.j.clientData.pos == 14) {
                    //procura o nome do vencedor e coloca na var win
                    for(int i = 0; i < ptd->nClients; i++){
                        if(ptd->clients[i].pos == 14)
                            strcpy(win, ptd->clients[i].nome);
                    }
                    //envia mensagem a todos os jogadores quem venceu o nivel
                    sprintf(mensagem, "%s venceu o nivel %d",win, f_jogo.j.nivel);
                    notificaClients(ptd->clients, ptd->nClients, mensagem);
                    printf("\n%s", mensagem);
                    if (f_jogo.j.nivel == 3) {
                        printf("\nNiveis Terminados\n");
                        notificaClients(ptd->clients, ptd->nClients, "Todos os niveis foram completados");
                        for (int i = 0; i < ptd->nClients; ++i) {
                            sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                            enviaFlagFifo(fifoCli, -2); // <- flag de termino dos niveis
                        }
                        printf("\nO jogo vai encerrar em");
                        notificaClients(ptd->clients, ptd->nClients, "O jogo vai encerrar em");
                        while (cont > 0) {
                            printf("\t%d", cont);
                            sprintf(n, "%d", cont);
                            notificaClients(ptd->clients, ptd->nClients, n);
                            cont--;
                            sleep(1);
                        }
                        END(ptd);
                    } else {
                        printf("\nFim do nivel %d\n", nivelAnt);
                        //apaga avatares nas posicoes em que estavam
                        for (int i = 0; i < ptd->nClients; i++) {
                            f_jogo.j.labirinto.mapa[ptd->clients[i].pos] = ' ';
                        }

                        //muda nivel
                        f_jogo.j.nivel++;
                        printf("\nInicio do nivel %d\n", f_jogo.j.nivel);
                        sprintf(mensagem, "Inicio do nivel %d", f_jogo.j.nivel);
                        notificaClients(ptd->clients, ptd->nClients, mensagem);

                        //zera posicoes ocupadas dos jogadores
                        for (int i = 0; i < ptd->numPosOcupadas; i++) {
                            pthread_mutex_unlock(ptd->ptrinco);
                            ptd->posOcupadas[i] = 0;
                            pthread_mutex_lock(ptd->ptrinco);
                        }

                        pthread_mutex_unlock(ptd->ptrinco);
                        ptd->numPosOcupadas = 0;
                        pthread_mutex_lock(ptd->ptrinco);

                        for (int i = 0; i < ptd->nClients; i++) {
                            pthread_mutex_unlock(ptd->ptrinco);
                            ptd->clients[i].pos = geraPosicao(ptd->posOcupadas, &ptd->numPosOcupadas);
                            pthread_mutex_unlock(ptd->ptrinco);

                            strcpy(f_jogo.j.clientData.nome, ptd->clients[i].nome);
                            f_jogo.j.clientData.pid = ptd->clients[i].pid;
                            f_jogo.j.clientData.pos = ptd->clients[i].pos;

                            sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                            enviaF_JOGO(fifoCli, &f_jogo);
                        }

                        f_jogo = mudaMapa(f_jogo);

                        //atualiza o mapa e envia para ambos os jogadores
                        for (int i = 0; i < ptd->nClients; i++) {
                            if (ptd->clients[i].tipo == 1) {
                                f_jogo.j.labirinto.mapa[ptd->clients[i].pos] = tolower(ptd->clients[i].nome[0]);
                            }
                        }

                        //printf("%s", f_jogo.j.labirinto.mapa);
                        for (int i = 0; i < ptd->nClients; i++) {
                            sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                            enviaF_JOGO(fifoCli, &f_jogo);
                        }

                        break;
                    }
                }

                //envia o mapa atualizado para todos os clientes
                for (int i = 0; i < ptd->nClients; ++i) {
                    sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                    enviaF_JOGO(fifoCli, &f_jogo);
                }
                break;
            case 3: // MSG
                printf("\n(Pedido de '%s' para enviar mensagem)\n", fifoCli);

                msg = recebeMSG(ptd->fd);

                f_msgResult.flag = 3;
                f_msgResult.msg_result.msg = msg;
                f_msgResult.msg_result.pid = encontraPidClient(ptd->clients, ptd->nClients, msg.to);

                enviaF_MSG_RESULT(fifoCli, &f_msgResult);
                break;
            case 4: // TABELA (pedido)
                printf("\n(Pedido de '%s' para tabela de jogadores)\n", fifoCli);

                f_tabela.flag = 4;
                preencheTabela(ptd->clients, ptd->nClients, f_tabela.t.tab);

                enviaF_TABELA(fifoCli, &f_tabela);

                break;
            case 5: // EXIT (pedido)
                printf("\n(Pedido de '%s' para sair)\n", fifoCli);

                encontraNomeClient(ptd->clients, ptd->nClients, pid, nome);

                pthread_mutex_lock(ptd->ptrinco);
                ptd->clients = dellClient(ptd->clients, &ptd->nClients, pid);
                pthread_mutex_unlock(ptd->ptrinco);

                enviaFlagFifo(fifoCli, 7);

                if (countPlayers(ptd->clients, ptd->nClients) == 0) {
                    printf("\nJogo terminou por falta de jogadores.");
                    END(ptd);
                }

                atualiza_jplayers(ptd->clients, ptd->nClients);

                sprintf(mensagem, "%s saiu do jogo", nome);
                notificaClients(ptd->clients, ptd->nClients, mensagem);

                break;
            case 6: // KICK_REQUEST
                position = recebeKickRequest(ptd->fd);

                f_jogo.pid = pid;
                f_jogo.j.labirinto.mapa[position] = ' ';

                for (int i = 0; i < ptd->nClients; i++) {
                    sprintf(fifoCli, "cli%d", ptd->clients[i].pid);
                    enviaF_JOGO(fifoCli, &f_jogo);
                }

                break;
        }
    } while (ptd->continua != 0);
    pthread_exit(NULL);
}

void enviaFlagFifo(const char *fifoCli, int flag) {
    int fd, nbytes;

    fd = open(fifoCli, O_WRONLY);
    if (fd == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        nbytes = write(fd, &flag, sizeof(int));
        if (nbytes == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            //printf("\nEnviei flag: %d (%d bytes)", flag, nbytes);
        }
        close(fd);
    }
}

void enviaFlagFd(int fd, int flag) {
    int nbytes;

    nbytes = write(fd, &flag, sizeof(int));
    if (nbytes == -1) {
        printf("\\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf( "\nEnviei flag: '%d' (%d bytes)", flag, nbytes);
    }
}

void enviaPid(int fd, int pid) {
    int nbytes;

    nbytes = write(fd, &pid, sizeof(int));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf( "\nEnviei pid: '%d' (%d bytes)", pid, nbytes);
    }
}

void enviaF_KICK_REQUEST(int fd, int position) {
    int nbytes;
    F_KICK_REQUEST f_kickRequest;

    f_kickRequest.flag = 6;
    f_kickRequest.kickRequest.position = position;

    nbytes = write(fd, &f_kickRequest, sizeof(F_KICK_REQUEST));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nEnviei F_KICK_REQUEST position: '%d' (%d bytes)\n", position, nbytes);
    }
}

void enviaF_JOGO(const char *fifoCli, const F_JOGO *f_jogo) {
    int fd, nbytes;

    fd = open(fifoCli, O_WRONLY);
    if (fd == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        nbytes = write(fd, f_jogo, sizeof(F_JOGO));
        if (nbytes == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            //printf("\nEnviei F_JOGO \n%s\n(%d bytes)\n", f_jogo->j.labirinto.mapa, nbytes);
        }
        close(fd);
    }
}

void enviaF_MSG_RESULT(const char *fifoCli, const F_MSG_RESULT *f_msgResult) {
    int fd, nbytes;

    fd = open(fifoCli, O_WRONLY);
    if (fd == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        nbytes = write(fd, f_msgResult, sizeof(F_MSG_RESULT));
        if (nbytes == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            //printf("\nEnviei MSG_RESULT (%d bytes)\n", nbytes);
        }
        close(fd);
    }
}

void enviaF_TABELA(const char *fifoCli, const F_TABELA *f_tabela) {
    int fd, nbytes;

    fd = open(fifoCli, O_WRONLY);
    if (fd == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        nbytes = write(fd, f_tabela, sizeof(F_TABELA));
        if (nbytes == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            //printf("\nEnviei F_TABELA (%d bytes)\n", nbytes);
        }
        close(fd);
    }
}

int recebeFlag(int fd) {
    int nbytes, flag;

    nbytes = read(fd, &flag, sizeof(int));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nRecebi flag: '%d'(%d bytes)", flag, nbytes);
    }

    return flag;
}

int recebePid(int fd) {
    int nbytes, pid;

    nbytes = read(fd, &pid, sizeof(int));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nRecebi pid: '%d'(%d bytes)", pid, nbytes);
    }

    return pid;
}


MSG recebeMSG(int fd) {
    int nbytes;
    MSG msg;

    nbytes = read(fd, &msg, sizeof(MSG));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nRecebi MSG (%d bytes)\n", nbytes);
        //printf("\n| from -> '%s' | to -> '%s' |\n| content -> '%s'", msg.from, msg.to, msg.content);
    }

    return msg;
}

LOGIN recebeLogin(int fd) {
    int nbytes;
    LOGIN login;

    nbytes = read(fd, &login, sizeof(LOGIN));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nRecebi LOGIN: pid -> '%d'| nome -> '%s' (%d bytes)", login.pid, login.nome, nbytes);
    }

    return login;
}

JOGO recebeJogo(int fd) {
    int nbytes;
    JOGO jogo;

    nbytes = read(fd, &jogo, sizeof(JOGO));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nRecebi JOGO:\n%s\n (%d bytes)", jogo.labirinto.mapa, nbytes);
    }

    return jogo;
}

int recebeKickRequest(int fd) {
    int nbytes;
    KICK_REQUEST kickRequest;

    nbytes = read(fd, &kickRequest, sizeof(KICK_REQUEST));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //printf("\nRecebi KICK_REQUEST: '%d' (%d bytes)", kickRequest.position, nbytes);
    }

    return kickRequest.position;
}

//*************************************************** THREAD TIMEINSCRICAO *********************************************

void *coordenadasBot(void *data) {
    TDATA *ptd = (TDATA*) data;

    int coord[TAM];
    int pos;

    while (ptd->continuaBots) {

        if (getFromBot(coord, TAM, ptd->interval, "25") == -1) {
            printf("\nBot falhou");
        } else {
            //printf("\nRECEBI apos %d segundos: ",ptd->interval);
            for (int i = 0; i < 3; ++i) {
                printf(" %d", coord[i]);
            }
            //converte as coordenadas obtidas pelo bot para o valor da posicao correspondente
            pos = coord[1] * N_COL + coord[0];
            //printf("\nPOSICAO correspondente %d\n", pos);
        }

    }

    pthread_exit(NULL);
}

void *timeInscricao(void *data) {
    TDATA *ptd = (TDATA*) data;
    int tempo = atoi(getenv("INSCRICAO"));
    int min = atoi(getenv("NPLAYERS"));

    printf("\nOs jogadores têm %d segundos para se registarem", tempo);

    pthread_mutex_lock(ptd->ptrinco);
    ptd->inscreve = 1;
    pthread_mutex_unlock(ptd->ptrinco);

    while (tempo > 0) {
        //decrementa o tempo quando chegar a 0 termina e começa jogo
        tempo--;
        printf("\n\t\t    %d", tempo);

        if (ptd->begin == 1) {
            pthread_mutex_lock(ptd->ptrinco);
            ptd->inscreve = 0;
            pthread_mutex_unlock(ptd->ptrinco);
            if (ptd->nClients > 1) {
                printf("\nJogo começou adiantadamente, registados %d jogadores. A iniciar jogo...\n", ptd->nClients);
            } else {
                printf("\nJogo começou adiantadamente, registado apenas %d jogador. A iniciar jogo...\n", ptd->nClients);
            }
            printf("\nInicio do jogo");
            pthread_exit(NULL);
        }
        sleep(1);
    }

    pthread_mutex_lock(ptd->ptrinco);
    ptd->inscreve = 0;
    ptd->begin = 1;
    pthread_mutex_unlock(ptd->ptrinco);

    if (ptd->nClients < min) {
        printf("\nTempo esgotado, o número mínimo de jogadores não foi atingido. A encerrar...\n");

        pthread_mutex_lock(ptd->ptrinco);
        ptd->continua = 0;
        pthread_mutex_unlock(ptd->ptrinco);

        enviaFlagFd(ptd->fd, -1);

        enviaFlagFd(ptd->fd_main, -1);
    } else {
        printf("\nTempo esgotado, registados %d jogadores. A iniciar jogo...\n", ptd->nClients);

        enviaFlagFd(ptd->fd, 0);
    }
    pthread_exit(NULL);
}



//*************************************************** OUTRAS ***********************************************************

void init_var_ambiente(){
    // CASO EXISTA ESTA VARIAVEL SUBSTITUA A EXISTENTE POR ESTA
    setenv("INSCRICAO", "15", 1);   //criação variavel ambiente INSCRICAO valor em segundos
    setenv("NPLAYERS","3", 1);      //NPLAYERS minimo para jogar o jogo
    setenv("DURACAO","40", 1);      //DURACAO de cada nivel
    setenv("DECREMENTO","10", 1);   //DECREMENTO de tempo de cada nivel
}

void preencheTabela(CLIENT *clients, int nClients, char tab[500]) {
    char line[100];

    if (nClients == 0) {return;}

    strcpy(tab, "\nPID\tNome\tTipo\t\tCoord");

    for (int i = 0; i < nClients; ++i) {
        strcpy(line, "");
        if (clients[i].tipo == 1) {
            sprintf(line, "\n%d\t%s\tPlayer\t\t(%d)", clients[i].pid, clients[i].nome, clients[i].pos);
        } else if (clients[i].tipo == 2) {
            sprintf(line, "\n%d\t%s\tSpectator\t(%d)", clients[i].pid, clients[i].nome, clients[i].pos);
        }
        strcat(tab, line);
    }
    strcat(tab, "\0");
}

void atualiza_jplayers(CLIENT *clients, int nClients) {
    int fd, nbytes;
    char line[100], aux[100], fifoCli[50], frase[100], nome[50];
    F_ATUALIZA_JPLAYERS f_atualiza_jplayers;

    if (nClients == 0) {return;}

    f_atualiza_jplayers.flag = 5;

    // Constroi a string
    for (int i = 0; i < nClients; ++i) {
        strcpy(f_atualiza_jplayers.p.str, "       PLAYERS\n");
        for (int j = 0; j < nClients; ++j) {
            if (clients[j].tipo == 1) {
                strcpy(line, "");
                if (clients[i].pid == clients[j].pid) {
                    sprintf(line, "   * %s [%d]\n", clients[j].nome, clients[j].pid);
                } else {
                    sprintf(line, "     %s [%d]\n", clients[j].nome, clients[j].pid);
                }
                strcat(f_atualiza_jplayers.p.str, line);
            }
        }
        strcat(f_atualiza_jplayers.p.str, "\0");

        sprintf(fifoCli, "cli%d", clients[i].pid);
        fd = open(fifoCli, O_WRONLY);
        if (fd == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            nbytes = write(fd, &f_atualiza_jplayers, sizeof(F_ATUALIZA_JPLAYERS));
            if (nbytes == -1) {
                printf("\n<ERRO> '%s'", strerror(errno));
            } else {
                //printf("\nEnviei: f_atualiza_jplayers (%d bytes)", nbytes);
            }
            close(fd);
        }
    }
}

void notificaClients(CLIENT *clients, int nClients, char *mensagem) {
    int fd, nbytes;
    char fifoCli[50];

    F_NOTIFICACAO f_notificacao;

    f_notificacao.flag = 6;

    strcpy(f_notificacao.n.str, mensagem);

    for (int i = 0; i < nClients; ++i) {
        sprintf(fifoCli, "cli%d", clients[i].pid);
        fd = open(fifoCli, O_WRONLY);
        if (fd == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        } else {
            nbytes = write(fd, &f_notificacao, sizeof(F_NOTIFICACAO));
            if (nbytes == -1) {
                printf("\n<ERRO> '%s'", strerror(errno));
            }
            close(fd);
        }
    }
}

// <- recebe um vetor de 3 inteiros vazio que será preenchido com a informação vinda do bot devolve -1 se algo correr mal
int getFromBot(int coord[], int tam, int interval, char *duration) {
    int i = 0;

    int id, status;         // <- fork

    int canal[2], nbytes;   // <- pipe (canal[0] -> read, canal[1] -> write)
    char str[50];           // <- mensagem a receber do pipe

    int result;             // <- sinal

    if (pipe(canal) == -1) {return -1;} // <- se o pipe falhar retorna -1

    id = fork();

    if (id == -1) {return -1;} // <- se o fork falhar retorna -1

    if (id == 0) {
        // Redirecionamento de Input
        close(canal[0]);    // <- close read end of canal
        close(1);           // <- close stdout
        dup(canal[1]);      // <- write end of canal passa para o lugar de stdout
        close(canal[1]);    // <- close write end of canal

        execl(BOT, BOT, "1", duration, NULL);
        perror("\n<ERRO> EXECL");
    }

    sleep(interval); // <- sleep para dar tempo ao bot de gerar as coordenadas

    result = kill(id, SIGKILL);

    if (result == -1) {return -1;} // <- se o sinal falhar retorna -1

    wait(&result);

    close(canal[1]); // <- close write end of canal

    do {
        nbytes = read(canal[0], str, sizeof(str)-1);
        if (nbytes > 0) {
            str[nbytes-1] = '\0'; // <- substituir o '\n' que o bot envia no final da frase porque não é necessário
        }
    } while(nbytes > 0);

    close(canal[0]); // <- close read end of canal

    coord[i] = atoi(strtok(str, " "));
    for (i = 1; i < tam; ++i) {
        coord[i] = atoi(strtok(NULL, " "));
    }

    return 1;
}

F_JOGO preencheF_JOGO(const TDATA td) {
    F_JOGO f_j;

    f_j.flag = 1;
    f_j.pid = getpid();
    f_j.j.nivel = td.nivel;

    if (td.inscreve == 1) {
        f_j.j.clientData.tipo = 1;
    } else {
        f_j.j.clientData.tipo = 2;
    }

    switch (f_j.j.nivel) {
        case 1:
            mapaNivel1(f_j.j.labirinto.mapa);
            break;
        case 2:
            mapaNivel2(f_j.j.labirinto.mapa);
            break;
        case 3:
            mapaNivel3(f_j.j.labirinto.mapa);
            break;
        default:
            break;
    }
    f_j.j.labirinto.nBlocks = 0;
    f_j.j.labirinto.nPedras = 0;

    return f_j;
}

int geraPosicao(int *posOcupadas, int *numPosOcupadas) {
    int positionInit[] = {605, 612, 619, 625, 632}; // possíveis posições iniciais dos jogadores
    int indiceAleatorio; // posição aleatória para buscar o valor no array positionInit
    int pos;

    do {
        indiceAleatorio = rand() % 5;  // 5 é o número de elementos no array
        pos = positionInit[indiceAleatorio];
    } while (posEstaOcupada(pos, posOcupadas, *numPosOcupadas));

    // Atualiza o array de posições ocupadas com a nova posição
    posOcupadas[*numPosOcupadas] = pos;
    (*numPosOcupadas)++;

    return pos;
}

// Verifica se a posição 'pos' está presente no array 'posOcupadas'
int posEstaOcupada(int pos, int *posOcupadas, int numPosOcupadas) {
    for (int i = 0; i < numPosOcupadas; i++) {
        if (pos == posOcupadas[i]) {
            return 1; // A posição está ocupada
        }
    }
    return 0; // A posição não está ocupada
}

int getPosicao(const TDATA *td, int pid) {
    for (int i = 0; i < td->nClients; ++i) {
        if (td->clients[i].pid == pid) {
            return td->clients[i].pos;
        }
    }
    return -1;
}

int verificaClientTipo(const TDATA *td, int pid) {
    for (int i = 0; i < td->nClients; ++i) {
        if (td->clients[i].pid == pid) {
            return td->clients[i].tipo == 1 ? 1 : 0;
        }
    }
    return -1;
}