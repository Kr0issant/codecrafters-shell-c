#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef struct {
    char** tokens;
    int num_tokens;
} Tokens;

Tokens get_tokens(char *input);

#endif