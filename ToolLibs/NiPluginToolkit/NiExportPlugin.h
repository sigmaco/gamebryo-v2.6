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

#ifndef NIEXPORTPLUGIN_H
#define NIEXPORTPLUGIN_H

#include "NiPluginToolkitLibType.h"
#include "NiPlugin.h"

class NiExportPluginInfo;

/// A file input/output plug-in.
class NIPLUGINTOOLKIT_ENTRY NiExportPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Constructor takes strings needed for NiPlugin constructor.
    NiExportPlugin(const char* pcName, const char* pcVersion,
        const char* pcShortDescription, const char* pcLongDescription);

    /// Virtual destructor.
    inline virtual ~NiExportPlugin(){}

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object. This is useful for doing versioning, as it allows plug-ins
    /// to take responsiblity for executing an NiPluginInfo object
    /// arbitrarily.
    /// @return Whether or not this plug-in can handle the NiPluginInfo
    ///         object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);

    /// Execute the NiPluginInfo script.
    /// @return The results of this execution.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    
    /// Determines whether or not this plug-in can handle this export plug-in
    /// info object. This function should be overridden by export plug-ins.
    /// @return Whether or not this plug-in can handle the NiExportPluginInfo
    ///         object.
    virtual bool CanExport(NiExportPluginInfo* pkInfo) = 0;

    /// Execute the NiExportPluginInfo script.
    /// @return The results of this execution.
    virtual NiExecutionResultPtr Export(const NiExportPluginInfo* pkInfo) = 0;

    /// Gets the string describing the supported file types.
    virtual const char* GetFileDescriptorString() = 0;

    /// Gets the string with the supported file type extensions.
    virtual const char* GetFileTypeString() = 0;

    /// Determine whether or not this is the primary export plugin for this
    /// file type.
    virtual bool IsPrimaryExportPlugin();

};

NiSmartPointer(NiExportPlugin);

#endif  // #ifndef NIEXPORTPLUGIN_H
