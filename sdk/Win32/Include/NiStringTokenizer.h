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

#ifndef NISTRINGTOKENIZER_H
#define NISTRINGTOKENIZER_H

#include "NiPluginToolkitLibType.h"
#include "NiString.h"

/// A class designed to take an existing NiString and break it into
/// tokens based on an NiString of delimiters.
class NIPLUGINTOOLKIT_ENTRY NiStringTokenizer
{
    public:
        /// Create a NiStringTokenizer for the string pcSource
        NiStringTokenizer(const char* pcSource);

        /// Get the next token from m_strSource, 
        /// using the delimiter string strDelimiters
        NiString GetNextToken(const char* pcDelimiters);

        /// Query if we've reached the end of the string
        bool IsEmpty();

        /// Consume chars, returns the number consumed
        unsigned int Consume(const char* pcChars);

        /// Consume unNumChars worth of chars, returns the number consumed
        unsigned int Consume(unsigned int uiNumChars);

        /// Peek at the next chars
        const char* Peek();

        /// Start from the beginning
        void Restart();

    protected:
        /// The source NiString
        NiString m_strSource;

        /// The current index
        unsigned int m_uiSourceIndex;
};
#endif