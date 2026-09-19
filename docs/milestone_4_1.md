# Milestone 4.1 - Executor and Process Management

## Completed

Milestone 4.1 executor functionality has been implemented and tested.

The shell executor supports:

- `fork()` for creating child processes
- `execvp()` for executing external commands
- `pipe()` for connecting commands
- `dup2()` for input/output file descriptor redirection
- `waitpid()` for waiting for child processes
- Input redirection
- Output redirection
- Append redirection
- Anonymous pipelines
- Built-in commands
- Background execution

## Tested Commands

```text
pwd
ls
date
echo hello
echo hello world | wc -w
ls | wc -l
ls | grep .c
printf "apple\nbanana\napple\n" | grep apple | wc -l
echo hello > test41.txt
cat test41.txt
echo world >> test41.txt
cat test41.txt
echo hello world | wc -w > count.txt
cat count.txt
