#ifndef CMD_STRUCTS_H
#define CMD_STRUCTS_H

typedef struct {
    char** tokens;
    int num_tokens;
} Tokens;

typedef struct {
    char** tokens;
    int num_tokens;
    int is_file;
    int status;
    char* link;
} Command;

typedef struct {
    Command* commands;
    int num_commands;
} Job;

#endif