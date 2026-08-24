#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"


/*
 * Builtin: cd
 *
 * Changes the current working directory.
 *
 * Usage:
 *     cd
 *     cd ~
 *     cd <directory>
 */
int builtin_cd(char **argv)
{
    char *directory;

    if (argv == NULL) {
        return 1;
    }

    /*
     * cd with no argument or cd ~
     * goes to the user's HOME directory.
     */
    if (argv[1] == NULL || strcmp(argv[1], "~") == 0) {

        directory = getenv("HOME");

        if (directory == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }

    } else {

        directory = argv[1];
    }

    /*
     * Check for too many arguments.
     */
    if (argv[2] != NULL) {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    }

    /*
     * Change directory.
     */
    if (chdir(directory) != 0) {
        perror("cd");
        return 1;
    }

    return 0;
}


/*
 * Builtin: pwd
 *
 * Prints the current working directory.
 */
int builtin_pwd(char **argv)
{
    char *current_directory;

    (void)argv;

    /*
     * getcwd(NULL, 0) asks the system to allocate
     * enough memory for the current directory.
     */
    current_directory = getcwd(NULL, 0);

    if (current_directory == NULL) {
        perror("pwd");
        return 1;
    }

    printf("%s\n", current_directory);

    free(current_directory);

    return 0;
}


/*
 * Builtin: echo
 *
 * Prints all arguments separated by spaces.
 *
 * Example:
 *     echo Hello World
 *
 * Output:
 *     Hello World
 */
int builtin_echo(char **argv)
{
    int i = 1;

    if (argv == NULL) {
        return 1;
    }

    while (argv[i] != NULL) {

        printf("%s", argv[i]);

        if (argv[i + 1] != NULL) {
            printf(" ");
        }

        i++;
    }

    printf("\n");

    return 0;
}


/*
 * Builtin: exit
 *
 * Terminates Shellforge.
 */
int builtin_exit(char **argv)
{
    (void)argv;

    printf("Exiting...\n");

    exit(0);
}


/*
 * Check whether a command is a builtin.
 */
int is_builtin(char *command)
{
    if (command == NULL) {
        return 0;
    }

    if (strcmp(command, "cd") == 0) {
        return 1;
    }

    if (strcmp(command, "pwd") == 0) {
        return 1;
    }

    if (strcmp(command, "echo") == 0) {
        return 1;
    }

    if (strcmp(command, "exit") == 0) {
        return 1;
    }

    return 0;
}


/*
 * Execute a builtin command.
 */
int execute_builtin(char **argv)
{
    if (argv == NULL || argv[0] == NULL) {
        return 1;
    }

    if (strcmp(argv[0], "cd") == 0) {
        return builtin_cd(argv);
    }

    if (strcmp(argv[0], "pwd") == 0) {
        return builtin_pwd(argv);
    }

    if (strcmp(argv[0], "echo") == 0) {
        return builtin_echo(argv);
    }

    if (strcmp(argv[0], "exit") == 0) {
        return builtin_exit(argv);
    }

    return 1;
}
