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

#ifndef NIDEFAULTSCRIPTWRITER_H
#define NIDEFAULTSCRIPTWRITER_H

#include "NiPluginToolkitLibType.h"
#include "NiScriptWriter.h"

/// The interface for the encoding of ScriptInfo objects into character
/// strings. This interface is the counterpart of the ScriptReader interface.
///
/// Format:
///    Script("SCRIPT_NAME")
///    {
///        Plugin("PLUGIN_CLASSNAME","PLUGIN_NAME","PLUGIN_VERSION",
///            "PLUGIN_TYPE")
///        {
///            Parameter("PARAM_KEY", "PARAM_VALUE");
///            ...
///        }
///        ...
///    }
class NIPLUGINTOOLKIT_ENTRY NiDefaultScriptWriter : public NiScriptWriter
{
public:
    /// Writes the ScriptInfo object into a character string.
    virtual NiString Encode(NiScriptInfo* pkScriptInfo);

    /// Returns the file extensions that this writer can support
    virtual NiString GetFileExtensions();
};

#endif 
