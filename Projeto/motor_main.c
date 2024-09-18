#include "motor.h"
#include "utils.h"
#include "structs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Variáveis Constantes
    const int size_M = 7;
    const char *arrayCmd_M[] = {"users", "kick", "bots", "bmov", "rbm", "begin", "end"};
    const int nArgCmd_M[] = {0, 1, 0, 0, 0, 0, 0};

    // Variáveis Referentes às Threads
    TDATA td;
    pthread_t thread[2];    // thread[0] -> comunicacaoCLiente; thread[1] -> timeInscricao;
    pthread_mutex_t trinco;

    // Variáveis relacionadas com o mecanismo SELECT
    struct timeval t;
    fd_set fds;
    int resSelect;

    // Outras
    int res, sair = 0, index, fd, fd_main, flag;
    char cmd[50];

    setbuf(stdout, NULL);

    if (argc > 1) {
        printf("Não são permitidos argumentos.\n");
        exit(1);
    }

    // Tenta aceder ao fifo do motor, se existir significa que já se encontra outro motor a correr
    if (access(FIFO_MOTOR, F_OK) == 0) {
        printf("Outro motor já se encontra em execução.\n");
        exit(1);
    }

    // Criar fifo da thread main
    mkfifo(FIFO_MAIN, 0666);
    fd_main = open(FIFO_MAIN, O_RDWR);
    if (fd_main == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    }

    // Criar fifo da thread comunicacaoCLiente
    mkfifo(FIFO_MOTOR, 0666);
    fd = open(FIFO_MOTOR, O_RDWR);
    if (fd == -1) {
        printf("\n<ERRO> '%s'", strerror(errno));
    }

    printf("\n--------------Entrou no ADMIN--------------\n");

    td = inicializaThreadData(fd, fd_main, &trinco);

    init_var_ambiente();

    /* Criação thread comunicacaoCliente */
    pthread_mutex_init(&trinco, NULL);
    if (pthread_create(&thread[0], NULL, comunicacaoCliente, (void *)&td) != 0) {
        printf("Erro na criação da thread comunicacaoCliente\n");
    }

    /* Criação thread timeInscricao */
    if (pthread_create(&thread[1], NULL, timeInscricao, (void *)&td) != 0) {
        printf("Erro na criação da thread timeInscricao\n");
    }

    while (1) {
        // SELECT
        t.tv_sec = 20;
        t.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(0, &fds);            // stdin
        FD_SET(fd_main, &fds);      // FIFO
        resSelect = select(fd_main + 1, &fds, NULL, NULL, &t);

        // TECLADO (stdin -> ficheiro da posição 0 da tabela de input output)
        if (resSelect > 0 && FD_ISSET(0, &fds)) {
            printf("\n\nComando -> ");
            scanf(" %49[^\n]", cmd);

            res = validaComando(cmd, size_M, arrayCmd_M, nArgCmd_M, &index);

            switch (res) {
                case 0:
                    executaComando_M(cmd, index, &td, thread[1]);
                    break;
                case 1:
                    printf("\nComando inválido");
                    break;
                case 2:
                    printf("\nFalta de argumentos");
                    break;
                case 3:
                    printf("\nExcesso de argumentos");
                    break;
            }
        }

        // FIFO
        if (resSelect > 0 && FD_ISSET(fd_main, &fds)) {
            if (recebeFlag(fd_main) == -1) {
                shutDownClients(td.clients, td.nClients);
                pthread_mutex_lock(td.ptrinco);
                td.clients = clearClientList(td.clients, &td.nClients);
                pthread_mutex_unlock(td.ptrinco);
                break;
            }
        }
    }

    printf("\n--------------Saiu do ADMIN--------------\n");

    if (pthread_join(thread[0], NULL) != 0) {
        perror("Erro");
    }

    if (td.begin != 1) {
        if (pthread_join(thread[1], NULL) != 0) {
            perror("Erro");
        }
    }

    pthread_mutex_destroy(td.ptrinco);

    close(fd);

    unlink(FIFO_MOTOR); // apaga fifo do motor

    unlink(FIFO_MAIN);

    exit(0);
}
