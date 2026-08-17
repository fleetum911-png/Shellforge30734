#include <stdio.h>
#include <readline/history.h>
#include "history.h"

void shell_history_print(void)
{
    HIST_ENTRY **entries;
    int i;

    entries = history_list();

    printf("----- Command History -----\n");

    if (entries != NULL) {

        for (i = 0; entries[i] != NULL; i++) {
            printf("%d  %s\n",
                   i + 1,
                   entries[i]->line);
        }
    }

    printf("---------------------------\n");
}
