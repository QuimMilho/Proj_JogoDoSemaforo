//                          DEIS-ISEC
//              Trabalho pratico programacao 2020/2021
//      Joaquim Milheiro
//      2020131794

#ifndef TRABALHOFINAL_V2_CONSTS_H
#define TRABALHOFINAL_V2_CONSTS_H

//Ficheiro temporario

#define TEMP_FILE "jogo.bin"

//Menu

#define MENU_SAIR 0
#define MENU_JOGO_NORMAL 1
#define MENU_JOGO_AI 2
#define MENU_VER_JOGO 3
#define MENU_RELOAD 4

//Menu de carregamento/export

#define MENU_NAO 0
#define MENU_SIM 1

//Tabuleiro

#define P_PEDRA -1
#define P_NADA 0
#define P_VERDE 1
#define P_AMARELO 2
#define P_VERMELHO 3

#define TAB_MIN_INIT 3
#define TAB_MAX_INIT 5

//Jogadores

#define MAX_PLAYERS 2

#define MAX_PEDRAS 1
#define MAX_PARTES 2

#define J_JOGADOR 0
#define J_AI 1

//Jogadas
//  Estes valores devem estar entre 0 e 7 para nao causar erros em outros sitios

#define ADD_PECA_VERDE 0
#define UP_PECA_AMARELA 1
#define UP_PECA_VERMELHA 2
#define ADD_PEDRA 3
#define ADD_LINHA 4
#define ADD_COLUNA 5
#define DESISTENCIA 6
#define SEE_PLAYS 7
#define JOGADA_NADA -1

//Ficheiros

#define FILE_MAX_CHARS 100
#define FILE_NAME_MAX 33

#endif //TRABALHOFINAL_V2_CONSTS_H
