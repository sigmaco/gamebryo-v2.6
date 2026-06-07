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

#ifndef NIFURGENERATORPLUGIN_H
#define NIFURGENERATORPLUGIN_H

#include <NiTStridedRandomAccessIterator.h>
#include <NiTSimpleArray.h>
#include <NiMesh.h>

class NiSkinningMeshModifier;
class NiMorphMeshModifier;

class NiFurGeneratorPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiFurGeneratorPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    static bool PerObjectCallback(NiAVObject* pkObject);
    static void ErrorCallback(NiUInt32 uiCode, NiObject* pkObject,
        const char* pcMessage);

protected:

    NiTPointerList<NiMesh*>* m_pkMeshList;

    void Init();
    void Destroy();
    void BuildMeshList(NiSceneGraphSharedData* pkSGSharedData);
    void RecusiveMeshSearch(NiNode* pkNode);

    void RemoveDetailMap(NiAVObject* pkNode);
    bool HasFurData(NiMesh* pkMesh);

    void GenerateFur();

    void SubdivideTriangle(
        NiUInt32 uiIndex0, NiUInt32 uiIndex1, NiUInt32 uiIndex2,
        NiTStridedRandomAccessIterator<NiPoint3>& kPositionIter,
        NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
        NiTStridedRandomAccessIterator<NiPoint2>& kTexCoordIter,
        NiTStridedRandomAccessIterator<NiColorA>& kColorIter,
        NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> >& kBoneIter,
        NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >& kWeightIter,
        NiTStridedRandomAccessIterator<NiPoint3>& kBindPosIter,
        NiTStridedRandomAccessIterator<NiPoint3>& kBindNormIter,
        NiUInt32 uiNumTargets,
        NiTStridedRandomAccessIterator<NiPoint3>* pkMorphPositionsIter,
        NiTStridedRandomAccessIterator<NiUInt32>& kIndexIter,
        NiUInt32& uiCurrentVertCount,
        NiUInt32& uiNewIndexCounter);

    NiMeshPtr GenerateFurMeshFromMesh(NiMesh* pkMesh);

    void GenerateShellTexture();

    void GenerateShellsFromMesh(NiMesh* pkMesh, 
        NiSortAdjustNode* pkSortNode, char* cpName);

    NiSkinningMeshModifier* CloneSkinningForShells(
        NiMesh* pkMesh,
        NiSkinningMeshModifier* pkSkinningMeshModifier, 
        const NiUInt32 uiShellCount, const NiUInt32 uiBaseVertCount,
        const NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> >& 
            kBoneIter,
        const NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >& 
            kWeightIter,
        const NiTStridedRandomAccessIterator<NiUInt16>& kRemapIter,
        const NiTStridedRandomAccessIterator<NiPoint3>& kPositionIter,
        const NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
        bool bIsMorphed);

    NiMorphMeshModifier* CloneMorphingForShells(NiMesh* pkNewMesh,
        NiMesh* pkOldMesh, NiMorphMeshModifier* pkMorphMeshModifier,
        const NiFixedString& kMorphSemantic, const NiUInt32 uiShellCount,
        const NiUInt32 uiBaseVertCount);

    bool IsEdgeInList(NiUInt32* puiTestEdge, NiUInt32* puiEdgeList, 
        NiInt32 iNumberOfEdges);

    void GenerateFinsFromMesh(NiMesh* pkMesh, 
        NiSortAdjustNode* pkSortNode, char* cpName);

    NiSkinningMeshModifier* CloneSkinningForFins(
        NiMesh* pkMesh, 
        NiSkinningMeshModifier* pkSkinningMeshModifier, 
        const NiUInt32* puiEdgeList, const NiUInt32 uiEgdgeCount,
        const NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> >& 
            kBoneIter,
        const NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >& 
            kWeightIter,
        const NiTStridedRandomAccessIterator<NiUInt16>& kRemapIter,
        const NiTStridedRandomAccessIterator<NiPoint3>& kPositionIter,
        const NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
        bool bIsMorphed);

    NiMorphMeshModifier* CloneMorphingForFins(NiMesh* pkNewMesh,
        NiMesh* pkOldMesh, NiMorphMeshModifier* pkMorphMeshModifier,
        const NiFixedString& kMorphSemantic, const NiUInt32* puiEdgeList,
        const NiUInt32 uiEgdgeCount);

    // Fur generation parameters
    bool m_bGenerateDenseFins;
    NiUInt32 m_uiNumberOfShells;
    float m_fFurLength;
    float m_fFurLengthRandomness;
    float m_fTextureTilesPerFin;
    float m_fTextureTilesPerFinRandomness;
    float m_fShellSizeFactor;
    float m_fShellTextureSize;
    float m_fFurDensity;
    NiColorA m_kFurTintColor;

    NiFixedString m_kShellsShaderName;
    NiFixedString m_kFinsShaderName;
    NiUInt32 m_uiShellTextureCellSize;
    NiUInt32 m_uiNumberOfCells;
    NiUInt32 m_uiBonesPerPartition;

    NiProgressDialog* m_pkProgressBar;
    static NiFurGeneratorPlugin* ms_pkThis;

    NiSourceTexture* m_pkShellTexture;
};

#endif  // #ifndef NIFURGENERATORPLUGIN_H
