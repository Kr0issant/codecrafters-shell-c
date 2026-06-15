#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokenizer.h"

const int MAX_INPUT_LENGTH = 65536;
char *input_buffer;
Tokens tk;
const char *valid_commands[] = {"exit", "echo", "type"};
const int valid_command_count = sizeof(valid_commands) / sizeof(valid_commands[0]);

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
		printf("%s ", tk.tokens[i]);
	}
	printf("\n");
}

void type(Tokens tk) {
	bool found;
	for (int i = 1; i < tk.num_tokens; i++) {
		found = false;
		for (int j = 0; j < valid_command_count; j++) {
			if (strcmp(valid_commands[j], tk.tokens[i]) == 0) {
				found = true;
				break;
			}
		}
		if (found) printf("%s is a shell builtin\n", tk.tokens[i]);
		else printf("%s: not found\n", tk.tokens[i]);
	}
}

int main(int argc, char *argv[]) {
	atexit(cleanup_heap);
	setbuf(stdout, NULL);

	input_buffer = malloc(MAX_INPUT_LENGTH);
	if (input_buffer == NULL) return 1;

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

		if      (strcmp(tk.tokens[0], "exit") == 0) return 0;
		else if (strcmp(tk.tokens[0], "echo") == 0) echo(tk);
		else if (strcmp(tk.tokens[0], "type") == 0) type(tk);
	
		else printf("%s: command not found\n", input_buffer);
	}

	return 0;
}