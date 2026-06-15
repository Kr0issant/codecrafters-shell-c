#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_INPUT_LENGTH = 65536;
char *input_buffer;

void cleanup_heap() {
	if (input_buffer != NULL) {
		free(input_buffer);
		input_buffer = NULL;
	}
}

char* read_input(char *buffer, int max_len) {
	if (fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
		return buffer;
    }

	return NULL;
}

int main(int argc, char *argv[]) {
	atexit(cleanup_heap);
	setbuf(stdout, NULL);

	input_buffer = malloc(MAX_INPUT_LENGTH);
	if (input_buffer == NULL) return 1;

	char *temp;

	while (1) {
		printf("$ ");
	
		temp = read_input(input_buffer, MAX_INPUT_LENGTH);
		if (temp == NULL) return 1;
		input_buffer = temp;
	
		printf("%s: command not found\n", input_buffer);
	}

	return 0;
}