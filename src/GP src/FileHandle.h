#pragma once
#include "AppCore.h"

struct FileHandle {

    void* file_data;
    FILE* file;

    FileHandle(const char* filename, const char* mode, size_t* out_file_size);

	FILE* FileOpen(const char* filename, const char* mode);
    bool FileClose();
    U64 FileSize();
    U64 FileRead(void* data, U64 sz, U64 count);
    U64 FileWrite(const void* data, U64 sz, U64 count);
    ~FileHandle();
};
