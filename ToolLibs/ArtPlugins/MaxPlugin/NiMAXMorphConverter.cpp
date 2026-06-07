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
#include "NiMAXMorphConverter.h"
#include "NiUtils.h"
#include "NiMAXHierConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXAnimationConverter.h"
#include <NiToolDataStream.h>
#include <NiMeshTools.h>


bool PFMorphHashEq(void *pSearch, void *p, void *p2Search, void *p2);
void PFMorphHashFree(void *p2, NiAVObjectPtr &spObject);

NiTHash<NiAVObjectPtr>*
NiMAXMorphConverter::ms_pkMorphHTab = NULL;

void *NiMAXMorphConverter::m_pUserData = NULL;

Point3 NfGetVertexNormal(Mesh* pMesh, int iFaceNo, RVertex* pRV);

//---------------------------------------------------------------------------
bool PFMorphHashEq(
    const void *pSearch, 
    const void *p, 
    const void *p2Search, 
    const void *p2)
{
    if (pSearch != p)
        return(false);

    ScaleInfo *pScaleSearch = (ScaleInfo*) p2Search;
    ScaleInfo *pScale = (ScaleInfo*) p2;

    if (pScaleSearch != NULL)
    {
        if (pScale->bIsIdent && pScaleSearch->bIsIdent)
            return(true);

        if (NiOptimize::CloseTo(pScaleSearch->scale.s.x, pScale->scale.s.x) &&
            NiOptimize::CloseTo(pScaleSearch->scale.s.y, pScale->scale.s.y) &&
            NiOptimize::CloseTo(pScaleSearch->scale.s.z, pScale->scale.s.z) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.x, pScale->scale.q.x) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.y, pScale->scale.q.y) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.z, pScale->scale.q.z) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.w, pScale->scale.q.w))
            return(true);

        // if the scales are isotropic the rots don't matter
        if (!pScaleSearch->IsAnisotropic() && !pScale->IsAnisotropic())
            return(true);
    }
    else
        return(true);

    return(false);
}
//---------------------------------------------------------------------------
void PFMorphHashFree(const void *p2, NiAVObjectPtr &spObject)
{
    spObject = NULL;
    NiDelete ((ScaleInfo *) p2);
}
//---------------------------------------------------------------------------
NiMAXMorphConverter::NiMAXMorphConverter(
    TimeValue animStart,
    TimeValue animEnd)
{
    m_animStart = animStart;
    m_animEnd = animEnd;
}
//---------------------------------------------------------------------------
Modifier* NiMAXMorphConverter::FindMorpherModifier(Object *pObj)
{
    CHECK_MEMORY();
    int i;

    if (!pObj)
        return(NULL);

    IDerivedObject *pDerived = NULL;
    if (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        pDerived = (IDerivedObject *) pObj;

        for (i = 0; i < pDerived->NumModifiers(); i++)
        {
            Modifier *pMod;
            Class_ID cID;

            pMod = pDerived->GetModifier(i);

            cID = pMod->ClassID();
            if (cID == MR3_CLASS_ID)
               return(pMod);
        }
    }

    if (pDerived)
        return FindMorpherModifier(pDerived->GetObjRef());
    else
        return(NULL);
}
//---------------------------------------------------------------------------
void NiMAXMorphConverter::Preprocess(INode* pNode)
{
    CHECK_MEMORY();
    for (int i = 0; i < pNode->NumberOfChildren(); i++)
        Preprocess(pNode->GetChildNode(i));
        
    ObjectState kOState = pNode->EvalWorldState(0);
    Object* pkEvalObj = kOState.obj;
    if (!pkEvalObj) 
        return;

    Object* pkObjectRef = pNode->GetObjectRef();
    // Disable Morph Modifier so that we can get the morph in its
    // base form
    Modifier* pkMod = FindMorpherModifier(pkObjectRef);

    if (pkMod)
        pkMod->DisableMod();

    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXMorphConverter::Postprocess(INode* pNode)
{
    CHECK_MEMORY();
    Object* pkObjectRef = pNode->GetObjectRef();
    
    // Re-Enable Morph Modifier
    Modifier* pkMod = FindMorpherModifier(pkObjectRef);
    if (pkMod)
        pkMod->EnableMod();

    for (int i = 0; i < pNode->NumberOfChildren(); i++)
        Postprocess(pNode->GetChildNode(i));
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
bool NiMAXMorphConverter::IsMorphObject(INode *pMaxNode)
{
    CHECK_MEMORY();
    Object *pObjRef = pMaxNode->GetObjectRef();
    Class_ID cID = pObjRef->ClassID();

    Modifier *pMorpher = FindMorpherModifier(pMaxNode->GetObjectRef());
    if (pMorpher)
        return(true);

    if (cID == Class_ID(DERIVOB_CLASS_ID, 0))
    {
        IDerivedObject *pDObj = (IDerivedObject*) pObjRef;
        Object *pObjRef2 = pDObj->GetObjRef();
        Class_ID cID2 = pObjRef2->ClassID();
        
        if (cID2 == Class_ID(MORPHOBJ_CLASS_ID, 0))
            return(true);
    }
    else if (cID == Class_ID(MORPHOBJ_CLASS_ID, 0))
        return(true);

    return(false);
}
//---------------------------------------------------------------------------
bool NiMAXMorphConverter::ConvertBaseObject(
    INode *pMaxNode,
    NiNode *pNode, 
    ScaleInfo *pParentScale, 
    unsigned short**& ppVertexRemapping,
    NiAVObject**& ppRemapTargets,
    unsigned int& uiNumRemapTargets,
    NiUInt32**& ppuiNewToOld)
{
    CHECK_MEMORY();
    NiMAXUV* pkNiMAXUV = NULL;
    NiAVObject *pResult;

    ObjectState kOState = pMaxNode->EvalWorldState(m_animStart);
    Object* pObj = kOState.obj;
    NIASSERT(pObj != NULL);

    bool bReturn = true;
    Class_ID cID = pObj->ClassID();

    if (cID == Class_ID(PATCHOBJ_CLASS_ID, 0) ||
        cID == Class_ID(NURBSOBJ_CLASS_ID, 0)) 
    {
        NiString strName = pMaxNode->GetName();
        NiString strWarning = "Object, \"" + strName;
        strWarning += "\", has a morpher modifier but is not an editable "
            "mesh.\n"
            "The exporter will only convert the object, not the morphs.\n"
            "Please convert the object and its morph targets into an "
            "editable mesh\n in order to export out the morpher modifier.\n";

        NILOGWARNING((const char*) strWarning);
        bReturn = false;

        char acString[256];
        NiSprintf(acString, 256, "Morph export unsuccessful for CID: ( 0x%x ,"
            " 0x%x )\n", cID.PartA(), cID.PartB());
        NiOutputDebugString(acString);    
    }


    // use the mesh converter to get an NI version of the base Morpher 
    // object. Note that we request a remapping array.
    NiMAXMeshConverter mesh(m_animStart, m_animEnd);
    int iResult = mesh.ConvertGeomSub(
        pObj,
        pMaxNode,
        pNode,
        &pResult, 
        true, 
        false,
        pParentScale,
        true,
        ppVertexRemapping,
        ppRemapTargets, 
        uiNumRemapTargets,
        pkNiMAXUV,
        ppuiNewToOld);

    CHECK_MEMORY();
    NiDelete pkNiMAXUV;

    if ((iResult == W3D_STAT_OK) && (pResult != NULL))
    {
        // place the morphee in the hierarchy
        pNode->AttachChild(pResult);
        return bReturn;
    }
    else
    {
        NiString strName = pMaxNode->GetName();
        NiString strWarning = "Object, \"" + strName;
        strWarning += "\" failed to convert the morph base object.\n"
            "This object will not get exported.\n";
        NILOGWARNING((const char*) strWarning);
        return false;
    }
}
//---------------------------------------------------------------------------
void NiMAXMorphConverter::ConvertMorpher(
    MorphR3 *pMorpher, 
    INode *pMaxNode, 
    NiNode *pCurNode, 
    ScaleInfo *pParentScale)
{
    unsigned int i, j;

    // first convert the base object
    unsigned int uiNewNumVerts;
    unsigned short **ppFromWhere;
    NiMesh *pMorphee;
    NiAVObject** ppRemapTargets;
    unsigned int uiNumRemapTargets;
    NiUInt32** ppuiNewToOld = NULL;
    NiPoint3 **ppOrigVerts = NULL;
    NI_UNUSED_ARG(ppOrigVerts);

    bool bContinueConversion = ConvertBaseObject(
        pMaxNode,
        pCurNode, 
        pParentScale,
        ppFromWhere,
        ppRemapTargets,
        uiNumRemapTargets,
        ppuiNewToOld);

    if (!bContinueConversion)
        return;

    bool bUpdateNormals = false;
    TSTR pcPropBuffer;
    pMaxNode->GetUserPropBuffer(pcPropBuffer);
    if (strstr(pcPropBuffer, "NiMorphUpdateNormals"))
    {
        bUpdateNormals = true;
    }

    for (unsigned int uiRemapIdx = 0; uiRemapIdx < uiNumRemapTargets; 
        uiRemapIdx++)
    {
        if (!ppRemapTargets[uiRemapIdx])
            continue;

        if (!NiIsKindOf(NiMesh, ppRemapTargets[uiRemapIdx]))
            continue;

        NIASSERT(ppFromWhere[uiRemapIdx]);

        pMorphee = (NiMesh*) ppRemapTargets[uiRemapIdx];

        NiSkinningMeshModifier* pkSkinningModifier =
            NiGetModifier(NiSkinningMeshModifier, pMorphee);

        bool bHasSkinModifier = false;
        if(pkSkinningModifier)
            bHasSkinModifier = true;

        NiFixedString kMorphDestSemantic = bHasSkinModifier ? 
            NiCommonSemantics::POSITION_BP() : NiCommonSemantics::POSITION();

        NiFixedString kNormSemantic =  bHasSkinModifier?
            NiCommonSemantics::NORMAL_BP() : NiCommonSemantics::NORMAL();

        NiFixedString kMorphTargetSemantic = 
            NiMorphMeshModifier::MorphElementSemantic(kMorphDestSemantic);

        uiNewNumVerts = pMorphee->GetVertexCount();

        NiDataStreamElementLock kBaseVertsLock = pMorphee->AddStreamGetLock(
            kMorphTargetSemantic, 
            0,
            NiDataStreamElement::F_FLOAT32_3,   
            uiNewNumVerts, 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX,
            true);

        NiTStridedRandomAccessIterator<NiPoint3> pkBaseVerts =
                kBaseVertsLock.begin<NiPoint3>(0); 
        
        NiDataStreamElementLock kLock(
            pMorphee,
            NiCommonSemantics::POSITION(),
            0, 
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_TOOL_READ); 

        // If this lock is not valid, then vertex positions
        // were not present on the mesh.
        NIASSERT(kLock.IsLocked()); 

        for (NiUInt32 uiSubmesh = 0;
            uiSubmesh < kLock.GetSubmeshCount(); uiSubmesh++)
        {
            NiTStridedRandomAccessIterator<NiPoint3> kPositionIterator =
                kLock.begin<NiPoint3>(uiSubmesh); 

            for (unsigned int uiVertCounter = 0; 
                uiVertCounter < uiNewNumVerts; uiVertCounter++)
            {
                pkBaseVerts[uiVertCounter]=kPositionIterator[uiVertCounter];
            }
        }
        kLock.Unlock();

        // then convert the delta arrays and the morph keys
        NiInterpolator* ppStaticInterps[MR3_NUM_CHANNELS];
        TSTR pStaticNames[MR3_NUM_CHANNELS];

        NiMAXAnimationConverter anim(m_animStart, m_animEnd);
        morphChannel *pChannel;

        Control *pMorphControl = NULL;
        unsigned int uiNextFreeChannel = 0;
        for (i = 0; i < MR3_NUM_CHANNELS; i++)
        {
            pChannel = &(pMorpher->chanBank[i]);

            NIASSERT(pChannel);
            // skip over any set of deltas we won't be able to deal with
            if (!pChannel->mActive ||
                pChannel->mActiveOverride == FALSE ||
                pChannel->mNumPoints != pMorpher->MC_Local.Count)
            {
                continue;
            }

            NiDataStreamElementLock kStaticDeltasLock =
                pMorphee->AddStreamGetLock(
                kMorphTargetSemantic, 
                i + 1,
                NiDataStreamElement::F_FLOAT32_3,   
                uiNewNumVerts, 
                NiDataStream::ACCESS_CPU_WRITE_STATIC |
                NiDataStream::ACCESS_CPU_READ,
                NiDataStream::USAGE_VERTEX,
                true); 

            NiTStridedRandomAccessIterator<NiPoint3> ppStaticDeltas =
                kStaticDeltasLock.begin<NiPoint3>(0); 

            for (j = 0; j < uiNewNumVerts; j++)
            {
                NiUInt32 uiNewToOldIdx = ppuiNewToOld[uiRemapIdx][j];
                ppStaticDeltas[j].x = pChannel->mPoints
                    [ppFromWhere[uiRemapIdx][uiNewToOldIdx]].x;
                ppStaticDeltas[j].y = pChannel->mPoints
                    [ppFromWhere[uiRemapIdx][uiNewToOldIdx]].y;
                ppStaticDeltas[j].z = pChannel->mPoints
                    [ppFromWhere[uiRemapIdx][uiNewToOldIdx]].z;
            }

            Object *pObj = pMaxNode->GetObjectRef();
            Modifier *pkMod = NiMAXMeshConverter::FindModifier(pObj,
                Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

            bool bForceDown = (pkSkinningModifier || pkMod);

            NiMAXMeshConverter::ApplyScaleInfo(
                pParentScale, 
                ppStaticDeltas,
                uiNewNumVerts,
                bForceDown);

            for (j = 0; j < uiNewNumVerts; j++)
                ppStaticDeltas[j] -= pkBaseVerts[j];

            pStaticNames[uiNextFreeChannel] = pChannel->mName;

            // employ the animation converter to convert each set of
            // morph keys
            Control *pControl = pChannel->cblock->GetController(0);
            NIASSERT(pControl);
            if (pMorphControl == NULL)
                pMorphControl = pControl;
                    
            ppStaticInterps[uiNextFreeChannel] = anim.ConvertFloatAnim(
                pControl);
            if (ppStaticInterps[uiNextFreeChannel] != 0 &&
                NiIsKindOf(NiKeyBasedInterpolator, 
                ppStaticInterps[uiNextFreeChannel]))
            {
                ((NiKeyBasedInterpolator*)
                    ppStaticInterps[uiNextFreeChannel])->
                    FillDerivedValues(0);
            }
            else if (ppStaticInterps[uiNextFreeChannel] == 0)
            {
                float fValue = pChannel->cblock->GetFloat(0);
                if (fValue < 0.001 && fValue > -0.001)
                    fValue = 0.0f;
                // Convert the value from Max's 0..100 scale to GB's 0..1
                // For animated morph weights this is done below
                ppStaticInterps[uiNextFreeChannel] = NiNew 
                    NiFloatInterpolator(fValue * 0.01f);
            }

            if (ppStaticInterps[uiNextFreeChannel])
                ppStaticInterps[uiNextFreeChannel]->Collapse();

            uiNextFreeChannel++;

            kStaticDeltasLock.Unlock();
        }

        if (uiNextFreeChannel == 0)
        {
            // we did not find any morph data, report error and exit
            NiString strName = pMaxNode->GetName();
            NiString strWarning = "Object \"" + strName;
            strWarning += "\" did not have any morph targets matching the "
                "base geometry. No morph will be exported.";
            NILOGWARNING((const char*) strWarning);

            kBaseVertsLock.Unlock();
            for (unsigned int i = 0; i < uiNumRemapTargets; i++)
            {
                NiFree(ppFromWhere[i]);
            }
            NiFree(ppFromWhere);
            memset(ppRemapTargets, 0, uiNumRemapTargets * sizeof(NiAVObject*));
            NiFree(ppRemapTargets);
            return;
        }

        kBaseVertsLock.Unlock();

        unsigned int uiNumTargets = uiNextFreeChannel + 1;

        // Create the mesh modifier
        NiMorphMeshModifier* pkModifier =
            NiNew NiMorphMeshModifier((NiUInt16)uiNumTargets);

        if (bUpdateNormals)
        {
            NiMeshTools::GenerateNormalSharing(pMorphee,
                kMorphDestSemantic, 0, kNormSemantic, 0);
        }

        pkModifier->SetUpdateNormals(bUpdateNormals);
        pkModifier->SetRelativeTargets(true);
        pkModifier->SetNeedsUpdate();
        pkModifier->SetAlwaysUpdate(false);
        pkModifier->SetSkinned(bHasSkinModifier);
        if(bHasSkinModifier)
        {
            pkModifier->SetSWSkinned(pkSkinningModifier->GetSoftwareSkinned());
        }
        else
        {
            pkModifier->SetSWSkinned(false);
        }

        NiLinFloatKey* pkFloatKeys = NiNew NiLinFloatKey[2];
        pkFloatKeys[0].SetValue(0.0f);
        pkFloatKeys[0].SetTime(0.0f);
        pkFloatKeys[1].SetValue(0.0f);
        pkFloatKeys[1].SetTime(0.00001f);

        NiFloatInterpolator* pkBaseInterp = NiNew NiFloatInterpolator();
        pkBaseInterp->SetKeys(pkFloatKeys, 2, NiFloatKey::LINKEY);

        // finally construct the morpher controller
        NiMorphWeightsController *pkNIWeightsMorpher;

        pkNIWeightsMorpher = NiNew NiMorphWeightsController (uiNumTargets);
        NIASSERT(pkNIWeightsMorpher);

        for (i = 1; i < uiNumTargets; i++)
        {
            unsigned int uiIndex = i - 1;

            pkNIWeightsMorpher->SetTargetName((NiUInt16)i, 
                (const char*)pStaticNames[uiIndex]);

            NiFloatInterpolator* pkInterp = NiDynamicCast(
                NiFloatInterpolator, ppStaticInterps[uiIndex]);

            if (pkInterp)
            {
                //Make weights sum to 1 instead of 100 (0.75 instead of 75.0)
                for (j = 0; j < pkInterp->GetKeyCount(0); j++)
                {
                    NiFloatKey* pStaticKey = (NiFloatKey*) 
                        pkInterp->GetKeyAt(j, 0);
                    pStaticKey->SetValue(pStaticKey->GetValue() * 0.01f);
                    if (pkInterp->GetKeyType(0) == NiFloatKey::BEZKEY)
                    {
                        // For Bez float keys, reset the tangent values
                        // as well
                        NiBezFloatKey* pBezKey = (NiBezFloatKey*)pStaticKey;
                        pBezKey->SetInTan(pBezKey->GetInTan() * 0.01f);
                        pBezKey->SetOutTan(pBezKey->GetOutTan() * 0.01f);
                    }
                }
            }
        }

        // Add the weights stream
        NiDataStreamElementLock kWeightLock = pMorphee->AddStreamGetLock(
            NiCommonSemantics::MORPHWEIGHTS(),
            0,
            NiDataStreamElement::F_FLOAT32_1,
            uiNumTargets,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_USER, 
            true);

        NiTStridedRandomAccessIterator<float> kWeightIterator =
            kWeightLock.begin<float>(0); 

        kWeightIterator[0] = 1.0f;
        for (unsigned int ui = 1; ui < uiNumTargets; ui++)
            kWeightIterator[ui] = 0.0f;

        kWeightLock.Unlock();

        pkNIWeightsMorpher->SetInterpolator(pkBaseInterp, 0);
        for (i = 1; i < uiNumTargets; i++)
        {
            unsigned int uiIndex = i - 1;
            pkNIWeightsMorpher->SetInterpolator(ppStaticInterps[uiIndex], 
                (unsigned short)i);
        }
        pkNIWeightsMorpher->ResetTimeExtrema();

        if (NiMAXConverter::GetUseAppTime())
            pkNIWeightsMorpher->SetAnimType(NiTimeController::APP_TIME);
        else
            pkNIWeightsMorpher->SetAnimType(NiTimeController::APP_INIT);

        NiFixedString kBaseName("Base");
        pkNIWeightsMorpher->SetTargetName(0, kBaseName);

        // Specify which semantics it is morphing
        pkModifier->AddMorphedElement(kMorphDestSemantic, 0);

        // Add it to the mesh as the first modifier to guarantee it runs before
        // the NiSkinningMeshModifier.
        pMorphee->AddModifierAt(0, pkModifier, false);

        pkModifier->CalculateMorphBound(pMorphee);

        pkNIWeightsMorpher->SetTarget(pMorphee);

        pkNIWeightsMorpher->Update(0.0);

        // Set the access mask on the output data. It needs to be mutable so
        // that it can be morphed but not necessarily on every frame.        
        NiDataStreamRef* pkRef = pMorphee->FindStreamRef(kMorphDestSemantic);

        NIASSERT(pkRef);
        NiDataStream* pkDS = pkRef->GetDataStream();

        NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
        NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
        pkTDS->MakeWritable();
        pkTDS->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);

        if (bUpdateNormals)
        {
            pkTDS->MakeReadable();

            pkRef = pMorphee->FindStreamRef(NiCommonSemantics::INDEX());
            pkDS = pkRef->GetDataStream();
            NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
            NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
            pkTDS->MakeReadable();

            pkRef = pMorphee->FindStreamRef(kNormSemantic);
            pkDS = pkRef->GetDataStream();
            NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
            pkTDS = (NiToolDataStream*)pkDS;
            pkTDS->MakeWritable();
            pkTDS->MakeReadable();
            pkTDS->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);
        }

        NiMAXAnimationConverter::SetORT(pMorphControl,pkNIWeightsMorpher);

    }
    for (unsigned int i = 0; i < uiNumRemapTargets; i++)
    {
        NiFree(ppFromWhere[i]);
    }
    NiFree(ppFromWhere);
    memset(ppRemapTargets, 0, uiNumRemapTargets * sizeof(NiAVObject*));
    NiFree(ppRemapTargets);
    if (ppuiNewToOld)
    {
        for (unsigned int uiRemapIdx = 0; uiRemapIdx < uiNumRemapTargets; 
            uiRemapIdx++)
        {
            NiFree(ppuiNewToOld[uiRemapIdx]);
        }
    }
    NiFree(ppuiNewToOld);
}
//---------------------------------------------------------------------------
int NiMAXMorphConverter::Convert(
    INode *pMaxNode,
    NiNode *pCurNode,
    ScaleInfo *pParentScale)
{
    CHECK_MEMORY();
    Object *pObjRef;
    Class_ID cID2;
    int iStatus;
    NiAVObject *pBase = NULL;
    NiAVObject **ppTargets = NULL;
    NiPoint3 **ppOrigVerts = NULL;
    unsigned int uiNumTargets;
    MorphObject *pMorph;
    Quat q;
    Point3 p;
    ScaleInfo scaleInfo, *pScaleInfo;
    NiAVObject *pMorphee;

    unsigned int *aNumKeys;
    unsigned int i, j;
    NiFloatKey **apMorphKeys;

    uiNumTargets = 0;

    pObjRef = pMaxNode->GetObjectRef();
    cID2 = pObjRef->ClassID();

    Modifier *pMorpher = FindMorpherModifier(pObjRef);
    if (pMorpher)
    {
        ConvertMorpher((MorphR3*) pMorpher, pMaxNode, 
                       pCurNode, pParentScale);
        return(W3D_STAT_OK);
    }

    pMorph = NULL;
    if (cID2 == Class_ID(DERIVOB_CLASS_ID, 0))
    {
        IDerivedObject *pDObj;
        Object *pReffed;
        Class_ID cID3;
        
        pDObj = (IDerivedObject *) pObjRef;
        pReffed = pDObj->GetObjRef();
        cID3 = pReffed->ClassID();

        if (cID3 == Class_ID(MORPHOBJ_CLASS_ID, 0))
            pMorph = (MorphObject*) pReffed;
    }
    else if (cID2 == Class_ID(MORPHOBJ_CLASS_ID, 0))
        pMorph = (MorphObject*) pObjRef;

    if (!pMorph)
        return(0);

    NiMAXHierConverter::GetOOTM(pMaxNode, &p, &q, pParentScale, &scaleInfo);

    if (!pMorph->TestAFlag(A_WORK1))
        SetFlags(pMorph, ND_SET);
    else
    {
        NiAVObject *pResult;

        // instanced node
        pResult = ms_pkMorphHTab->Retrieve(pMorph, &scaleInfo);
        if (pResult)
        {
            NiAVObject* pkClonedObj = (NiAVObject*)pResult->Clone();
            Matrix3 kMaxRot;
            NiMatrix3 kNiRot;

            q.MakeMatrix(kMaxRot);
            MaxToNI(kMaxRot, kNiRot);
            pkClonedObj->SetRotate(kNiRot);
            pkClonedObj->SetTranslate(p.x, p.y, p.z);
            pkClonedObj->SetScale(scaleInfo.scale.s.x);
            pCurNode->AttachChild(pkClonedObj);
            return(W3D_STAT_OK);
        }
    }

    MorphControl *pMorphCont = pMorph->morphCont;
    Class_ID cID5 = pMorphCont->ClassID();
    
    uiNumTargets = pMorphCont->NumMorphTargs();
    ppTargets = NiAlloc(NiAVObject*, uiNumTargets);
    NIASSERT(ppTargets);

    ppOrigVerts = NiAlloc(NiPoint3*, uiNumTargets);
    NIASSERT(ppOrigVerts);
    memset(ppOrigVerts, 0, uiNumTargets * sizeof(NiPoint3*));

    apMorphKeys = NiAlloc(NiFloatKey*, uiNumTargets);
    NIASSERT(apMorphKeys);

    aNumKeys = NiAlloc(unsigned int, uiNumTargets);
    NIASSERT(aNumKeys);
    memset(aNumKeys, 0, uiNumTargets * sizeof(unsigned int));

    for (i = 0; i < uiNumTargets; i++)
    {
        apMorphKeys[i] = NULL;

        if (cID5 == Class_ID(CUBICMORPHCONT_CLASS_ID, 0))
        {
            CubicMorphCont *pCubic = (CubicMorphCont*)pMorphCont;
            CubicMorphKey key;

            unsigned int uiNumKeys = pCubic->keys.Count();

            if (uiNumKeys)
            {
                NiTCBFloatKey* pFloatKeys = NiNew NiTCBFloatKey[uiNumKeys];

                for (j = 0; j < uiNumKeys; j++)
                {
                    key = pCubic->keys[j];

                    float fWeight = 
                        pCubic->keys[j].targ == (int)i ? 1.0f : 0.0f;

                    pFloatKeys[j].SetTime(key.time * SECONDSPERTICK);
                    pFloatKeys[j].SetValue(fWeight);
                    pFloatKeys[j].SetTension(key.tens);
                    pFloatKeys[j].SetContinuity(key.cont);
                    pFloatKeys[j].SetBias(key.bias);
                }
                apMorphKeys[i] = pFloatKeys;
                aNumKeys[i] = uiNumKeys;

            }
        }
        else if (cID5 == Class_ID(BARYMORPHCONT_CLASS_ID, 0))
        {
            BaryMorphCont *pBary = (BaryMorphCont*)pMorphCont;
            BaryMorphKey key;

            unsigned int uiNumKeys = pBary->keys.Count();

            if (uiNumKeys)
            {
                NiTCBFloatKey* pFloatKeys = NiNew NiTCBFloatKey[uiNumKeys];

                for (j = 0; j < uiNumKeys; j++)
                {
                    key = pBary->keys[j];

                    float fWeight = 0.0f;
                    for (int k = 0; k < key.wts->Count(); k++)
                    {
                        if ((*key.wts)[k].targ == (int)i)
                        {
                            fWeight = (*key.wts)[k].percent;
                            break;
                        }
                    }

                    pFloatKeys[j].SetTime(key.time * SECONDSPERTICK);
                    pFloatKeys[j].SetValue(fWeight);
                    pFloatKeys[j].SetTension(key.tens);
                    pFloatKeys[j].SetContinuity(key.cont);
                    pFloatKeys[j].SetBias(key.bias);
                }
                apMorphKeys[i] = pFloatKeys;
                aNumKeys[i] = uiNumKeys;

            }
        }
    }

    unsigned int uiTargetNumVerts, uiMasterNumVerts=0;
    unsigned short ** ppRemapping = NULL;
    unsigned int uiNumRemapTargets = 0;
    NiUInt32** ppuiNewToOld = NULL;
    NiAVObject** ppRemapTargets = NULL;

    for (i = 0; i < uiNumTargets; i++)
    {
        ObjectState kOState = pMorphCont->GetMorphTarg(i)->Eval(m_animStart);
        Object* pkTargetEval = kOState.obj;

        if (pkTargetEval != NULL)
        {
            SClass_ID sID;

            sID = pkTargetEval->SuperClassID(); 

            if (sID == GEOMOBJECT_CLASS_ID)
            {

                {
                    NiMAXMeshConverter mesh(m_animStart, m_animEnd);

                    mesh.GetVerts(pkTargetEval, pMaxNode, pParentScale,
                                  uiTargetNumVerts, ppOrigVerts[i]);

                    if (i == 0)
                    {
                        NiMAXUV* pkNiMAXUV = NULL;
                        iStatus = mesh.ConvertGeomSub(pkTargetEval, pMaxNode, 
                            pCurNode, &pBase, true, false, pParentScale, 
                            true, ppRemapping, ppRemapTargets,
                            uiNumRemapTargets, pkNiMAXUV, ppuiNewToOld);
                        if (pkNiMAXUV && pBase)
                        {
                            NiDelete pkNiMAXUV;
                        }
                        // if morphing a multi-material mesh, exit
                        if (uiNumRemapTargets > 1)
                        {
                            NiFree(ppRemapTargets);
                            for (unsigned int ui = 0; ui < uiNumRemapTargets; 
                                ui++)
                            {
                                NiFree(ppRemapping[ui]);
                                NiFree(ppuiNewToOld[ui]);
                            }
                            NiFree(ppRemapping);
                            NiFree(ppuiNewToOld);
                            NiFree(aNumKeys);
                            NiFree(ppTargets);
                            for (unsigned int ui = 0; ui < uiNumTargets; ui++)
                            {
                                NiDelete[] apMorphKeys[ui];
                                NiDelete[] ppOrigVerts[ui];
                            }
                            NiFree(apMorphKeys);
                            NiFree(ppOrigVerts);

                            NILOGERROR("Failed to convert Morph modifier. \n"
                                "Multiple materials not supported on Morphs.");
                            return(W3D_STAT_OK);
                        }
                        uiMasterNumVerts = uiTargetNumVerts;
                    }

                }

                NIASSERT(uiMasterNumVerts == uiTargetNumVerts);
            }
        }
    }

    pMorphee = pBase;

    NiMesh* pkMesh = NiDynamicCast(NiMesh, pMorphee);

    NiSkinningMeshModifier* pkSkinningModifier =
        NiGetModifier(NiSkinningMeshModifier, pkMesh);

    bool bHasSkinModifier = false;
    if(pkSkinningModifier)
        bHasSkinModifier = true;

    NiFixedString kMorphDestSemantic = bHasSkinModifier ? 
        NiCommonSemantics::POSITION_BP() : NiCommonSemantics::POSITION();

    NiFixedString kNormSemantic = bHasSkinModifier?
        NiCommonSemantics::NORMAL_BP() : NiCommonSemantics::NORMAL();

    NiFixedString kMorphTargetSemantic = 
        NiMorphMeshModifier::MorphElementSemantic(kMorphDestSemantic);

    uiMasterNumVerts = pkMesh->GetVertexCount();

    // remap the target vertices to the geometry we finished with
    for (i = 0; i < uiNumTargets; i++)
    {
            NiDataStreamElementLock kPositionLock = pkMesh->AddStreamGetLock(
            kMorphTargetSemantic,
            i,
            NiDataStreamElement::F_FLOAT32_3,
            uiMasterNumVerts, 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX,
            true);

        NiTStridedRandomAccessIterator<NiPoint3> kPositionIterator =
            kPositionLock.begin<NiPoint3>(0); 

        for (unsigned int j = 0; j < uiMasterNumVerts; j++)
        {
            // Multi-material morph not supported, so only use index 0 of
            // the remapping arrays
            NiUInt32 uiNewToOldIdx=ppuiNewToOld[0][j];
            kPositionIterator[j] = 
                ppOrigVerts[i][ppRemapping[0][uiNewToOldIdx]];
        }

        kPositionLock.Unlock();
    }

    NiFree(ppRemapTargets);
    NiFree(ppRemapping[0]);
    NiFree(ppRemapping);
    for (unsigned int ui = 0; ui < uiNumRemapTargets; ui++)
    {
        NiFree(ppuiNewToOld[ui]);
    }
    NiFree(ppuiNewToOld);

    bool bUpdateNormals = false;
    TSTR pcPropBuffer;
    pMaxNode->GetUserPropBuffer(pcPropBuffer);
    if (strstr(pcPropBuffer, "NiMorphUpdateNormals"))
    {
        bUpdateNormals = true;
    }

    // Create the mesh modifier
    NiMorphMeshModifier* pkModifier =
        NiNew NiMorphMeshModifier((NiUInt16)uiNumTargets);

    // Precalculate the derived value information needed for interpolation
    NiFloatKey::FillDerivedValsFunction pDerived;

    pDerived = NiFloatKey::GetFillDerivedFunction(NiFloatKey::TCBKEY);
    NIASSERT(pDerived);

    unsigned char ucSize = NiFloatKey::GetKeySize(NiFloatKey::TCBKEY);

    NiFloatInterpolator** apMorphInterps = 
        NiAlloc(NiFloatInterpolator*, uiNumTargets);

    NiMorphWeightsController *pkNIWeightsMorpher;

    pkNIWeightsMorpher = NiNew NiMorphWeightsController (uiNumTargets);
    NIASSERT(pkNIWeightsMorpher);

    for (i = 0; i < uiNumTargets; i++)
    {
        if (aNumKeys[i])
        {
            (*pDerived)((NiAnimationKey*) apMorphKeys[i], aNumKeys[i], 
                ucSize);
        }

        apMorphInterps[i] = NiNew NiFloatInterpolator;
        apMorphInterps[i]->SetKeys(apMorphKeys[i], aNumKeys[i], 
            NiFloatKey::TCBKEY);

        TSTR strMorphName;
        pMorphCont->GetMorphTargName(i, strMorphName);

        pkNIWeightsMorpher->SetTargetName((NiUInt16)i, 
            (const char*)strMorphName);
    }

    // Add the weights stream

    NiDataStreamElementLock kWeightLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::MORPHWEIGHTS(),
        0,
        NiDataStreamElement::F_FLOAT32_1,
        uiNumTargets, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_USER,
        true);

    NiTStridedRandomAccessIterator<float> kWeightIterator =
        kWeightLock.begin<float>(0); 

    kWeightIterator[0] = 1.0f;
    for (unsigned int ui = 1; ui < uiNumTargets; ui++)
        kWeightIterator[ui] = 0.0f;

    kWeightLock.Unlock();

    for (i = 0; i < uiNumTargets; i++)
    {
        pkNIWeightsMorpher->SetInterpolator(apMorphInterps[i],
            (unsigned short)i);
    }
    NiFree(apMorphInterps);

    pkNIWeightsMorpher->ResetTimeExtrema();

    if (NiMAXConverter::GetUseAppTime())
        pkNIWeightsMorpher->SetAnimType(NiTimeController::APP_TIME);
    else
        pkNIWeightsMorpher->SetAnimType(NiTimeController::APP_INIT);

    if (bUpdateNormals)
    {
        NiMeshTools::GenerateNormalSharing(pkMesh,
            kMorphDestSemantic, 0, kNormSemantic, 0);
    }

    // Set flags for skinning and normals
    pkModifier->SetUpdateNormals(bUpdateNormals);
    pkModifier->SetRelativeTargets(false);
    pkModifier->SetNeedsUpdate();
    pkModifier->SetSkinned(bHasSkinModifier);
    if(bHasSkinModifier)
    {
        pkModifier->SetSWSkinned(pkSkinningModifier->GetSoftwareSkinned());
    }
    else
    {
        pkModifier->SetSWSkinned(false);
    }

    // Set the access mask on the output data. It needs to be mutable so
    // that it can be morphed but not necessarily on every frame.        
    NiDataStreamRef* pkRef = pkMesh->FindStreamRef(kMorphDestSemantic);

    NIASSERT(pkRef);
    NiDataStream* pkDS = pkRef->GetDataStream();

    NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
    NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
    pkTDS->MakeWritable();
    pkTDS->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);

    if (bUpdateNormals)
    {
        pkRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
        pkDS = pkRef->GetDataStream();
        NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
        NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
        pkTDS->MakeReadable();

        pkRef = pkMesh->FindStreamRef(kNormSemantic);
        pkDS = pkRef->GetDataStream();
        NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
        pkTDS = (NiToolDataStream*)pkDS;
        pkTDS->MakeWritable();
        pkTDS->SetCloningBehavior(NiObject::CLONE_BLANK_COPY);
    }

    // Specify which semantics it is morphing
    pkModifier->AddMorphedElement(kMorphDestSemantic, 0);

    // Add it to the mesh as the first modifier to guarantee it runs before 
    // the NiSkinningMeshModifier.
    pkMesh->AddModifierAt(0, pkModifier, false);

    pkNIWeightsMorpher->SetTarget(pkMesh);

    pkModifier->CalculateMorphBound(pkMesh);

    NiMAXAnimationConverter::SetORT(pMorphCont,pkNIWeightsMorpher);

    pkNIWeightsMorpher->Update(0.0);
    pkMesh->Update(0.0);

    // finish up the creation of the NI object
    pCurNode->AttachChild(pMorphee);

    pScaleInfo = NiNew ScaleInfo;
    *pScaleInfo = scaleInfo;

    ms_pkMorphHTab->Insert(pMorph, pScaleInfo, pMorphee);

    NiFree(apMorphKeys);
    NiFree(aNumKeys);
    NiFree(ppTargets);

    for (i = 0; i < uiNumTargets; i++)
        NiDelete[] ppOrigVerts[i];
    NiFree(ppOrigVerts);

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
void NiMAXMorphConverter::Init()
{
    CHECK_MEMORY();
    NIASSERT(!ms_pkMorphHTab);
    ms_pkMorphHTab = NiNew NiTHash<NiAVObjectPtr>(113, 
        PFMorphHashEq, PFMorphHashFree);
    
    NIASSERT(ms_pkMorphHTab->GetNumHashed() == 0);
}
//---------------------------------------------------------------------------
void NiMAXMorphConverter::Shutdown()
{
    CHECK_MEMORY();
    // cleanup the morph hash table
    NiDelete ms_pkMorphHTab;
    ms_pkMorphHTab = NULL;
}
//---------------------------------------------------------------------------
