#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"


char * criaStringPagina(struct pagina newPagina, char * stringPagina) {
    char *string[TAM_PAGE*sizeof(int)+TAM_PAGE];
    char valor[sizeof(int)];
    memset(valor, 0, sizeof(valor));
    memset(string, 0, sizeof(string));

    int cntChave = 0;
    int cntFilho = 0;

    for(int i = 0; i < TAM_PAGE; i++) {
        if(i%2 == 0) {
            itoa(newPagina.filhos[cntFilho], valor, 10);
            strcat(string, valor);
            cntFilho++;
        } else {
            itoa(newPagina.chaves[cntChave], valor, 10);
            strcat(string, valor);
            cntChave++;
        }
        strcat(string, DELIM_STR);
    }
    strcpy(stringPagina, string);
    return string;
}

struct pagina criaStructPagina(char *string) {

    if(string[0] == 'n') {
        struct pagina primeiraPagina;
    
        for (int i = 0; i < ORDEM; i++){    
            primeiraPagina.filhos[i] = -1;
        }
        for (int i = 0; i < ORDEM-1; i++){    
            primeiraPagina.chaves[i] = -1;
        }

        primeiraPagina.qtdchaves = 0;

        return primeiraPagina;
    }

    struct pagina newPagina;
    newPagina.qtdchaves = 0;
    char *token;
    char *stringCopy = strdup(string);
    int cnt = 0;
    int cntFilho = 0;
    int cntChave = 0;

    token = strtok(stringCopy, DELIM_STR);
    while(token != NULL) {
         if(cnt%2 == 0) {
            newPagina.filhos[cntFilho] = atoi(token);
            cntFilho++;
        } else {
            newPagina.chaves[cntChave] = atoi(token);
            if(atoi(token) != -1) {
                newPagina.qtdchaves++;
            }
            cntChave++;
        }
        token = strtok(NULL, DELIM_STR);
        cnt++;
    }
    return newPagina;
}


int insereChave(FILE *chavesBinarios, int *rnnRaiz, int *rrnFilhoD, int *chaveP){

    char paginaAtualStr[TAM_PAGE*sizeof(int)+TAM_PAGE];
    memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
    fseek(chavesBinarios, *rnnRaiz, SEEK_SET);
    fread(paginaAtualStr, 1, sizeof(paginaAtualStr), chavesBinarios);

    struct pagina paginaAtual = criaStructPagina(paginaAtualStr);

    // printf("pagina atual: %s", paginaAtualStr);
    // printf(" <- rrn: %d", *rnnRaiz);
    // printf("\nqtdChaves : %d", paginaAtual.qtdchaves);

    int percorreChaves = 0;

    while(percorreChaves < paginaAtual.qtdchaves +1) {
        if(*chaveP < paginaAtual.chaves[percorreChaves]){
            if(paginaAtual.filhos[percorreChaves] != -1) {
                if(insereChave(chavesBinarios, &paginaAtual.filhos[percorreChaves], rrnFilhoD, chaveP) != 0) {
                    if(paginaAtual.qtdchaves < ORDEM-1) {
                        for(int i = paginaAtual.qtdchaves; i > percorreChaves; i--) {
                            paginaAtual.chaves[i] = paginaAtual.chaves[i-1];
                            paginaAtual.filhos[i+1] = paginaAtual.filhos[i];
                        }
                        paginaAtual.chaves[percorreChaves] = *chaveP;
                        paginaAtual.filhos[percorreChaves+1] = *rrnFilhoD;

                        memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
                        criaStringPagina(paginaAtual, paginaAtualStr);
                        fseek(chavesBinarios, *rnnRaiz, SEEK_SET);
                        fwrite(paginaAtualStr, 1, sizeof(paginaAtualStr), chavesBinarios);

                        return 0;
                    }else{
                        return 1;
                    }
                }
                return 0;
            } else {
                if(paginaAtual.qtdchaves < ORDEM-1) {
                    for(int i = paginaAtual.qtdchaves; i > percorreChaves; i--) {
                        paginaAtual.chaves[i] = paginaAtual.chaves[i-1];
                        paginaAtual.filhos[i+1] = paginaAtual.filhos[i];
                    }
                    paginaAtual.chaves[percorreChaves] = *chaveP;

                    memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
                    criaStringPagina(paginaAtual, paginaAtualStr);
                    fseek(chavesBinarios, *rnnRaiz, SEEK_SET);
                    fwrite(paginaAtualStr, 1, sizeof(paginaAtualStr), chavesBinarios);
                    return 0;
                }else{

                    char paginaFilhoDirStr[TAM_PAGE*sizeof(int)+TAM_PAGE];
                    memset(paginaFilhoDirStr, 0, sizeof(paginaFilhoDirStr));
                    
                    struct pagina paginaFilhoDir = criaStructPagina("n");

                    int chavesParaDivisao[ORDEM];
                    for(int i = 0; i < ORDEM; i++) {
                        chavesParaDivisao[i] = paginaAtual.chaves[i];
                    }
                    chavesParaDivisao[ORDEM-1] = *chaveP;

                    ordenaVetor(chavesParaDivisao, ORDEM);

                    for(int i = 0; i < ORDEM; i++) {
                        if(i < (ORDEM-1)/2) {
                            paginaAtual.chaves[i] = chavesParaDivisao[i];
                        }else if(ORDEM%2 == 0 && i == ORDEM/2) {
                            paginaAtual.chaves[i] = -1;
                            *chaveP = chavesParaDivisao[i];
                        }else if(ORDEM%2 != 0 && i == ((ORDEM-1)/2)) {
                            paginaAtual.chaves[i] = -1;
                            *chaveP = chavesParaDivisao[i];
                        } else {
                            paginaFilhoDir.chaves[i-(ORDEM+2)/2] = chavesParaDivisao[i];
                            paginaFilhoDir.filhos[i-(ORDEM+2)/2] = paginaAtual.filhos[i];
                            paginaAtual.chaves[i] = -1;
                            paginaAtual.filhos[i] = -1;
                        }
                    }

                    fseek(chavesBinarios, *rnnRaiz, SEEK_SET);
                    memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
                    criaStringPagina(paginaAtual, paginaAtualStr);
                    fwrite(paginaAtualStr, sizeof(paginaAtualStr), 1, chavesBinarios);

                    fseek(chavesBinarios, 0, SEEK_END);
                    *rrnFilhoD = ftell(chavesBinarios);
                    printf("\nrrnFilhoD: %d", *rrnFilhoD);
                    memset(paginaFilhoDirStr, 0, sizeof(paginaFilhoDirStr));
                    criaStringPagina(paginaFilhoDir, paginaFilhoDirStr);
                    fwrite(paginaFilhoDirStr,  sizeof(paginaFilhoDirStr), 1, chavesBinarios);

                    return 1;
                }
            }
        }else if(percorreChaves == paginaAtual.qtdchaves){
            if(paginaAtual.qtdchaves < ORDEM-1) {
                paginaAtual.chaves[paginaAtual.qtdchaves] = *chaveP;

                memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
                criaStringPagina(paginaAtual, paginaAtualStr);
                fseek(chavesBinarios, *rnnRaiz, SEEK_SET);
                fwrite(paginaAtualStr, sizeof(paginaAtualStr), 1, chavesBinarios);
                return 0;
            }else{

                char paginaFilhoDirStr[TAM_PAGE*sizeof(int)+TAM_PAGE];
                memset(paginaFilhoDirStr, 0, sizeof(paginaFilhoDirStr));
                
                struct pagina paginaFilhoDir = criaStructPagina("n");

                int chavesParaDivisao[ORDEM];
                for(int i = 0; i < ORDEM; i++) {
                    chavesParaDivisao[i] = paginaAtual.chaves[i];
                }
                chavesParaDivisao[ORDEM-1] = *chaveP;

                ordenaVetor(chavesParaDivisao, ORDEM);

                for(int i = 0; i < ORDEM; i++) {
                    if(i < (ORDEM-1)/2) {
                        paginaAtual.chaves[i] = chavesParaDivisao[i];
                    }else if(ORDEM%2 == 0 && i == ORDEM/2) {
                        paginaAtual.chaves[i] = -1;
                        *chaveP = chavesParaDivisao[i];
                    }else if(ORDEM%2 != 0 && i == ((ORDEM-1)/2)) {
                        paginaAtual.chaves[i] = -1;
                        *chaveP = chavesParaDivisao[i];
                    } else {
                        paginaFilhoDir.chaves[i-(ORDEM+2)/2] = chavesParaDivisao[i];
                        paginaFilhoDir.filhos[i-(ORDEM+2)/2] = paginaAtual.filhos[i];
                        paginaAtual.chaves[i] = -1;
                        paginaAtual.filhos[i] = -1;
                    }
                }

                fseek(chavesBinarios, *rnnRaiz, SEEK_SET);
                memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
                criaStringPagina(paginaAtual, paginaAtualStr);
                fwrite(paginaAtualStr, sizeof(paginaAtualStr), 1, chavesBinarios);

                fseek(chavesBinarios, 0, SEEK_END);
                *rrnFilhoD = ftell(chavesBinarios);
                printf("\nrrnFilhoD: %d", *rrnFilhoD);
                memset(paginaFilhoDirStr, 0, sizeof(paginaFilhoDirStr));
                criaStringPagina(paginaFilhoDir, paginaFilhoDirStr);
                fwrite(paginaFilhoDirStr,  sizeof(paginaFilhoDirStr), 1, chavesBinarios);

                return 1;
            }

        }else if(paginaAtual.filhos[percorreChaves+1] != -1 && *chaveP > paginaAtual.chaves[percorreChaves] && *chaveP < paginaAtual.chaves[percorreChaves+1]){

            if(insereChave(chavesBinarios, &paginaAtual.filhos[percorreChaves+1], rrnFilhoD, chaveP) != 0) {
                if(paginaAtual.qtdchaves < ORDEM-1) {
                    paginaAtual.chaves[paginaAtual.qtdchaves] = *chaveP;
                    paginaAtual.filhos[paginaAtual.qtdchaves+1] = *rrnFilhoD;
                    return 0;
                }else{
                    return 1;
                }
            }
            return 0;
        }

        percorreChaves++;
    }

    printf("\nPagina Atual: %s", paginaAtualStr);

    return 0;
}

int inicializacao(char* chaves){
    FILE *chavesTxt;
    if( (chavesTxt = fopen(chaves, "r") ) == NULL ){
        printf("\nAlgo deu errado na leitura do arquivo %s", chaves);
        return 0;
        exit(EXIT_FAILURE);
    }

    FILE *chavesBinarios;
    if( (chavesBinarios = fopen("btree.dat","w+b") ) == NULL ){
        printf("\nAlgo deu errado na criação do arquivo btree.dat");
        return 0;
        exit(EXIT_FAILURE);
    }

    char cabecalho[sizeof(int)];
    memset(cabecalho, 0, sizeof(cabecalho));
    strcpy(cabecalho, "-1");
    fwrite(cabecalho, 1, sizeof(cabecalho), chavesBinarios);

    fseek(chavesBinarios, 0, SEEK_SET);
    fread(cabecalho, sizeof(cabecalho), 1, chavesBinarios);
    int rrnRaiz = atoi(cabecalho);
    printf("\nrrnRaiz: %d", rrnRaiz);

    char chaveStr[sizeof(int)];
    memset(chaveStr, 0, sizeof(chaveStr));
    int i = 0;
    char buffer;

    buffer = fgetc(chavesTxt);
    while (buffer != EOF){
        if(buffer != '|'){
            chaveStr[i] = buffer;
            i++;
        }else{

            int chave = atoi(chaveStr);

            printf("\ninserindo chave: %d", chave);
            printf("\nrrn raiz: %d", rrnRaiz);

            if(rrnRaiz == -1){
                struct pagina novaPagina = criaStructPagina("n"); //cria uma nova pagina vazia
                novaPagina.chaves[0] = chave;
                novaPagina.qtdchaves += 1;

                char novaPaginaString[TAM_PAGE*sizeof(int)+TAM_PAGE];
                memset(novaPaginaString, 0, sizeof(novaPaginaString));
                criaStringPagina(novaPagina, novaPaginaString);

                int posicao = sizeof(cabecalho);

                memset(cabecalho, 0, sizeof(cabecalho));
                itoa(posicao, cabecalho, 10);
                fseek(chavesBinarios, 0, SEEK_SET);
                fwrite(cabecalho, 1, sizeof(cabecalho), chavesBinarios);

                fwrite(novaPaginaString, 1, sizeof(novaPaginaString), chavesBinarios);

                rrnRaiz = posicao;
                printf("\nPagina atual: %s", novaPaginaString);
            }else{
                int chavePro = chave;
                int rrnFilhoD = -1;
                if(insereChave(chavesBinarios, &rrnRaiz, &rrnFilhoD, &chavePro) != 0){

                    struct pagina novaPaginaRaiz = criaStructPagina("n");
                    novaPaginaRaiz.chaves[0] = chavePro;
                    novaPaginaRaiz.filhos[0] = rrnRaiz;
                    novaPaginaRaiz.filhos[1] = rrnFilhoD;
                    novaPaginaRaiz.qtdchaves += 1;

                    fseek(chavesBinarios, 0, SEEK_END);
                    rrnRaiz = ftell(chavesBinarios);
                    printf("\ntroquei a raiz: %d", rrnRaiz);
                    char novaPaginaRaizString[TAM_PAGE*sizeof(int)+TAM_PAGE];
                    memset(novaPaginaRaizString, 0, sizeof(novaPaginaRaizString));
                    criaStringPagina(novaPaginaRaiz, novaPaginaRaizString);
                    fwrite(novaPaginaRaizString, 1, sizeof(novaPaginaRaizString), chavesBinarios);

                    memset(cabecalho, 0, sizeof(cabecalho));
                    itoa(rrnRaiz, cabecalho, 10);
                    fseek(chavesBinarios, 0, SEEK_SET);
                    fwrite(cabecalho, sizeof(cabecalho), 1, chavesBinarios);
                }
            }

            memset(chaveStr, 0, sizeof(chaveStr));
            i = 0;
        }
        buffer = fgetc(chavesTxt);
    }
 
    fclose(chavesBinarios);
    fclose(chavesTxt);
    return 1;
}