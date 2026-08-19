#include <stdlib.h>
#include <string.h>

#include "expand.h"

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

static char *expand_variable(const char *text)
{
    const char *value;
    char name[256];
    char result[256];

    int i = 0;
    int j = 0;

    if (text == NULL) {
        return NULL;
    }

    while (text[i] != '\0') {

        if (text[i] == '$' && text[i + 1] == '{') {

            i += 2;
            j = 0;

            while (text[i] != '\0' &&
                   text[i] != '}' &&
                   j < (int)sizeof(name) - 1) {

                name[j++] = text[i++];
            }

            name[j] = '\0';

            if (text[i] == '}') {
                i++;
            }

            value = getenv(name);

            if (value != NULL) {
                strncpy(result, value, sizeof(result) - 1);
                result[sizeof(result) - 1] = '\0';

                return duplicate_string(result);
            }

            return duplicate_string("");
        }

        if (text[i] == '$') {

            i++;
            j = 0;

            while (text[i] != '\0' &&
                   ((text[i] >= 'A' && text[i] <= 'Z') ||
                    (text[i] >= 'a' && text[i] <= 'z') ||
                    (text[i] >= '0' && text[i] <= '9') ||
                    text[i] == '_') &&
                   j < (int)sizeof(name) - 1) {

                name[j++] = text[i++];
            }

            name[j] = '\0';

            value = getenv(name);

            if (value != NULL) {
                strncpy(result, value, sizeof(result) - 1);
                result[sizeof(result) - 1] = '\0';

                return duplicate_string(result);
            }

            return duplicate_string("");
        }

        i++;
    }

    return duplicate_string(text);
}

int expand_command_list(command_list_t *list)
{
    int i;
    int j;
    char *expanded;

    if (list == NULL) {
        return -1;
    }

    for (i = 0; i < list->count; i++) {

        for (j = 0; j < list->commands[i].argc; j++) {

            expanded =
                expand_variable(list->commands[i].argv[j]);

            if (expanded == NULL) {
                return -1;
            }

            free(list->commands[i].argv[j]);
            list->commands[i].argv[j] = expanded;
        }

        if (list->commands[i].input_file != NULL) {

            expanded =
                expand_variable(list->commands[i].input_file);

            if (expanded == NULL) {
                return -1;
            }

            free(list->commands[i].input_file);
            list->commands[i].input_file = expanded;
        }

        if (list->commands[i].output_file != NULL) {

            expanded =
                expand_variable(list->commands[i].output_file);

            if (expanded == NULL) {
                return -1;
            }

            free(list->commands[i].output_file);
            list->commands[i].output_file = expanded;
        }
    }

    return 0;
}
