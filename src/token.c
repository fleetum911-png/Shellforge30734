#include <stdio.h>
#include <string.h>
#include "token.h"

void token_list_init(token_list_t *list)
{
    if (list != NULL) {
        list->count = 0;
    }
}

void token_add(token_list_t *list, token_type_t type, const char *text)
{
    token_t *token;

    if (list == NULL || list->count >= MAX_TOKENS) {
        return;
    }

    token = &list->tokens[list->count];

    token->type = type;

    if (text != NULL) {
        strncpy(token->text, text, MAX_TOKEN_LEN - 1);
        token->text[MAX_TOKEN_LEN - 1] = '\0';
    } else {
        token->text[0] = '\0';
    }

    list->count++;
}

const char *token_name(token_type_t type)
{
    switch (type) {
        case TOKEN_WORD:
            return "WORD";

        case TOKEN_PIPE:
            return "PIPE";

        case TOKEN_INPUT:
            return "INPUT";

        case TOKEN_OUTPUT:
            return "OUTPUT";

        case TOKEN_APPEND:
            return "APPEND";

        case TOKEN_BACKGROUND:
            return "BACKGROUND";

        case TOKEN_END:
            return "END";

        default:
            return "UNKNOWN";
    }
}

void token_print(const token_list_t *list)
{
    int i;

    if (list == NULL) {
        return;
    }

    printf("\n");
    printf("------------- TOKENS -------------\n");

    for (i = 0; i < list->count; i++) {
        printf("%d : %-10s %s\n",
               i,
               token_name(list->tokens[i].type),
               list->tokens[i].text);
    }

    printf("----------------------------------\n");
}
