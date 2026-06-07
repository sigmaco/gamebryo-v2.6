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

#ifndef NIDEFAULTSCRIPTREADER_H
#define NIDEFAULTSCRIPTREADER_H

#include "NiPluginToolkitLibType.h"
#include "NiScriptReader.h"

class NiScriptInfo;
class NiString;
class NiPluginInfo;
class NiParameterInfo;

/// The parser of ScriptInfo objects from character strings.
/// This interface is the counterpart of the DefaultScriptWriter object.
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
class NIPLUGINTOOLKIT_ENTRY NiDefaultScriptReader : public NiScriptReader
{
    public:
        // Takes an input character script and creates the ScriptInfo object
        // from it.
        virtual NiScriptInfo* Parse(const char* pcScript, NiString& strErrors,
            bool bSilentRunning);

        // Returns the file extensions that this reader can support
        virtual NiString GetFileExtensions();
        
        // Creates an NiPluginInfo of the appropriate class based on the input
        // parameters
        virtual NiPluginInfo* CreateByType(NiString strClassName, 
            NiString strName, NiString strVersion, NiString strType);

private:

    // Converts old scripts to new ones.
    void ConvertScript(NiScriptInfo* pkScriptInfo, bool bSilentRunning);
};

#endif  // #ifndef NISCRIPTREADER_H
