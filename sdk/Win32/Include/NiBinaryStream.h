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

#ifndef NIBINARYSTREAM_H
#define NIBINARYSTREAM_H
#include "NiSystemLibType.h"
#include "NiEndian.h"
#include "NiMemObject.h"

//---------------------------------------------------------------------------
class NISYSTEM_ENTRY NiBinaryStream : public NiMemObject
{
public:
    NiBinaryStream();
    virtual ~NiBinaryStream();
    virtual operator bool() const = 0;
    virtual void Seek(int iNumBytes) = 0;
    virtual unsigned int GetPosition() const;

    // Endian-aware struct read, returns the number of bytes read.
    inline unsigned int BinaryRead(void* pvBuffer, unsigned int uiTotalBytes,
        unsigned int* puiComponentSizes, unsigned int uiNumComponents = 1);
    // Endian-aware struct write, returns the number of bytes written.
    inline unsigned int BinaryWrite(const void* pvBuffer,
        unsigned int uiTotalBytes, unsigned int* puiComponentSizes,
        unsigned int uiNumComponents = 1);

    // Raw read, no endian swapping, returns number of bytes read
    inline unsigned int Read(void *pvBuffer, unsigned int uiBytes);
    // Raw write, no endian swapping, returns number of bytes written
    inline unsigned int Write(const void *pvBuffer, unsigned int uiBytes);

    // GetLine reads bytes until it reachs a '\n' character, or it reaches
    // the end of the stream, or it reads uiMaxBytes - 1 bytes. It does not
    // copy the '\n' to pvBuffer, but it does terminate pvBuffer with a 0. The
    // return value is the number of bytes read.
    unsigned int GetLine(char* pvBuffer, unsigned int uiMaxBytes);

    // PutS writes a 0-terminated string. The 0 is not written. The return
    // value is the number of bytes written.
    unsigned int PutS(const char* pvBuffer);

    // Set if endianness should be swapped during read/write.
    // Returns true if setting was successful.
    virtual void SetEndianSwap(bool bDoSwap) = 0;

    // Get/set whether only streams that match the current platform's 
    // endianness should be loaded.  This is not enforced by 
    // NiBinaryStream itself, but rather serves as a global hint to classes
    // that handle NiBinaryStreams whether they should load streams that need
    // an endian swap.
    static bool GetEndianMatchHint();
    static void SetEndianMatchHint(bool bForceMatch);

    void WriteCString(const char* pcString);
    char* ReadCString();
protected:
    // Define function pointers. (Mimic "function pointer to a member
    // function" by passing along the "this" pointer.)
    typedef unsigned int (*NIBINARYSTREAM_READFN)(
        NiBinaryStream* pkThis, void* pvBuffer, unsigned int uiBytes, 
        unsigned int* puiComponentSizes, unsigned int uiNumComponents);
    typedef unsigned int (*NIBINARYSTREAM_WRITEFN)(
        NiBinaryStream* pkThis, const void* pvBuffer, unsigned int uiBytes,
        unsigned int* puiComponentSizes, unsigned int uiNumComponents);

    // Convenience function for derived classes
    static void DoByteSwap(void* pvData, unsigned int uiBytes,
        unsigned int* puiComponentSizes, unsigned int uiNumComponents);

    static bool ms_bEndianMatchHint;
    unsigned int m_uiAbsoluteCurrentPos;
    
    NIBINARYSTREAM_READFN m_pfnRead;
    NIBINARYSTREAM_WRITEFN m_pfnWrite;
};

// Include macros for derived classes
#include "NiBinaryStreamMacro.h"

//---------------------------------------------------------------------------
// Inline load/save routines for native data.
//---------------------------------------------------------------------------
template <class T>
void NiStreamLoadBinary(NiBinaryStream& binstream, T& value)
{
    NiBinaryStreamLoad(binstream, &value, 1);
}
//---------------------------------------------------------------------------
template <class T>
void NiStreamLoadBinary(NiBinaryStream& binstream, T* value, 
    unsigned int uiNumEls)
{
    NiBinaryStreamLoad(binstream, value, uiNumEls);
}
//---------------------------------------------------------------------------
template <class T>
void NiStreamSaveBinary(NiBinaryStream& binstream, T& value)
{
    NiBinaryStreamSave(binstream, &value, 1);
}
//---------------------------------------------------------------------------
template <class T>
void NiStreamSaveBinary(NiBinaryStream& binstream, T* value, 
    unsigned int uiNumEls)
{
    NiBinaryStreamSave(binstream, value, uiNumEls);
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryStream::BinaryRead(void* pvBuffer, 
    unsigned int uiTotalBytes, unsigned int* puiComponentSizes, 
    unsigned int uiNumComponents)
{
    unsigned int uiBytesRead = m_pfnRead(this, pvBuffer, uiTotalBytes,
        puiComponentSizes, uiNumComponents);
    m_uiAbsoluteCurrentPos += uiBytesRead;
    return uiBytesRead;
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryStream::BinaryWrite(const void* pvBuffer, 
    unsigned int uiTotalBytes, unsigned int* puiComponentSizes, 
    unsigned int uiNumComponents)
{
    unsigned int uiBytesWritten = m_pfnWrite(this, pvBuffer, uiTotalBytes, 
        puiComponentSizes, uiNumComponents);
    m_uiAbsoluteCurrentPos += uiBytesWritten;
    return uiBytesWritten;
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryStream::Read(void *pvBuffer, 
    unsigned int uiBytes)
{
    unsigned int uiSize = 1;
    unsigned int uiBytesRead = BinaryRead(pvBuffer, uiBytes, &uiSize, 1);
    return uiBytesRead;
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryStream::Write(const void *pvBuffer, 
    unsigned int uiBytes)
{
    unsigned int uiSize = 1;
    unsigned int uiBytesWritten =  BinaryWrite(pvBuffer, uiBytes, &uiSize, 1);
    return uiBytesWritten;
}
//---------------------------------------------------------------------------

#endif // NIBINARYSTREAM_H
