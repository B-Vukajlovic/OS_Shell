# Basic Shell Implementation

This C program implements a minimal shell that executes commands, manages processes, and handles built-in commands like `exit`, `cd`, and `pwd`. It also supports:
- **Pipelines** (chaining commands with `|`)
- **Command Sequences** (running multiple commands in sequence)
- **Background Execution** (detaching commands from the terminal)

---

## Features
- **Builtin Commands**: `exit`, `cd`, `pwd`
- **External Commands**: Executed via `fork` and `execvp`
- **Pipelining**: Handles a simple two-command pipeline
- **Process Management**: Waits for processes to finish, can detach processes into the background
- **Error Handling**: Basic checks and error messages for invalid operations

---

## Building
Compile the code along with its dependencies (e.g., `shell.h`, `arena.h`, `front.h`, `parser/ast.h`, etc.):

gcc -o shell shell.c front.c arena.c parser/ast.c -Wall -Wextra -std=c99

*(Adjust source files and paths as needed.)*

---

## Usage
1. **Run the Shell**:

./shell

2. **Enter Commands**: Type a command, optionally with arguments. Built-in commands (`exit`, `cd`, `pwd`) are handled internally; others use external binaries.
3. **Use Pipes**: For example, `ls | grep shell`.
4. **Background Execution**: Detach a process with `&` or via the shell’s built-in background handling (as implemented).

---

## Notes
- The shell uses **fork** to create child processes for commands.
- **Signals** such as `SIGINT` are partially managed within detached background processes.
- Pipelining is currently limited to **two commands**.
