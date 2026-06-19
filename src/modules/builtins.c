#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../structs/cmd_structs.h"
#include "../utils/file_utils.h"
#include "tokenizer.h"
#include "builtins.h"

const char *valid_builtins[] = {"exit", "echo", "type", "pwd", "cd"};
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