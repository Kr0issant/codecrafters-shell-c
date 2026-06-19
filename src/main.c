#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "structs/cmd_structs.h"
#include "utils/misc.h"
#include "utils/file_utils.h"
#include "modules/builtins.h"
#include "modules/tokenizer.h"
#include "modules/get_a_job.h"
#include "modules/run_job.h"

const int MAX_INPUT_LENGTH = 65536;
char *input_buffer;
Tokens tk;

Job job = {NULL, 0};

char *cwd; // current working directory

void cleanup_heap() {
	if (input_buffer != NULL) {
		free(input_buffer);
		input_buffer = NULL;
	}
	free_tokens(&tk);

	for (int i = 0; i < job.num_commands; i++) {
        free_command(&(job.commands[i]));
    }
    if (job.commands != NULL) {
        free(job.commands);
    }

    if (cwd != NULL) {
        free(cwd);
        cwd = NULL;
    }
}

int main(int argc, char *argv[]) {
	atexit(cleanup_heap);
	setbuf(stdout, NULL);
	// clear_screen();
	
	input_buffer = malloc(MAX_INPUT_LENGTH);
	if (input_buffer == NULL) return 1;
	
	char *exe_path;
	
	cwd = malloc(4096);
	if (getcwd(cwd, 4096) == NULL) return 1;
	
	char *temp_inp;
	Tokens temp_tk;

	Job temp_job;

	int exit;
	
	while (true) {
		printf("$ ");
	
		temp_inp = read_input(input_buffer, MAX_INPUT_LENGTH);
		if (temp_inp == NULL) return 1;
		input_buffer = temp_inp;

		temp_tk = get_tokens(input_buffer);
		if (temp_tk.tokens == NULL) return 1;
		free_tokens(&tk);
		tk = temp_tk;

		temp_job = get_job(tk);
		if (temp_job.commands == NULL) return 1;
		for (int i = 0; i < job.num_commands; i++) free_command(&(job.commands[i]));
		free(job.commands);
		job = temp_job;
		
		exit = run_job(job, &cwd);
		if (exit == -2) break;
		if (exit != 0) printf("invalid command\n");
	}

	return 0;
}