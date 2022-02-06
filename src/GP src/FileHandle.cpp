#include <Windows.h>
#include <stringapiset.h>
#include <vector>
#include "FileHandle.h"

FileHandle::FileHandle(const char* filename, const char* mode, size_t* out_file_size)
{
    ASSERT("no mode or file name", !(filename || mode) );
    if (out_file_size)
        *out_file_size = 0;

    if ((file = FileOpen(filename, mode)) == NULL) {
        file_data = NULL;
        return; 
    }

    size_t file_size = (size_t)FileSize();
    if (file_size == (size_t)-1)
    {
        FileClose();
        file_data = NULL;
        return;
    }

    file_data = new(std::nothrow) uint8_t[file_size]; // return null of null has found instead of exception.
    if (file_data == NULL)
    {
        FileClose();
        file_data = NULL;
        return;
    }
    if (FileRead(file_data, 1, file_size) != file_size)
    {
        FileClose();
        //IM_FREE(file_data);
        file_data = NULL;
        return;
    }

    if(mode != "wb") FileClose();
    if (out_file_size)
        *out_file_size = file_size;

}

FILE* FileHandle::FileOpen(const char* filename, const char* mode)
{
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS) && !defined(__CYGWIN__) && !defined(__GNUC__)
    // We need a fopen() wrapper because MSVC/Windows fopen doesn't handle UTF-8 filenames.
    // Previously we used ImTextCountCharsFromUtf8/ImTextStrFromUtf8 here but we now need to support ImWchar16 and ImWchar32!
    const int filename_wsize = ::MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    const int mode_wsize = ::MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0);
    std::vector<U16> buf;
    buf.resize(filename_wsize + mode_wsize);
    ::MultiByteToWideChar(CP_UTF8, 0, filename, -1, (wchar_t*)&buf[0], filename_wsize);
    ::MultiByteToWideChar(CP_UTF8, 0, mode, -1, (wchar_t*)&buf[filename_wsize], mode_wsize);

    FILE* peFile = NULL;
    errno_t err = _wfopen_s(&peFile,(const wchar_t*)&buf[0], (const wchar_t*)&buf[filename_wsize]);

    if (err != 0)
    {
        printf("The file %s was not opened", filename);
        peFile = NULL;
    }

    return peFile; 
#else
    return fopen(filename, mode);
#endif
}

// We should in theory be using fseeko()/ftello() with off_t and _fseeki64()/_ftelli64() with __int64, waiting for the PR that does that in a very portable pre-C++11 zero-warnings way.
bool FileHandle::FileClose()
{ 
    return fclose(file) == 0;
}
U64 FileHandle::FileSize()
{
    long off = 0, sz = 0; 
    off = ftell(file);
    bool c1 = off != -1 && !fseek(file, 0, SEEK_END);
    sz = ftell(file);
    bool c2 = sz  != -1 && !fseek(file, off, SEEK_SET);

    return c1 && c2 ? (U64)sz : (U64)-1;
}
U64 FileHandle::FileRead(void* data, U64 sz, U64 count)
{ 
    return fread(data, (size_t)sz, (size_t)count, file); 
}
U64 FileHandle::FileWrite(const void* data, U64 sz, U64 count)
{
    return fwrite(data, (size_t)sz, (size_t)count, file); 
}

FileHandle::~FileHandle()
{
    FileClose();
}
