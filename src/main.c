#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "tokenizer.h"
#include "run_program.h"

#ifdef _WIN32
    #define PATH_SEPARATOR ";"
	#define DIR_SEPARATOR "\\"
	#define EXE_EXTENSION ".exe"
#else
    #define PATH_SEPARATOR ":"
	#define DIR_SEPARATOR "/"
	#define EXE_EXTENSION ""
#endif

const int MAX_INPUT_LENGTH = 65536;
char *input_buffer;
Tokens tk;

const char *valid_commands[] = {"exit", "echo", "type", "pwd"};
const int valid_command_count = sizeof(valid_commands) / sizeof(valid_commands[0]);

char cwd[1024];

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

char* find_executable(char *program_name) {
	char *path_env = getenv("PATH");
    if (path_env == NULL) return NULL;

	char *path_copy = strdup(path_env);
	if (path_copy == NULL) return NULL;

	char *full_path = NULL;
	char *temp;
	bool found = false;
	
	char *dir = strtok(path_copy, PATH_SEPARATOR);
	while (dir != NULL) {
		temp = realloc(full_path, strlen(dir) + strlen(program_name) + strlen(EXE_EXTENSION) + 2);
		if (temp == NULL) {
			free(path_copy);
			free(full_path);
			return NULL;
		}
		full_path = temp;

		strcpy(full_path, dir);
		strcat(full_path, DIR_SEPARATOR);
		strcat(full_path, program_name);
		strcat(full_path, EXE_EXTENSION);

		if ((access(full_path, F_OK) == 0) && (access(full_path, X_OK) == 0)) {
			found = true;
			break;
		}

		dir = strtok(NULL, PATH_SEPARATOR);
	}
	
	free(path_copy);

	if (found) return full_path;
	else {
		free(full_path);

		if ((access(program_name, F_OK) == 0) && (access(program_name, X_OK) == 0)) return strdup(program_name);
		return NULL;
	}
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
		for (int j = 0; j < valid_command_count; j++) {
			if (strcmp(valid_commands[j], tk.tokens[i]) == 0) {
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

int main(int argc, char *argv[]) {
	atexit(cleanup_heap);
	setbuf(stdout, NULL);

	input_buffer = malloc(MAX_INPUT_LENGTH);
	if (input_buffer == NULL) return 1;
	
	char *exe_path;

	if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

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