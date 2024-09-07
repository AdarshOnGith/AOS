// Author: Adarsh Sharma on 30/08/2024.

#include "utils.h"
#include "header_files.h"
char prv_address[BUFF_SIZE] = "";

// handles relative and home and absolute addressing
void getRawAddress(char *new_addr, char *cd_loc, const char *curr_dir, const char *home_dir) {
    if (cd_loc[0] == '-') {
        if (strlen(prv_address) == 0) {
        // If prv_address is empty, there is no previous directory to return to.
        printf("No previous directory to return to.\n");
        strcpy(new_addr, curr_dir); // No change, just stay in the current directory
    } else {
         strcpy(new_addr, prv_address);
    }
        return; // Return after handling 'cd -'
    } else if (cd_loc[0] == '/') {
        // Absolute address
        strcpy(new_addr, cd_loc);  // Copy the absolute path
    } else if (cd_loc[0] == '~') {
        // Home directory address
        strcpy(new_addr, home_dir);     // Start with home directory
        strcat(new_addr, cd_loc + 1);  // Concatenate the rest after '~'
    } else {
        // Relative address
        strcpy(new_addr, curr_dir);     // Start with current directory
        // Ensure there is a trailing '/' before concatenating cd_location
        if (new_addr[strlen(new_addr) - 1] != '/')
            strcat(new_addr, "/");
        strcat(new_addr, cd_loc);  // Concatenate the relative path
    }
}

char *rmv_whitespace(char *line) {
    // leading
    int t = 0;
    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
            t++;
        } else {
            break;
        }
    }
    for (int i = 0; i < t; i++) {
        line++;
    }
    // trailing
    for (int i = (int) strlen(line) - 1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
            line[i] = '\0';
        } else {
            break;
        }
    }
    return line;
}

void printColor(char colorInitial) {
    switch (colorInitial) {
        case 'G': // Green
            printf("%s", "\033[1m\033[32m");
            break;
        case 'B': // Blue
            printf("%s", "\033[1m\033[34m");
            break;
        case 'C': // Cyan
            printf("%s", "\033[1m\033[36m");
            break;
        case 'x' : // white
            printf("%s", "\033[97m");
            break;
        case 'Y': // Yellow
            printf("%s", "\033[1m\033[33m");
            break;
         // Light Colors
        case 'g': // Light Green
            printf("%s", "\033[1m\033[92m");
            break;
        case 'b': // Light Blue
            printf("%s", "\033[1m\033[94m");
            break;
        case 'c': // Light Cyan
            printf("%s", "\033[1m\033[96m");
            break;
        // Background Colors
        case 'R': // Background Red
            printf("%s", "\033[41m");
            break;
        case 'M': // Background Magenta
            printf("%s", "\033[45m");
            break;
        case 'W': // Background White
            printf("%s", "\033[47m");
            break;
        // Light Background Colors
        case 'k': // Light Background Black (Gray)
            printf("%s", "\033[100m");
            break;
        case 'r': // Light Background Red
            printf("%s", "\033[101m");
            break;
        case 'm': // Light Background Magenta
            printf("%s", "\033[105m");
            break;
        case 'w': // Light Background White
            printf("%s", "\033[107m");
            break;
        default:
            // printf("Invalid color initial.\n");
            return;
    }
}

void resetColor() {
    printf("%s", "\033[0m");
}


void clearLine() {
    printf("\033[2K"); // Clear entire line
}

void movdeCursorToBegining() {
    printf("\033[0G"); // Move cursor to beginning of line
}

void displayShellName(){
    printColor('G');
    printf("%s", myShellName);
    printColor('B');
    printf("%s>", displayDir);
    resetColor();
}

void clearScr() {
   printf("\e[1;1H\e[2J");
    fflush(stdout); 
}
void printFirstMsg(){

    printColor('W');
    printColor('B');
    printf("%s", "\033[1m");
    printf("*************************************\n");
    printf("*                                   *\n");
    printf("*     YOU ARE IN THE TERMINAL       *\n");
    printf("*         OF POSIX SHELL            *\n");
    printf("*      copyrights: © Adarsh         *\n");
    printf("*                                   *\n");
    printf("*************************************\n");
    resetColor();
}