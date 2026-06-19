#include "tokenizer.h"

#ifndef BUILTINS_H
#define BUILTINS_H

extern const char* valid_builtins[];
extern const int valid_builtins_count;

void echo(Command cmd);
void type(Command cmd);
void pwd(char *cwd);
void cd(char *new_path, char **cwd);

#endif