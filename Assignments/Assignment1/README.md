# Assignment1 : C++ File Processing Programs

This repository contains solutions to two questions whicha are related to file processing tasks implemented in C++ using system calls for file handling. These programs are designed to handle large files efficiently by processing them in chunks.

## Question 1: File Reversal Based on Flag

### Description

The program performs the following tasks:

- **Reverses the Contents of a Fileased on Flag Input:**
   - The contents of the specified file are reversed and saved in a new file located within a directory named `Assignment1`.
  - **Flag Input 0:** Reverses the entire content of the file.
  - **Flag Input 1:** Reverses the content of the file up to (but not including) the start index and after the end index, while keeping the content between the start and end indices in its original order.

- **Progress Reporting:**
  - Displays the percentage of the file written to the console, overwriting the progress each time.

- **Efficient Handling of Large Files:**
  - Processes large files in chunks to handle memory usage efficiently.

- **Directory and File Creation:**
  - Creates the `Assignment1` directory with read, write, and execute permissions for the user.
  - Creates the output file named as `"flagValue_InputFileName.txt"`.
 For example, 0_input.txt where input.txt is input file name.


### Steps to Use
**1. Pre-requisite:**

Open Linux or similar Terminal
Use 'cd' command to traverse to the folder 2024201066_A1 so that present working directory is 2024201066_A1, have your input file in same folder/directory i.e. 2024201066_A1 and run below commands.

**2. Compilation:**
```bash
g++ 2024201066_A1_Q1.cpp -o 'Executable File Name'
```
**3. Execution:**

To run the compiled program, use one of the following commands based on the flag value:
```bash
For Flag = 0

./'Executable File Name' <input file_Name>  0  


For Flag = 1

./'Executable File Name' <input file_Name>   1 'start index' 'end index'  
```

**4. Output:**

Assignment1 directory is created and an output file generated is stored in this directory.
Output file will have name-

For Flag = 0

    0_inputFileName.txt

For Flag = 1

    1_inputFileName.txt



## Question 2: File and Directory Permission check with output file Verification for flag 0

### Description

The program performs the following checks :

    Permissions for two files and the directory created in Question 1.
    Whether the content of the new file is the reverse of the original file for flag 0.


### Steps to Use
**1. Pre-requisite:**

Linux or similar Terminal should have present working directory as folder '2024201066_A1', have your input file in same folder i.e. 2024201066_A1 and run below commands. Output file and directory should be created from 2024201066_A1_Q1 executable file.

    <newfile_Name>: Name of the new file created by the program 2024201066_A1_Q1.

    <oldfile_Name>: Name of the original input file.

    <directory_Name>: Name of the directory created by the program  2024201066_A1_Q1 i.e. (Assignment1).

**2. Compilation:**
```bash
g++ 2024201066_A1_Q2.cpp -o 'Executable File Name'
```
**3. Execution:**

To run the compiled program, use one of the following commands based on the flag value:
```bash
./'Executable File Name' <newfile_Name> <oldfile_Name>  <directory_Name>
```

**4. Output:**

    Displays permissions and verifies if the new file is correctly reversed and if file sizes match. 
    
Output is in following order :-

    Directory is created: Yes/No
    Whether file contents are reversed in newfile: Yes/No
    Both Files Sizes are Same : Yes/No

    
    User has read permissions on directory <directory name>: Yes/No
    User has write permission on directory <directory name>: Yes/No
    User has execute permission on directory <directory name>: Yes/No
    Group has read permissions on directory <directory name>: Yes/No
    Group has write permission on directory <directory name>: Yes/No
    Group has execute permission on directory <directory name>: Yes/No
    Others has read permissions on directory <directory name>: Yes/No
    Others has write permission on directory <directory name>: Yes/No
    Others has execute permission on directory <directory name>: Yes/No
    
The last 9 lines is printed for the old file and the new file too. Total 30 lines (3 + 9 + 9 + 9) will be printed.


