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

#ifndef NISCRIPTWRITER_H
#define NISCRIPTWRITER_H

#include "NiPluginToolkitLibType.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiString.h"

class NiScriptInfo;

/// The interface for the encoding of ScriptInfo objects into character
/// strings. This interface is the counterpart of the ScriptReader interface.
class NIPLUGINTOOLKIT_ENTRY NiScriptWriter: public NiRefObject
{
public:
    /// Writes the ScriptInfo object into a character string.
    virtual NiString Encode(NiScriptInfo* pkScriptInfo) = 0;

    /// Returns the file extensions that this writer can support
    virtual NiString GetFileExtensions() = 0;
};

NiSmartPointer(NiScriptWriter);

#endif  //#ifndef NISCRIPTWRITER_H
