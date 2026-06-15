#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include "run_program.h"

int run_program(char *program_path, char *args[]) {
    pid_t pid;
    int status;

    if (posix_spawn(&pid, program_path, NULL, NULL, args, NULL) == 0) {
        waitpid(pid, &status, 0);

        // if (WIFEXITED(status)) printf("Program exited with code %d\n", WEXITSTATUS(status));
    } else {
        printf("Failed to spawn process\n");
        return 1;
    }

    return 0;
}