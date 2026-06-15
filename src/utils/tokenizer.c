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

Tokens get_tokens(char *input) {
    int max_tokens = 8;
    int num_tokens = 0;
    char **tokens = (char **)malloc(max_tokens * sizeof(char *));

    if (tokens == NULL) {
        Tokens fail_result = {NULL, 0};
        return fail_result;
    }

    bool is_subtoken = false;
    int start = 0;
    int length;
    char c;
    char opening = '\0';

    int i = 0;
    while (true) {
        c = *(input + i);

        if ((c == '\"' || c == '\'') && (!is_subtoken || (is_subtoken && c == opening))) {
            length = i - start;
            if (length > 0) {
                tokens[num_tokens] = (char *)malloc((length + 1) * sizeof(char));
                if (tokens[num_tokens] != NULL) {
                    strncpy(tokens[num_tokens], input + start, length);
                    tokens[num_tokens][length] = '\0';

                    num_tokens++;
                    if (num_tokens == max_tokens) {
                        if (enlarge_tokens(&tokens, max_tokens) == 1) {
                            Tokens partial_result = {tokens, num_tokens};
                            return partial_result;
                        }
                        max_tokens += 8;
                    }
                }
            }
            if (!is_subtoken) opening = c;
            is_subtoken = !is_subtoken;
            start = i + 1;
        }

        else if (!is_subtoken && (c == ' ' || c == '\0')) {
            length = i - start;
            if (length > 0) {
                tokens[num_tokens] = (char *)malloc((length + 1) * sizeof(char));
                if (tokens[num_tokens] != NULL) {
                    strncpy(tokens[num_tokens], input + start, length);
                    tokens[num_tokens][length] = '\0';

                    num_tokens++;
                    if (num_tokens == max_tokens) {
                        if (enlarge_tokens(&tokens, max_tokens) == 1) {
                            Tokens partial_result = {tokens, num_tokens};
                            return partial_result;
                        }
                        max_tokens += 8;
                    }
                }
            }
            start = i + 1;
        }

        if (c == '\0') break;
        i++;
    }

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