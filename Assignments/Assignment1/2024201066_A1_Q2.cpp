#include <iostream>
#include <string.h>
#include <algorithm>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

off_t checkAndRead(int filePtr, char* buffer, off_t readSize, int outputFilePtr) { 
  off_t status = read(filePtr, buffer, readSize);
  if(status == -1) {
    cerr << endl << "Error reading to the input file" << strerror(errno) << endl;
    close(filePtr);
    close(outputFilePtr);
    exit(EXIT_FAILURE);
  }
  return status;
}

off_t checkAndSeek(int filePtr, off_t offset, int whence, int outputFilePtr) {
  off_t seekStatus = lseek(filePtr, offset, whence);
  if( seekStatus == -1 ) { 
    string activity = "beginning";
    if(whence == SEEK_END)
      activity = "end";
    else if(whence == SEEK_CUR)
      activity = "current position ";
    cerr << endl << "Error while seek from " + activity + " within the input file" << strerror(errno) << endl;
    close(filePtr);
    close(outputFilePtr);
    exit(EXIT_FAILURE);
  }
  return seekStatus;
}

string compareFiles(string ipFileName, string opFileName) {
  off_t chunkSize = 4096;
  int filePtr = open(ipFileName.data(), O_RDONLY);
  if (filePtr == -1) {
      cerr << endl  << "Error opening the Input file " + ipFileName + "! " << endl; 
      close(filePtr);
      exit(EXIT_FAILURE);
  } 

  int opFilePtr = open(opFileName.data(), O_RDONLY);
  if (opFilePtr == -1) {
      cerr << endl  << "Error opening the Input file " + opFileName + "! " << endl; 
      close(opFilePtr);
      exit(EXIT_FAILURE);
  } 
  vector<char> ipBuffer(chunkSize);
  vector<char> opBuffer(chunkSize);

  off_t opSizeBytesFirst =  lseek(opFilePtr, 0, SEEK_END);

  off_t ipSizeBytes = lseek(filePtr, 0, SEEK_END); 
  off_t ipSizeBytesFirst = lseek(filePtr, 0, SEEK_SET);

  for(off_t ipReadPtr = ipSizeBytesFirst, opReadPtr = opSizeBytesFirst; 
                                    (ipReadPtr <= ipSizeBytes)&& ( opReadPtr >= 0); 
                                            ipReadPtr += chunkSize, opReadPtr -= chunkSize) {
        off_t ipReadSize =  min(chunkSize, static_cast<off_t>((ipSizeBytes - ipReadPtr)));  
        checkAndRead(filePtr, ipBuffer.data(), ipReadSize, opFilePtr);
        checkAndSeek(filePtr, (ipReadPtr + ipReadSize), SEEK_SET, opFilePtr);
        string ipChunk(ipBuffer.data(), ipReadSize);
        
        off_t opReadSize = min(chunkSize, static_cast<off_t>(opReadPtr));
        checkAndSeek(opFilePtr, (opReadPtr- opReadSize), SEEK_SET, filePtr);
        checkAndRead(opFilePtr, opBuffer.data(), opReadSize, filePtr);
        string opChunk(opBuffer.data(), opReadSize);
        reverse(opChunk.begin(), opChunk.end());
        int minSizeChunkLastIndx = min(opChunk.size(),ipChunk.size()) - 1;
        int a = strcmp(ipChunk.data(), opChunk.data());

        if(a) {
            close(filePtr);
            close(opFilePtr);
            return "No";
        }
      }
  return "Yes";
}

void queryAnswers(string directoryName, int printCntrl) {
  struct stat info;
  // Use stat to get the file status
    if (stat(directoryName.c_str(), &info) != 0) {
        cerr << "Error: " << strerror(errno) << endl;
        cout << "Directory is created: " << "No"<< endl;
    } else if (info.st_mode & S_IFDIR) {
        if(printCntrl) {
          // Check user permissions
          cout << "User has read permissionon for directory " + directoryName + ": " << ((info.st_mode & S_IRUSR)? "Yes": "No")<< endl;
          cout << "User has write permission for directory " + directoryName + ": " << ((info.st_mode & S_IWUSR)? "Yes": "No")<< endl;
          cout << "User has execute permission for directory " + directoryName + ": " << ((info.st_mode & S_IXUSR)? "Yes": "No")<< endl;
          // Check group permissions
          cout << "Group has read permission for directory " + directoryName + ": " << ((info.st_mode & S_IRGRP)? "Yes": "No")<< endl;
          cout << "Group has write permission for directory " + directoryName + ": " << ((info.st_mode & S_IWGRP)? "Yes": "No")<< endl;
          cout << "Group has execute permission for directory " + directoryName + ": " << ((info.st_mode & S_IXGRP)? "Yes": "No")<< endl;

          // Check other permissions
          cout << "Others have read permission for " + directoryName + ": " << ((info.st_mode & S_IROTH)? "Yes": "No")<< endl;
          cout << "Others have write permission for " + directoryName + ": " << ((info.st_mode & S_IWOTH)? "Yes": "No")<< endl;
          cout << "Others have execute permission for " + directoryName + ": " << ((info.st_mode & S_IXOTH)? "Yes": "No")<< endl;
        }
        else {
          cout << "Directory is created: " <<  "Yes" << endl;
        }
    } else {
         cout << "Directory is created: " << "No"<< endl;
    }
}

void queryAnswersFile(string filePath, off_t &fileSizeBytes, string fileNamePrint, int printCntrl) {
    struct stat info;
    
    // Using stat to get the file status
    if (stat(filePath.c_str(), &info) != 0) {
      cerr << "Error: " << strerror(errno) << " " << filePath << endl;
    } else {
      if (S_ISDIR(info.st_mode)) {
          cout << filePath << " is a directory." << endl;
      } else if (S_ISREG(info.st_mode)) {
        if(printCntrl) {
          // Check user permissions
          cout << "User has read permission for " + fileNamePrint + ": " << ((info.st_mode & S_IRUSR)? "Yes": "No")<< endl;
          cout << "User has write permission for " + fileNamePrint + ": " << ((info.st_mode & S_IWUSR)? "Yes": "No")<< endl;
          cout << "User has execute permission for " + fileNamePrint + ": " << ((info.st_mode & S_IXUSR)? "Yes": "No")<< endl;
          // Check group permissions
          cout << "Group has read permission for " + fileNamePrint + ": " << ((info.st_mode & S_IRGRP)? "Yes": "No")<< endl;
          cout << "Group has write permission for " + fileNamePrint + ": " << ((info.st_mode & S_IWGRP)? "Yes": "No")<< endl;
          cout << "Group has execute permission for " + fileNamePrint + ": " << ((info.st_mode & S_IXGRP)? "Yes": "No")<< endl;

          // Check other permissions
          cout << "Others have read permission for " + fileNamePrint + ": " << ((info.st_mode & S_IROTH)? "Yes": "No")<< endl;
          cout << "Others have write permission for " + fileNamePrint + ": " << ((info.st_mode & S_IWOTH)? "Yes": "No")<< endl;
          cout << "Others have execute permission for " + fileNamePrint + ": " << ((info.st_mode & S_IXOTH)? "Yes": "No")<< endl;
        }
        else {
          fileSizeBytes = info.st_size;
        }      
      } else {
          cerr << filePath << " is invalid input: Not file or directory." << endl;
      }
    }
}

int main(int arg, char * argv[]) {
//input order: path of  new_file,  old_file,  directoryName

  if(arg < 4 || arg > 4|| !argv[1] || !argv[2] || !argv[3]) {
    cerr << "Invalid Input given : give 3 Inputs" << endl;
  }
  else {
    string dirRelPath = argv[3];
    string inFileRelPath = argv[2];
    string outFileRelPath =  dirRelPath + "/" + argv[1];
    queryAnswers(dirRelPath, 0);
    off_t inFileSizeBytes = -1;
    queryAnswersFile(inFileRelPath, inFileSizeBytes, "oldfile", 0);
    off_t outFileSizeBytes = -1;
    queryAnswersFile(outFileRelPath, outFileSizeBytes, "newfile", 0);
    if(inFileSizeBytes != -1 && inFileSizeBytes == outFileSizeBytes) {
      cout << "Both Files Sizes are Same: " << "Yes" << endl;
      cout << "Whether file contents are reversed in newfile: " <<compareFiles(inFileRelPath, outFileRelPath) << endl;
    }
    else {
      cout << "Both Files Sizes are Same: " << "No" << endl; 
      cout << "Whether file contents are reversed in newfile: No" << endl;
    }  

    cout<<endl;
    cout<<endl;
    queryAnswers(dirRelPath, 1);
    cout<< endl;
    queryAnswersFile(inFileRelPath, inFileSizeBytes, "oldfile", 1);
    cout<< endl;
    queryAnswersFile(outFileRelPath, outFileSizeBytes, "newfile", 1);
    cout<< endl;
  }
  
  return 0;
}

