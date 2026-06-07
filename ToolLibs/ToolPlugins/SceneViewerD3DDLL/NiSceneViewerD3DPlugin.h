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

#ifndef NISCENEVIEWERD3DPLUGIN_H
#define NISCENEVIEWERD3DPLUGIN_H

#include <NiViewerPlugin.h>
#include "SceneViewerDLL.h"

class NiSceneViewerD3DPlugin : public NiViewerPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiSceneViewerD3DPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this Viewer plug-in
    /// info object. This function should be overridden by Viewer plug-ins.
    /// @return Whether or not this plug-in can handle the NiViewerPluginInfo
    ///         object.
    virtual bool CanView(NiViewerPluginInfo* pkInfo);

    /// Execute the NiViewerPluginInfo script.
    /// @return The results of this execution.
    virtual NiExecutionResultPtr View(const NiViewerPluginInfo* pkInfo);

    virtual bool SupportsRenderer(NiSystemDesc::RendererID eRenderer);

protected:
    CSceneViewerDll m_kSceneViewerDll;
};

#endif  // #ifndef NISCENEVIEWRPLUGIN_H
