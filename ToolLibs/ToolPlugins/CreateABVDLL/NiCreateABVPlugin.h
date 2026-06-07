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

#ifndef NICREATEABVPLUGIN_H
#define NICREATEABVPLUGIN_H

#include <NiPlugin.h>

class NiCreateABVPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiCreateABVPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

protected:

    void ClearAllErrors();
    void ClearAllSectionErrors();
    void AppendAllSectionErrors();
    void ShowAllErrors();

    NiSceneGraphSharedData* m_pkSGSharedData;
    NiTPointerList<NiString*> m_pkErrors;
    NiTPointerList<NiString*> m_pkSectionErrors;

};

#endif  // #ifndef NICREATEABVPLUGIN_H
