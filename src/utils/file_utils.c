#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "misc.h"
#include "file_utils.h"

#ifdef _WIN32
    #define PATH_SEPARATOR ";"
	#define DIR_SEPARATOR "\\"
	#define EXE_EXTENSION ".exe"
#else
    #define PATH_SEPARATOR ":"
	#define DIR_SEPARATOR "/"
	#define EXE_EXTENSION ""
#endif

int is_valid_directory(const char *path) {
    struct stat path_stat;
    
    if (stat(path, &path_stat) != 0) return 0;
    
    return S_ISDIR(path_stat.st_mode);
}

char* find_executable(char *program_name) {
	char *path_env = getenv("PATH");
    if (path_env == NULL) return NULL;

	char *path_copy = strdup(path_env);
	if (path_copy == NULL) return NULL;

	char *full_path = NULL;
	char *temp;
	bool found = false;
	
	char *dir = strtok(path_copy, PATH_SEPARATOR);
	while (dir != NULL) {
		temp = realloc(full_path, strlen(dir) + strlen(program_name) + strlen(EXE_EXTENSION) + 2);
		if (temp == NULL) {
			free(path_copy);
			free(full_path);
			return NULL;
		}
		full_path = temp;

		strcpy(full_path, dir);
		strcat(full_path, DIR_SEPARATOR);
		strcat(full_path, program_name);
		strcat(full_path, EXE_EXTENSION);

		if ((access(full_path, F_OK) == 0) && (access(full_path, X_OK) == 0)) {
			found = true;
			break;
		}

		dir = strtok(NULL, PATH_SEPARATOR);
	}
	
	free(path_copy);

	if (found) return full_path;
	else {
		free(full_path);

		if ((access(program_name, F_OK) == 0) && (access(program_name, X_OK) == 0)) return strdup(program_name);
		return NULL;
	}
}

char* resolve_relative_path(char *cwd, char *new_path) {
	char *path_copy = strdup(new_path);
	char *path = malloc(4096);
	
	strcpy(path, cwd);

	char *path_token = strtok(path_copy, DIR_SEPARATOR);

	while (path_token != NULL) {
		if (strcmp(path_token, "~") == 0) strcpy(path, getenv("HOME"));
		else if (strcmp(path_token, ".") == 0) continue;
		else if (strcmp(path_token, "..") == 0) remove_last_token(path, *DIR_SEPARATOR);
		else {
			strcat(path, DIR_SEPARATOR);
			strcat(path, path_token);
		}

		path_token = strtok(NULL, DIR_SEPARATOR);
	}

	free(path_copy);

	return path;
}