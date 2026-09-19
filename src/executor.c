#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "executor.h"
#include "builtin.h"


/*
 * Redirect input from a file.
 */
static int redirect_input(const char *filename)
{
    int fd;

    if (filename == NULL) {
        return 0;
    }

    fd = open(filename, O_RDONLY);

    if (fd < 0) {
        perror("open input");
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) < 0) {
        perror("dup2 input");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}


/*
 * Redirect output to a file.
 */
static int redirect_output(const char *filename, int append)
{
    int fd;
    int flags;

    if (filename == NULL) {
        return 0;
    }

    if (append) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }

    fd = open(filename, flags, 0644);

    if (fd < 0) {
        perror("open output");
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("dup2 output");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}


/*
 * Execute a single external command.
 */
static pid_t execute_external_command(
    const command_t *command,
    int input_fd,
    int output_fd
)
{
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {

        /*
         * Pipe/input file input.
         */
        if (input_fd != STDIN_FILENO) {

            if (dup2(input_fd, STDIN_FILENO) < 0) {
                perror("dup2 input");
                exit(EXIT_FAILURE);
            }

            close(input_fd);
        }

        /*
         * Pipe/output file output.
         */
        if (output_fd != STDOUT_FILENO) {

            if (dup2(output_fd, STDOUT_FILENO) < 0) {
                perror("dup2 output");
                exit(EXIT_FAILURE);
            }

            close(output_fd);
        }

        /*
         * Input redirection.
         */
        if (command->input_file != NULL) {

            if (redirect_input(command->input_file) < 0) {
                exit(EXIT_FAILURE);
            }
        }

        /*
         * Output redirection.
         */
        if (command->output_file != NULL) {

            if (redirect_output(
                    command->output_file,
                    command->append) < 0) {

                exit(EXIT_FAILURE);
            }
        }

        /*
         * Execute external command.
         */
        execvp(command->argv[0], command->argv);

        /*
         * execvp returns only if execution fails.
         */
        perror(command->argv[0]);

        exit(EXIT_FAILURE);
    }

    return pid;
}


/*
 * Execute a single command.
 */
static int execute_single_command(const command_t *command)
{
    pid_t pid;
    int status;

    if (command == NULL || command->argc == 0) {
        return 0;
    }

    /*
     * Builtins must run in the parent process.
     *
     * This is important for cd because changing
     * directory in a child would not affect the shell.
     */
    if (is_builtin(command->argv[0])) {

        int saved_stdin;
        int saved_stdout;
        int result;

        /*
         * Save shell stdin/stdout.
         */
        saved_stdin = dup(STDIN_FILENO);
        saved_stdout = dup(STDOUT_FILENO);

        if (saved_stdin < 0 || saved_stdout < 0) {
            perror("dup");

            if (saved_stdin >= 0) {
                close(saved_stdin);
            }

            if (saved_stdout >= 0) {
                close(saved_stdout);
            }

            return 1;
        }

        /*
         * Apply input redirection.
         */
        if (command->input_file != NULL) {

            if (redirect_input(command->input_file) < 0) {

                dup2(saved_stdin, STDIN_FILENO);
                dup2(saved_stdout, STDOUT_FILENO);

                close(saved_stdin);
                close(saved_stdout);

                return 1;
            }
        }

        /*
         * Apply output redirection.
         */
        if (command->output_file != NULL) {

            if (redirect_output(
                    command->output_file,
                    command->append) < 0) {

                dup2(saved_stdin, STDIN_FILENO);
                dup2(saved_stdout, STDOUT_FILENO);

                close(saved_stdin);
                close(saved_stdout);

                return 1;
            }
        }

        /*
         * Execute builtin in parent.
         */
        result = execute_builtin(command->argv);

        /*
         * Restore shell stdin/stdout.
         */
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);

        close(saved_stdin);
        close(saved_stdout);

        return result;
    }

    /*
     * Execute external command.
     */
    pid = execute_external_command(
        command,
        STDIN_FILENO,
        STDOUT_FILENO
    );

    if (pid < 0) {
        return 1;
    }

    /*
     * Wait unless command is running in background.
     */
    if (!command->background) {

        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }

    return 0;
}


/*
 * Execute a pipeline.
 *
 * Example:
 *
 *     ls | grep txt
 */
static int execute_pipeline(const command_list_t *commands)
{
    int i;
    int pipe_fd[2];
    int previous_fd = STDIN_FILENO;
    pid_t pids[64];
    int pid_count = 0;
    int status;

    for (i = 0; i < commands->count; i++) {

        const command_t *command = &commands->commands[i];

        /*
         * Create pipe except for last command.
         */
        if (i < commands->count - 1) {

            if (pipe(pipe_fd) < 0) {
                perror("pipe");
                return 1;
            }

        } else {

            pipe_fd[1] = STDOUT_FILENO;
        }

        /*
         * Builtins inside pipelines run in a child.
         */
        if (is_builtin(command->argv[0])) {

            pid_t pid;

            pid = fork();

            if (pid < 0) {
                perror("fork");
                return 1;
            }

            if (pid == 0) {

                /*
                 * Connect previous pipe to stdin.
                 */
                if (previous_fd != STDIN_FILENO) {

                    if (dup2(previous_fd, STDIN_FILENO) < 0) {
                        perror("dup2 input");
                        exit(EXIT_FAILURE);
                    }

                    close(previous_fd);
                }

                /*
                 * Connect stdout to next pipe.
                 */
                if (i < commands->count - 1) {

                    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0) {
                        perror("dup2 output");
                        exit(EXIT_FAILURE);
                    }

                    close(pipe_fd[1]);
                }

                /*
                 * Apply input redirection.
                 */
                if (command->input_file != NULL) {

                    if (redirect_input(command->input_file) < 0) {
                        exit(EXIT_FAILURE);
                    }
                }

                /*
                 * Apply output redirection.
                 */
                if (command->output_file != NULL) {

                    if (redirect_output(
                            command->output_file,
                            command->append) < 0) {

                        exit(EXIT_FAILURE);
                    }
                }

                execute_builtin(command->argv);

                exit(EXIT_SUCCESS);
            }

            pids[pid_count++] = pid;

        } else {

            pid_t pid;

            pid = execute_external_command(
                command,
                previous_fd,
                pipe_fd[1]
            );

            if (pid < 0) {
                return 1;
            }

            pids[pid_count++] = pid;
        }

        /*
         * Close previous pipe input.
         */
        if (previous_fd != STDIN_FILENO) {
            close(previous_fd);
        }

        /*
         * Close current pipe write end.
         */
        if (i < commands->count - 1) {
            close(pipe_fd[1]);
            previous_fd = pipe_fd[0];
        }
    }

    /*
     * Wait for all processes unless background.
     */
    if (!commands->commands[commands->count - 1].background) {

        for (i = 0; i < pid_count; i++) {

            if (waitpid(pids[i], &status, 0) < 0) {
                perror("waitpid");
            }
        }
    }

    return 0;
}


/*
 * Main command-list executor.
 */
int execute_command_list(const command_list_t *commands)
{
    if (commands == NULL || commands->count == 0) {
        return 0;
    }

    /*
     * Single command.
     */
    if (commands->count == 1) {
        return execute_single_command(
            &commands->commands[0]
        );
    }

    /*
     * Multiple commands means pipeline.
     */
    return execute_pipeline(commands);
}
