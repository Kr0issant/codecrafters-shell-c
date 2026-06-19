#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../structs/cmd_structs.h"
#include "misc.h"
#include "file_utils.h"

#ifdef _WIN32
    #define PATH_SEPARATOR ";"
	#define DIR_SEPARATOR "\\"
	#define EXE_EXTENSION ".exe"
	#define HOME_DIR "USERPROFILE"
	#else
    #define PATH_SEPARATOR ":"
	#define DIR_SEPARATOR "/"
	#define EXE_EXTENSION ""
	#define HOME_DIR "HOME"
#endif

int is_valid_file(const char *path) {
	struct stat path_stat;
	if (stat(path, &path_stat) != 0) return 0;
	return S_ISREG(path_stat.st_mode);
}

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
	if (!new_path || !cwd) return NULL;

	char *path_copy = strdup(new_path);
	if (!path_copy) return NULL;

	size_t max_len = strlen(cwd) + strlen(new_path) + 2;
    if (max_len < 4096) max_len = 4096;
    char *path = malloc(max_len);
    if (!path || !path_copy) {
        free(path_copy);
        return NULL;
    }

	path[0] = '\0';

    if (new_path[0] == DIR_SEPARATOR[0]) strcpy(path, DIR_SEPARATOR);
    else if (new_path[0] == '~') {
        char *home = getenv(HOME_DIR);
        if (!home) {
            free(path_copy);
            free(path);
            return NULL;
        }
        strcpy(path, home);
    } else strcpy(path, cwd);

    char *path_token = strtok(path_copy, DIR_SEPARATOR);
    int is_first_token = 1;

    while (path_token != NULL) {
        if (strcmp(path_token, "~") == 0 && is_first_token && strcmp(path_token, "~") == 0);
        else if (strcmp(path_token, ".") == 0);
        else if (strcmp(path_token, "..") == 0) remove_last_token(path, *DIR_SEPARATOR);
        else {
            size_t len = strlen(path);
            if (len > 0 && path[len - 1] != DIR_SEPARATOR[0]) strcat(path, DIR_SEPARATOR);
            strcat(path, path_token);
        }

        is_first_token = 0;
        path_token = strtok(NULL, DIR_SEPARATOR);
    }

    if (strlen(path) == 0) {
        strcpy(path, DIR_SEPARATOR);
    }

    free(path_copy);
    return path;
}

int mark_files_in_job(Job *job, char *cwd) {
    if (job == NULL || cwd == NULL) return 1;

    for (int i = 0; i < job->num_commands; i++) {
        char *token = job->commands[i].tokens[0];
        if (token == NULL) continue;

        size_t rel_path_len = strlen(cwd) + strlen(token) + 2;
        char *rel_path = malloc(rel_path_len);
        if (rel_path == NULL) return 1;

        snprintf(rel_path, rel_path_len, "%s%s%s", cwd, DIR_SEPARATOR, token);

        char *full_path = resolve_relative_path(cwd, rel_path);
        
        if (full_path != NULL && is_valid_file(full_path)) {
            job->commands[i].is_file = 1;
        } else {
            free(full_path);
            full_path = resolve_relative_path(cwd, token);
            if (full_path != NULL) job->commands[i].is_file = is_valid_file(full_path);
            else job->commands[i].is_file = 0;
        }
        
        if (job->commands[i].is_file && full_path != NULL) {
            free(job->commands[i].tokens[0]);
            job->commands[i].tokens[0] = full_path;
            full_path = NULL;
        }

        free(rel_path);
        free(full_path);
    }
    return 0;
}