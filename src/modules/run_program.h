#include "../structs/program_structs.h"

#ifndef RUN_PROGRAM_H
#define RUN_PROGRAM_H

Program run_program(char *program_path, char *args[], char *output_file, int fd, int append, int is_bg);
void run_builtin(Command cmd, char **cwd, char *output_file, int fd, int append, int is_bg);

#endif