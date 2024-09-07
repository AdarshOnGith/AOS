// Author: Adarsh Sharma on 30/08/2024.

#include "header_files.h"
#include "parse_commands.h"
#include <fcntl.h>  
#include <ctype.h>
#include <stdbool.h>

// Function to handle command processing including redirection and background execution
void processInputs(char *command, bool bg, bool hasRedirection) {
    // Prepare command arguments
    char *args[BUFF_SIZE];
    char *token = strtok(command, " ");
    int argCount = 0;

    // Tokenize the command into arguments
    while (token != NULL) {
        args[argCount++] = token;
        token = strtok(NULL, " ");
    }
    args[argCount] = NULL; // NULL-terminate the argument list

    // Fork and execute the command
    pid_t pid = fork();
    if (pid == -1) {
        perror("Redirection Error: fork failed");
        fflush(stdout);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process

        // Handle redirection
        if (hasRedirection) {
            // Find and handle redirection operators
            char *inputFile = NULL;
            char *outputFile = NULL;
            bool append = false;
            
            // Check for redirection operators
            for (int i = 0; i < argCount; i++) {
                if (strcmp(args[i], "<") == 0) {
                    inputFile = args[i + 1];
                    args[i] = NULL; // End the arguments here
                } else if (strcmp(args[i], ">") == 0) {
                    outputFile = args[i + 1];
                    args[i] = NULL; // End the arguments here
                } else if (strcmp(args[i], ">>") == 0) {
                    outputFile = args[i + 1];
                    append = true;
                    args[i] = NULL; // End the arguments here
                }
            }
            // Redirect input if necessary
            if (inputFile) {
                int inputFd = open(inputFile, O_RDONLY);
                if (inputFd < 0) {
                    perror("Redirection Error: Failed to open input file");
                    exit(EXIT_FAILURE);
                }
                dup2(inputFd, STDIN_FILENO);
                close(inputFd);
            }

            // Redirect output if necessary
            if (outputFile) {
                int outputFd = open(outputFile, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
                if (outputFd < 0) {
                    perror("Redirection Error: open output file");
                    exit(EXIT_FAILURE);
                }
                dup2(outputFd, STDOUT_FILENO);
                close(outputFd);
            }
        }

        // Execute the command
        execvp(args[0], args);
        perror("Redirection Error: execvp failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        addJob(pid, args[0], 1);
        foregroundPid = pid; // Set as the current foreground job
        // If background execution is not requested, wait for the child process
        if (!bg) {
            waitpid(pid, NULL, 0);
        }
    }
}


// Function to handle piped commands
void pipeHandler(char *line) {
    char *commands[100]; // Array to hold commands separated by pipes
    int commandCount = 0;

    // Tokenize based on pipe symbol '|'
    commands[commandCount] = strtok(line, "|");
    while (commands[commandCount] != NULL) {
        commandCount++;
        commands[commandCount] = strtok(NULL, "|");
    }

    int pipefds[2];
    int prevPipeFD = -1; // Used to keep track of previous pipe's read end

    for (int i = 0; i < commandCount; i++) {
        if (i < commandCount - 1) {
            // Create a pipe only if there is another command after the current one
            if (pipe(pipefds) == -1) {
                perror("Pipe Error: file may not be present");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        }
        
        pid_t pid = fork();
        if (pid == -1) {
            perror("Pipe Errro: fork failed");
            fflush(stdout);
            exit(EXIT_FAILURE);
        } 
        if (pid == 0) {
            // Child process
            if (i > 0) {
                dup2(prevPipeFD, STDIN_FILENO); // Redirect stdin from previous pipe
            }
            if (i < commandCount - 1) {
                dup2(pipefds[1], STDOUT_FILENO); // Redirect stdout to current pipe
            }

            // Close all pipe file descriptors
            close(pipefds[0]);
            close(pipefds[1]);
            if (prevPipeFD != -1) {
                close(prevPipeFD);
            }
            // Process the command
            char *cmd = commands[i];
            bool bg = false;
            if (strchr(cmd, '&')) {
                bg = true;
                strtok(cmd, "&"); // Remove the '&' character
            }

            // Check for redirection operators
            bool hasRedirection = false;
            if (strstr(cmd, "<") || strstr(cmd, ">>") || strstr(cmd, ">")) {
                hasRedirection = true;
            }
            // Execute command
            if (hasRedirection) {
                char *outFile = strstr(cmd, ">") ? strstr(cmd, ">") + 1 : NULL;
                if (outFile) {
                    // Remove any leading whitespace
                    while (*outFile == ' ') outFile++;
                    int fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("Error opening file"); // Report error if file can't be opened
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                processInputs(cmd, bg, true);
            } else {
                processInput(cmd, bg);
            }

            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            addJob(pid, commands[i], 1); // Add background job

            if (prevPipeFD != -1) {
                close(prevPipeFD); // Close the previous pipe's read end in the parent
            }
            if (i < commandCount - 1) {
                prevPipeFD = pipefds[0]; // Save the current pipe's read end for the next iteration
                close(pipefds[1]); // Close the current pipe's write end in the parent
            }
        }
    }

    // Wait for all child processes to complete
    while (wait(NULL) > 0);
}

void commandParser(char *line) {
    char *command; // Pointer to hold each tokenized command
    char line2[BUFF_SIZE], line3[BUFF_SIZE]; // Buffers to hold copies of the input line

    // Make copies of the input line for processing
    strcpy(line2, line);
    strcpy(line3, line);

    // Create a copy of the input line for checking the command
    char line4[BUFF_SIZE];
    strcpy(line4, line);

    // Check if the first token is 'echo'
    char *firstToken = strtok(line4, " ");
    bool isEcho = (firstToken && strcmp(firstToken, "echo") == 0);

        // Tokenize the input line based on ';' and '&' for non-echo commands
        command = strtok(line, ";&");
        int commandCount = 0;

        // Count the number of commands
        while (command != NULL) {
            commandCount++;
            command = strtok(NULL, ";&");
        }

        // Array to hold the pointers to each command
        char *commands[commandCount + 1];
        int i = 0;

        // Set the beginning of the command extraction process
        commands[0] = strtok(line2, ";&");

        // Continue tokenizing and storing commands
        while (commands[i] != NULL && strcmp(commands[i], "") != 0) {
            i++;
            commands[i] = strtok(NULL, ";&");
        }
    
        // Process each command
        for (int j = 0; j < commandCount; j++) {
            bool bg = false;
            
            // Check if the commands(commands[i]) contains a pipe
            if (strchr(commands[j], '|') != NULL) {
                pipeHandler(commands[j]);
                continue; // Skip the rest of the loop iteration for this command
            }

            // Check if the command ends with '&' indicating background execution
            if (line3[strlen(commands[j]) + (commands[j] - line2)] == '&') {
                bg = true;
            }

            // Check for redirection operators
            bool hasRedirection = false;
            if (strstr(commands[j], "<") || strstr(commands[j], ">>") || strstr(commands[j], ">")) {
                hasRedirection = true;
            }

            // Process the command with redirection and background info
            if (hasRedirection) {
                processInputs(commands[j], bg, true);
            } else {
                processInput(commands[j], bg);
            }
        }
}


/*
void commandParser(char *line) {
    char *command; // Pointer to hold each tokenized command
    char line2[BUFF_SIZE], line3[BUFF_SIZE]; // Buffers to hold copies of the input line

    // Make copies of the input line for processing
    strcpy(line2, line);
    strcpy(line3, line);

    // Create a copy of the input line for checking the command
    char line4[BUFF_SIZE];
    strcpy(line4, line);

    // Check if the first token is 'echo'
    char *firstToken = strtok(line4, " ");
    bool isEcho = (firstToken && strcmp(firstToken, "echo") == 0);

    // if (!isEcho) {
        // Tokenize the input line based on ';' and '&' for non-echo commands
        command = strtok(line, ";&");
        int commandCount = 0;

        // Count the number of commands
        while (command != NULL) {
            commandCount++;
            command = strtok(NULL, ";&");
        }

        // Array to hold the pointers to each command
        char *commands[commandCount + 1];
        int i = 0;

        // Set the beginning of the command extraction process
        commands[0] = strtok(line2, ";&");

        // Continue tokenizing and storing commands
        while (commands[i] != NULL && strcmp(commands[i], "") != 0) {
            i++;
            commands[i] = strtok(NULL, ";&");
        }
    
        // Process each command
        for (int j = 0; j < commandCount; j++) {
            bool bg = false;

            // Check if the command ends with '&' indicating background execution
            if (line3[strlen(commands[j]) + (commands[j] - line2)] == '&') {
                bg = true;
            }

            // Check for redirection operators
            bool hasRedirection = false;
            if (strstr(commands[j], "<") || strstr(commands[j], ">>") || strstr(commands[j], ">")) {
                hasRedirection = true;
            }

            // Process the command with redirection and background info
            if(hasRedirection)
                processInputs(commands[j], bg, true);
            // Process the command
            else
                processInput(commands[j], bg);
        }
    // } 
    // else {
    //     // Handle 'echo' command separately
    //     bool inQuotes = false;  // Track if we are inside quotes
    //     char finalBuffer[BUFF_SIZE] = {0};
    //     int index = 0;
    //     char *start = line2;

    //     // Process the line character by character
    //     while (*start) {
    //         if (*start == '"' || *start == '\'') {
    //             // Toggle the inQuotes flag when encountering a quote
    //             inQuotes = !inQuotes;
    //             finalBuffer[index++] = *start;
    //         } else if (!inQuotes && *start == ';') {
    //             // Replace ';' with newline only if not inside quotes
    //             finalBuffer[index++] = '\n';
    //         } else {
    //             finalBuffer[index++] = *start;
    //         }
    //         start++;
    //     }
    //     finalBuffer[index] = '\0'; // Null-terminate the final buffer

    //     // Tokenize the finalBuffer based on newline characters
    //     char *commands[BUFF_SIZE];
    //     int num_commands = 0;

    //     command = strtok(finalBuffer, "\n");
    //     while (command != NULL) {
    //         // // Remove extra spaces from the start and end of each command, outside quotes
    //         // char *end = command + strlen(command) - 1;
            
    //         // // Trim trailing spaces if outside of quotes
    //         // while (end > command && isspace(*end) && (end[-1] != '"' && end[-1] != '\'')) end--;
            
    //         // Null-terminate the trimmed command
    //         // *(end + 1) = '\0';
            
    //         // // Trim leading spaces if outside of quotes
    //         // while (isspace(*command) && (*command != '"' && *command != '\'')) command++;
            
    //         // Add command to the array if it's not empty
    //         if (*command) {
    //             commands[num_commands++] = command;
    //         }
    //         command = strtok(NULL, "\n");
    //     }

    //     // Process each command
    //     for (int j = 0; j < num_commands; ++j) {
    //         bool bg = false;

    //         // Check if the command ends with '&' indicating background execution
    //         size_t len = strlen(commands[j]);
    //         if (len > 0 && commands[j][len - 1] == '&') {
    //             bg = true;
    //             commands[j][len - 1] = '\0'; // Remove the '&' character
    //         }

    //         // Process the command
    //         processInput(commands[j], bg);
    //     }
    // }
} 
*/