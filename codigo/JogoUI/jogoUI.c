#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "jogoUI.h"
#include "../Util/structs.h"
#include "../Util/utils.h"

//*************************************************** COMANDOS *********************************************************

void executaComando_J(char *cmd, char *playerName, int index, WINDOW *jOutput, int fd) {
    MSG_REQUEST msgRequest;

    switch (index) {
        case 0: // players
            enviaPedidoPlayers(fd, jOutput);
            break;
        case 1: // msg
            msgRequest.flag = 3;
            msgRequest.pid = getpid();
            msgRequest.msg = preencheMsg(cmd, playerName);

            enviaMSG_Request(fd, msgRequest, jOutput);
            break;
        case 2: // exit
            enviaPedidoExit(fd, jOutput);
            break;
    }
}

//*************************************************** MOVIMENTACAO *****************************************************

int verificaMovimento(int position, LABIRINTO labirinto, int ch) {
    switch (ch) {
        case KEY_UP:
            if (position - 40 < 0) { return 0; }

            if (labirinto.mapa[position - 40] == 'x') { return 0; }

            if (labirinto.mapa[position - 40] == 'P') { return 0; }

            if (labirinto.mapa[position - 40] == 'B') { return 0; }

            return 1;

        case KEY_DOWN:
            if (position + 40 >= MAP_SIZE) { return 0; }

            if (labirinto.mapa[position + 40] == 'x') { return 0; }

            if (labirinto.mapa[position + 40] == 'P') { return 0; }

            if (labirinto.mapa[position + 40] == 'B') { return 0; }

            return 1;

        case KEY_LEFT:
            if (position <= 0) { return 0; } else if (labirinto.mapa[position - 1] == 'x') { return 0; } else if (
                labirinto.mapa[position - 1] == 'P') { return 0; } else if (labirinto.mapa[position - 1] == 'B') {
                return 0;
            } else { return 1; }
            break;
        case KEY_RIGHT:
            if (position >= MAP_SIZE - 1) { return 0; } else if (
                labirinto.mapa[position + 1] == 'x') { return 0; } else if (labirinto.mapa[position + 1] == 'P') {
                return 0;
            } else if (labirinto.mapa[position + 1] == 'B') {
                return 0;
            } else { return 1; }
            break;
        default:
            return 0;
            break;
    }
}

JOGO handleMovimento(int ch, JOGO jogo, int *position, char *playerName) {
    JOGO newJogo = jogo;

    switch (ch) {
        case KEY_UP:
            if (verificaMovimento(*position, newJogo.labirinto, ch) == 1) {
                newJogo.labirinto.mapa[*position] = ' '; // Limpar a posição atual
                (*position) -= 40;
                newJogo.labirinto.mapa[*position] = tolower(playerName[0]);
            }
            break;
        case KEY_DOWN:
            if (verificaMovimento(*position, newJogo.labirinto, ch) == 1) {
                newJogo.labirinto.mapa[*position] = ' '; // Limpar a posição atual
                (*position) += 40;
                newJogo.labirinto.mapa[*position] = tolower(playerName[0]);
            }
            break;
        case KEY_LEFT:
            if (verificaMovimento(*position, newJogo.labirinto, ch) == 1) {
                newJogo.labirinto.mapa[*position] = ' '; // Limpar a posição atual
                (*position)--;
                newJogo.labirinto.mapa[*position] = tolower(playerName[0]);
            }
            break;
        case KEY_RIGHT:
            if (verificaMovimento(*position, newJogo.labirinto, ch) == 1) {
                newJogo.labirinto.mapa[*position] = ' '; // Limpar a posição atual
                (*position)++;
                newJogo.labirinto.mapa[*position] = tolower(playerName[0]);
            }
            break;
    }
    return newJogo;
}

//*************************************************** PREENCHIMENTO DE ESTRUTURAS **************************************

MSG preencheMsg(char *cmd, char *playerName) {
    MSG msg;
    int i = 0, j = 0, k = 0;

    strcpy(msg.from, playerName);

    while (!isspace(cmd[i])) { i++; }

    i++;

    while (!isspace(cmd[i])) {
        msg.to[j] = cmd[i];
        i++;
        j++;
    }
    msg.to[j] = '\0';

    i++;

    while (cmd[i] != '\0') {
        msg.content[k] = cmd[i];
        i++;
        k++;
    }
    msg.content[k] = '\0';

    return msg;
}

F_LOGIN preencheLogin(const char *nome) {
    F_LOGIN f_l;

    f_l.pid = getpid();
    f_l.flag = 1;
    f_l.l.pid = getpid();
    strcpy(f_l.l.nome, nome);

    return f_l;
}

F_JOGO preencheF_JOGO(JOGO jogo, F_LOGIN f_login, int position) {
    F_JOGO f_j;

    jogo.clientData.pid = f_login.pid;
    jogo.clientData.pos = position;
    f_j.flag = 2;
    f_j.pid = getpid();
    f_j.j = jogo;

    return f_j;
}

//*************************************************** ENVIAR INFORMACAO ************************************************

void enviaFlag(const int fd, int flag, WINDOW *jOutput) {
    int nbytes;

    nbytes = write(fd, &flag, sizeof(int));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nEnviei flag: '%d' (%d bytes)", flag, nbytes);
        //wrefresh(jOutput);
    }
}

void enviaPid(const int fd, int pid, WINDOW *jOutput) {
    int nbytes;

    nbytes = write(fd, &pid, sizeof(int));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nEnviei pid: '%d' (%d bytes)", pid, nbytes);
        //wrefresh(jOutput);
    }
}

void enviaPedidoExit(const int fd, WINDOW *jOutput) {
    int nbytes;
    PEDIDO_EXIT pedidoExit;

    pedidoExit.pid = getpid();
    pedidoExit.flag = 5;

    nbytes = write(fd, &pedidoExit, sizeof(PEDIDO_EXIT));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nEnviei pedidoExit (%d bytes)", nbytes);
        //wrefresh(jOutput);
    }
}

void enviaPedidoPlayers(const int fd, WINDOW *jOutput) {
    int nbytes;
    PEDIDO_PLAYERS pedidoPlayers;

    pedidoPlayers.pid = getpid();
    pedidoPlayers.flag = 4;

    nbytes = write(fd, &pedidoPlayers, sizeof(PEDIDO_PLAYERS));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nEnviei pedidoPlayers (%d bytes)", nbytes);
        //wrefresh(jOutput);
    }
}

void enviaF_Jogo(const int fd, F_JOGO f_jogo, WINDOW *jOutput) {
    int nbytes;

    nbytes = write(fd, &f_jogo, sizeof(F_JOGO));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nEnviei F_JOGO (%d bytes)", nbytes);
        //wrefresh(jOutput);
    }
}

void enviaMSG_Request(const int fd, MSG_REQUEST msgRequest, WINDOW *jOutput) {
    int nbytes;

    nbytes = write(fd, &msgRequest, sizeof(MSG_REQUEST));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput,"\nEnviei MSG_REQUEST (%d bytes)", nbytes);
        //wrefresh(jOutput);
    }
}

void enviaF_MSG(const char *fifoCli, F_MSG f_msg, WINDOW *jOutput) {
    int fd, nbytes;

    fd = open(fifoCli, O_WRONLY);
    if (fd == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        nbytes = write(fd, &f_msg, sizeof(F_MSG));
        if (nbytes == -1) {
            wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
            wrefresh(jOutput);
        } else {
            //wprintw(jOutput,"\nEnviei F_MSG (%d bytes)", nbytes);
            //wrefresh(jOutput);
        }
        close(fd);
    }
}

//*************************************************** RECEBER INFORMACAO ***********************************************

int recebeFlag(const int fd, WINDOW *jOutput) {
    int nbytes, flag;
    nbytes = read(fd, &flag, sizeof(int));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nRecebi flag: '%d'(%d bytes)", flag, nbytes);
        //wrefresh(jOutput);
    }
    return flag;
}

void recebeLixo(const int fd, WINDOW *jOutput) {
    int nbytes, lixo;

    nbytes = read(fd, &lixo, sizeof(int));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        //wprintw(jOutput, "\nRecebi lixo: '%d'(%d bytes)", lixo, nbytes);
        //wrefresh(jOutput);
    }
}

JOGO recebeJogo(const int fd, WINDOW *jOutput, WINDOW *jInfo, WINDOW *jMapa, int *pos, int *nivelAnt) {
    JOGO jogo;
    int nbytes;

    nbytes = read(fd, &jogo, sizeof(JOGO));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        if (*nivelAnt != jogo.nivel) {
            //significa que mudou de nivel
            //atualiza nova posicao
            *pos = jogo.clientData.pos;
            *nivelAnt = jogo.nivel;
        }
        //wprintw(jOutput,"\nPOSICAO %d\n", *pos);
        //wrefresh(jOutput);
        //wprintw(jOutput, "\nRecebi: JOGO (%d bytes)\n", nbytes);
        //wrefresh(jOutput);
        wprintw(jInfo, "\nNivel: %d     Bloqueios: %d     Pedras: %d", jogo.nivel, jogo.labirinto.nBlocks,
                jogo.labirinto.nPedras);
        wrefresh(jInfo);
    }

    werase(jMapa);
    wrefresh(jMapa);
    wprintw(jMapa, "%s\t", jogo.labirinto.mapa);
    wrefresh(jMapa);

    return jogo;
}

void recebeMSG(const int fd, WINDOW *jOutput) {
    MSG msg;
    int nbytes;

    nbytes = read(fd, &msg, sizeof(MSG));
    if (nbytes == -1) {
        wprintw(jOutput, "\n<ERRO> '%s'", strerror(errno));
        wrefresh(jOutput);
    } else {
        wprintw(jOutput, "\n[%s]: '%s'", toupperString(msg.from), msg.content);
        wrefresh(jOutput);
    }
}

void recebeAtualizaJplayers(const int fd, WINDOW *jplayers, WINDOW *jOutput) {
    int nbytes;
    ATUALIZA_JPLAYERS atualizaJplayers;

    nbytes = read(fd, &atualizaJplayers, sizeof(ATUALIZA_JPLAYERS));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //wprintw(jOutput, "\nRecebi: atualizaJplayers (%d bytes)", nbytes);
        //wrefresh(jOutput);
        werase(jplayers);
        wprintw(jplayers, "%s", atualizaJplayers.str);
        wrefresh(jplayers);
    }
}

void recebeMsgResult(const int fd, WINDOW *jOutput) {
    MSG_RESULT msgResult;
    F_MSG f_msg;
    int nbytes, fd_aux;
    char fifoCli[20];

    nbytes = read(fd, &msgResult, sizeof(MSG_RESULT));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        //wprintw(jOutput, "\nRecebi MSG_RESULT: '%d' \n(%d bytes)\n", msgResult.pid, nbytes);
        //wprintw(jOutput, "\n| from -> '%s' | to -> '%s' |\n| content -> '%s'(%d bytes)\n", msgResult.msg.from, msgResult.msg.to, msgResult.msg.content, nbytes);
        //wrefresh(jOutput);
        if (msgResult.pid == 0) {
            wprintw(jOutput, "\nJogador nao existe\n");
            wrefresh(jOutput);
        } else {
            sprintf(fifoCli, "cli%d", msgResult.pid);

            f_msg.flag = 2;
            f_msg.msg = msgResult.msg;

            enviaF_MSG(fifoCli, f_msg, jOutput);
        }
    }
}

void recebeTabela(const int fd, WINDOW *jOutput) {
    int nbytes;
    TABELA tabela;

    nbytes = read(fd, &tabela, sizeof(TABELA));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        wprintw(jOutput, "%s\n", tabela.tab);
        wrefresh(jOutput);
    }
}

void recebeNotificacao(const int fd, WINDOW *jOutput) {
    int nbytes;
    NOTIFICACAO notificacao;

    nbytes = read(fd, &notificacao, sizeof(NOTIFICACAO));
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        wprintw(jOutput, "\n[MOTOR]: '%s'", notificacao.str);
        wrefresh(jOutput);
    }
}

//*************************************************** OUTRAS ***********************************************************

void desenhaMoldura(int alt, int comp, int linha, int coluna) {
    --linha;
    --coluna;
    alt += 2;
    comp += 2;
    // desenha barras verticais laterais
    for (int l = linha; l <= linha + alt - 1; ++l) {
        mvaddch(l, coluna, '|'); // MoVe para uma posição e ADDiciona um CHar lá
        mvaddch(l, coluna+comp-1, '|');
    }
    // desenha as barras horizontais
    for (int c = coluna; c <= coluna + comp - 1; ++c) {
        mvaddch(linha, c, '-');
        mvaddch(linha+alt-1, c, '-');
    }
    // desenha os cantos
    mvaddch(linha, coluna, '+');
    mvaddch(linha, coluna+comp-1, '+');
    mvaddch(linha+alt-1, coluna, '+');
    mvaddch(linha+alt-1, coluna+comp-1, '+');
    refresh();
}
