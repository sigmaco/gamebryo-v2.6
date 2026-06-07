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

#ifndef DEPRECATEDIMPORTPLUGIN_H
#define DEPRECATEDIMPORTPLUGIN_H

/// This is our sample import plug-in. It currently has no functionality
/// and is meant to be a template from which customers can create their own
/// import plug-ins.
class DeprecatedImportPlugin : public NiImportPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    DeprecatedImportPlugin(const char* pcName);

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo();

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();

    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiImportPlugin.
    //-----------------------------------------------------------------------
    /// Determines whether or not this plug-in can handle this import plug-in
    /// info object. This function should be overridden by import plug-ins.
    virtual bool CanImport(NiImportPluginInfo* pkInfo);

    /// Execute the NiImportPluginInfo script.
    virtual NiExecutionResultPtr Import(const NiImportPluginInfo* pkInfo);

    /// Gets the string describing the supported file types.
    virtual const char* GetFileDescriptorString();

    /// Gets the string with the supported file type extensions.
    virtual const char* GetFileTypeString();
    
    virtual bool CanCreateDefaultInstance();
};

#endif  // #ifndef DEPRECATEDIMPORTPLUGIN_H
