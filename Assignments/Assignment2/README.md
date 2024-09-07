# POSIX Shell Implementation

## Overview

This project implements a POSIX-compliant shell program written in C. The shell supports a variety of commands and features including process management, built-in commands, I/O redirection, Pipe and history tracking. The shell can handle both foreground and background processes, manage user input, and execute system commands.


It has modules for the shell each module handles particular functionality of shell, Make file will help you to build the modules and generate the executable files.

### Steps To Run 
- Open a linux like terminal
- Navigate to directory `2024201066_Assignment2` using cd
- Now to build the program run make command in terminal as below

    ```bash
    make
    ```
- now one build is successfull run below command to execute the shell
    ```bash
    ./mypshell
    ```
- to clean the object file below command can be used
    ```bash
    make clean
    ```


## Features

- **Interactive Shell Prompt**: Displays the prompt in the format `<username>@<system_name>:<current_directory>`. The shell dynamically updates to reflect the user's current directory and supports basic command-line navigation. The Directory where shell is executed is considered the Root directory of shell (pseudo-root).

- **Built-in Commands**:
  - `cd`: Change the current directory. Supports flags such as `.` (current directory), `..` (parent directory), `-` (previous directory), and `~` (home directory), It won't take more than one argument otherwise it gives error: `too many arguments`.
  - `echo`: Prints the provided string to the terminal. Handles spaces and tabs, but it cannot preserve the spaces inside quotes.
  - multiple commands can be given separated by delimitter `;`
  - `pwd`: Displays the current working directory.

- **System Commands**:
  - `ls`: Lists directory contents. Supports flags `-a` (all files) and `-l` (long format). Handles various combinations of flags and directory arguments.
  - `pinfo`: Displays information about the shell process or a specified PID. Shows process ID, status, memory usage, and executable path.
  - `search`: Recursively searches for files or directories in the current directory and reports their existence.

- **Background and Foreground Processes**:
  - Commands followed by `&` are executed in the background, and their process ID is displayed.
  - Foreground commands are executed in the shell’s current context, and the shell waits for their completion.

- **I/O Redirection**:
  - Supports input redirection (`<`), output redirection (`>`), and output appending (`>>`).
  - Handles cases where files are created or overwritten as needed.

- **Signal Handling**:
  - `CTRL-Z`: Moves the currently running foreground job to the background.
  - `CTRL-C`: Interrupts the currently running foreground job.
  - `CTRL-D`: Logs out of the shell.

- **Autocomplete**:
  - TAB key completion for commands, files, and directories.

- **Arrow Keys**
  - UP Arrow: moves and shows from most recent commands to last 20th command used, doesn't go above 20th command.
  - DOWN Arrow: moves back towards most recent command used. doesn't go below most recent command.
- **History Management**:
  - history files is hidden and created in `"/tmp/.myshell_history"`
  - `history`: Displays the last 10 commands. Maintains a history of up to 20 commands.
  - `history <num>`: Displays the latest `<num>` commands.
  - Up Arrow Key: Navigates through previous commands in history.
  - If num>20 or num<0 given to history then it prints the required input range for history.
- **Pipe**
  - It takes more than one command separated by | so it uses output of one command as input two other command.
  - you should choose commands that takes input similar to what other command generates as output. 


