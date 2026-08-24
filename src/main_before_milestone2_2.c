#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "token.h"
#include "history.h"

int main(void)
{
    char *input;
    token_list_t list;
printf("=====================================\n");
printf("           Shellforge\n");
printf("   A Unix Style Shell written in C\n");
printf("=====================================\n\n");
    printf("Shellforge\n\n");
    printf("A Unix Style Shell written in C\n\n");

    while (1) {

        input = readline("shellforge$ ");

        /*
         * Ctrl+D
         */
        if (input == NULL) {
            printf("\nExiting...\n");
            break;
        }

        /*
         * Ignore empty input.
         */
        if (input[0] == '\0') {
            free(input);
            continue;
        }

        /*
         * Exit is not stored in history.
         */
        if (strcmp(input, "exit") == 0) {
            free(input);
            printf("Exiting...\n");
            break;
        }

        /*
         * Print command history.
         */
        if (strcmp(input, "history") == 0) {
            shell_history_print();
            free(input);
            continue;
        }

        /*
         * Add normal command to history.
         */
        add_history(input);

        /*
         * Tokenize the command.
         */
        if (lexer(input, &list) == 0) {
            token_print(&list);
        }

        free(input);
    }

    return 0;
}
