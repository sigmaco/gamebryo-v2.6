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

#ifndef NISKINANALYZERPLUGIN_H
#define NISKINANALYZERPLUGIN_H

class NiDataStreamElementLock;

class NiSkinAnalyzerPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiSkinAnalyzerPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

protected:
    static void FindSkinnedMesh(NiAVObject* pkRoot, 
        NiTList<NiMesh*>& kSkins);

    void NiSkinAnalyzerPlugin::AnalyzeMesh(NiMesh* pkMesh);

    NiUInt32 NiSkinAnalyzerPlugin::CountUsedBones(
        NiUInt32 uiSubmesh,
        NiUInt32 uiBonesInPartition,
        const NiDataStreamElementLock& kLockBlendweights,
        const NiDataStreamElementLock& kLockBlendindices,
        NiUInt32 uiVerts);

    NiString m_strMessage;
};

#endif  // #ifndef NISKINANALYZERPLUGIN_H
