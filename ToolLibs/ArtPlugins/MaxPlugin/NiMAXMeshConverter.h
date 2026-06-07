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

#ifndef NIMAXMESHCONVERTER_H
#define NIMAXMESHCONVERTER_H


#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiMAXUV.h"
#include "NiTHash.h"
#include "NiMAXGeomConverter.h"
#include "NiMAXVNormal.h"

#define USE_RADIOSITY_MESH
#pragma warning(push)
// unreferenced formal parameter
#pragma warning(disable: 4100)
// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
// nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4238)
// nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4239)
// 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4245)
// '' : assignment operator could not be generated
#pragma warning(disable: 4512)
#include <radiosityMesh.h>    
#pragma warning(pop)

#define USE_EDIT_NORMALS_MODIFIER
//---------------------------------------------------------------------------

class NiMAXMeshConverter : public NiMAXGeomConverter
{
public:
    NiMAXMeshConverter(TimeValue animStart, TimeValue animEnd);
    ~NiMAXMeshConverter() {};

    static void Preprocess(INode* pNode);
    static void Postprocess(INode* pNode);

    int ConvertGeomSub(
        Object *pObj,
        INode *pMaxNode, 
        NiNode *pNode,
        NiAVObject **ppGeom, 
        bool bIgnoreHash,
        bool bIgnoreMtls, 
        ScaleInfo *pParentScale,
        bool bDoVertexRemapping,
        unsigned short**& ppVertexRemapping,
        NiAVObject**& ppRemapTargets,
        unsigned int& uiNumRemappings,
        NiMAXUV*& pkNiMAXUV,
        NiUInt32**& ppuiNewToOld);

    void GetVerts(
        Object *pObj,
        INode *pMaxNode,
        ScaleInfo *pParentScale,
        unsigned int &uiNumVerts,
        NiPoint3 *&pVerts);

    static void ComputeVertexNormals(Mesh *pkMesh, 
        NiTStridedRandomAccessIterator<NiPoint3>& kNormalIterator,
        int iMaxAttr, 
        int iAttrId);
    

    class ISkinWrapper
    {
        public:
            ISkinWrapper(Modifier* pkMod);
            ISkin* m_pkSkinMod;
            bool m_bUseRigidVertices;

    };

    static int ConvertMesh(
        Mesh *pkMesh,
        MNMesh* pkMNMesh,
        NiMesh **ppTris, 
        const int iAttrId, 
        const int iMaxAttr,
        NiMAXUV* pkNiMAXUV,
        NiMatrix3 *pMat,
        bool bIsTwoSided,
        ScaleInfo *pParentScale,
        NtPointTracker *pTracker,
        unsigned short **ppRemapping,
        ISkinWrapper* pkSkinWrapper, 
        INode* pkMaxNode,
        NiUInt32*& puiNewToOld);

    static void Init();
    static void Shutdown();

    static void SetExportVertWeights(bool bSaveVertWeights);
    static bool GetExportVertWeights();

    static float ApplyScaleInfo(
        ScaleInfo *pParentScale, 
        NiPoint3 *pVertices, 
        int iNumVerts);

    static float ApplyScaleInfo(
        ScaleInfo *pParentScale, 
        NiTStridedRandomAccessIterator<NiPoint3>& pVertices, 
        int iNumVerts,
        bool bForceDown);

    static float ApplyScaleInfo(
        ScaleInfo *pParentScale, 
        NiMesh *pkMesh, 
        int iNumFaces, 
        bool bForceDown);

private:

#ifdef USE_EDIT_NORMALS_MODIFIER
    static bool MapMNMeshNormalsToMeshNormals(
        Mesh* pkMesh,
        int iFaceID, 
        MNMesh* pkMNMesh,
        MNNormalSpec* pkNormalSpec,
        Point3& norm0,
        Point3& norm1,
        Point3& norm2);
#endif
    static bool FaceUsesOtherMaterial(
        Mesh* pMesh,
        int iFace,
        const int iMaxAttr,
        const int iAttrId);
    static void ConvertUVs(
        NiPoint2 &UV,
        float* pUVs,
        int iMapIndex,
        NiMAXUV* pkNiMAXUV);    
    static NiStencilPropertyPtr ms_spStencilTwoSidedProperty;


#ifdef NIDEBUG
    static bool HasSimpleUVs(Mesh *pMesh);
    static bool HasSimpleVertexColors(Mesh *pMesh);
    static bool HasSimpleNormals(Mesh *pMesh);
    static bool AllOneMaterial(Mesh *pMesh, 
        const int iAttrId, 
        const int iMaxAttr);
#endif

    static bool ComputePrecacheFlags(NiMesh* pTris, Animatable * pObj);
    static bool ComputeHierPrecacheFlags(NiMesh* pTris, INode* pkMaxNode);
    static bool ComputeObjectPrecacheFlags(NiMesh* pTris, Object* pObj);
    static void NiMAXMeshConverter::SetStreamKeepFlag(
        NiMesh* pTris, 
        const NiFixedString& kSemantic,
        const NiUInt32 uiSemanticIndex = 0);

    TimeValue m_animStart, m_animEnd;

    static bool ProcessInstance(
        NiAVObject* pkOrig,
        NiAVObject* pkInst, 
        NiNode* pkInstParent,
        NiPoint3 kPos, 
        NiMatrix3 kRot, 
        ScaleInfo kScale);
    // the ObjHTab has to be AVObjects since multimaterials will
    // result in a NiNode and a bunch of Shape children
    static NiTHash<NiAVObjectPtr>* ms_pkMeshHTab;

    static bool ms_bExportVertWeights;
    static bool ms_bConvertNormals;

    static void *ms_pUserData;
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXMeshConverter.inl"

//---------------------------------------------------------------------------

#endif

