#ifndef TOOLS_H
#define TOOLS_H
#include <stdio.h>
#include "tools.h"

#define DELIM_CH '|'
#define DELIM_STR "|"
#define ORDEM 5
#define TAM_PAGE 9

struct pagina{
    int qtdchaves;
    int chaves[ORDEM-1];
    int filhos[ORDEM];
};

int input(char *, int);

int input(char * str, int size) {
    int i = 0;    
    char c = getchar();    
    while (c != '\n') {        
        if (i < size - 1) {            
            str[i] = c;            
            i++;                 
        }        
        c = getchar();   
    }    
    str[i] = '\0';    
    return i;
}

#endif