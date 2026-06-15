#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

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