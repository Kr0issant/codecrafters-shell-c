#include "../structs/cmd_structs.h"

#ifndef MISC_H
#define MISC_H

void remove_last_token(char *str, char delimiter);
void free_tokens(Tokens *tk);
void free_command(Command *cmd);
char* read_input(char *buffer, int max_len);
void clear_screen();

#endif