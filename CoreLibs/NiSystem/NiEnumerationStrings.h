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

#ifndef NIENUMERATIONSTRINGS_H
#define NIENUMERATIONSTRINGS_H

#include "NiSystemLibType.h"

/**
    @name Viewer Strings Enumeration Helper Macros

    Assist in generating viewer strings for member variables of enumeration 
    types. 

    Example:
    @code
    const char* NiExample::GetTypeAsString()
    {
        const char* pcStr;
        ENUM_STRING_BEGIN(m_eType, pcStr);
        ENUM_STRING_ITEM(TYPE_A);
        ENUM_STRING_ITEM(TYPE_B);
        ENUM_STRING_ITEM(TYPE_C);
        ENUM_STRING_END(m_eType, pcStr);
        return pcStr;
    }
    @endcode
*/
//@{
#define ENUM_STRING_BEGIN(kEnumVariable, pcOutputString) \
    { \
        const char*& pcOutputStringLocal = pcOutputString; \
        pcOutputStringLocal = ""; \
        switch (kEnumVariable) \
        { 

#define ENUM_STRING_ITEM(kEnumerationValue) \
        case kEnumerationValue: \
            pcOutputStringLocal = #kEnumerationValue; \
            break;

#define ENUM_STRING_END(kEnumVariable, pcOutputString) \
        default: \
            NIASSERT(!"Unknown enumeration value in " #kEnumVariable \
            ". Need to update ENUM_STRING_ITEM() lines." ); \
            break; \
        } \
    }

    /// Alternate end macro, if enum not found string will be NULL
#define ENUM_STRING_END__ALLOW_MISSING_ENUM(kEnumVariable, pcOutputString) \
        default: \
            pcOutputStringLocal = NULL; \
        break; \
        } \
    }
//@}

#endif
