//  File Name:      cmd.c
//  Author:         Angel Penaloza
//  Description:    Command Shell
//  Known Flaws:     -  (xsh) can only output one set variable at a time,
//                      function stops after one variable is found
//                   -  missing 3 pipe functions
//                   -  the pipe function implemented works, 
//                      but breaks out of the loop
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

int searchForVar(char* args[]) {
    int index = 0;
    while(args[index] != NULL) {
        if(args[index][0] == '$')
            return index;
        index++;
    } return -1;
}

int searchForOp(char* op, char* args[]) {
    int index = 0;
    while(args[index] != NULL) {
        if(strcmp(op, args[index]) == 0)
            return index;
        index++;
    } return -1;
}

int main() {
    var table[MAX_ARG_LIMIT];
    init_table(table, MAX_ARG_LIMIT);
    char user_input[MAX_CHAR_LIMIT];
    char curdir[MAX_CHAR_LIMIT] = "\0";
    char* token;
    int index = 0; 
    int op_index; 
    int status; 
    pid_t pid; 

    printf("running command shell (type 'help' for support)...\n");
    while(1) {
        char* args[MAX_ARG_LIMIT] = {0};
        printf("user >> %s ", curdir);
        if(fgets(user_input, MAX_CHAR_LIMIT, stdin) == NULL) break;
        user_input[strcspn(user_input, "\n")] = '\0';
        index = 0; 
        token = strtok(user_input, " ");
        while(token != NULL) {
            args[index] = (char*)malloc(sizeof(char) * strlen(token));
            strcpy(args[index], token);
            token = strtok(NULL, " ");
            index++; 
        }

        if(strcmp(user_input, "\n") == 0) continue;
        if(strcmp(user_input, "exit") == 0) break;
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
            set_var(args[1], args[2], table);
            continue;
        }

        if(strcmp(args[0], "unset") == 0) {
            unset_var(args[1], table);
            continue;
        }

        int vari = searchForVar(args);
        if(vari > 0) {
            char temp[MAX_CHAR_LIMIT];
            strcpy(temp, args[vari] + 1);
            strcpy(args[vari], table[hash(temp)].value);
        }

        if((op_index = searchForOp("|", args)) >= 0) {
            if(args[op_index + 1] == NULL || op_index == 0) 
                printf("invalid operator placement\n");
            else 
                pipe_function(args);
            continue;
        }

        if((op_index = searchForOp("<", args)) >=0) {
            if(args[op_index + 1] == NULL || op_index == 0) 
                printf("invalid operator placement\n");
            else
                //run_on_file_content(args);
            continue;
        }

        if((op_index = searchForOp(">", args)) >= 0) {
            if(args[op_index + 1] == NULL || op_index == 0) 
                printf("invalid operator placement\n");
            else
                redirect_output_to_file(args);
            continue;
        }

        if((op_index = searchForOp("&", args) >= 0)) {
            if(op_index == 0 || args[op_index + 1] != NULL)
                printf("invalid operator placement\n");
            else 
                //run_in_background(args);
            continue;
        }

        pid = fork();
        if(pid > 0) {
            waitpid(pid, &status, 0);
        } else if(pid == 0) {
            if(execvp(args[0], args) == -1) break;
        } else {
            perror("error creating fork\n");
            break;
        }

        for(index--; index >= 0; index--)
            free(args[index]);
    }
    return 0; 
}