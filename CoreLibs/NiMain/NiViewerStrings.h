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

#ifndef NIVIEWERSTRINGS_H
#define NIVIEWERSTRINGS_H

#include "NiMainLibType.h"
#include "NiTArray.h"

typedef NiTPrimitiveArray<char*>  NiViewerStringsArray;
// Macro for declaring viewer string function
//
// Insert in class declaration (note the start of public scope).  The
// routine GetViewerStrings supports viewers, scene graph printers, and
// other debugging tools that want a readable form of object data.
#define NiDeclareViewerStrings \
    public: \
        virtual void GetViewerStrings (NiViewerStringsArray* pStrings)

// viewer string routines for native data
NIMAIN_ENTRY char* NiGetViewerString (const char* pRttiName);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, bool bValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, char cValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix,
                                      unsigned char ucValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, short sValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix,
                                      unsigned short usValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, int iValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix,
                                      unsigned int uiValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, long lValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix,
                                      unsigned long ulValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, float fValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, double dValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix, void* pValue);
NIMAIN_ENTRY char* NiGetViewerString (const char* pPrefix,
                                      const char* pValue);

#endif
