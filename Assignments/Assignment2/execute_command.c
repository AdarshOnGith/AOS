// Author: Adarsh Sharma on 30/08/2024.
#include "header_files.h"
#include <dirent.h>

extern struct termios orgMode;

void updateDisplayDir() {
    int homeDirLen = (int) strlen(homeDir);

    if (strlen(currDir) < (size_t) homeDirLen) {
        strcpy(displayDir, currDir);
        if (displayDir[strlen(displayDir) - 1] == '/') {
            displayDir[strlen(displayDir) - 1] = '\0';
        }
        return;
    }
    for (int i = 0; i < homeDirLen; i++) {
        if (homeDir[i] != currDir[i]) {
            strcpy(displayDir, currDir);
            return;
        }
    }
    strcpy(displayDir, "~/");
    strcat(displayDir, currDir + homeDirLen);
    if (displayDir[strlen(displayDir) - 1] == '/') {
        displayDir[strlen(displayDir) - 1] = '\0';
    }

}

// Function to remove balanced quotes from a string
char *removeQuotes(const char *str) {
    int len = strlen(str);
    char *result = malloc(len + 1); // Allocate memory for result string
    int res_idx = 0;

    bool in_single_quote = false;
    bool in_double_quote = false;

    for (int i = 0; i < len; i++) {
        if (str[i] == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote; // Toggle single quote status
        } else if (str[i] == '\"' && !in_single_quote) {
            in_double_quote = !in_double_quote; // Toggle double quote status
        } else {
            result[res_idx++] = str[i]; // Copy character to result
        }
    }

    result[res_idx] = '\0'; // Null-terminate the result string
    return result;
}

// Function to handle 'echo' command
void echoHandler(char *tokens[], int num) {
    // Step 1: Concatenate tokens into a single string
    int total_length = 0;
    for (int i = 1; i < num; i++) {
        total_length += strlen(tokens[i]) + 1; // +1 for space or null terminator
    }

    char *combined_string = malloc(total_length + 1);
    combined_string[0] = '\0';

    for (int i = 1; i < num; i++) {
        strcat(combined_string, tokens[i]);
        if (i < num - 1) {
            strcat(combined_string, " "); // Add space between tokens
        }
    }

    // Step 2: Remove balanced quotes from the combined string
    char *processed_string = removeQuotes(combined_string);

    // Print the result
    printf("%s\n", processed_string);
    // Free allocated memory
    free(combined_string);
    free(processed_string);
}

bool cehckMultipleArgs(char *token[]) {
if(token[2]== NULL) return true;
printf("Cd Error: Too many Arguments\n");
return false;
}
// handles cd
void cdHandler(char *token[]) {
    char cd_location[BUFF_SIZE];
    if(cehckMultipleArgs(token)) {
        strcpy(cd_location, token[1]);
        char new_address[BUFF_SIZE];
        getRawAddress(new_address, cd_location, currDir, homeDir);
        struct stat stats_dir;

        if (stat(new_address, &stats_dir) == 0 && (S_IFDIR & stats_dir.st_mode)) {
            strcpy(prv_address, currDir);
            if (chdir(new_address) == -1) {
                printf("cd Error: directory does not exist\n");
            }
            if (getcwd(currDir, BUFF_SIZE) == NULL) { // go to new path
                printf("cd Error: getcwd failed\n");
            }
            strcat(currDir, "/");
            updateDisplayDir();
        } else {
            printf("cd Error: directory does not exist: %s\n", token[1]);
        }
    }

}

// pwd handler
void pwdHandler() {
    printf("%s\n", currDir);
}

// searching impl

// Helper function to check if a path starts with a given prefix
bool startsWith(const char *path, const char *prefix) {
    return strncmp(path, prefix, strlen(prefix)) == 0;
}

// Helper function to recursively search for the file or folder
bool searchDirectory(const char *dirPath, const char *target) {
    DIR *dir;
    struct dirent *entry;
    // struct stat entryStat;

    // Open the directory
    if ((dir = opendir(dirPath)) == NULL) {
        perror("opendir");
        return false;
    }

    // Read entries in the directory
    while ((entry = readdir(dir)) != NULL) {
        char path[BUFF_SIZE];
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        // Check if entry is the target file or folder
        if (strcmp(entry->d_name, target) == 0) {
            closedir(dir);
            return true;
        }

        // Recursively search if entry is a directory
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (searchDirectory(path, target)) {
                closedir(dir);
                return true;
            }
        }
    }

    closedir(dir);
    return false;
}

// Main function to handle search command
void handleSearch(char *tokens[], int no, const char *curr_dir, const char *home_dir) {
    if (no < 2) {
        fprintf(stderr, "Usage: search <filename>\n");
        return;
    }

    const char *target = tokens[1];
    char searchBase[BUFF_SIZE];

    // Determine the search base directory
    if (startsWith(curr_dir, home_dir)) {
        snprintf(searchBase, sizeof(searchBase), "%s", curr_dir);
    } else {
        snprintf(searchBase, sizeof(searchBase), "%s", home_dir);
    }

    // Perform the search
    if (searchDirectory(searchBase, target)) {
        printf("True\n");
    } else {
        printf("False\n");
    }
}
// searching impl ends

void processInput(char *input, int bg) {
 char *tokens[1000]; //char *tokens[BUFF_SIZE]; // Array to hold tokens (command arguments)
    int num_tokens = 0; // Number of tokens parsed

    // Tokenize the input based on whitespace characters
    tokens[0] = strtok(input, " \t\n"); // First token

    // break for echo or arrow key maybe?
    while (tokens[num_tokens] != NULL) {
        tokens[++num_tokens] = strtok(NULL, " \t"); // Subsequent tokens
    }

    // If no tokens are found, exit the function
    if (num_tokens == 0)
        return;

    // Handle specific commands based on the first token (command)
    if (strcmp(tokens[0], "cd") == 0) {
        // Handle 'cd' command (change directory)
        if (num_tokens == 1) {
            // If no argument is provided, default to home directory
            tokens[1] = malloc(4);
            strcpy(tokens[1], "~");
        }
        cdHandler(tokens); // Call the 'cd' handler function

    } else if (strcmp(tokens[0], "pwd") == 0) {
        // Handle 'pwd' command (print working directory)
        pwdHandler(); // Call the 'pwd' handler function

    } else if (strcmp(tokens[0], "ls") == 0) {
        // Handle 'ls' command (list directory contents)
        // printf("lsHandler\n"); 
        lsHandler(tokens, num_tokens, currDir, homeDir); // Call the 'ls' handler function

    } else if (strcmp(tokens[0], "echo") == 0) {
        // Handle 'echo' command (print arguments)
        echoHandler(tokens, num_tokens); // Call the 'echo' handler function

    } else if (strcmp(tokens[0], "exit") == 0) {
        // Handle 'exit' command (terminate the shell)
        checkAndKillBgProcess(); // Terminate background processes
        checkResetTerminalMode(&orgMode);
        _exit(EXIT_SUCCESS); // Exit the shell

    } else if (strcmp(tokens[0], "search") == 0) {
        handleSearch(tokens, num_tokens, currDir, homeDir);
    } else if (strcmp(tokens[0], "clear") == 0) {
        // Handle 'clear' command (clear the terminal screen)
        clearScr(); // Call the 'clear' handler function

    } else if (strcmp(tokens[0], "pinfo") == 0) {
        // Handle 'pinfo' command (print process information)
        if (num_tokens == 1) {
            // If no argument is provided, default to current process ID
            tokens[1] = malloc(10);
            sprintf(tokens[1], "%d", getpid());
        }
        // printf("pinfoHandler\n"); 
        pinfo_handler(tokens); // Call the 'pinfo' handler function

    } else if (strcmp(tokens[0], "history") == 0) {
        // Handle 'history' command (show command history)
        if (num_tokens == 1) {
            showHistory(10); // Show last 10 history entries
        } else {
            // Validate and process history argument
            if (strtol(tokens[1], NULL, 10) <= 0 || strtol(tokens[1], NULL, 10) > 20) {
                printf("history <int n> \n n > 0 && n <= 20\n"); // Invalid argument message
                return;
            }
            showHistory(atoi(tokens[1])); // Show specified number of history entries
        }

    } else {
        // Handle unknown commands by creating a new process
        // printf("makeProcess for %s \n", tokens[0]); 
        creatProcess(tokens, num_tokens, bg); // Call the function to create a new process
    }
}
