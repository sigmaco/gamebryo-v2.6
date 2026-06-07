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

#include "NiString.h"
#include "NiStringTokenizer.h"


//---------------------------------------------------------------------------
NiStringTokenizer::NiStringTokenizer(const char* pcSource)
{
    m_strSource = pcSource;
    m_uiSourceIndex = 0;
}
//---------------------------------------------------------------------------
NiString NiStringTokenizer::GetNextToken(const char* pcDelimiters)
{
    int iNewSourceIdx = m_strSource.FindOneOf(pcDelimiters, m_uiSourceIndex);
      
    // Get the next token string
    while ((unsigned int)iNewSourceIdx == m_uiSourceIndex)
    {
        m_uiSourceIndex++;
        iNewSourceIdx = m_strSource.FindOneOf(pcDelimiters, m_uiSourceIndex);
    }

    // We've reached the final token, return it
    if (iNewSourceIdx == -1)
    {
        unsigned int uiLength = m_strSource.Length();
        NiString strReturn = 
            m_strSource.GetSubstring(m_uiSourceIndex, uiLength);
        m_uiSourceIndex = uiLength;
        return strReturn;
    }
    // We have a token in the string, return it
    else
    {
        NiString strReturn = m_strSource.GetSubstring(m_uiSourceIndex, 
            (unsigned int) iNewSourceIdx);
        m_uiSourceIndex = (unsigned int) iNewSourceIdx;
        return strReturn;
    }
}
//---------------------------------------------------------------------------
bool NiStringTokenizer::IsEmpty()
{
    return m_strSource.Length() <= m_uiSourceIndex;
}
//---------------------------------------------------------------------------
unsigned int NiStringTokenizer::Consume(const char* pcChars)
{
    unsigned int start = m_uiSourceIndex;
    int iNewSourceIdx = m_strSource.FindOneOf(pcChars, m_uiSourceIndex);
      
    // Get the next token string
    while ((unsigned int)iNewSourceIdx == m_uiSourceIndex)
    {
        m_uiSourceIndex++;
        iNewSourceIdx = m_strSource.FindOneOf(pcChars, m_uiSourceIndex);
    }

    return m_uiSourceIndex - start;
}
//---------------------------------------------------------------------------
unsigned int NiStringTokenizer::Consume(unsigned int uiNumChars)
{
    m_uiSourceIndex += uiNumChars;
    return uiNumChars;
}
//---------------------------------------------------------------------------
const char* NiStringTokenizer::Peek()
{
    return (const char*) m_strSource + m_uiSourceIndex;
}
//---------------------------------------------------------------------------
void NiStringTokenizer::Restart()
{
    m_uiSourceIndex = 0;
}
