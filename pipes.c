// Input Details:   input for all three will be an array of n strings
//                  > and < operators will not be first or last in the array
//                  & operator will be last in the array
// Pipe Details:    file_desc[0] = read end of the pipe
//                  file_desc[1] = write end of the pipe

#include "my_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_LIM 1024
#define MAX_ARG_LIM 100
#define TRUE 0
#define FALSE 1
#define FILE_NAME "send.txt"

void printStrings(char* words[]) {
    int index = 0;
    while(words[index] != NULL) {
        printf("%s\n", words[index]);
        index++;
    }
}

void freeArr(char* words[]) {
    int index = 0;
    while(words[index] != NULL) 
        free(words[index]);
}

void pipe_function(char* args[]) {
    pid_t pid;
    int intermediateFile = open(FILE_NAME, O_RDONLY | O_WRONLY);
    int file_desc[2];
    char* firstHalf[MAX_ARG_LIM] = {0};
    char* secondHalf[MAX_ARG_LIM] = {0};
    int index = 0;
    int halfIndex = 0;
    int inSecondHalf = FALSE;
    const char* errmess = "failed to execute";

    while(args[index] != NULL) {
        if(strcmp(args[index], "|") == 0) {
            inSecondHalf = TRUE;
            halfIndex = 0;
        } else if(inSecondHalf == FALSE) {
            firstHalf[halfIndex] = (char*)malloc(sizeof(char) * BUFF_LIM);
            strcpy(firstHalf[halfIndex], args[index]);
            halfIndex++;
        } else {
            secondHalf[halfIndex] = (char*)malloc(sizeof(char) * BUFF_LIM);
            strcpy(secondHalf[halfIndex], args[index]);
            halfIndex++;
        } index++;
    }
    if(pipe(file_desc) == -1) return;
    pid = fork();
    if(pid > 0) {
        close(file_desc[0]);
        dup2(intermediateFile, STDOUT_FILENO);
        close(file_desc[1]);
        // anything printed from this point forward will be sent to pipe
        execvp(firstHalf[0], firstHalf);

        // ensures data is sent properly and no zombies are created
        fflush(stdout);
        // wait(NULL);
        //close(intermediateFile);
        freeArr(firstHalf);
    } else if(pid == 0) {
        close(file_desc[1]);
        //dup2(file_desc[0], STDIN_FILENO);
        close(file_desc[0]);

        secondHalf[halfIndex] = (char*)malloc(sizeof(char) * BUFF_LIM);
        strcpy(secondHalf[halfIndex], FILE_NAME);
        secondHalf[halfIndex + 1] = NULL;

        execvp(secondHalf[0], secondHalf);
        freeArr(secondHalf);
    } else
        printf("fork failed\n");
} 

void run_on_file_content(char* args[]) {
    
}

void redirect_output_to_file(char* args[]) {
    printf("testing\n");
    pid_t pid = fork();
    if(pid > 0){
        printf("hello from parent\n");
        wait(NULL);
    } else if(pid == 0)
        printf("hello from child\n");
    else 
        printf("error forking\n");
}

// int main() {
//     printf("testing pipes...\n");
//     char* args[] = {"sort", "data.txt", "|", "uniq", NULL};

//     pipe_function(args);

//     return 0;
// }