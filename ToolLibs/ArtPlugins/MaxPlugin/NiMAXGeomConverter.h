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

#ifndef NIMAXGEOMCONVERTER_H
#define NIMAXGEOMCONVERTER_H

#include "NiMAX.h"
#include <NiTSimpleArray.h>

class NiMAXGeomConverter
{
public:
    static Modifier *FindModifier(Object *pObj, Class_ID kModID);
#ifndef NO_CSTUDIO
    
    // NtBoneData stores the bone information for a single NiTriShape vertex
    class  NtBoneData : public NiMemObject
    {
    public:
        INode *pBone;
        unsigned int uiBoneIndex;   // pBone's index into the NiBoneList
        float fWeight;

         NtBoneData *pNext;
    };
    

    class NtPointTracker;
    // NiBoneList is a very simple list class to help in
    // switching from bones indexed by vertices to vertices
    // indexed by bones. If time permits it should be replaced
    // with sometime that has:
    //      a fast insert (checking for prior copies) that
    //      always returns a unique index
    // A think a slightly modified hash table would be ideal
    class NiBoneList : public NiMemObject
    {
    public:
        NiBoneList(unsigned int uiTableSize);
        ~NiBoneList();
        
        unsigned int AddBone(INode *pBone, NtPointTracker *pTracker);
        inline unsigned int GetNumBones();
        
        class NtBoneEle : public NiMemObject
        {
        public:
            INode *pNode;
            unsigned int uiID;
            NiTransform initXform;
            NtBoneEle *pNext;
        };

        NtBoneEle* GetBoneEle(NtBoneData *pBoneData);
        
        NtBoneEle **m_ppBones;
        unsigned int m_uiTableSize;
        unsigned int m_uiNumBones;
    };

    // A NtPointTracker provides the information required to create a Skin.
    // The ppBoneData array contains a list of bones that influence each 
    // vertex.
    // The pBoneList contains a list of all the bones in ppBoneData
    class NtPointTracker : public NiMemObject
    {
    public:
        Modifier *pMod;
        IPhysiqueExport *pPExport;
        IPhyContextExport *pMCExport;
        
        unsigned int uiNumVerts;
        NtBoneData **ppBoneData;    // a [uiNumVerts] array of NtBoneData *

        NiBoneList *pBoneList;      // a list off all the bones in ppBoneData
        NiTransform kSkinInitTM;
        INode* pkSkin;

        bool bInvalid;
    } ;


    static void TrackInit(
        NtPointTracker *pTracker, 
        unsigned int uiNumVerts);
    static void TrackDelete(NtPointTracker *pTracker);
    static void TrackVert(
        NtPointTracker *pTracker, 
        int iIndex);
    static void TrackExtractInfo(
        NtPointTracker *pTracker, 
        NiMesh* pkMesh,
        unsigned short *pRemapping,
        unsigned int *pNumBones,
        INode ***pppBones,
        INode **ppRootParent,
        NiTransform** ppInitXforms,
        NiTransform* pkRootParentToSkin);

#else
    typedef void* NtPointTracker;
#endif
    static bool CreateBindPoseStream(NiMesh*pkMesh, NiUInt32 uiVertCount,
        const NiFixedString& kSourceSemantic, 
        const NiFixedString& kDestSemantic, bool bHasMorph);
    static void SetupSkinningBPStreams(NiMesh* pkMesh);
    static void AddFourHighestWeights(
        const unsigned int uiSourceNumBones,
        float *pfSourceWeights, 
        unsigned int *puiSourceBoneIndices, 
        const unsigned int uiDestinationVertexIdx, 
        NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> > 
        &puiDestinationBoneIndices, 
        NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >
        &pfDestinationBoneWeights);

    static void ConvertSkin(
        ISkin* pkSkinMod,
        NiMesh* pkMesh, 
        unsigned short* pusRemapping,
        unsigned int& uiNumBones,
        INode**& ppkBones,
        INode*& pkRootParent,
        NiTransform*& pkInitXforms,
        NiTransform& kRootParentToSkin,
        INode* pkMaxNode,
        bool bRigidVertices);

    static NiTPointerList<NiSkinningMeshModifier *> m_skinList2;

    static void Init();
    static int Finish(NiNode *pRoot, TimeValue animStart);
    static void Shutdown();

    //
    // Skin and bones
    //
    static bool ResolveBones(NiSkinningMeshModifier *pSkin);

private:
};

#include "NiMAXGeomConverter.inl"

#endif
