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

#ifndef ASSETANALYZERPLUGIN_H
#define ASSETANALYZERPLUGIN_H

#include "NiTPointerList.h"

/// This is our Asset Analyzer process plug-in. It is used to examine art 
/// during export and report when unexpected things appear. These conditions
/// are not strictly errors, they are expected results from artists.
class AssetAnalyzerPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    AssetAnalyzerPlugin();

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

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);

protected:

    bool MaxTriangleCount(NiUInt32 uiMaxTriangles);
    NiUInt32 RecursiveMaxTriangleCount(NiAVObject* pkObject);
    NiUInt32 GetTriangleCount(NiMesh* pkMesh);

    bool MaxTextureSize(NiAVObject* pkObject, NiUInt32 uiMaxTextureSize);
    bool CheckTextureSize(NiTexturingProperty::Map* pkMap,
        NiUInt32 uiSize, const char* pcMapName, const char* pcObjectName );

    bool MaxSceneMemorySize(NiUInt32 uiSceneMemorySize);
    NiUInt32 RecursiveMaxSceneMemorySize(NiAVObject* pkObject);
    NiUInt32 ComputeMeshDataSize(NiAVObject* pkObjecct);
    NiUInt32 ComputeAnimationDataSize(NiAVObject* pkObject);
    NiUInt32 ComputeAnimationDataSize(NiTimeController* pkCtlr);

    bool MaxTotalTextureSize(NiUInt32 uiTotalTextureSize);
    void RecursiveMaxTotalTextureSize(NiAVObject* pkObject, 
        NiTPointerList<NiTexture*>& kTextureList);
    void InsertUniqueTexture(NiTPointerList<NiTexture*>& kTextureList,
        NiTexturingProperty::Map* pkMap);
    void InsertUniqueTexture(NiTPointerList<NiTexture*>& kTextureList,
        NiTexture* pkTexture);
    NiUInt32 ComputeTotalTextureSize(NiTPointerList<NiTexture*>& kTextureList);

    bool MaxObjectCount(NiUInt32 uiMaxObjectCount);
    NiUInt32 RecursiveMaxObjectCount(NiAVObject* pkObject);

    bool MinTriangleMeshRatio(float fRatio);
    void RecursiveMinTriangleMeshRatio(NiAVObject* pkObject, 
        NiUInt32& uiTriangles, NiUInt32& uiMeshes);
   
    bool MinTriangleToStripRatio(float fRatio);
    void RecursiveMinTriangleToStripRatio(NiAVObject* pkObject,
        NiUInt32& uiTriangles, NiUInt32& uiStrips);
   
    bool MultiSubObjectWarnings(NiAVObject* pkObject,
        NiUInt32 uiMaxSubObjects);
    bool CheckForMultiSubObject(NiAVObject* pkObject,
        NiUInt32 uiMaxSubObjects);

    bool MaxMorphingVertexPerObject(NiAVObject* pkObject,
        NiUInt32 uiMaxVertexes);

    bool MaxTriangleCountPerObject(NiAVObject* pkObject,
        NiUInt32 uiMaxTriangles);

    bool MaxSceneGraphDepth(NiAVObject* pkObject, NiUInt32 uiMaxDepth);

    bool MaxLightsPerObject(NiAVObject* pkObject, NiUInt32 uiMaxLights);

    bool RequiredObjects(NiAVObject* pkObject,
        NiTPointerList<NiString*>* pkObjectList);
    void BuildRequireObjectList(NiTPointerList<NiString*>* pkObjectList,
        const NiPluginInfo* pkInfo);

    bool NoSpotLights(NiAVObject* pkObject);

    bool NoPointLights(NiAVObject* pkObject);

    bool NoDirectionalLights(NiAVObject* pkObject);

    bool NoAmbientLights(NiAVObject* pkObject);

    void ClearAllErrors();
    void ClearAllSectionErrors();
    void AppendAllSectionErrors();
    void ShowAllErrors();

    NiSceneGraphSharedData* m_pkSGSharedData;
    NiTPointerList<NiString*> m_pkErrors;
    NiTPointerList<NiString*> m_pkSectionErrors;

};

#endif  // #ifndef ASSETANALYZERPLUGIN_H
