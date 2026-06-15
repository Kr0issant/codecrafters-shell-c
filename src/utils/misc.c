#include <stdlib.h>
#include <string.h>
#include "misc.h"

void remove_last_token(char *str, char delimiter) {
    size_t len = strlen(str);
    if (len == 0) return;

    if (str[len - 1] == delimiter) str[len - 1] = '\0';
    
    char *last_delim = strrchr(str, delimiter);
    
    if (last_delim != NULL) *last_delim = '\0';
    else str[0] = '\0'; 
}