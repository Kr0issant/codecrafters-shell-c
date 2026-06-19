#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../structs/cmd_structs.h"
#include "builtins.h"
#include "run_program.h"

// int run_program(char *program_path, char *args[]) {
//     pid_t pid;
//     int status;

//     if (posix_spawn(&pid, program_path, NULL, NULL, args, NULL) == 0) {
//         waitpid(pid, &status, 0);

//         // if (WIFEXITED(status)) printf("Program exited with code %d\n", WEXITSTATUS(status));
//     } else {
//         printf("Failed to spawn process\n");
//         return 1;
//     }

//     return WEXITSTATUS(status);
// }

int run_program(char *program_path, char *args[], char *output_file, int fd, int append) {
    pid_t pid;
    int status;

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);

    if (output_file != NULL) {
        int flags = O_WRONLY | O_CREAT;
        if (append) flags |= O_APPEND;
        else flags |= O_TRUNC;

        int target_fd = 3;
        posix_spawn_file_actions_addopen(&actions, target_fd, output_file, flags, 0644);
        posix_spawn_file_actions_adddup2(&actions, target_fd, fd);
        posix_spawn_file_actions_addclose(&actions, target_fd);
    }

    if (posix_spawn(&pid, program_path, &actions, NULL, args, NULL) == 0) {
        waitpid(pid, &status, 0);

        // if (WIFEXITED(status)) printf("Program exited with code %d\n", WEXITSTATUS(status));
    } else {
        printf("Failed to spawn process\n");
        return 1;
    }

    posix_spawn_file_actions_destroy(&actions);
    return WEXITSTATUS(status);
}

void run_builtin(Command cmd, char **cwd, char *output_file) {
    int saved_stdout = dup(STDOUT_FILENO);

    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);

    if (strcmp(cmd.tokens[0], "echo") == 0) echo(cmd);
    else if (strcmp(cmd.tokens[0], "type") == 0) type(cmd);
    else if (strcmp(cmd.tokens[0], "pwd") == 0) pwd(*cwd);
    else if (strcmp(cmd.tokens[0], "cd") == 0) cd(cmd.tokens[1], cwd);

    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}