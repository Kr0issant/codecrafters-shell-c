#ifndef BUILTINS_H
#define BUILTINS_H

#include "../structs/cmd_structs.h"

extern const char* valid_builtins[];
extern const int valid_builtins_count;

void run_builtin(Command cmd, char **cwd, char *output_file, int fd, int append, int is_bg);
void echo(Command cmd);
void type(Command cmd);
void pwd(char *cwd);
void cd(char *new_path, char **cwd);

#endif