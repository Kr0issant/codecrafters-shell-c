#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../structs/cmd_structs.h"
#include "../structs/program_structs.h"
#include "../utils/file_utils.h"
#include "../utils/misc.h"
#include "tokenizer.h"
#include "builtins.h"

const char *valid_builtins[] = {"exit", "echo", "type", "pwd", "cd", "jobs"};
const int valid_builtins_count = sizeof(valid_builtins) / sizeof(valid_builtins[0]);

void echo(Command cmd) {
	for (int i = 1; i < cmd.num_tokens; i++) {
		printf("%s", cmd.tokens[i]);
		if (i < cmd.num_tokens - 1) printf(" ");
	}
	printf("\n");
}

void type(Command cmd) {
	bool found;
	char *exe_path;

	for (int i = 1; i < cmd.num_tokens; i++) {
		found = false;
		for (int j = 0; j < valid_builtins_count; j++) {
			if (strcmp(valid_builtins[j], cmd.tokens[i]) == 0) {
				found = true;
				break;
			}
		}
		if (found) {
			printf("%s is a shell builtin\n", cmd.tokens[i]);
			continue;
		}

		exe_path = find_executable(cmd.tokens[i]);
		if (exe_path != NULL) {
			printf("%s is %s\n", cmd.tokens[i], exe_path);
			free(exe_path);
		}

		else printf("%s: not found\n", cmd.tokens[i]);
	}
}

void pwd(char *cwd) {
	printf("%s\n", cwd);
}

void cd(char *new_path, char **cwd) {
	if (new_path == NULL || (new_path) == 0) new_path = "~";

    char *temp = resolve_relative_path(*cwd, new_path);
    if (temp == NULL) {
        printf("cd: %s: No such file or directory\n", new_path);
        return;
    }
    if (chdir(temp) != 0) {
        // perror("cd");
		printf("cd: %s: No such file or directory\n", new_path);
        free(temp);
        return;
    }

    free(*cwd);
    *cwd = temp; 
}

void jobs(BackgroundJobs *mgr) {
    int status;
    pid_t pid;
    
    for (int i = 0; i < mgr->count; i++) {
        pid = waitpid(mgr->jobs[i].pid, &status, WNOHANG);
        if (pid > 0) {
            free(mgr->jobs[i].status);
            mgr->jobs[i].status = strdup("Done");
        }
    }

    for (int i = 0; i < mgr->count; i++) {
		char marker = ' ';
		if (i == mgr->count - 1) marker = '+';
		else if (i == mgr->count - 2) marker = '-';

		printf("[%d]%c  ", mgr->jobs[i].job_no, marker);

		printf("%-20s", mgr->jobs[i].status);

		for (int j = 0; j < mgr->jobs[i].command.num_tokens; j++) {
			printf("%s", mgr->jobs[i].command.tokens[j]);
			if (j < mgr->jobs[i].command.num_tokens - 1) {
				printf(" ");
			}
		}

		if (strcmp(mgr->jobs[i].status, "Running") == 0) {
			printf(" &");
		}
		
		printf("\n");
	}

    for (int i = 0; i < mgr->count; i++) {
        if (strcmp(mgr->jobs[i].status, "Done") == 0) {
            free(mgr->jobs[i].status);
            free_command(&(mgr->jobs[i].command));

            for (int j = i; j < mgr->count - 1; j++) {
                mgr->jobs[j] = mgr->jobs[j + 1];
            }
            mgr->count--;
            i--;
        }
    }
}