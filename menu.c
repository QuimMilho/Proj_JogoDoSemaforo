//                          DEIS-ISEC
//              Trabalho pratico programacao 2020/2021
//      Joaquim Milheiro
//      2020131794

#include <stdio.h>

#include "menu.h"
#include "consts.h"

void printMenuPart(int index, char *texto);

//Menu principal
int menu() {
    printf("\n\nMenu:\n");
    printMenuPart(MENU_SAIR, "Sair do jogo");
    printMenuPart(MENU_JOGO_NORMAL, "Jogo normal");
    printMenuPart(MENU_JOGO_AI, "Jogo contra AI");
    printMenuPart(MENU_VER_JOGO, "Ver um jogo antigo");
    printf("Insira um valor:");
    int k;
    scanf("%d", &k);
    return k;
}

//Menu de carregar um jogo guardado
int menuCarregar() {
    printf("Tem um jogo guardado no seu PC!\nDeseja carregar?\n");
    printMenuPart(MENU_NAO, "Nao");
    printMenuPart(MENU_SIM, "Sim");
    printf("Insira um valor:");
    int k;
    scanf("%d", &k);
    return k;
}

//Menu para exportacao do jogo como ficheiro de texto
int menuAcabar() {
    printf("Deseja exportar o jogo para ficheiro de texto?\n");
    printMenuPart(MENU_NAO, "Nao");
    printMenuPart(MENU_SIM, "Sim");
    printf("Insira um valor:");
    int k;
    scanf("%d", &k);
    return k;
}

//Menu para cada jogada
int menuJogada(int *opt) {
    int i;
    printf("Estas sao as jogadas disponiveis:\n");
    //Percorre a array das opcoes ate encontrar uma jogada invalida
    //A partir desse momento deixa de haver opcoes
    for (i = 0; i < 8; i++) {
        if (opt[i] == JOGADA_NADA)
            break;
        switch (opt[i]) {
            case ADD_PECA_VERDE:
                printMenuPart(i, "Introduzir uma peca verde");
                break;
            case UP_PECA_AMARELA:
                printMenuPart(i, "Transformar peca verde em amarela");
                break;
            case UP_PECA_VERMELHA:
                printMenuPart(i, "Transformar peca amarela em vermelha");
                break;
            case ADD_PEDRA:
                printMenuPart(i, "Introduzir uma pedra");
                break;
            case ADD_LINHA:
                printMenuPart(i, "Adicionar uma linha");
                break;
            case ADD_COLUNA:
                printMenuPart(i, "Adicionar uma coluna");
                break;
            case DESISTENCIA:
                printMenuPart(i, "Desistir");
                break;
            case SEE_PLAYS:
                printMenuPart(i, "Ver jogadas anteriores (Nao conta como jogada!)");
                break;
        }
    }
    printf("Insira um valor:");
    int k;
    scanf("%d", &k);
    return k;
}

//Imprime as várias opções do menu
//Utilizado para facilitar a escrita do codigo
void printMenuPart(int index, char *texto) {
    printf("%d -> %s\n", index, texto);
}
