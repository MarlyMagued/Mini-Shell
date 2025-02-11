# Mini-Shell 

## Overview
This project is a mini shell implementation for the Operating Systems course (CC373) at Alexandria University, Faculty of Engineering. The shell is built using Lex and Yacc to scan and parse commands and supports various shell functionalities such as process creation, file redirection, piping, and background execution.

## Features
### **Phase 1: Understanding the Provided Source Code**
- The shell uses Lex and Yacc for scanning and parsing.
- Provided source files:
  - `examples/` - Contains helpful code snippets.
  - `Commands.cc` & `command.h` - Main C++ files for command handling.
  - `Makefile` - Compilation instructions (no changes required).
  - `shell.l` & `shell.y` - Lex and Yacc configuration files.

### **Phase 2: Execution and Process Management**
- Implements command parsing and execution.
- Supports process creation using `fork()` and execution using `execvp()`.
- Implements process waiting with `waitpid()`.
- Implements file redirection using `dup2()`.
- Supports command pipelines using `pipe()`.
- Handles background execution (`&`).

### **Phase 3: Additional Features**
- **Ctrl+C Handling**: Ignores SIGINT to prevent shell termination.
- **Exit Command**: Implements an internal `exit` command to terminate the shell.
- **Change Directory**: Implements `cd [dir]` to navigate directories.
- **Improved Argument Parsing**: Allows any character except special ones (`|`, `>`, `<`, `&`).
- **Process Log File**: Logs child process terminations using SIGCHLD.

## Installation and Running
### **Build the Shell**
```sh
make
```

### **Run the Shell**
```sh
./shell
```

### **Example Commands**
```sh
ls -al
ls -al > output.txt
ls | grep text
ls | grep command | grep .o > out.txt
httpd &
cd /home/user
exit
```

## Project Architecture
```
├── examples/
├── command.h
├── Commands.cc
├── Makefile
├── shell.l
├── shell.y
└── README.md
```

## References
- Lex & Yacc documentation
- Linux system calls: `fork()`, `execvp()`, `waitpid()`, `dup2()`, `pipe()`, `chdir()`

## Contributors
- **Marly Magued**
