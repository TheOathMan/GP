#pragma once
#include "AppCore.h"

enum class FileBinAccess {READ,WRITE};

struct FileHandle {

    uchar* file_data;
    FILE* file;
    size_t file_size;

    FileHandle(const char* filename, FileBinAccess Binaccess);

	FILE* FileOpen(const char* filename, const char* mode);
    bool FileClose();
    U64 FileSize();
    U64 FileRead(void* data, U64 sz, U64 count);
    U64 FileWrite(const void* data, U64 sz, U64 count);
    ~FileHandle();
};
