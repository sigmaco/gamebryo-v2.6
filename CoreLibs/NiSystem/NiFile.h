// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIFILE_H
#define NIFILE_H

#include "NiRTLib.h"
#include "NiBinaryStream.h"

#if !defined(WIN32) && !defined(_XENON) && !defined (_PS3) && !defined(_WII)
#error One of these macros must be defined: WIN32, _XENON, _PS3, _WII
#endif

#if defined(_PS3)
#include <cell/cell_fs.h>
#endif

class NISYSTEM_ENTRY NiFile : public NiBinaryStream
{
     NiDeclareDerivedBinaryStream();

public:
    typedef enum
    {
        READ_ONLY,
        WRITE_ONLY,
        APPEND_ONLY
    } OpenMode;
    
    NiFile(const char* pcName, OpenMode eMode,
        unsigned int uiBufferSize = 32768);
    virtual ~NiFile();

    // GetFile is used throughout Gamebryo to create NiFile objects. Use 
    // SetFileCreateFunc to override its behavior (to create an instance of 
    // an NiFile-derived class rather than an NiFile).
    static NiFile* GetFile(const char *pcName, OpenMode eMode,
        unsigned int uiBufferSize = 32768);

    typedef NiFile* (*FILECREATEFUNC)(const char *pcName, 
        OpenMode eMode, unsigned int uiBufferSize);

    // Set the file creation function or restore to default
    // if pfnFunc is NULL.
    static void SetFileCreateFunc(FILECREATEFUNC pfnFunc);
    
    // Check if a file exists with permissions defined by eMode.
    static bool Access(const char* pcName, OpenMode eMode);

    // Override the behavior of NiFile::Access in the same way that
    // NiFile::SetFileCreateFunc overrides NiFile::GetFile:
    typedef bool (*FILEACCESSFUNC)(const char* pcName, OpenMode eMode);
    static void SetFileAccessFunc(FILEACCESSFUNC pfnFunc);

    // Create a single directory
    static bool CreateDirectory(const char* pcDirName);
    static bool DirectoryExists(const char* pcDirName);

    // Override the behavior of NiFile::CreateDirectory in the same way that
    // NiFile::SetFileCreateFunc overrides NiFile::GetFile:
    typedef bool (*CREATEDIRFUNC)(const char* pcName);
    static void SetCreateDirectoryFunc(CREATEDIRFUNC pfnFunc);

    // Override the behavior of NiFile::DirectoryExists in the same way that
    // NiFile::SetFileCreateFunc overrides NiFile::GetFile:
    typedef bool (*DIREXISTSFUNC)(const char* pcName);
    static void SetDirectoryExistsFunc(DIREXISTSFUNC pfnFunc);

    // Recursively check to see if all directories exist in the path. If not,
    // create them one at a time.
    static bool CreateDirectoryRecursive(const char* pcFullPath);

    virtual operator bool() const;

    virtual void Seek(int iNumBytes);
    virtual void Seek(int iOffset, int iWhence);
    static const int ms_iSeekSet;
    static const int ms_iSeekCur;
    static const int ms_iSeekEnd;

    virtual unsigned int GetFileSize() const;

    virtual void SetEndianSwap(bool bDoSwap);

#if defined(_XENON)
    enum FileMethod
    {
        FILE_USE_POINTER = 0x00,
        FILE_USE_HANDLE = 0x01,
        FILE_USE_MASK   = 0x0f,
        FILE_USE_DOUBLEBUFFER = 0x80
    };

    static void SetFileMethod(unsigned int uiFileMethod);
    static unsigned int GetFileMethod(void);
#endif

#if defined(_PS3)
    enum NiFileError
    {
        NIFILE_ERROR_OPEN,
        NIFILE_ERROR_CLOSE,
        NIFILE_ERROR_SEEK,
        NIFILE_ERROR_READ,
        NIFILE_ERROR_WRITE,
    };
    typedef void (*PS3_NIFILE_ERROR_HANDLER_FUNC)(const char* pcFilename,
        OpenMode eMode, NiFileError eError, CellFsErrno eResult);
    static void SetErrorCallback(PS3_NIFILE_ERROR_HANDLER_FUNC pfnFunc);
#endif

protected:
    
    NiFile();
    bool Flush();
    unsigned int DiskWrite(const void* pvBuffer, unsigned int uiBytes);

    static FILECREATEFUNC ms_pfnFileCreateFunc;
    static FILEACCESSFUNC ms_pfnFileAccessFunc;
    static CREATEDIRFUNC  ms_pfnCreateDirFunc;
    static DIREXISTSFUNC  ms_pfnDirExistsFunc;

    static NiFile* DefaultFileCreateFunc(const char *pcName,
        OpenMode eMode, unsigned int uiBufferSize);
    static bool DefaultFileAccessFunc(const char *pcName, OpenMode eMode);
    static bool DefaultCreateDirectoryFunc(const char* pcDir);
    static bool DefaultDirectoryExistsFunc(const char* pcDir);

    unsigned int m_uiBufferAllocSize;
    unsigned int m_uiBufferReadSize;
    unsigned int m_uiPos;
#if defined(_XENON)
    unsigned int DiskRead(void* pvBuffer, unsigned int uiBytes);
    void StartDiskRead(void);
    bool CompleteDiskRead(void);

    char* m_pBuffer;
    unsigned int m_uiFileSize;
    unsigned int m_uiAbsolutePos;
    unsigned int m_uiReadingSector;
    LARGE_INTEGER m_liFileSize;

    // Global method. Used when opening a file...
    static unsigned int ms_uiFileMethodFlags;
    // Local method. Once a file is open, the method can't change. However,
    // the global can change, so we have to track which method creation
    // occured with!
    FileMethod m_eFileMethod;
    bool m_bUseDoubleBuffer;
    FILE* m_pFile;
    HANDLE m_hFile;
    static DWORD ms_dwMoveMethods[3][2];

    // Double-buffer members
    enum
    {
        NIFILE_BUFFER_COUNT     = 2
    };
    unsigned int m_uiStreamBuffer;
    unsigned int m_uiReadBuffer;
    unsigned char* m_apBuffers[NIFILE_BUFFER_COUNT];
    unsigned int m_uiDiskReads;

    class OverlappedContext
    {
    public:
        // Overlapped IO doesn't move the file pointer!
        unsigned int m_uiFilePointer;
        unsigned int m_uiBytesExpected;
        bool m_bIOPending;
        OVERLAPPED m_kOverlapped;
    };
    OverlappedContext m_kOLContext;
#elif defined(WIN32)
    unsigned int DiskRead(void* pvBuffer, unsigned int uiBytes);

    char* m_pBuffer;
    FILE* m_pFile;
#elif defined(_PS3)
    unsigned int DiskRead(void* pvBuffer, unsigned int uiBytes);

    char* m_pBuffer;
    int m_uiFileHandle;
    static PS3_NIFILE_ERROR_HANDLER_FUNC ms_pfnErrorCallback;
#elif defined(_WII)
    unsigned int DiskRead(void* pvBuffer, unsigned int uiBytes);

    char* m_pBuffer;
    unsigned int m_uiBufferLocation;
    unsigned int m_uiFileSize;
    struct DVDFileInfo* m_pFile;
#endif
    OpenMode m_eMode;
    bool m_bGood;

    // Read or write a chunk of data from a file (no endian swapping)
    unsigned int FileRead(void* pvBuffer, unsigned int uiBytes);
    unsigned int FileWrite(const void* pvBuffer, unsigned int uiBytes);

};

#endif // NIFILE_H

