#ifndef RUN_PROGRAM_H
#define RUN_PROGRAM_H

int run_program(char *program_path, char *args[], char *output_file, int fd, int append);
void run_builtin(Command cmd, char **cwd, char *output_file);

#endif