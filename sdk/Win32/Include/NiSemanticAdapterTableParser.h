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

#ifndef NISEMANTICADAPTERTABLEPARSER_H
#define NISEMANTICADAPTERTABLEPARSER_H

//---------------------------------------------------------------------------

#include "NiMainLibType.h"
#include "NiFixedString.h"
#include "NiTArray.h"
#include "NiSemanticAdapterTable.h"

#include <NiMemObject.h>
#include <NiUniversalTypes.h>

//---------------------------------------------------------------------------

class NIMAIN_ENTRY NiSemanticAdapterTableParser : public NiMemObject
{
public:
    static bool ParseString(const char* pcString,
        NiSemanticAdapterTable& kTable);
private:
    enum SATEntryTokenType
    {
       UNKNOWN,
       INVALID,
       NUMBER,
       COLON,
       SEMICOLON,
       AT_SYMBOL,
       IDENTIFIER,
       END_OF_BUFFER
    };

    /// Input buffer
    const char* m_pcBuffer;
    NiUInt32 m_uiDeclarationCounter;

    /// Start of current token in input buffer
    NiUInt32 m_uiTokenIndex;

    /// Index of first unused character in input buffer
    NiUInt32 m_uiReadIndex;

    /// Type of current token
    SATEntryTokenType m_eTokenType;

    //
    NiSemanticAdapterTableParser(const char* pcString);

    NiUInt32 GetTokenLength() const;
    void ReadTokenAsString(char* pcBuffer, NiUInt32 uiBufferLength) const;

    void ReadAndClassifyToken();
    bool ReadSATEntry(NiSemanticAdapterTable& kTable);

    bool ReadSemantic(NiFixedString& kSemantic, NiUInt32& uiSemanticIndex);
    bool ReadNumber(NiUInt32& uiValue);
    bool ReadIdentifier(NiFixedString& kIdentifier);

    // Returns true if it consumed the token; prints an error if the token was
    // missing and bReportMismatch is true.
    bool ConsumeToken(SATEntryTokenType eType, bool bReportMismatch);
    bool TokenMatches(SATEntryTokenType eType, bool bReportMismatch) const;
};

//---------------------------------------------------------------------------

#endif 
