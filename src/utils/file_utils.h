#ifndef FILE_UTILS_H
#define FILE_UTILS_H

int is_valid_directory(const char *path);
char* find_executable(char *program_name);
char* resolve_relative_path(char *cwd, char *new_path);
int mark_files_in_job(Job *job, char *cwd);

#endif