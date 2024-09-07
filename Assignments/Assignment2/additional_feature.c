// Author: Adarsh Sharma on 3/09/2024.

#include "header_files.h"
#include "additional_feature.h"

// Function to set terminal to non-canonical mode
void checkSetTerminalMode(struct termios *orgMode) {
    if(termOpen == 0) {
        termOpen = 1;
        
        struct termios newt;
    
        // Get terminal attributes
        tcgetattr(STDIN_FILENO, orgMode);
        newt = *orgMode;
        
        // Disable canonical mode and echo
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
}

// Function to reset terminal to original mode
void checkResetTerminalMode(struct termios *orgMode) {
    if(termOpen) {
        tcsetattr(STDIN_FILENO, TCSANOW, orgMode);
        termOpen = 0;
    }
}

// Function to trim trailing white spaces
int trimTrailingWhitespace(char *str, int buflen) {
    if (buflen <= 0) {
        return 0; // No valid length, nothing to trim
    }

    // Ensure we operate within the valid length
    char *end = str + buflen - 1;

    // Move back to the last non-space character
    while (end >= str && isspace((unsigned char)*end)) {
        end--;
    }

    // Null-terminate the string after the last non-space character
    *(end + 1) = '\0';

    // Return the new length of the trimmed buffer
    return (end - str + 1);
}


void tabKeyHandling(char *buffer, int buflen) {
    const int MAXM_RESULTS = 100;
    const char *standard_commands[12] = {
        "ls", "cd", "pwd", "mkdir", "history", "rmdir",
        "touch", "rm", "cp", "mv", "echo",  "exit"
    };
     buffer[buflen] = '\0'; // Null-terminate the buffer if necessary

    // Get the current working directory
    char cwd[BUFF_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd failed for autocomplete");
        return;
    }

    // Open the directory
    DIR *dir = opendir(cwd);
    if (dir == NULL) {
        perror("opendir failed for autocomplete");
        return;
    }

    struct dirent *entry;
    char *matches[MAXM_RESULTS];
    int matchCount = 0;

    // Copy buffer to a modifiable string and tokenize
    char bufferCopy[BUFF_SIZE];
    strncpy(bufferCopy, buffer, buflen);
    bufferCopy[buflen] = '\0'; // Ensure null-termination
    
    buflen = trimTrailingWhitespace(buffer,buflen);

    char *lastToken = NULL;
    char *token = strtok(bufferCopy, " ");
    while (token != NULL) {
        lastToken = token;
        token = strtok(NULL, " ");
    }
    char *prefix = lastToken ? lastToken : buffer; // Use last token or entire buffer if no spaces
    int preflen = strlen(prefix);

    // Match standard commands
    for (int i = 0; i <= 11; i++) {
        if (strncmp(standard_commands[i], prefix, strlen(prefix)) == 0) {
            if (matchCount < MAXM_RESULTS) {
                matches[matchCount] = strdup(standard_commands[i]);
                matchCount++;
            }
        }
    }

    // Read directory entries and match prefix
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, prefix, strlen(prefix)) == 0) {
            if (matchCount < MAXM_RESULTS) {
                matches[matchCount] = strdup(entry->d_name);
                matchCount++;
            }
        }
    }

    closedir(dir);

    // Display the matching results
    if (matchCount > 0) {
        if (matchCount == 1) {
            // Replace the last component in buffer
            // char *beforeLastToken;
            buffer[buflen-preflen] = '\0';

            if(strlen(buffer)) 
                strcat(buffer,matches[0]);
            else
                strcpy(buffer,matches[0]);

                // Rebuild the buffer with the matched result
            clearLine(), movdeCursorToBegining(),  displayShellName();
            printf("%s", buffer);
            fflush(stdout);
            free(matches[0]);
        } else {
            printf("\n");
            for (int i = 0; i < matchCount; i++) {
                printf("%s\n", matches[i]);
                free(matches[i]);
            }
            strcpy(buffer, "");
            displayShellName();
            fflush(stdout);
        }
    }
}

// Function to handle special keys (down arrow, up arrow)
void handArrwKeys(char * escp_seq, char * buffer) {
    if (escp_seq[0] == '\033' && escp_seq[1] == '[') {
        char * his[HISTORY_BUFF];
        int siz = loadHistory(his)-1;       
        switch (escp_seq[2]) {
            case 'A':  // Up arrow
               if(arrcnt < siz) {
                if(downEffect && arrcnt+1 <= 20)
                    arrcnt++;
                upEffect = 1;
                clearLine(), movdeCursorToBegining(),  displayShellName();
                printf("%s", his[siz - arrcnt]);
                fflush(stdout);
                strcpy(buffer, his[siz - arrcnt]);
                arrcnt++;
                downEffect = 0;
               }
               else arrcnt = siz;
                break;
            case 'B':  // Down arrow
                if(upEffect && arrcnt-1 > 0)
                    arrcnt--;
                if (arrcnt > 0) {
                    arrcnt--;
                    downEffect = 1;
                    clearLine(), movdeCursorToBegining(),  displayShellName();
                    printf("%s", his[siz - (arrcnt)]);
                    fflush(stdout);
                    strcpy(buffer, his[siz - arrcnt]);
                    upEffect = 0;
                }
                break;
            default:
                // printf("Unknown escape sequence\n");
                arrcnt = 0;
                break;
        }  
    }
}
