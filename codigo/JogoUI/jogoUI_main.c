#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "jogoUI.h"
#include "../Util/utils.h"
#include "../Util/structs.h"

int main(int argc, char* argv[]) {
    // Constantes
    const int size_J = 3;
    const char* arrayCmd_J[] = {"players", "msg", "exit"};
    const int nArgCmd_J[] = {0, 2, 0};

    // Variáveis relacionadas com comunicação com o motor
    int fd, fd_resposta, nbytes, tipo;
    char playerName[50];
    char fifoCli[50];

    // Estruturas que envia
    F_LOGIN f_login;
    F_JOGO f_jogo;

    // Estruturas que recebe
    int flagRecebe;
    JOGO jogo;

    // Variáveis relacionadas com o mecanismo SELECT
    struct timeval t;
    fd_set fds;
    int resSelect;

    // Vriáveis relacionadas com as janelas ncurses
    WINDOW * jComandos;
    WINDOW * jInfo; //janela de informaçoes de nivel, bloqueios e pedras
    WINDOW * jMapa;
    WINDOW * jplayers;
    WINDOW * jOutput;
    WINDOW * jTipo;
    const char * p;
    int ch;

    // Outras Variáveis
    int index, res, sair = 0, begin = 0, end = 0;
    char cmd[50];
    
    int position = 0; // posicao do jogador
    int nivelAnt = 0; // nivel anterior serve para verificar se o nivel foi alterado pelo motor ou nao

    setbuf(stdout, NULL);

    if (argc < 2 || argc > 2) {
        printf("N.º de argumentos inválido.\n");
        exit(1);
    }

    //verifica se existe algum motor a correr
    //caso nao exista nao deixa abrir o jogo

    if (access(FIFO_MOTOR, F_OK) != 0) {
        printf("Sessão negada! Motor ainda não está aberto.\n");
        exit(1);
    }

    strcpy(playerName, argv[1]);

    f_login = preencheLogin(playerName);

    sprintf(fifoCli, "cli%d", getpid());                // cria o nome do fifo com base no pid
    mkfifo(fifoCli, 0640);                              // cria fifo cliente

    fd = open(FIFO_MOTOR, O_WRONLY | O_CREAT, 0640);    //abre fifo do motor para escrita
    if (fd == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    }

    nbytes = write(fd, &f_login, sizeof(F_LOGIN));      //escreve no fifo do motor os dados da estrutura do pedido
    if (nbytes == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    }


    //RECEBE A RESPSOTA...
    fd_resposta = open(fifoCli, O_RDONLY);              //abre o fifo do cliente para leitura
    if (fd_resposta == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    } else {
        nbytes = read(fd_resposta, &flagRecebe, sizeof(int));
        if (nbytes == -1) {
            printf("\n<ERRO> '%s'", strerror(errno));
        }
    }

    if (flagRecebe == -1) {
        printf("\nJá existe um jogador com esse nome\n");
        close(fd_resposta);
        close(fd);
        unlink(fifoCli);
        exit(1);
    }

    //INICIALIZA NCURSES E AS VARIAS jComandosS
    initscr();      // Obrigatorio e sempre a primeira operação de ncurses
    start_color();  // Inicializa o motor de cores
    erase();        // Limpa o ecrã e posiciona o cursor no canto superir esquerdo

    // Define cor. formato: par (ID, cor do caracter, cor do fundo)
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    attron(COLOR_PAIR(1));   // associa uma cor ao ecrã para desenhar uma moldura

    desenhaMoldura(19, 40, 1, 1);      //
    jMapa = newwin(18, 40, 1, 1);   // cria a jComandos do mapa 18 lin x 40 col em lin 1, col 1
    jInfo = newwin(1, 40, 19, 1);

    desenhaMoldura(1, 40, 21, 1);
    jTipo = newwin(1, 40, 21, 1);

    desenhaMoldura(8, 21, 1, 45);       //
    jplayers = newwin(8, 21, 1, 45);    // cria jComandos de players 8 lin x 20 col em lin 1, col 45

    jComandos = newwin(10, 40, 23, 1);  // cria jComandos de comandos 10 lin x 40 col em lin 19, col 1

    jOutput = newwin(9, 60, 10, 44);    // cria jComandos de output 9 lin x 40 col em lin 10, col 45

    wattrset(jComandos, COLOR_PAIR(1)); // define foreground/backgorund dos caracteres
    wbkgd(jComandos, COLOR_PAIR(1));    // define backgound dos espaço vazio
    scrollok(jComandos, TRUE);          // liga scroll na jComandos
    keypad(jComandos, FALSE);           // para ligar as teclas de direção (aplicar à jComandos)
    werase(jComandos);                  // limpa a jComandos
    wrefresh(jComandos);                // wreferesh necessário para mostrar as alterações de conteúdo

    wattrset(jInfo, COLOR_PAIR(1)); // define foreground/backgorund dos caracteres
    wbkgd(jInfo, COLOR_PAIR(1));    // define backgound dos espaço vazio
    scrollok(jInfo, TRUE);          // liga scroll na jComandos
    keypad(jInfo, FALSE);           // para ligar as teclas de direção (aplicar à jComandos)
    werase(jInfo);                  // limpa a jComandos
    wrefresh(jInfo);                // wreferesh necessário para mostrar as alterações de conteúdo

    wattrset(jTipo, COLOR_PAIR(1)); // define foreground/backgorund dos caracteres
    wbkgd(jTipo, COLOR_PAIR(1));    // define backgound dos espaço vazio
    scrollok(jTipo, TRUE);          // liga scroll na jComandos
    keypad(jTipo, FALSE);           // para ligar as teclas de direção (aplicar à jComandos)
    werase(jTipo);                  // limpa a janela
    wrefresh(jTipo);                // wreferesh necessário para mostrar as alterações de conteúdo

    wattrset(jOutput, COLOR_PAIR(1));   // define foreground/backgorund dos caracteres
    wbkgd(jOutput, COLOR_PAIR(1));      // define backgound dos espaço vazio
    scrollok(jOutput, TRUE);            // liga scroll na jComandos
    keypad(jOutput, FALSE);             // para ligar as teclas de direção (aplicar à jComandos)
    keypad(jMapa, TRUE);                // para ligar as teclas de direção (aplicar à jComandos)
    werase(jOutput);                    // limpa a jComandos
    wrefresh(jOutput);                  // wreferesh necessário para mostrar as alterações de conteúdo

    wattrset(jplayers, COLOR_PAIR(1));  // define foreground/backgorund dos caracteres
    wbkgd(jplayers, COLOR_PAIR(1));     // define backgound dos espaço vazio
    scrollok(jplayers, TRUE);           // liga scroll na jComandos
    keypad(jplayers, FALSE);            // para ligar as teclas de direção (aplicar à jComandos)
    werase(jplayers);                   // limpa a jComandos
    wrefresh(jplayers);                 // wreferesh necessário para mostrar as alterações de conteúdo
    wrefresh(jplayers);

    wattrset(jMapa, COLOR_PAIR(1));     // define foreground/backgorund dos caracteres
    wbkgd(jMapa, COLOR_PAIR(1));        // define backgound dos espaço vazio
    scrollok(jMapa, TRUE);              // liga scroll na jComandos
    keypad(jMapa, TRUE);                // para ligar as teclas de direção (aplicar à jComandos)
    werase(jMapa);                      // limpa a jComandos
    wrefresh(jMapa);                    // wreferesh necessário para mostrar as alterações de conteúdo

    //wprintw(jOutput, "\nRecebi: flagRecebe: '%d' (%d bytes)", flagRecebe, nbytes);
    //wrefresh(jOutput);

    recebeLixo(fd_resposta, jOutput);

    jogo = recebeJogo(fd_resposta, jOutput, jInfo, jMapa, &position, &nivelAnt);

    tipo = recebeFlag(fd_resposta, jOutput);
    if (tipo == 1) {
        wprintw(jTipo, "\t     %s - Player", playerName);
    } else {
        wprintw(jTipo, "\t     %s - Spectator", playerName);
    }
    wrefresh(jTipo);

    flagRecebe = recebeFlag(fd_resposta, jOutput);

    recebeAtualizaJplayers(fd_resposta, jplayers, jOutput);

    close(fd_resposta);                                 //fecha a leitura do fifo

    noecho();       // Desliga o echo porque a seguir vai ler telcas de direção
                    // não se pretende "ecoar" essa tecla no ecrã
    cbreak();       // desliga a necessidade de enter. cada tecla é lida imediatamente

    p = NULL;

    fd_resposta = open(fifoCli, O_RDWR);
    if (fd_resposta == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    }
    while (sair == 0) {

        // SELECT
        t.tv_sec = 20;
        t.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(0, &fds);            // stdin
        FD_SET(fd_resposta, &fds);  // FIFO
        resSelect = select(fd_resposta + 1, &fds, NULL, NULL, &t);

        // TECLADO (stdin -> ficheiro da posição 0 da tabela de input output)
        if (resSelect > 0 && FD_ISSET(0, &fds)) {
            ch = wgetch(jMapa);             // MUITO importante: o input é feito sobre a jComandos em questão

            if (ch != ' ') {
                if (tipo == 1 && begin == 1 && end == 0) {
                    jogo = handleMovimento(ch, jogo, &position, playerName);

                    //reescreve o mapa no ecra com posicao do jogador atualizada
                    werase(jMapa);
                    wprintw(jMapa, "%s\t", jogo.labirinto.mapa);
                    wrefresh(jMapa);

                    f_jogo = preencheF_JOGO(jogo, f_login, position);

                    //wprintw(jOutput, "\nEnviei pid %d", jogo.clientData.pid);
                    //wrefresh(jOutput);

                    enviaF_Jogo(fd, f_jogo, jOutput);

                    if (p != NULL) {
                        wprintw(jOutput, "%s %d\n", p, position);
                        p = NULL;
                        wrefresh(jOutput); //refresh na janela de outputs
                        wrefresh(jMapa);  //refresh na janela do Mapa
                    }
                }
            } else {
                echo();          // Volta a ligar o echo para se ver o que se está
                nocbreak();      // a escrever. Idem input baseado em linha + enter

                do {
                    wprintw(jComandos, "Comando -> ");
                } while (wscanw(jComandos, " %99[^\n]", cmd) == -1);

                res = validaComando(cmd, size_J, arrayCmd_J, nArgCmd_J, &index);

                switch (res) {
                    case 0:
                        executaComando_J(cmd, playerName, index, jOutput, fd);
                        break;
                    case 1:
                        wprintw(jOutput, "\nComando inválido");
                        break;
                    case 2:
                        wprintw(jOutput, "\nFalta de argumentos");
                        break;
                    case 3:
                        wprintw(jOutput, "\nExcesso de argumentos");
                        break;
                    default:
                        break;
                }
                wrefresh(jOutput);
                wrefresh(jMapa);
                noecho();        // Volta a desligar o echo das teclas premidas
                cbreak();        // e a lógica de input em linha+enter no fim
            }
        }

        // FIFO
        if (resSelect > 0 && FD_ISSET(fd_resposta, &fds)) {
            flagRecebe = recebeFlag(fd_resposta, jOutput);

            if (flagRecebe == 1) {
                recebeLixo(fd_resposta, jOutput);
            }

            switch (flagRecebe) {
                case -2: // FIM DO JOGO
                    end = 1;
                    break;
                case 0: // BEGIN
                    begin = 1;
                    break;
                case 1: // JOGO
                    jogo = recebeJogo(fd_resposta, jOutput, jInfo, jMapa, &position, &nivelAnt);
                    break;
                case 2: // MSG
                    recebeMSG(fd_resposta, jOutput);
                    break;
                case 3: // MSG_RESULT
                    recebeMsgResult(fd_resposta, jOutput);
                    break;
                case 4: // TABELA (reposta)
                    recebeTabela(fd_resposta, jOutput);
                    break;
                case 5: // ATUALIZA_JPLAYERS
                    recebeAtualizaJplayers(fd_resposta, jplayers, jOutput);
                    break;
                case 6: // NOTIFICACAO
                    recebeNotificacao(fd_resposta, jOutput);
                    break;
                case 7: // EXIT (resposta)
                    sair = 1;
                    break;
            }
            noecho();
            cbreak();
        }
    }
    close(fd_resposta);

    close(fd);

    endwin();

    unlink(fifoCli);

    exit(0);
}
