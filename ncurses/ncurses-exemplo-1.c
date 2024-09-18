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
//    https://gist.github.com/lancejpollard/b64426f51ac5504d6e481421cc8d907c

// Instalação de ncurses
//   sudo apt install libncurses5 libncurses5-dev
//
// Para compilar: indicar -lncurses (tem que ser no final do comando)
// exemplo: gcc -o programa programa.c -lncurses

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

// Função utilitária que faz parte do exemplo
// Desenha molduras
void desenhaMoldura(int alt, int comp, int linha, int coluna);


// Exemplo propriamente dito
// - Semeia caracteres em posições e cores aleatórias
// - Usa uma janela para imprimir mensagens com funcionalidade scroll
// - Lê teclas de direção sem exigir "enter"
// - Lê uma cadeia de caracteres, alternando entre
//     modo "teclas direção" e "input normal"

int main(void) {
   int lin, col, cor, sair;
   WINDOW * janela;
   char texto[11];
   char msg[21];
   int ch;
   const char * p;
   srand(time(NULL));  // p/ números aleatórios. não tem nada a ver com ncurses

   // Inicializa assuntos de ncurses
   // -> mostra que tipo de inicialização pode ser necessária
   initscr();  // Obrigatorio e sempre a primeira operação de ncurses
   start_color();          // Inicializa o motor de cores
   clear();    // Limpa ecrã e posiciona cursos no canto superior esquerdo (refresh mais adiante)
   noecho();   // Desliga o echo porque a seguir vai ler telcas de direção
                  // não se pretende "ecoar" essa tecla no ecrã
   cbreak();   // desliga a necessidade de enter. cada tecla é lida imediatamente

   // Define 4 cores. formato: par (ID, cor do caracter, cor do fundo)
   // -> mostra como se definem cores
   init_pair(1, COLOR_RED, COLOR_BLACK);
   init_pair(2, COLOR_GREEN, COLOR_BLACK);
   init_pair(3, COLOR_BLUE, COLOR_BLACK);
   init_pair(4, COLOR_YELLOW, COLOR_BLUE);

   // Semeia 'x' pelo ecrã, cada um com a sua cor
   // -> mostra como se pode posicionar a impressão em linha,coluna
   // -> mostra como se especifica a cor pretendida
   for (int i = 0; i <125; ++i) {
      lin = rand() % 25;     // sorteia a coordelana y
      col = rand() % 60;     // sorteia a coordenada x
      cor = rand() % 3 + 1;  // sorteia a cor a usar
      attron(COLOR_PAIR(cor));   // "ativa" a cor sorteada
      mvaddch(lin, col, 'x'); // coloca 'x' nas posição e cor sorteadas
   }                          // mvaddch -> MoVe cursor para e ADDiciona CHar
   refresh();  // atualiza o ecrã, fazendo o conteúdo realmente aparecer
               // -> é sempre necessário fazer refresh (wrefresh se for uma janela)


   // Passo seguinte no exemplo: usa uma janela para mostrar conteúdo com scroll
   // -> mostra como se lê teclas de direção
   // -> mostra como se pode alternar entre leitura de teclas de direção e input normal
   // -> mostra como se pode fazer scroll de conteúdo sem afetar o resto do ecrã
   //    (o conteúdo e scorll ficam contidos na janela

   // ------------
   // janela de 10x20 a começar em linha 6, coluna 15
   //    - as janelas são posicionadas no ecrã "principal"
   // desenha uma borda manualmente à volta da janela pretendida
   // a borda é desenhada FORA da janela (são indicadas as dimensões da janela)
   //   as bordas "nativas" das janelas podem ser sobrepostas pelo conteúdo da janela
   //   nesta janela isso ia dar mais problemas que vantagens por causa da 
   //   funcionalidade de scroll e por isso desenha-se manualmente fora da janela


   attron(COLOR_PAIR(3));   // associa uma cor ao ecrã para desenhar uma moldura
   desenhaMoldura(10,20,6,15);
   janela = newwin(10, 20, 6, 15);   // cria uma janela 10 lin x 20 col em lin 6, col 15
   wattrset(janela, COLOR_PAIR(4));  // define foreground/backgorund dos caracteres
   wbkgd(janela, COLOR_PAIR(4));     // define backgound dos espaço vazio
   scrollok(janela, TRUE);           // liga scroll na janela
   keypad(janela, TRUE);   // para ligar as teclas de direção (aplicar à janela)
   wclear(janela);         // limpa a janela
   wrefresh(janela);       // wreferesh necessário para mostrar as alterações de conteúdo
   wprintw(janela,"Teclas de direção e espaço e q para sair\n");
   wrefresh(janela);

   // Reparar na direfença das funções. as que começam com "w" são
   // relativas a janelas. As sem o "w" incial são relativas ao ecrã todo

   // A moldura está a ser desenhada "manualmente". existem molduras "built-in"
   // borda: caracteres por esta ordem:
   //       vert. esquerda, vert. direita, horiz. topo, horiz. fundo
   //       canto sup, esq., canto sup. dir., canto inf. esq., canto inf. dir.
   // wborder(janela, '|', '|', '-', '-', '+', '+', '+', '+');
   // A moldura "build-in" com wborder ou box fica dentro da janela
   // e pode ser escrita por cima com o conteúdo da janela
   // Neste exemplo, a janela tem scroll e daria problemas. Por isso,
   // é desenhada uma moldura "manualmente" à volta e fora da janela


   // Ciclo principal
   // lê teclas. se for direção age em conformidade
   //   se for espaço, muda para modo input normal, lê string e volta
   //      a leitura de teclas
   //   se for 'q' sai do ciclo
   sair = 0;
   p = NULL;
   while (sair != 1) {
      ch = wgetch(janela);  // MUITO importante: o input é feito sobre a janela em questão
      switch (ch) {
      case KEY_UP:
         p = "subir";
         break;
      case KEY_DOWN:
         p = "descer";
         break;
      case KEY_LEFT:
         p = "esqerda";
         break;
      case KEY_RIGHT:
         p = "direita";
         break;
      case ' ':
         echo();          // Volta a ligar o echo para se ver o que se está
         nocbreak();      // a escrever. Idem input baseado em linha + enter
         wprintw(janela,"comando > ");
         wscanw(janela," %10[^\n]", texto);
         sprintf(msg,"output:[%s]",texto);
         p = msg;
         noecho();        // Volta a desligar o echo das teclas premidas
         cbreak();        // e a lógica de input em linha+enter no fim
         break;
      case 'q':
         sair = 1;
         break;
      }
      if (p != NULL) {
         wprintw(janela, "%s\n", p);   // reparar "w" inicial: imprime na janela
         p = NULL;
         wrefresh(janela);             // sem isto nada apareceria
      }
   }

   endwin();   // MUITO importante. é necessário fazer isto antes de sair senão o
   return 0;   // terminal fica inconsistente (idem se sair por outras vias)
}

// Funções auxiliares do exemplo

// Esta função desenha uma moldura à volta * no exterior * de uma janela
// os parâmetros são a dimensão e coordenadas da moldura ja janela
// a moldura é desenhada à volta das coordenadas e dimensão indicadas
// esta moldura é sempre desenhada no ecrã "principal (fora de janelas)
// - mudar conforme o pretendido

void desenhaMoldura(int alt, int comp, int linha, int coluna) {
   --linha;
   --coluna;
   alt+=2;
   comp+=2;
   // desenha barras verticais laterais
   for (int l=linha; l<=linha+alt-1; ++l) {
      mvaddch(l, coluna, '|');            // MoVe para uma posição e ADDiciona um CHar lá
      mvaddch(l, coluna+comp-1, '|');
   }
   // desenha as barras horizontais
   for (int c=coluna; c<=coluna+comp-1; ++c) {
      mvaddch(linha, c, '-');
      mvaddch(linha+alt-1, c, '-');
   }
   // desenha os cantos
   mvaddch(linha, coluna, '+');
   mvaddch(linha, coluna+comp-1, '+');
   mvaddch(linha+alt-1, coluna, '+');
   mvaddch(linha+alt-1,coluna+comp-1,'+');
}

