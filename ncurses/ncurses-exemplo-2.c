// SO 23/24
// Exemplo de ncurses
// Informação acicional sobre o API de ncurses
// Documentação oficial
//    https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
// Resumo e exemplos
//    https://jbwyatt.com/ncurses.html
// Resumo
//    https://github.com/thenamankumar/ncurses-cheatsheet
// Man pages
//    https://invisible-island.net/ncurses/man/ncurses.3x.html
//    https://linux.die.net/man/3/ncurses
// Exemplos
//    https://gist.github.com/lancejpollard/b64426f51ac5504d6e481421cc8d907
//nota para compilar o programa, não esquecer a flag -lncurses (no final do comando)
//exemplo: gcc -o programa programa.c -lncurses

#include <ncurses.h> //biblioteca a incluir para utilizar o ncurses
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Função exemplo que faz o desenho de uma janela
// É chamada sempre que queremos atualizar o mapa
// Primeiro parâmetro  - recebe o ponteiro da janela (WINDOW)
// Segundo parâmetro   - recebe um inteiro, valor 1 - habilita o scrol na janel
void desenhaMapa(WINDOW *janela, int tipo)
{

    // quando temos o scroll ativo, não deveremos ter a borda desenhada na janela para não termos o problema escrever em cima das bordas
    if (tipo == 1)
    {
        scrollok(janela, TRUE); // liga o scroll na "janela".
        wprintw(janela, "\n #> ");
    }
    else 
    {
        keypad(janela, TRUE); // para ligar as teclas de direção (aplicar à janela)
        wclear(janela);// limpa a janela
        wborder(janela, '|', '|', '-', '-', '+', '+', '+', '+'); // Desenha uma borda. Nota importante: tudo o que escreverem, devem ter em conta a posição da borda
    }
    refresh(); // necessário para atualizar a janela
    wrefresh(janela); // necessário para atualizar a janela
}


// Função exemplo para tratar o teclado
// este exemplo aceita:
// as setas de direção para apresentar um texto na janelaTopo sem recurso à tecla enter
// tecla espaço para mudar o foco da janelaTopo para a janelaBaixo, e ativar a possibilidade de receber comandos (string) do utilizador
// letra q para finalizar o programa 
// O primeiro parâmetro da função é o ponteiro para a janela superior
// O segundo parâmetro da função é um ponteiro para a janela inferior
void trataTeclado(WINDOW *janelaTopo, WINDOW *janelaBaixo) 
{
    keypad(janelaTopo, TRUE);       // para ligar as teclas de direção (aplicar à janelaTopo)
    wmove(janelaTopo, 1, 1);        // posiciona o cursor,visualmente, na posicao 1,1 da janelaTopo
    //nota a posição é relativa à janelaTopo e não ao ecrã.
    int tecla = wgetch(janelaTopo); // MUITO importante: o input é feito sobre a janela em questão, neste caso na janelaTopo
    char comando[100];             // string que vai armazenar o comando

    while (tecla != 113) // trata as tecla até introduzirem a letra q. O código asci de q é 113
    {

        if (tecla == KEY_UP) // quando o utilizador introduz a seta para cima
        {
            desenhaMapa(janelaTopo, 2); // atualiza toda a janela
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla UP na posição 1,1 "); // escreve a palavra UP na posição  1,1. 
            //Nota: não escreve na posição 0,0 porque está lá a borda da janela que foi criada anteriormente
            wrefresh(janelaTopo);// // necessário para atualizar a janelaTopo. Nota: é apenas esta janela que pretendemos atualizar
        }
        else if (tecla == KEY_RIGHT)
        {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla RIGHT na posição 1,1");
            wrefresh(janelaTopo);
        }
        else if (tecla == KEY_LEFT)
        {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla LEFT na posição 1,1");
            wrefresh(janelaTopo);
        }
        else if (tecla == KEY_DOWN)
        {
            desenhaMapa(janelaTopo, 2);
            mvwprintw(janelaTopo, 1, 1, "Estou a carregar na tecla DOWN na posição 1,1");
            wrefresh(janelaTopo);
        }
        else if (tecla == ' ') // trata a tecla espaço
        {  // a tecla espaço ativa a janela inferior e tem o scroll ativo
          //  o wprintw e o wgetstr devem ser utilizados em janelas que tem o scroll ativo.
            echo();                         // habilita a maneira como o utilizador visualiza o que escreve
                                             // ou seja volta a ligar o echo para se ver o que se está a escrever
            wprintw(janelaBaixo, "\n #> "); // utilizada para imprimir. 
                                            //nota como a janelaBaixo tem o scroll ativo, ele vai imprimindo linha a linha
            wgetstr(janelaBaixo, comando);  // para receber do teclado uma string na "janelaBaixo" para a variavel comando
            wprintw(janelaBaixo, "\n [%s] ", comando);
            noecho(); //voltar a desabilitar o que o utilizador escreve
            wrefresh(janelaBaixo); //sempre que se escreve numa janela, tem de se fazer refresh
        }
        wmove(janelaTopo, 1, 1); // posiciona o cursor (visualmente) na posicao 1,1 da janelaTopo
        tecla = wgetch(janelaTopo); //espera que o utilizador introduza um inteiro. Importante e como já referido anteriormente introduzir a janela onde queremos receber o input
    }
}

//função main
//exemplo propriamente dito
// - Cria duas janelas
// - Uma janela na parte superior do ecrã
//         * na posição linha 3, coluna 1
//         * com o tamanho de 22 linhas e 82 colunas
//         * com uma borda
//         * que imprime no ecrã na posição 1,1, da própria janela um texto a informar qual a seta de direção o utilizador pressionou
//         * se o utilizador introduzir a tecla q termina o programa ordeiramente
// - Uma segunda janela logo por baixo da primeira janela, na posição, linha 23, coluna 1, com o tamanho de 15 linhas, 82 colunas
//         * na posição linha 23, coluna 1
//         * com o tamanho de 15 linhas, 82 colunas
//         * sem borda
//         * que recebe comandos do utilizador e imprime na linha o que o utilizador introduziu
//         * esta janela tem o scroll ativo
// - O programa muda o foco entre janelas, com recurso ao espaço para mudar da janelasuperior para a inferior. Enter para mudar o foco da janela inferior para a superior
// - Ou seja, sempre que o utilizador quiser escrever na janela de baixo, de carregar primeiro na tecla espaço
int main()
{
    // Inicializa assuntos de ncurses
    // -> mostra que tipo de inicialização pode ser necessária
    initscr(); // Obrigatorio e sempre a primeira operação de ncurses
    raw();  // desativa o buffer de input, cada tecla é lida imediatamente
    noecho();  // desliga o echo no ecrã, para voltar ativar devem utilizar a função echo();
    keypad(stdscr, TRUE);  // habilita as teclas  especiais, exemplo -> <-
    attrset(A_DIM);  //altera o brilho no print das janelas
    //as duas linhas seguintes são utilizadas para imprimir no ecrã, não utilizando nenhuma janela
    //as coordenas são relativas ao ecrã                                                               
    mvprintw(1, 10, "[ Up,Down,Right e Left comandos janela de cima ]");  // mensagem fora da janela, na linha 1, coluna 10 do ecrã
    mvprintw(2, 10, "[ space - muda para o foco da janela de baixo / q - sair ]"); // mensagem fora da janela, na linha 2, coluna 10 do ecrã
    WINDOW *janelaTopo = newwin(22, 82, 3, 1);  // Criar janela para a matriz de jogo, tendo os parametro numero de linhas,numero de colunas, posição onde começa a janela  e posição onde termina
    WINDOW *janelaBaixo = newwin(15, 82, 26, 1);  
    desenhaMapa(janelaTopo, 2);  // função exemplo que desenha o janela no ecrã
    desenhaMapa(janelaBaixo, 1);  // função exemplo que desenha o janela no ecrã
    trataTeclado(janelaTopo, janelaBaixo); // função exemplo que trata o teclado
    wclear(janelaTopo); // função que limpa o ecrã
    wrefresh(janelaTopo);  // função que faz atualização o ecrã com as operações realizadas anteriormente
    delwin(janelaTopo);  // apaga a janela.
    wclear(janelaBaixo); // função que limpa o ecrã
    wrefresh(janelaBaixo); // função que faz atualiza o ecrã com as operações realizadas anteriormente
    delwin(janelaBaixo);  // apaga a janela.
    endwin();  // encerra a utilização do ncurses. Muito importante senão o terminal fica inconsistente (idem se sair por outras vias)
    return 0;
}