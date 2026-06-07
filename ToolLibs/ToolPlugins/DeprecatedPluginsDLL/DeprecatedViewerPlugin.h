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

#ifndef DEPRECATEDVIEWERPLUGIN_H
#define DEPRECATEDVIEWERPLUGIN_H

#include <NiViewerPlugin.h>

class DeprecatedViewerPlugin : public NiViewerPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    DeprecatedViewerPlugin(const char* pcName);

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
    // Required virtual function overrides from NiViewerPlugin.
    //-----------------------------------------------------------------------
    /// Determines whether or not this plug-in can handle this viewer plug-in
    /// info object. This function should be overridden by viewer plug-ins.
    virtual bool CanView(NiViewerPluginInfo* pkInfo);

    /// Execute the NiViewerPluginInfo script.
    virtual NiExecutionResultPtr View(const NiViewerPluginInfo* pkInfo);

    /// Does this plug-in support the specified renderer?
    virtual bool SupportsRenderer(NiSystemDesc::RendererID eRenderer);

    virtual bool CanCreateDefaultInstance();
protected:
};

#endif  // #ifndef DEPRECATEDVIEWERPLUGIN_H
