// new pipe file
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "my_commands.h"

#define BUFF_SIZE 1024

void printStrings(char* words[]) {
    int index = 0;
    while (words[index] != NULL) {
        printf("{%s}, ", words[index]);
        index++;
    }
    printf("\n");
}

// cuts the argument list to desired length 
// does nothing if desired length is greater than actual length
void realloc_args(char* args[], int size) {
    while (args[size] != NULL) {
        args[size] = NULL;
        size++;
    }
}

void close_pipes(int total_indexes, int pipe_fds[]) {
    for (int ix = 0; ix < (total_indexes - 1) * 2; ix++) {
        close(pipe_fds[ix]);
        close(pipe_fds[ix + 1]);
    }
}

// unfinished function: supposed to handle various | operators and a < operator at the end
void handle_pipes(char* args[], int* indexes, int total_indexes) {
    printf("testing from beginning...\n");

    int pipe_fds[(total_indexes - 1) * 2];
    for (int ix = 0; ix < total_indexes; ix++) {
        if (pipe(pipe_fds + (ix * 2)) == -1) {
            printf("error creating pipes\n");
            exit(1);
        }
    }
    pid_t main = fork();
    if (main < 0) {
        printf("error forking\n");
        exit(1);
    } else if (main > 0) {
        printf("main parent\n");
        wait(NULL);
    } else {
        pid_t pid = fork();
        if (pid < 0) {
            printf("error forking child\n");
            exit(1);
        } else if (pid == 0) {
            dup2(pipe_fds[0], STDIN_FILENO);
            execvp(args[1], args + 1);
            close_pipes(total_indexes, pipe_fds);
            exit(0);
        } else {
            dup2(pipe_fds[1], STDOUT_FILENO);
            realloc_args(args, indexes[0]);
            execvp(args[0], args);
            wait(NULL);
            close_pipes(total_indexes, pipe_fds);
        }
        exit(0);
    }
}

// open file, use its contents as input for execvp()
void file_as_input(char* args[]) {
    pid_t pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        int file_fd = open(args[0], O_RDONLY);
        if (file_fd < 0) {
            printf("file does not exist\n");
            exit(1);
        }
        dup2(file_fd, STDIN_FILENO);
        close(file_fd);
        execvp(args[2], args + 2);
        exit(0);
    }
}

void run_in_background(char* args[], int* indexes, int total_indexes) {
    pid_t pid = fork();
    if (pid < 0) {
        printf("error forking\n");
        return;
    } else if (pid > 0) {
        printf("running commands in background...\n");
    } else {
        handle_pipes(args, indexes, total_indexes);
        exit(0);
    }
}