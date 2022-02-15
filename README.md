# Simple Win32 File IO Functions
These are some simple functions for doing some basic file IO operations with Win32 in C/C++. Since the Win32 File API can be a bit cumbersome, these functions provide a simple way to do things like read a file, write a file, count files, etc. I have not accounted for all possible errors with these functions and they are intended to be used as a quick-and-dirty solution for small programs. 

The functions are all in the single header file: "win32fileio.h" The documentation for the functions themselves are in the file.

## Examples

### Get the number of files in a directory
    int numFiles = win32GetNumFiles("c:/users/*");

### Read entire file into memory
    char buffer[128] = {0};
    win32LoadFile("./test.txt", (void *)buffer);

### Write a new file to disk
    char message[] = "well hello there, my old friend!";
    win32SaveFile("../message.txt", (void *)message, sizeof(message) - 1, 1);

### Print all file names in a directory
    // allocate memory to store all the file names
    char *allFileNames = (char *)malloc(1024);
    int numFilesRead = win32GetAllFileNames("../*", allFileNames);

    // print each file name
    char *curChar = allFileNames;
    for (int i = 0; i < numFilesRead; ++i) {
      printf("%c", *curChar);
      while (*curChar++) {
        printf("%c", *curChar);
      }
      printf("\n");
    }