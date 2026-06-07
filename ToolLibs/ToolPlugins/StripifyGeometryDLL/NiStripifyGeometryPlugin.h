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

#ifndef NISTRIPIFYGEOMETRYPLUGIN_H
#define NISTRIPIFYGEOMETRYPLUGIN_H

class NiStripifyGeometryPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiStripifyGeometryPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    static bool PerObjectCallback(NiAVObject* pkObject);
    static void ErrorCallback(unsigned int uiCode, NiObject* pkObject,
        const char* pcMessage);

protected:

    unsigned int CountMeshes(NiAVObject* pkObject);

    NiProgressDialog* m_pkProgressBar;
    static NiStripifyGeometryPlugin* ms_pkThis;
};

#endif  // #ifndef NISTRIPIFYGEOMETRYLUGIN_H
