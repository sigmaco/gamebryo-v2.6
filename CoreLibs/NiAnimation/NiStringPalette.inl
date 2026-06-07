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
inline NiStringPalette::NiStringPalette() : m_pcBuffer(NULL),
    m_uiAllocatedSize(0), m_uiEndOfBuffer(0)
{
}
//---------------------------------------------------------------------------
inline NiStringPalette::NiStringPalette(unsigned int uiAllocatedSize) :
    m_pcBuffer(NULL), m_uiAllocatedSize(uiAllocatedSize), m_uiEndOfBuffer(0)
{
    if (m_uiAllocatedSize > 0)
    {
        m_pcBuffer = NiAlloc(char, m_uiAllocatedSize);
        memset(m_pcBuffer, 0, m_uiAllocatedSize * sizeof(char));
    }
}
//---------------------------------------------------------------------------
inline NiStringPalette::~NiStringPalette()
{
    NiFree(m_pcBuffer);
}
//---------------------------------------------------------------------------
inline char* NiStringPalette::GetBasePointer() const
{
    return m_pcBuffer;
}
//---------------------------------------------------------------------------
inline unsigned int NiStringPalette::AddString(const char* pcString)
{
    NIASSERT(pcString != NULL);

    unsigned int uiStringLocation;
    if (FindString(pcString, uiStringLocation))
    {
        return uiStringLocation;
    }

    size_t uiStringLength = strlen(pcString) + 1;
    if (m_uiEndOfBuffer + uiStringLength > m_uiAllocatedSize)
    {
        ReallocateBuffer();
        NIASSERT(m_uiEndOfBuffer + uiStringLength <= m_uiAllocatedSize);
    }

    uiStringLocation = m_uiEndOfBuffer;
    char* pcStringLocation = m_pcBuffer + uiStringLocation;
    *pcStringLocation = '\0';
    NiStrcpy(pcStringLocation, m_uiAllocatedSize - m_uiEndOfBuffer, pcString);
    m_uiEndOfBuffer = (unsigned int) (m_uiEndOfBuffer + uiStringLength);

    return uiStringLocation;
}
//---------------------------------------------------------------------------
inline bool NiStringPalette::FindString(const char* pcString,
    unsigned int& uiOffset)
{
    NIASSERT(pcString != NULL);

    uiOffset = 0;
    char* pcBufString = m_pcBuffer;
    while (uiOffset < m_uiEndOfBuffer)
    {
        if (strcmp(pcBufString, pcString) == 0)
        {
            return true;
        }
        size_t uiIncrement = strlen(pcBufString) + 1;
        pcBufString += uiIncrement;
        uiOffset = (unsigned int) (uiOffset + uiIncrement);
    }

    return false;
}
//---------------------------------------------------------------------------
inline void NiStringPalette::ReallocateBuffer()
{
    unsigned int uiNewAllocatedSize = m_uiAllocatedSize * 2;
    char* pcNewBuffer = NiAlloc(char, uiNewAllocatedSize);
    unsigned int uiDestSize = uiNewAllocatedSize * sizeof(char);
    memset(pcNewBuffer, 0, uiDestSize);

    NiMemcpy(pcNewBuffer, uiDestSize, m_pcBuffer,
        m_uiAllocatedSize * sizeof(char));
    NiFree(m_pcBuffer);
    m_pcBuffer = pcNewBuffer;
    m_uiAllocatedSize = uiNewAllocatedSize;
}
//---------------------------------------------------------------------------
