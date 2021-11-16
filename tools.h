#ifndef TOOLS_H
#define TOOLS_H
#include <stdio.h>
#include "tools.h"

#define DELIM_CH '|'
#define DELIM_STR "|"
#define TAM_MAX_REG 64

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