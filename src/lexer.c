#include <ctype.h>
#include <stddef.h>
#include "lexer.h"

int lexer(const char *input, token_list_t *list)
{
    int i = 0;

    if (input == NULL || list == NULL) {
        return -1;
    }

    token_list_init(list);

    while (input[i] != '\0') {

        char word[MAX_TOKEN_LEN];
        int j = 0;

        /* Skip whitespace */
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        /* Pipe */
        if (input[i] == '|') {
            token_add(list, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        /* Input redirection */
        if (input[i] == '<') {
            token_add(list, TOKEN_INPUT, "<");
            i++;
            continue;
        }

        /* Output / append redirection */
        if (input[i] == '>') {

            if (input[i + 1] == '>') {
                token_add(list, TOKEN_APPEND, ">>");
                i += 2;
            } else {
                token_add(list, TOKEN_OUTPUT, ">");
                i++;
            }

            continue;
        }

        /* Background */
        if (input[i] == '&') {
            token_add(list, TOKEN_BACKGROUND, "&");
            i++;
            continue;
        }

        word[0] = '\0';

        /*
         * Build a WORD token.
         */
        while (input[i] != '\0') {

            char c = input[i];

            /*
             * A delimiter means the current WORD is finished.
             */
            if (isspace((unsigned char)c) ||
                c == '|' ||
                c == '<' ||
                c == '>' ||
                c == '&') {
                break;
            }

            /*
             * Escape character.
             */
            if (c == '\\') {

                i++;

                if (input[i] == '\0') {
                    break;
                }

                if (j < MAX_TOKEN_LEN - 1) {
                    word[j++] = input[i];
                }

                i++;
                continue;
            }

            /*
             * Single or double quoted text.
             * Quotes themselves are not included in the WORD.
             */
            if (c == '\'' || c == '"') {

                char quote = c;

                i++;

                while (input[i] != '\0' &&
                       input[i] != quote) {

                    if (j < MAX_TOKEN_LEN - 1) {
                        word[j++] = input[i];
                    }

                    i++;
                }

                if (input[i] == quote) {
                    i++;
                }

                continue;
            }

            /*
             * Normal character.
             */
            if (j < MAX_TOKEN_LEN - 1) {
                word[j++] = c;
            }

            i++;
        }

        word[j] = '\0';

        if (j > 0) {
            token_add(list, TOKEN_WORD, word);
        }
    }

    /*
     * Every command ends with END.
     */
    token_add(list, TOKEN_END, "END");

    return 0;
}
