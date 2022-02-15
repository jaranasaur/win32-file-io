#include <windows.h>

static unsigned int win32GetNumFiles(char *filePath) {
  WIN32_FIND_DATAA findData;
  unsigned int numFiles = 0;
  HANDLE firstFileHandle = FindFirstFileA(filePath, &findData);
  if (firstFileHandle == INVALID_HANDLE_VALUE) return numFiles;
  
  numFiles = 1;
  while(FindNextFileA(firstFileHandle, &findData)) {
    ++numFiles;
  }

  return numFiles;
}


static unsigned int win32LoadFile(char *filePath, void *buffer) {
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


static void win32SaveFile(char *filePath, void *buffer, unsigned int length, int newFile) {
  HANDLE fileHandle;
  if (newFile) {
    fileHandle = CreateFileA(filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  } else {
    fileHandle = CreateFileA(filePath, GENERIC_WRITE, 0, 0, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  }

  if (fileHandle == INVALID_HANDLE_VALUE) {
    // PRINT("file handle was invalid\n");
    return;
  }

  DWORD numBytesWritten;
  if (WriteFile(fileHandle, buffer, length, &numBytesWritten, 0)) {
  }

  CloseHandle(fileHandle);
}


static unsigned int win32GetAllFileNames(
  char *path,                   // the path for the files, can include wild cards i.e. "dir/*.bmp"
  void *buffer                 // buffer to store the file names in
  // unsigned int *usedBufferSpace // this will be set to the amount of buffer space used by the file names
) {
  HANDLE fileHandle;
  WIN32_FIND_DATAA findData;

  char *fileNameBuffer = (char *)buffer;
  unsigned int numFiles = 0;

  fileHandle = FindFirstFileA(path, &findData);
  if (fileHandle == INVALID_HANDLE_VALUE) return 0;

  unsigned int curBufferIndex = 0;
  unsigned int fileNameIndex = 0;
  while (1) {
    fileNameBuffer[curBufferIndex] = findData.cFileName[fileNameIndex];
    ++curBufferIndex;
    if (findData.cFileName[fileNameIndex] == 0) break;
    ++fileNameIndex;
  }
  ++numFiles;

  while(FindNextFileA(fileHandle, &findData)) {
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