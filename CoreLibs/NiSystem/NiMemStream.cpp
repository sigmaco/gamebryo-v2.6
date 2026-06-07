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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiSystemPCH.h"

#include "NiRTLib.h"
#include "NiMemStream.h"
#include "NiSystem.h"

NiImplementDerivedBinaryStream(NiMemStream, MemRead, MemWrite);

NiMemStream::STREAMGROWFUNC NiMemStream::ms_pfnGrow = 
    NiMemStream::DefaultGrowFunc;
unsigned int NiMemStream::ms_uiDefaultGrowSizeLimit = 32*1024*1024;

//---------------------------------------------------------------------------
NiMemStream::NiMemStream(const void* pBuffer, unsigned int uiSize)
{
    m_pBuffer = (char *) pBuffer;
    m_uiPos = 0;
    m_uiAllocSize = uiSize;
    m_uiEnd = uiSize;
    m_bUserMemory = true;
    m_bFreeze = false;
    SetEndianSwap(false);
}

//---------------------------------------------------------------------------
NiMemStream::NiMemStream()
{
    m_uiAllocSize = 1024;
    m_pBuffer = NiAlloc(char, m_uiAllocSize);
    NIASSERT(m_pBuffer != NULL);
    m_uiPos = 0;
    m_uiEnd = 0;
    m_bUserMemory = false;
    m_bFreeze = false;
    SetEndianSwap(false);
}

//---------------------------------------------------------------------------
NiMemStream::~NiMemStream()
{
    if (!m_bUserMemory && !m_bFreeze)
    {
        NiFree(m_pBuffer);
    }
}

//---------------------------------------------------------------------------
NiMemStream::operator bool() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiMemStream::MemRead(void* pBuffer, unsigned int uiBytes)
{
    unsigned int uiUnreadBytes;

    uiUnreadBytes = m_uiEnd - m_uiPos;
    if (uiBytes > uiUnreadBytes)
    {
        uiBytes = uiUnreadBytes;
    }
    
    NiMemcpy(pBuffer, &m_pBuffer[m_uiPos], uiBytes);
    m_uiPos += uiBytes;
    return uiBytes;
}

//---------------------------------------------------------------------------
unsigned int NiMemStream::MemWrite(const void* pBuffer, unsigned int uiBytes)
{
    // can't write to a stream created with a user-specified buffer
    if (m_bUserMemory)
        return 0;

    unsigned int uiUnwrittenBytes = m_uiAllocSize - m_uiPos;

    if (uiBytes > uiUnwrittenBytes)
    {
        //Call the growth callback to get the new size of the buffer.
        m_uiAllocSize = ms_pfnGrow(m_uiAllocSize, uiBytes, uiUnwrittenBytes);
        NIASSERT(m_uiAllocSize >= m_uiPos + uiBytes);

        char *pNewBuffer = NiAlloc(char, m_uiAllocSize);
        NIASSERT(pNewBuffer != NULL);

        NiMemcpy(pNewBuffer, m_pBuffer, m_uiEnd);
        NiFree(m_pBuffer);
        m_pBuffer = pNewBuffer;
    }

    NiMemcpy(&m_pBuffer[m_uiPos], pBuffer, uiBytes);
    m_uiPos += uiBytes;
    if (m_uiPos > m_uiEnd)
        m_uiEnd = m_uiPos;

    return uiBytes;
}

//---------------------------------------------------------------------------
void* NiMemStream::Str()
{
    m_bFreeze = true;
    return m_pBuffer;
}

//---------------------------------------------------------------------------
void NiMemStream::Freeze(bool bFreeze)
{
    m_bFreeze = bFreeze;
}
//---------------------------------------------------------------------------
void NiMemStream::Seek(int iNumBytes)
{
    int iValue = (int)m_uiPos + iNumBytes;
    if (iValue >= 0 && (unsigned int)iValue < m_uiAllocSize)
    {
        m_uiPos = iValue;       
        m_uiAbsoluteCurrentPos = m_uiPos;
    }     
}
//---------------------------------------------------------------------------
unsigned int NiMemStream::DefaultGrowFunc(unsigned int uiCurrent, 
    unsigned int uiWriteSize, unsigned int uiBytesLeft)
{
    // Grow the buffer by 2X if that will be large enough. If not, grow it by
    // the number of bytes that we need.
    unsigned int uiNewSize = (uiWriteSize <= uiBytesLeft + uiCurrent) ?
        (2 * uiCurrent) : 
        (uiCurrent + uiWriteSize - uiBytesLeft);

    // Clamp new size to the grow limit to avoid unbounded exponential growth.
    if ((uiNewSize - uiCurrent) > ms_uiDefaultGrowSizeLimit)
    {
        // Make sure the limit will suffice. Otherwise, we must grow by the
        // necessary number of bytes even if it is over the limit.
        if ((uiWriteSize - uiBytesLeft) < ms_uiDefaultGrowSizeLimit)
        {
            uiNewSize = uiCurrent + ms_uiDefaultGrowSizeLimit;
        }
    }

    return uiNewSize;
}
//---------------------------------------------------------------------------
NiMemStream::STREAMGROWFUNC NiMemStream::GetStreamGrowCallback()
{
    return ms_pfnGrow;
}
//---------------------------------------------------------------------------
void NiMemStream::SetStreamGrowCallback(STREAMGROWFUNC pfnGrow)
{
    if (pfnGrow)
        ms_pfnGrow = pfnGrow;
}
//---------------------------------------------------------------------------
unsigned int NiMemStream::GetDefaultGrowSizeLimit()
{
    return ms_uiDefaultGrowSizeLimit;
}
//---------------------------------------------------------------------------
void NiMemStream::SetDefaultGrowSizeLimit(unsigned int uiLimit)
{
    if (uiLimit)
        ms_uiDefaultGrowSizeLimit = uiLimit;
}
//---------------------------------------------------------------------------
