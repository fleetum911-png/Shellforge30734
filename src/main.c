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
#include "builtin.h"
#include "executor.h"


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

        /*
         * Ctrl+D / EOF
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
         * History command.
         */
        if (strcmp(input, "history") == 0) {
            shell_history_print();
            free(input);
            continue;
        }

        /*
         * Add command to readline history.
         */
        add_history(input);

        /*
         * Lex input.
         */
        if (lexer(input, &tokens) == 0) {

            /*
             * Parse tokens.
             */
            if (parser(&tokens, &commands) == 0) {

                /*
                 * Perform expansion.
                 */
                if (expand_command_list(&commands) == 0) {

                    /*
                     * Execute the parsed command list.
                     */
                    execute_command_list(&commands);

                } else {

                    fprintf(stderr, "Expansion failed\n");
                }

                /*
                 * Free command memory.
                 */
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
