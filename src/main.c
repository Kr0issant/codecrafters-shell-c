#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils/tokenizer.h"
#include "utils/file_utils.h"
#include "modules/run_program.h"

const int MAX_INPUT_LENGTH = 65536;
char *input_buffer;
Tokens tk;

const char *valid_builtins[] = {"exit", "echo", "type", "pwd", "cd"};
const int valid_builtins_count = sizeof(valid_builtins) / sizeof(valid_builtins[0]);

char *cwd;

void free_tk(Tokens *tk) {
	if (tk->tokens != NULL) {
		if (tk->num_tokens > 0) {
			for (int i = 0; i < tk->num_tokens; i++) free(tk->tokens[i]);
		}
		free(tk->tokens);
		tk->tokens = NULL;
	}
}

void cleanup_heap() {
	if (input_buffer != NULL) {
		free(input_buffer);
		input_buffer = NULL;
	}
	free_tk(&tk);
}

char* read_input(char *buffer, int max_len) {
	if (fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
		return buffer;
    }

	return NULL;
}

void echo(Tokens tk) {
	for (int i = 1; i < tk.num_tokens; i++) {
		printf("%s", tk.tokens[i]);
		if (i < tk.num_tokens - 1) printf(" ");
	}
	printf("\n");
}

void type(Tokens tk) {
	bool found;
	char *exe_path;

	for (int i = 1; i < tk.num_tokens; i++) {
		found = false;
		for (int j = 0; j < valid_builtins_count; j++) {
			if (strcmp(valid_builtins[j], tk.tokens[i]) == 0) {
				found = true;
				break;
			}
		}
		if (found) {
			printf("%s is a shell builtin\n", tk.tokens[i]);
			continue;
		}

		exe_path = find_executable(tk.tokens[i]);
		if (exe_path != NULL) {
			printf("%s is %s\n", tk.tokens[i], exe_path);
			free(exe_path);
		}

		else printf("%s: not found\n", tk.tokens[i]);
	}
}

void pwd() {
	printf("%s\n", cwd);
}

void cd(char *new_path) {
	char *temp;
	int unchanged = 1;

	temp = resolve_relative_path(cwd, new_path);
	if (temp != NULL) {
		strcpy(cwd, temp);
		free(temp);
		unchanged = chdir(cwd);
	} else {
		printf("cd: %s: No such file or directory\n", new_path);
		return;
	}
	
	// if (unchanged) printf("cd: %s: Could not change directory\n", cwd);
	if (unchanged) printf("cd: %s: No such file or directory\n", new_path);
}

int main(int argc, char *argv[]) {
	atexit(cleanup_heap);
	setbuf(stdout, NULL);
	
	input_buffer = malloc(MAX_INPUT_LENGTH);
	if (input_buffer == NULL) return 1;
	
	char *exe_path;
	
	cwd = malloc(4096);
	if (getcwd(cwd, 4096) == NULL) return 1;
	
	char *temp_inp;
	Tokens temp_tk;
	
	while (true) {
		printf("$ ");
	
		temp_inp = read_input(input_buffer, MAX_INPUT_LENGTH);
		if (temp_inp == NULL) return 1;
		input_buffer = temp_inp;

		temp_tk = get_tokens(input_buffer);
		if (temp_tk.tokens == NULL) return 1;
		free_tk(&tk);
		tk = temp_tk;

		if (tk.num_tokens == 0 || tk.tokens[0] == NULL || strlen(tk.tokens[0]) == 0) continue;

		else if (strcmp(tk.tokens[0], "exit") == 0) return 0;
		else if (strcmp(tk.tokens[0], "echo") == 0) echo(tk);
		else if (strcmp(tk.tokens[0], "type") == 0) type(tk);
		else if (strcmp(tk.tokens[0], "pwd") == 0) pwd();
		else if (strcmp(tk.tokens[0], "cd") == 0) cd(tk.tokens[1]);
		
		else {
			exe_path = find_executable(tk.tokens[0]);
			if (exe_path != NULL) {
				run_program(exe_path, tk.tokens);
				free(exe_path);
			}
	
			else printf("%s: command not found\n", tk.tokens[0]);
		}
	}

	return 0;
}