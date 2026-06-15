#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_input() {
	const int MAX_LENGTH = 65536;

	char *buffer = malloc(MAX_LENGTH);
	if (buffer == NULL) return NULL;

	if (fgets(buffer, MAX_LENGTH, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }

	return buffer;
}

int main(int argc, char *argv[]) {
	// Flush after every printf
	setbuf(stdout, NULL);

	printf("$ ");

	char *input = read_input();
	if (input == NULL) return 1;

	printf("%s: command not found\n", input);

	return 0;
}