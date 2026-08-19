#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

void command_list_init(command_list_t *list)
{
    int i;

    if (list == NULL) {
        return;
    }

    list->count = 0;

    for (i = 0; i < MAX_TOKENS; i++) {
        list->commands[i].argc = 0;
        list->commands[i].input_file = NULL;
        list->commands[i].output_file = NULL;
        list->commands[i].append = 0;
        list->commands[i].background = 0;

        for (int j = 0; j < MAX_ARGS; j++) {
            list->commands[i].argv[j] = NULL;
        }
    }
}

static char *duplicate_string(const char *text)
{
    char *copy;

    if (text == NULL) {
        return NULL;
    }

    copy = malloc(strlen(text) + 1);

    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, text);

    return copy;
}

int parser(const token_list_t *tokens, command_list_t *commands)
{
    int i;
    command_t *current;

    if (tokens == NULL || commands == NULL) {
        return -1;
    }

    command_list_init(commands);

    current = &commands->commands[0];
    commands->count = 1;

    for (i = 0; i < tokens->count; i++) {

        token_t *token = (token_t *)&tokens->tokens[i];

        switch (token->type) {

            case TOKEN_WORD:

                if (current->argc >= MAX_ARGS - 1) {
                    return -1;
                }

                current->argv[current->argc] =
                    duplicate_string(token->text);

                if (current->argv[current->argc] == NULL) {
                    command_list_free(commands);
                    return -1;
                }

                current->argc++;
                current->argv[current->argc] = NULL;

                break;

            case TOKEN_INPUT:

                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD) {
                    command_list_free(commands);
                    return -1;
                }

                i++;

                current->input_file =
                    duplicate_string(tokens->tokens[i].text);

                if (current->input_file == NULL) {
                    command_list_free(commands);
                    return -1;
                }

                break;

            case TOKEN_OUTPUT:

                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD) {
                    command_list_free(commands);
                    return -1;
                }

                i++;

                current->output_file =
                    duplicate_string(tokens->tokens[i].text);

                if (current->output_file == NULL) {
                    command_list_free(commands);
                    return -1;
                }

                current->append = 0;

                break;

            case TOKEN_APPEND:

                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD) {
                    command_list_free(commands);
                    return -1;
                }

                i++;

                current->output_file =
                    duplicate_string(tokens->tokens[i].text);

                if (current->output_file == NULL) {
                    command_list_free(commands);
                    return -1;
                }

                current->append = 1;

                break;

            case TOKEN_PIPE:

                if (current->argc == 0) {
                    command_list_free(commands);
                    return -1;
                }

                if (commands->count >= MAX_TOKENS) {
                    command_list_free(commands);
                    return -1;
                }

                current = &commands->commands[commands->count];

                current->argc = 0;
                current->input_file = NULL;
                current->output_file = NULL;
                current->append = 0;
                current->background = 0;

                for (int j = 0; j < MAX_ARGS; j++) {
                    current->argv[j] = NULL;
                }

                commands->count++;

                break;

            case TOKEN_BACKGROUND:

                current->background = 1;
                break;

            case TOKEN_END:

                break;

            default:

                command_list_free(commands);
                return -1;
        }
    }

    if (commands->count > 0 &&
        commands->commands[commands->count - 1].argc == 0) {

        commands->count--;
    }

    return 0;
}

void command_list_free(command_list_t *list)
{
    int i;
    int j;

    if (list == NULL) {
        return;
    }

    for (i = 0; i < list->count; i++) {

        for (j = 0; j < list->commands[i].argc; j++) {
            free(list->commands[i].argv[j]);
            list->commands[i].argv[j] = NULL;
        }

        free(list->commands[i].input_file);
        free(list->commands[i].output_file);

        list->commands[i].input_file = NULL;
        list->commands[i].output_file = NULL;
    }

    list->count = 0;
}

void command_print(const command_list_t *list)
{
    int i;
    int j;

    if (list == NULL) {
        return;
    }

    printf("\n");
    printf("------------ COMMANDS ------------\n");

    for (i = 0; i < list->count; i++) {

        printf("Command %d:\n", i + 1);

        printf("  argv:");

        for (j = 0; j < list->commands[i].argc; j++) {
            printf(" [%s]", list->commands[i].argv[j]);
        }

        printf("\n");

        if (list->commands[i].input_file != NULL) {
            printf("  input: %s\n",
                   list->commands[i].input_file);
        }

        if (list->commands[i].output_file != NULL) {
            printf("  output: %s%s\n",
                   list->commands[i].output_file,
                   list->commands[i].append ? " (append)" : "");
        }

        printf("  background: %s\n",
               list->commands[i].background ? "yes" : "no");
    }

    printf("----------------------------------\n");
}
