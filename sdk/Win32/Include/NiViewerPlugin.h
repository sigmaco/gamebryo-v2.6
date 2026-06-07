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

#ifndef NIVIEWERPLUGIN_H
#define NIVIEWERPLUGIN_H

#include "NiPluginToolkitLibType.h"
#include "NiPlugin.h"

class NiViewerPluginInfo;

/// A file input/output plug-in.
class NIPLUGINTOOLKIT_ENTRY NiViewerPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Constructor takes strings needed for NiPlugin constructor.
    NiViewerPlugin(const char* pcName, const char* pcVersion,
        const char* pcShortDescription, const char* pcLongDescription);

    /// Virtual destructor.
    inline virtual ~NiViewerPlugin(){}

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
    
    /// Determines whether or not this plug-in can handle this Viewer plug-in
    /// info object. This function should be overridden by Viewer plug-ins.
    /// @return Whether or not this plug-in can handle the NiViewerPluginInfo
    ///         object.
    virtual bool CanView(NiViewerPluginInfo* pkInfo) = 0;

    /// Execute the NiViewerPluginInfo script.
    /// @return The results of this execution.
    virtual NiExecutionResultPtr View(const NiViewerPluginInfo* pkInfo) = 0;

    /// Does this viewer plugin support the input renderer?
    virtual bool SupportsRenderer(NiSystemDesc::RendererID eRenderer) = 0;
};

NiSmartPointer(NiViewerPlugin);

#endif  // #ifndef NIVIEWERPLUGIN_H
