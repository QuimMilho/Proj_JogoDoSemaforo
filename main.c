//                          DEIS-ISEC
//              Trabalho pratico programacao 2020/2021
//      Joaquim Milheiro
//      2020131794

#include <stdio.h>

#include "utils.h"
#include "consts.h"
#include "jogo.h"
#include "menu.h"

int main() {
    FILE *f;
    int opt, err = 0;
    //Inicializa os numeros random
    initRandom();
    //Verifica se o ficheiro guardado em TEMP_FILE existe ou nao
    if ((f = fopen(TEMP_FILE, "rb"))) {
        //Fecha o ficheiro, uma vez que nao e necessario ler para ja
        fclose(f);
        do {
            //Vai buscar o valor de k ao respetivo menu
            opt = menuCarregar();
            switch (opt) {
                //Caso resposta seja sim
                case MENU_SIM:
                    //Carrega o jogo
                    err = jogarJogo(MENU_RELOAD);
                    break;
                case MENU_NAO:
                    //Remove o ficheiro
                    remove(TEMP_FILE);
                    break;
                default:
                    printf("Opcao invalida!\n\n");
            }
            if (err == -1) {
                printf("\n\n\nAlguma coisa forcou o programa a fechar!\nAte a proxima!\n");
                return -1;
            }
        } while (opt != MENU_SIM && opt != MENU_NAO);
    }
    do {
        opt = menu();
        switch (opt) {
            case MENU_SAIR:
                //Sai do jogo
                printf("Ate a proxima!\n");
                break;
            case MENU_JOGO_NORMAL:
                //Cria um jogo player contra player
                err = jogarJogo(MENU_JOGO_NORMAL);
                break;
            case MENU_JOGO_AI:
                //Cria um jogo player contra AI
                err = jogarJogo(MENU_JOGO_AI);
                break;
            case MENU_VER_JOGO:
                //Carrega um jogo a partir de um ficheiro de texto que exista no pc
                verJogo();
                break;
            default:
                printf("Opcao invalida!\n\n");
        }
        if (err == -1) {
            //Em caso de erro o programa vai fechar forcadamente
            printf("\n\n\nAlguma coisa forcou o programa a fechar!\nAte a proxima!\n");
            return -1;
        }
    } while (opt != MENU_SAIR);
    return 0;
}
