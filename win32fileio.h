#include <windows.h>

/* returns the number of files (excludes directories) in the specified path */
static unsigned int win32GetNumFiles(
  char *filePath // the path to the files to search, can contain wildcards * 
) {
  WIN32_FIND_DATAA findData;
  unsigned int numFiles = 0;
  HANDLE firstFileHandle = FindFirstFileA(filePath, &findData);
  if (firstFileHandle == INVALID_HANDLE_VALUE) return numFiles;
  
  if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
    numFiles = 1;
  }

  while(FindNextFileA(firstFileHandle, &findData)) {
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
    ++numFiles;
  }

  return numFiles;
}

/* reads the entire file contents into the specified buffer
you must make sure there is enough space for the file in the buffer */
static unsigned int win32LoadFile(
  char *filePath, // the full path to the file to load
  void *buffer    // the buffer to read the file into, make sure it's big enough!
) {
  HANDLE fileHandle = CreateFileA(filePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (fileHandle == INVALID_HANDLE_VALUE) {
    // PRINT("file handle to path: '%s' was invalid\n", filePath);
    return -1;
  }

  DWORD fileSize = GetFileSize(fileHandle, 0);
  // this put the file size before the file in the buffer
  // ((int32 *)buffer)[0] = fileSize;
  // buffer = (void *)&(((int32 *)buffer)[1]);

  DWORD numBytesRead;
  if (ReadFile(fileHandle, buffer, fileSize, &numBytesRead, 0)) {
  }

  CloseHandle(fileHandle);
  return numBytesRead;
}


static void win32SaveFile(
  char *filePath,             // the full path to the file to save/create
  void *buffer,               // the data to write to the file
  unsigned int bufferLength,  // the size of the buffer in bytes to write
  int newFile                 // Boolean, 1 if the file should be created new, 0 if the file should overwrite an existing file
) {
  HANDLE fileHandle;
  if (newFile) {
    fileHandle = CreateFileA(filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  } else {
    fileHandle = CreateFileA(filePath, GENERIC_WRITE, 0, 0, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  }

  if (fileHandle == INVALID_HANDLE_VALUE) {
    // TODO handle this
    return;
  }

  DWORD numBytesWritten;
  if (WriteFile(fileHandle, buffer, bufferLength, &numBytesWritten, 0)) {
  }

  CloseHandle(fileHandle);
}


/* this writes all the file names into the specified buffer (from the specified path),
the names are written one after another as null-terminated strings and the number of files is returned*/
static unsigned int win32GetAllFileNames(
  char *filePath,   // the path for the files, can include wild cards i.e. "dir/*.txt"
  void *buffer      // buffer to store the file names in
) {
  HANDLE fileHandle;
  WIN32_FIND_DATAA findData;

  char *fileNameBuffer = (char *)buffer;
  unsigned int numFiles = 0;

  fileHandle = FindFirstFileA(filePath, &findData);
  if (fileHandle == INVALID_HANDLE_VALUE) return 0;

  unsigned int curBufferIndex = 0;
  unsigned int fileNameIndex = 0;

  // if the first file is a directory, just skip 
  if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
    while (1) {
      fileNameBuffer[curBufferIndex] = findData.cFileName[fileNameIndex];
      ++curBufferIndex;
      if (findData.cFileName[fileNameIndex] == 0) break;
      ++fileNameIndex;
    }
    ++numFiles;
  }

  // process each subsequent file, until there are no more
  while(FindNextFileA(fileHandle, &findData)) {
    // skip to the next file if this file is a directory
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

    fileNameIndex = 0;
    while (1) {
      fileNameBuffer[curBufferIndex] = findData.cFileName[fileNameIndex];
      ++curBufferIndex;
      if (findData.cFileName[fileNameIndex] == 0) break;
      ++fileNameIndex;
    }
    ++numFiles;
  }
  // TODO CloseHandle seems to error out probably because the last FindNextFile becomes nothing (cause there're no more files)
  // CloseHandle(fileHandle);
  // *usedBufferSpace = curBufferIndex;
  return numFiles;
}