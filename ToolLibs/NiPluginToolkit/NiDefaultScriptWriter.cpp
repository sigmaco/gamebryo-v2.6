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

#include "NiDefaultScriptWriter.h"
#include "NiScriptInfo.h"

//---------------------------------------------------------------------------
/// Writes the ScriptInfo object into a character string.
NiString NiDefaultScriptWriter::Encode(NiScriptInfo* pkScriptInfo)
{
    if (!pkScriptInfo)
        return NiString();

    NiPluginInfoPtrSet& kPlugins = pkScriptInfo->GetPluginSet();

    // Create a buffer with roughly the correct amount of information
    unsigned int uiSize = 256 + kPlugins.GetSize() * 256;
    NiString strScript((int)uiSize);

    
    /* Format:
    Script("SCRIPT_NAME","TYPE")
    {
        Plugin("PLUGIN_CLASSNAME","PLUGIN_NAME","PLUGIN_VERSION","PLUGIN_TYPE")
        {
            Parameter("PARAM_KEY", "PARAM_VALUE");
            ...
        }
        ...
    }
    */

    // Write the header
    strScript.Concatenate(SCRIPT_HEADER);
    strScript.Concatenate(" ");
    strScript.Concatenate(SCRIPT_VERSION);
    strScript.Concatenate("\n");

    // Open the script block
    strScript.Concatenate("Script (\"");
    strScript.Concatenate(pkScriptInfo->GetName());
    strScript.Concatenate("\",\"");
    strScript.Concatenate(pkScriptInfo->GetType());
    strScript.Concatenate("\")\n");
    strScript.Concatenate("{\n");

    // Write the plugins
    
    for (unsigned int ui = 0; ui < pkScriptInfo->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPluginInfo = pkScriptInfo->GetPluginInfoAt(ui);
        NIASSERT(pkPluginInfo);
        // Plugin("PLUGIN_CLASSNAME","PLUGIN_NAME","PLUGIN_VERSION",
        //     "PLUGIN_TYPE")
        strScript.Concatenate("\tPlugin (");
        strScript.Concatenate("\"");
        strScript.Concatenate(pkPluginInfo->GetClassName());
        strScript.Concatenate("\",");
        strScript.Concatenate("\"");
        strScript.Concatenate(pkPluginInfo->GetName());
        strScript.Concatenate("\",");
        strScript.Concatenate("\"");
        strScript.Concatenate(pkPluginInfo->GetVersion());
        strScript.Concatenate("\",");
        strScript.Concatenate("\"");
        strScript.Concatenate(pkPluginInfo->GetType());
        strScript.Concatenate("\"");
        strScript.Concatenate(")\n");

        // Open parameter block
        strScript.Concatenate("\t{\n");

        for (unsigned int ui = 0; ui < pkPluginInfo->GetParameterCount(); ui++)
        {
            // Parameter("PARAM_KEY", "PARAM_VALUE");
            NiParameterInfo* pkParam = pkPluginInfo->GetParameterAt(ui);
            if (pkParam)
            {
                strScript.Concatenate("\t\tParameter (");
                strScript.Concatenate("\"");
                strScript.Concatenate(pkParam->GetKey());
                strScript.Concatenate("\",\"");
                strScript.Concatenate(pkParam->GetValue());
                strScript.Concatenate("\"");
                strScript.Concatenate(");\n");
            }
        }
        // End parameter block
        strScript.Concatenate("\t}\n");
    }

    // End the script block
    strScript.Concatenate("}\n");

    pkScriptInfo->MarkAsDirty(false);
    return strScript;
}

//---------------------------------------------------------------------------
NiString NiDefaultScriptWriter::GetFileExtensions()
{
    return "*.script";
}
