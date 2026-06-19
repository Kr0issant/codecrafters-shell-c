#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../structs/cmd_structs.h"
#include "misc.h"

void remove_last_token(char *str, char delimiter) {
    size_t len = strlen(str);

    while (len > 1 && str[len - 1] == delimiter) {
        str[len - 1] = '\0';
        len--;
    }

    char *last_sep = strrchr(str, delimiter);
    
    if (last_sep != NULL) {
        if (last_sep == str) *(last_sep + 1) = '\0';
        else *last_sep = '\0';
    }
}

void free_tokens(Tokens *tk) {
	if (tk->tokens != NULL) {
		if (tk->num_tokens > 0) {
			for (int i = 0; i < tk->num_tokens; i++) free(tk->tokens[i]);
		}
		free(tk->tokens);
		tk->tokens = NULL;
	}
}

void free_command(Command *cmd) {
    if (cmd->tokens != NULL) {
		if (cmd->num_tokens > 0) {
			for (int i = 0; i < cmd->num_tokens; i++) free(cmd->tokens[i]);
		}
		free(cmd->tokens);
		cmd->tokens = NULL;
	}
}

char* read_input(char *buffer, int max_len) {
	if (fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
		return buffer;
    }

	return NULL;
}

void clear_screen() {
    printf("\e[1;1H\e[2J"); 
}