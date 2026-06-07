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

#ifndef MYITRIMESH_H
#define MYITRIMESH_H

#include <NiMesh.h>
#include <NiSmartPointer.h>
#include <NiTSimpleArray.h>
#include <NiTStringMap.h>

#include "MyiObject.h"
#include "MyiMaterialManager.h"
#include "MyiTextureManager.h"
#include <maya/MFnMesh.h>
#include <maya/MFnBlendShapeDeformer.h>

//NiSmartPointer(NiMesh);
class NiPoint3;
class NiColorA;
class NiTextureProperty;

typedef NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> > 
    NiTIndexIter;
typedef NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> > NiTWeightIter;

class MyiTriMesh : public MyiObject
{
public:
    MyiTriMesh( int iShapeNum, int iGroupID,
        MyiMaterialManager* pkMaterialManager,
        MyiTextureManager* pkTextureManager);

    ~MyiTriMesh( void );

    inline NiMesh* GetMesh( void );

    static bool GetMaterialProperties( int iShapeID,int iGroupID,
        int iMaterialID,
        MyiMaterialManager *pkMaterialManager, 
        NiTexturingProperty* pTexturingProperty, bool bNeedsAlphaProperty,
        bool bHasVertexColors,
        NiAVObject* pkAVObject);

    bool CreateMultiTexture( int iShapeID, int iGroupID, 
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        bool &bNeedsAlphaProperty);

    static bool CreateMultiTexture( int iMaterialID, 
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        bool &bNeedsAlphaProperty);

    bool CreateMultiTextureForType( int iShapeID, int iGroupID,
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        char *szType,
        bool &bNeedsAlphaProperty);
    
    static bool CreateMultiTextureForType( int iMaterialID,
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        char *szType,
        bool &bNeedsAlphaProperty);
    static bool HasAnimatedTextureTransform(NiTexturingProperty* pkProperty,
        NiTexturingProperty::Map* pkMap);
    
    bool GetShapeAttributesForNi( int iShapeID, int iGroupID );

    bool GetVertsForNi(int iShapeID, int iGroupID);

    bool GetTriIndexListForNi(int iShapeID, int iGroupID);

    bool GetNormsForNi(int iShapeID, int iGroupID );

    bool GetUVsForNi(int iShapeID, int iGroupID );

    bool GetVertexColorsForNi(int iShapeID, int iGroupID);

    bool GetBinormalTangents(int iShapeID, int iGroupID, int iUVSet);

    bool ConvertSkinModifier(NiInt32 iShapeID, NiInt32 iGroupID, 
        NiMesh* pkMesh);

    // helper function for transforming MDt transform format to Gamebryo's
    static void MatrixToTransform(float afMat[4][4], 
        NiTransform& kTransform);

protected:

    int AssignUVs(int iShapeID, int iGroupID, char *pTextureName, 
        int iTextureID);
    void BakeUVs(NiTexturingProperty* pkTexProp);

    void AttachMorpherControllers(int iShapeID,int iGroupID, 
        const unsigned int* puiNewToOld, MStatus* pkStatus = NULL);
    void AddUniqueName(NiMorphWeightsController* pkMorph,
        const char *szTargetName, int iTarget);
    MFnMesh GetBaseMesh(const MFnBlendShapeDeformer& kBlendShape);
    NiPoint3* GetUndeformedBaseVertices(
        const MFnBlendShapeDeformer& kBlendShape, 
        const unsigned int* puiNewToOld, const int* piFaceList);
    NiPoint3* GetCollapsedVertices(const MPointArray& kPointArray,
        const unsigned int* puiNewToOld, const int* piFaceList);
    bool GetVertsForNiAsArray(int iShapeID, int iGroupID,
        NiPoint3*& pkPosition, unsigned int& uiNumVertices);

    void AttachProperties(int iShapeID, int iGroupID, 
        NiTexturingProperty* pTexturingProperty);

    static NiTexturingProperty::ClampMode GetClampMode(char* pTextureName);

    static void CreateFlipBookProperty(int iShapeID, int iGroupID, 
        int iTextureID, char* szType, MyiTextureManager* pkTextureManager, 
        NiTexturingProperty::Map *pMap, 
        NiTexturingProperty *pTexturingProperty);

    static void CreateFlipBookProperty(int iMaterialID, int iTextureID, 
        char* szType, MyiTextureManager* pkTextureManager, 
        NiTexturingProperty::Map *pMap, 
        NiTexturingProperty *pTexturingProperty);

    bool GetBlendShape(int iShapeID, int iGroupID, MObject& BlendShape);

    bool GetMorphUpdateNormals(int iShapeID);

    void AddNoStripifyAttributes(MFnDagNode& dgNode);

    MStatus ConvertUVSetNameToId(const MStringArray& kUvSetNames, 
        MIntArray& kUVSetIds);

    unsigned int FindNextAvailableUVId(unsigned int iUVSetCount, 
        int* pAvailableIds);

    bool CreateSkinDataStreams(NiMesh* pkMesh, NiUInt32 uiVertCount, 
        NiUInt32 uiBoneCount, NiDataStreamElementLock& kBoneLock, 
        NiDataStreamElementLock& kWeightLock);

    bool CreateBindPoseStream(NiMesh*pkMesh, NiUInt32 uiVertCount,
        const NiFixedString& kSourceSemantic, 
        const NiFixedString& kDestSemantic, bool bHasMorph);

    bool AttachPerMeshProfileData(MObject kShapeNode);

    const char *m_pName;
    float     m_fScale[3];
    bool      m_bHasVertexColors;
    bool      m_bHasAlpha;
    
    NiMeshPtr m_spMesh;
    bool m_bGotTriangles;
    
    unsigned int m_uiNiVertices;
    unsigned int m_uiNiTriangles;
    
    unsigned int m_uiTriListLength;

    unsigned int m_uiMultiIndex;

    static const char* UV_SET_PREFIX;

    NiTStringMap<int> m_UVNameToIdMap; 
   
};

//---------------------------------------------------------------------------
inline NiMesh* MyiTriMesh::GetMesh()
{
    return m_spMesh;
}
//---------------------------------------------------------------------------

#endif
