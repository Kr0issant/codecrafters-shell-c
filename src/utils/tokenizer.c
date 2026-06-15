#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tokenizer.h"

int enlarge_tokens(char ***tokens, int max) {
    char **temp = (char **)realloc(*tokens, (max + 8) * sizeof(char *));

    if (temp == NULL) {
        printf("Could not enlarge tokens array\n");
        return 1;
    }

    *tokens = temp;
    return 0;
}

int add_token(char ***tokens, int *num_tokens, int *max_tokens, const char *str) {
    if (strlen(str) == 0) return true;

    (*tokens)[*num_tokens] = strdup(str);
    if ((*tokens)[*num_tokens] == NULL) return 1;

    (*num_tokens)++;
    if (*num_tokens == *max_tokens) {
        if (enlarge_tokens(tokens, *max_tokens) == 1) {
            return 1;
        }
        *max_tokens += 8;
    }
    return 0;
}

Tokens get_tokens(char *input) {
    int max_tokens = 8;
    int num_tokens = 0;
    char **tokens = (char **)malloc(max_tokens * sizeof(char *));

    if (tokens == NULL) {
        Tokens fail_result = {NULL, 0};
        return fail_result;
    }

    int buf_size = strlen(input) + 1;
    char *current_token = (char *)malloc(buf_size);
    int buf_idx = 0;

    bool is_subtoken = false;
    char opening = '\0';
    int i = 0;

    while (true) {
        char c = input[i];

        if ((c == '\"' || c == '\'') && (!is_subtoken || (is_subtoken && c == opening))) {
            if (!is_subtoken) opening = c;
            else opening = '\0';
            
            is_subtoken = !is_subtoken;
            i++;
            continue;
        }

        if (is_subtoken) {
            if (c == '\0') break;

            if (opening == '\"' && c == '\\' && (input[i + 1] == '\\' || input[i + 1] == '\"')) {
                current_token[buf_idx++] = input[i + 1];
                i += 2;
                continue;
            }

            current_token[buf_idx++] = c;
            i++;
            continue;
        }

        bool is_op2 = (c == '>' && input[i+1] == '>') || 
                      (c == '&' && input[i+1] == '&') || 
                      (c == '|' && input[i+1] == '|');

        bool is_op1 = !is_op2 && (c == '>' || c == '<' || c == '|');

        if (c == ' ' || c == '\0' || is_op1 || is_op2) {
            if (buf_idx > 0) {
                current_token[buf_idx] = '\0';
                if (add_token(&tokens, &num_tokens, &max_tokens, current_token)) {
                    free(current_token);
                    Tokens partial_result = {tokens, num_tokens};
                    return partial_result;
                }
                buf_idx = 0;
            }

            if (is_op2) {
                char op[3] = {c, input[i+1], '\0'};
                add_token(&tokens, &num_tokens, &max_tokens, op);
                i += 2;
                continue;
            } else if (is_op1) {
                char op[2] = {c, '\0'};
                add_token(&tokens, &num_tokens, &max_tokens, op);
                i++;
                continue;
            }

            if (c == '\0') break;
            
            i++;
            continue;
        }

        if (c == '\\' && input[i + 1] != '\0') {
            current_token[buf_idx++] = input[i + 1];
            i += 2;
            continue;
        }

        current_token[buf_idx++] = c;
        i++;
    }

    free(current_token);

    Tokens result;
    result.tokens = tokens;
    result.num_tokens = num_tokens;
    return result;
}

// int main(int argc, char *argv[]) {
//     if (argc < 2) {
//         printf("No arguments provided\n");
//         return 1;
//     }

//     Tokens tokens = get_tokens(argv[1]);

//     for (int i = 0; i < tokens.num_tokens; i++) {
//         printf("Token %d: %s\n", i, tokens.tokens[i]);
//         free(tokens.tokens[i]);
//     }
//     free(tokens.tokens);

//     return 0;
// }