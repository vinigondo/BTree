#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"

char * criaStringPagina(struct pagina newPagina, char * stringPagina) {
    char *string[TAM_PAGE*sizeof(int)+TAM_PAGE];
    char valor[sizeof(int)];
    memset(valor, '\0', sizeof(valor));
    memset(string, '\0', sizeof(string));

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

int ordenaVetor(int *vetor, int tamanho) {
    int aux;
    for(int i = 0; i < tamanho; i++) {
        for(int j = 0; j < tamanho; j++) {
            if(vetor[i] < vetor[j] && vetor[j] != -1 && vetor[i] != -1) {
                aux = vetor[i];
                vetor[i] = vetor[j];
                vetor[j] = aux;
            }
        }
    }
    return 0;
}

int insereChave(int chave, FILE *chavesBinarios){
    printf("\ninserindo chave: %d", chave);

    char cabecalho[sizeof(int)];
    fseek(chavesBinarios, 0, SEEK_SET);
    fread(cabecalho, sizeof(int), 1, chavesBinarios);
    int rrnRaiz = atoi(cabecalho);
    printf("\nrrnRaiz: %d", rrnRaiz);

    if(rrnRaiz == -1){
        struct pagina novaPagina = criaStructPagina("n"); //cria uma nova pagina vazia
        novaPagina.chaves[0] = chave;
        novaPagina.qtdchaves += 1;

        char *novaPaginaString[TAM_PAGE*sizeof(int)+TAM_PAGE];
        criaStringPagina(novaPagina, &novaPaginaString);
        // printf("\nNova pagina criada: %s", &novaPaginaString);

        int posicao = sizeof(cabecalho);
        fseek(chavesBinarios, posicao, SEEK_SET);
        fwrite(novaPaginaString, sizeof(char), strlen(novaPaginaString), chavesBinarios);

        memset(cabecalho, 0, sizeof(int));
        itoa(posicao, cabecalho, 10);
        fseek(chavesBinarios, 0, SEEK_SET);
        fwrite(cabecalho, 1, sizeof(cabecalho), chavesBinarios);
        printf("\nArquivo btree.dat criado com sucesso!");
        return 0;
    }

    char paginaAtualStr[TAM_PAGE*sizeof(int)+TAM_PAGE];
    memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
    fseek(chavesBinarios, rrnRaiz, SEEK_SET);
    fread(paginaAtualStr, 1, TAM_PAGE*sizeof(int)+TAM_PAGE, chavesBinarios);
    struct pagina paginaAtual = criaStructPagina(paginaAtualStr);


    if(paginaAtual.qtdchaves < ORDEM-1) {
        paginaAtual.chaves[paginaAtual.qtdchaves] = chave;
        paginaAtual.qtdchaves++;
        ordenaVetor(paginaAtual.chaves, ORDEM-1);
        
        memset(paginaAtualStr, 0, sizeof(paginaAtualStr));
        criaStringPagina(paginaAtual, paginaAtualStr);
        fseek(chavesBinarios, rrnRaiz, SEEK_SET);
        fwrite(paginaAtualStr, 1, sizeof(paginaAtualStr), chavesBinarios);
        printf("\nPagina atual: %s", paginaAtualStr);
    }

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

    int cabecalho[sizeof(int)];
    memset(cabecalho, '\0', sizeof(int));
    strcpy(cabecalho, "-1");
    fwrite(cabecalho, 1, sizeof(int), chavesBinarios);

    char chave[sizeof(int)];
    memset(chave, '\0', sizeof(chave));
    int i = 0;
    char buffer;

    buffer = fgetc(chavesTxt);
    while (buffer != EOF){
        if(buffer != '|'){
            chave[i] = buffer;
            i++;
        }else{
            insereChave(atoi(chave), chavesBinarios);
            memset(chave, '\0', sizeof(chave));
            i = 0;
        }
        buffer = fgetc(chavesTxt);
    }
 
    fclose(chavesBinarios);
    fclose(chavesTxt);
    return 1;
}