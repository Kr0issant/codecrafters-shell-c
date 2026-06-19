#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "get_a_job.h"

char *links[] = {">", "<", "|", ";", "&", ">>", "0>", "1>", "2>", "&&", "||", "0>>", "1>>", "2>>"};
int num_links = sizeof(links) / sizeof(links[0]);

int enlarge_commands(Command** commands, int max_commands) {
    Command *temp = realloc(*commands, (max_commands + 4) * sizeof(Command));
    
    if (temp == NULL) {
        printf("Could not enlarge commands array\n");
        return 1;
    }

    *commands = temp;
    return 0;
}

Command new_command(Tokens tk, int tk_start, int tk_end, char *link) {
    int len = tk_end - tk_start;
    if (len <= 0) return (Command){NULL, 0, 0, -1, NULL};

    Command cmd;
    cmd.tokens = malloc((len + 1) * sizeof(char *));
    if (cmd.tokens == NULL) return (Command){NULL, 0, 0, -1, NULL};
    cmd.num_tokens = len;
    cmd.link = link ? strdup(link) : NULL;

    for (int i = tk_start; i < tk_end; i++) {
        cmd.tokens[i - tk_start] = strdup(tk.tokens[i]);
    }
    
    cmd.tokens[len] = NULL;
    return cmd;
}

int add_command(Command** commands, int* max_commands, int* num_commands, Command* new_command) {
    if (*num_commands == *max_commands) {
        if (enlarge_commands(commands, *max_commands)) return 1;
        *max_commands += 4;
    }

    (*commands)[*num_commands] = *(new_command);
    (*num_commands)++;
    return 0;
}

Job get_job(Tokens tk) {
    int max_commands = 4;
    int num_commands = 0;
    Command *commands = malloc(max_commands * sizeof(Command));

    if (commands == NULL) {
        Job fail_result = {NULL, 0};
        return fail_result;
    }

    bool ended = false;
    bool last_link = false;
    char *token;
    Command cmd;

    int start = 0;
    int i = 0;

    for (i = 0; i < tk.num_tokens; i++) {
        token = tk.tokens[i];

        for (int j = 0; j < num_links; j++) {
            if (strcmp(token, links[j]) == 0) {
                if (last_link) {
                    start = i + 1;
                    ended = true;
                    break;
                }

                cmd = new_command(tk, start, i, links[j]);
                if (cmd.tokens == NULL || add_command(&commands, &max_commands, &num_commands, &cmd)) {
                    free(commands);
                    return (Job){NULL, 0};
                }
                start = i + 1;
                ended = true;
                last_link = true;
                break;
            }
        }
        if (ended) { ended = false; continue; }
        last_link = false;

        if (i == tk.num_tokens - 1 && start <= i) {
            cmd = new_command(tk, start, i + 1, NULL);
            if (cmd.tokens == NULL || add_command(&commands, &max_commands, &num_commands, &cmd)) {
                free(commands);
                return (Job){NULL, 0};
            }
            break;
        }
    }

    Job result;
    result.commands = commands;
    result.num_commands = num_commands;
    return result;
}

// #include "tokenizer.h"
// int main(int argc, char *argv[]) {
//     if (argc < 2) {
//         printf("No arguments provided\n");
//         return 1;
//     }

//     Tokens tokens = get_tokens(argv[1]);
//     Job job = get_job(tokens);

//     if (job.commands == NULL) {
//         printf("error\n");
//         return 1;
//     }

//     for (int i = 0; i < job.num_commands; i++) {
//         printf("Command %d: ", i);
//         for (int j = 0; j < job.commands[i].num_tokens; j++) {
//             printf("(%s) ", job.commands[i].tokens[j]);
//             free(job.commands[i].tokens[j]);
//         }
//         printf(" %s ", job.commands[i].link);
//         printf("\n");
//         free(job.commands[i].tokens);
//         free(job.commands[i].link);
//     }
//     free(job.commands);

//     for (int i = 0; i < tokens.num_tokens; i++) free(tokens.tokens[i]);
//     free(tokens.tokens);

//     return 0;
// }