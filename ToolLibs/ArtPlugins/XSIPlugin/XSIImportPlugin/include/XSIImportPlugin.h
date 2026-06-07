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

#ifndef XSI_IMPORT_PLUGIN_H
#define XSI_IMPORT_PLUGIN_H

#include "NiPluginToolkit.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // XSI import plugin for the Gamebryo plugin toolkit.
    //---------------------------------------------------------------------------

    class XSIImportPlugin : public NiImportPlugin
    {
    public:
        /// RTTI declaration macro.
        NiDeclareRTTI;

        /// Default constructor.
        XSIImportPlugin();

        //-----------------------------------------------------------------------
        // Required virtual function overrides from NiPlugin.
        //-----------------------------------------------------------------------
        /// Returns a plugin script with default parameters.
        virtual NiPluginInfo* GetDefaultPluginInfo();

        /// Does this plugin have editable options?
        virtual bool HasManagementDialog();

        /// Bring up the options dialog and handle user interaction.
        virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
            NiWindowRef hWndParent);

        //-----------------------------------------------------------------------
        // Required virtual function overrides from NiImportPlugin.
        //-----------------------------------------------------------------------
        /// Determines whether or not this plugin can handle this import plugin
        /// info object. This function should be overridden by import plugins.
        virtual bool CanImport(NiImportPluginInfo* pkInfo);

        /// Execute the NiImportPluginInfo script.
        virtual NiExecutionResultPtr Import(const NiImportPluginInfo* pkInfo);

        /// Gets the string describing the supported file types.
        virtual const char* GetFileDescriptorString();

        /// Gets the string with the supported file type extensions.
        virtual const char* GetFileTypeString();
    };
}

#endif /* XSI_IMPORT_PLUGIN_H */
