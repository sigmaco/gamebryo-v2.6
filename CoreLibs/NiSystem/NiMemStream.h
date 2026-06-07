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

#ifndef NIMEMSTREAM_H
#define NIMEMSTREAM_H

#include "NiSystemLibType.h"
#include "NiRTLib.h"
#include "NiBinaryStream.h"

class NISYSTEM_ENTRY NiMemStream : public NiBinaryStream
{
    NiDeclareDerivedBinaryStream();

public:
    // create a read-only stream.
    NiMemStream(const void* pBuffer, unsigned int uiSize);
    // create a read-write stream
    NiMemStream();
    virtual ~NiMemStream();

    virtual operator bool() const;    
    virtual void Seek(int iNumBytes);

    void* Str();
    void Freeze(bool bFreeze);
    inline unsigned int GetSize() const;

    virtual void SetEndianSwap(bool bDoSwap);

    typedef unsigned int (*STREAMGROWFUNC)(unsigned int uiCurrent,
        unsigned int uiWriteSize, unsigned int uiBytesLeft);
    static STREAMGROWFUNC GetStreamGrowCallback();
    static void SetStreamGrowCallback(STREAMGROWFUNC pfnGrow);

    static unsigned int DefaultGrowFunc(unsigned int uiCurrent,
        unsigned int uiWriteSize, unsigned int uiBytesLeft);

    static unsigned int GetDefaultGrowSizeLimit();
    static void SetDefaultGrowSizeLimit(unsigned int uiLimit);

protected:
    char* m_pBuffer;
    unsigned int m_uiPos;
    unsigned int m_uiEnd;

    unsigned int m_uiAllocSize;
    bool m_bUserMemory;
    bool m_bFreeze;

    // Read or write a chunk of data (no endian swapping)
    unsigned int MemRead(void* pvBuffer, unsigned int uiBytes);
    unsigned int MemWrite(const void* pvBuffer, unsigned int uiBytes);

    static STREAMGROWFUNC ms_pfnGrow;
    static unsigned int ms_uiDefaultGrowSizeLimit;
};

//---------------------------------------------------------------------------
inline unsigned int NiMemStream::GetSize() const
{
    return m_uiEnd;
}
//---------------------------------------------------------------------------

#endif // NIMEMSTREAM_H
