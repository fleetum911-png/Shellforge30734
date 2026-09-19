#ifndef BUILTIN_H
#define BUILTIN_H

int is_builtin(const char *command);
int execute_builtin(char * const *argv);
int builtin_cd(char * const *argv);
int builtin_pwd(char * const *argv);
int builtin_echo(char * const *argv);
int builtin_exit(char * const *argv);

#endif
