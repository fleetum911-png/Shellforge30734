#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

/*
 * Execute a complete command list.
 *
 * Handles:
 * - Builtin commands
 * - External commands
 * - Input redirection
 * - Output redirection
 * - Append redirection
 * - Pipes
 * - Background execution
 */
int execute_command_list(const command_list_t *commands);

#endif
