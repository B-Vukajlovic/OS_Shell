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
