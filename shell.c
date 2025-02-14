/*
 * Name: Boris Vukajlovic
 * This program implements a basic shell, which allows users to execute commands
 * and manage processes. It supports built-in commands like 'exit', 'cd', and 'pwd',
 * as well as external commands executed via fork and execvp. The shell can handle
 * pipelines, command sequences, and process detachment for background execution.
 * Additionally, it provides basic error handling and signal management.
 */

#include "shell.h"
#include "arena.h"
#include "front.h"
#include "parser/ast.h"
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

typedef enum {
    CMD_EXIT,
    CMD_CD,
    CMD_PWD,
    CMD_EXTERNAL,
    CMD_UNKNOWN
} CommandType;

// Function to determine the type of command based on its name.
// Input: The command name (cmd).
// Output: The command type (CMD_EXIT, CMD_CD, CMD_PWD, CMD_EXTERNAL, or CMD_UNKNOWN).
CommandType get_command_type(char* cmd) {
    if (strcmp(cmd, "exit") == 0) return CMD_EXIT;
    if (strcmp(cmd, "cd") == 0) return CMD_CD;
    if (strcmp(cmd, "pwd") == 0) return CMD_PWD;
    return CMD_EXTERNAL;
}

// Function to free the memory allocated for the abstract syntax tree.
// Input: Pointer to the abstract syntax tree root (pt).
void my_free_tree(void *pt) {
	free_tree((node_t *)pt);
}

// Function to initialize the shell.
void initialize(void) {

}

// Function to handle shell exit.
void shell_exit(void) {

}

// Function to wait for the completion of a process with the given PID.
// Input: The process ID to wait for (pid).
void wait_for_completion(pid_t pid) {
    int status;
    waitpid(pid, &status, 0);
}

// Function to execute an external command in a child process.
// Input: The command to execute (cmd), and its parameters (params).
void execute_external_command(char *cmd, char **params) {
    pid_t process_id = fork();
    if (process_id == -1) {
        perror("Error forking process");
        return;
    }

    if (process_id == 0) {
        if (execvp(cmd, params) == -1) {
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
    } else {
        wait_for_completion(process_id);
        fflush(stdout);
    }
}

// Function to execute a command node.
// Input: The command node to execute (current).
void execute_command(node_t *current) {
    char *cmd = current->command.program;
    char **params = current->command.argv;
    int arg_count = current->command.argc;

    CommandType type = get_command_type(cmd);

    switch (type) {
        case CMD_EXIT:
            if (arg_count > 1) {
                exit(atoi(params[1]));
            } else {
                exit(EXIT_SUCCESS);
            }
            break;

        case CMD_CD:
            if (arg_count > 1) {
                if (chdir(params[1]) != 0) {
                    perror("Failed to change directory");
                }
            } else {
                fprintf(stderr, "Error: 'cd' requires a directory path\n");
            }
            break;

        case CMD_PWD:
            {
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("%s\n", cwd);
                } else {
                    perror("Failed to get current working directory");
                }
            }
            break;

        case CMD_EXTERNAL:
            execute_external_command(cmd, params);
            break;

        default:
            fprintf(stderr, "Unknown command\n");
    }
    fflush(stdout);
}

// Function to execute a pipeline of commands.
// Input: The pipeline node to execute (node).
void execute_pipeline(node_t *node) {
    if (node->pipe.n_parts != 2) {
        fprintf(stderr, "Error: Pipeline must contain exactly two commands.\n");
        exit(EXIT_FAILURE);
    }

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Failed to create pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);

        execute_command(node->pipe.parts[0]);

        exit(EXIT_SUCCESS);
    }

    pid = fork();
    if (pid == -1) {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);

        execute_command(node->pipe.parts[1]);

        exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    while (wait(NULL) > 0);
}

// Function to detach a child process, redirecting its output to /dev/null.
// Input: The node representing the command to detach (node).
void detach_process(node_t *node) {
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
            perror("signal");
            exit(EXIT_FAILURE);
        }

        int dev_null_fd = open("/dev/null", O_WRONLY);
        if (dev_null_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        if (dup2(dev_null_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(dev_null_fd);

        execvp(node->command.program, node->command.argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        printf("Started background job %d\n", child_pid);
    }
}

// Function to execute a command node or a sequence of nodes recursively.
// Input: The root node of the command(s) to execute (node).
void run_command(node_t *node) {

    arena_push();

    switch (node->type) {
        case NODE_COMMAND:
            execute_command(node);
            break;
        case NODE_PIPE:
            execute_pipeline(node);
            break;
        case NODE_SEQUENCE:
            run_command(node->sequence.first);
            run_command(node->sequence.second);
            break;
        case NODE_DETACH:
            detach_process(node->detach.child);
            break;
        case NODE_REDIRECT:
            break;
        default:
            fprintf(stderr, "Unsupported node type\n");
            break;
    }
    arena_pop();
}
