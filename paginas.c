#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"

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

    struct pagina primeiraPagina;
    
    for (int i = 0; i < ORDEM; i++){    
        primeiraPagina.filhos[i] = -1;
    }
    for (int i = 0; i < ORDEM-1; i++){    
        primeiraPagina.chaves[i] = -1;
    }

    char buffer[sizeof(int)*TAM_PAGE];
    fread(&buffer, sizeof(buffer), 1, chavesTxt);

    char *pt;
    pt = strtok(buffer, DELIM_STR);
    for (int i = 0; i < ORDEM-1; i++){
        primeiraPagina.chaves[i] = atoi(pt);
        pt = strtok(NULL, DELIM_STR);
    }

    char cabecalho[sizeof(int)+1];
    strcpy(cabecalho, "5...|");
    fwrite(cabecalho, 1, sizeof(cabecalho), chavesBinarios);

    char temp[sizeof(int)];
    memset(temp, 0, sizeof(int));
    int cntChaves = 0;
    int cntFilhos = 0;
    for (int i = 0; i < TAM_PAGE; i++){
        if(i%2 != 0){
            itoa(primeiraPagina.chaves[cntChaves],temp,10);
            cntChaves++;
        }else{
            itoa(primeiraPagina.filhos[cntFilhos],temp,10);
            cntFilhos++;
        }

        fwrite(temp, 1 , sizeof(int) , chavesBinarios);
        fwrite(DELIM_STR , 1 , sizeof(char) , chavesBinarios);

    }

    fclose(chavesTxt);
    fclose(chavesBinarios);
    return 1;
}