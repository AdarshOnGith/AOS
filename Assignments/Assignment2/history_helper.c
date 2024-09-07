// Author: Adarsh Sharma on 3/09/2024.

#include "header_files.h"
#include "history_helper.h"

// Define the path to the history file
#define HISTORY_FILE "/tmp/.myshell_history"

// Function to add a command to the history
void addHistory(char tokens[]) {
    // If the command is empty, do nothing
    if (strcmp(tokens, "") == 0) return;

    FILE *f;
    
    // Open the history file in append mode to ensure the file exists
    f = fopen(HISTORY_FILE, "a");
    fclose(f);

    // Open the history file in read mode
    f = fopen(HISTORY_FILE, "r");
    char *lines[100]; // Array to store lines from the history file
    int n = 0;       // Counter for the number of lines

    if (f != NULL) {
        // Allocate memory for the first line
        lines[n] = malloc(BUFF_SIZE);
        size_t s = BUFF_SIZE;

        // Read each line from the history file
        while (getline(&lines[n], &s, f) != -1) {
            // Skip empty lines
            if (strcmp(lines[n], "\n") != 0) {
                // Allocate memory for the next line
                lines[++n] = malloc(BUFF_SIZE);
            }
        }
    }
    fclose(f); // Close the file after reading

    // Open the history file in write mode to overwrite it
    f = fopen(HISTORY_FILE, "w");
    int i;
    // Calculate the starting index for the last 20 lines
    i = ((n - 20) > 0) ? (n - 20) : 0;

    // Write the last 20 commands (or fewer if there are less than 20)
    for (; i < n; i++) {
        fprintf(f, "%s", lines[i]);
    }

    // Prepare the new command with a newline
    char new[BUFF_SIZE];
    sprintf(new, "%s\n", tokens);

    // Check if the new command is different from the last command
    if (i == 0 || strcmp(new, lines[i - 1]) != 0) {
        fprintf(f, "%s", new);
    }
    fclose(f); // Close the file after writing

    // Free allocated memory for lines
    for (i = 0; i <= n; i++) {
        free(lines[i]);
    }
}

// Function to show the last 'no' commands from the history
void showHistory(int no) {
    char *lines[HISTORY_BUFF];
    int n = loadHistory(lines);

    // Calculate the starting index for the last 'no' lines
    int i = ((n - no) > 0) ? (n - no) : 0;

    // Display the last 'no' commands
    for (; i < n; i++) {
        printf("%s\n", lines[i]);
        free(lines[i]); // Free memory after printing
    }
}

int loadHistory(char *lines[HISTORY_BUFF]) {
    FILE *f;
    
    // Open the history file in append mode to ensure the file exists
    f = fopen(HISTORY_FILE, "a");
    fclose(f);

    // Open the history file in read mode
    f = fopen(HISTORY_FILE, "r");
    // char *lines[100]; // Array to store lines from the history file
    int n = 0;       // Counter for the number of lines

    if (f != NULL) {
        // Allocate memory for the first line
        lines[n] = malloc(BUFF_SIZE);
        size_t s = BUFF_SIZE;

        // Read each line from the history file
        while (getline(&lines[n], &s, f) != -1) {
            char *linee = lines[n];
            // size_t len = strlen(linee);
            for (int i = (int) strlen(linee) - 1; i >= 0; i--) {
            if (linee[i] == ' ' || linee[i] == '\t' || linee[i] == '\n') {
                linee[i] = '\0';
            } else {
                break;
            }
    }
            // Skip empty lines
            if (strcmp(lines[n], "\n") != 0) {
                // Allocate memory for the next line
                lines[++n] = malloc(BUFF_SIZE);
            }
        }
    }
    fclose(f); // Close the file after reading
    return n;
}


