#include <stdlib.h>
#include <string.h>
#include "misc.h"

void remove_last_token(char *str, char delimiter) {
    size_t len = strlen(str);

    while (len > 1 && str[len - 1] == delimiter) {
        str[len - 1] = '\0';
        len--;
    }

    char *last_sep = strrchr(str, delimiter);
    
    if (last_sep != NULL) {
        if (last_sep == str) *(last_sep + 1) = '\0';
        else *last_sep = '\0';
    }
}