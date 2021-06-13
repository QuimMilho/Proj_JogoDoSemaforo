//                          DEIS-ISEC
//              Trabalho pratico programacao 2020/2021
//      Joaquim Milheiro
//      2020131794

#ifndef TRABALHOFINAL_V2_STRUCTS_H
#define TRABALHOFINAL_V2_STRUCTS_H

//Vetores
//Serve para guardar posicoes e dimensoes ao longo do programa

typedef struct vetor vetor;

struct vetor {
    //Coordenadas
    short x, y;
};

//Tabuleiro
//Guarda todas as informacoes relativas ao tabuleiro do jogo

typedef struct tab tabuleiro;

struct tab {
    //Dimensao do tabuleiro
    vetor dim;
    //Pecas do tabuleiro
    short *pecas;
};

//Jogadores
//Guarda todas as informacoes relativas aos jogadores

typedef struct jogador player;

struct jogador {
    //Numero de pedras
    short pedrasAdd;
    //Numero de partes do tabuleiro adicionadas
    short partesAdd;
    //Tipo de jogador
    short tipoDeJogador;
    //Numero do jogador
    short nJogador;
};

//Jogadas

typedef struct jogadas jogada;

struct jogadas {
    //Tipo da jogada
    short tipo;
    //Posicao da jogada
    vetor pos;
    //Numero da jogada
    short nJogada;
    //Jogada seguinte e anterior
    jogada *nextJogada;
    jogada *prevJogada;
    //Jogador que realizou a jogada
    short playerID;
};

//Jogo

typedef struct jo jogo;

struct jo {
    //Diemnsao inicial do tabuleiro
    vetor dimInicial;
    //Tabuleiro do jogo
    tabuleiro tab;
    //Jogadores
    player *players;
    //Array das jogadas e ultimas jogadas
    jogada *jogadas;
    jogada *lastJogada;
    //Numero total de jogadas
    short nJogadas;
};

#endif //TRABALHOFINAL_V2_STRUCTS_H
