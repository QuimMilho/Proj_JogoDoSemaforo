//                          DEIS-ISEC
//              Trabalho pratico programacao 2020/2021
//      Joaquim Milheiro
//      2020131794

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jogo.h"
#include "consts.h"
#include "structs.h"
#include "utils.h"
#include "menu.h"

//Guarda o jogo num ficheiro binario
//Path esta registado nas consts.h como TEMP_FILE
void saveJogo(jogo *j);
//Funcao recursiva que apaga as jogadas no final do jogo
void apagarJogada(jogada *j);
//Verifica as jogadas possiveis a serem realizadas, registando a ordem no ponteiro
//Utilizado para ver as jogadas que se podem fazer no menu e para a AI selecionar
void verificarJogadas(jogo *j, int *opts);
//Imprime o tabuleiro
void printTab(tabuleiro *tab);
//Guarda o texto respetivo a uma jogada no ponteiro txt para ser impresso ou guardado no ficheiro
void getPlaysText(jogada *atual, char *txt);
//Exporta o jogo para ficheiro, utilizada apenas no final do jogo
void export(jogo *j, char *path, short p);
//Remove todos os elementos dinamicos da memoria, pergunta se se quer guardar o jogo
//Apaga o ficheiro temporario
void finishJogo(jogo *j, short p);
//Da load ao jogo a partir do ficheiro temporario que e criado ao longo dos jogos, em caso de o programa ser fechado
//Da return -1 na variavel k caso haja algum erro, 0 caso contrario
void loadJogo(jogo *j, int *k);

//Gera um numero aleatorio dentro das possiveis opcoes de jogada para o bot
//Da return no indice escolhido pelo bot
int botPlay(int *opt);
//Cria o jogo do zero, utilizando os valores default. Aloca na memoria tudo o que e necessario
//Da return -1 caso haja algum erro, 0 caso contrario
int createJogo(int option, jogo *j);
//Verifica se alguem ganhou. Utilizado apos cada jogada
//Da return 1 se detetar alguma vitoria ou 0, caso contrario
int verificarVitoria(tabuleiro *tab);
//Funcao de cada jogada. E chamado no loop principal do jogo sempre que deve haver uma jogada
//Da return no vencedor, ou em caso de erro -1
int jogarJogada(jogo *j);
//Verifica se a posicao esta dentro ou fora do tabuleiro
//Return 1 caso esteja fora e 0 caso contrario
int foraDoTab(vetor dim, vetor pos);

//Pede ao utilizador a posicao onde deseja jogar
vetor introduzirPos(tabuleiro *tab);
//Gera uma posicao possivel para o bot jogar a jogada que selecionou aleatoriamente
vetor botPos(int tipo, tabuleiro *tab);

int jogarJogo(int option) {
    int k;
    jogo j;
    //Carrega ou cria o jogo
    if (option == MENU_RELOAD)
        //Carrega o jogo a partir do ficheiro cujo caminho esta em TEMP_FILE
        loadJogo(&j, &k);
    else
        //Cria um jogo do zero de acordo com a opcao selecionada
        k = createJogo(option, &j);

    //Caso haja um erro
    if (k == -1)
        return -1;
    //Guarda o vencedor do jogo
    int winner;
    do {
        //Vai buscar a jogada do respetivo player
        winner = jogarJogada(&j);
        if (winner == -1) {
            //Caso aconteca um erro ao alocar a memoria
            printf("Erro ao alocar memoria!");
            return -1;
        }
        saveJogo(&j);
    } while (winner == 0);
    //Volta a subtrair o vencedor
    winner--;
    //Anuncia o vencedor
    printf("\n\nO vencedor e o Jogador %c!\nParabens!\n\n", 'A' + winner);
    //Remove toda a memoria dinamica e pergunta se e suposto guardar o jogo ou nao
    finishJogo(&j, (short) winner);
    return 0;
}

//Criar jogo

int createJogo(int option, jogo *j) {
    int i;
    //Inicializar o n de jogadas
    j->nJogadas = 0;
    //Define a dimensao inicial do tabuleiro
    short dim = (short) intUniformRnd(TAB_MIN_INIT, TAB_MAX_INIT);
    j->dimInicial.x = dim;
    j->dimInicial.y = dim;
    //Inicializar tabuleiro e suas dimensoes
    j->tab.dim = j->dimInicial;
    j->tab.pecas = malloc(sizeof(short) * dim * dim);
    if (j->tab.pecas == NULL) {
        printf("Erro ao alocar a memoria!");
        return -1;
    }
    //Inicia as pecas como vazio
    for (i = 0; i < dim * dim; i++) {
        j->tab.pecas[i] = P_NADA;
    }
    //Inicializar players
    j->players = malloc(sizeof(player) * MAX_PLAYERS);
    if (j->players == NULL) {
        printf("Erro ao alocar a memoria!");
        return -1;
    }
    //Inicia o tipo de jogador, o jogador A (ou 0) e sempre um jogador normal!
    j->players[0].tipoDeJogador = J_JOGADOR;
    //Inicializa o numero de pedras e partes do tabuleiro adicionados
    j->players[0].pedrasAdd = 0;
    j->players[0].partesAdd = 0;
    //Define o id do jogador
    j->players[0].nJogador = 0;
    for (i = 1; i < MAX_PLAYERS; i++) {
        //Inicia o tipo de jogador de acordo com a opcao selecionada no menu
        if (option == MENU_JOGO_AI) {
            //Jogador AI
            j->players[i].tipoDeJogador = J_AI;
        } else {
            //Jogador normal
            j->players[i].tipoDeJogador = J_JOGADOR;
        }
        //Inicializa o numero de pedras e partes do tabuleiro adicionados
        j->players[i].pedrasAdd = 0;
        j->players[i].partesAdd = 0;
        //Define o id do jogador
        j->players[i].nJogador = (short) i;
    }
    //Inicializa a primeira e ultimas jogadas
    j->jogadas = NULL;
    j->lastJogada = NULL;
    //Guarda o jogo no seu estado atual
    saveJogo(j);
    return 0;
}

//Load jogo

void loadJogo(jogo *j, int *k) {
    int i;
    //Abre o ficheiro
    FILE *f = fopen(TEMP_FILE, "rb");
    //Le o ficheiro
    //Le a dimensao
    fread(&j->dimInicial, sizeof(vetor), 1, f);
    //Le o tabuleiro
    fread(&j->tab.dim, sizeof(vetor), 1, f);
    j->tab.pecas = malloc(sizeof(short) * j->tab.dim.x * j->tab.dim.y);
    if (j->tab.pecas == NULL) {
        printf("Erro ao alocar a memoria!");
        *k = -1;
        return;
    }
    fread(j->tab.pecas, sizeof(short), j->tab.dim.x * j->tab.dim.y, f);
    //Le os players
    j->players = malloc(sizeof(player) * MAX_PLAYERS);
    if (j->players == NULL) {
        printf("Erro ao alocar a memoria!");
        *k = -1;
        return;
    }
    fread(j->players, sizeof(player), MAX_PLAYERS, f);
    //Le o numero de jogadores
    fread(&j->nJogadas, sizeof(short), 1, f);
    //Le as jogadas
    jogada *atual;
    for (i = 0; i < j->nJogadas; i++) {
        //Aloca a jogada
        jogada *temp = malloc(sizeof(jogada));
        if (temp == NULL) {
            printf("Erro ao alocar a memoria!");
            *k = -1;
            return;
        }
        //Le a jogada
        fread(temp, sizeof(jogada), 1, f);
        //Ve se e a primeira jogada
        if (i == 0) {
            //Define a primeira jogada
            j->jogadas = temp;
            //Define a jogada anterior a primeira como NULL, uma vez que nao existe!
            temp->prevJogada = NULL;
        }
        else {
            //Define a jogada a seguir na lida anteriormente
            atual->nextJogada = temp;
            //Define a jogada anterior a lida atualmente como a anterior
            temp->prevJogada = atual;
        }
        //Define a proxima jogada como NULL
        temp->nextJogada = NULL;
        //Define a jogada atual como a ultima lida
        atual = temp;
    }
    //Defina a utima jogada como a ultima lida
    j->lastJogada = atual;
    //Fecha o ficheiro
    fclose(f);
    *k = 0;
}

//Guardar jogo

void saveJogo(jogo *j) {
    //Abre o ficheiro
    FILE *f = fopen(TEMP_FILE, "wb");
    //Escreve no ficheiro
    //Dimensao inicial, dimensao atual, pecas, players, numero de jogadas, jogadas
    fwrite(&j->dimInicial, sizeof(vetor), 1, f);
    fwrite(&j->tab.dim, sizeof(vetor), 1, f);
    fwrite(j->tab.pecas, sizeof(short), j->tab.dim.x * j->tab.dim.y, f);
    fwrite(j->players, sizeof(player), MAX_PLAYERS, f);
    fwrite(&j->nJogadas, sizeof(short), 1, f);
    if (j->nJogadas != 0) {
        jogada *atual = j->jogadas;
        while(atual != NULL) {
            fwrite(atual, sizeof(jogada), 1, f);
            atual = atual->nextJogada;
        }
    }
    //Fecha o ficheiro
    fclose(f);
}

//Acaba o jogo

void finishJogo(jogo *j, short p) {
    int opt, i;
    do {
        //Menu para selecionar opcao
        opt = menuAcabar();
        //Sitio onde fica guardado o ficheiro
        char path[FILE_NAME_MAX + 4];
        switch (opt) {
            case MENU_NAO:
                break;
            case MENU_SIM:
                //Selecionar o nome do ficheiro
                printf("Nomes de ficheiros nao podem incluir espacos!\nInsira o nome do ficheiro:");
                scanf("%s", path);
                //Da clear nos ultimos 5 digitos da string, usado para o caso de o utilizador introduzir
                //Mais de FILE_NAME_MAX - 1 caracteres
                for (i = 0; i < 5; i++) {
                    path[FILE_NAME_MAX - 1 + i] = '\0';
                }
                //Adiciona .txt no final do ficheiro
                strcat(path, ".txt");
                //Exporta o ficheiro
                export(j, path, p);
                break;
            default:
                printf("Opcao invalida!\n\n");
        }
    } while (opt != MENU_NAO && opt != MENU_SIM);
    //Liberta da memoria todas as jogadas
    apagarJogada(j->jogadas);
    //Liberta da memoria o tabuleiro
    free(j->tab.pecas);
    //Liberta da memoria os jogadores
    free(j->players);
    //Remove o ficheiro temporario, uma vez que o jogo ja acabou!
    remove(TEMP_FILE);
}

void export(jogo *j, char *path, short p) {
    char txt[FILE_MAX_CHARS] = "", temp[2] = "";
    //Abre o ficheiro
    FILE *f = fopen(path, "w");
    //Escreve a dimensao inicial do tabuleiro
    strcat(txt, "O tabuleiro comecou com dimensao (");
    temp[0] = (char) ('0' + j->tab.dim.x);
    strcat(txt, temp);
    strcat(txt, ", ");
    strcat(txt, temp);
    strcat(txt, ")\n");
    //Adiciona ao ficheiro
    fputs(txt, f);
    //Escreve todas as jogadas
    jogada *atual = j->jogadas;
    while(atual != NULL) {
        getPlaysText(atual, txt);
        //Adiciona ao ficheiro
        fputs(txt, f);
        //Passa a proxima jogada
        atual = atual->nextJogada;
    }
    //Indica o vencedor
    strcpy(txt, "O vencedor do jogo e o Jogador ");
    temp[0] = (char) ('A' + p);
    strcat(txt, temp);
    strcat(txt, "!");
    fputs(txt, f);
    //Fecha o ficheiro
    fclose(f);
}

void apagarJogada(jogada *j) {
    //Funcao recursiva para apagar as jogadas
    if (j->nextJogada != NULL)
        apagarJogada(j->nextJogada);
    //Liberta a jogada
    free(j);
}

//Jogadas

int jogarJogada(jogo *j) {
    //Calcula qual e o jogador que esta a jogar
    short p = (short) (j->nJogadas % MAX_PLAYERS);
    //Sitio onde vai ficar guardado o texto se for necessario imprimir alguma jogada
    char txt[FILE_MAX_CHARS] = "";
    //Done: O done e o que controla o loop. Quando a jogada acaba done = 1
    int opt, done = 0, i;
    //Vetor que guarda a posicao onde a jogada e executada
    vetor pos = {0, 0};
    do {
        //Array que guarda as opcoes do menu e as suas localizacoes
        int options[8];
        //Ve que jogadas sao possiveis serem feitas e guarda na array a cima
        verificarJogadas(j, options);
        //Verifica se o jogador e uma AI ou real
        if (j->players[p].tipoDeJogador == J_AI) {
            //Seleciona a jogada para o bot
            //Gera um numero aleatorio de uma jogada que o bot pode fazer
            opt = botPlay(options);
        } else {
            //Indica qual e o jogador a jogar agora
            printf("\n\nJogador %c:\n", 'A' + p);
            //Imprime o tabuleiro
            printTab(&j->tab);
            //Vai para o menu de escolha.
            //So sao apresentadas as jogadas que sao possiveis de serem feitas no momento
            opt = menuJogada(options);
        }
        //Verifica se o valor introduzido e valido ou nao
        if (opt < 0 || opt > 8) {
            //Caso nao seja considera a jogada invalida
            opt = JOGADA_NADA;
        } else {
            //Caso seja vai buscar a array das opcoes o valor da jogada correspondente
            //Estes valores encontram-se no consts.h
            opt = options[opt];
        }
        //Processa as varias jogadas
        switch (opt) {
            //Adicionar uma peca verde
            case ADD_PECA_VERDE:
                //Verifica se e um jogador real ou nao
                if (j->players[p].tipoDeJogador == J_JOGADOR)
                    //Caso afirmativo pede umas coordenadas ao utilizador
                    pos = introduzirPos(&j->tab);
                else
                    //Caso negativo gera umas coordenadas possiveis para realizar a jogada
                    pos = botPos(ADD_PECA_VERDE, &j->tab);
                //Verifica se a localizacao e valida, no caso de ser o bot vai ser sempre falso
                if (j->tab.pecas[pos.x + pos.y * j->tab.dim.x] != P_NADA || foraDoTab(j->tab.dim, pos)) {
                    printf("Localizacao invalida!\n");
                    break;
                }
                //Troca o valor da peca
                j->tab.pecas[pos.x + pos.y * j->tab.dim.x] = P_VERDE;
                //Diz que a jogada foi feita
                done = 1;
                break;
            //Upgrade de peca verde para amarela
            case UP_PECA_AMARELA:
                //Verifica se e um jogador real ou nao
                if (j->players[p].tipoDeJogador == J_JOGADOR)
                    //Caso afirmativo pede umas coordenadas ao utilizador
                    pos = introduzirPos(&j->tab);
                else
                    //Caso negativo gera umas coordenadas possiveis para realizar a jogada
                    pos = botPos(UP_PECA_AMARELA, &j->tab);
                //Verifica se a localizacao e valida, no caso de ser o bot vai ser sempre falso
                if (j->tab.pecas[pos.x + pos.y * j->tab.dim.x] != P_VERDE || foraDoTab(j->tab.dim, pos)) {
                    printf("Localizacao invalida!\n");
                    break;
                }
                //Troca o valor da peca
                j->tab.pecas[pos.x + pos.y * j->tab.dim.x] = P_AMARELO;
                //Diz que a jogada foi feita
                done = 1;
                break;
            //Upgrade de peca amarela para vermelha
            case UP_PECA_VERMELHA:
                //Verifica se e um jogador real ou nao
                if (j->players[p].tipoDeJogador == J_JOGADOR)
                    //Caso afirmativo pede umas coordenadas ao utilizador
                    pos = introduzirPos(&j->tab);
                else
                    //Caso negativo gera umas coordenadas possiveis para realizar a jogada
                    pos = botPos(UP_PECA_VERMELHA, &j->tab);
                //Verifica se a localizacao e valida, no caso de ser o bot vai ser sempre falso
                if (j->tab.pecas[pos.x + pos.y * j->tab.dim.x] != P_AMARELO || foraDoTab(j->tab.dim, pos)) {
                    printf("Localizacao invalida!\n");
                    break;
                }
                //Troca o valor da peca
                j->tab.pecas[pos.x + pos.y * j->tab.dim.x] = P_VERMELHO;
                //Diz que a jogada foi feita
                done = 1;
                break;
            //Adiciona uma pedra ao tabuleiro
            case ADD_PEDRA:
                //Verifica se e um jogador real ou nao
                if (j->players[p].tipoDeJogador == J_JOGADOR)
                    //Caso afirmativo pede umas coordenadas ao utilizador
                    pos = introduzirPos(&j->tab);
                else
                    //Caso negativo gera umas coordenadas possiveis para realizar a jogada
                    pos = botPos(ADD_PEDRA, &j->tab);
                //Verifica se a localizacao e valida, no caso de ser o bot vai ser sempre falso
                if (j->tab.pecas[pos.x + pos.y * j->tab.dim.x] != P_NADA || foraDoTab(j->tab.dim, pos)) {
                    printf("\nLocalizacao invalida!\n");
                    break;
                }
                //Troca o valor da peca
                j->tab.pecas[pos.x + pos.y * j->tab.dim.x] = P_PEDRA;
                //Aumenta o numero de pedras que foram introduzidas pelo utilizador
                j->players[p].pedrasAdd++;
                //Diz que a jogada foi feita
                done = 1;
                break;
            //Adicionar uma linha ao tabuleiro
            case ADD_LINHA:
                //Aumenta as coordenadas y por 1 para adicionar a linha
                j->tab.dim.y++;
                //Realoca a memoria para puder ser gerada a nova linha do tabuleiro
                j->tab.pecas = realloc(j->tab.pecas, sizeof(short) * j->tab.dim.y * j->tab.dim.x);
                //Caso tenha acontecido algum erro na alocacao da return -1
                if (j->tab.pecas == NULL)
                    return -1;
                //Vai preencher os ultimos elementos da lista, que representa a linha, com a peca vazia
                for (i = j->tab.dim.x * (j->tab.dim.y - 1); i < j->tab.dim.x * j->tab.dim.y; i++) {
                    j->tab.pecas[i] = P_NADA;
                }
                //Aumenta o numero de partes do tabuleiro que foram adicionadas
                j->players[p].partesAdd++;
                //Diz que a jogada foi feita
                done = 1;
                break;
            case ADD_COLUNA:
                //Aumenta as coordenadas x por 1 para adicionar a coluna
                j->tab.dim.x++;
                //Realoca a memoria para puder ser gerada a nova coluna do tabuleiro
                j->tab.pecas = realloc(j->tab.pecas, sizeof(short) * j->tab.dim.y * j->tab.dim.x);
                //Caso tenha acontecido algum erro na alocacao da return -1
                if (j->tab.pecas == NULL)
                    return -1;
                //Percorre todos os elementos da lista do final para o principio
                //Nao percorre os que nao precisam de ser trocados
                for (i = j->tab.dim.x * j->tab.dim.y; i >= j->tab.dim.x; i--) {
                    //Todos os multiplos de j->tab.dim.x pertencem a ultima coluna, que terminou de ser adicionada
                    if (i % j->tab.dim.x == 0) {
                        //Troca as pecas nas novas ultimas posicoes para a peca vazio
                        j->tab.pecas[i - 1] = P_NADA;
                    } else {
                        //Troca as pecas para a sua nova posicao
                        //i - i / j->tab.dim.x - 1 representa a localizacao antiga, sendo i / j->tab.dim.x
                        //      a diferenca entre a sua posicao antiga e a nova
                        j->tab.pecas[i - 1] = j->tab.pecas[i - i / j->tab.dim.x - 1];
                    }
                }
                //Aumenta o numero de partes do tabuleiro que foram adicionadas
                j->players[p].partesAdd++;
                //Diz que a jogada foi feita
                done = 1;
                break;
            //Caso o jogador desista do jogo esta aba vai ser chamada
            case DESISTENCIA:
                //Na desistencia nao acontece nada so precisa de ser passado depois que esta aconteceu
                //Diz que a jogada foi feita
                done = 1;
                break;
            //Caso algum jogador queira ver as ultimas jogadas que houve no jogo
            //Nao conta como jogada
            case SEE_PLAYS:
                //Pede ao utilizador quantas jogadas quer ver de um total apresentado no ecra
                printf("\nIntroduza quantas jogadas quer ver(%d):", j->nJogadas);
                scanf("%d", &opt);
                //Verifica se o valor e valido ou nao
                if (opt > j->nJogadas || opt < 1) {
                    printf("Valor invalido!\n\n");
                    break;
                }
                //Guarda a jogada atual e vai a procura da primeira jogada que tem de imprimir
                jogada *atual = j->lastJogada;
                for (i = 0; i < opt - 1; i++) {
                    //Vai procurar a jogada anterior
                    atual = atual->prevJogada;
                }
                printf("\n");
                //Depois da primeira jogada a imprimir ter sido detetada vai imprimir todas as pedidas
                while (atual != NULL) {
                    //Funcao que coloca no ponteiro txt o texto da jogada
                    getPlaysText(atual, txt);
                    //Imprime o texto da jogada
                    printf("%s", txt);
                    //Passa para a proxima jogada
                    atual = atual->nextJogada;
                }
                break;
            default:
                printf("Opcao invalida!\n\n");
        }
    } while(!done);
    //Aloca a jogada que terminou de acontececr
    jogada *temp = malloc(sizeof(jogada));
    //Caso de erro da return -1
    if (temp == NULL)
        return -1;
    //Guarda o tipo de jogada que foi feito
    temp->tipo = (short) opt;
    //Guarda a posicao da jogada, caso nao aplicavel e por default (0, 0)
    temp->pos = pos;
    //Guarda o ponteiro para o jogador que executou a jogada
    temp->playerID = j->players[p].nJogador;
    //Define a jogada anterior a esta que terminou de acontecer
    temp->prevJogada = j->lastJogada;
    //Caso ainda haja alguma jogada
    if (j->jogadas != NULL) {
        //Define a jogada que terminou de acontecer como seguinte da antiga jogada anterior
        j->lastJogada->nextJogada = temp;
    } else {
        //Caso contrario vai defenir esta como a primeira jogada
        j->jogadas = temp;
    }
    //Diz que a ultima jogada do programa e esta
    j->lastJogada = temp;
    //Atualiza o numero de jogadas executadas
    j->nJogadas++;
    //Define o numero da ultima jogada
    temp->nJogada = j->nJogadas;
    //Defina a proxima jogada como null
    temp->nextJogada = NULL;
    //Caso a jogada tenha sido desistencia
    if (opt == DESISTENCIA)
        //Da return no vencedor (Outro player)
        return ((p + 1) % 2) + 1;
    //Verifica a vitoria
    if (verificarVitoria(&j->tab)) {
        //Da return no jogador que ganhou
        return p + 1;
    }
    return 0;
}

//Verifica se a posicao esta dentro ou fora do tabuleiro
int foraDoTab(vetor dim, vetor pos) {
    if (pos.x >= dim.x || pos.x < 0)
        return 1;
    if (pos.y >= dim.y || pos.y < 0)
        return 1;
    return 0;
}

//Seleciona a jogada do bot de acordo com as disponiveis para a ocasiao
int botPlay(int *opt) {
    int i;
    for (i = 0; i < 8; i++) {
        //Um bot nao consegue desistir ou ver as jogadas.
        if (opt[i] == DESISTENCIA || opt[i] == SEE_PLAYS || opt[i] == JOGADA_NADA)
            break;
    }
    //Gera o numero aleatorio entre os disponiveis
    return intUniformRnd(0, i - 1);
}

//Seleciona uma posicao valida para o bot jogar
vetor botPos(int tipo, tabuleiro *tab) {
    short looking_for, i, total = 0;
    vetor pos = {0, 0};
    //Guarda o que esta a procura
    switch (tipo) {
        case ADD_PECA_VERDE:
            looking_for = P_NADA;
            break;
        case UP_PECA_AMARELA:
            looking_for = P_VERDE;
            break;
        case UP_PECA_VERMELHA:
            looking_for = P_AMARELO;
            break;
        case ADD_PEDRA:
            looking_for = P_NADA;
            break;
        default:
            looking_for = -1;
    }
    //Conta quantos desse tipo ha
    for (i = 0; i < tab->dim.x * tab->dim.y; i++) {
        if (looking_for == tab->pecas[i])
            total++;
    }
    //Gera o numero da posicao onde vai ser jogado
    int k = intUniformRnd(1, total);
    total = 0;
    //Vai buscar as coordenadas selecionadas aleatoriamente antes
    for (i = 0; i < tab->dim.x * tab->dim.y; i++) {
        //Encontra o indice do que foi pedido
        if (looking_for == tab->pecas[i])
            total++;
        if (total == k) {
            //Substitui os valores do vetor pelo gerado
            pos.x = (short) (i % tab->dim.x);
            pos.y = (short) (i / tab->dim.x);
            break;
        }
    }
    return pos;
}

//Verifica que jogadas sao possiveis de serem feitas para depois serem colocadas no menu
void verificarJogadas(jogo *j, int *opts) {
    int valid[8], p = j->nJogadas % MAX_PLAYERS, i, next = 0;
    //Define todas como impossivel
    for (i = 0; i < 8; i++) {
        valid[i] = 0;
    }
    //Verifica se ja exite alguma jogada feita
    //Caso afirmativo vai dizer que e possivel ver algumas jogadas passadas
    if (j->nJogadas != 0)
        valid[SEE_PLAYS] = 1;
    //Verifica se o jogador ainda pode adicionar partes ao tabuleiro
    if (j->players[p].partesAdd < MAX_PARTES) {
        valid[ADD_LINHA] = 1;
        valid[ADD_COLUNA] = 1;
    }
    //Verifica se o jogador pode desistir
    //A possibilidade de desistir aparece apenas quando MAX_PLAYERS e igual a 2, uma vez que se torna mais
    //Facil para o resto do codigo que isto aconteca no caso de haver mais players
    if (MAX_PLAYERS == 2) {
        valid[DESISTENCIA] = 1;
    }
    //Verifica onde podem ser jogadas pecas verdes, pedras ou serem dados upgrades para amarelas e vermelhas
    //Verifica todas as casas a procura de uma disponivel para a jogada
    for (i = 0; i < j->tab.dim.x * j->tab.dim.y; i++) {
        switch (j->tab.pecas[i]) {
            //Verificar se podem ser colocadas pecas verdes aqui
            case P_NADA:
                valid[ADD_PECA_VERDE] = 1;
                //Verifica se o jogador ainda pode inserir pedras ou nao
                if (j->players[p].pedrasAdd < MAX_PEDRAS)
                    valid[ADD_PEDRA] = 1;
                break;
            //Verifica se pode ser dado upgrade de uma peca verde para amarela
            //(Se uma peca verde existe no tabuleiro)
            case P_VERDE:
                valid[UP_PECA_AMARELA] = 1;
                break;
            //Verifica se pode ser dado upgrade de uma peca amarela para vermelha
            //(Se uma peca amarela existe no tabuleiro)
            case P_AMARELO:
                valid[UP_PECA_VERMELHA] = 1;
        }
        //Se ja verificou que todas sao validas entao para o loop
        if (valid[ADD_PECA_VERDE] && valid[UP_PECA_AMARELA] && valid[UP_PECA_VERMELHA])
            break;
    }
    //Percorre o vetor valido e caso seja 1 vai adicionar a opcao ao ponteiro das opcoes
    for (i = 0; i < 8; i++) {
        if (valid[i]) {
            opts[next] = i;
            //Passa ao proximo indice das opcoes
            next++;
        }
    }
    //Para o resto das casas livres, caso existam, preenche com uma jogada invalida
    while (next < 8) {
        opts[next] = JOGADA_NADA;
        next++;
    }
}

//Pede ao utilizador para introduzir a localizacao da sua jogada
vetor introduzirPos(tabuleiro *tab) {
    vetor temp;
    int x, y;
    //Imprime o tabuleiro
    printTab(tab);
    //Pede o x
    printf("Introduza o x:");
    scanf("%d", &x);
    temp.x = (short) x;
    //Pede o y
    printf("Introduza o y:");
    scanf("%d", &y);
    temp.y = (short) y;
    //Da return no vetor
    return temp;
}

//Imprime o tabuleiro
void printTab(tabuleiro *tab) {
    int x, y;
    char c;
    //Imprime a primeira linha
    printf("\ny|x |");
    for (x = 0; x < tab->dim.x; x++) {
        //Valores de x
        if (x > 9)
            //Caso tenha 2 digitos
            printf("%d |", x);
        else
            //Caso tenha 1 digito
            printf(" %d |", x);
    }
    printf("\n");
    //Imprime o resto das linhas
    for (y = 0; y < tab->dim.y; y++) {
        //Valores de y
        if (y > 9)
            //Caso tenha 2 digitos
            printf("%d  |", y);
        else
            //Caso tenha 1 digito
            printf("%d   |", y);
        //Percorre todas as casas do tabuleiro da respetiva linha e imprime as pecas
        for (x = 0; x < tab->dim.x; x++) {
            switch (tab->pecas[y * tab->dim.x + x]) {
                //Peca verde
                case P_VERDE:
                    c = 'G';
                    break;
                //Peca amarela
                case P_AMARELO:
                    c = 'Y';
                    break;
                //Peca vermelha
                case P_VERMELHO:
                    c = 'R';
                    break;
                //Pedra
                case P_PEDRA:
                    c = 'P';
                    break;
                //Caso contrario imprime a peca vazio
                default:
                    //No caso de algum erro acontecer e a peca ser invalida e atribuido um valor de casa vazia
                    if (tab->pecas[y * tab->dim.x + x] != P_NADA)
                        tab->pecas[y * tab->dim.x + x] = P_NADA;
                    c = '#';
            }
            //imprime a letra correspondente a peca
            printf(" %c |", c);
        }
        printf("\n");
    }
    printf("\n");
}

//Imprimir jogadas

void getPlaysText(jogada *atual, char *txt) {
    char temp[2] = " ";
    strcpy(txt, "");
    //Numero da jogada
    strcat(txt, "Jogada ");
    int nJogada = atual->nJogada;
    if (nJogada >= 100) {
        temp[0] = (char) ('0' + nJogada / 100);
        strcat(txt, temp);
        nJogada /= 10;
    }
    if (nJogada >= 10) {
        temp[0] = (char) ('0' + nJogada / 10);
        strcat(txt, temp);
    }
    temp[0] = (char) ('0' + nJogada % 10);
    strcat(txt, temp);
    //Informacao do jogador
    temp[0] = (char) ('A' + atual->playerID);
    strcat(txt, ": O jogador ");
    strcat(txt, temp);
    strcat(txt, " ");
    //Continua de acordo com o tipo da jogada
    switch (atual->tipo) {
        case ADD_PECA_VERDE:
            strcat(txt, "jogou uma peca verde");
            break;
        case UP_PECA_AMARELA:
            strcat(txt, "trocou uma peca verde para amarela");
            break;
        case UP_PECA_VERMELHA:
            strcat(txt, "trocou uma peca amarela para vermelha");
            break;
        case ADD_PEDRA:
            strcat(txt, "colocou uma pedra");
            break;
        case ADD_LINHA:
            strcat(txt, "adicionou uma linha");
            break;
        case ADD_COLUNA:
            strcat(txt, "adicionou uma coluna");
            break;
        case DESISTENCIA:
            strcat(txt, "desistiu");
            break;
        default:
            //No caso do tipo de jogada ser diferente de qualquer um existente vai passar ao proximo e mostrar na
            //      consola que o erro aconteceu
            printf("Ocorreu um erro ao guardar uma jogada e os dados foram corrompidos!\n");
            strcat(txt, "Ocorreu um erro ao ler esta jogada!");
    }
    //No caso de haver coordenadas estas vao ser adicionadas a string
    if (atual->tipo == ADD_PECA_VERDE || atual->tipo == UP_PECA_AMARELA || atual->tipo == UP_PECA_VERMELHA ||
        atual->tipo == ADD_PEDRA) {
        strcat(txt,  " nas coordenadas (");
        temp[0] = (char) ('0' + atual->pos.x);
        strcat(txt, temp);
        strcat(txt, ", ");
        temp[0] = (char) ('0' + atual->pos.y);
        strcat(txt, temp);
        strcat(txt, ")");
    }
    strcat(txt, "\n");
}

//Vitoria

int verificarVitoria(tabuleiro *tab) {
    int x, y;
    //Percorre todas as linhas
    for (y = 0; y < tab->dim.y; y++) {
        //Guarda o tipo da primeira peca
        short tipo = tab->pecas[y * tab->dim.x];
        if (tipo == P_NADA) {
            //Se o tipo for nada a vitoria nao acontece, logo salta o loop seguinte
            continue;
        }
        //Guarda se e tudo igual ou nao
        int is = 1;
        for (x = 1; x < tab->dim.x; x++) {
            if (tipo != tab->pecas[y * tab->dim.x + x]) {
                //Se algum for diferente vai guardar esse dado e vai acabar o loop, uma vez que a vitoria neste caso
                //      nao e possivel
                is = 0;
                break;
            }
        }
        //Caso haja vitoria da automaticamente return a informar que e uma vitoria
        if (is)
            return 1;
    }
    //Percorre todas as colunas
    for (x = 0; x < tab->dim.x; x++) {
        //Guarda o tipo da primeira peca
        short tipo = tab->pecas[x];
        if (tipo == P_NADA) {
            //Se o tipo for nada a vitoria nao acontece, logo salta o loop seguinte
            continue;
        }
        //Guarda se e tudo igual ou nao
        int is = 1;
        for (y = 1; y < tab->dim.y; y++) {
            if (tipo != tab->pecas[y * tab->dim.x + x]) {
                //Se algum for diferente vai guardar esse dado e vai acabar o loop, uma vez que a vitoria neste caso
                //      nao e possivel
                is = 0;
                break;
            }
        }
        if (is)
            return 1;
    }
    //Percorre as diagonais, caso seja um tabuleiro quadrado
    if (tab->dim.x == tab->dim.y) {
        //Guarda o tipo da primeira peca de ambas as diagonais
        short tipo1 = tab->pecas[0], tipo2 = tab->pecas[tab->dim.x - 1];
        if (tipo1 == P_NADA && tipo2 == P_NADA) {
            //Se o tipo for nada a vitoria nao acontece, logo da return como nao ha vitoria
            return 0;
        }
        //Guarda se e tudo igual ou nao, por default e sim
        int is1 = 1, is2 = 1;
        for (x = 0; x < tab->dim.x; x++) {
            //Verifica a diagonal 1 (x,x)
            if (tab->pecas[x + x * tab->dim.x] != tipo1 || tipo1 == P_NADA) {
                is1 = 0;
            }
            //Verifica a diagonal 2 (x, dim.x - x)
            if (tab->pecas[(x + 1) * tab->dim.x - (x + 1)] != tipo2 || tipo2 == P_NADA) {
                is2 = 0;
            }
            //Se ja for confirmada a nao vitoria da return 0 automaticamente para evitar ciclos desnecessarios
            if (is1 == 0 && is2 == 0)
                return 0;
        }
        //Se alguma das variaveis is for 1, entao
        if (is1 || is2)
            return 1;
    }
    //Caso nada a cima se verifique da return 0
    return 0;
}

//Ver jogo

int verJogo() {
    char path[FILE_NAME_MAX + 4];
    int i;
    //Pede o nome do ficheiro
    printf("Nomes de ficheiros nao podem incluir espacos!\nInsira -1 para voltar!\nInsira o nome do ficheiro:");
    scanf("%s", path);
    if (strcmp(path, "-1") == 0)
        return 0;
    //Da clear nos ultimos 5 digitos da string, usado para o caso de o utilizador introduzir
    //Mais de FILE_NAME_MAX - 1 caracteres
    for (i = 0; i < 5; i++) {
        path[FILE_NAME_MAX - 1 + i] = '\0';
    }
    //Adiciona .txt ao caminho
    strcat(path, ".txt");
    printf("\n\n");
    FILE *f;
    //Verifica se o ficheiro existe ou nao
    if ((f = fopen(path, "r"))) {
        char str[FILE_MAX_CHARS];
        //Le e imprime o conteudo do ficheiro
        while (fgets(str, FILE_MAX_CHARS, f) != NULL) {
            printf("%s\n", str);
        }
        fclose(f);
    } else {
        printf("Esse ficheiro nao existe!");
        return -1;
    }
    return 0;
}