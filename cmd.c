//  File Name:      cmd.c
//  Author:         Angel Penaloza
//  Description:    Command Shell
//  Known Flaws:    -   missing 2 pipe functions
//                  -   the pipe function implemented works, 
//                      but breaks out of the loop
//                  -   pipe only runs between two commands
// Fixed:           -   xsh now supports any number of variables
//                  -   redirect pipe function implemented
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "my_commands.h"

#define MAX_CHAR_LIMIT 1024
#define MAX_ARG_LIMIT 500
#define TRUE 0
#define FALSE 1

void help() {
    printf("exit\t - exit command shell\n");
    printf("^D\t - end of file\n");
    printf("ls -l\t - show all files in folder\n");
    printf("mkdir\t - create new directory\n");
    printf("rmdir\t - remove new directory\n");
    printf("set\t - create a variable and set its valuable\n");
    printf("unset\t - remove value from variable\n");
}

// returns sanitized directory name for shell to display 
// treats 'user' as main and does not go beyond it
char* getDirectory(char* curdir) {
    char CWD[MAX_CHAR_LIMIT] = { 0 };
    getcwd(CWD, sizeof(CWD));
    char* directory = strrchr(CWD, '/');
    if( directory != NULL 
        && strcmp(directory, "/command-shell-angelpenlza") != 0
        && strstr(CWD, "/command-shell-angelpenlza") != NULL) {
        directory = strrchr(directory, '/') + 1;
        strcat(directory, " >> ");
        return directory;
    } return "";
}

// returns two arrays: one filled with the indexes of arguments with a $ in front of them (variables)
// and another with the indexes of all the operators (| , <, >, &) in the argument list
int** getIndexes(int** indexes, int* total_ops, char* args[]) {
    indexes = (int**)malloc(sizeof(int**));
    int *vars = NULL;
    int *operators = NULL;
    int ix = 0, var_ix = 0, op_ix = 0;
    while(args[ix] != NULL) {
        if(args[ix][0] == '$') {
            vars = (int*)realloc(vars, (var_ix + 1) * sizeof(int));
            vars[var_ix] = ix;
            var_ix++;
        }
        if( strcmp(args[ix], "|") == 0 || 
            strcmp(args[ix], ">") == 0 || 
            strcmp(args[ix], "<") == 0 || 
            strcmp(args[ix], "&") == 0) {
                operators = (int*)realloc(operators, (op_ix + 1) * sizeof(int));
                operators[op_ix] = ix;
                op_ix++;
        } ix++;
    }
    vars = (int*)realloc(vars, (var_ix + 1) * sizeof(int));
    vars[var_ix] = -1;
    operators = (int*)realloc(operators, (op_ix + 2) * sizeof(int));
    operators[op_ix] = ix;
    operators[op_ix + 1] = -1;

    indexes[0] = (int*)malloc(sizeof(int) * var_ix);
    indexes[0] = vars;
    indexes[1] = (int*)malloc(sizeof(int) * op_ix);
    indexes[1] = operators; 

    *total_ops = op_ix + 1; 
    return indexes; 
 }

 void freeIndexes(int** indexes) {
    if(indexes[0] != NULL) 
        free(indexes[0]);
    if(indexes[1] != NULL)
        free(indexes[1]);
    free(indexes);
 }

int main() {
    var table[MAX_ARG_LIMIT];
    init_table(table, MAX_ARG_LIMIT);
    char user_input[MAX_CHAR_LIMIT];
    char curdir[MAX_CHAR_LIMIT] = "\0";
    char* token;
    int index = 0; 
    int** indexes;
    int total_ops = 0;
    int status; 
    pid_t pid; 

    printf("running command shell (type 'help' for support)...\n");
    while(1) {
    // display shell output and sanitize input properly for optimal use
        char* args[MAX_ARG_LIMIT] = {0};
        printf("user >> %s ", curdir);
        if(fgets(user_input, MAX_CHAR_LIMIT, stdin) == NULL) break;
        if(strcmp(user_input, "\n") == 0) continue;
        user_input[strcspn(user_input, "\n")] = '\0';

        // separate user input into a char* args[] argument list by whitespace
        index = 0; 
        token = strtok(user_input, " ");
        while(token != NULL) {
            args[index] = (char*)malloc(sizeof(char) * strlen(token));
            strcpy(args[index], token);
            token = strtok(NULL, " ");
            index++; 
        }

        if(strcmp(user_input, "exit") == 0) break;
        if(strcmp(user_input, "quit") == 0) break;
        if(strcmp(user_input, "help") == 0) {
            help();
            continue;
        } 
        
        if(strcmp(user_input, "cd") == 0) {
            if(args[2] != NULL)
                perror("too many arguments\n");
            else {
                chdir(args[1]);
                strcpy(curdir, getDirectory(curdir));
                if(strcmp(curdir, "") == 0)
                    chdir("command-shell-angelpenlza");
            } continue;
        } 
        
        if(strcmp(user_input, "pwd") == 0) {
            char PWD[MAX_CHAR_LIMIT];
            if(getcwd(PWD, sizeof(PWD)) != NULL)
                printf("%s\n", PWD);
            continue;
        }

        if(strcmp(args[0], "set") == 0) {
            if(args[1] == NULL || args[2] == NULL) 
                printf("no value to set\n");
            else
                set_var(args[1], args[2], table);
            continue;
        }

        if(strcmp(args[0], "unset") == 0) {
            if(args[1] == NULL) 
                printf("no value to unset\n");
            else 
                unset_var(args[1], table);
            continue;
        }

        // indexes are terminated with a -1
        indexes = getIndexes(indexes, &total_ops, args);
        if(indexes[0][0] != -1) {
            int err = 0; 
            int index = 0;
            while(indexes[0][index] != -1) {
                char temp[MAX_CHAR_LIMIT] = {0};
                strcpy(temp, args[indexes[0][index]] + 1);
                if(strcmp(table[hash(temp)].value, "\0") == 0) {
                    printf("variable %s not found\n", temp);
                    err = -1; 
                    break;
                }
                strcpy(args[indexes[0][index]], table[hash(temp)].value);
                index++;
            }
            if(err == -1) continue;
        }

        // indexes[1][0] has the size of the array, indexes[1][1] is terminator
        if(indexes[1][1] != -1 && total_ops > 2) {
            if(strcmp(args[indexes[1][total_ops - 2]], "&") == 0)
                run_in_background(args, indexes[1], total_ops);
            else if(strcmp(args[indexes[1][1]], "<") == 0)  
                file_as_input(args);
            else 
                handle_pipes(args, indexes[1], total_ops);
            continue;
        }

        // run command if no other user-implemented commands detected
        pid = fork();
        if(pid > 0) {
            waitpid(pid, &status, 0);
        } else if(pid == 0) {
            if(execvp(args[0], args) == -1) break;
        } else {
            perror("error creating fork\n");
            break;
        }

        freeIndexes(indexes);
        for(index--; index >= 0; index--)
            free(args[index]);
    }
    return 0; 
}