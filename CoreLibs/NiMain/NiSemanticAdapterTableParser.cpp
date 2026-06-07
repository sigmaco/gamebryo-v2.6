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
#include "NiMainPCH.h"

#include "NiSemanticAdapterTableParser.h"
#include "NiMemoryDefines.h"

#include <ctype.h>

//---------------------------------------------------------------------------

#define ERROR_PREFIX "Error in semantic adapter table : "

//---------------------------------------------------------------------------
// Implementation of NiSemanticAdapterTableParser
//---------------------------------------------------------------------------
NiSemanticAdapterTableParser::NiSemanticAdapterTableParser(
    const char* pcString) :
    m_pcBuffer(pcString),
    m_uiDeclarationCounter(1),
    m_uiTokenIndex(0),
    m_uiReadIndex(0),
    m_eTokenType(UNKNOWN)
{
    NIASSERT(pcString);

    ReadAndClassifyToken();
}
//---------------------------------------------------------------------------
NiUInt32 NiSemanticAdapterTableParser::GetTokenLength() const
{
    NIASSERT(m_uiTokenIndex <= m_uiReadIndex);
    return m_uiReadIndex - m_uiTokenIndex;
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTableParser::ReadTokenAsString(char* pcBuffer,
    NiUInt32 uiBufferLength) const
{
    NIASSERT(pcBuffer && (uiBufferLength > 0));

    NiUInt32 uiLength = GetTokenLength();
    if (uiLength == 0)
    {
        pcBuffer[0] = 0;
    }
    else
    {
        NiStrncpy(pcBuffer, uiBufferLength,
            m_pcBuffer + m_uiTokenIndex, uiLength);
    }
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTableParser::ReadSemantic(NiFixedString& kSemantic,
    NiUInt32& uiSemanticIndex)
{
    NiFixedString kIdentifier;
    if (ReadIdentifier(kIdentifier))
    {
        NiUInt32 uiReadIndex = 0;

        // Check for and process a semantic index if present (otherwise use 0)
        if (ConsumeToken(COLON, false))
        {
            // The index is needs to be a valid number
            if (!ReadNumber(uiReadIndex))
                return false;
     
            // Validate the index range
            if (uiReadIndex > 255)
            {
                NILOG(ERROR_PREFIX "In declaration #%u, semantic %s:%u had "
                    "an index greater than 255.\n",
                    m_uiDeclarationCounter,
                    (const char*)kIdentifier,
                    uiReadIndex);
                return false;
            }
        }

        // Both the semantic name and index are valid
        kSemantic = kIdentifier;
        uiSemanticIndex = uiReadIndex;

        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTableParser::ReadNumber(NiUInt32& uiValue)
{
    if (!TokenMatches(NUMBER, true))
        return false;

    // Parse the current token as an unsigned integer
    char acBuffer[16];
    ReadTokenAsString(acBuffer, 16);
    uiValue = (NiUInt32)atoi(acBuffer);

    // Read the next token
    ReadAndClassifyToken();

    return true;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTableParser::ReadIdentifier(NiFixedString& kIdentifier)
{
    if (!TokenMatches(IDENTIFIER, true))
        return false;

    // Create a temporary buffer on the stack to hold the token
    NiUInt32 uiBufferLength = GetTokenLength() + 1;
    char* pcBuffer = NiStackAlloc(char, uiBufferLength);

    // Read the token into the buffer and convert it into a fixed string
    ReadTokenAsString(pcBuffer, uiBufferLength);
    kIdentifier = NiFixedString(pcBuffer);

    // Read the next token
    ReadAndClassifyToken();

    NiStackFree(pcBuffer);
    return true;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTableParser::ConsumeToken(SATEntryTokenType eType,
    bool bReportMismatch)
{
    if (TokenMatches(eType, bReportMismatch))
    {
        ReadAndClassifyToken();
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTableParser::TokenMatches(SATEntryTokenType eType,
    bool bReportMismatch) const
{
    const bool bMatches = m_eTokenType == eType;

    if (!bMatches && bReportMismatch)
    {
        const char* pcExpected = "";
        switch (eType)
        {
        case NUMBER:
            pcExpected = "unsigned integer";
            break;
        case IDENTIFIER:
            pcExpected = "semantic";
            break;
        case COLON:
            pcExpected = ":";
            break;
        case SEMICOLON:
            pcExpected = ";";
            break;
        case AT_SYMBOL:
            pcExpected = "@";
            break;
        case INVALID:
        case UNKNOWN:
        default:
            // If you hit this assert, a token was added and this switch
            // statement is missing a case, or the caller passed in
            // INVALID or UNKNOWN as an expected value, which is illegal.
            NIASSERT(false);
            break;
        }
       
        char acFound[64];
        ReadTokenAsString(acFound, 64);

        NILOG(ERROR_PREFIX "In declaration #%u, expected '%s' but found "
            "'%s'\n.",
            m_uiDeclarationCounter,
            pcExpected,
            (m_eTokenType == END_OF_BUFFER) ? "(end of string)" : acFound);
    }

    return bMatches;
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTableParser::ReadAndClassifyToken()
{
    m_uiTokenIndex = m_uiReadIndex;
    m_eTokenType = UNKNOWN;

    char c = m_pcBuffer[m_uiReadIndex];
    while (c)
    {
        switch (m_eTokenType)
        {
        case UNKNOWN:
            if (isspace(c))
            {
                // Advance the start index of the token (ignore whitespace)
                ++m_uiTokenIndex;
            }
            else if (isdigit(c))
            {
                m_eTokenType = NUMBER;
            }
            else if (isalpha(c) || (c == '_'))
            {
                m_eTokenType = IDENTIFIER;
            }
            else
            {
                switch (c)
                {
                case ':':
                    m_eTokenType = COLON;
                    break;
                case ';':
                    m_eTokenType = SEMICOLON;
                    break;
                case '@':
                    m_eTokenType = AT_SYMBOL;
                    break;
                default:
                    m_eTokenType = INVALID;
                    break;
                }

                // All of these are single-character tokens, so don't
                // wait until the next loop iteration to stop
                ++m_uiReadIndex;
                return;
            }
            break;
        case NUMBER:
            if (!isdigit(c))
                return;
            break;
        case IDENTIFIER:
            if (!isalnum(c) && (c != '_'))
                return;
            break;
        case INVALID:
        default:
            // Should not be possible to reach here
            m_eTokenType = INVALID;
            return;
        }

        // Read the next character
        ++m_uiReadIndex;
        c = m_pcBuffer[m_uiReadIndex];
    }

    if ((c == 0) && (m_eTokenType == UNKNOWN))
    {
        m_eTokenType = END_OF_BUFFER;
    }
}
//---------------------------------------------------------------------------
// Returns true if a SAT entry was added, and false if there was an error
bool NiSemanticAdapterTableParser::ReadSATEntry(
    NiSemanticAdapterTable& kTable)
{
    NiUInt32 uiComponentCount = 0;
    NiFixedString kGenericSemantic;
    NiUInt32 uiGenericIndex = 0;
    NiFixedString kRendererSemantic;
    NiUInt32 uiRendererIndex = 0;
    NiUInt32 uiRendererSemanticSlot = 0;

    // Read the component count
    if (!ReadNumber(uiComponentCount))
       return false;
    if ((uiComponentCount == 0) || (uiComponentCount > 4))
    {
        NILOG(ERROR_PREFIX "In declaration #%u, component count was %u, but "
            "must be between 1 and 4.\n",
            m_uiDeclarationCounter,
            uiComponentCount);
        return false;
    }

    // Read the generic semantic+index pair
    if (!ReadSemantic(kGenericSemantic, uiGenericIndex))
        return false;

    // Is there a renderer semantic?
    if (TokenMatches(IDENTIFIER, false))
    {
        // Read the renderer semantic and index pair
        if (!ReadSemantic(kRendererSemantic, uiRendererIndex))
            return false;

        // Read the slot index if present
        if (ConsumeToken(AT_SYMBOL, false))
        {
            if (!ReadNumber(uiRendererSemanticSlot))
                return false;
            
            if (uiRendererSemanticSlot >= 4)
            {
                NILOG(ERROR_PREFIX "In declaration #%u, slot index was %u, "
                    "but must be between 0 and 3.\n",
                    m_uiDeclarationCounter,
                    uiRendererSemanticSlot);
                return false;
            }
        }
    }
    else
    {
        // Assume the renderer semantic is the same as the generic semantic
        kRendererSemantic = kGenericSemantic;
        uiRendererIndex = uiGenericIndex;
        uiRendererSemanticSlot = 0;
    }

    // Everything parsed OK and had legitimate ranges, add to the table
    NiUInt32 ui = kTable.GetFreeEntry();
    kTable.SetComponentCount(ui, (NiUInt8)uiComponentCount);
    kTable.SetGenericSemantic(ui, kGenericSemantic, (NiUInt8)uiGenericIndex);
    kTable.SetRendererSemantic(ui, kRendererSemantic,
        (NiUInt8)uiRendererIndex);
    kTable.SetSharedRendererSemanticPosition(ui,
        (NiUInt8)uiRendererSemanticSlot);

    ++m_uiDeclarationCounter;
    return true;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTableParser::ParseString(const char* pcString,
    NiSemanticAdapterTable& kTable)
{
    if (pcString == NULL)
        return false;

    NiSemanticAdapterTableParser kParser(pcString);

    // Keep reading table entries until all tokens are gone or an error occurs
    bool bResult = true;
    while (bResult && (kParser.m_eTokenType != END_OF_BUFFER))
    {
        bResult = bResult && kParser.ReadSATEntry(kTable);
        bResult = bResult && kParser.ConsumeToken(SEMICOLON, true);
    }
    return bResult;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
