#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "expand.h"
#include "history.h"

int main(void)
{
    char *input;
    token_list_t tokens;
    command_list_t commands;

    printf("=====================================\n");
    printf("           Shellforge\n");
    printf("   A Unix Style Shell written in C\n");
    printf("=====================================\n\n");

    while (1) {

        input = readline("shellforge$ ");

        if (input == NULL) {
            printf("\nExiting...\n");
            break;
        }

        if (input[0] == '\0') {
            free(input);
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            free(input);
            printf("Exiting...\n");
            break;
        }

        if (strcmp(input, "history") == 0) {
            shell_history_print();
            free(input);
            continue;
        }

        add_history(input);

        if (lexer(input, &tokens) == 0) {

            token_print(&tokens);

            if (parser(&tokens, &commands) == 0) {

                if (expand_command_list(&commands) == 0) {
                    command_print(&commands);
                } else {
                    fprintf(stderr, "Expansion failed\n");
                }

                command_list_free(&commands);

            } else {
                fprintf(stderr, "Parser error\n");
            }
        } else {
            fprintf(stderr, "Lexer error\n");
        }

        free(input);
    }

    return 0;
}
