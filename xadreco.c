/***************************************************************************
 *   xadreco version 5.6. Chess engine compatible                          *
 *   with XBoard/WinBoard Protocol (C)                                     *
 *   Copyright (C) 2004 by Ruben Carlo Benante                             *
 *   dr.beco@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 * To contact the author, please write to:                                 *
 * Ruben Carlo Benante.                                                    *
 * email: dr.beco@gmail.com                                                *
 * web page: http:                                                         *
 * www.geocities.com/pag_sax/xadreco/                                      *
 * address:                                                                *
 * Federal University of Pernambuco                                        *
 * Center of Computer Science                                              *
 * PostGrad Room 7-1                                                       *
 * PO Box 7851                                                             *
 * Zip Code 50670-970                                                      *
 * Recife, PE, Brazil                                                      *
 * FAX: +55 (81) 2126-8438                                                 *
 * PHONE: +55 (81) 2126-8430  x.4067                                       *
 ***************************************************************************/
//
//xadreco vers�o 5.6. Motor de xadrez compat�vel com o XBoard/WinBoard (C)
//Copyright (C) 2004, Ruben Carlo Benante.
//
//Este programa � software livre; voc� pode redistribu�-lo e/ou
//modific�-lo sob os termos da Licen�a P�blica Geral GNU, conforme
//publicada pela Free Software Foundation; tanto a vers�o 2 da
//Licen�a como (a seu crit�rio) qualquer vers�o mais nova.
//
//Este programa � distribu�do na expectativa de ser �til, mas SEM
//QUALQUER GARANTIA; sem mesmo a garantia impl�cita de
//COMERCIALIZA��O ou de ADEQUA��O A QUALQUER PROP�SITO EM
//PARTICULAR. Consulte a Licen�a P�blica Geral GNU para obter mais
//detalhes.
//
//Voc� deve ter recebido uma c�pia da Licen�a P�blica Geral GNU
//junto com este programa; se n�o, escreva para a Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
//02111-1307, USA.
//
//Para contactar o autor, por favor escreva para:
//Ruben Carlo Benante.
//email: dr.beco@gmail.com
//p�gina web: http:
//www.geocities.com/pag_sax/xadreco/
//endere�o:
//Universidade Federal de Pernambuco
//Centro de Inform�tica
//Sala de P�s-gradua��o 7-1
//Caixa Postal 7851
//CEP 50670-970
//Recife, PE, Brasil
//FAX: +55 (81) 2126-8438
//FONE: +55 (81) 2126-8430  ramal.4067
//
//-----------------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%% Direitos Autorais segundo normas do c�digo-livre: (GNU - GPL)
//%% Universidade Estadual Paulista - UNESP, Universidade Federal de Pernambuco - UFPE
//%% Jogo de xadrez: xadreco
//%% Arquivo xadreco.cpp
//%% T�cnica: MiniMax
//%% Autor: Ruben Carlo Benante
//%% criacao: 10/06/1999
//%% vers�o para XBoard/WinBoard: 26/09/04
//%% vers�o 5.6, de 21/11/04
//%% e-mail do autor: dr.beco@gmail.com
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//-----------------------------------------------------------------------------
//#include <alloc.h> 
//#include <dos.h> //not portable.
//#include <conio.h> //not portable.
//#include <io.h> //not portable.

#ifndef _WIN32
#include <sys/select.h>
#include <sys/time.h>
#else
#include <conio.h>
#include <windows.h>
#include <winbase.h>
#include <wincon.h>
#include <io.h>
#endif


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

// 106550
#define MARGEM_PREGUICA 400
//margem para usar a estatica preguicosa
//margin to use the lazy evaluetion
#define PORCENTO_MOVIMENTOS 0.85
//a porcentagem de movimentos considerados, do total de op��es
//the percent of the total moves considered
#define FIMTEMPO 106550
//usada para indicar que o tempo acabou, na busca minimax
//used to indicate that the time is over, under minimax search
#define LIMITE 105000
//define o limite para a fun��o de avalia��o estatica
//define the min/max limite of the static valuation function
#define XEQUEMATE 100000
//define o valor do xequemate
//defines the value of the checkmate
#define LINHASDOLIVRO 408
//define a qtd de linhas no livro de abertura
//define how much lines there are in the openning book
#define MOVE_EMPATE1 52
//numero do movimento que a m�quina pode pedir empate pela primeira vez
//ply that engine can ask first draw
#define MOVE_EMPATE2 88
//numero do movimento que a m�quina pode pedir empate pela segunda vez
//ply that engine can ask second draw
#define QUANTO_EMPATE1 -200
// pede empate se tiver menos que 2 PEOES
//ask for draw if he has less then 2 PAWNS
#define QUANTO_EMPATE2 20
//pede empate se lances > MOVE_EMPATE2 e tem menos que 0.2 PEAO
//ask for draw if ply > MOVE_EMPATE2 and he has less then 0.2 PAWN
// dados ----------------------
int debug = 1;
//coloque zero para evitar gravar arquivo. 0:sem debug, 1:debug, 2:debug minimax
//0:do not save file xadreco.log, 1:save file, 2:minimax debug

typedef struct tabuleiro
{
  int tab[8][8];
//contem as pecas, sendo [coluna][linha], ou seja: e4
//the pieces. [column][line], example: e4
  int vez;
//contem -1 ou 1 para 'branca' ou 'preta'
//the turn to play: -1 white, 1 black
  int peao_pulou;
//contem coluna do peao adversario que andou duas, ou -1 para 'nao pode comer enpassant'
//column pawn jump two squares. -1 used to say "cannot capture 'en passant'"
  int roqueb, roquep;
//1:pode para os 2 lados. 0:nao pode mais. 3:mexeu TD. 2:mexeu TR.
//castle. 1: can do both. 0: none. 3:moved queen rook. 2:moved king rook.
  float empate_50;
//contador:quando chega a 50, empate.
//counter: when equal 50, draw by 50 moves rule.
  int situa;
//0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.) 7: sem resultado
//board situation: 0:nothing, 1:draw!, 2:check!, 3:white mated, 4:black mated, 5 and 6: time fell (white and black respec.) 7: * {Game was unfinished}
  int lancex[4];
//lance executado originador deste tabuleiro.
//the move that originate this board. (integer notation)
  int especial;
//0:nada. 1:roque pqn. 2:roque grd. 3:comeu en passant.
//promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
//0:nothing. 1:king castle. 2:queen castle. 3:en passant capture.
//promotion: 4=queen, 5=knight, 6=rook, 7=bishop.
  int numero;
//numero do movimento: 1-e2e4 2-e7e5 3-g1f3 4-b8c6
//ply number
} tabuleiro;

typedef struct movimento
{
  int lance[4];
//lance em nota��o inteira
//move in integer notation
  int peao_pulou;
//contem coluna do peao que andou duas neste lance
//column pawn jump two squares. -1 used to say "cannot capture 'en passant'"
  int roque;
//0:mexeu rei. 1:ainda pode. 2:mexeu TR. 3:mexeu TD.
//0: moved king. 1: can yet. 2:moved king rook. 3:moved queen rook.
  int flag_50;
//Quando igual a:0=nada,1=Moveu peao,2=Comeu,3=Peao Comeu. Entao zera empate_50;
//when equal to: 0:nothing, 1:pawn moved, 2:capture, 3:pawn capture. Put zero to empate_50;
  int especial;
//0:nada. 1:roque pqn. 2:roque grd. 3:comeu enpassant.
//promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
//0:nothing. 1:king castle. 2:queen castle. 3:en passant capture.
//promotion: 4=queen, 5=knight, 6=rook, 7=bishop.
  int valor_estatico;
//dado um tabuleiro, este � o valor est�tico deste tabuleiro ap�s a execu��o deste movimento, no contexto da analise minimax
//given a board, this is the static value of the board position after this move, in the context of minimax analyzes
  int ordena;
//flag para dizer se j� foi inserido esse elemento na lista ordenada;
//flag that says if this move was already inserted in the new ordered list
  struct movimento *prox;
//ponteiro para o pr�ximo movimento da lista
//pointer to the next move on the list
} movimento;

typedef struct resultado
//resultado de uma an�lise de posi��o
//result of a position analyze
{
  int valor;
//valor da variante
//variation value
  struct movimento *plance;
//os movimentos da variante
//variation moves
} resultado;

typedef struct listab
//usado para armazenar o hist�rico do jogo e comparar posi��es repetidas
//used to store the history of the game and analyze the repeated positions
{
  int tab[8][8];
//[coluna][linha], exemplo, lance: [e][2] para [e][4]
//the pieces. [column][line], example: e4
  int vez;
//de quem � a vez
//the turn to play: -1 white, 1 black
  int peao_pulou;
//contem coluna do peao adversario que andou duas, ou -1 para nenhuma
//column pawn jump two squares. -1 used to say "cannot capture 'en passant'"
  int roqueb, roquep;
//1:pode para os 2 lados. 0:nao pode mais. 3:mexeu TD. 2:mexeu TR.
//castle. 1: can do both. 0: none. 3:moved queen rook. 2:moved king rook.
  float empate_50;
//contador:quando chega a 50, empate.
//counter: when equal 50, draw by 50 moves rule.
  int situa;
//0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.) 7: sem resultado
//board situation: 0:nothing, 1:draw!, 2:check!, 3:white mated, 4:black mated, 5 and 6: time fell (white and black respec.) 7: * {Game was unfinished}
  int lancex[4];
//lance executado originador deste tabuleiro.
//the move that originate this board. (integer notation)
  int especial;
//0:nada. 1:roque pqn. 2:roque grd. 3:comeu enpassant.
//promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
//0:nothing. 1:king castle. 2:queen castle. 3:en passant capture.
//promotion: 4=queen, 5=knight, 6=rook, 7=bishop.
  int numero;
//numero do movimento: 1-e2e4 2-e7e5 3-g1f3 4-b8c6
//move number (not ply)
  int rep;
//quantidade de vezes que essa posicao ja repetiu comparada
//how many times this position repeated
  struct listab *prox;
//ponteiro para o pr�ximo da lista
//pointer to next on list
  struct listab *ant;
//ponteiro para o anterior da lista
//pointer to before on list
} listab;

struct resultado result;
//a melhor variante achada
//the best variation found
struct listab *plcabeca;
//cabeca da lista encadeada de posicoes repetidas
//pointer to the first position on list
struct listab *plfinal;
//fim da lista encadeada
//pointer to the last position on list
struct movimento *succ_geral;
//ponteiro para a primeira lista de movimentos de uma seq��ncia de n�veis a ser analisadas;
//pointer to the first list move that should be analyzed
FILE *fsaida, *ftemp, *fmini;
//fsaida: arquivo de log xadreco.log, ftemp: arquivo de trava
//log file xadreco.log
int machine = 0;
//numero da engine na memoria;
//number of engines in memory
int USALIVRO = 1;
//1:consulta o livro de aberturas livro.txt. 0:nao consulta
//1:use opening book. 0:do not use.
int ofereci = 1;
//o computador pode oferecer um empate (duas|uma) vezes.
//the engine can offer draw twice: one as he got a bad position, other in the end of game
char disc;
//sem uso. vari�vel de descarte, para nao sujar o stdin, dic=pega()
//not used. discard value from pega(), just to do not waring and trash stdin
int mostrapensando = 0;
//mostra as linhas que o computador est� escolhendo
//show thinking option
enum piece_values
{
  REI = 10000, DAMA = 900, TORRE = 500, BISPO = 325, CAVALO = 300, PEAO =
    100, VAZIA = 0, NULA = -1
};
//valor das pecas (positivo==pretas)
//pieces value (positive==black)
char primeiro = 'h', segundo = 'c';
//primeiro e segundo jogadores: h:humano, c:computador
//first and second players: h:human, c:computer
int xboard = 0;
//1:usando interface gr�fica. 0:somente no prompt
//1:using GUI. 0:only prompt
int analise = 0;
//-1 para primeira vez. 0 para n�o analise. 1 para analise. ("exit" coloca ela como 0 novamente)
//-1 first time. 0 do not analyze. 1 analyze game. ("exit" put it 0 again)
int setboard = 0;
//-1 para primeira vez. 0 n�o edita. 1 edita. Posi��o FEN.
//-1 first time. 0 do not edit. 1 edit game. FEN Position.
char ultimo_resultado[80] = "";
//armazena o ultimo resultado, ex: 1-0 {White mates}
//stores the last result
char pausa;
//pause entre lances (computador x computador)
//pause between moves (computer x computer)
int nivel = 3;
//nivel de profundidade (agora com aprofundamento iterativo, est� sem uso)
//deep level (now with iterative search deepning it is useless)
int profflag = 1;
//Flag de captura ou xeque para liberar mais um nivel em profsuf
//when capturing or checking, let the search go one more level
int totalnodo = 0;
//total de nodos analisados para fazer um lance
//total of nodes analyzed before one move
int totalnodonivel = 0;
//total de nodos analisados em um n�vel da �rvore
//total of nodes analyzed per level of tree
clock_t clock1, clock2, difclock, inputcheckclock;
//calcular o tempo gasto no minimax e outras
//calculating time in minimax function and others
const int brancas = -1;
//pe�as brancas s�o negativas
//white pieces are negatives
const int pretas = 1;
//pe�as pretas s�o positivas
//black pieces are positives
unsigned char gira = (unsigned char) 0;
//para mostrar um rostinho sorrindo (sem uso)
//to show a smile face (useless)
long int tempomovclock = 300;	//120 centiseg = 1,2 seg
//3000 miliseg = 3 seg por lance = 300 seg por 100 lances = 5 min por jogo (de 100 lances)
//time per move. Examplo: 3000milisec = 3 sec per move = 300 sec per 100 moves = 5 min per game (of 100 moves)
const float version = 5.6;
//Vers�o do programa
//program version
char flog[] = "log.xadreco.second";
//Nome do arquivo de log
//log file name
int ABANDONA = -2430;
//abandona a partida quando perder algo como DAMA, 2 TORRES, 1 BISPO e 1 PEAO; ou nunca, quando jogando contra outra engine
//the engine will resign when loosing a QUEEN, 2 ROOKS, 1 BISHOP and 1 PAWN or something like that
int COMPUTER = 0;
//flag que diz se est� jogando contra outra engine.
//Flag that say it is playing agains other engine
//int teminterroga = 0;
//flag que diz que o comando "?" foi executado
//flag to mark that command "?" run

// prototipos gerais ---------------------------------------------------------
//eliminada para portabilidade com linux: posiciona o cursor
//eliminated to linux portability: cursor position
//void clrscr(void);
//eliminada para portabilidade com linux: apaga a tela
//eliminated to linux portability: clear screen
void imptab (tabuleiro tabu);
//imprime o tabuleiro
//print board
void mostra_lances (tabuleiro tabu);
//mostra na tela informacoes do jogo e an�lises
//show on screen game information and analyzes
void lance2movi (char *m, int *l, int especial);
//transforma lances int 0077 em char tipo a1h8
//change integer notation to algebric notation
int movi2lance (int *l, char *m);
//faz o contrario: char b1c3 em int 1022. Retorna falso se nao existe.
//change algebric notation to integer notation. Return FALSE if it is not possible.
inline int adv (int vez);
//retorna o adversario de quem esta na vez
//return the other player to play
inline int sinal (int x);
//retorna 1, -1 ou 0. Sinal de x
//return 1, -1 or 0: signal of x.
int igual (int *lance1, int *lance2);
//compara dois vetores de lance[4]. Se igual, retorna 1
//return 1 if lance1==lance2
char pega (char *no, char *msg);
//pegar caracter (linux e windows)
//to solve getch, getche, getchar and whatever problems of portability
float rand_minmax (float min, float max);
//retorna um valor entre [min,max], inclusive
//return a random value between [min,max], inclusive
void sai (int error);
//termina o programa
//to end the program
void inicia (tabuleiro * tabu);
//para inicializar alguns valores no in�cio do programa;
//to start some values in the beggining of the program
void coloca_pecas (tabuleiro * tabu);
//coloca as pe�as na posi��o inicial
//put the pieces in the start position
void testapos (char *pieces, char *color, char *castle, char *enpassant,
	       char *halfmove, char *fullmove);
//testa posi��o dada. dever� ser melhorado.
// used to test a determined position; should be improved.
void testajogo (char *movinito, int numero);
//retorna um lance do jogo de teste
//returns a move in the test game
void limpa_pensa (void);
//limpa algumas vari�veis para iniciar a pondera��o
//to clean some variables to start pondering
void enche_pmovi (movimento * pmovi, int c0, int c1, int c2, int c3, int p,
		  int r, int e, int f);
//preenche a estrutura movimento
//fullfil movimento structure
void msgsai (char *msg, int error);
//emite mensagem antes de sair do programa (por falta de mem�ria ou outros itens), ou tudo certo
//message before exit program because lack of memory or other problems, or just exiting ok.
void imprime_linha (movimento * loop, int numero, int vez);
//imprime uma sequencia de lances armazenada na lista movimento, numerados.
//print a sequence of moves in movimento list, numbered.
int pollinput (void);
// retorna verdadeiro se existe algum caracter no buffer para ser lido
// return true if there are some character in the buffer to be read

// apoio xadrez -----------------------------------------------------
int ataca (int cor, int col, int lin, tabuleiro tabu);
//retorna 1 se "cor" ataca casa(col,lin) no tabuleiro tabu
//return 1 if "color" atack square(col, lin) in board tabu
int qataca (int cor, int col, int lin, tabuleiro tabu, int *menor);
//retorna o numero de ataques da "cor" na casa(col,lin) de tabu, *menor � a menor pe�a que ataca.
//return the number of atacks of "color" at square(col,lin) in board tabu, *menor is the minor piece that atack
int xeque_rei_das (int cor, tabuleiro tabu);
//retorna 1 se o rei de cor "cor" esta em xeque
//return 1 if "color" king is in check
void volta_lance (tabuleiro * tabu);
//para voltar um movimento. Use duas vezes para voltar um lance inteiro.
//take back one ply. Use twice to take back one move
char analisa (tabuleiro * tabu);
//analisa uma posi��o mas n�o joga
//analyze a position, but do not play it
struct movimento *valido (tabuleiro tabu, int *lanc);
//procura nos movimentos de geramov se o lance em questao eh valido. Retorna o *movimento preenchido. Se nao, retorna NULL.
//search on the list generated by geramov if the move lanc is valid. Return *movimento fullfiled. If not, return NULL.
char situacao (tabuleiro tabu);
//retorna char que indica a situacao do tabuleiro, como mate, empate, etc...
//return a char that indicate the situation of the board, as mate, draw, etc...
void ordena_succ (int nmov);
//ordena succ_geral
//sort succ_geral

// livro --------------------------------------------------------------
void usalivro (tabuleiro tabu);
//retorna em result.plance uma variante do livro, baseado na posi��o do tabuleiro tabu
//put in result.plance a line found in the book, from the position on the board tabu
void listab2string (char *strlance);
//pega a lista de tabuleiros e cria uma string de movimentos, como "e1e2 e7e5"
//generate a string with all moves, like "e1e2 e7e5"
struct movimento *string2pmovi (int numero, char *linha);
//retorna uma (lista) linha de jogo como se fosse a resposta do minimax
//return a variation on a list, like the minimax's return.
int igual_strlances_strlinha (char *strlances, char *strlinha);
//retorna verdadeiro se o jogo atual strlances casa com a linha do livro atual strlinha
//return TRUE if the game strlances match with one line strlinha of the opening book
int estatico_pmovi (tabuleiro tabu, movimento * cabeca);
//retorna o valor est�tico de um tabuleiro ap�s jogada a lista de movimentos cabeca

// computador joga ----------------------------------------------------------
struct movimento *geramov (tabuleiro tabu, int *nmov);
//retorna lista de lances possiveis, ordenados por xeque e captura. Deveria ser uma ordem melhor aqui.
//return a list of possibles moves, ordered by check and capture. Should put a best order here.
void minimax (tabuleiro atual, int prof, int alfa, int beta, int niv);
//coloca em result a melhor variante e seu valor.
//put in result the better variation and its value
int profsuf (tabuleiro atual, int prof, int alfa, int beta, int niv);
//retorna verdadeiro se (prof>nivel) ou (prof==nivel e nao houve captura ou xeque) ou (houve Empate!)
//return TRUE if (deep>level) or (deep==level and not capture or check) or (it is a draw)
int estatico (tabuleiro tabu, int cod, int niv, int alfa, int beta);
//retorna um valor estatico que avalia uma posicao do tabuleiro, fixa. Cod==1: tempo estourou no meio da busca. Niv: o nivel de distancia do tabuleiro real para a c�pia examinada
//return the static value of a position. If cod==1, means that this position was here only because the time is over in the middle of the search. Niv: the distance between the real board and the virtual copy examined.
char joga_em (tabuleiro * tabu, movimento movi, int cod);
//joga o movimento movi em tabuleiro tabu. retorna situacao. Insere no listab *plfinal se cod==1
//do the move! Move movi on board tabu. Return the situation. Insert the new board in the listab *plfina list if cod==1

// listas dinamicas ----------------------------------------------------------------
struct movimento *copimel (movimento pmovi, movimento * plance);
//retorna nova lista contendo o movimento pmovi mais a sequencia de movimentos plance. (para melhor_caminho)
//return a new list adding (new single move pmovi)+(old list moves plance)
void copimov (movimento * dest, movimento * font);
//copia os itens da estrutura movimento, mas nao copia ponteiro prox. dest=font
//copy only the contents of the structure, but not the pointer. dest=font
void copilistmovmel (movimento * dest, movimento * font);
//mantem dest, e copia para dest->prox a lista encadeada font. Assim, a nova lista � (dest+font)
//keep dest, and copy the list font to dest->prox. So, the new list is (dest+font)
struct movimento *copilistmov (movimento * font);
//copia uma lista encadeada para outra nova. Retorna cabeca da lista destino
//copy one list to another new one. Return the new head.
void insere_listab (tabuleiro tabu);
//Insere o tabuleiro tabu na lista listab. posiciona plcabeca, plfinal.   Para casos de empate de repeticao de lances, e para pegar o hist�rico de lances
//insert the board tabu in the list listab *plcabeca, positioning plcabeca and plfinal. Use to see draw, and to get history
void retira_listab (void);
//posiciona plfinal no *ant. ou seja: apaga o ultimo da lista. (usada para voltar de variantes ruins ou para voltar um lance a pedido do jogador)
//delete the last board in the listab. (used to retract from worst variations, or to take back a move asked by player)
void copitab (tabuleiro * dest, tabuleiro * font);
//copia font para dest. dest=font.
//copy font to dest. dest=font
void libera_lances (movimento * cabeca);
//libera da memoria uma lista encadeada de movimentos
//free memory of a list of moves
void retira_tudo_listab (void);
//zera a lista de tabuleiros
//free all listab list.
char humajoga (tabuleiro * tabu);
//humano joga. Aceita comandos XBoard/WinBoard.
//human play. Accept XBoard/WinBoard commands.
char compjoga (tabuleiro * tabu);
//computador joga. Chama o livro de aberturas ou o minimax.
//computer play. Call the opening book or the minimax functions.

// funcoes -------------------------------------------------------------------------
int
main (int argc, char *argv[])
{
  tabuleiro tabu;
  char feature[256];
  char resultado, opc;
  int moves, minutes, incre;
  setbuf (stdout, NULL);
  setbuf (stdin, NULL);
  sprintf (feature, "%s%.1f%s",
	   "feature ping=0 setboard=1 playother=1 san=0 usermove=0 time=0 draw=1 sigint=0 sigterm=0 reuse=1 analyze=1 myname=\"Xadreco v.",
	   version,
	   ", by Beco\" variants=\"normal\" colors=0 ics=0 name=0 pause=0");
  if (debug)
    {
      //strcpy (flog,"log.xadreco.temp");
      if ((ftemp = fopen ("log.xadreco.temp", "r")) == NULL)
//arquivo n�o exite? entao � machine 1.
	{
	  ftemp = fopen ("log.xadreco.temp", "w");
	  if (ftemp != NULL)
	    fprintf (ftemp, "xadreco : first engine active!\n");
	  (void) fclose (ftemp);
	  strcpy (flog, "log.xadreco.first");
	  fsaida = fopen (flog, "w");
	  machine = 1;
	}
      else
//arquivo j� existe? ent�o � machine 2.
	{
	  (void) fclose (ftemp);
	  machine = 2;
	  debug = 1;
//m�quina 2 n�o pode fazer debug avan�ado.
	  strcpy (flog, "log.xadreco.second");
	  fsaida = fopen (flog, "w");
	}
      if (!fsaida)
	debug = 0;
//n�o conseguiu abrir nenhum dos dois arquivos. desliga debug!
    }
  printf ("Xadreco version %.1f, Copyright (C) 2004 Ruben Carlo Benante\n"
	  "Xadreco comes with ABSOLUTELY NO WARRANTY;\n"
	  "This is free software, and you are welcome to redistribute it "
	  "under certain conditions; Please, visit http://www.fsf.org/licenses/gpl.html\n"
	  "for details.\n\n", version);
  printf ("Xadreco vers�o %.1f, Copyright (C) 2004 Ruben Carlo Benante\n"
	  "O Xadreco n�o possui QUALQUER GARANTIA;\n"
	  "Ele � software livre e voc� est� convidado a redistribui-lo "
	  "sob certas condi��es; Por favor, visite http://www.fsf.org/licenses/gpl.html\n"
	  "para obter detalhes.\n\n", version);
  if (debug)
    {
      fprintf (fsaida,
	       "Xadreco version %.1f, Copyright (C) 2004 Ruben Carlo Benante\n"
	       "Xadreco comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to redistribute it "
	       "under certain conditions; Please, visit http://www.fsf.org/licenses/gpl.html\n for details.\n\n",
	       version);
      fprintf (fsaida,
	       "Xadreco vers�o %.1f, Copyright (C) 2004 Ruben Carlo Benante\n"
	       "O Xadreco n�o possui QUALQUER GARANTIA;\n"
	       "Ele � software livre e voc� est� convidado a redistribui-lo "
	       "sob certas condi��es; Por favor, visite http://www.fsf.org/licenses/gpl.html\n"
	       "para obter detalhes.\n\n", version);
      if (machine == 1)
	fprintf (fsaida, "xadreco : primeira m�quina\n");
      else if (machine == 2)
	fprintf (fsaida, "xadreco : segunda m�quina\n");
    }
  if (argc > 1)
    if (!strcmp (argv[1], "xboard"))
//� xboard!
      {
	xboard = 1;
	setboard = -1;
	analise = -1;
	if (debug)
	  {
	    fprintf (fsaida, "xadreco : feature done=0\n");
	    fprintf (fsaida, "xadreco : %s\n", feature);
	    fprintf (fsaida, "xadreco : feature done=1\n");
	  }
	printf ("feature done=0\n");
	printf ("%s\n", feature);
	printf ("feature done=1\n");
      }
  do
    {
      inicia (&tabu);
      coloca_pecas (&tabu);
      insere_listab (tabu);
//------------------------------------------------------------------------------
      if (xboard)
	{
	  primeiro = 'h';
	  segundo = 'c';
//            nivel = 3;
//nao esta sendo usada. Implementar o comando "sd n"
	}
      else
	{
	  printf ("\n\n");
	  primeiro =
	    pega ("ch", "\nQuem joga de brancas (c:Computador, h:Humano)? ");
	  segundo =
	    pega ("ch", "\nQuem joga de  pretas (c:Computador, h:Humano)? ");
	  if (primeiro == 'c' || segundo == 'c')
	    {
	      printf
		("\n\nQual o tempo de jogo? MOVIMENTOS SEGUNDOS INCREMENTO: ");
	      scanf ("%d %d %d", &moves, &minutes, &incre);
	      if (moves <= 0)
		moves = 100;
	      if (minutes <= 0)
		minutes = 2;
	      incre -= 2;
	      if (incre < 0)
		incre = 0;
	      tempomovclock = minutes * 6000 / moves + incre * 100;
	    }
	  if (primeiro == segundo && primeiro == 'c')
	    pausa = pega ("sn", "\nPausa entre os lances? (s/n): ");
	  imptab (tabu);
	  if (pausa == 's')
	    disc = pega (" ", "\nTecle <ESPACO>+<ENTER> para prosseguir\n");
	}
    joga_novamente:
      if (tabu.vez == brancas)
// jogam as brancas
	if (primeiro == 'c')
	  resultado = compjoga (&tabu);
	else
	  resultado = humajoga (&tabu);
      else
// jogam as pretas
      if (segundo == 'c')
	resultado = compjoga (&tabu);
      else
	resultado = humajoga (&tabu);
      if (resultado != 'w' && (!xboard || resultado != 'c'))
	imptab (tabu);
      switch (resultado)
	{
	case '*':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "* {Game was unfinished}");
	      if (debug)
		fprintf (fsaida, "xadreco : * {Game was unfinished}\n");
	      printf ("* {Game was unfinished}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nJogo terminou sem resultado.");
//tabu.situa=7
	      break;
	    }
	case 'M':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "0-1 {Black mates}");
	      if (debug)
		fprintf (fsaida, "xadreco : 0-1 {Black mates}\n");
	      printf ("0-1 {Black mates}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nAs pretas deram xequemate.");
//tabu.situa=3
	      break;
	    }
	case 'm':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "1-0 {White mates}");
	      if (debug)
		fprintf (fsaida, "xadreco : 1-0 {White mates}\n");
	      printf ("1-0 {White mates}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nAs brancas deram xequemate.");
//tabu.situa=4
	      break;
	    }
	case 'a':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "1/2-1/2 {Stalemate}");
	      if (debug)
		fprintf (fsaida, "xadreco : 1/2-1/2 {Stalemate}\n");
	      printf ("1/2-1/2 {Stalemate}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nEmpate por afogamento.");
//tabu.situa=1
	      break;
	    }
	case 'p':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "1/2-1/2 {endless check}");
	      if (debug)
		fprintf (fsaida, "xadreco : 1/2-1/2 {endless check}\n");
	      printf ("1/2-1/2 {endless check}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nEmpate por xeque perpetuo.");
//tabu.situa=1
	      break;
	    }
	case 'c':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado,
		      "1/2-1/2 {both players agreed to draw}");
//aceitar empate
	      if (debug)
		{
		  fprintf (fsaida, "xadreco : draw accepted\n");
		  fprintf (fsaida,
			   "xadreco : 1/2-1/2 {both players agreed to draw}\n");
		}
	      printf ("draw accepted\n");
	      printf ("1/2-1/2 {both players agreed to draw}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nEmpate por comum acordo.");
	      break;
	    }
	case 'i':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado,
		      "1/2-1/2 {insufficient mating material}");
	      if (debug)
		fprintf (fsaida,
			 "xadreco : 1/2-1/2 {insufficient mating material}\n");
	      printf ("1/2-1/2 {insufficient mating material}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nEmpate por insuficiencia de material.");
//tabu.situa=1
	      break;
	    }
	case '5':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "1/2-1/2 {Draw by fifty moves rule}");
	      if (debug)
		fprintf (fsaida,
			 "xadreco : 1/2-1/2 {Draw by fifty moves rule}\n");
	      printf ("1/2-1/2 {Draw by fifty moves rule}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf
		("\nEmpate apos 50 lances sem movimento de peao, e sem captura.");
//tabu.situa=1
	      break;
//Inclui o caso de Rei x Rei e Pecas: obrigatorio dar mate em 50 lances
	    }
	case 'r':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado,
		      "1/2-1/2 {Draw by triple repetition}");
	      if (debug)
		fprintf (fsaida,
			 "xadreco : 1/2-1/2 {Draw by triple repetition}\n");
	      printf ("1/2-1/2 {Draw by triple repetition}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf
		("\nEmpate por repeticao da mesma posicao por tres vezes.");
//tabu.situa=1
	      break;
	    }
	case 'T':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "0-1 {White flag fell}");
	      if (debug)
		fprintf (fsaida, "xadreco : 0-1 {White flag fell}\n");
	      printf ("0-1 {White flag fell}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nAs brancas perderam por tempo.");
//tabu.situa=5
	      break;
	    }
	case 't':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "1-0 {Black flag fell}");
	      if (debug)
		fprintf (fsaida, "xadreco : 1-0 {Black flag fell}\n");
	      printf ("1-0 {Black flag fell}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nAs pretas perderam por tempo.");
//tabu.situa=6
	      break;
	    }
	case 'B':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "0-1 {White resigns}");
	      if (debug)
		fprintf (fsaida, "xadreco : 0-1 {White resigns}\n");
	      printf ("0-1 {White resigns}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nAs brancas abandonam.");
	      break;
	    }
	case 'b':
	  if (xboard)
	    {
	      strcpy (ultimo_resultado, "1-0 {Black resigns}");
	      if (debug)
		fprintf (fsaida, "xadreco : 1-0 {Black resigns}\n");
	      printf ("1-0 {Black resigns}\n");
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf ("\nAs pretas abandonam.");
	      break;
	    }
	case 'e':
	  if (xboard)
	    {
	      if (ultimo_resultado[0] != '\0')
//se existe um resultado, envia ele para finalizar a partida
		{
		  if (debug)
		    fprintf (fsaida, "xadreco : %s\n", ultimo_resultado);
		  printf ("%s\n", ultimo_resultado);
		}
	      else
		msgsai ("Computador sem lances v�lidos 1", 35);
//algum problema ocorreu que esta sem lances
	      primeiro = 'h';
	      segundo = 'h';
	      goto joga_novamente;
	    }
	  else
	    {
	      printf
		("\nSem movimentos v�lidos para fazer a partir deste ponto!");
	      break;
	    }
	case 'w':
	  opc = 's';
//Novo jogo
	  continue;
	case 'x':
	  if (!xboard)
	    {
	      printf ("\nXeque !");
//tabu.situa=2
	    }
	case 'y':
//retorna y: simplesmente gira o la�o para jogar de novo. Troca de adv.
	default:
//'-' para tudo certo...
	  if (!xboard && (pausa == 's' || pausa == 'c'))
	    {
	      pausa =
		pega ("c ",
		      "\nTecle <ESPACO>+<ENTER> para prosseguir ou <c>+<ENTER> para comando");
	      if (pausa != 'c')
		pausa = 's';
	    }
	  goto joga_novamente;
	}
//fim do switch(resultado)
      if (!xboard)
	opc = pega ("sn", "\nQuer recomecar? (s/n): ");
      else
	opc = 's';
    }
  while (opc == 's');

  if (!xboard)
    msgsai ("obrigado por jogar comigo!", 0);
//vai para tela
  else
    msgsai ("quit\n", 0);
//vai apenas para o log, mas n�o para a sa�da
  return EXIT_SUCCESS;
}

// mostra a lista de lances da tela, a melhor variante, e responde ao comando "hint"
void
mostra_lances (tabuleiro tabu)
{
  struct movimento *pmovi, *loop;
  char m[80];
  int linha = 1, coluna = 34, nmov = 0;
  m[0] = '\0';
  if (!xboard)
    {
      pmovi = geramov (tabu, &nmov);
      loop = pmovi;
      printf ("\n");
      while (loop != NULL)
	{
	  lance2movi (m, loop->lance, loop->especial);
	  printf ("%s, ", m);
	  linha++;
	  if (linha == 17)
	    {
	      linha = 1;
	      coluna += 9;
	    }
	  loop = loop->prox;
	}
      printf ("\nNao move peao ou captura a %.1f lances", tabu.empate_50);
      printf ("\nValor do  Lance: %+.2f", result.valor / 100.0);
      printf ("\nMelhor variante: ");
      imprime_linha (result.plance, tabu.numero, tabu.vez);
      printf ("                         \n");
//linha de comando
      libera_lances (pmovi);
//libera memoria da lista de lances ja utilizada
    }
  else
    {
//nivel pontua��o tempo totalnodo variante
//nivel est� errado!
      printf ("%d %d %d %d ", nivel, result.valor, difclock, totalnodo);
//result.valor*10 o Winboard divide por 100. centi-peao
      if (debug)
	fprintf (fsaida, "xadreco : %d %+.2f %d %d ", nivel,
		 result.valor / 100.0, difclock, totalnodo);
//result.valor/100 para a Dama ficar com valor 9
      imprime_linha (result.plance, tabu.numero, tabu.vez);
      printf ("\n");
      if (debug)
	fprintf (fsaida, "\n");
    }
}

//fim do mostra_lances
void
libera_lances (movimento * cabeca)
{
  movimento *loop, *aux;
  if (cabeca == NULL)
    return;
  loop = cabeca;
  aux = cabeca;
  while (loop != NULL)
    {
      aux = aux->prox;
      free (loop);
      loop = aux;
    }
  cabeca = NULL;
}

void
imptab (tabuleiro tabu)
{
  int col, lin, casacor;
//nao precisa se tem textbackground
  int colmax, colmin, linmax, linmin, inc;
  char movinito[80];
  movinito[79] = '\0';
  if (!xboard)
    {
      linmin = 7;
      linmax = -1;
      inc = -1;
      colmin = 0;
      colmax = 8;
      if (primeiro == 'c' && segundo == 'h')
	{
	  linmin = 0;
	  linmax = 8;
	  inc = 1;
	  colmin = 7;
	  colmax = -1;
	}
      printf ("\n\r  ");
      printf ("+------------------------+");
      lin = linmin;
      do
	{
	  printf ("\n\r%d ", lin + 1);
	  printf ("|");
	  col = colmin;
	  do
	    {
	      if ((lin + col) % 2)
		casacor = 1;
	      else
		casacor = 2;
	      switch (tabu.tab[col][lin])
		{
		case PEAO:
		  printf (" p ");
		  break;
		case CAVALO:
		  printf (" c ");
		  break;
		case BISPO:
		  printf (" b ");
		  break;
		case TORRE:
		  printf (" t ");
		  break;
		case DAMA:
		  printf (" d ");
		  break;
		case REI:
		  printf (" r ");
		  break;
// pecas brancas
		case -PEAO:
		  printf (" P ");
		  break;
		case -CAVALO:
		  printf (" C ");
		  break;
		case -BISPO:
		  printf (" B ");
		  break;
		case -TORRE:
		  printf (" T ");
		  break;
		case -DAMA:
		  printf (" D ");
		  break;
		case -REI:
		  printf (" R ");
		  break;
		default:
		  if (casacor == 1)
		    printf (" . ");
//casa branca
		  else
		    {
		      printf (" * ");
//casa preta
		    }
		  break;
		}
	      col -= inc;
	    }
	  while (col != colmax);

	  printf ("|");
	  lin += inc;
	}
      while (lin != linmax);

      printf ("\n\r  ");
      printf ("+------------------------+");
      printf ("\n\r  ");
      if (primeiro == 'h' || primeiro == segundo)
	printf ("  a  b  c  d  e  f  g  h\n\n\r");
      if (primeiro == 'c' && segundo == 'h')
	printf ("  h  g  f  e  d  c  b  a\n\n\r");
    }
//nao eh xboard.
  if (tabu.lancex[0] == 0 && tabu.lancex[1] == 0 && tabu.lancex[2] == 0
      && tabu.lancex[3] == 0)
    {
      if (!xboard)
	printf ("Lance executado :       ");
//apagar o lance anterior da tela
    }
  else
    {
      lance2movi (movinito, tabu.lancex, tabu.especial);
//imprime o movimento
      if (!xboard)
	if (tabu.vez == pretas)
	  printf ("Lance executado :%d. %s",
		  (int) ((tabu.numero + 1.0) / 2.0), movinito);
	else
	  printf ("Lance executado :%d. ... %s",
		  (int) ((tabu.numero + 1.0) / 2.0), movinito);
      else
	{
	  if ((tabu.vez == brancas && segundo == 'c')
	      || (tabu.vez == pretas && primeiro == 'c'))
	    {
	      if (debug)
		fprintf (fsaida, "xadreco : move %s\n", movinito);
	      printf ("move %s\n", movinito);
	    }
	}
    }
}

void
lance2movi (char *m, int *l, int espec)
//int para char
{
  int i;
  for (i = 0; i < 2; i++)
    {
      m[2 * i] = l[2 * i] + 'a';
      m[2 * i + 1] = l[2 * i + 1] + '1';
    }
  m[4] = '\0';
//n[2]='-';
//colocar um tracinho no meio
//if(flag_50>1)
// ou xizinho caso capturou
//              n[2]='x';
  switch (espec)
//promocao para 4,5,6,7 == D,C,T,B
    {
    case 4:
//promoveu a Dama
      m[4] = 'q';
      m[5] = '\0';
      break;
    case 5:
//promoveu a Cavalo
      m[4] = 'n';
      m[5] = '\0';
      break;
    case 6:
//promoveu a Torre
      m[4] = 'r';
      m[5] = '\0';
      break;
    case 7:
//promoveu a Bispo
      m[4] = 'b';
      m[5] = '\0';
      break;
    }
}

int
movi2lance (int *l, char *m)
//transforma char de entrada em int
{
  int i;
  for (i = 0; i < 2; i++)
    //as coordenadas origem e destino estao no tabuleiro
    if (m[2 * i] < 'a' || m[2 * i] > 'h' || m[2 * i + 1] < '1'
	|| m[2 * i + 1] > '8')
      //2i e 2i+1
      return (0);
  for (i = 0; i < 2; i++)
    {
      l[2 * i] = (int) (m[2 * i] - 'a');
      //l[0]=m[0]-'a'   l[2]=m[3]-'a'
      l[2 * i + 1] = (int) (m[2 * i + 1]) - '1';
      //l[1]=m[1]-'1'   l[3]=m[4]-'1'
    }
  return (1);
}

inline int
adv (int vez)
{
  return ((-1) * vez);
}

inline int
sinal (int x)
{
  if (!x)
    return (0);
  return (abs (x) / x);
}

void
copitab (tabuleiro * dest, tabuleiro * font)
{
  int i, j;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      dest->tab[i][j] = font->tab[i][j];
  dest->vez = font->vez;
  dest->peao_pulou = font->peao_pulou;
  dest->roqueb = font->roqueb;
  dest->roquep = font->roquep;
  dest->empate_50 = font->empate_50;
  for (i = 0; i < 4; i++)
    dest->lancex[i] = font->lancex[i];
  dest->numero = font->numero;
  dest->especial = font->especial;
  //0:nada. 1:roque pqn. 2:roque grd. 3:comeu enpassant.
  //promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
}

movimento *
geramov (tabuleiro tabu, int *nmovi)
{
  tabuleiro tabaux;
  movimento *pmovi = NULL, *pmoviant = NULL, *cabeca = NULL;
  int i, j, k, l, m, flag;
  int col, lin;
  int peca;
//    (*nmovi)=0; se nmovi==-1, acha um lance e retorna;
  if ((*nmovi) != -1)
    {
      cabeca = (movimento *) malloc (sizeof (movimento));
      if (cabeca == NULL)
	msgsai ("Erro de alocacao de memoria em geramov 1", 1);
      pmovi = cabeca;
      pmoviant = pmovi;
      pmovi->prox = NULL;
    }
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      {
	if ((tabu.tab[i][j]) * (tabu.vez) <= 0)
	  //casa vazia, ou cor errada:
	  continue;
	// (+)*(-)==(-) , (-)*(+)==(-)
	peca = abs (tabu.tab[i][j]);
	switch (peca)
	  {
	  case REI:
	    for (col = i - 1; col <= i + 1; col++)
	      //Rei anda normalmente
	      for (lin = j - 1; lin <= j + 1; lin++)
		{
		  if (lin < 0 || lin > 7 || col < 0 || col > 7)
		    continue;
		  //casa de indice invalido
		  if (sinal (tabu.tab[col][lin]) == tabu.vez)
		    continue;
		  //casa possui peca da mesma cor ou o proprio rei
		  if (ataca (adv (tabu.vez), col, lin, tabu))
		    continue;
		  //adversario esta protegendo a casa
		  copitab (&tabaux, &tabu);
		  tabaux.tab[i][j] = VAZIA;
		  tabaux.tab[col][lin] = REI * tabu.vez;
		  if (!xeque_rei_das (tabu.vez, tabaux))
		    {
		      //nao pode mais fazer roque. Mexeu Rei
		      //Rei capturou peca adversaria. flag=2
		      //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
		      if ((*nmovi) == -1)
			return (movimento *) TRUE;
		      enche_pmovi (pmovi, i, j, col, lin, -1, 0, 0, 2);
		      (*nmovi)++;
		      //preenche a estrutura movimento
		      if (tabu.tab[col][lin] == VAZIA)
			pmovi->flag_50 = 0;
		      //Rei capturou peca adversaria.
		      pmovi->prox = (movimento *) malloc (sizeof (movimento));
		      if (pmovi->prox == NULL)
			msgsai ("Erro de alocacao de memoria em geramov 2",
				2);
		      pmoviant = pmovi;
		      pmovi = pmovi->prox;
		      pmovi->prox = NULL;
		    }
		}
//----------------------------- Roque de Brancas e Pretas
	    if (tabu.vez == brancas)
	      if (tabu.roqueb == 0)
		//Se nao pode mais rocar: break!
		break;
	      else
		{
		  //roque de brancas
		  if (tabu.roqueb != 2 && tabu.tab[7][0] == -TORRE)
		    {
		      //Nao mexeu TR (e ela est� l�! Adv poderia ter comido antes de mexer)
		      // roque pequeno
		      if (tabu.tab[5][0] == VAZIA && tabu.tab[6][0] == VAZIA)
//f1,g1
			{
			  flag = 0;
			  for (k = 4; k < 7; k++)
//colunas e,f,g
			    if (ataca (pretas, k, 0, tabu))
			      {
				flag = 1;
				break;
			      }
			  if (flag == 0)
			    {
//nao pode mais fazer roque. Mexeu Rei
//Rei rocou pqn, espec=1. flag=0
//enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			      if ((*nmovi) == -1)
				return (movimento *) TRUE;
			      enche_pmovi (pmovi, 4, 0, 6, 0, -1, 0, 1, 0);
			      (*nmovi)++;
			      pmovi->prox =
				(movimento *) malloc (sizeof (movimento));
			      if (pmovi->prox == NULL)
				msgsai
				  ("Erro de alocacao de memoria em geramov 3",
				   3);
			      pmoviant = pmovi;
			      pmovi = pmovi->prox;
			      pmovi->prox = NULL;
			    }
			}
// roque grande
		    }
//mexeu TR
		  if (tabu.roqueb != 3 && tabu.tab[0][0] == -TORRE)
//Nao mexeu TD (e a torre existe!)
		    {
		      if (tabu.tab[1][0] == VAZIA && tabu.tab[2][0] == VAZIA
			  && tabu.tab[3][0] == VAZIA)
//b1,c1,d1
			{
			  flag = 0;
			  for (k = 2; k < 5; k++)
//colunas c,d,e
			    if (ataca (pretas, k, 0, tabu))
			      {
				flag = 1;
				break;
			      }
			  if (flag == 0)
			    {
			      //nao pode mais fazer roque. Mexeu Rei
			      //Rei rocou grd, espec=2. flag=0
			      //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			      if ((*nmovi) == -1)
				return (movimento *) TRUE;
			      enche_pmovi (pmovi, 4, 0, 2, 0, -1, 0, 2, 0);
			      (*nmovi)++;
			      //preenche a estrutura movimento
			      pmovi->prox =
				(movimento *) malloc (sizeof (movimento));
			      if (pmovi->prox == NULL)
				msgsai
				  ("Erro de alocacao de memoria em geramov 4",
				   4);
			      pmoviant = pmovi;
			      pmovi = pmovi->prox;
			      pmovi->prox = NULL;
			    }
			}
		    }
//mexeu TD
		}
	    else
// vez das pretas jogarem
	    if (tabu.roquep == 0)
//Se nao pode rocar: break!
	      break;
	    else
//roque de pretas
	      {
		if (tabu.roquep != 2 && tabu.tab[7][7] == TORRE)
//Nao mexeu TR (e a torre n�o foi capturada)
		  {
// roque pequeno
		    if (tabu.tab[5][7] == VAZIA && tabu.tab[6][7] == VAZIA)
//f8,g8
		      {
			flag = 0;
			for (k = 4; k < 7; k++)
//colunas e,f,g
			  if (ataca (brancas, k, 7, tabu))
			    {
			      flag = 1;
			      break;
			    }
			if (flag == 0)
			  {
			    //nao pode mais fazer roque. Mexeu Rei
			    //Rei rocou pqn, espec=1. flag=0.
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, 4, 7, 6, 7, -1, 0, 1, 0);
			    (*nmovi)++;
			    //preenche a estrutura movimento
			    pmovi->prox =
			      (movimento *) malloc (sizeof (movimento));
			    if (pmovi->prox == NULL)
			      msgsai
				("Erro de alocacao de memoria em geramov 5",
				 5);
			    pmoviant = pmovi;
			    pmovi = pmovi->prox;
			    pmovi->prox = NULL;
			  }
		      }
// roque grande.
		  }
//mexeu TR
		if (tabu.roquep != 3 && tabu.tab[0][7] == TORRE)
//Nao mexeu TD (e a torre est� l�)
		  {
		    if (tabu.tab[1][7] == VAZIA && tabu.tab[2][7] == VAZIA &&
			tabu.tab[3][7] == VAZIA)
//b8,c8,d8
		      {
			flag = 0;
			for (k = 2; k < 5; k++)
//colunas c,d,e
			  if (ataca (brancas, k, 7, tabu))
			    {
			      flag = 1;
			      break;
			    }
			if (flag == 0)
			  {
			    //nao pode mais fazer roque. Mexeu Rei
			    //Rei rocou grd, espec=2. flag=0.
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, 4, 7, 2, 7, -1, 0, 2, 0);
			    (*nmovi)++;
			    //preenche a estrutura movimento
			    pmovi->prox =
			      (movimento *) malloc (sizeof (movimento));
			    if (pmovi->prox == NULL)
			      msgsai
				("Erro de alocacao de memoria em geramov 6",
				 6);
			    pmoviant = pmovi;
			    pmovi = pmovi->prox;
			    pmovi->prox = NULL;
			  }
		      }
		  }
//mexeu TD
	      }
	    break;
	  case PEAO:
//peao comeu en passant
	    if (tabu.peao_pulou != -1)
//Existe peao do adv que pulou...
	      {
		if (i == tabu.peao_pulou - 1 || i == tabu.peao_pulou + 1)
//este peao esta na coluna certa
		  {
		    copitab (&tabaux, &tabu);
// tabaux = tabu
		    if (tabu.vez == brancas)
		      {
			if (j == 4)
//E tambem esta na linha certa. Peao branco vai comer enpassant!
			  {
			    tabaux.tab[tabu.peao_pulou][5] = -PEAO;
			    tabaux.tab[tabu.peao_pulou][4] = VAZIA;
			    tabaux.tab[i][4] = VAZIA;
			    if (!xeque_rei_das (brancas, tabaux))
			      //nao deixa rei branco em xeque
			      {
				//enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
				if ((*nmovi) == -1)
				  return (movimento *) TRUE;
				enche_pmovi (pmovi, i, j, tabu.peao_pulou, 5,
					     -1, 1, 3, 3);
				(*nmovi)++;
				//preenche a estrutura movimento
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 7",
				     7);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			  }
		      }
		    else
// vez das pretas.
		      {
			if (j == 3)
// Esta na linha certa. Peao preto vai comer enpassant!
			  {
			    tabaux.tab[tabu.peao_pulou][2] = PEAO;
			    tabaux.tab[tabu.peao_pulou][3] = VAZIA;
			    tabaux.tab[i][3] = VAZIA;
			    if (!xeque_rei_das (pretas, tabaux))
			      {
				//enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
				if ((*nmovi) == -1)
				  return (movimento *) TRUE;
				enche_pmovi (pmovi, i, j, tabu.peao_pulou, 2,
					     -1, 1, 3, 3);
				(*nmovi)++;
				//preenche a estrutura movimento
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 8",
				     8);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			    //deixa rei em xeque
			  }
			//nao esta na fila correta
		      }
		    //fim do else da vez
		  }
		//nao esta na coluna adjacente ao peao_pulou
	      }
	    //nao peao_pulou.
	    //peao andando uma casa
	    copitab (&tabaux, &tabu);
	    //tabaux = tabu;
	    if (tabu.vez == brancas)
	      {
		if (j + 1 < 8)
		  {
		    if (tabu.tab[i][j + 1] == VAZIA)
		      {
			tabaux.tab[i][j] = VAZIA;
			tabaux.tab[i][j + 1] = -PEAO;
			if (!xeque_rei_das (brancas, tabaux))
			  {
			    k = 7;
			    //4:dama, 5:cavalo, 6:torre, 7:bispo
			  promoveb:
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, i, j, i, j + 1, -1, 1, 0, 1);
			    (*nmovi)++;
			    //preenche a estrutura movimento
			    if (j + 1 == 7)
			      //se promoveu
			      {
				pmovi->especial = k;
				k--;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 9",
				     9);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
				if (k > 3)
				  goto promoveb;
			      }
			    else
//nao promoveu
			      {
				pmovi->especial = 0;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 10",
				     10);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			  }
//deixa rei em xeque
		      }
		    //casa ocupada
		  }
		//passou da casa de promocao e caiu do tabuleiro!
	      }
	    else
	      //vez das pretas andar com peao uma casa
	      {
		if (j - 1 > -1)
		  {
		    if (tabu.tab[i][j - 1] == VAZIA)
		      {
			tabaux.tab[i][j] = VAZIA;
			tabaux.tab[i][j - 1] = PEAO;
			if (!xeque_rei_das (pretas, tabaux))
			  {
			    k = 7;
			    //4:dama, 5:cavalo, 6:torre, 7:bispo
			  promovep:
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    //preenche a estrutura movimento
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, i, j, i, j - 1, -1, 1, 0, 1);
			    (*nmovi)++;
			    //movimento de peao
			    if (j - 1 == 0)
			      //se promoveu
			      {
				pmovi->especial = k;
				k--;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 11",
				     11);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
				if (k > 3)
				  goto promovep;
			      }
			    else
//nao promoveu
			      {
				pmovi->especial = 0;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 12",
				     12);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			  }
//deixa rei em xeque
		      }
//casa ocupada
		  }
//passou da casa de promocao e caiu do tabuleiro!
	      }
//peao anda duas casas
	    copitab (&tabaux, &tabu);
	    if (tabu.vez == brancas)
//vez das brancas
	      {
		if (j == 1)
//esta na linha inicial
		  {
		    if (tabu.tab[i][2] == VAZIA && tabu.tab[i][3] == VAZIA)
		      {
			tabaux.tab[i][1] = VAZIA;
			tabaux.tab[i][3] = -PEAO;
			if (!xeque_rei_das (brancas, tabaux))
			  {
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    //preenche a estrutura movimento
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, i, 1, i, 3, i, 1, 0, 1);
			    (*nmovi)++;
			    pmovi->prox =
			      (movimento *) malloc (sizeof (movimento));
			    if (pmovi->prox == NULL)
			      msgsai
				("Erro de alocacao de memoria em geramov 13",
				 13);
			    pmoviant = pmovi;
			    pmovi = pmovi->prox;
			    pmovi->prox = NULL;
			  }
//deixa rei em xeque
		      }
//alguma das casas esta ocupada
		  }
//este peao ja mexeu antes
	      }
	    else
//vez das pretas andar com peao 2 casas
	      {
		if (j == 6)
//esta na linha inicial
		  {
		    if (tabu.tab[i][5] == VAZIA && tabu.tab[i][4] == VAZIA)
		      {
			tabaux.tab[i][6] = VAZIA;
			tabaux.tab[i][4] = PEAO;
			if (!xeque_rei_das (pretas, tabaux))
			  {
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    //preenche a estrutura movimento
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, i, 6, i, 4, i, 1, 0, 1);
			    (*nmovi)++;
			    pmovi->prox =
			      (movimento *) malloc (sizeof (movimento));
			    if (pmovi->prox == NULL)
			      msgsai
				("Erro de alocacao de memoria em geramov 14",
				 14);
			    pmoviant = pmovi;
			    pmovi = pmovi->prox;
			    pmovi->prox = NULL;
			  }
//deixa rei em xeque
		      }
//alguma das casas esta ocupada
		  }
//este peao ja mexeu antes
	      }
// peao comeu normalmente (i.e., nao eh en passant)
	    copitab (&tabaux, &tabu);
	    if (tabu.vez == brancas)
//vez das brancas
	      {
		k = i - 1;
		if (k < 0)
		  k = i + 1;
		do
//diagonal esquerda e direita do peao.
		  {
		    //peca preta
		    if (tabu.tab[k][j + 1] > 0)
		      {
			tabaux.tab[i][j] = VAZIA;
			tabaux.tab[k][j + 1] = -PEAO;
			if (!xeque_rei_das (brancas, tabaux))
			  {
			    l = 7;
//4:dama, 5:cavalo, 6:torre, 7:bispo
			  comoveb:
			    //peao branco promove comendo;
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    //preenche a estrutura movimento
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, i, j, k, j + 1, -1, 1, 0, 3);
			    (*nmovi)++;
			    if (j + 1 == 7)
			      //se promoveu comendo
			      {
				pmovi->especial = l;
				//4,5,6,7 == D,C,T,Bispo
				l--;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 15",
				     15);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
				if (l > 3)
				  goto comoveb;
			      }
			    else
//comeu sem promover
			      {
				pmovi->especial = 0;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 16",
				     16);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			  }
//deixa rei em xeque
		      }
//nao tem peca preta para comer
		    k += 2;
//proxima diagonal do peao
		  }
		while (k <= i + 1 && k < 8);

	      }
// vez das pretas comer normalmente (i.e., nao eh en passant)
	    else
	      {
		k = i - 1;
		if (k < 0)
		  k = i + 1;
		do
//diagonal esquerda e direita do peao.
		  {
		    if (tabu.tab[k][j - 1] < 0)
//peca branca
		      {
			tabaux.tab[i][j] = VAZIA;
			tabaux.tab[k][j - 1] = PEAO;
			if (!xeque_rei_das (pretas, tabaux))
			  {
			    l = 7;
//4:dama, 5:cavalo, 6:torre, 7:bispo
			  comovep:
			    //peao preto promove comendo
			    //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			    //preenche a estrutura movimento
			    if ((*nmovi) == -1)
			      return (movimento *) TRUE;
			    enche_pmovi (pmovi, i, j, k, j - 1, -1, 1, 0, 3);
			    (*nmovi)++;
			    if (j - 1 == 0)
			      //se promoveu comendo
			      {
				pmovi->especial = l;
				//4,5,6,7 == D,C,T,Bispo
				l--;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 17",
				     17);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
				if (l > 3)
				  goto comovep;
			      }
			    else
//comeu sem promover
			      {
				pmovi->especial = 0;
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 18",
				     18);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			  }
//rei em xeque
		      }
//nao tem peao branco para comer
		    k += 2;
		  }
		while (k <= i + 1 && k < 8);

	      }
	    break;
	  case CAVALO:
	    for (col = -2; col < 3; col++)
	      for (lin = -2; lin < 3; lin++)
		{
		  if (abs (col) == abs (lin) || col == 0 || lin == 0)
		    continue;
		  if (i + col < 0 || i + col > 7 || j + lin < 0
		      || j + lin > 7)
		    continue;
		  if (sinal (tabu.tab[i + col][j + lin]) == tabu.vez)
		    continue;
//casa possui peca da mesma cor.
		  copitab (&tabaux, &tabu);
		  tabaux.tab[i][j] = VAZIA;
		  tabaux.tab[i + col][j + lin] = CAVALO * tabu.vez;
		  if (!xeque_rei_das (tabu.vez, tabaux))
		    {
		      //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
		      //preenche a estrutura movimento
		      if ((*nmovi) == -1)
			return (movimento *) TRUE;
		      enche_pmovi (pmovi, i, j, col + i, lin + j, -1, 1, 0,
				   2);
		      (*nmovi)++;
		      if (tabu.tab[col + i][lin + j] == VAZIA)
//Cavalo nao capturou
			pmovi->flag_50 = 0;
// Cavalo capturou peca adversaria.
		      pmovi->prox = (movimento *) malloc (sizeof (movimento));
		      if (pmovi->prox == NULL)
			msgsai ("Erro de alocacao de memoria em geramov 19",
				19);
		      pmoviant = pmovi;
		      pmovi = pmovi->prox;
		      pmovi->prox = NULL;
		    }
//deixa rei em xeque
		}
	    break;
	  case DAMA:
	  case TORRE:
	  case BISPO:
//Anda reto - So nao o bispo
	    if (peca != BISPO)
	      {
		for (k = -1; k < 2; k += 2)
//k= -1, 1
		  {
		    col = i + k;
		    lin = j + k;
//l=flag da horizontal para a primeira peca que trombar
		    l = 0;
		    m = 0;
//m=idem para a vertical
		    do
		      {
			if (col >= 0 && col <= 7
			    && sinal (tabu.tab[col][j]) != tabu.vez && l == 0)
//gira col, mantem lin
			  {
//inclui esta casa na lista
			    copitab (&tabaux, &tabu);
			    tabaux.tab[i][j] = VAZIA;
			    tabaux.tab[col][j] = peca * tabu.vez;
			    if (!xeque_rei_das (tabu.vez, tabaux))
			      {
				//enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
				//preenche a estrutura movimento
				if ((*nmovi) == -1)
				  return (movimento *) TRUE;
				enche_pmovi (pmovi, i, j, col, j, -1, 1, 0,
					     0);
				(*nmovi)++;
				if (peca == TORRE)
				  //mas, se for torre ...
				  {
				    if (tabu.vez == brancas && j == 0)
				      //se for vez das brancas e linha 1
				      {
					if (i == 0)
					  //e coluna a
					  pmovi->roque = 3;
					//mexeu TD
					if (i == 7)
					  //ou coluna h
					  pmovi->roque = 2;
					//mexeu TR
				      }
				    if (tabu.vez == pretas && j == 7)
				      //se for vez das pretas e linha 8
				      {
					if (i == 0)
					  //e coluna a
					  pmovi->roque = 3;
					//mexeu TD
					if (i == 7)
					  //ou coluna h
					  pmovi->roque = 2;
					//mexeu TR
				      }
				  }
				//pmovi->especial=0;
				if (tabu.tab[col][j] != VAZIA)
				  //pmovi->flag_50=0;
				  //else
				  {
				    pmovi->flag_50 = 2;
				    //Dama ou Torre capturou peca adversaria.
				    l = 1;
				  }
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 20",
				     20);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			    else
//deixa rei em xeque
			    if (tabu.tab[col][j] != VAZIA)
//alem de nao acabar o xeque, nao pode mais seguir nessa dire��o pois tem pe�a
			      l = 1;
			  }
			else
//nao inclui mais nenhuma casa desta linha; A casa esta fora do tabuleiro, ou tem peca de mesma cor ou capturou
			  l = 1;
			if (lin >= 0 && lin <= 7
			    && sinal (tabu.tab[i][lin]) != tabu.vez && m == 0)
//gira lin, mantem col
			  {
//inclui esta casa na lista
			    copitab (&tabaux, &tabu);
			    tabaux.tab[i][j] = VAZIA;
			    tabaux.tab[i][lin] = peca * tabu.vez;
			    if (!xeque_rei_das (tabu.vez, tabaux))
			      {
				//enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
				//preenche a estrutura movimento
				if ((*nmovi) == -1)
				  return (movimento *) TRUE;
				enche_pmovi (pmovi, i, j, i, lin, -1, 1, 0,
					     0);
				(*nmovi)++;
				if (peca == TORRE)
				  //mas, se for torre ...
				  {
				    if (tabu.vez == brancas && j == 0)
				      //se for vez das brancas e linha 1
				      {
					if (i == 0)
					  //e coluna a
					  pmovi->roque = 3;
					//mexeu TD
					if (i == 7)
					  //ou coluna h
					  pmovi->roque = 2;
					//mexeu TR
				      }
				    if (tabu.vez == pretas && j == 7)
				      //se for vez das pretas e linha 8
				      {
					if (i == 0)
					  //e coluna a
					  pmovi->roque = 3;
					//mexeu TD
					if (i == 7)
					  //ou coluna h
					  pmovi->roque = 2;
					//mexeu TR
				      }
				  }
				if (tabu.tab[i][lin] != VAZIA)
				  {
				    pmovi->flag_50 = 2;
//Dama ou Torre capturou peca adversaria.
				    m = 1;
				  }
				pmovi->prox =
				  (movimento *) malloc (sizeof (movimento));
				if (pmovi->prox == NULL)
				  msgsai
				    ("Erro de alocacao de memoria em geramov 21",
				     21);
				pmoviant = pmovi;
				pmovi = pmovi->prox;
				pmovi->prox = NULL;
			      }
			    else
//deixa rei em xeque
			    if (tabu.tab[i][lin] != VAZIA)
//alem de nao acabar o xeque, nao pode mais seguir nessa dire��o pois tem pe�a
			      m = 1;
			  }
			else
//nao inclui mais nenhuma casa desta coluna; A casa esta fora do tabuleiro, ou tem peca de mesma cor ou capturou
			  m = 1;
			col += k;
			lin += k;
		      }
		    while (((col >= 0 && col <= 7) || (lin >= 0 && lin <= 7))
			   && (m == 0 || l == 0));

		  }
//roda o incremento
	      }
//a peca era bispo
	    if (peca != TORRE)
//anda na diagonal
	      {
		col = i;
		lin = j;
		flag = 0;
		for (k = -1; k < 2; k += 2)
		  for (l = -1; l < 2; l += 2)
		    {
		      col += k;
		      lin += l;
		      while (col >= 0 && col <= 7 && lin >= 0 && lin <= 7
			     && sinal (tabu.tab[col][lin]) != tabu.vez
			     && flag == 0)
			{
//inclui esta casa na lista
			  copitab (&tabaux, &tabu);
			  tabaux.tab[i][j] = VAZIA;
			  tabaux.tab[col][lin] = peca * tabu.vez;
			  if (!xeque_rei_das (tabu.vez, tabaux))
			    {
			      //enche_pmovi(pmovi, c0, c1, c2, c3, peao, roque, espec, flag);
			      //preenche a estrutura movimento
			      if ((*nmovi) == -1)
				return (movimento *) TRUE;
			      enche_pmovi (pmovi, i, j, col, lin, -1, 1, 0,
					   0);
			      (*nmovi)++;
			      if (tabu.tab[col][lin] != VAZIA)
				{
				  pmovi->flag_50 = 2;
				  //Dama ou Bispo capturou peca adversaria.
				  flag = 1;
				}
			      pmovi->prox =
				(movimento *) malloc (sizeof (movimento));
			      if (pmovi->prox == NULL)
				msgsai
				  ("Erro de alocacao de memoria em geramov 22",
				   22);
			      pmoviant = pmovi;
			      pmovi = pmovi->prox;
			      pmovi->prox = NULL;
			    }
			  else
//deixa rei em xeque
			  if (tabu.tab[col][lin] != VAZIA)
//alem de nao acabar o xeque, nao pode mais seguir nessa dire��o pois tem pe�a
			    flag = 1;
			  col += k;
			  lin += l;
			}
//rodou toda a diagonal, ate tab acabar ou achar peca da mesma cor ou capturar
		      col = i;
		      lin = j;
		      flag = 0;
		    }
//for
	      }
//peca era torre
	  case VAZIA:
	    break;
	  case NULA:
	  default:
	    msgsai ("Achei uma peca esquisita! Geramov, default.", 23);
	  }
// switch(peca)
      }
//loop das pecas todas do tabuleiro
  if (cabeca == pmovi)
//se a lista esta vazia
    {
      free (cabeca);
      return (NULL);
    }
  else
//ordena e retorna! flag_50: 0-nada, 1-peao andou, 2-captura, 3-peao capturou
    {
//                                               especial: 0-nada, roque 1-pqn, 2-grd, 3-enpassant
//                                               4,5,6,7 promocao de Dama, Cavalo, Torre, Bispo
      free (pmovi);
      pmoviant->prox = NULL;
      pmoviant = cabeca;
      pmovi = cabeca->prox;
      while (pmovi != NULL)
//percorre, passa para cabeca tudo de interessante.
	{
	  if (pmovi->flag_50 > 1 || pmovi->especial)
//achou captura ou especial
	    {
//move para a cabeca da lista!
	      pmoviant->prox = pmovi->prox;
//o anterior aponta para o prox.
	      pmovi->prox = cabeca;
//o do meio aponta para o primeiro.
	      cabeca = pmovi;
//o cabeca agora eh pmovi!
	      pmovi = pmoviant->prox;
	    }
	  else
	    {
	      pmoviant = pmovi;
	      pmovi = pmovi->prox;
	    }
	}
//fim do while, e consequentemente da lista
      return (cabeca);
    }
//fim do else da lista vazia.
}

//fim de   ----------- movimento *geramov(tabuleiro tabu)

int
ataca (int cor, int col, int lin, tabuleiro tabu)
{
//retorna verdadeiro (1) ou falso (0)
//cor==brancas   => brancas atacam casa(col,lin)
//cor==pretas    => pretas  atacam casa(col,lin)
  int icol, ilin, casacol, casalin;
//torre ou dama atacam a casa...
  for (icol = col - 1; icol >= 0; icol--)
//desce coluna
    {
      if (tabu.tab[icol][lin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[icol][lin] == -TORRE || tabu.tab[icol][lin] == -DAMA)
	  return (1);
	else
	  break;
      else
// pretas atacam a casa
      if (tabu.tab[icol][lin] == TORRE || tabu.tab[icol][lin] == DAMA)
	return (1);
      else
	break;
    }
  for (icol = col + 1; icol < 8; icol++)
//sobe coluna
    {
      if (tabu.tab[icol][lin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[icol][lin] == -TORRE || tabu.tab[icol][lin] == -DAMA)
	  return (1);
	else
	  break;
      else if (tabu.tab[icol][lin] == TORRE || tabu.tab[icol][lin] == DAMA)
	return (1);
      else
	break;
    }
  for (ilin = lin + 1; ilin < 8; ilin++)
// direita na linha
    {
      if (tabu.tab[col][ilin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[col][ilin] == -TORRE || tabu.tab[col][ilin] == -DAMA)
	  return (1);
	else
	  break;
      else if (tabu.tab[col][ilin] == TORRE || tabu.tab[col][ilin] == DAMA)
	return (1);
      else
	break;
    }
  for (ilin = lin - 1; ilin >= 0; ilin--)
// esquerda na linha
    {
      if (tabu.tab[col][ilin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[col][ilin] == -TORRE || tabu.tab[col][ilin] == -DAMA)
	  return (1);
	else
	  break;
      else if (tabu.tab[col][ilin] == TORRE || tabu.tab[col][ilin] == DAMA)
	return (1);
      else
	break;
    }
// cavalo ataca casa...
  for (icol = -2; icol < 3; icol++)
    for (ilin = -2; ilin < 3; ilin++)
      {
	if (abs (icol) == abs (ilin) || icol == 0 || ilin == 0)
	  continue;
	if (col + icol < 0 || col + icol > 7 || lin + ilin < 0
	    || lin + ilin > 7)
	  continue;
	if (cor == brancas)
	  if (tabu.tab[col + icol][lin + ilin] == -CAVALO)
	    return (1);
	  else
	    continue;
	else if (tabu.tab[col + icol][lin + ilin] == CAVALO)
	  return (1);
      }
// bispo ou dama atacam casa...
  for (icol = -1; icol < 2; icol += 2)
    for (ilin = -1; ilin < 2; ilin += 2)
      {
	casacol = col;
//para cada diagonal, comece na casa origem.
	casalin = lin;
	do
	  {
	    casacol = casacol + icol;
	    casalin = casalin + ilin;
	    if (casacol < 0 || casacol > 7 || casalin < 0 || casalin > 7)
	      break;
	  }
	while (tabu.tab[casacol][casalin] == VAZIA);

	if (casacol >= 0 && casacol <= 7 && casalin >= 0 && casalin <= 7)
	  if (cor == brancas)
	    if (tabu.tab[casacol][casalin] == -BISPO
		|| tabu.tab[casacol][casalin] == -DAMA)
	      return (1);
	    else
	      continue;
// achou peca, mas esta nao anda em diagonal ou e' peca propria
	  else
	    if (tabu.tab[casacol][casalin] == BISPO
		|| tabu.tab[casacol][casalin] == DAMA)
	    return (1);
      }
// proxima diagonal
// ataque de rei...
  for (icol = col - 1; icol <= col + 1; icol++)
    for (ilin = lin - 1; ilin <= lin + 1; ilin++)
      {
	if (icol == col && ilin == lin)
	  continue;
	if (icol < 0 || icol > 7 || ilin < 0 || ilin > 7)
	  continue;
	if (cor == brancas)
	  if (tabu.tab[icol][ilin] == -REI)
	    return (1);
	  else
	    continue;
	else if (tabu.tab[icol][ilin] == REI)
	  return (1);
      }
  if (cor == brancas)
// ataque de peao branco
    {
      if (lin > 1)
	{
	  ilin = lin - 1;
	  if (col - 1 >= 0)
	    if (tabu.tab[col - 1][ilin] == -PEAO)
	      return (1);
	  if (col + 1 <= 7)
	    if (tabu.tab[col + 1][ilin] == -PEAO)
	      return (1);
	}
    }
  else
//ataque de peao preto
    {
      if (lin < 6)
	{
	  ilin = lin + 1;
	  if (col - 1 >= 0)
	    if (tabu.tab[col - 1][ilin] == PEAO)
	      return (1);
	  if (col + 1 <= 7)
	    if (tabu.tab[col + 1][ilin] == PEAO)
	      return (1);
	}
    }
  return (0);
}

//fim de int ataca(int cor, int col, int lin, tabuleiro tabu)
int
xeque_rei_das (int cor, tabuleiro tabu)
{
  int ilin, icol;
  for (ilin = 0; ilin < 8; ilin++)
//roda linha
    for (icol = 0; icol < 8; icol++)
//roda coluna
      if (tabu.tab[icol][ilin] == (REI * cor))
//achou o rei
	if (ataca (adv (cor), icol, ilin, tabu))
//alguem ataca o rei
	  return (1);
	else
	  return (0);
//ninguem ataca o rei
  return (0);
//nao achou o rei!!
}

// fim de int xeque_rei_das(int cor, tabuleiro tabu)

char
humajoga (tabuleiro * tabu)
//----------------------------------------------
{
  char movinito[80];
//movimento ou comando entrado pelo usu�rio ou XBoard/WinBoard
  movimento *pval = 0;
  char res;
//    char aux;
  char peca;
  int tente, lanc[4], moves, minutes, incre;
  int i, j, k;
  int casacor;
//nao precisa se tem textbackground
  listab *plaux;
  char pieces[80],
    color[80], castle[80], enpassant[80], halfmove[80], fullmove[80];
//para testar uma posi��o
  int testasim = 0, estat;
  movinito[79] = '\0';
  do
    {
      tente = 0;
      if (!xboard)
	printf ("\n\nComando: ");
      scanf ("%s", movinito);	//---------------- (*quase) Toda entrada do Winboard
      clock1 = clock () * 100 / CLOCKS_PER_SEC;	// retorna cloock1 em centesimos de segundos...
      inputcheckclock = clock1;
//cloock1 marca o tempo do jogo do advers�rio. difcloock=cloock2-cloock1. � long int %ld
      difclock = 0;
      fflush (fsaida);		//grava o arquivo atual, para o caso de bug, poder ler algo
      fclose (fsaida);
      fsaida = fopen (flog, "a");
      if (!fsaida)
	debug = 0;
      if (debug)
	fprintf (fsaida, "winboard: %s\n", movinito);
      if (!strcmp (movinito, "lista") && !xboard)
	{
	  mostra_lances (*tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "hint") && xboard)
// Dica
	{
	  mostra_lances (*tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "force") && xboard)
// nao joga, apenas acompanha
	{
	  primeiro = 'h';
	  segundo = 'h';
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "undo"))
// volta um movimento (ply)
	{
	  primeiro = 'h';
	  segundo = 'h';
	  volta_lance (tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "remove"))
// volta dois movimentos == 1 lance
	{
	  volta_lance (tabu);
	  volta_lance (tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "post") && xboard)
	{
//showthinking ou mostrapensando
	  analise = 0;
	  mostrapensando = 1;
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "nopost") && xboard)
	{
//no show thinking ou desliga o mostrapensando
	  mostrapensando = 0;
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "analyze") && xboard)
// analise inicia
	{
	  if (analise == -1)
	    {
	      analise = 0;
	      tente = 1;
	      continue;
	    }
	  analise = 1;
	  mostrapensando = 0;
	  primeiro = 'h';
	  segundo = 'h';
	  disc = analisa (tabu);
//disc=vari�vel de descarte
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "exit") && xboard)
// analise termina
	{
	  analise = 0;
//            testasim=0; nao � necess�ria, pois � vari�vel local.
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "empate") || !strcmp (movinito, "draw"))
//aceitar empate? (o outro est� oferecendo)
	{
	  if (primeiro == segundo && primeiro == 'h')
//humano contra humano, aceita sempre
	    return ('c');
	  else
//humano contra computador ou computador contra computador
	    {
	      estat = -estatico (*tabu, 0, 0, -LIMITE, +LIMITE);
//ao receber proposta, o valor � calculado em fun��o de quem � a vez.
// 0:nao � por falta de tempo. 0: nivel do tabuleiro real
//se comp. versus humano: estamos na fun��o "humano-joga", logo a vez � do humano!
//quando vc est� perdendo e pede empate, eh sua vez, e portanto o valor
//que estat retorna � negativo (vc est� perdendo)
//O computador precisa avaliar esse resultado com sinal trocado, pois ele est� ganhando
//Se o computador (positivo) < Limite QUANTO_EMPATE1, ent�o ele aceita.
//O detalhe � que o computador s� aceita se ele estiver perdendo dois pe�es.
//Ou seja, voc� precisa estar positivo em 20 pontos, para o computador aceitar
	      if (estat < QUANTO_EMPATE1)	// && !COMPUTER)
//bugbug deve aceitar de outro computer se est� perdendo!
//so se perdendo 2 pe�es (e n�o est� jogando contra outra engine)
		{
		  if (debug)
		    fprintf (fsaida,
			     "xadreco : draw accepted (HumaJ-1) %d  turn: %d\n",
			     estat, tabu->vez);
		  return ('c');
		}
	      else
		{
		  if (debug)
		    fprintf (fsaida,
			     "xadreco : draw rejected (HumaJ-2) %d  turn: %d\n",
			     estat, tabu->vez);
		  if (!xboard)
		    {
		      disc =
			pega (" ",
			      "\nEmpate nao foi aceito. Jogue!\nTecle <ESPACO>+<ENTER>");
		      imptab (*tabu);
		    }
		  tente = 1;
		  continue;
		}
	    }
	}
      if (!strcmp (movinito, "versao") || !strcmp (movinito, "version"))
	{
	  if (!xboard)
	    {
	      printf ("\nXadreco vers�o %.1f para XBoard/WinBoard", version);
	      disc =
		pega (" ",
		      "\nBaseado no Algoritmo Minimax.\n\n		Por Ruben Carlo Benante, 22/10/04.\nTecle <ESPACO>+<ENTER>");
	      imptab (*tabu);
	    }
	  else
	    {
	      if (debug)
		fprintf (fsaida,
			 "tellopponent Xadreco v.%.1f para XBoard/WinBoard, baseado no Algoritmo Minimax, por Ruben Carlo Benante, 22/10/04.\n",
			 version);
	      printf
		("tellopponent Xadreco v.%.1f para XBoard/WinBoard, baseado no Algoritmo Minimax, por Ruben Carlo Benante, 22/10/04.\n",
		 version);
	    }
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "sd") && xboard)
//set deep: coloca o n�vel de profundidade. Nao est� sendo usado. Implementar.
	{
	  nivel = (int) (movinito[3] - '0');
	  nivel = (nivel > 6 || nivel < 0) ? 2 : nivel;
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "nivel") && !xboard)
	{
	  printf
	    ("\n\nQual o tempo de jogo? MOVIMENTOS SEGUNDOS INCREMENTO: ");
	  scanf ("%d %d %d", &moves, &minutes, &incre);
	  if (moves <= 0)
	    moves = 100;
	  if (minutes <= 0)
	    minutes = 2;
	  incre -= 2;
	  if (incre < 0)
	    incre = 0;
	  tempomovclock = minutes * 6000 / moves + incre * 100;
//para campeonato, tem que jogar mais r�pido!
	  imptab (*tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "go"))
//troca de lado e joga. (o mesmo que "adv")
	{
	  if (tabu->vez == brancas)
	    {
	      primeiro = 'c';
	      segundo = 'h';
	    }
	  else
	    {
	      primeiro = 'h';
	      segundo = 'c';
	    }
	  return ('y');
//returna para jogar ou humano ou computador.
	}
      if (!strcmp (movinito, "playother"))
//coloca o computador com a cor que n�o � a da vez.
	{
	  if (tabu->vez == brancas)
	    {
	      primeiro = 'h';
	      segundo = 'c';
	    }
	  else
	    {
	      primeiro = 'c';
	      segundo = 'h';
	    }
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "computer") && xboard)
//altera estrat�gia para jogar contra outra engine;
	{
	  ofereci = 2;
//pode oferecer mais empates
	  ABANDONA = -LIMITE;
//n�o abandona nunca
	  COMPUTER = 1;
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "adv") && !xboard)
	{
	  printf ("\n\n");
	  primeiro =
	    pega ("ch", "\nQuem joga de brancas (c:Computador, h:Humano)? ");
	  segundo =
	    pega ("ch", "\nQuem joga de  pretas (c:Computador, h:Humano)? ");
	  if (primeiro == segundo && primeiro == 'c')
	    pausa = pega ("sn", "\nPausa entre os lances? (s/n): ");
	  else
	    pausa = 'n';
	  return ('y');
//se for comp. ou humano, gira o la�o e joga quem for.
	}
      if (!strcmp (movinito, "new"))
//novo jogo
	return ('w');
      if (!strcmp (movinito, "desisto") || !strcmp (movinito, "resign"))
	if (tabu->vez == pretas)
	  return ('b');
//pretas abandonam
	else
	  return ('B');
//brancas abandonam
      if (!strcmp (movinito, "quit"))
	msgsai ("Obrigado por jogar com o Xadreco.", 0);
//vai para o tela ou para o log
// controle de tempo:
//level 100 2 0
//100 moves
//2 min
//0 inc
//= 1200 milisec per mov
//
//
//level 40 5 12
//40 moves
//5 min
//12 inc
//= 7500 milisec per mov + 12 inc
//
//level 0 5 0
//0 all moves = 100 moves
//5 min
//0 inc
//= 3000 milisec per mov
      if (!strcmp (movinito, "level") && xboard)
	{
	  scanf ("%s", movinito);
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  moves = atoi (movinito);
	  scanf ("%s", movinito);
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  minutes = atoi (movinito);
	  scanf ("%s", movinito);
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  incre = atoi (movinito);
	  if (moves <= 0)
	    moves = 100;
	  if (minutes <= 0)
	    minutes = 2;
	  incre -= 2;
	  if (incre < 0)
	    incre = 0;
	  tempomovclock = minutes * 6000 / moves + incre * 100;
//            tempomovcloock/=2;
//apesar de correto, est� gastando o dobro do tempo. Problema com as rotinas de apoio? Problema com o minimax que n�o sabe retornar?
	  if (debug)
	    fprintf (fsaida, "xadreco : st %.2f\n",
		     (float) tempomovclock / 100.0);
//        printf ("st %d\n", tempomovclock / 100); //esse comando n�o � da engine para o xboard, mas sim o inverso.
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "setboard"))
//funciona no prompt tamb�m
	{
	  if (setboard == -1)
	    {
	      setboard = 0;
	      tente = 1;
	      continue;
	    }
	  inicia (tabu);
	  setboard = 1;
//o jogo � editado
//Posi��o FEN
	  scanf ("%s", movinito);
//trava se colocar uma posi��o FEN inv�lida!
	  if (!strcmp (movinito, "testpos"))
	    {
	      testasim = 1;
//a posi��o vem da fun��o testapos, e n�o de scanf()
	      testapos (pieces, color, castle, enpassant, halfmove, fullmove);
	    }
	  if (testasim)
	    strcpy (movinito, pieces);
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  j = 7;
//linha de '8' a '1'
	  i = 0;
//coluna de 'a' a 'h'
	  k = 0;
//indice da string
	  while (movinito[k] != '\0')
	    {
	      if (i < 0 || i > 8)
//8 � errado, mas � atingido. precisa arrumar isso.
		msgsai ("Posi��o FEN incorreta.", 24);
	      switch (movinito[k])
//KkQqRrBbNnPp
		{
		case 'K':
		  tabu->tab[i][j] = -REI;
		  i++;
		  break;
		case 'k':
		  tabu->tab[i][j] = REI;
		  i++;
		  break;
		case 'Q':
		  tabu->tab[i][j] = -DAMA;
		  i++;
		  break;
		case 'q':
		  tabu->tab[i][j] = DAMA;
		  i++;
		  break;
		case 'R':
		  tabu->tab[i][j] = -TORRE;
		  i++;
		  break;
		case 'r':
		  tabu->tab[i][j] = TORRE;
		  i++;
		  break;
		case 'B':
		  tabu->tab[i][j] = -BISPO;
		  i++;
		  break;
		case 'b':
		  tabu->tab[i][j] = BISPO;
		  i++;
		  break;
		case 'N':
		  tabu->tab[i][j] = -CAVALO;
		  i++;
		  break;
		case 'n':
		  tabu->tab[i][j] = CAVALO;
		  i++;
		  break;
		case 'P':
		  tabu->tab[i][j] = -PEAO;
		  i++;
		  break;
		case 'p':
		  tabu->tab[i][j] = PEAO;
		  i++;
		  break;
		case '/':
		  i = 0;
		  j--;
		  break;
		default:
//numero de casas vazias
		  i += (movinito[k] - '0');
		}
	      k++;
	    }
//Cor da vez
	  if (testasim)
	    strcpy (movinito, color);
//Veja o que o sono faz no c�digo da gente: (apertando ctrl-s para salvar... ainda bem que n�o foi ctrl-z)
//sssssssssss
	  else
	    scanf ("%s", movinito);
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  if (movinito[0] == 'w')
	    tabu->vez = brancas;
	  else
	    tabu->vez = pretas;
//int roqueb, roquep: 1:pode para os dois lados. 0:nao pode mais. 3:mexeu TD. So pode K. 2:mexeu TR. So pode Q
	  if (testasim)
	    strcpy (movinito, castle);
	  else
	    scanf ("%s", movinito);
//Roque
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  tabu->roqueb = 0;
//nao pode mais
	  tabu->roquep = 0;
//nao pode mais
	  if (!strchr (movinito, '-'))
//algu�m pode
	    {
	      if (strchr (movinito, 'K'))
		tabu->roqueb = 3;
//so pode REI
	      if (strchr (movinito, 'Q'))
		if (tabu->roqueb == 3)
		  tabu->roqueb = 1;
//pode dos dois
		else
		  tabu->roqueb = 2;
//so pode DAMA
	      if (strchr (movinito, 'k'))
		tabu->roquep = 3;
	      if (strchr (movinito, 'q'))
		if (tabu->roquep == 3)
		  tabu->roquep = 1;
//pode dos dois
		else
		  tabu->roquep = 2;
//so pode DAMA
	    }
	  if (testasim)
	    strcpy (movinito, enpassant);
	  else
	    scanf ("%s", movinito);
//En passant
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  tabu->peao_pulou = -1;
//nao pode
	  if (!strchr (movinito, '-'))
//algu�m pode
	    tabu->peao_pulou = movinito[0] - 'a';
//pulou 2 na coluna dada
	  if (testasim)
	    strcpy (movinito, halfmove);
	  else
	    scanf ("%s", movinito);
//Num. de movimentos (ply)
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  tabu->empate_50 = atoi (movinito);
//contador:quando chega a 50, empate.
	  if (testasim)
	    strcpy (movinito, fullmove);
	  else
	    scanf ("%s", movinito);
//Num. de lances
	  if (debug)
	    fprintf (fsaida, "winboard: %s\n", movinito);
	  tabu->numero = 0;
//mudou para ply.
//(atoi(movinito)-1)+0.3;
//inicia no 0.3 para indicar 0
	  ultimo_resultado[0] = '\0';
//0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.)
	  res = situacao (*tabu);
	  switch (res)
	    {
	    case 'a':
//afogado
	    case 'p':
//perpetuo
	    case 'i':
//insuficiencia
	    case '5':
//50 lances
	    case 'r':
	      tabu->situa = 1;
	      break;
//repeticao
	    case 'x':
	      tabu->situa = 2;
	      break;
//xeque
	    case 'M':
	      tabu->situa = 3;
	      break;
//Brancas perderam por Mate
	    case 'm':
	      tabu->situa = 4;
	      break;
//Pretas perderam por mate
	    case 'T':
	      tabu->situa = 5;
	      break;
//Brancas perderam por Tempo
	    case 't':
	      tabu->situa = 6;
	      break;
//Pretas perderam  por tempo
	    case '*':
	      tabu->situa = 7;
	      break;
//sem resultado * {Game was unfinished}
	    default:
	      tabu->situa = 0;
//'-' nada...
	    }
	  USALIVRO = 0;
// Baseado somente nos movimentos, nao na posicao.
	  imptab (*tabu);
	  insere_listab (*tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "ping") || !strcmp (movinito, "san")
	  || !strcmp (movinito, "usermove") || !strcmp (movinito, "time")
	  || !strcmp (movinito, "sigint") || !strcmp (movinito, "sigterm")
	  || !strcmp (movinito, "reuse") || !strcmp (movinito, "myname")
	  || !strcmp (movinito, "variants") || !strcmp (movinito, "colors")
	  || !strcmp (movinito, "ics") || !strcmp (movinito, "name")
	  || !strcmp (movinito, "pause") || !strcmp (movinito, "xboard")
	  || !strcmp (movinito, "protover") || !strcmp (movinito, "2")
	  || !strcmp (movinito, "accepted") || !strcmp (movinito, "done")
	  || !strcmp (movinito, "random") || !strcmp (movinito, "hard")
	  || !strcmp (movinito, ".") || !strcmp (movinito, "computer")
	  || !strcmp (movinito, "easy") || !strcmp (movinito, "{White")
	  || !strcmp (movinito, "{Black") || !strcmp (movinito, "mates}")
	  || !strcmp (movinito, "triple") || !strcmp (movinito, "result")
	  || !strcmp (movinito, "{Draw") || !strcmp (movinito, "by")
	  || !strcmp (movinito, "repetition}")
	  || !strcmp (movinito, "{Stalemate}")
	  || !strcmp (movinito, "resigns}") || !strcmp (movinito, "1-0")
	  || !strcmp (movinito, "0-1") || !strcmp (movinito, "1/2-1/2")
	  || !strcmp (movinito, "{Forfeit") || !strcmp (movinito, "due")
	  || !strcmp (movinito, "to") || !strcmp (movinito, "illegal")
	  || !strcmp (movinito, "move}") || !strcmp (movinito, "{both")
	  || !strcmp (movinito, "players") || !strcmp (movinito, "agreed")
	  || !strcmp (movinito, "draw}") || !strcmp (movinito, "*")
	  || !strcmp (movinito, "{Game") || !strcmp (movinito, "was")
	  || !strcmp (movinito, "unfinished}") || !strcmp (movinito, "?"))
	{
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "mem") && !xboard)
//So Deus explica esta funcao funcionar!
	{
	  imptab (*tabu);
	  tente = 1;
	  continue;
	}
      if (!strcmp (movinito, "t") && xboard)
	testajogo (movinito, tabu->numero);
//retorna um lance do jogo de teste (em movinito)
//e vai la embaixo jogar ele
      if (!strcmp (movinito, "repete") && !xboard)
	{
	  plaux = plcabeca;
	  while (plaux != NULL)
	    {
	      printf ("\n\r  ");
	      printf ("+------------------------+");
	      for (j = 7; j > -1; j--)
		{
		  printf ("\n");
		  printf ("%d ", j + 1);
		  printf ("|");
		  for (i = 0; i < 8; i++)
		    {
		      if ((i + j) % 2)
			casacor = 1;
		      else
			casacor = 2;
		      switch (plaux->tab[i][j])
			{
			case PEAO:
			  printf (" p ");
			  break;
			case CAVALO:
			  printf (" c ");
			  break;
			case BISPO:
			  printf (" b ");
			  break;
			case TORRE:
			  printf (" t ");
			  break;
			case DAMA:
			  printf (" d ");
			  break;
			case REI:
			  printf (" r ");
			  break;
			case -PEAO:
			  printf (" P ");
			  break;
			case -CAVALO:
			  printf (" C ");
			  break;
			case -BISPO:
			  printf (" B ");
			  break;
			case -TORRE:
			  printf (" T ");
			  break;
			case -DAMA:
			  printf (" D ");
			  break;
			case -REI:
			  printf (" R ");
			  break;
			case VAZIA:
			  if (casacor == 1)
			    printf (" . ");
//casa branca
			  else
			    printf (" * ");
//casa preta
			}
		    }
		  printf ("|");
		}
	      printf ("\n\r  ");
	      printf ("+------------------------+");
	      printf ("\n\r  ");
	      printf ("  a  b  c  d  e  f  g  h\n\n\r");
	      printf ("repetiu: %d vez(es). Lance num.: %d\n", plaux->rep,
		      (int) ((plaux->numero + 1.0) / 2.0));
	      disc = pega (" ", "\nTecle <ESPACO>+<ENTER>");
	      plaux = plaux->prox;
	    }
//fim do while
	  printf ("\nA lista de tabuleiros acabou.");
	  disc = pega (" ", "\nTecle <ESPACO>+<ENTER>");
	  imptab (*tabu);
	  res = situacao (*tabu);
	  if (res == 'x')
	    {
	      printf ("\nXeque !!!");
	    }
	  tente = 1;
	  continue;
	}
//comando repete
      if (!movi2lance (lanc, movinito))
//se nao existe este lance
	{
	  if (!xboard)
	    {
//se nao eh o comando lista e nao eh adv
//esse if n�o � mais necess�rio?
	      printf ("\nPara jogar por exemplo C3BD (notacao descritiva),");
	      printf (" digite assim: b1c3<enter>");
	      printf ("\nb1 : casa origem  --> coluna b, linha 1.");
	      printf ("\nc3 : casa destino --> coluna c, linha 3.");
	      printf ("\n\n'desisto' para desistir.");
	      printf ("\n'empate' para pedir empate.");
	      printf ("\n'versao' para saber mais sobre...");
	      printf ("\n'lista' para ver a lista de lances possiveis.");
	      printf
		("\n'nivel' para mudar o nivel de dificuldade atual (tempo de jogo).");
	      printf ("\n'cor' para trocar de cor com o adversario.");
	      printf ("\n'repete' para ver a lista de tabuleiros.");
	      printf ("\n'adv' para novos adversarios neste mesmo jogo.");
	      printf ("\n'inclui' para adicionar esta abertura no livro.");
	      printf ("\n'partida' lista os lances desta partida.");
	      printf ("\n'quit' para abortar o programa.");
	      printf ("\n'mem' para avaliar a memoria disponivel.");
	      disc = pega (" ", "\nTecle <ESPACO>+<ENTER>");
	      imptab (*tabu);
	    }
	  else
	    {
	      if (debug)
		fprintf (fsaida, "xadreco : Illegal move: %s\n", movinito);
	      printf ("Illegal move: %s\n", movinito);
	    }
	  tente = 1;
	  continue;
	}
//fim do if(nao existe o lance)
      pval = valido (*tabu, lanc);
//lanc eh int lanc[4]; cria pval com tudo preenchido
      if (pval == NULL)
	{
	  if (xboard)
	    {
	      if (debug)
		fprintf (fsaida, "xadreco : Illegal move: %s\n", movinito);
	      printf ("Illegal move: %s\n", movinito);
	    }
	  else
	    {
	      printf ("\n");
	      disc =
		pega (" ",
		      "\nMovimento Ilegal. Tente outra vez.\nTecle <ESPACO>+<ENTER>");
	      imptab (*tabu);
	      res = situacao (*tabu);
	      if (res == 'x' && !xboard)
		{
		  printf ("\nXeque !!!");
		}
	    }
	  tente = 1;
	  continue;
	}
    }
  while (tente);

  if (pval->especial > 3)
//promocao do peao 4,5,6,7: Dama,Cavalo, Torre, Bispo
    {
      if (xboard)
	{
	  switch (movinito[4])
//exemplo: e7e8q
	    {
	    case 'q':
	      pval->especial = 4;
	      break;
//dama
	    case 'r':
	      pval->especial = 6;
	      break;
//torre
	    case 'b':
	      pval->especial = 7;
	      break;
//bispo
	    case 'n':
	      pval->especial = 5;
	      break;
//cavalo
	    default:
	      pval->especial = 4;
	      break;
//dama
	    }
	}
      else
	{
//promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
	  printf ("\n");
	  peca =
	    pega ("dtcb",
		  "\nO peao foi promovido. Escolha uma peca (d,t,b,c)? ");
	  switch (peca)
	    {
	    case 'd':
	      pval->especial = 4;
	      break;
	    case 'c':
	      pval->especial = 5;
	      break;
	    case 't':
	      pval->especial = 6;
	      break;
	    case 'b':
	      pval->especial = 7;
	      break;
	    default:
	      printf ("Erro na fun��o pega(\"dtcb\",\"Promove?\")");
	      pval->especial = 4;
//para nao travar, escolhe dama e segue
	    }
	}
    }
  res = joga_em (tabu, *pval, 1);
//a funcao joga_em deve inserir no listab cod==1
  free (pval);
  if (analise > 0 && xboard)
// analise outro movimento
    disc = analisa (tabu);
  return (res);
//vez da outra cor jogar. retorna a situacao.
}

//fim do huma_joga---------------

movimento *
valido (tabuleiro tabu, int *lanc)
{
  int nmov = 0;
  movimento *pmovi, *loop, *auxloop;
  pmovi = geramov (tabu, &nmov);
  loop = pmovi;
  auxloop = NULL;
  while (pmovi != NULL)
//vai comparando e...
    {
      if (igual (pmovi->lance, lanc))
	break;
//o lance valido eh o cabeca da lista que restou
      pmovi = pmovi->prox;
      free (loop);
//...liberando os diferentes
      loop = pmovi;
    }
//apagar a lista que restou da memoria
  if (pmovi != NULL)
//o lance esta na lista, logo eh valido,
    {
      loop = pmovi->prox;
//apaga o restante da lista.
      pmovi->prox = NULL;
    }

  while (loop != NULL)
    //ou loop==pmovi==NULL e o lance nao vale
    {
//ou loop==pmovi->prox e apaga o restante
      auxloop = loop->prox;
      free (loop);
      loop = auxloop;
    }
  return (pmovi);
}

//fim da valido
int
igual (int *l1, int *l2)
//dois lances[4] sao iguais
{
  int j;
  for (j = 0; j < 4; j++)
    if (l1[j] != l2[j])
      return (0);
  return (1);
}

char
situacao (tabuleiro tabu)
{
// pega o tabuleiro e retorna: M,m,a,p,i,5,r,T,t,x
// tabu.situa:      retorno da situacao:
//       0                              -               tudo certo... nada a declarar
//       1                              a:              empate por afogamento
//       1                              p:              empate por xeque-perpetuo
//       1                              i:              empate por insuficiencia de material
//       1                              5:              empate apos 50 lances sem movimento de peao e captura
//       1                              r:              empate apos repetir uma mesma posicao 3 vezes
//       2                              x:              xeque
//      3,4                             M,m:            xeque-mate (nas Brancas e nas Pretas respec.)
//      5,6                             T,t:            o tempo acabou (das Brancas e das Pretas respec.)
//      7                               *:              {Game was unfinished}
  movimento *cabeca;
  int insuf_branca = 0, insuf_preta = 0, nmov = -1;
  int i, j;
  listab *plaux;
  if (tabu.empate_50 >= 50.0)
//empate apos 50 lances sem captura ou movimento de peao
    return ('5');
  for (i = 0; i < 8; i++)
//insuficiencia de material
    for (j = 0; j < 8; j++)
      {
	switch (tabu.tab[i][j])
	  {
	  case -DAMA:
	  case -TORRE:
	  case -PEAO:
	    insuf_branca += 3;
//3: eh suficiente
	    break;
	  case DAMA:
	  case TORRE:
	  case PEAO:
	    insuf_preta += 3;
//3: eh suficiente
	    break;
	  case -BISPO:
	    insuf_branca += 2;
//2: falta pelo menos mais um
	    break;
	  case BISPO:
	    insuf_preta += 2;
//2: falta pelo menos mais um
	    break;
	  case -CAVALO:
	    insuf_branca++;
//1: falta pelo menos mais dois
	    break;
	  case CAVALO:
	    insuf_preta++;
//1: falta pelo menos mais dois
	    break;
	  }
	if (insuf_branca > 2 || insuf_preta > 2)
	  break;
      }
  if (insuf_branca < 3 && insuf_preta < 3)
//os dois est�o com insufici�ncia de material
    return ('i');
  plaux = plfinal;
//posicao repetiu 3 vezes
  while (plaux != NULL)
    {
      if (plaux->rep == 3)
	return ('r');
      plaux = plaux->ant;
    }
  cabeca = geramov (tabu, &nmov);
//criar funcao gera1mov() retorna verdadeiro ou falso: quando nmov == -1, geramov retorna no primeiro v�lido
  if (cabeca == NULL)
//Sem lances: Mate ou Afogamento.
    if (!xeque_rei_das (tabu.vez, tabu))
      return ('a');
//empate por afogamento.
    else if (tabu.vez == brancas)
      return ('M');
//as brancas estao em xeque-mate
    else
      return ('m');
//as pretas estao em xeque-mate
  else
//Tem lances... mudou a geramov: tem mais n�o carrega: nmovi==-1
    {
//        libera_lances(cabeca);
      if (xeque_rei_das (tabu.vez, tabu))
	return ('x');
    }
  return ('-');
//nada
}

// ------------------------------- jogo do computador -----------------------
char
compjoga (tabuleiro * tabu)
{
  char command[10], res, com;
  int i, j, casacor;
//nao precisa se tem textbackground
  int nv = 1, nmov = 0;
  int moves, minutes, incre;
  listab *plaux;
  int melhorvalor1;
// lances calc. em maior nivel tem mais importancia?
  movimento *melhorcaminho1;
  if (pausa == 'c' && !xboard)
    {
      do
	{
	  printf ("\ncomando: ");
	  scanf ("%s", command);
	  if (!strcmp (command, "quit"))
	    msgsai ("Obrigado por jogar com o Xadreco.", 0);
	  else if (!strcmp (command, "desisto")
		   || !strcmp (command, "resign"))
	    if (tabu->vez == brancas)
	      return ('B');
	    else
	      return ('b');
	  else if (!strcmp (command, "lista"))
	    {
	      mostra_lances (*tabu);
	      com = pega (" ", "\nTecle <ESPACO>+<ENTER>");
	    }
	  else if (!strcmp (command, "repete"))
	    {
	      plaux = plcabeca;
	      while (plaux != NULL)
		{
		  printf ("\n\r  ");
		  printf ("+------------------------+");
		  for (j = 7; j > -1; j--)
		    {
		      printf ("\n");
		      printf ("%d ", j + 1);
		      printf ("|");
		      for (i = 0; i < 8; i++)
			{
			  if ((i + j) % 2)
			    casacor = 1;
			  else
			    casacor = 2;
			  switch (plaux->tab[i][j])
			    {
			    case PEAO:
			      printf (" p ");
			      break;
			    case CAVALO:
			      printf (" c ");
			      break;
			    case BISPO:
			      printf (" b ");
			      break;
			    case TORRE:
			      printf (" t ");
			      break;
			    case DAMA:
			      printf (" d ");
			      break;
			    case REI:
			      printf (" r ");
			      break;
			    case -PEAO:
			      printf (" P ");
			      break;
			    case -CAVALO:
			      printf (" C ");
			      break;
			    case -BISPO:
			      printf (" B ");
			      break;
			    case -TORRE:
			      printf (" T ");
			      break;
			    case -DAMA:
			      printf (" D ");
			      break;
			    case -REI:
			      printf (" R ");
			      break;
			    case VAZIA:
			      if (casacor == 1)
				printf (" . ");
//casa branca
			      else
				printf (" * ");
//casa preta
			    }
			}
		      printf ("|");
		    }
		  printf ("\n\r  ");
		  printf ("+------------------------+");
		  printf ("\n\r  ");
		  printf ("  a  b  c  d  e  f  g  h\n\n\r");
		  printf ("repetiu: %d vez(es). Lance num.: %d\n", plaux->rep,
			  (int) ((plaux->numero + 1.0) / 2.0));
		  disc = pega (" ", "\nTecle <ESPACO>+<ENTER>");
		  plaux = plaux->prox;
		}
//fim do while
	      com =
		pega ("c ",
		      "\nA lista de tabuleiros acabou. \nTecle <ESPACO>+<ENTER> para prosseguir ou <c>+<ENTER> para comando");
	    }
//comando repete
	  else if (!strcmp (command, "nivel"))
	    {
	      printf
		("\n\nQual o tempo de jogo? MOVIMENTOS SEGUNDOS INCREMENTO: ");
	      scanf ("%d %d %d", &moves, &minutes, &incre);
	      if (moves <= 0)
		moves = 100;
	      if (minutes <= 0)
		minutes = 2;
	      incre -= 2;
	      if (incre < 0)
		incre = 0;
	      tempomovclock = minutes * 6000 / moves + incre * 100;
	      imptab (*tabu);
	      com =
		pega ("c ",
		      "\nTecle <ESPACO>+<ENTER> para prosseguir ou <c>+<ENTER> para comando");
	    }
	  else
	    {
	      printf ("\nOs comandos sao:\n");
	      printf
		("	lista  : mostra os movimentos possiveis e o melhor lance.\n");
	      printf
		("	desisto: determina o fim deste jogo, com abandono para a cor da vez.\n");
	      printf ("	quit   : termino anormal do programa.\n");
	      printf ("	repete : mostra a lista de tabuleiros.\n");
	      printf
		("	nivel  : muda o nivel de dificuldade atual (tempo do jogo).\n");
	      com =
		pega ("c ",
		      "\nTecle <ESPACO>+<ENTER> para prosseguir ou <c>+<ENTER> para comando");
	    }
	}
      while (com == 'c');
      //outro comando
    }
  //variavel pausa!='c'
  //cloock1 = cloock()*10000/CLOoCKS_PER_SEC; // retorna cloock1 em milesimos de segundos...
  //cloock_t � long int %ld
  //mudou para logo abaixo do scanf de humajoga
  limpa_pensa ();
  //limpa algumas vari�veis para iniciar a pondera��o
  melhorvalor1 = -LIMITE;
  melhorcaminho1 = NULL;
  //o rostinho nao eh compativel com linux. nao imprima nunca.
  //    if (!xboard && 1 == 0)
  //    {
  //        printf(":)");
  //    }
  if (debug == 2)
    //nivel extra de debug
    {
      fmini = fopen ("minimax.log", "w");
      if (fmini == NULL)
	debug = 1;
    }
  if (USALIVRO && tabu->numero < 52 && setboard != 1)
    {
      usalivro (*tabu);
      if (result.plance == NULL)
	USALIVRO = 0;
      libera_lances (melhorcaminho1);
      melhorcaminho1 = copilistmov (result.plance);
      melhorvalor1 = result.valor;
      clock2 = clock () * 100 / CLOCKS_PER_SEC;	// retorna cloock em centesimos de segundos...
      difclock = clock2 - clock1;
    }
  if (result.plance == NULL)
    {
      //mudou para busca em amplitude: variavel nivel sem uso. Implementar "sd n"
      nv = 1;
      nmov = 0;
      libera_lances (succ_geral);
      succ_geral = geramov (*tabu, &nmov);
      //gera os sucessores
      totalnodo = 0;
      //funcao COMPJOGA (CONFERIR)
      while (result.valor < XEQUEMATE)
	{
	  limpa_pensa ();
	  if (debug == 2)
	    //nivel extra de debug
	    {
	      fprintf (fmini,
		       "\n\n*************************************************************");
	      fprintf (fmini,
		       "\nminimax(*tabu, prof=0, alfa=%d, beta=%d, nv=%d)",
		       -LIMITE, LIMITE, nv);
	    }
	  //tabuleiro atual, profundidade zero, limite minimo de estatico (alfa ou passo), limite maximo de estatico (beta ou uso), nivel da busca
	  //profflag=1; //pode analisar.
	  minimax (*tabu, 0, -LIMITE, +LIMITE, nv);
	  //retorna o melhor caminho a partir de tab...
	  if (result.plance == NULL)
	    //Nova defini��o: sem lances, pode ser que queira avan�ar ap�s mate.
	    break;
	  if (difclock < tempomovclock)
	    //18/10/2004, 19:20 +3 GMT. Descobri e criei esse teste ap�s muito sofrimento em debugs. Fim da ver. cinco!
	    {
	      libera_lances (melhorcaminho1);
	      melhorcaminho1 = copilistmov (result.plance);
	      melhorvalor1 = result.valor;
	    }
	  totalnodo += totalnodonivel;
	  clock2 = clock () * 100 / CLOCKS_PER_SEC;	// retorna cloock em centesimos de segundos...
	  difclock = clock2 - clock1;
	  ordena_succ (nmov);
	  //ordena succ_geral
	  if (debug == 2)
	    {
	      fprintf (fmini,
		       "\nresult.valor: %+.2f totalnodo: %d\nresult.plance: ",
		       result.valor / 100.0, totalnodo);
	      if (!mostrapensando || abs (result.valor) == FIMTEMPO
		  || abs (result.valor) == LIMITE)
		imprime_linha (result.plance, 1, 2);
	      //1=numero do lance, 2=vez: pular impressao na tela.
	      //tabu->numero+1, -tabu->vez);
	    }
	  //nivel pontua��o tempo totalnodo varia��o === usado!
	  //nivel tempo valor lances no arena. (nao usado aqui. testar)
	  if (mostrapensando && abs (result.valor) != FIMTEMPO
	      && abs (result.valor) != LIMITE)
	    {
	      printf ("%d %d %d %d ", nv, result.valor, difclock, totalnodo);
	      if (debug)
		fprintf (fsaida, "xadreco : %d %+.2f %d %d ", nv,
			 (float) result.valor / 100.0, difclock, totalnodo);
	      imprime_linha (result.plance, tabu->numero + 1, -tabu->vez);
	      printf ("\n");
	      if (debug)
		fprintf (fsaida, "\n");
	    }
	  // termino do la�o infinito baseado no tempo
	  if (difclock > tempomovclock && debug != 2
	      || (debug == 2 && nv == 3))
	    // termino do la�o infinito baseado no tempo
	    break;
	  nv++;
	}
    }
  // fim do se n�o usou livro
  libera_lances (result.plance);
  result.plance = copilistmov (melhorcaminho1);
  result.valor = melhorvalor1;
  //nivel extra de debug
  if (debug == 2)
    (void) fclose (fmini);
  //Utilizado: ABANDONA==-2730, alterado quando contra outra engine
  if (result.valor < ABANDONA && ofereci == 0)
    {
      if (debug)
	fprintf (fsaida, "xadreco : resign. value: %+.2f\n",
		 result.valor / 100.0);
      --ofereci;
      printf ("resign\n");
    }
  //pode oferecer empate duas vezes. Uma assim que perder 2 pe�es. Outra ap�s 60 lances e com menos de 2 pe�es
  if (tabu->numero == MOVE_EMPATE2 && ofereci == 0)
    ofereci = 1;
  //oferecer empate: result.valor est� invertido na vez.
  if (result.valor < QUANTO_EMPATE1 && tabu->numero > MOVE_EMPATE1
      && tabu->numero < MOVE_EMPATE2 && ofereci > 0)
    {
      if (debug)
	fprintf (fsaida, "xadreco : offer draw (1) value: %+.2f\n",
		 result.valor / 100.0);
      --ofereci;
      //aten��o: oferecer pode significar aceitar, se for feito logo ap�s uma oferta recebida.
      printf ("offer draw\n");
    }
  //oferecer empate: result.valor est� invertido na vez.
  if (result.valor < QUANTO_EMPATE2 && tabu->numero >= MOVE_EMPATE2
      && ofereci > 0)
    {
      if (debug)
	fprintf (fsaida, "xadreco : offer draw (2) value: %+.2f\n",
		 result.valor / 100.0);
      --ofereci;
      //aten��o: oferecer pode significar aceitar, se for feito logo ap�s uma oferta recebida.
      printf ("offer draw\n");
    }
  //Nova defini��o: sem lances, pode ser que queira avan�ar ap�s mate.
  //algum problema ocorreu que esta sem lances
  if (result.plance == NULL)
    return ('e');
  res = joga_em (tabu, *result.plance, 1);
  //vez da outra cor jogar. retorna a situacao(*tabu)
  return (res);
}

//fim da compjoga

char
analisa (tabuleiro * tabu)
{
  tabuleiro tanalise;
  int nv = 0, nmov = 0;
// lances calc. em maior nivel tem mais importancia?
//mudou para logo abaixo do scanf de humajoga
  copitab (&tanalise, tabu);
  limpa_pensa ();
  if (debug == 2)
//nivel extra de debug
    {
      fmini = fopen ("minimax.log", "w");
      if (fmini == NULL)
	debug = 1;
    }
//mudou para busca em amplitude: variavel nivel obsoleta!
  if (USALIVRO && tabu->numero < 50 && setboard != 1)
    usalivro (*tabu);
  if (result.plance != NULL)
    {
      clock2 = clock () * 100 / CLOCKS_PER_SEC;	// retorna cloock em centesimos de segundos...
      difclock = clock2 - clock1;
//nivel pontua��o tempo totalnodo varia��o === usado!
      printf ("%d %d %d %d Livro: ", nv, result.valor, difclock, totalnodo);
      if (debug)
	fprintf (fsaida, "xadreco : %d %+.2f %d %d Livro: ", nv,
		 (float) result.valor / 100.0, difclock, totalnodo);
      imprime_linha (result.plance, tabu->numero + 1, -tabu->vez);
      printf ("\n");
      if (debug)
	fprintf (fsaida, "\n");
    }
  else
    {
      nv = 1;
      nmov = 0;
      libera_lances (succ_geral);
      succ_geral = geramov (*tabu, &nmov);
//gera os sucessores
      totalnodo = 0;
      while (result.valor < XEQUEMATE)
//for(nv=1; nv<=7; nv++)
//funcao ANALISA (CONFERIR)
	{
	  limpa_pensa ();
// tabuleiro atual, profundidade zero, limite minimo de estatico (alfa ou passo), limite maximo de estatico (beta ou uso), nivel da busca
	  minimax (*tabu, 0, -LIMITE, LIMITE, nv);
//retorna o melhor caminho a partir de tab...
	  totalnodo += totalnodonivel;
	  clock2 = clock () * 100 / CLOCKS_PER_SEC;	// retorna cloock em centesimos de segundos...
	  difclock = clock2 - clock1;
	  ordena_succ (nmov);
//nivel pontua��o tempo totalnodo varia��o === usado!
	  if (abs (result.valor) != FIMTEMPO && abs (result.valor) != LIMITE)
	    {
	      printf ("%d %d %d %d ", nv, result.valor, difclock, totalnodo);
	      if (debug)
		fprintf (fsaida, "xadreco : %d %+.2f %d %d ", nv,
			 (float) result.valor / 100.0, difclock, totalnodo);
	      imprime_linha (result.plance, tabu->numero + 1, -tabu->vez);
//1=numero do lance, 2=vez: pular impressao na tela
	      if (result.plance == NULL)
		printf ("no moves\n");
	      else
		printf ("\n");
	      if (debug)
		if (result.plance == NULL)
		  printf ("no moves\n");
		else
		  fprintf (fsaida, "\n");
	    }
	  if (debug == 2)
	    {
	      fprintf (fmini,
		       "\nresult.valor: %+.2f totalnodo: %d\nresult.plance: ",
		       result.valor / 100.0, totalnodo);
	      imprime_linha (result.plance, 1, 2);
//1=numero do lance, 2=vez: pular impressao na tela.
// tabu->numero+1, -tabu->vez);
	    }
	  if ((difclock > tempomovclock && debug != 2)
	      || (debug == 2 && nv == 3))
// termino do la�o infinito baseado no tempo
	    break;
	  if (result.plance == NULL)
//Nova defini��o: sem lances, pode ser que queira avan�ar ap�s mate.
	    break;
	  nv++;
	}
    }
  if (debug == 2)
//nivel extra de debug
    (void) fclose (fmini);
  if (result.plance == NULL)
//...ap�s o t�rmino da partida, s� pode-se usar edi��o, undo, etc.
//              msgsai("Computador sem lances v�lidos 3", 36);
//algum problema ocorreu que esta sem lances
    return ('e');
//erro! computador sem lances!? nivel deve ser > zero
  return ('-');
//tudo certo... (situa)
}

//--------------------------------------------------------------------------
//tabuleiro atual, profundidade zero, limite maximo de estatico (beta ou uso), limite minimo de estatico (alfa ou passo), nivel da busca
//Finding the rotten fish in the second bag was like exceeding beta.
//If there had been no fish in the bag, determining that the six-pack of pop
//bag was better than the sandwich bag would have been like exceeding alpha (one ply back).
//source: http://www.seanet.com/~brucemo/topics/alphabeta.htm
void
minimax (tabuleiro atual, int prof, int alfa, int beta, int niv)
{
  movimento *succ, *melhor_caminho, *cabeca_succ;
  int novo_valor, nmov = 0, contamov = 0;
  tabuleiro tab;
  char m[8];
  succ = NULL;
  melhor_caminho = NULL;
  cabeca_succ = NULL;
  if (profsuf (atual, prof, alfa, beta, niv))
    // profundidade suficiente ==1 ou ==-1:tempo estourou
    {
      //coloque um nulo no ponteiro plance
      //nao eh necessario libera_lance, pois plance eh tempor�rio apesar de global.
      //estatico(tabuleiro, 1: acabou o tempo, 0: nao acabou. Prof: qual nivel est� analisando?)
      //if (debug == 2)
      //fprintf(fmini, "\nprofsuf! ");
      return;
    }
  if (debug == 2)
    {
      fprintf (fmini,
	       "\n----------------------------------------------Minimax prof: %d",
	       prof);
      //fprintf(fmini, "\nalfa= %d     beta= %d", alfa, beta);
    }
  if (prof == 0)
    succ = succ_geral;
  //copilistmov(succ_geral);
  else
    succ = geramov (atual, &nmov);
  //succ==NULL se alguem ganhou ou empatou
  //if (debug == 2) {
  //fprintf(fmini, "\nsucc=geramov(): ");
  //imprime_linha(succ, 1, 2);
  //1=numero do lance, 2=vez: pular impressao na tela
  //}
  if (succ == NULL)
    {
      //entao o estatico refletira isso: afogamento
      //libera_lances(result.plance); //bugbug
      result.plance = NULL;
      //coloque um nulo no ponteiro plance
      //nao eh necessario libera_lance, pois plance eh tempor�rio apesar de global.
      result.valor = estatico (atual, 0, prof, alfa, beta);
      //estatico(tabuleiro, 1: acabou o tempo, 0: nao acabou. prof: qual nivel est� analisando?)
      if (debug == 2)
	fprintf (fmini, "NULL ");	//result.valor=%+.2f", result.valor);
      return;
    }
  cabeca_succ = succ;
  //laco para analisar todos sucessores (ou corta no porcento)
  while (succ != NULL)
    {
      copitab (&tab, &atual);
      disc = (char) joga_em (&tab, *succ, 1);
      //joga o lance atual, a funcao joga_em deve inserir no listab
      totalnodonivel++;
      profflag = succ->flag_50 + 1;	//se for zero, fim da busca.
      //flag_50:0=nada,1=Moveu peao,2=Comeu,3=Peao Comeu;
      //flag_50== 2 ou 3 : houve captura :Liberou
      //tab.situa:0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.) 7: sem resultado
      switch (tab.situa)
	{
	case 0:
	  break;
	  //0:nada... Quem decide eh flag_50;
	case 2:
	  profflag = 4;
	  break;
	  //2:Xeque!  Liberou
	default:
	  profflag = 0;
	  //tab.situa: 1=Empate, 3,4=Mate ou 5,6=Tempo. 7=sem resultado. Nao pode passar o nivel
	}
      if (debug == 2)
	{
	  lance2movi (m, succ->lance, succ->especial);
	  fprintf (fmini, "\nnivel %d, %d-lance %s (%d%d%d%d):", prof,
		   totalnodonivel, m, succ->lance[0], succ->lance[1],
		   succ->lance[2], succ->lance[3]);
	}
      minimax (tab, prof + 1, -beta, -alfa, niv);
      //analisa o novo tabuleiro
      retira_listab ();
      novo_valor = -result.valor;
      //implementar o "random"
      if (novo_valor > alfa)
	// || (novo_valor==alfa && rand()%10>7))
	{
	  alfa = novo_valor;
	  libera_lances (melhor_caminho);
	  melhor_caminho = copimel (*succ, result.plance);
	  //cria nova cabeca
	}
      if (debug == 2 && prof == 0)
	{
	  lance2movi (m, succ->lance, succ->especial);
	  fprintf (fmini, "\nnovo_valor=%+.2f", novo_valor / 100.0);
	  if (melhor_caminho != NULL)
	    {
	      fprintf (fmini, "\nmelhor_caminho=");
	      imprime_linha (melhor_caminho, 1, 2);
	      //1=numero do lance, 2=vez: pular impressao na tela
	    }
	}
      //implementar o NULL-MOVE
      if (novo_valor >= beta)
	//corte alfa-beta! Isso est� certo? N�o � alfa<=beta? Conferir. (alfa==passo, beta==uso, para brancas)
	{
	  alfa = beta;		//Aha! Retorna beta como o melhor poss�vel desta �rvore
	  if (debug == 2)
	    {
	      lance2movi (m, succ->lance, succ->especial);
	      fprintf (fmini,
		       "\nsucc: novo_valor>=beta (%+.2f>=%+.2f) %s (%d%d%d%d) Corte Alfa/Beta!",
		       novo_valor / 100.0, beta / 100.0, m, succ->lance[0],
		       succ->lance[1], succ->lance[2], succ->lance[3]);
	    }
	  break;
	  //faz o corte!
	}
      if (prof == 0)
	succ->valor_estatico = novo_valor;
      succ = succ->prox;
      contamov++;
      if (contamov > nmov * PORCENTO_MOVIMENTOS + 1 && prof != 0)	//tentando com 60%
	break;
    }
  //while(succ!=NULL)
  result.valor = alfa;
  result.plance = copilistmov (melhor_caminho);
  //funcao retorna uma cabeca nova
  libera_lances (melhor_caminho);
  if (prof != 0)
    libera_lances (cabeca_succ);
  //caso contrario, cabeca_succ esta apontando para succ_geral
  if (debug == 2)
    fprintf (fmini,
	     "\n------------------------------------------END Minimax prof: %d",
	     prof);
}

int
profsuf (tabuleiro atual, int prof, int alfa, int beta, int niv)
{
  clock2 = clock () * 100 / CLOCKS_PER_SEC;	// retorna cloock em centesimos de segundos...  
  difclock = clock2 - clock1;
  char input;
  //se tem captura ou xeque... liberou
  //se ja passou do nivel estipulado, pare a busca incondicionalmente
  if (prof > niv)
    {
      result.plance = NULL;
      result.valor = estatico (atual, 0, prof, alfa, beta);
      //estatico(tabuleiro, 1: acabou o tempo, 0: nao acabou. Prof: qual nivel est� analisando?)
      return 1;
    }
  //retorna sem analisar... Deve desconsiderar o lance
  if (difclock > tempomovclock && debug != 2)
    {
      result.plance = NULL;
      result.valor = estatico (atual, 1, prof, alfa, beta);	//-FIMTEMPO;//
      return 1;
    }
  //retorna sem analisar... Deve desconsiderar o lance. Usu�rio clicou em "move now" (?)
  //if((int)(difclock) % 100 == 0) // a cada 100 centesimos, examina
  //{
//  if(moveagora())
//  {
//	result.plance = NULL;
//	result.valor = estatico (atual, 1, prof, alfa, beta); //-FIMTEMPO;//
//	ungetc (input, stdin);
//	return 1;
//  }
//  }

  if(clock2 - inputcheckclock > 8) // && teminterroga==0) //CLOCKS_PER_SEC/10000)
  {
//		printf("(%d) ",clock2 - inputcheckclock);
		if (pollinput ())
		{
			do
			{
				input = getc (stdin);
//				printf("input: %c", input);
			}
			while ((input == '\n' || input == '\r') && pollinput ());
//			printf ("%c\n", input);
	//		ungetc (input, stdin);
			if (input == '?')
			{
				result.plance = NULL;
				result.valor = estatico (atual, 1, prof, alfa, beta); //-FIMTEMPO;//
				ungetc (input, stdin);
//				teminterroga=1;
//				printf("teminterroga: %d",teminterroga);
				return 1;
			}
			else
			{
				ungetc (input, stdin);
				inputcheckclock = clock2;
//				return 0;
				//inputcheckclock = clock2;
			}
		}
  }
  
//  if(teminterroga)
//  {
//	result.plance = NULL;
//	result.valor = estatico (atual, 1, prof, alfa, beta); //-FIMTEMPO;//
//	return 1;
//  }

  if (!profflag)
    //nao liberou profflag==0 retorna
    {
      result.plance = NULL;
      result.valor = estatico (atual, 0, prof, alfa, beta);
      //estatico(tabuleiro, 1: acabou o tempo, 0: nao acabou. Prof: qual nivel est� analisando?)
      return 1;
      //a profundidade ja eh sufuciente
    }
  //se esta no nivel estipulado e nao liberou
  //if(prof==niv && profflag<2)
  //return 1;
  //a profundidade ja eh sufuciente
//se OU Nem-Chegou-no-Nivel OU Liberou, pode ir fundo
  return 0;
}

char
joga_em (tabuleiro * tabu, movimento movi, int cod)
{
  int i;
  char res;
  if (movi.especial == 7)
//promocao do peao: BISPO
    tabu->tab[movi.lance[0]][movi.lance[1]] = BISPO * tabu->vez;
  if (movi.especial == 6)
//promocao do peao: TORRE
    tabu->tab[movi.lance[0]][movi.lance[1]] = TORRE * tabu->vez;
  if (movi.especial == 5)
//promocao do peao: CAVALO
    tabu->tab[movi.lance[0]][movi.lance[1]] = CAVALO * tabu->vez;
  if (movi.especial == 4)
//promocao do peao: DAMA
    tabu->tab[movi.lance[0]][movi.lance[1]] = DAMA * tabu->vez;
  if (movi.especial == 3)
//comeu en passant
    tabu->tab[tabu->peao_pulou][movi.lance[1]] = VAZIA;
  if (movi.especial == 2)
//roque grande
    {
      tabu->tab[0][movi.lance[1]] = VAZIA;
      tabu->tab[3][movi.lance[1]] = TORRE * tabu->vez;
    }
  if (movi.especial == 1)
//roque pequeno
    {
      tabu->tab[7][movi.lance[1]] = VAZIA;
      tabu->tab[5][movi.lance[1]] = TORRE * tabu->vez;
    }
  if (movi.flag_50)
//empate de 50 lances sem mover peao ou capturar
    tabu->empate_50 = 0;
  else
    tabu->empate_50 += .5;
  if (tabu->vez == brancas)
    switch (movi.roque)
//avalia o que este lance causa para futuros roques
      {
      case 0:
	tabu->roqueb = 0;
	break;
//mexeu o rei
      case 1:
	break;
//nao fez lance que interfere o roque
      case 2:
	if (tabu->roqueb == 1 || tabu->roqueb == 2)
//mexeu torre do rei
	  tabu->roqueb = 2;
	else
	  tabu->roqueb = 0;
	break;
      case 3:
	if (tabu->roqueb == 1 || tabu->roqueb == 3)
//mexeu torre da dama
	  tabu->roqueb = 3;
	else
	  tabu->roqueb = 0;
	break;
      }
  else
//vez das pretas
    switch (movi.roque)
//avalia o que este lance causa para futuros roques
      {
      case 0:
	tabu->roquep = 0;
	break;
//mexeu o rei
      case 1:
	break;
//nao fez lance que interfere o roque
      case 2:
	if (tabu->roquep == 1 || tabu->roquep == 2)
//mexeu torre do rei
	  tabu->roquep = 2;
	else
	  tabu->roquep = 0;
	break;
      case 3:
	if (tabu->roquep == 1 || tabu->roquep == 3)
//mexeu torre da dama
	  tabu->roquep = 3;
	else
	  tabu->roquep = 0;
	break;
      }
  tabu->peao_pulou = movi.peao_pulou;
  tabu->tab[movi.lance[2]][movi.lance[3]] =
    tabu->tab[movi.lance[0]][movi.lance[1]];
  tabu->tab[movi.lance[0]][movi.lance[1]] = VAZIA;
  for (i = 0; i < 4; i++)
    tabu->lancex[i] = movi.lance[i];
  tabu->numero++;
//conta os movimentos de cada pe�a (� chamado de dentro de fun��o recursiva!)
  tabu->vez = adv (tabu->vez);
  tabu->especial = movi.especial;
  if (cod)
    insere_listab (*tabu);
//insere o lance no listab cod==1
  res = situacao (*tabu);
  switch (res)
    {
    case 'a':
//afogado
    case 'p':
//perpetuo
    case 'i':
//insuficiencia
    case '5':
//50 lances
    case 'r':
      tabu->situa = 1;
      break;
//repeticao
    case 'x':
      tabu->situa = 2;
      break;
//xeque
    case 'M':
      tabu->situa = 3;
      break;
//Brancas perderam por Mate
    case 'm':
      tabu->situa = 4;
      break;
//Pretas perderam por mate
    case 'T':
      tabu->situa = 5;
      break;
//Brancas perderam por Tempo
    case 't':
      tabu->situa = 6;
      break;
//Pretas perderam  por tempo
    case '*':
      tabu->situa = 7;
      break;
//sem resultado * {Game was unfinished}
    default:
      tabu->situa = 0;
//'-' nada...
    }
  return (res);
}

//fim da joga_em
//copia ummovi no comeco da lista *plan, e retorna a cabeca desta NOVA lista (mel)
movimento *
copimel (movimento ummovi, movimento * plan)
{
  movimento *mel;
  mel = (movimento *) malloc (sizeof (movimento));
  if (mel == NULL)
    msgsai ("Erro de alocacao de memoria em copimel 1", 25);
  copimov (mel, &ummovi);
  copilistmovmel (mel, plan);
  return (mel);
}

// copia do segundo lance para frente, mantendo o primeiro
void
copilistmovmel (movimento * dest, movimento * font)
{
  movimento *loop;
  if (font == dest)
    msgsai ("Funcao copilistmovmel precisa de duas listas DIFERENTES.", 26);
  if (dest == NULL)
    msgsai ("Funcao copilistmovmel precisa dest!=NULL.", 27);
  if (font == NULL)
    {
      dest->prox = NULL;
      return;
    }
  dest->prox = (movimento *) malloc (sizeof (movimento));
  if (dest->prox == NULL)
    msgsai ("Erro de alocacao de memoria em copilistmovmel 1", 28);
  loop = dest->prox;
  while (font != NULL)
    {
      copimov (loop, font);
      font = font->prox;
      if (font != NULL)
	{
	  loop->prox = (movimento *) malloc (sizeof (movimento));
	  if (loop->prox == NULL)
	    msgsai ("Erro de alocacao de memoria em copilistmovmel 2", 29);
	  loop = loop->prox;
	}
    }
  loop->prox = NULL;
}

void
copimov (movimento * dest, movimento * font)
//nao compia o ponteiro prox
{
  int i;
  for (i = 0; i < 4; i++)
    dest->lance[i] = font->lance[i];
  dest->peao_pulou = font->peao_pulou;
  dest->roque = font->roque;
  dest->flag_50 = font->flag_50;
  dest->especial = font->especial;
  dest->ordena = font->ordena;
  dest->valor_estatico = font->valor_estatico;
}

movimento *
copilistmov (movimento * font)
//cria nova lista de movimentos para destino
{
  movimento *cabeca, *pmovi;
  if (font == NULL)
    return NULL;
  cabeca = (movimento *) malloc (sizeof (movimento));
//valor novo que sera do result.plance
  if (cabeca == NULL)
    msgsai ("Erro ao alocar memoria em copilistmov 1", 30);
  pmovi = cabeca;
  while (font != NULL)
    {
      copimov (pmovi, font);
      font = font->prox;
      if (font != NULL)
	{
	  pmovi->prox = (movimento *) malloc (sizeof (movimento));
	  if (pmovi->prox == NULL)
	    msgsai ("Erro ao alocar memoria em copilistmov 2", 31);
	  pmovi = pmovi->prox;
	}
    }
  pmovi->prox = NULL;
  return (cabeca);
}

//retorna o valor t�tico e estrat�gico de um tabuleiro, sendo valor positivo melhor quem est� na vez
//cod: 1: acabou o tempo, 0: ou eh avalia��o normal?
//niv: qual a dist�ncia do tabuleiro real para a c�pia tabu avaliada?
int
estatico (tabuleiro tabu, int cod, int niv, int alfa, int beta)
{
  int totb = 0, totp = 0;
  int i, j, k, cor, peca;
  int peaob, peaop, isob, isop, pecab = 0, pecap = 0, material;
  int menorb = 0, menorp = 0;
  int qtdb, qtdp;
  int ordem[64][7];
//coloca todas pe�as do tabuleiro em ordem de valor
//64 casas, cada uma com 7 info: 0:i, 1:j, 2:peca,
//3: qtdataquebranco, 4: menorb, 5: qtdataquepreto, 6: menorp
// Estranho de implementar isso. Precisa de mais testes
// Quando est� avaliando uma posi�ao e o tempo estourou, por
// via das d�vidas � melhor n�o confiar muito nessa posi��o...
//------------------------------------------------------------
//ignorando a avalia��o est�tica, pois precisa retornar
  if (cod)
    {
      k = -1;
      for (i = 0; i < niv; i++)
	k *= (-1);
      return k * FIMTEMPO;	//bugbug
    }
//estando em profsuf, resultado ruim retorna invertido
//        else
//              return LIMITE;
//levando em conta a situacao do tabuleiro
  switch (tabu.situa)
    {
    case 3:
//Brancas tomaram mate
      if (tabu.vez == brancas)
//vez das brancas, mas...
	return -XEQUEMATE + 20 * (niv + 1);
//elas estao em xeque-mate. Conclusao: pessimo negocio, ganha -10000. Por�m, quanto mais distante o mate, melhor
      else
	return +XEQUEMATE - 20 * (niv + 1);
//na vez das pretas, o mate nas brancas � positivo, e quanto mais distante, pior...
    case 4:
//Pretas tomaram mate
      if (tabu.vez == pretas)
	return -XEQUEMATE + 20 * (niv + 1);
      else
	return +XEQUEMATE - 20 * (niv + 1);
    case 1:
//Empatou
      return 0;
//valor de uma posicao empatada.
    case 2:
//A posicao eh de XEQUE!
//analisada abaixo. nao precisa mais desse.
//        if (tabu.numero > 40)
//xeque no meio-jogo e final vale mais que na abertura
      if (tabu.vez == brancas)
//                totp += 30;
//            else
//                totb += 30;
//        else if (tabu.vez == brancas)
	totp += 10;		//10 centi-peoes
      else
	totb += 10;
    }

//coloca todas pe�as do tabuleiro em ordem de valor
//64 casas, cada uma com 7 info: 0:i, 1:j, 2:peca,
//3: qtdataquebranco, 4: menorb, 5: qtdataquepreto, 6: menorp

//levando em conta o valor material de cada peca,
//acha os reis.
  k = 0;
  for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  peca = tabu.tab[i][j];
	  if (abs (peca) != REI)
	    continue;
	  ordem[k][0] = i;
	  ordem[k][1] = j;
	  ordem[k][2] = peca;
	  ordem[k][3] = qataca (brancas, i, j, tabu, &ordem[k][4]);
	  ordem[k][5] = qataca (pretas, i, j, tabu, &ordem[k][6]);
	  if (peca < 0)
	    //caso REI branco
	    {
	      totp += ordem[k][5] * 20;
//                pecab+=peca; //nao soma mais o REI!
	    }
	  //pretas ganham 5 pontos por ataque ao REI
	  else
	    {
	      totb += ordem[k][3] * 20;
//                pecap+=peca; //nao soma mais o REI!
	    }
	  k++;
	  if (k > 1)
	    break;
	}
      if (k > 1)
	break;
    }
//acha damas.
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      {
	peca = tabu.tab[i][j];
	if (abs (peca) != DAMA)
	  continue;
	ordem[k][0] = i;
	ordem[k][1] = j;
	ordem[k][2] = peca;
	ordem[k][3] = qataca (brancas, i, j, tabu, &ordem[k][4]);
	ordem[k][5] = qataca (pretas, i, j, tabu, &ordem[k][6]);
	if (peca < 0)
	  {
	    //caso peca branca
	    //pretas ganham 5 pontos por ataque nela
	    totp += ordem[k][5] * 20;
	    //dama branca no ataque ganha bonus
	    if (j > 4 && tabu.numero > 30)
	      totb += 90;
	    pecab += peca;
	  }
	else
	  {
	    totb += ordem[k][3] * 20;
	    if (j < 3 && tabu.numero > 30)
	      totp += 90;
	    pecap += peca;
	  }
	k++;
      }
//acha torres.
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      {
	peca = tabu.tab[i][j];
	if (abs (peca) != TORRE)
	  continue;
	ordem[k][0] = i;
	ordem[k][1] = j;
	ordem[k][2] = peca;
	ordem[k][3] = qataca (brancas, i, j, tabu, &ordem[k][4]);
	ordem[k][5] = qataca (pretas, i, j, tabu, &ordem[k][6]);
	if (peca < 0)
	  {
	    //caso peca branca
	    //pretas ganham 5 pontos por ataque nela
	    totp += ordem[k][5] * 20;
	    //torre branca no ataque ganha bonus
	    if (j > 4)
	      totb += 90;
	    pecab += peca;
	  }
	else
	  {
	    totb += ordem[k][3] * 20;
	    if (j < 3)
	      totp += 90;
	    pecap += peca;
	  }
	k++;
      }
// acha bispos
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      {
	peca = tabu.tab[i][j];
	if (abs (peca) != BISPO)
	  continue;
	ordem[k][0] = i;
	ordem[k][1] = j;
	ordem[k][2] = peca;
	ordem[k][3] = qataca (brancas, i, j, tabu, &ordem[k][4]);
	ordem[k][5] = qataca (pretas, i, j, tabu, &ordem[k][6]);
	if (peca < 0)
	  {
	    //caso peca branca
	    totp += ordem[k][5] * 10;
	    pecab += peca;
	  }
	//pretas ganham pontos por ataque nela
	else
	  {
	    totb += ordem[k][3] * 10;
	    pecap += peca;
	  }
	k++;
      }
//acha cavalos.
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      {
	peca = tabu.tab[i][j];
	if (abs (peca) != CAVALO)
	  continue;
	ordem[k][0] = i;
	ordem[k][1] = j;
	ordem[k][2] = peca;
	ordem[k][3] = qataca (brancas, i, j, tabu, &ordem[k][4]);
	ordem[k][5] = qataca (pretas, i, j, tabu, &ordem[k][6]);
	if (peca < 0)
	  {
	    //caso peca branca
	    totp += ordem[k][5] * 10;
	    pecab += peca;
	  }
	//pretas ganham 5 pontos por ataque nela
	else
	  {
	    totb += ordem[k][3] * 10;
	    pecap += peca;
	  }
	k++;
      }
//acha pe�es.
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      {
	peca = tabu.tab[i][j];
	if (abs (peca) != PEAO)
	  continue;
	ordem[k][0] = i;
	ordem[k][1] = j;
	ordem[k][2] = peca;
	ordem[k][3] = qataca (brancas, i, j, tabu, &ordem[k][4]);
	ordem[k][5] = qataca (pretas, i, j, tabu, &ordem[k][6]);
	if (peca < 0)
	  {
	    //caso peca branca
	    totp += ordem[k][5] * 10;
	    pecab += peca;
	  }
	//pretas ganham 5 pontos por ataque nela
	else
	  {
	    totb += ordem[k][3] * 10;
	    pecap += peca;
	  }
	k++;
      }
  if (k < 64)
    ordem[k][0] = -1;
  //sinaliza o fim
  //as pecas estao em ordem de valor

  //--------------------------- lazy evaluation
  //ponto de vista branco
  //inverte sinal
  pecab = (-pecab);
  material =
    (int) ((1.0 + 75.0 / (float) (pecab + pecap)) * (float) (pecab - pecap));
  if (tabu.vez == pretas)
    material = (-material);

  //se estou melhor que a op��o do oponente, retorna a op��o do oponente
  if (material - MARGEM_PREGUICA >= -beta)
    {
//      if (debug == 2)
//      fprintf (fmini, "material>=beta (%+.2f >= %+.2f)\n", material / 100.0,
//               beta / 100.0);
      return (material - MARGEM_PREGUICA);
    }
  //se o que ganhei � menor que minha outra op��o, retorna minha outra op��o
  if (material + MARGEM_PREGUICA <= -alfa)
    {
//      if (debug == 2)
//      fprintf (fmini, "material<=alfa (%+.2f >= %+.2f)\n", material / 100.0,
//               alfa / 100.0);
      return (material + MARGEM_PREGUICA);
    }

  //usar o 'material'
  //totb+=pecab;
  //totp+=pecap;

  //e a quantidade de ataques nelas
  for (k = 0; k < 64; k++)
    {
      if (ordem[k][0] == -1)
	break;
      i = ordem[k][0];
      j = ordem[k][1];
      peca = ordem[k][2];
      if (peca < 0)
	{
	  //se peca branca:
	  //totb += abs(peca);
	  //qtdp = qataca(pretas, i, j, tabu, &menorp);
	  //se uma pe�a preta j� comeu, ela n�o pode contar de novo como atacante! (corrigir)
	  if (ordem[k][5] == 0)
	    continue;
	  //qtdb = qataca(brancas, i, j, tabu, &menorb);
	  //defesas
	  if (ordem[k][5] > ordem[k][3])
	    totp += (ordem[k][5] - ordem[k][3]) * 10;
	  else
	    totb += (ordem[k][3] - ordem[k][5] + 1) * 10;
	  if (ordem[k][6] < -peca)
	    totp += 50;
	}
      else
	{
	  //se peca preta:
	  //totp += peca;
	  //qtdb = qataca(brancas, i, j, tabu, &menorb);
	  if (ordem[k][3] == 0)
	    continue;
	  //qtdp = qataca(pretas, i, j, tabu, &menorp);
	  if (ordem[k][3] > ordem[k][5])
	    totb += (ordem[k][3] - ordem[k][5]) * 10;
	  else
	    totp += (ordem[k][5] - ordem[k][3] + 1) * 10;
	  if (ordem[k][4] < peca)
	    totb += 50;
	}
    }

//falta explicar como contar defesas com pe�as cravadas
//falta explicar que o peao passado vale muito!
//e passado protegido de pe�o vale mais
//no final, cercar o rei
//nao est� vendo 2 defesas por exemplo com P....RQ na mesma linha. O pe�o tem
//defesa da torre e da dama.

//incluindo pe�a cravada
  for (k = 2; k < 64; k++)
//tirando os dois reis da jogada k=2
    {
      if (ordem[k][0] == -1)
//todas pecas podem ser cravadas, exceto Rei
	break;
      i = ordem[k][0];
//as pecas estao em ordem de valor
      j = ordem[k][1];
      peca = ordem[k][2];
//        if (tabu.vez == brancas && peca > 0)
//nao eh meu prejuizio a pe�a cravada do adversario
//            continue;
//        if (tabu.vez == pretas && peca < 0)
//nao eh meu prejuizio a pe�a cravada do adversario
//            continue;
      tabu.tab[i][j] = VAZIA;
//imagina se essa pe�a n�o existisse?
      if (peca < 0)
	{
	  qtdp = qataca (pretas, i, j, tabu, &menorp);
	  if (qtdp > ordem[k][5] && menorp < abs (peca))
	    totb -= (abs (peca) / 7);
	}
      else
	{
	  qtdb = qataca (brancas, i, j, tabu, &menorb);
	  if (qtdb > ordem[k][3] && menorb < peca)
	    totp -= (peca / 7);
	}
//perde uma fra��o do valor da pe�a cravada;
      tabu.tab[i][j] = peca;
//recoloca a pe�a no lugar.
    }
//avaliando os peoes avancados
  for (k = 0; k < 64; k++)
    {
      if (ordem[k][0] == -1)
	break;
      if (abs (ordem[k][2]) != PEAO)
	continue;
      i = ordem[k][0];
      j = ordem[k][1];
      if (ordem[k][2] == -PEAO)
	{
//Peao branco
	  if (j > 2)
//faltando 4 casas ou menos para promover ganha +1
	    totb += 10;
	  if (j > 3)
//faltando 3 casas ou menos para promover ganha +1+1
	    totb += 20;
	  if (j > 4)
	    totb += 30;
//faltando 2 casas ou menos para promover ganha +1+1+3
	  if (j > 5)
	    totb += 40;
//faltando 1 casas ou menos para promover ganha +1+1+3+3
	}
      else
	{
	  if (j < 5)
	    totp += 10;
//faltando 4 casas ou menos para promover ganha +1
	  if (j < 4)
	    totp += 20;
//faltando 3 casas ou menos para promover ganha +1+1
	  if (j < 3)
	    totp += 30;
//faltando 2 casas ou menos para promover ganha +1+1+3
	  if (j < 2)
	    totp += 40;
//faltando 1 casas ou menos para promover ganha +1+1+3+3
	}
    }
//peao dobrado e isolado.
  isob = 1;
  isop = 1;
// Maquina de Estados de isob
  for (i = 0; i < 8; i++)
//for(j=1;j<7;j++)
// isob| achou peao na col | nao achou
    {
//  0  |   isob = 0        | isob = 1
      peaob = 0;
//inicio: 1  |   isob = 2        | isob = 1
      peaop = 0;
//  2  |   isob = 0        | isob = 3
      for (j = 1; j < 7; j++)
//for(i=0;i<8;i++)
//PEAO ISOLADO! 3  |   isob = 2        | isob = 1
	{
	  if (tabu.tab[i][j] == VAZIA)
	    continue;
	  if (tabu.tab[i][j] == -PEAO)
	    peaob++;
	  if (tabu.tab[i][j] == PEAO)
	    peaop++;
	}
//peaob e peaop tem o total de peoes na coluna (um so, dobrado, trip...)
      if (peaob != 0)
	totb -= ((peaob - 1) * 30);
//penalidade: para um so=0, para dobrado=3, trip=6!
      if (peaop != 0)
	totp -= ((peaop - 1) * 30);
      switch (isob)
	{
	case 0:
	  if (!peaob)
//se nao achou, peaob==0
	    isob++;
//isob=1, se achou isob continua 0
	  break;
	case 1:
	  if (peaob)
//se achou, peaob !=0
	    isob++;
//isob=2, se nao achou isob continua 1
	  break;
	case 2:
	  if (peaob)
//se achou...
	    isob = 0;
//isob volta a zero
	  else
//senao...
	    isob++;
//isob=3 => PEAO ISOLADO!
	  break;
	}
      if (isob == 3)
	{
	  totb -= 40;
//penalidade para peao isolado
	  isob = 1;
	}
      switch (isop)
	{
	case 0:
	  if (!peaop)
//se nao achou, peaop==0
	    isop++;
	  break;
	case 1:
	  if (peaop)
//se achou, peaop !=0
	    isop++;
	  break;
	case 2:
	  if (peaop)
//se achou...
	    isop = 0;
	  else
//senao...
	    isop++;
//isop=3 => PEAO ISOLADO!
	  break;
	}
      if (isop == 3)
	{
	  totp -= 40;
//penalidade para peao isolado
	  isop = 1;
	}
    }
  if (isob == 2)
//o PTR branco esta isolado
    totb -= 40;
  if (isop == 2)
//o PTR preto esta isolado
    totp -= 40;

  //controle do centro
  for (cor = -1; cor < 2; cor += 2)
    for (i = 2; i < 6; i++)
      //c,d,e,f
      for (j = 2; j < 6; j++)
	//3,4,5,6
	if (ataca (cor, i, j, tabu))
	  if (cor == brancas)
	    {
	      totb += 10;
	      if ((i == 3 || i == 4) && (j == 3 || j == 4))
		totb += 20;
	      //mais pontos para d4,d5,e4,e5
	    }
	  else
	    {
	      totp += 10;
	      if ((i == 3 || i == 4) && (j == 3 || j == 4))
		totp += 20;
	      //mais pontos para d4,d5,e4,e5
	    }
//bonificacao para quem nao mexeu a dama na abertura
  if (tabu.numero < 32 && setboard != 1)
    {
      if (tabu.tab[3][0] == -DAMA)
	totb += 50;
      if (tabu.tab[3][7] == DAMA)
	totp += 50;
    }
//bonificacao para quem fez roque na abertura
  if (tabu.numero < 32 && setboard != 1)
    {
      if (tabu.tab[6][0] == -REI && tabu.tab[5][0] == -TORRE)
//brancas com roque pequeno
	totb += 70;
      if (tabu.tab[2][0] == -REI && tabu.tab[3][0] == -TORRE)
//brancas com roque grande
	totb += 50;
      if (tabu.tab[6][7] == REI && tabu.tab[5][7] == TORRE)
//pretas com roque pequeno
	totp += 70;
      if (tabu.tab[2][7] == REI && tabu.tab[3][7] == TORRE)
//pretas com roque grande
	totp += 50;
    }
//bonificacao para rei protegido na abertura com os peoes do Escudo Real
  if (tabu.numero < 60 && setboard != 1)
    {
      if (tabu.tab[6][0] == -REI &&
//brancas com roque pequeno
	  tabu.tab[6][1] == -PEAO &&
	  tabu.tab[7][1] == -PEAO && tabu.tab[7][0] == VAZIA)
//apenas peoes g e h
	totb += 60;
      if (tabu.tab[2][0] == -REI &&
//brancas com roque grande
	  tabu.tab[0][1] == -PEAO &&
	  tabu.tab[1][1] == -PEAO &&
	  tabu.tab[2][1] == -PEAO &&
	  tabu.tab[0][0] == VAZIA && tabu.tab[1][0] == VAZIA)
//peoes a, b e c
	totb += 50;
      if (tabu.tab[6][7] == REI &&
//pretas com roque pequeno
	  tabu.tab[6][6] == PEAO &&
	  tabu.tab[7][6] == PEAO && tabu.tab[7][7] == VAZIA)
//apenas pe�es g e h
	totp += 60;
      if (tabu.tab[2][7] == REI &&
//pretas com roque grande
	  tabu.tab[0][6] == PEAO &&
	  tabu.tab[1][6] == PEAO &&
	  tabu.tab[2][6] == PEAO &&
	  tabu.tab[0][7] == VAZIA && tabu.tab[1][7] == VAZIA)
//peoes a, b e c
	totp += 50;
    }
//penalidade se mexer o peao do bispo, cavalo ou da torre no comeco da abertura!
  if (tabu.numero < 16 && setboard != 1)
    {
//caso das brancas------------------
      if (tabu.tab[5][1] != -PEAO)
//PBR
	totb -= 50;
      if (tabu.tab[6][1] != -PEAO)
//PCR
	totb -= 40;
      if (tabu.tab[7][1] != -PEAO)
//PTR
	totb -= 30;
      if (tabu.tab[0][1] != -PEAO)
//PTD
	totb -= 30;
      if (tabu.tab[1][1] != -PEAO)
//PCD
	totb -= 40;
//              if(tabu.tab[2][1]==VAZIA)
//PBD   nao eh penalizado!
//                      totb-=10;
//caso das pretas-------------------
      if (tabu.tab[5][6] != PEAO)
//PBR
	totp -= 50;
      if (tabu.tab[6][6] != PEAO)
//PCR
	totp -= 40;
      if (tabu.tab[7][6] != PEAO)
//PTR
	totp -= 30;
      if (tabu.tab[0][6] != PEAO)
//PTD
	totp -= 30;
      if (tabu.tab[1][6] != PEAO)
//PCD
	totp -= 40;
//PBD   nao eh penalizado!
    }
//prepara o retorno:----------------
  if (tabu.vez == brancas)
// se vez das brancas
    return material + totb - totp;
// retorna positivo se as brancas estao ganhando (ou negativo c.c.)
  else
// se vez das pretas
    return material + totp - totb;
// retorna positivo se as pretas estao ganhando (ou negativo c.c.)
}

//insere o tabuleiro tabu na lista de tabuleiros
void
insere_listab (tabuleiro tabu)
{
  int i, j, flag = 0;
  listab *plaux;
  if (plcabeca == NULL)
//lista vazia?
    {
      plcabeca = (listab *) malloc (sizeof (listab));
      if (plcabeca == NULL)
	msgsai ("Erro ao alocar memoria em insere_listab 1", 32);
      plfinal = plcabeca;
      plcabeca->ant = NULL;
      plfinal->prox = NULL;
      for (i = 0; i < 8; i++)
	for (j = 0; j < 8; j++)
	  plfinal->tab[i][j] = tabu.tab[i][j];
//posi��o das pe�as
      plfinal->vez = tabu.vez;
//de quem � a vez
      plfinal->rep = 1;
//primeira vez que aparece
      plfinal->peao_pulou = tabu.peao_pulou;
//contem coluna do peao adversario que andou duas, ou -1 para nenhuma
      plfinal->roqueb = tabu.roqueb;
      plfinal->roquep = tabu.roquep;
//1:pode para os 2 lados. 0:nao pode mais. 3:mexeu TD. 2:mexeu TR.
      plfinal->empate_50 = tabu.empate_50;
//contador:quando chega a 50, empate.
      plfinal->situa = tabu.situa;
//0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.)
      for (i = 0; i < 4; i++)
	plfinal->lancex[i] = tabu.lancex[i];
//lance executado originador deste tabuleiro.
      plfinal->numero = tabu.numero;
//numero do lance
      plfinal->especial = tabu.especial;
    }
  else
    {
      plfinal->prox = (listab *) malloc (sizeof (listab));
      if (plfinal->prox == NULL)
	msgsai ("Erro ao alocar memoria em insere_listab 2", 33);
      plaux = plfinal;
      plfinal = plfinal->prox;
      plfinal->ant = plaux;
      plfinal->prox = NULL;
      for (i = 0; i < 8; i++)
	for (j = 0; j < 8; j++)
	  plfinal->tab[i][j] = tabu.tab[i][j];
      plfinal->vez = tabu.vez;
      plfinal->peao_pulou = tabu.peao_pulou;
//contem coluna do peao adversario que andou duas, ou -1 para nenhuma
      plfinal->roqueb = tabu.roqueb;
      plfinal->roquep = tabu.roquep;
//1:pode para os 2 lados. 0:nao pode mais. 3:mexeu TD. 2:mexeu TR.
      plfinal->empate_50 = tabu.empate_50;
//contador:quando chega a 50, empate.
      plfinal->situa = tabu.situa;
//0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.)
      for (i = 0; i < 4; i++)
	plfinal->lancex[i] = tabu.lancex[i];
//lance executado originador deste tabuleiro.
      plfinal->numero = tabu.numero;
//numero do lance
      plfinal->especial = tabu.especial;
//compara o inserido agora com todos os anteriores...
      plaux = plfinal->ant;
      while (plaux != NULL)
	{
	  flag = 0;
	  for (i = 0; i < 8; i++)
	    {
	      for (j = 0; j < 8; j++)
		{
		  if (plfinal->tab[i][j] != plaux->tab[i][j])
		    {
//para ser diferente tem que mudar as pe�as e a vez
		      flag = 1;
		      break;
		    }
//flag==1:tabuleiro diferente nao eh posicao repetida.
		}
	      if (flag)
		break;
	    }
	  if (plfinal->vez != plaux->vez)
	    flag = 1;
//flag==1: vez diferente nao eh posicao repetida
	  if (!flag)
//flag == 0?
	    {
	      plfinal->rep = plaux->rep + 1;
	      break;
	    }
	  plaux = plaux->ant;
	}
      if (flag)
//flag == 1?
	plfinal->rep = 1;
    }
}

//retira o plfinal: o �ltimo tabuleiro da lista
void
retira_listab (void)
{
  listab *plaux;
  if (plfinal == NULL)
    return;
  plaux = plfinal->ant;
  free (plfinal);
  if (plaux == NULL)
    {
      plcabeca = NULL;
      plfinal = NULL;
      return;
    }
  plfinal = plaux;
  plfinal->prox = NULL;
}

void
volta_lance (tabuleiro * tabu)
//para voltar um lance
{
  int i, j;
  if (plcabeca == NULL || plfinal == NULL)
//nao tem ningu�m... erro.
    return;
  if (plcabeca == plfinal)
//j� est� na posi��o inicial
    return;
  retira_listab ();
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      tabu->tab[i][j] = plfinal->tab[i][j];
//posi��o das pe�as
  tabu->vez = plfinal->vez;
//de quem � a vez
  tabu->peao_pulou = plfinal->peao_pulou;
//contem coluna do peao adversario que andou duas, ou -1 para nenhuma
  tabu->roqueb = plfinal->roqueb;
  tabu->roquep = plfinal->roquep;
//1:pode para os 2 lados. 0:nao pode mais. 3:mexeu TD. 2:mexeu TR.
  tabu->empate_50 = plfinal->empate_50;
//contador:quando chega a 50, empate.
  tabu->situa = plfinal->situa;
//0:nada,1:Empate!,2:Xeque!,3:Brancas em mate,4:Pretas em mate,5 e 6: Tempo (Brancas e Pretas respec.)
  for (i = 0; i < 4; i++)
    tabu->lancex[i] = plfinal->lancex[i];
//lance executado originador deste tabuleiro.
  tabu->numero = plfinal->numero;
//numero do lance
  tabu->especial = plfinal->especial;
//0:nada. 1:roque pqn. 2:roque grd. 3:comeu enpassant.
//promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
  if (tabu->numero < 50 && setboard < 1)
//nao usar abertura em posi��es de setboard
    USALIVRO = 1;
}

void
retira_tudo_listab (void)
//zera a lista de tabuleiros
{
  while (plcabeca != NULL)
    retira_listab ();
}

int
qataca (int cor, int col, int lin, tabuleiro tabu, int *menor)
{
//retorna o numero de ataques que a "cor" faz na casa(col,lin)
//cor==brancas   => brancas atacam casa(col,lin)
//cor==pretas    => pretas  atacam casa(col,lin)
//menor == � a menor pe�a da "cor" que ataca a casa
  int icol, ilin, casacol, casalin;
  int total = 0;
  *menor = REI;
//torre ou dama atacam a casa...
  for (icol = col - 1; icol >= 0; icol--)
//desce coluna
    {
      if (tabu.tab[icol][lin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[icol][lin] == -TORRE || tabu.tab[icol][lin] == -DAMA)
	  {
	    total++;
	    if (-tabu.tab[icol][lin] < *menor)
	      *menor = -tabu.tab[icol][lin];
	    break;
	  }
	else
	  break;
      else
// pretas atacam a casa
      if (tabu.tab[icol][lin] == TORRE || tabu.tab[icol][lin] == DAMA)
	{
	  total++;
	  if (tabu.tab[icol][lin] < *menor)
	    *menor = tabu.tab[icol][lin];
	  break;
	}
      else
	break;
    }
  for (icol = col + 1; icol < 8; icol++)
//sobe coluna
    {
      if (tabu.tab[icol][lin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[icol][lin] == -TORRE || tabu.tab[icol][lin] == -DAMA)
	  {
	    total++;
	    if (-tabu.tab[icol][lin] < *menor)
	      *menor = -tabu.tab[icol][lin];
	    break;
	  }
	else
	  break;
      else if (tabu.tab[icol][lin] == TORRE || tabu.tab[icol][lin] == DAMA)
	{
	  total++;
	  if (tabu.tab[icol][lin] < *menor)
	    *menor = tabu.tab[icol][lin];
	  break;
	}
      else
	break;
    }
  for (ilin = lin + 1; ilin < 8; ilin++)
// direita na linha
    {
      if (tabu.tab[col][ilin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[col][ilin] == -TORRE || tabu.tab[col][ilin] == -DAMA)
	  {
	    total++;
	    if (-tabu.tab[col][ilin] < *menor)
	      *menor = -tabu.tab[col][ilin];
	    break;
	  }
	else
	  break;
      else if (tabu.tab[col][ilin] == TORRE || tabu.tab[col][ilin] == DAMA)
	{
	  total++;
	  if (tabu.tab[col][ilin] < *menor)
	    *menor = tabu.tab[col][ilin];
	  break;
	}
      else
	break;
    }
  for (ilin = lin - 1; ilin >= 0; ilin--)
// esquerda na linha
    {
      if (tabu.tab[col][ilin] == VAZIA)
	continue;
      if (cor == brancas)
	if (tabu.tab[col][ilin] == -TORRE || tabu.tab[col][ilin] == -DAMA)
	  {
	    total++;
	    if (-tabu.tab[col][ilin] < *menor)
	      *menor = -tabu.tab[col][ilin];
	    break;
	  }
	else
	  break;
      else
//pecas pretas atacam
      if (tabu.tab[col][ilin] == TORRE || tabu.tab[col][ilin] == DAMA)
	{
	  total++;
	  if (tabu.tab[col][ilin] < *menor)
	    *menor = tabu.tab[col][ilin];
	  break;
	}
      else
	break;
    }
// cavalo ataca casa...
  for (icol = -2; icol < 3; icol++)
    for (ilin = -2; ilin < 3; ilin++)
      {
	if (abs (icol) == abs (ilin) || icol == 0 || ilin == 0)
	  continue;
	if (col + icol < 0 || col + icol > 7 || lin + ilin < 0
	    || lin + ilin > 7)
	  continue;
	if (cor == brancas)
//cavalo branco ataca?
	  if (tabu.tab[col + icol][lin + ilin] == -CAVALO)
	    {
	      total++;
//sim,ataca!
	      if (CAVALO < *menor)
		*menor = CAVALO;
	    }
	  else
	    continue;
	else
//cavalo preto ataca?
	if (tabu.tab[col + icol][lin + ilin] == CAVALO)
	  {
	    if (CAVALO < *menor)
	      *menor = CAVALO;
	    total++;
//sim,ataca!
	  }
      }
// bispo ou dama atacam casa...
  for (icol = -1; icol < 2; icol += 2)
    for (ilin = -1; ilin < 2; ilin += 2)
      {
	casacol = col;
//para cada diagonal, comece na casa origem.
	casalin = lin;
	do
	  {
	    casacol = casacol + icol;
	    casalin = casalin + ilin;
	    if (casacol < 0 || casacol > 7 || casalin < 0 || casalin > 7)
	      break;
	  }
	while (tabu.tab[casacol][casalin] == 0);

	if (casacol >= 0 && casacol <= 7 && casalin >= 0 && casalin <= 7)
	  if (cor == brancas)
	    if (tabu.tab[casacol][casalin] == -BISPO
		|| tabu.tab[casacol][casalin] == -DAMA)
	      {
		total++;
		if (-tabu.tab[casacol][casalin] < *menor)
		  *menor = -tabu.tab[casacol][casalin];
		continue;
//proxima diagonal
	      }
	    else
	      continue;
	// achou peca, mas esta nao anda em diagonal ou e' peca propria
	  else
	    //ataque de peca preta
	  if (tabu.tab[casacol][casalin] == BISPO
		|| tabu.tab[casacol][casalin] == DAMA)
	    {
	      total++;
	      if (tabu.tab[casacol][casalin] < *menor)
		*menor = tabu.tab[casacol][casalin];
	      continue;
//proxima diagonal
	    }
      }
// proxima diagonal
// ataque de rei...
// nao preciso colocar como *menor, pois � a maior e ja come�a com ele
  for (icol = col - 1; icol <= col + 1; icol++)
    for (ilin = lin - 1; ilin <= lin + 1; ilin++)
      {
	if (icol == col && ilin == lin)
	  continue;
	if (icol < 0 || icol > 7 || ilin < 0 || ilin > 7)
	  continue;
	if (cor == brancas)
	  if (tabu.tab[icol][ilin] == -REI)
	    {
	      total++;
	      break;
	    }
	  else
	    continue;
	else if (tabu.tab[icol][ilin] == REI)
	  {
	    total++;
	    break;
	  }
      }
  if (cor == brancas)
// ataque de peao branco
    {
      if (lin > 1)
	{
	  ilin = lin - 1;
	  if (col - 1 >= 0)
	    if (tabu.tab[col - 1][ilin] == -PEAO)
	      {
		if (PEAO < *menor)
		  *menor = PEAO;
		total++;
	      }
	  if (col + 1 <= 7)
	    if (tabu.tab[col + 1][ilin] == -PEAO)
	      {
		if (PEAO < *menor)
		  *menor = PEAO;
		total++;
	      }
	}
    }
  else
//ataque de peao preto
    {
      if (lin < 6)
	{
	  ilin = lin + 1;
	  if (col - 1 >= 0)
	    if (tabu.tab[col - 1][ilin] == PEAO)
	      {
		if (PEAO < *menor)
		  *menor = PEAO;
		total++;
	      }
	  if (col + 1 <= 7)
	    if (tabu.tab[col + 1][ilin] == PEAO)
	      {
		if (PEAO < *menor)
		  *menor = PEAO;
		total++;
	      }
	}
    }
  if (total == 0)
//ninguem ataca
    *menor = 0;
  return (total);
}

// ----------------------------------------------------------------------------
// Livro de aberturas
void
listab2string (char *strlance)
//pega a lista de tabuleiros e cria uma string de movimentos
{
  listab *plaux;
  char m[8];
  int n = 0, i;
  if (plcabeca == NULL)
    {
      strlance = NULL;
      return;
    }
  plaux = plcabeca->prox;
//o primeiro tabuleiro nao tem lancex
  while (plaux != NULL)
    {
      lance2movi (m, plaux->lancex, plaux->especial);
//flag_50 nao esta sendo usada!
      m[4] = '\0';
      for (i = 0; i < 4; i++)
	{
	  strlance[n + i] = m[i];
	}
      n += 4;
      strlance[n] = ' ';
      n++;
      plaux = plaux->prox;
//gira o la�o
    }
  strlance[n] = '\0';
  return;
}

//retorna uma linha de jogo como se fosse a resposta do minimax
//com inicio no lance da vez.
movimento *
string2pmovi (int numero, char *linha)
{
  char m[8];
  int n = 0, lanc[4], i, conta = 0;
  movimento *pmovi, *pmoviant, *cabeca;
//posi��o inicial
  tabuleiro tab = {
    {
     {-TORRE, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, TORRE},
     {-CAVALO, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, CAVALO},
     {-BISPO, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, BISPO},
     {-DAMA, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, DAMA},
     {-REI, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, REI},
     {-BISPO, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, BISPO},
     {-CAVALO, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, CAVALO},
     {-TORRE, -PEAO, VAZIA, VAZIA, VAZIA, VAZIA, PEAO, TORRE}
     },
    -1, -1, 1, 1, 0, 0,
    {0, 0, 0, 0},
    0, 0
  };
  cabeca = NULL;
  pmoviant = NULL;
  pmovi = NULL;
  while (linha[n] != '\0')
    {
      n++;
      conta++;
    }
  n = 0;
  while (n < conta - 1)
    {
      for (i = 0; i < 4; i++)
	m[i] = linha[n + i];
      m[4] = '\0';
      movi2lance (lanc, m);
      pmovi = valido (tab, lanc);
//lanc eh int lanc[4]; cria pval com tudo preenchido
      if (pmovi == NULL)
	break;
//chamar joga_em() apenas para atualizar esse tabuleiro local, para usar a fun��o valido()
      disc = (char) joga_em (&tab, *pmovi, 0);
//a funcao joga_em deve inserir no listab, cod: 1:insere, 0:nao insere
      if (n / 5 >= numero)
//chegou na posi��o atual! come�a inserir na lista
	{
	  if (cabeca == NULL)
	    {
	      cabeca = pmovi;
	      pmoviant = pmovi;
	    }
	  else
	    {
	      pmoviant->prox = pmovi;
	      pmoviant = pmoviant->prox;
	      pmoviant->prox = NULL;
	    }
	}
      pmovi = NULL;
      n += 5;
    }
  return cabeca;
}

int
igual_strlances_strlinha (char *strlances, char *strlinha)
//retorna verdadeiro se o jogo atual casa com a linha do livro atual
{
  int i = 0;
  if (strlances[0] == '\0')
    return 1;
  while (strlances[i] != '\0')
    {
      if (strlances[i] != strlinha[i])
	return 0;
      i++;
    }
  return 1;
}

void
usalivro (tabuleiro tabu)
//retorna em result.plance uma variante do livro
{
  movimento *cabeca;
  char linha[256], strlance[256];
  FILE *flivro;
  int i = 0, sorteio;
  int novovalor;
  cabeca = NULL;
  flivro = fopen ("livro.txt", "r");
  if (!flivro)
    {
      if (!xboard)
	printf ("Erro ao ler o livro de abertura: livro.txt nao existe.\n");
      USALIVRO = 0;
    }
  else
    {
      if (tabu.numero == 0)
// No primeiro lance, sorteia uma abertura
	{
	  sorteio = (int) rand_minmax (0, LINHASDOLIVRO);
//maximo de linhas no livro!
	  while (!feof (flivro) && i < sorteio)
	    {
	      (void) fgets (linha, 256, flivro);
	      i++;
	    }
	  while (linha[0] == '#')
//a ultima linha n�o deve ser coment�rio
	    (void) fgets (linha, 256, flivro);
	  cabeca = string2pmovi (tabu.numero, linha);
	  result.valor = estatico_pmovi (tabu, cabeca);
	  libera_lances (result.plance);
	  result.plance = copilistmov (cabeca);
	}
      else
//Responde com a primeira abertura que achar (primeira tentativa)
	{
//Pega a primeira, e troca por outras via sorteio. (segunda tentativa)
//mudar isso para avaliar as linhas e escolher a melhor (terceira)
	  listab2string (strlance);
//pega a lista de tabuleiros e faz uma string
	  while (!feof (flivro))
	    {
	      (void) fgets (linha, 256, flivro);
	      if (igual_strlances_strlinha (strlance, linha))
		{
		  if (cabeca == NULL)
		    {
		      cabeca = string2pmovi (tabu.numero, linha);
//eh a primeira linha que casou
		      result.valor = estatico_pmovi (tabu, cabeca);
		      libera_lances (result.plance);
		      result.plance = copilistmov (cabeca);
		    }
		  else
		    {
//                        sorteio=(int)rand_minmax(0,100);
//sorteia num. entre zero e 100 inclusive
//                        if(sorteio>85)
//as melhores linhas devem estar no topo do livro (so troca para baixo com P()=15%
//                        {
//                            libera_lances(cabeca);
//                              cabeca=string2pmovi(tabu.numero, linha);
//                        }
// Ordenar por valor est�tico da variante
		      libera_lances (cabeca);
		      cabeca = string2pmovi (tabu.numero, linha);
//eh a primeira linha que casou
		      novovalor = estatico_pmovi (tabu, cabeca);
		      if (novovalor > result.valor)
			{
			  result.valor = novovalor;
			  libera_lances (result.plance);
			  result.plance = copilistmov (cabeca);
			}
		    }
//                    libera_lances(cabeca);
		}
//se achou no livro a abertura atual
	    }
//while tem linhas no arquivo
	}
//else n�o � o primeiro lance
    }
//erro ao abrir arquivo Livro.txt
  if (flivro)
    (void) fclose (flivro);
  return;
}

char
pega (char *no, char *msg)
//pegar caracter (linux e windows)
{
  int p;
  printf ("%s", msg);
  p = getchar ();
  while (!strchr (no, p))
    {
      printf ("?");
      p = getchar ();
    }
  return (char) p;
}

float
rand_minmax (float min, float max)
//retorna um valor entre [min,max], inclusive
{
  float sorteio;
//    cloock_t s;
//      time_t t;
//      t=time(NULL);
//      s = cloock()*10000/CLOCKS_PER_SEC; // retorna cloock em milesimos de segundos...
  static int ini = 1;
  if (ini)
    {
      srand ((unsigned) (clock () * 100 / CLOCKS_PER_SEC + time (NULL)));
//+(unsigned) time(&t));
      ini--;
    }
  sorteio = (float) (rand () % 2719);
//rand retorna entre 0 e RAND_MAX;
  sorteio /= 2718.281828459;
  sorteio *= (max - min);
  sorteio += min;
  return sorteio;
}

void
sai (int error)
//termina o programa
{
  if (debug)
    {
      (void) fclose (fsaida);
//achar fun��o para LINUX/WINDOWS que apaga arquivo
//find function to LINUX/WINDOWS that remove a file (bugbug)
      if (machine == 1)
	remove ("log.xadreco.temp");
    }
  libera_lances (result.plance);
  result.plance = NULL;
  retira_tudo_listab ();
//zera a lista de tabuleiros
//  printf("error: %d",error);
  exit (error);
}

void
inicia (tabuleiro * tabu)
//inicializa vari�veis do programa. (new game)
{
  int i, j;
  pausa = 'n';
  result.valor = 0;
  libera_lances (result.plance);
  result.plance = NULL;
  retira_tudo_listab ();
  plcabeca = NULL;
//cabeca da lista de repeteco
  ofereci = 1;
//computador pode oferecer 1 empate
  USALIVRO = 1;
//use o livro nas posi��es iniciais
  if (setboard == 1)
//se for -1, deixa aparecer o primeiro.
    setboard = 0;
  tabu->roqueb = 1;
// inicializar variaveis do roque e peao_pulou
  tabu->roquep = 1;
//0:mexeu R e/ou 2T. 1:pode dos 2 lados. 2:mexeu TR. 3:mexeu TD.
  tabu->peao_pulou = -1;
//-1:o adversario nao andou duas com peao.
//0-7:coluna do peao que andou duas.
  tabu->vez = brancas;
  tabu->empate_50 = 0.0;
  tabu->numero = 0;
//(int)0.8 == 0, (int)1.3==1, (int)1.8==1, (int)2.3==2...
  ultimo_resultado[0] = '\0';
  tabu->situa = 0;
  tabu->especial = 0;
  for (i = 0; i < 4; i++)
    tabu->lancex[i] = 0;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      tabu->tab[i][j] = VAZIA;
  primeiro = 'h';
  segundo = 'h';
  nivel = 3;
  ABANDONA = -2430;
// volta o abandona para jogar contra humanos...
  COMPUTER = 0;
  mostrapensando = 0;
//post e nopost command
}

void
coloca_pecas (tabuleiro * tabu)
//coloca as pe�as na posi��o inicial
{
  int i;
  for (i = 0; i < 8; i++)
    {
      tabu->tab[i][1] = -PEAO;
      tabu->tab[i][6] = PEAO;
    }
  tabu->tab[0][0] = tabu->tab[7][0] = -TORRE;
  tabu->tab[0][7] = tabu->tab[7][7] = TORRE;
  tabu->tab[1][0] = tabu->tab[6][0] = -CAVALO;
  tabu->tab[1][7] = tabu->tab[6][7] = CAVALO;
  tabu->tab[2][0] = tabu->tab[5][0] = -BISPO;
  tabu->tab[2][7] = tabu->tab[5][7] = BISPO;
  tabu->tab[3][0] = -DAMA;
  tabu->tab[4][0] = -REI;
  tabu->tab[3][7] = DAMA;
  tabu->tab[4][7] = REI;
}

void
limpa_pensa (void)
//limpa algumas vari�veis para iniciar pondera��o
{
  libera_lances (result.plance);
  result.plance = NULL;
//eh necessario
  result.valor = -LIMITE;
//conferir
  profflag = 1;
//    totalnodo=0;
  totalnodonivel = 0;
//  teminterroga = 0;
}

void
enche_pmovi (movimento * pmovi, int c0, int c1, int c2, int c3, int p, int r,
	     int e, int f)
//preenche a estrutura movimento
{
  if (!pmovi)
    msgsai ("enche_pmovi precisa de um ponteiro v�lido", 34);
  pmovi->lance[0] = c0;
//lance em nota��o inteira
  pmovi->lance[1] = c1;
//move in integer notation
  pmovi->lance[2] = c2;
  pmovi->lance[3] = c3;
  pmovi->peao_pulou = p;
//contem coluna do peao que andou duas neste lance
  pmovi->roque = r;
//0:mexeu rei. 1:ainda pode. 2:mexeu TR. 3:mexeu TD.
  pmovi->especial = e;
//0:nada. 1:roque pqn. 2:roque grd. 3:comeu enpassant.
//promocao: 4=Dama, 5=Cavalo, 6=Torre, 7=Bispo.
  pmovi->flag_50 = f;
//Quando igual a:0=nada,1=Moveu peao,2=Comeu,3=Peao Comeu. Entao zera empate_50;
  pmovi->ordena = 0;
//utilizado como flag para ordenar a lista de movimentos segundo o valor_estatico
  pmovi->valor_estatico = 0;
//valor deste lance, dado um tabuleiro para ele.
}

void
msgsai (char *msg, int error)
//aborta programa por falta de mem�ria
{
  if (!xboard)
    printf ("\n%s\n", msg);
  else if (debug)
    fprintf (fsaida, "\nxadreco : %s\n", msg);
  sai (error);
}

//cuidado para n�o estourar o limite da cadeia, teclando muito <t>
//be carefull to do not overflow the string limit, pressing to much <t>
void
testajogo (char *movinito, int numero)
//retorna um lance do jogo de teste
{
//      char *jogo1="e2e4 e7e5 g1f3 b8c6 b1c3 g8f6 f1b5 a7a6 b5c6 d7c6 d2d4 e5d4 f3d4 f8c5 c1e3 d8e7 e1g1 f6e4 d1f3 c5d4 e3d4 e4d2 f3g3 d2f1 g3g7 h8f8 a1f1 c8f5 d4f6 e7c5 f6d4 c5d6 d4e5 d6b4 a2a3 b4b6 f1c1 f7f6 g7c7 b6c5 e5d6 f8f7 c1e1 c5e5 e1e5 f6e5 c7b6 f5c2 d6c7 a8c8 c7e5 f7e7 b6d4 c8d8";
//      char *jogo1="e2e4 e7e5 f1c4 b8c6 d1h5 g8f6 h5f7";
//mate do pastor
//      char *jogo1="g1f3 g8f6 c2c4 b7b6 g2g3 c8b7 f1g2 d4d5 e1g1 d4c4 d1a4 c7c6 a4c4 b7a6 c4d4 a6e2 f1e1";
//problema de travamento
//ultimo lance:
//36. e1c1 b2c1  ou
//36. Tc1 Dc1+
//pr�ximo lance:
//37.e2e1 c1e1++ ou
//37. Te1 Dxe1++
  char *jogo1 =
    "g1f3 g8f6 c2c4 b7b6 g2g3 c8b7 f1g2 d7d5 e1g1 d5c4 d1a4 c7c6 a4c4 b7a6 c4d4 a6e2 f1e1";
//travou computador
  char move[5];
  int i;

  move[0] = '\0';
  for (i = 0; i < 4; i++)
//int(numero)*5; i<int(numero)*5+4; i++)
// 0.3 0.8 1.3 1.8
    {
      move[i] = jogo1[numero * 5 + i];
    }
  strcpy (movinito, move);
  printf ("%d:%s\n", (numero + 2) / 2, move);
}

void
testapos (char *pieces, char *color, char *castle, char *enpassant,
	  char *halfmove, char *fullmove)
//testa posi��o
{
//      char *p1="
//      char *p1="r3k2r/ppp3pp/4p3/2Bq4/3P2n1/6Q1/bK2PPPP/3R1B1R b kq - 0 18";
//      char *p1="8/3k4/2p5/3p4/1B1P3P/3K4/4Q3/8 b - - 0 51";
//      char *p1="8/4Q3/k1p5/2Bp4/3P3P/1K6/8/8 w - - 0 6";
//      char *p1="rnbqk2r/p3bppp/2p1pn2/1p4B1/3P4/1QN2N2/PP2PPPP/R3KB1R b KQkq - 0 8";
//      char *p1="rQbqk2r/p3bppp/2p1pn2/6B1/3P4/2p2N2/PP2PPPP/2KR1B1R b kq - 0 3";
//      char *p1="2r1k3/1p2r2p/p1p5/4B3/3Q4/P1N5/1Pb2PPP/6K1 b - - 0 27";
//      char *p1="1b3k2/Ppp4p/8/r5pR/6P1/r6P/1P3P2/5K2 w - - 0 1";
//      char *p1="1b3k2/Ppp3Rp/8/r5p1/6P1/r4q1P/1P3P2/5K2 b - - 0 1";
//      char *p1="7k/bp5P/1P6/8/8/8/8/7K w - - 0 1";
//      char *p1="7k/7P/8/1p6/b4pr1/7K/8/N7 w - - 0 1";
//      char *p1="7k/7P/8/1p6/b7/5pr1/7K/N7 w - - 0 1";
//      char *p1="7k/7P/8/6pK/8/8/8/8 w - - 0 1";
//erro na avalia��o! perde partida empatada
//      char *p1="8/7k/7P/8/8/6pK/8/8 b - - 0 1";
//    char           *p1 = "8/7k/7P/8/8/6pK/8/8 w - - 0 1";
//fiz a arvore completa
//      char *p1="8/8/8/8/8/k1Q5/2K5/8 b - - 0 1";
//mate em 5, com possibilidades de afogamento e de mate em 1.
  char *p1 = "8/8/8/8/4BB2/1K6/7p/k7 w - - 0 1";
//2 bispos... mate em 1, ou em 2, ou em 3... Se comer pe�o, afoga
//      char *p1="k7/2K5/1P6/2B5/3p4/8/8/8 w - - 0 1";
//mate de peao em 1 ply ou em 3 ply. se comer pe�o, afoga
//      char *p1="4k3/P6P/RP4PR/1NPPPPN1/2BQKB2/8/8/8 w - - 0 1";
//est� se sentindo sozinho?
//      char *p1="r1bqkb1r/1p3pp1/p1nppn1p/6B1/3NP3/2N5/PPPQ1PPP/2KR1B1R w kq - 0 9";
//Move o rei na abertura? erro!
//      char *p1="r2q1rk1/1Pp2p1p/3pb1p1/p3p3/4PPn1/2P2N2/P1P1B1PP/R2QK1BR b KQ - 0 12";
//      char *p1="r1b1kb1r/1p1n1ppp/4pn1B/1p1p4/1PPN4/2p5/P3PPPq/1R2KBR1 w kq - 0 12";
//g2g3, sendo que podia salvar a torre com d4f3
//    char *p1="6k1/p2rnppp/Rp1pr3/1Pp1p1PP/2P1P1K1/2RQ4/B4P2/4N3 w - - 50 80";
//      char *p1="8/3p4/4k3/8/4P3/2K5/2P5/BB6 w - - 50 1"; //procurou o mate
//      char *p1="r1bq1bnr/pp3ppp/8/2P1N3/R4P1k/2pB2P1/1PPPQ2P/2B1K2R b K - 0 16";

  sscanf (p1, "%s %s %s %s %s %s", pieces, color, castle, enpassant, halfmove,
	  fullmove);
}

//imprime uma sequencia de lances armazenada na lista movimento
void
imprime_linha (movimento * loop, int numero, int tabuvez)
//tabuvez==2 para pular numera��o em debug==2
{
  int num, vez;
  char m[80];
  FILE *fimprime;
  num = (int) ((numero + 1.0) / 2.0);
  vez = tabuvez;
  fimprime = NULL;
  if (debug == 2)
    fimprime = fmini;
  if (debug == 1)
    fimprime = fsaida;
  while (loop != NULL)
    {
      lance2movi (m, loop->lance, loop->especial);
      if (vez == brancas)
//jogou anterior as pretas
	{
	  if (tabuvez == brancas && num == (int) ((numero + 1.0) / 2.0))
	    {
	      printf ("%d. ... %s ", num, m);
//primeiro lance da variante
	      if (fimprime != NULL)
		fprintf (fimprime, "%d. ... %s ", num, m);
//primeiro lance da variante
	    }
	  else
	    {
	      if (tabuvez != 2)
		printf ("%s ", m);
	      if (fimprime != NULL)
		fprintf (fimprime, "%s ", m);
	    }
	  num++;
	}
      else
// jogou anterior as brancas
	{
	  if (tabuvez == 2)
//codigo so para log, n�o imprimir na tela.
	    {
	      if (fimprime != NULL)
		fprintf (fimprime, "%s ", m);
	    }
	  else
//vez das pretas, normal. Tela e log.
	    {
	      printf ("%d. %s ", num, m);
	      if (fimprime != NULL)
		fprintf (fimprime, "%d. %s ", num, m);
	    }
	}
      loop = loop->prox;
      vez *= (-1);
    }
}

//void ordena_succ(void){}
//para testar sem a fun��o ordena_succ. (notei que h� ganho no corte de nodos)
void
ordena_succ (int nmov)
//ordena succ_geral
{
  movimento *loop, *pior, *insere, *aux;
  int peguei, pior_valor;
  insere = NULL;
  pior = NULL;
  loop = succ_geral;
  if (loop == NULL)
    //se a lista esta vazia
    return;
  while (loop != NULL)
    {
      if (loop->flag_50 > 1 || loop->especial)
	//para manter os especiais em primeiro
	loop->ordena = 1;
      else
	loop->ordena = 0;
      loop = loop->prox;
    }
  peguei = 1;
  while (peguei)
    {
      pior_valor = +XEQUEMATE + 1;
      loop = succ_geral;
      peguei = 0;
      while (loop != NULL)
	{
	  if (loop->valor_estatico <= pior_valor && loop->ordena != 1)
	    {
	      pior = loop;
	      pior_valor = loop->valor_estatico;
	      peguei = 1;
	    }
	  loop = loop->prox;
	}
      //fim do while loop, e consequentemente da lista
      if (peguei)
	{
	  aux = copimel (*pior, insere);
	  //insere pior na cabe�a (a lista  fica em ordem decrescente: o melhor na cabe�a)
	  libera_lances (insere);
	  insere = aux;
	  pior->ordena = 1;
	}
    }
  //fim do while trocou
  //os primeiros ficam intactos  //conta com a fun��o geramov, que coloca os especiais na cabe�a
  loop = succ_geral;
  while (loop != NULL)
    if (loop->flag_50 > 1 || loop->especial)
      {
	aux = copimel (*loop, insere);
	//movimento *copimel(movimento ummovi, movimento *plan)
	libera_lances (insere);
	insere = aux;
	loop = loop->prox;
      }
    else
      break;
  libera_lances (succ_geral);
  succ_geral = insere;
}

int
estatico_pmovi (tabuleiro tabu, movimento * cabeca)
//retorna o valor est�tico de um tabuleiro ap�s jogada a lista de movimentos cabeca
{
//cod: 1: acabou o tempo, 0: ou eh avalia��o normal?
//niv: qual a dist�ncia do tabuleiro real para a c�pia tabu avaliada?
  int niv = 0;
  while (cabeca != NULL)
    {
      disc = (char) joga_em (&tabu, *cabeca, 0);
//a funcao joga_em deve inserir no listab, cod: 1:insere, 0:nao insere
//com isso surge o problema de que o programa n�o detecta empate por 3 repeti��es enquanto l� o livro de aberturas.
//a solu��o simples �: n�o incluir no livro de abertura linhas que empatam com tr�s repeti��es na fase de abertura (se � que isso existe!)
      cabeca = cabeca->prox;
      niv++;
    }
  return estatico (tabu, 0, niv, -LIMITE, LIMITE);
//      return rand_minmax(-8, +8);
}

// pollinput() Emprestado do jogo de xadrez pepito: dica de Fabio Maia
// pollinput() Borrowed from pepito: tip from Fabio Maia
int
pollinput (void)
// retorna verdadeiro se existe algum caracter no buffer para ser lido
// return true if there are some character in the buffer to be read
{
#ifndef _WIN32
  static fd_set readfds;
  struct timeval tv;
  int ret;

  FD_ZERO (&readfds);
  FD_SET (fileno (stdin), &readfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select (16, &readfds, NULL, NULL, &tv);
  ret = FD_ISSET (fileno (stdin), &readfds);
  return (ret);
#else
  static int init = 0;
  static int pipe;
  static HANDLE inh;
  static INPUT_RECORD Buffer[256];

  DWORD dw = 0;
  if (!init)
    {
      init = 1;
      inh = GetStdHandle (STD_INPUT_HANDLE);
      pipe = !GetConsoleMode (inh, &dw);
      if (!pipe)
	{
	  SetConsoleMode (inh,
			  dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
	  FlushConsoleInputBuffer (inh);
	}
    }
  if (pipe)
    {
      if (!PeekNamedPipe (inh, NULL, 0, NULL, &dw, NULL))
	return 1;
      return dw;
    }
  else
    {
      GetNumberOfConsoleInputEvents (inh, &dw);
      return (dw <= 1 ? 0 : dw);
    }
#endif
}

//retorna verdadeiro se leu o comando "?" para mover agora
//return true if there is a "?" command to move now
//int moveagora(void)
