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
#include "NiMAXHierConverter.h"
#include "NiMAXAnimationConverter.h"
#include "NiMAXLODConverter.h"
#include "NiMAXCameraConverter.h"
#include "NiMAXLightConverter.h"
#include "NiMAXMorphConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiMAXPhysicsConverter.h"
#include "NiMAXShapeConverter.h"
#include "NiUtils.h"
#include "NiMAXCustAttribConverter.h"
#ifndef NO_PARTICLES
#include "NiMAXPSysConverter.h"
#endif

void* NiMAXHierConverter::m_pUserData = NULL;

//---------------------------------------------------------------------------
bool PFNodeHashEq(const void* pSearch, const void* p, const void* p2Search, 
    const void* p2);
void PFNodeHashFree(const void* p2, NiNodePtr& pspNode);
NiString* NiMAXHierConverter::ms_pstrCurrentName = NULL;
NiTHash<NiNodePtr>*  NiMAXHierConverter::ms_pkNodeHTab = NULL;
NiTPointerList<INode *> NiMAXHierConverter::ms_kSelectedNodes;
//---------------------------------------------------------------------------
void NiMAXHierConverter::AddSelectedObjectsRecursive(
    INode* pNode, 
    bool& bAddedNode)
{
    if (!pNode)
    {
        return;
    }

    bool bAddMe = false;
    if (pNode->Selected() != 0)
    {
        bAddMe = true;
    }

    for (int i = 0; i < pNode->NumberOfChildren(); i++)
    {
        bool bAddedMyChild = false;
        AddSelectedObjectsRecursive(pNode->GetChildNode(i), bAddedMyChild);
        if (bAddedMyChild)
            bAddMe = true;
    }

    if (bAddMe)
    {
        ms_kSelectedNodes.AddHead(pNode);
        bAddedNode = true;
    }
}
//---------------------------------------------------------------------------
void NiMAXHierConverter::Preprocess(INode* pNode)
{
    ms_kSelectedNodes.RemoveAll();

    if (NiMAXConverter::GetExportSelected())
    {
        bool bAddedChild = false;
        // The root node may be added twice using this logic, but in the 
        // off chance that nothing is selected, we still need the root node.
        ms_kSelectedNodes.AddHead(pNode);
        AddSelectedObjectsRecursive(pNode, bAddedChild);
    }
}
//---------------------------------------------------------------------------
void NiMAXHierConverter::Postprocess(INode*)
{
    ms_kSelectedNodes.RemoveAll();
}
//---------------------------------------------------------------------------
NiMAXHierConverter::NiMAXHierConverter(
    TimeValue animStart,
    TimeValue animEnd)
{
    m_animStart = animStart;
    m_animEnd = animEnd;
}
//---------------------------------------------------------------------------
bool PFNodeHashEq(
    const void* pSearch, 
    const void* p,
    const void* p2Search, 
    const void* p2)
{
    if (pSearch != p)
        return(false);

    ScaleInfo* pScaleSearch = (ScaleInfo *) p2Search;
    ScaleInfo* pScale = (ScaleInfo *) p2;

    if (pScaleSearch != NULL)
    {
        if (pScale->bIsIdent && pScaleSearch->bIsIdent)
            return(true);

        // if the scales & rots are the same
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
void PFNodeHashFree(const void* p2, NiNodePtr& spNode)
{
    spNode = NULL;
    NiDelete ((ScaleInfo *) p2);
}
//---------------------------------------------------------------------------
// Get 3DSMAX's Object-Offset Transformation Matrix. Factor it into
// pScOut (unless it is the identity).
void NiMAXHierConverter::GetOOTM(
    INode* pMaxNode,
    Point3* pP,
    Quat* pQ, 
    const ScaleInfo* pScIn,
    ScaleInfo* pScOut)
{ 
    pP->x = pP->y = pP->z = 0.0f;
    pQ->Identity();
    *pScOut = *pScIn;

    INode *pParent = pMaxNode->GetParentNode();
    if (pParent)
    {
        ScaleValue s;
        Point3 p;
        Quat q;

        p = pMaxNode->GetObjOffsetPos();
        q = pMaxNode->GetObjOffsetRot();
        s = pMaxNode->GetObjOffsetScale();

        if (p.x == 0.0f && p.y == 0.0f && p.z == 0.0f &&
            q.x == 0.0f && q.y == 0.0f && q.z == 0.0f && q.w == 1.0f &&
            s.q.x == 0.0f && s.q.y == 0.0f && s.q.z == 0.0f && 
            s.q.w == 1.0f &&
            s.s.x == 1.0f && s.s.y == 1.0f && s.s.z == 1.0f)
        {
            return;
        }
        else
        {
            Matrix3 ptm, rtm, srtm, stm, NiNewtm, srtminv;
            AffineParts parts;
            Quat sqinv;
            
            sqinv = Inverse(s.q);
            ptm = TransMatrix(p);
            stm = ScaleMatrix(s.s);
            q.MakeMatrix(rtm);
            s.q.MakeMatrix(srtm);
            sqinv.MakeMatrix(srtminv);
            
            NiNewtm = srtm * stm * srtminv * rtm * ptm;
            
            stm = ScaleMatrix(pScIn->scale.s);
            pScIn->scale.q.MakeMatrix(srtm);
            sqinv = Inverse(pScIn->scale.q);
            sqinv.MakeMatrix(srtminv);
            NiNewtm = NiNewtm * srtm * stm * srtminv;
            
            decomp_affine(NiNewtm, &parts);
            
            *pQ = parts.q;
            *pP = parts.t;
            ScaleValue& sc = pScOut->scale;
            sc.s = parts.k;
            sc.q = parts.u;
            if (parts.f < 0.0f)
                sc.s = -sc.s;

            if (NiOptimize::CloseTo(sc.s.x, 1.0f) &&
                NiOptimize::CloseTo(sc.q.x, 0.0f) &&
                NiOptimize::CloseTo(sc.s.y, 1.0f) &&
                NiOptimize::CloseTo(sc.s.z, 1.0f) &&
                NiOptimize::CloseTo(sc.q.y, 0.0f) &&
                NiOptimize::CloseTo(sc.q.z, 0.0f) &&
                NiOptimize::CloseTo(sc.q.w, 1.0f))
            {
                pScOut->bIsIdent = true;
            }
            else 
            {
                pScOut->bIsIdent = false;
            }
        }
    }
}
//---------------------------------------------------------------------------
Matrix3 NiMAXHierConverter::UniformMatrix(Matrix3 orig_cur_mat)
{
    AffineParts   parts;  
    Matrix3       mat;   
     
    // Remove scaling from orig_cur_mat
    // 1) Decompose original and get decomposition info
    decomp_affine(orig_cur_mat, &parts); 
     
    // 2) construct 3x3 rotation from quaternion parts.q
    parts.q.MakeMatrix(mat);
     
    // 3) construct position row from translation parts.t  
    mat.SetRow(3, parts.t);
     
    return mat;
}

//---------------------------------------------------------------------------
//
// This implementation of NfGetLocalTM removes all scales from the returned
// Matrix3, but also stores them in the parameter pScaleOut (if specified)
// so that they can be pushed down to the geometry level.
//
Matrix3 NiMAXHierConverter::GetLocalTM(
    INode* pMaxNode,
    TimeValue t,
    const ScaleInfo* pScaleIn,
    ScaleInfo* pScaleOut)
{
    Matrix3 localScaleTM;
    INode* pParent = pMaxNode->GetParentNode();

    if (pParent)
    {
        Matrix3 parentTM = pParent->GetNodeTM(t);
        Matrix3 thisTM = pMaxNode->GetNodeTM(t);

        localScaleTM = thisTM * Inverse(parentTM);
    }
    else
        localScaleTM.IdentityMatrix();

    Matrix3     srtm, stm, srtminv;
    stm = ScaleMatrix(pScaleIn->scale.s);
    pScaleIn->scale.q.MakeMatrix(srtm);

    Quat        qinv;        
    qinv = Inverse(pScaleIn->scale.q);
    qinv.MakeMatrix(srtminv);

    localScaleTM = localScaleTM * srtm * stm * srtminv;

    Matrix3 localNoScaleTM;
    localNoScaleTM = UniformMatrix(localScaleTM);

    if (pScaleOut)
    {
        AffineParts parts;

        decomp_affine(localScaleTM, &parts);
        ScaleValue& sc = pScaleOut->scale;
        sc.q = parts.u;
        sc.s = parts.k;
        if (parts.f < 0.0f)
            sc.s = -sc.s;
        if (NiOptimize::CloseTo(sc.s.x, 1.0f) &&
            NiOptimize::CloseTo(sc.s.y, 1.0f) &&
            NiOptimize::CloseTo(sc.s.z, 1.0f) &&
            NiOptimize::CloseTo(sc.q.x, 0.0f) &&
            NiOptimize::CloseTo(sc.q.y, 0.0f) &&
            NiOptimize::CloseTo(sc.q.z, 0.0f) &&
            NiOptimize::CloseTo(sc.q.w, 1.0f))
            pScaleOut->bIsIdent = true;
        else
            pScaleOut->bIsIdent = false;
    }

    return localNoScaleTM;
}
//---------------------------------------------------------------------------

#define NfDeterminant2(a,b,c,d) ((a)*(d) - (c)*(b))

#define NfDeterminant3(a,b,c,d,e,f,g,h,i)\
    ((a)*NfDeterminant2((e),(f),(h),(i)) - \
     (b)*NfDeterminant2((d),(f),(g),(i)) + \
     (c)*NfDeterminant2((d),(e),(g),(h)))
                                              
float NiMAXHierConverter::GetLocalUScale(
    INode* pMaxNode,
    TimeValue t,
    const ScaleInfo* pScaleIn)
{
    Matrix3 localScaleTM;
    Matrix3     srtm, stm, srtminv;
    Quat        qinv;        
    float       fDet;

    if (INode* pParent = pMaxNode->GetParentNode())
    {
        Matrix3 parentTM = pParent->GetNodeTM(t);
        Matrix3 thisTM = pMaxNode->GetNodeTM(t);

        localScaleTM = thisTM * Inverse(parentTM);
    }
    else
        localScaleTM.IdentityMatrix();

    stm = ScaleMatrix(pScaleIn->scale.s);
    pScaleIn->scale.q.MakeMatrix(srtm);
    qinv = Inverse(pScaleIn->scale.q);
    qinv.MakeMatrix(srtminv);

    localScaleTM = localScaleTM * srtm * stm * srtminv;

    Point3 m[3];

    m[0] = localScaleTM.GetRow(0);
    m[1] = localScaleTM.GetRow(1);
    m[2] = localScaleTM.GetRow(2);
    fDet = NfDeterminant3(m[0].x, m[0].y, m[0].z,
                          m[1].x, m[1].y, m[1].z,
                          m[2].x, m[2].y, m[2].z);
    if (fDet < 0)
        fDet = (float) pow(-fDet, 0.33333333f);
    else
        fDet = (float) pow(fDet, 0.33333333f);
    return(fDet);
}
//---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4239)
NiMatrix3* NfScaleFactorsMatrix(INode* pMaxNode, TimeValue t)
{
    AffineParts parts;
    Matrix3 stm;
    Matrix3 srtm;
    NiMatrix3* pNiMat = NiNew NiMatrix3;

    decomp_affine(pMaxNode->GetNodeTM(t), &parts);

    if (parts.f < 0.0f)
        stm = ScaleMatrix(-parts.k);
    else
        stm = ScaleMatrix(parts.k);
    parts.u.MakeMatrix(srtm);

    MaxToNI((srtm * stm * Inverse(srtm)), *pNiMat);
    return pNiMat;
}
#pragma warning(pop)

//---------------------------------------------------------------------------
int NiMAXHierConverter::ConvertXform(
    INode* pMaxNode,
    NiAVObject* pAVObj, 
    const ScaleInfo* pScIn,
    ScaleInfo* pScOut)
{
    Matrix3 localTM = GetLocalTM(pMaxNode, m_animStart, pScIn, pScOut);

    AffineParts parts;
    decomp_affine(localTM, &parts);
    
    Matrix3 rot;
    NiMatrix3 niRot;
    parts.q.MakeMatrix(rot);
    MaxToNI(rot, niRot);

    pAVObj->SetRotate(niRot);
    pAVObj->SetTranslate(parts.t.x, parts.t.y, parts.t.z);
    
    return W3D_STAT_OK;
}

//---------------------------------------------------------------------------
NiNode* NiMAXHierConverter::Lookup(INode* pNode, ScaleInfo* pParentScale)
{
    return ((NiNode *) ms_pkNodeHTab->Retrieve(pNode, pParentScale));
}

//---------------------------------------------------------------------------
static int BillboardReplaceNode(
    NiNodePtr* pspNode, 
    NiBillboardNode::FaceMode eType)
{
    CHECK_MEMORY();
    NiBillboardNode* pNewNode = NiNew NiBillboardNode;
    if (pNewNode == NULL)
        return(W3D_STAT_NO_MEMORY);

    pNewNode->SetMode(eType);

    pNewNode->SetName((*pspNode)->GetName());

    for (unsigned int i = 0; i < (*pspNode)->GetArrayCount(); i++)
    {
        NiAVObjectPtr spChild = (*pspNode)->DetachChildAt(i);
        if (spChild)
            pNewNode->AttachChild(spChild);
    }

    NiPropertyList* pPropList = &(*pspNode)->GetPropertyList();
    NiTListIterator kIter = pPropList->GetHeadPos();
    while (kIter)
    {
        NiPropertyPtr spProp = pPropList->GetNext(kIter);
        if (spProp)
            pNewNode->AttachProperty(spProp);
    }

    pNewNode->SetTranslate((*pspNode)->GetTranslate());
    pNewNode->SetRotate((*pspNode)->GetRotate());
    pNewNode->SetScale((*pspNode)->GetScale());

    // Move Controllers to control the new BillboardNode
    NiTimeControllerPtr spControl;
    while (spControl = (*pspNode)->GetControllers())
        spControl->SetTarget(pNewNode);

    //Propagate extra data to new NiBillboardNode
    for (unsigned int j=0; j < (*pspNode)->GetExtraDataSize(); j++)
    {
        NiExtraData* pkExtraData = (*pspNode)->GetExtraDataAt((unsigned short)j);
        if (pkExtraData)
        {
            pNewNode->AddExtraData(pkExtraData);
        }
    }

    *pspNode = pNewNode;
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
static int SwitchReplaceNode(
    NiNodePtr* pspNode,
    unsigned int uiWhichChild)
{
    CHECK_MEMORY();
    NiSwitchNode* pNewNode = NiNew NiSwitchNode;
    if (pNewNode == NULL)
        return(W3D_STAT_NO_MEMORY);

    pNewNode->SetName((*pspNode)->GetName());

    for (unsigned int i = 0; i < (*pspNode)->GetArrayCount(); i++)
    {
        NiAVObjectPtr spChild = (*pspNode)->DetachChildAt(i);
        if (spChild)
            pNewNode->AttachChild(spChild);
    }

    NiPropertyList* pPropList = &(*pspNode)->GetPropertyList();
    NiTListIterator kIter = pPropList->GetHeadPos();
    while (kIter)
    {
        NiPropertyPtr spProp = pPropList->GetNext(kIter);
        if (spProp)
            pNewNode->AttachProperty(spProp);
    }

    pNewNode->SetTranslate((*pspNode)->GetTranslate());
    pNewNode->SetRotate((*pspNode)->GetRotate());
    pNewNode->SetScale((*pspNode)->GetScale());

    // Move Controllers to control the new SwitchNode
    NiTimeControllerPtr spControl;
    while (spControl = (*pspNode)->GetControllers())
        spControl->SetTarget(pNewNode);

    //Propagate extra data to new NiSwitchNode
    for (unsigned int j=0; j < (*pspNode)->GetExtraDataSize(); j++)
    {
        NiExtraData* pkExtraData = (*pspNode)->GetExtraDataAt((unsigned short)j);
        if (pkExtraData)
        {
            pNewNode->AddExtraData(pkExtraData);
        }
    }

    pNewNode->SetIndex(uiWhichChild);
    *pspNode = pNewNode;
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
static int SortAdjustReplaceNode(
    NiNodePtr* pspNode, 
    NiSortAdjustNode::SortingMode eType)
{
    CHECK_MEMORY();
    NiSortAdjustNode* pNewNode = NiNew NiSortAdjustNode;
    if (pNewNode == NULL)
        return(W3D_STAT_NO_MEMORY);

    pNewNode->SetSortingMode(eType);
    pNewNode->SetName((*pspNode)->GetName());

    for (unsigned int i = 0; i < (*pspNode)->GetArrayCount(); i++)
    {
        NiAVObjectPtr spChild = (*pspNode)->DetachChildAt(i);
        if (spChild)
            pNewNode->AttachChild(spChild);
    }

    NiPropertyList* pPropList = &(*pspNode)->GetPropertyList();
    NiTListIterator kIter = pPropList->GetHeadPos();
    while (kIter)
    {
        NiPropertyPtr spProp = pPropList->GetNext(kIter);
        if (spProp)
            pNewNode->AttachProperty(spProp);
    }

    pNewNode->SetTranslate((*pspNode)->GetTranslate());
    pNewNode->SetRotate((*pspNode)->GetRotate());
    pNewNode->SetScale((*pspNode)->GetScale());

    // Move Controllers to control the new SortAdjustNode
    NiTimeControllerPtr spControl;
    while (spControl = (*pspNode)->GetControllers())
        spControl->SetTarget(pNewNode);

    //Propagate extra data to new NiSortAdjustNode
    for (unsigned int j=0; j < (*pspNode)->GetExtraDataSize(); j++)
    {
        NiExtraData* pkExtraData = (*pspNode)->GetExtraDataAt((unsigned short)j);
        if (pkExtraData)
        {
            pNewNode->AddExtraData(pkExtraData);
        }
    }

    *pspNode = pNewNode;
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
// Nodes are exported if:
//      * The mode is export selected and they are selected
//      * The mode is export selected and one of their children is selected
//      * The node does NOT have the "NiNoExport" user property
//---------------------------------------------------------------------------
bool NiMAXHierConverter::ExportThisNode(INode* pMaxNode)
{      
    if (!pMaxNode)
        return false;

    if (NiMAXConverter::GetExportSelected() && !IsNodeSelected(pMaxNode))
        return false;
    
    if (pMaxNode->UserPropExists("NiNoExport"))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
// Objects are NOT exported if:
//      * The mode is export selected and their parent node is NOT selected
//---------------------------------------------------------------------------
bool NiMAXHierConverter::ExportObjectOwnedByNode(INode* pMaxNode, Object*)
{
    if (!pMaxNode)
        return false;

    if (NiMAXConverter::GetExportSelected() && pMaxNode->Selected() == 0)
        return false;
   
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXHierConverter::IsNodeSelected(INode* pMaxNode)
{
    if (!pMaxNode)
        return false;

    if (pMaxNode->Selected() != 0)
        return true;
    
    if (ms_kSelectedNodes.FindPos(pMaxNode) != NULL)
        return true;
    
    return false;
}
//---------------------------------------------------------------------------
// Take a MAX INode and create a corresponding NI node.
int NiMAXHierConverter::BuildNode(
    Interface* pIntf,
    INode* pMaxNode,
    NiNodePtr* pspResult,
    ScaleInfo* pParentScale,
    bool bIgnoreHiddenFlag,
    bool bIgnoreVisAnim)
{
    CHECK_MEMORY();

    // Check to see if this Node should be exported.
    if (pMaxNode && !ExportThisNode(pMaxNode))
        return W3D_STAT_OK;

    //Update the SceneGraph creation progress UI
    char* pcString = NiAlloc(char, 256);
    NiSprintf(pcString, 256, "%s", pMaxNode->GetName());
    NIASSERT(ms_pstrCurrentName);
    *ms_pstrCurrentName = pMaxNode->GetName();
    NiMAXConverter::GetDialog()->SetLineTwo(pcString);
#ifdef NIDEBUG
    NiSprintf(pcString, 256, "Current Object: %s\n", pMaxNode->GetName());
    NiOutputDebugString(pcString);
#endif
    NiFree(pcString);

    int i, iStatus;

    *pspResult = (NiNode *) ms_pkNodeHTab->Retrieve(pMaxNode, pParentScale);
    if (*pspResult)
    {
        //We have already seen this node elsewhere - so instance it
        *pspResult = (NiNode*) (*pspResult)->Clone();
        return(W3D_STAT_OK);
    }

    NiNode* pkNiNode = NiNew NiNode;

    if (pkNiNode == NULL)
        return(W3D_STAT_NO_MEMORY);

    // export user defined properties
    TSTR pcPropBuf;
    pMaxNode->GetUserPropBuffer(pcPropBuf);

    unsigned int uiBufLen = (unsigned int) _tcslen(pcPropBuf);

    if (uiBufLen > 0)
    {
        NiStringExtraData* pStringExtra = 
            NiNew NiStringExtraData((const char*)pcPropBuf);
        NIASSERT(pStringExtra);
        pkNiNode->AddExtraData("UserPropBuffer", pStringExtra);
    }

    NiMAXCustAttribConverter kAttribConverter(m_animStart, m_animEnd);

    // set the node's name
    pkNiNode->SetName(pMaxNode->GetName());

    NiTextKeyExtraData* pkNoteTrackExtraData = 
        NiMAXAnimationConverter::BuildNoteTrack(pMaxNode);

    if (pkNoteTrackExtraData)
        pkNiNode->AddExtraData(pkNoteTrackExtraData);

    // old Insert location

    *pspResult = pkNiNode;

    ObjectState kOState = pMaxNode->EvalWorldState(m_animStart);
    Object* pObj = kOState.obj;

    // Check to see if this object should be exported
    if (pObj && !ExportObjectOwnedByNode(pMaxNode, pObj))
    {
        pObj = NULL;
    }

    ScaleInfo scaleInfo;
    iStatus = ConvertXform(pMaxNode, pkNiNode, pParentScale, &scaleInfo);
    if (iStatus)
        return(iStatus);

    if (NiMAXLODConverter::IsLODNode(pMaxNode))
    {
        NiMAXLODConverter lod(m_animStart, m_animEnd);

        iStatus = lod.Convert(pIntf, pkNiNode, pMaxNode, &scaleInfo, 
            bIgnoreHiddenFlag ? false : 
            (pMaxNode->IsNodeHidden() ? true : false));
        if (iStatus)
            return(iStatus);

        NiMAXAnimationConverter anim(m_animStart, m_animEnd);

        iStatus = anim.Convert(pkNiNode, pMaxNode,
            pParentScale, bIgnoreVisAnim);
        if (iStatus)
            return(iStatus);
    
        goto exit;
    }
    
    // Deal with Physics stuff that must be done before children are processed.
    NiMAXPhysicsConverter::
        PreChildrenProcess(pMaxNode, pkNiNode, pObj, m_animStart);

    NiMAXConverter::GetDialog()->StepIt();
    for (i = 0; i < pMaxNode->NumberOfChildren(); i++)
    {
        INode *pMaxChild;
        char *pChildName;
        
        pMaxChild = pMaxNode->GetChildNode(i);

        pChildName = pMaxChild->GetName();

        NiNodePtr spNiChild;
        iStatus = BuildNode(pIntf, pMaxChild, &spNiChild, &scaleInfo,
            bIgnoreHiddenFlag, false);
        if (spNiChild)
            pkNiNode->AttachChild(spNiChild);
        if (iStatus)
        {
            return(iStatus);
        }
    }

    // Deal with Physics stuff that must be done after children are processed.
    NiMAXPhysicsConverter::
        PostChildrenProcess(pMaxNode, pkNiNode, pObj, m_animStart);
    
    {
        NiMAXAnimationConverter anim(m_animStart, m_animEnd);

        iStatus = anim.Convert(pkNiNode, 
            pMaxNode, pParentScale, bIgnoreVisAnim);
        if (iStatus)
        {
            return(iStatus);
        }
    }



    if (pObj != NULL)
    {
        SClass_ID sID;

        sID = pObj->SuperClassID();

        while( sID == GEN_DERIVOB_CLASS_ID )
        {
            pObj = ((IDerivedObject*)pObj)->GetObjRef();
            sID = pObj->SuperClassID();
        }

        switch (sID)
        {
        case CAMERA_CLASS_ID:
            {
                NiMAXCameraConverter cam(m_animStart, m_animEnd);

                iStatus = cam.Convert(pIntf, pMaxNode, pObj, 
                    pkNiNode, m_animStart);
                if (iStatus)
                    return(iStatus);
                break;
            }
        case LIGHT_CLASS_ID:
            {
                NiMAXLightConverter light(m_animStart, m_animEnd);

                iStatus = light.Convert(pObj, pkNiNode, pMaxNode, 
                    &scaleInfo);
                if (iStatus)
                    return(iStatus);
                break;
            }
        case GEOMOBJECT_CLASS_ID:
            {
                if (NiMAXPhysicsConverter::
                    IsPhysicsJoint(pMaxNode, m_animStart))
                {
                    NiMAXPhysicsConverter::ConvertGeom(pObj, pMaxNode,
                        pkNiNode, m_animStart, &scaleInfo);
                    break;
                }

                bool bHidden = pMaxNode->IsNodeHidden() ? true : false;
                bHidden = bIgnoreHiddenFlag ? false : bHidden;
                bool bNiHide = 
                    pMaxNode->UserPropExists("NiHide") ? true : false;

                if (bNiHide)
                    bHidden = true;

                // determine if the node doesn't cast or receive shadows
                // These extra data are used and removed in NiMAXLightConverter
                bool bCaster = (pMaxNode->CastShadows() != 0);
                bool bReceiver = (pMaxNode->RcvShadows() != 0);
                if (!bCaster)
                {
                    NiBooleanExtraData* pkNonCasterData = 
                        NiNew NiBooleanExtraData(true);
                    pkNonCasterData->SetName(NON_SHADOW_CASTER_KEY);
                    pkNiNode->AddExtraData(pkNonCasterData);
                }
                if (!bReceiver)
                {
                    NiBooleanExtraData* pkNonReceiverData = 
                        NiNew NiBooleanExtraData(true);
                    pkNonReceiverData->SetName(NON_SHADOW_RECEIVER_KEY);
                    pkNiNode->AddExtraData(pkNonReceiverData);
                }

                iStatus = ConvertGeom(pObj, pkNiNode, 
                    pMaxNode, pIntf, &scaleInfo, bHidden);
                if (iStatus)
                    return(iStatus);

                // Add any physics information to this node
                NiMAXPhysicsConverter::ConvertGeom(pObj, pMaxNode,
                    pkNiNode, m_animStart, &scaleInfo);
        
                break;
            }
        case SHAPE_CLASS_ID:
            {
                if (pMaxNode->UserPropExists("NiExportLines"))
                {
                    bool bHidden = pMaxNode->IsNodeHidden() ? true : false;
                    bHidden = bIgnoreHiddenFlag ? false : bHidden;

                    if (pMaxNode->UserPropExists("NiHide"))
                        bHidden = true;

                    NiAVObject* pkNiObject;
                    pkNiObject = NULL;
                    iStatus = NiMAXShapeConverter::ConvertShape(pMaxNode, 
                        (ShapeObject*)pObj, pkNiObject, &scaleInfo);
                    if (pkNiObject)
                    {
                        pkNiObject->SetAppCulled(bHidden);
                        pkNiNode->AttachChild(pkNiObject);
                    }

                    if (iStatus)
                        return(iStatus);  
                }

                break;
            }
        case HELPER_CLASS_ID:
            {
                if (NiMAXPhysicsConverter::
                    IsPhysicsJoint(pMaxNode, m_animStart))
                {
                    NiMAXPhysicsConverter::ConvertGeom(pObj, pMaxNode,
                        pkNiNode, m_animStart, &scaleInfo);
                }

                break;
            }
        default:
            {
                // unknown class

                break;
            }
        }

        CHECK_MEMORY();
        kAttribConverter.Convert(pObj, pkNiNode, true);
        kAttribConverter.Convert(pMaxNode, pkNiNode, true);
        if (pMaxNode->GetObjectRef() != pObj)
        {
            kAttribConverter.Convert(pMaxNode->GetObjectRef(), pkNiNode, 
                true);
        }
    }

exit:
    if (strstr(pcPropBuf, "zMode") && 
        strcmp((*pspResult)->GetName(), "Scene Root") != 0)
    {
        bool bTest, bWrite;
        NiZBufferProperty* pZMode;

        bTest = true;
        bWrite = true;

        if (strstr(pcPropBuf, "zMode00"))
        {
            bTest = false;
            bWrite = false;
        }
        else if (strstr(pcPropBuf, "zMode01"))
        {
            bTest = false;
            bWrite = true;
        }
        else if (strstr(pcPropBuf, "zMode10"))
        {
            bTest = true;
            bWrite = false;
        }

        pZMode = NiNew NiZBufferProperty;

        pZMode->SetZBufferTest(bTest);
        pZMode->SetZBufferWrite(bWrite);

        (*pspResult)->AttachProperty(pZMode);
    }


    char* pcBillboardString = strstr(pcPropBuf, "billboard");
    if (pcBillboardString != NULL && NiIsExactKindOf(NiNode, (*pspResult)) && 
        strcmp((*pspResult)->GetName(), "Scene Root") != 0)
    {
        bool bMakeBillboard = true;
        if (pcBillboardString != pcPropBuf)
        {
            pcBillboardString -= 1;
            if (pcBillboardString[0] != ' ' &&
                pcBillboardString[0] != '\t' &&
                pcBillboardString[0] != '\n' &&
                pcBillboardString[0] != '\r')
            {
                bMakeBillboard = false;
            }
        }

        if (bMakeBillboard)
        {
            NiBillboardNode::FaceMode eType;

            if (strstr(pcPropBuf, "billboardUp"))
                eType = NiBillboardNode::ROTATE_ABOUT_UP;
            else if (strstr(pcPropBuf, "billboardRigidCenter"))
                eType = NiBillboardNode::RIGID_FACE_CENTER;
            else if (strstr(pcPropBuf, "billboardRigid"))
                eType = NiBillboardNode::RIGID_FACE_CAMERA;
            else if (strstr(pcPropBuf, "billboardCenter"))
                eType = NiBillboardNode::ALWAYS_FACE_CENTER;
            else                 
                eType = NiBillboardNode::ALWAYS_FACE_CAMERA;

            // Replace NiNode w/ a NiBillboardNode
            BillboardReplaceNode(pspResult, eType);
        }
    }

    if (strstr(pcPropBuf, "NiSwitchNode") && NiIsExactKindOf(NiNode, 
        (*pspResult)) && strcmp((*pspResult)->GetName(), "Scene Root") != 0)
    {
        // Replace NiNode w/ a NiSwitchNode
        int iWhichChild = 0;
        pMaxNode->GetUserPropInt("Index", iWhichChild);
        SwitchReplaceNode(pspResult, iWhichChild);
    }

    if (strstr(pcPropBuf, "NiSortAdjustNode") && 
        NiIsExactKindOf(NiNode, (*pspResult)) && 
        strcmp((*pspResult)->GetName(), "Scene Root") != 0)
    {
        NiSortAdjustNode::SortingMode eType;

        if (strstr(pcPropBuf, "SORTING_OFF"))
            eType = NiSortAdjustNode::SORTING_OFF;
        else
            eType = NiSortAdjustNode::SORTING_INHERIT;

        // Replace NiNode w/ a NiSortAdjustNode
        SortAdjustReplaceNode(pspResult, eType);

    }

    ScaleInfo* pScaleInfo = NiNew ScaleInfo;
    *pScaleInfo = *pParentScale;

    ms_pkNodeHTab->Insert(pMaxNode, pScaleInfo, *pspResult);
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}


//---------------------------------------------------------------------------
int NiMAXHierConverter::ConvertGeom(
    Object* pObj,
    NiNode* pCurNode,
    INode* pMaxNode,
    Interface* pIntf,
    ScaleInfo* pParentScale, 
    bool bIsHidden)
{
    CHECK_MEMORY();
    int iStatus;

    if (!pObj->IsRenderable())
        return(W3D_STAT_OK);
#ifndef NO_PARTICLES
    if (NiMAXPSysConverter::IsPSys(pMaxNode))
    {
        NiMAXPSysConverter psys(m_animStart, m_animEnd);

        iStatus = psys.Convert(pIntf, pMaxNode, pCurNode, pParentScale);
        return(iStatus);
    }
#endif
    if (NiMAXMorphConverter::IsMorphObject(pMaxNode))
    { 
        NiMAXMorphConverter morph(m_animStart, m_animEnd);

        iStatus = morph.Convert(pMaxNode, pCurNode, pParentScale);
        if (bIsHidden)
        {
            for (unsigned int ui = 0 ;ui < pCurNode->GetArrayCount() ;ui++ )
            {
                NiAVObject *pkObj = pCurNode->GetAt(ui);
                if (NiIsKindOf(NiMesh, pkObj))
                {
                    pkObj->SetAppCulled(bIsHidden);
                }
            }
        }

        return(iStatus);
    }
    NiAVObject* pGeom;
    NiMAXMeshConverter mesh(m_animStart, m_animEnd);

    NiAVObject** ppRemapTargets = NULL;
    unsigned int uiNumRemapTargets = 0;
    unsigned short** ppRemappings;
    NiUInt32** ppuiNewToOld = NULL;

    NiMAXUV* pkNiMAXUV = NULL;
    iStatus = mesh.ConvertGeomSub(
        pObj,
        pMaxNode,
        pCurNode,
        &pGeom, 
        false,
        false,
        pParentScale,
        false,
        ppRemappings,
        ppRemapTargets, 
        uiNumRemapTargets,
        pkNiMAXUV,
        ppuiNewToOld);

    if (pkNiMAXUV)
    {
        NiDelete pkNiMAXUV;
    }

    if (iStatus)
        return(iStatus);

    if (pGeom)
    {
        pGeom->SetAppCulled(bIsHidden);
        pCurNode->AttachChild(pGeom);
    }
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
void NiMAXHierConverter::Init()
{
    NIASSERT(ms_pkNodeHTab == NULL);
    ms_pkNodeHTab = NiNew NiTHash<NiNodePtr>(113, PFNodeHashEq, 
        PFNodeHashFree);
    ms_pstrCurrentName = NiNew NiString("");
    NIASSERT(ms_pkNodeHTab->GetNumHashed() == 0);
    
    NiMAXPhysicsConverter::Init();
}

//---------------------------------------------------------------------------
void NiMAXHierConverter::Shutdown()
{
    NiMAXPhysicsConverter::Shutdown();
    
    // cleanup the node hash table
    NiDelete ms_pkNodeHTab;
    ms_pkNodeHTab = NULL;
    NiDelete ms_pstrCurrentName;
    ms_pstrCurrentName = NULL;
}
//---------------------------------------------------------------------------
const char* NiMAXHierConverter::GetCurrentExportObjectName()
{
    NIASSERT(ms_pstrCurrentName);
    return *ms_pstrCurrentName;
}
//---------------------------------------------------------------------------

