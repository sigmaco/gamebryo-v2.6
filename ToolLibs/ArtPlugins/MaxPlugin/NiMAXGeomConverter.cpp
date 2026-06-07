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

#include "MaxImmerse.h"
#include "NiMaxGeomConverter.h"
#include "NiMAXHierConverter.h"
#include "NiUtils.h"
#include "NiMAXCustAttribConverter.h"
#include <NiToolDataStream.h>
extern const char* gpcMultiMtlName;

NiTPointerList<NiSkinningMeshModifier *> NiMAXGeomConverter::m_skinList2;

#if CURRENT_VERSION < 3
static bool MoveSkins(NiAVObject* pkObject);
#endif

//---------------------------------------------------------------------------
void NiMAXGeomConverter::Init()
{
    NIASSERT(m_skinList2.IsEmpty());
}
//---------------------------------------------------------------------------
void RemoveDeadSkins(NiAVObject* pkObj)
{
    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            RemoveDeadSkins(pkNode->GetAt(ui));
        }
    }

    if (NiIsKindOf(NiMesh, pkObj))
    {
        NiMesh* pkMesh = (NiMesh*) pkObj;

        NiSkinningMeshModifier* pkSkinningModifier =
            NiGetModifier(NiSkinningMeshModifier, pkMesh);
        bool bRemoveModifier=false;
        if(pkSkinningModifier)
        {
            NiAVObject** pkBones=pkSkinningModifier->GetBones();
            for(int i=0;i<(int)pkSkinningModifier->GetBoneCount();i++)
            {
                if(pkBones[i]==NULL)
                {
                    bRemoveModifier=true;
                    break;
                }
            }
        }
        if(bRemoveModifier)
        {
            pkMesh->RemoveModifier(pkSkinningModifier);
        }
    }
}
//---------------------------------------------------------------------------
int NiMAXGeomConverter::Finish(NiNode* pRoot, TimeValue)
{
    // resolve bone pointers in any skin instances 
    NiMesh::CompleteSceneModifiers(pRoot);

    if (m_skinList2.IsEmpty())
        return(W3D_STAT_OK);

    NiTListIterator pSkinList = m_skinList2.GetHeadPos();
    bool bDeadSkins = false;
    while (pSkinList)
    {
        NiSkinningMeshModifier* pSkin = m_skinList2.GetNext(pSkinList); 

        NIASSERT(pSkin);
        if (!ResolveBones(pSkin))
        {
            bDeadSkins = true;
            NILOGWARNING("Skin uses bones that weren't exported. Skinning"
                " will be disabled!\n");
        }
    }

    if (bDeadSkins)
    {
        RemoveDeadSkins(pRoot);
    }

#if CURRENT_VERSION < 3
    MoveSkins(pRoot);
#endif

    NiMesh::ResetSceneModifiers(pRoot);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
#if CURRENT_VERSION < 3
bool MoveSkins(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        unsigned int uiChildren = pkNode->GetArrayCount();

        // By the time this loop exits, the number of children that pkNode 
        // has could change (because a skinned object might be moved), but 
        // if that happens, we only want to loop over the children that
        // already exist before we start the loop.

        for (unsigned int i = 0; i < uiChildren; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild != NULL)
            {
                if (!MoveSkins(pkChild))
                    break;
            }
        }
    }
    else if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*) pkObject;
        // If the object is skinned, make it a sibling of its root bone.
        const NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
        if (pkSkinInstance != NULL)
        {
            NiNode* pkRootParent = (NiNode*) pkSkinInstance->GetRootParent();
            NIASSERT(NiIsKindOf(NiNode, pkRootParent));

            // If multiple skinned TriShapes were created because of multiple
            // materials in MAX, we can move all the skinned objects at 
            // once by moving their parent node.

            // if the skin is a child of an __NDL_MultiMtl_Node then 
            // set spRelocatedObject to the __NDL_MultiMtl_Node.  Otherwise
            // set it to the NiTriShape.

            NiAVObjectPtr spRelocatedObject;
            if (strcmp(gpcMultiMtlName, 
                pkMesh->GetParent()->GetName()) == 0)
            {
                NiNode* pkNode = pkMesh->GetParent();

                // If these assertions fail, insert assignments to make them 
                // true.
                NIASSERT(pkNode->GetRotate() == NiMatrix3::IDENTITY);
                NIASSERT(pkNode->GetTranslate() == NiPoint3::ZERO);
                NIASSERT(pkNode->GetScale() == 1.0f);

                unsigned int i, uiChildren = pkNode->GetArrayCount();

                for (i = 0; i < uiChildren; i++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(i);
                    NIASSERT(pkChild != NULL);
                    pkChild->SetRotate(NiMatrix3::IDENTITY);
                    pkChild->SetTranslate(NiPoint3::ZERO);
                    pkChild->SetScale(1.0f);
                }

                spRelocatedObject = pkNode;
            }
            else
            {
                spRelocatedObject = pkGeometry;
                pkGeometry->SetRotate(NiMatrix3::IDENTITY);
                pkGeometry->SetTranslate(NiPoint3::ZERO);
                pkGeometry->SetScale(1.0f);
            }

            // check if spRelocatedObject's parent has any NiDynamicEffects
            // on it
            NiNode* pkROParent = spRelocatedObject->GetParent();
            const NiDynamicEffectList* pkList = &pkROParent->GetEffectList();
            if (!pkList->IsEmpty())
            {
                // insert a new node to put the effects on
                NiNodePtr spNode = NiNew NiNode;
                spNode->AttachChild(spRelocatedObject);

                NiTListIterator kIter = pkList->GetHeadPos();
                while (kIter)
                {
                    NiDynamicEffect* pkEffect = pkList->GetNext(kIter);
                    pkEffect->AttachAffectedNode(spNode);
                }
                spRelocatedObject = spNode;
            }

            spRelocatedObject->SetName(pkROParent->GetName());
            pkRootParent->AttachChild(spRelocatedObject, false);

            // delete spRelocatedObject's old parent.
            pkROParent->GetParent()->DetachChild(pkROParent);

            return false;
        }
    }

    return true;
}
#endif
//---------------------------------------------------------------------------
void NiMAXGeomConverter::Shutdown()
{
    m_skinList2.RemoveAll();
}

//---------------------------------------------------------------------------
Modifier* NiMAXGeomConverter::FindModifier(Object* pObj, Class_ID kModID)
{
    if (!pObj)
        return(NULL);

    IDerivedObject *pDerived = NULL;
    if (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        pDerived = (IDerivedObject *) pObj;

        for (int i = 0; i < pDerived->NumModifiers(); i++)
        {
            Modifier* pMod = pDerived->GetModifier(i);

            Class_ID cID = pMod->ClassID();
            if (cID == kModID)
                return(pMod);
        }
    }

    if (pDerived)
        return FindModifier(pDerived->GetObjRef(), kModID);
    else
        return(NULL);
}

//---------------------------------------------------------------------------
bool NiMAXGeomConverter::CreateBindPoseStream(NiMesh*pkMesh, 
    NiUInt32 uiVertCount, const NiFixedString& kSourceSemantic, 
    const NiFixedString& kDestSemantic, bool bHasMorph)
{
    NiDataStreamElementLock kLock = NiDataStreamElementLock(pkMesh,
        kSourceSemantic, 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_TOOL_READ);
    if (!kLock.DataStreamExists() || !kLock.IsLocked())
        return false;

    NiTStridedRandomAccessIterator<NiPoint3> kDataIter = 
        kLock.begin<NiPoint3>();

    // set the access to make sure it works with skinning
    NiToolDataStream* pkToolDataStream =
        NiDynamicCast(NiToolDataStream, kLock.GetDataStream());
    pkToolDataStream->MakeWritable();
    pkToolDataStream->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);

    NiUInt8 ucDestAccessMask = (bHasMorph) ? (NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE) :
    (NiDataStream::ACCESS_CPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_STATIC);

    NiDataStreamElementLock kBindPoseLock = NiDataStreamElementLock(pkMesh,
        kDestSemantic, 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    if (!kBindPoseLock.DataStreamExists())
    {
        kBindPoseLock = pkMesh->AddStreamGetLock(
            kDestSemantic, 0, NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
            ucDestAccessMask, NiDataStream::USAGE_VERTEX, true);
    }
    else
    {
        pkToolDataStream =
            NiDynamicCast(NiToolDataStream, kBindPoseLock.GetDataStream());
        pkToolDataStream->SetAccessMask(ucDestAccessMask);
    }

    if (kBindPoseLock.IsLocked())
    {
        // if we have successfully grabbed the bind pose stream, fill it in
        // with position data
        NiTStridedRandomAccessIterator<NiPoint3> kBindPose = 
            kBindPoseLock.begin<NiPoint3>();
        for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
        {
            kBindPose[ui] = kDataIter[ui];
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void NiMAXGeomConverter::SetupSkinningBPStreams(NiMesh* pkMesh)
{
    NiUInt32 uiNumVerts=pkMesh->GetVertexCount();

    // check for morphing to see if we need to make data mutable
    NiMorphMeshModifier* pkMorphModifier =
        NiGetModifier(NiMorphMeshModifier, pkMesh);
    bool bHasMorphModifier = (pkMorphModifier != 0);

    // copy position data into the position bind pose
    if (!CreateBindPoseStream(pkMesh, uiNumVerts, 
        NiCommonSemantics::POSITION(), NiCommonSemantics::POSITION_BP(), 
        bHasMorphModifier))
    {
        NILOGERROR("Failed to create position bind pose. Skinning will be "
            "disabled.");
        return;
    }

    CreateBindPoseStream(pkMesh, uiNumVerts, NiCommonSemantics::NORMAL(), 
        NiCommonSemantics::NORMAL_BP(), bHasMorphModifier);
    CreateBindPoseStream(pkMesh, uiNumVerts, NiCommonSemantics::BINORMAL(),
        NiCommonSemantics::BINORMAL_BP(), bHasMorphModifier);
    CreateBindPoseStream(pkMesh, uiNumVerts, NiCommonSemantics::TANGENT(),
        NiCommonSemantics::TANGENT_BP(), bHasMorphModifier);
}
//---------------------------------------------------------------------------
void NiMAXGeomConverter::ConvertSkin(
    ISkin* pkSkinMod,
    NiMesh* pkMesh,
    unsigned short* pusRemapping,
    unsigned int& uiNumBones,
    INode**& ppkBones,
    INode*& pkRootParent,
    NiTransform*& pkInitXforms,
    NiTransform& kRootParentToSkin,
    INode* pkMaxNode,
    bool bRigidVertices)
{
    CHECK_MEMORY();

    // Get skin initialization matrix
    Matrix3 kMaxInitTM;
    pkSkinMod->GetSkinInitTM(pkMaxNode, kMaxInitTM, true);
    bool bInitTMOkay;        
    NiTransform kNiInitTM;
    bInitTMOkay = MaxToNI(kMaxInitTM, kNiInitTM);
    NIASSERT(bInitTMOkay);
    // invert it
    kNiInitTM.Invert(kRootParentToSkin);
    unsigned int uiNumVerts=pkMesh->GetVertexCount();

    // allocate ni bones and init transforms -- one extra for skin as bone
    uiNumBones = pkSkinMod->GetNumBones();
    if (!uiNumBones)
        return;

    ++uiNumBones;

    SetupSkinningBPStreams(pkMesh);

    NiSkinningMeshModifier* pkModifier = NiNew NiSkinningMeshModifier(
        uiNumBones);

    ppkBones=(INode**)pkModifier->GetBones();
    pkInitXforms=pkModifier->GetSkinToBoneTransforms();

    ISkinContextData* pkSkinData = pkSkinMod->GetContextInterface(pkMaxNode);

    unsigned int uiHighestMAXVertex = 0;
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        if (pusRemapping[i] > uiHighestMAXVertex)
            uiHighestMAXVertex = pusRemapping[i];
    }

    if ((int)uiHighestMAXVertex >= pkSkinData->GetNumPoints())
    {
        NILOGWARNING("Object has a skin modifier that references fewer"
            " vertices than the object's geometry contains.\n"
            " Make sure skin modifier is applied to a mesh rather than"
            " a surface.\n");
        return;
    }

    // fill in bones and init matrices
    unsigned int uiBone = 0;
    for (; uiBone < uiNumBones-1; uiBone++)
    {
        ppkBones[uiBone] = pkSkinMod->GetBone(uiBone);

        pkSkinMod->GetBoneInitTM(ppkBones[uiBone], kMaxInitTM);
        bInitTMOkay = MaxToNI(kMaxInitTM, pkInitXforms[uiBone]);
        NIASSERT(bInitTMOkay);

        // invert initial pose matrices, and factor out skin's init tm.
        pkInitXforms[uiBone].Invert(pkInitXforms[uiBone]);
        pkInitXforms[uiBone] = pkInitXforms[uiBone] * kNiInitTM;
    }

    // skin as bone
    ppkBones[uiBone] = pkMaxNode;
    pkInitXforms[uiBone].MakeIdentity();

    unsigned int j=0;
    const unsigned int uiMaxInfluencingBones = 4;
    
    //add the weights and indicies streams
    NiDataStreamElementLock kBoneIndicesLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDINDICES(), 
        0,
        NiDataStreamElement::F_INT16_4,
        uiNumVerts, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_CPU_READ,
        NiDataStream::USAGE_VERTEX,
        true);

    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16, 4> > 
        puiBoneIndices =
        kBoneIndicesLock.begin<NiTSimpleArray<NiInt16, 4> >(0); 

    NiDataStreamElementLock kBoneWeightLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDWEIGHT(), 
        0,
        NiDataStreamElement::F_FLOAT32_4,
        uiNumVerts, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_CPU_READ,
        NiDataStream::USAGE_VERTEX,
        true);

    NiTStridedRandomAccessIterator<NiTSimpleArray<float, 4> > pfBoneWeights =
        kBoneWeightLock.begin<NiTSimpleArray<float, 4> >(0); 

    // fill in bone assignments and weights per vertex
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        // convert the NI index into the appropriate MAX index
        // We will use this index to get the bone information
        // from the pkSkinData.
        unsigned int uiMAXVertex = pusRemapping[i];

        unsigned int uiNumVertBones = 
            pkSkinData->GetNumAssignedBones(uiMAXVertex) + 1;

        unsigned int uiAddedIndexes[uiMaxInfluencingBones];

        //initialize current bone
        for(j = 0;j < uiMaxInfluencingBones; j++)
        {
            pfBoneWeights[i][j] = 0.0f;
            puiBoneIndices[i][j] = 0;
            uiAddedIndexes[j] = INT_MAX;
        }

        unsigned int *puiCurrentBoneIndices =
            NiAlloc(unsigned int, uiNumVertBones);
        NIASSERT(puiCurrentBoneIndices);

        float *pfCurrentWeights = NiAlloc(float, uiNumVertBones);
        NIASSERT(pfCurrentWeights);

        float fTotalWeight = 0.0f;

        bool bWarnedAlready = false;
        for (uiBone = 0; uiBone < uiNumVertBones-1; uiBone++)
        {
            // weight
            pfCurrentWeights[uiBone] = 
                pkSkinData->GetBoneWeight(uiMAXVertex, uiBone);

            // Check for -1.#IND, which can occur when Max attempts to
            // normalize weights and all the weights are 0.0f. 
            // Condition in the if statement must be set up this way - 
            // don't change it without testing this case.
            if (!(pfCurrentWeights[uiBone] >= -FLT_MAX &&
                pfCurrentWeights[uiBone] <= FLT_MAX))
            {
                if (!bWarnedAlready)
                {
                    char acstring[1024];
                    NiSprintf(acstring, 1024, "Vertex #%d in object \"%s\" "
                        "has unassigned bone weights. Please make sure that "
                        "the weights are properly set for this vertex.",
                        i, (const char*)NiMAXHierConverter::
                        GetCurrentExportObjectName());
                    NILOGWARNING(acstring);
                    bWarnedAlready = true;
                }
                pfCurrentWeights[uiBone] = 0.0f;
            }

            fTotalWeight += pfCurrentWeights[uiBone];

            // assignment 
            puiCurrentBoneIndices[uiBone] = 
                pkSkinData->GetAssignedBone(uiMAXVertex, uiBone);
        }

        // skin as bone

        // weight
        if (fTotalWeight >= 1.0f)
            pfCurrentWeights[uiBone] = 0.0f;
        else
            pfCurrentWeights[uiBone] = 1.0f - fTotalWeight;

        // assignment 
        puiCurrentBoneIndices[uiBone] = uiNumBones - 1;

        if (bRigidVertices)
        {
            float fBestWeight = -1.0f;
            unsigned int uiBestBone = (unsigned int) -1;

            for (unsigned int uiBone = 0; uiBone < uiNumVertBones; uiBone++)
            {
                // weight
                if (pfCurrentWeights[uiBone] > fBestWeight)
                {
                    fBestWeight = pfCurrentWeights[uiBone];
                    uiBestBone = puiCurrentBoneIndices[uiBone];
                }
            }

            if (uiBestBone != (unsigned int) -1)
            {
                uiNumVertBones = 1;
                pfCurrentWeights[0] = 1.0f;
                puiCurrentBoneIndices[0] = uiBestBone;
            }
            else
            {
                uiNumVertBones = 1;
                pfCurrentWeights[0] = 0.0f;
                puiCurrentBoneIndices[0] = 0;
            }
        }
        AddFourHighestWeights(
            uiNumVertBones,
            pfCurrentWeights,
            puiCurrentBoneIndices,
            i,puiBoneIndices,
            pfBoneWeights);

        NiFree(puiCurrentBoneIndices);
        NiFree(pfCurrentWeights);
    }

    kBoneWeightLock.Unlock();
    kBoneIndicesLock.Unlock();

    // find root bone
    INode* pkParent = ppkBones[0];
    while (pkParent != NULL)
    {
        pkRootParent = pkParent;
        pkParent = pkParent->GetParentNode();
    }
    pkModifier->SetRootBoneParentToSkinTransform(kRootParentToSkin);
    pkModifier->SetRootBoneParent((NiAVObject*)pkRootParent);    

    pkModifier->SetSoftwareSkinned(true);

    pkMesh->AddModifier(pkModifier);
    NiOptimize::CalculateBoneBounds(pkMesh);

    CHECK_MEMORY();

}
//---------------------------------------------------------------------------



#ifndef NO_CSTUDIO
NiMAXGeomConverter::NiBoneList::NiBoneList(unsigned int uiTableSize)
{
    m_uiTableSize = uiTableSize;

    m_ppBones = NiAlloc(NtBoneEle*, uiTableSize);
    NIASSERT(m_ppBones);
    memset(m_ppBones, 0, uiTableSize * sizeof(NtBoneEle *));

    m_uiNumBones = 0;
}

//---------------------------------------------------------------------------
NiMAXGeomConverter::NiBoneList::~NiBoneList()
{
    for (unsigned int i = 0; i < m_uiTableSize; i++)
    {
        NtBoneEle* pNext;
        for (NtBoneEle* pCur = m_ppBones[i]; pCur; pCur = pNext)
        {
            pNext = pCur->pNext;
            NiDelete pCur;
        }
    }

    NiFree(m_ppBones);
    m_ppBones = NULL;
    m_uiNumBones = 0;
}

//---------------------------------------------------------------------------
unsigned int NiMAXGeomConverter::NiBoneList::AddBone(INode* pBone, 
    NtPointTracker *pTracker)
{
    if (pBone == NULL)
        return 0;

    unsigned int uiHashVal = ((((int ) PtrToInt(pBone)) >> 2) ^ 0x19fa83) % 
        m_uiTableSize;

    NtBoneEle* pCur;
    NtBoneEle* pPrev;

    for (pPrev = NULL, pCur = m_ppBones[uiHashVal]; 
        pCur; 
        pPrev = pCur, pCur = pCur->pNext)
    {
        if (pCur->pNode == pBone)
            return(uiHashVal);

        // The following will keep list in order of increasing
        // INode* addresses.  pNew will be inserted before pCur.
        // The search/insert time benefits to this optimization are
        // probably negligable.
        if (pCur->pNode > pBone)
            break;
    }

    NtBoneEle* pNew = NiNew NtBoneEle;
    NIASSERT(pNew);

    pNew->pNode = pBone;
    pNew->uiID = m_uiNumBones;

    Matrix3 initTM;

    int iStatus = pTracker->pPExport->GetInitNodeTM(pBone, initTM);

    if (iStatus != MATRIX_RETURNED || 
        !MaxToNI(initTM, pNew->initXform))
    {
        // Bone init matrices are bogus, set flag to disable skin.
        pTracker->bInvalid = true;
    }

    if (pPrev == NULL)
    {
        // insert at beginning
        pNew->pNext = m_ppBones[uiHashVal];
        m_ppBones[uiHashVal] = pNew;
    }
    else
    {
        // insert in middle or at end
        pNew->pNext = pCur;
        pPrev->pNext = pNew;
    }

    m_uiNumBones++;
    return(uiHashVal);
}
//---------------------------------------------------------------------------
void NiMAXGeomConverter::TrackInit(NtPointTracker* pTracker, 
    unsigned int uiNumVerts)
{
    if (!pTracker)  
        return;

    pTracker->ppBoneData = NiAlloc(NtBoneData*, uiNumVerts);
    NIASSERT(pTracker->ppBoneData);

    memset(pTracker->ppBoneData, 0, uiNumVerts * sizeof(NtBoneData *));
    pTracker->uiNumVerts = uiNumVerts;

    pTracker->pBoneList = NiNew NiBoneList(113);
    NIASSERT(pTracker->pBoneList);
}

//---------------------------------------------------------------------------
// Extract the skin & bone information for vertex "iIndex"
void NiMAXGeomConverter::TrackVert(NtPointTracker* pTracker, int iIndex)
{

    if (!pTracker)  
        return;

    IPhyVertexExport* pVertex = (IPhyVertexExport *) 
        pTracker->pMCExport->GetVertexInterface(iIndex);

    if (pVertex)
    {
        NtBoneData* pBone;

        int iType = pVertex->GetVertexType();

        if (iType == RIGID_BLENDED_TYPE)
        {
            IPhyBlendedRigidVertex *pBlended;
            int i;

            // multiple bones for this vertex
            pBlended = (IPhyBlendedRigidVertex *) pVertex;

            for (i = 0; i < pBlended->GetNumberNodes(); i++)
            {
                pBone = NiNew NtBoneData;

                // add this bone to the start of this vertex' list
                pBone->pNext = pTracker->ppBoneData[iIndex];
                pTracker->ppBoneData[iIndex] = pBone;

                pBone->pBone = pBlended->GetNode(i);
                pBone->uiBoneIndex = 
                    pTracker->pBoneList->AddBone(pBone->pBone,
                    pTracker);

                pBone->fWeight = pBlended->GetWeight(i);
            }
        }
        else
        {
            NIASSERT( iType == RIGID_TYPE );

            IPhyRigidVertex *pRigid;

            // only one bone for this vertex
            pRigid = (IPhyRigidVertex *) pVertex;

            pBone = NiNew NtBoneData;

            // add this bone to the start of this vertex' list
            pBone->pNext = NULL;
            pTracker->ppBoneData[iIndex] = pBone;

            pBone->pBone = pRigid->GetNode();
            pBone->uiBoneIndex = pTracker->pBoneList->AddBone(pBone->pBone,
                pTracker);
            pBone->fWeight = 1.0f;
        }

        pTracker->pMCExport->ReleaseVertexInterface(pVertex);
    }
    else
        pTracker->bInvalid = true;


    // CS3 floating bones support
#if CURRENT_VERSION > 3

    IPhyFloatingVertex* pkFVertex = (IPhyFloatingVertex*) 
        pTracker->pMCExport->GetFloatingVertexInterface(iIndex);
    if (pkFVertex)
    {
        for (int i = 0; i < pkFVertex->GetNumberNodes(); i++)
        {
            NtBoneData* pBone = NiNew NtBoneData;

            // add this bone to the start of this vertex' list
            pBone->pNext = pTracker->ppBoneData[iIndex];
            pTracker->ppBoneData[iIndex] = pBone;

            pBone->pBone = pkFVertex->GetNode(i);
            pBone->uiBoneIndex = 
                pTracker->pBoneList->AddBone(pBone->pBone,
                pTracker);

            float fTotalWeight;
            pBone->fWeight = pkFVertex->GetWeight(i, fTotalWeight);
        }

        pTracker->pMCExport->ReleaseVertexInterface(pkFVertex);
    }

#endif
}

//---------------------------------------------------------------------------
void NiMAXGeomConverter::TrackDelete(NtPointTracker* pTracker)
{

    for (unsigned int i = 0; i < pTracker->uiNumVerts; i++)
    {
        NtBoneData *pCur, *pNext;

        for (pCur = pTracker->ppBoneData[i]; pCur; pCur = pNext)
        {
            pNext = pCur->pNext;
            NiDelete pCur;
        }
    }

    NiFree(pTracker->ppBoneData);
    pTracker->ppBoneData = NULL;
    pTracker->uiNumVerts = 0;

    NiDelete pTracker->pBoneList;
    pTracker->pBoneList = NULL;
}

//---------------------------------------------------------------------------
// This routine moves the skin and bone information from the NtPointTracker 
// into the NiMesh
void NiMAXGeomConverter::TrackExtractInfo(
    NtPointTracker* pTracker, 
    NiMesh* pkMesh,
    unsigned short* pRemapping,
    unsigned int* pNumBones, 
    INode*** pppBones,
    INode **ppRootParent,
    NiTransform** ppInitXforms,
    NiTransform* pkRootParentToSkin)
{
    *pNumBones = 0;
    *pppBones = NULL;
    *ppRootParent = NULL;
    *ppInitXforms = 0;
    unsigned int uiNumVerts=pkMesh->GetVertexCount();

    if (!pTracker)
        return;

    if (pTracker->bInvalid)
    {
        NILOGWARNING("Object has a Physique modifier that has\n"
            " invalid initial bone transformation matrices.\n"
            " Delete the Physique modifier and create a new one\n"
            " in order to export skinning data correctly.\n");
        return;
    }

    SetupSkinningBPStreams(pkMesh);    

    unsigned int uiNumBones = *pNumBones = 
        pTracker->pBoneList->GetNumBones();

    NiSkinningMeshModifier* pkModifier = NiNew NiSkinningMeshModifier(
        uiNumBones);

    NiAVObject** ppBones=pkModifier->GetBones();
    NiTransform* pInitXforms=pkModifier->GetSkinToBoneTransforms();
    NIASSERT(pInitXforms);
    NIASSERT(ppBones);

    //add the weights and indicies streams
    NiDataStreamElementLock kBoneIndicesLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDINDICES(), 
        0,
        NiDataStreamElement::F_INT16_4,
        uiNumVerts, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_CPU_READ,
        NiDataStream::USAGE_VERTEX,
        true);

    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16, 4> >
        puiBoneIndices =
        kBoneIndicesLock.begin<NiTSimpleArray<NiInt16, 4> >(0); 

    NiDataStreamElementLock kBoneWeightLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDWEIGHT(), 
        0,
        NiDataStreamElement::F_FLOAT32_4,
        uiNumVerts, 
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_CPU_READ,
        NiDataStream::USAGE_VERTEX,
        true);

    NiTStridedRandomAccessIterator<NiTSimpleArray<float, 4> > pfBoneWeights =
        kBoneWeightLock.begin<NiTSimpleArray<float, 4> >(0); 

    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        // convert the NI index into the appropriate MAX index
        // We will use this index to get the information from the ppBoneData
        // array
        unsigned int uiVertex = pRemapping[i];

        NIASSERT(uiVertex < pTracker->uiNumVerts);
        NIASSERT(pTracker->ppBoneData[uiVertex]);

        unsigned int j;

        // Count the number of bones influencing this vertex. We could
        // count this up as the bones are accumulated.
        unsigned int uiCurNumBones = 0;
        NtBoneData* pCur;
        for (pCur = pTracker->ppBoneData[uiVertex]; pCur; pCur = pCur->pNext)
            uiCurNumBones++;

        unsigned int *puiCurrentBoneIndices =
            NiAlloc(unsigned int, uiCurNumBones);
        NIASSERT(puiCurrentBoneIndices);

        float *pfCurrentWeights = NiAlloc(float, uiCurNumBones);
        NIASSERT(pfCurrentWeights);

        for (pCur = pTracker->ppBoneData[uiVertex], j = 0; 
            pCur; 
            pCur = pCur->pNext, j++)
        {
            NiBoneList::NtBoneEle* pBoneEle =
                pTracker->pBoneList->GetBoneEle(pCur);

            unsigned int uiBoneID = pBoneEle->uiID;

            puiCurrentBoneIndices[j] = uiBoneID;
            pfCurrentWeights[j] = pCur->fWeight;

            ppBones[uiBoneID] = (NiAVObject*)pCur->pBone;
            pInitXforms[uiBoneID] = pBoneEle->initXform;
        }
        AddFourHighestWeights(
            uiCurNumBones,
            pfCurrentWeights,
            puiCurrentBoneIndices,
            i,puiBoneIndices,
            pfBoneWeights);

        NiFree(puiCurrentBoneIndices);
        NiFree(pfCurrentWeights);

    }

#ifdef NIDEBUG
    for (unsigned int uiB = 0; uiB < uiNumBones; uiB++)
    {
        NIASSERT(ppBones[uiB]);
    }
#endif

    // find root bone

#if CURRENT_VERSION >= 3
    INode* pParent = (INode*)ppBones[0];

    while (pParent != NULL)
    {
        *ppRootParent = pParent;
        pParent = pParent->GetParentNode();
    }
    pTracker->kSkinInitTM.Invert(*pkRootParentToSkin);
#else
    *ppRootParent = pTracker->pkSkin->GetParentNode()->GetParentNode();
    pkRootParentToSkin->MakeIdentity();
#endif

    pkModifier->SetRootBoneParentToSkinTransform(*pkRootParentToSkin);
    pkModifier->SetRootBoneParent((NiAVObject*)*ppRootParent);    

    // invert initial pose matrices, and factor out skin's init tm.

    for (unsigned int uiBone = 0; uiBone < uiNumBones; uiBone++)
    {
        pInitXforms[uiBone].Invert(pInitXforms[uiBone]);
        pInitXforms[uiBone] = pInitXforms[uiBone] * pTracker->kSkinInitTM;
    }

    *pppBones = (INode**)ppBones;
    *ppInitXforms = pInitXforms;

    pkModifier->SetSoftwareSkinned(true);

    pkMesh->AddModifier(pkModifier);
    NiOptimize::CalculateBoneBounds(pkMesh);

    // We can delete all the information since it is now stored in the
    // NiMesh
    TrackDelete(pTracker);
}
#endif
//---------------------------------------------------------------------------
//This adds the four highest weights to the bone
void NiMAXGeomConverter::AddFourHighestWeights(
    const unsigned int uiSourceNumBones, 
    float *pfSourceWeights,
    unsigned int *puiSourceBoneIndices, 
    const unsigned int uiDestinationVertexIdx,
    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16, 4> >
    &puiDestinationBoneIndices, 
    NiTStridedRandomAccessIterator<NiTSimpleArray<float, 4> >
    &pfDestinationBoneWeights)
{
    const unsigned int uiMaxInfluencingBones = 4;
    unsigned int auiAddedIndexes[uiMaxInfluencingBones];

    //initialize bone weights and indices
    unsigned int j=0;
    for(j = 0;j < uiMaxInfluencingBones; j++)
    {
        pfDestinationBoneWeights[uiDestinationVertexIdx][j] = 0.0f;
        puiDestinationBoneIndices[uiDestinationVertexIdx][j] = 0;
        auiAddedIndexes[j] = INT_MAX;
    }
    unsigned int uiMaxIndex = 0;
    unsigned int uiCurrentIndex = 0;
    bool bAlreadyAdded = false;
    float fSummedWeight = 0.0f;
    float fMaxWeight = -FLT_MAX;
    for(int iBoneIndex = 0; iBoneIndex < (int)uiMaxInfluencingBones &&
        iBoneIndex < (int)uiSourceNumBones; iBoneIndex++)
    {
        for(j = 0;j < uiSourceNumBones; j++)
        {
            bAlreadyAdded = false;
            //iterate through added list to check if the bone is already added
            for(unsigned int k = 0;k < uiMaxInfluencingBones; k++)
            {
                if(auiAddedIndexes[k] == j)
                {
                    bAlreadyAdded = true;
                    break;
                }
            }
            //if the bone is already added, skip it
            if(bAlreadyAdded)
                continue;

            //check if this is the bone with the largest weight
            if(pfSourceWeights[j] > fMaxWeight)
            {
                uiMaxIndex = j;
                fMaxWeight = pfSourceWeights[j];
            }
        }
        //add it to the added list
        auiAddedIndexes[uiCurrentIndex] = uiMaxIndex;

        //then add the bone to the weight list
        unsigned int uiBoneIndex = puiSourceBoneIndices[uiMaxIndex];
        float fWeight = pfSourceWeights[uiMaxIndex];
        puiDestinationBoneIndices[uiDestinationVertexIdx][uiCurrentIndex] = 
            (unsigned short)uiBoneIndex;

        pfDestinationBoneWeights[uiDestinationVertexIdx][uiCurrentIndex] =
            fWeight;

        //sum of all weights
        fSummedWeight += fWeight;
        uiCurrentIndex++;
        fMaxWeight =- FLT_MAX;
    }

    if(uiSourceNumBones > uiMaxInfluencingBones)
    {
        //divide by the sum of all bones weights to normalize the weights
        for(int j = 0;j < uiMaxInfluencingBones; j++)
        {
            pfDestinationBoneWeights[uiDestinationVertexIdx][j] /=
                fSummedWeight;
        }
    }
}
//---------------------------------------------------------------------------
bool NiMAXGeomConverter::ResolveBones(NiSkinningMeshModifier* pSkin)
{
    // Now the bone weights and bone indices data streams are filled in
    // Gather the data that the skin mesh modifier needs
    NiAVObject* pAVRootParent =
        NiMAXHierConverter::Lookup((INode*) pSkin->GetRootBoneParent(), NULL);
    pSkin->SetRootBoneParent(pAVRootParent);

    NiAVObject** ppkAVBones = pSkin->GetBones();
    NiUInt32 uiNumBones = pSkin->GetBoneCount();
    for (NiUInt32 i = 0; i < uiNumBones; i++)
    {
        void* pBone = ppkAVBones[i];
        NiAVObject* pkBone = NiMAXHierConverter::Lookup((INode*) pBone, NULL);
        if(!pkBone)
        {
            ppkAVBones[i] = NULL;
            return false;
        }
        ppkAVBones[i] = pkBone;
    }

    return true;
}
//---------------------------------------------------------------------------

