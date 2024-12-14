#include "my_commands.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TABLE_SIZE 500
#define VAR_CHAR_LIMIT 500
#define TRUE 0
#define FALSE 1

unsigned long long hash(char* word) {
    unsigned long long hash = 391;
    int len = strlen(word);
    int index = 0;

    hash += (int)(word[len - 1]) * (int)(word[0]);

    while(word[index] != '\0') {
        if(hash % 2 == 0)
            hash *= 31;
        else 
            hash -= 7;
        hash += (int)word[index];
        index++;
    }
    return hash % TABLE_SIZE;
}

int found(char* word, var table[]) {
    if(strcmp(table[hash(word)].var_name, "\0") == 0) return FALSE;
    else return TRUE;
}

void init_table(var table[], int size) {
    for(int ix = 0; ix < size; ix++) {
        table[ix].var_name = (char*)malloc(sizeof(char) * VAR_CHAR_LIMIT);
        strcpy(table[ix].var_name, "\0");
        table[ix].value = (char*)malloc(sizeof(char) * VAR_CHAR_LIMIT);
        strcpy(table[ix].value, "\0");
    }
}

void set_var(char* variable, char* value, var table[]) {
    if(found(variable, table) == TRUE && strcmp(table[hash(variable)].var_name, variable) != 0) {
        printf("error setting variable: overlapping space\n");
        printf("located: %s\n", table[hash(variable)].var_name);
        printf("your input: %s\n", variable);
        return;
    }
    if(found(variable, table) == FALSE)
        strcpy(table[hash(variable)].var_name, variable);

    strcpy(table[hash(variable)].value, value);
}

void unset_var(char* variable, var table[]) {
    if(found(variable, table) == TRUE)
        strcpy(table[hash(variable)].value, "\0");
    else 
        printf("variable name not found\n");
}

