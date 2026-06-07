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

// NiCollisionLabInfo.cpp

#include "stdafx.h"
#include "resource.h"
#include <NiOptimizeABV.h>
#include <NiCollisionTraversals.h>
#include "NiCollisionLabInfo.h"
#include "NifUserPreferences.h"
#include "ProgressBarDlg.h"
#include "NifDoc.h"
#include <NifAnimationInfo.h>
#include <NifTimeManager.h>
#include <NiMesh.h>

//---------------------------------------------------------------------------
NiCollisionLabInfo::NiCollisionLabInfo()
{
    m_bDynamicSimOn = false;
    m_bCollided = false;
    m_bAuxCallbacks = false;

    m_fRemainingDistance = 0.0f;  
    m_fVelocity = 460.0f;

    m_kDirection = NiPoint3::ZERO;

    ClearIntersectData();
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::ClearIntersectData()
{
    memset(&m_kIntersectData,0,sizeof(m_kIntersectData));
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::SetAuxCallbacksOnSceneGraph(bool bValue)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    pkDoc->Lock();

    NiAVObject* pkLabObj = FindObjectByName(STR_COLLISION_LAB);
    if (pkLabObj)
    {
        RecursiveSetAuxCallbacks(pkLabObj, bValue);
    }
    
    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::RecursiveSetSelectives(NiAVObject* pkObj,
    bool bSelective, bool bTrans, bool bProps, bool bRigid)
{
    pkObj->SetSelectiveUpdate(bSelective);
    pkObj->SetSelectiveUpdateTransforms(bTrans);
    pkObj->SetSelectiveUpdatePropertyControllers(bProps);
    pkObj->SetSelectiveUpdateRigid(bRigid);
                   
    if (NiIsKindOf(NiNode,pkObj))
    {
        // Call for each child...
        NiNode* pkNode = (NiNode*)pkObj;

        unsigned int uiTotal = pkNode->GetArrayCount();
        for(unsigned int ui=0; ui<uiTotal; ui++)
        {
            NiAVObject* pkChildObj = pkNode->GetAt(ui);

            if(pkChildObj == NULL)
                continue;

            RecursiveSetSelectives(pkChildObj, bSelective, bTrans, bProps,
                bRigid);
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::RecursiveSetAuxCallbacks(NiAVObject* pkObj,
    bool bEnable)
{
    NiCollisionData* pkCD = NiGetCollisionData(pkObj);
    
    if (pkCD)
        pkCD->SetEnableAuxCallbacks(bEnable);

    if (NiIsKindOf(NiNode,pkObj))
    {
        // Call for each child...
        NiNode* pkNode = (NiNode*)pkObj;

        unsigned int uiTotal = pkNode->GetArrayCount();
        for(unsigned int ui=0; ui<uiTotal; ui++)
        {
            NiAVObject* pkChildObj = pkNode->GetAt(ui);

            if(pkChildObj == NULL)
                continue;

            RecursiveSetAuxCallbacks(pkChildObj,bEnable);
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::RecursiveSetWireABV(NiAVObject* pkObj,
    bool bCull, bool bSelective, bool bTransform, bool bRigid, 
    bool bUpdateChild)
{
    if (NiIsKindOf(NiNode,pkObj))
    {
        // Call for each child...
        NiNode* pkNode = (NiNode*)pkObj;

        unsigned int uiTotal = pkNode->GetArrayCount();
        for(unsigned int ui=0;ui<uiTotal;ui++)
        {
            NiAVObject* pkChildObj = pkNode->GetAt(ui);

            if(pkChildObj == NULL)
                continue;

            RecursiveSetWireABV(pkChildObj,bCull,
                bSelective,bTransform, bRigid,bUpdateChild);
        }
    }

    if (pkObj->GetName())
    {
        if (strstr(pkObj->GetName(), STR_COLLISION_WIREABV)!=0)
        {
            pkObj->SetAppCulled(bCull);
            pkObj->SetSelectiveUpdate(bSelective);
            pkObj->SetSelectiveUpdateTransforms(bTransform);
            pkObj->SetSelectiveUpdateRigid(bRigid);

            NIASSERT(NiIsKindOf(NiNode,pkObj));

            NiAVObject* pkChild = ((NiNode*)pkObj)->GetAt(0);
            
            NIASSERT(pkChild);
            NIASSERT(NiIsKindOf(NiMesh,pkChild));

            if (bUpdateChild)
            {
                pkChild->SetAppCulled(false);
                pkChild->SetSelectiveUpdate(true);
                pkChild->SetSelectiveUpdateTransforms(true);
                pkChild->SetSelectiveUpdateRigid(false);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::ResetCollisionIndicator(bool bDynamic)
{
    NiNode* pkIndicator = NULL;
    if (bDynamic)
        pkIndicator = (NiNode*)FindObjectByName(STR_COLLISION_DYNAMIC);
    else
        pkIndicator = (NiNode*)FindObjectByName(STR_COLLISION_STATIC);

    if (pkIndicator == NULL)
        return;

    unsigned int uiCnt = pkIndicator->GetArrayCount();
    for (unsigned int ui=0; ui<uiCnt; ui++)
    {
        NiAVObject* pkChild = pkIndicator->GetAt(ui);

        if (pkChild == NULL)
            continue;

        pkIndicator->DetachChild(pkChild);
    }
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::CreateCollisionIndicator()
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    NiNode* pkScene = pkDoc->GetSceneGraph();

    NiNode* pkDynamic = 
        (NiNode*)(FindObjectByName(STR_COLLISION_DYNAMIC));

    // create a collision indicator intersection.point - scence.trans    
    // create the center of the indicator (sphere)
    NiSphere kSphere;
    kSphere.m_fRadius = 1.0f;
    kSphere.m_kCenter = NiPoint3(0, 0, 0);
    NiMeshPtr spCenter = NiDrawableBV::CreateFromSphere(kSphere,1.0f);
    
    // create the collider normal of the indicator (capsule)
    NiPoint3::UnitizeVector(m_kIntersectData.kNormal0);
    NiCapsule kCapsule;
    kCapsule.m_kSegment.m_kOrigin = NiPoint3::ZERO;
    kCapsule.m_kSegment.m_kDirection = 4 * m_kIntersectData.kNormal0;
    kCapsule.m_fRadius = 0.25f;
    NiMeshPtr spNormal0 = NiDrawableBV::CreateFromCapsule(kCapsule,1.0f);

    // create the collidee normal of the indicator (capsule)
    NiPoint3::UnitizeVector(
        m_kIntersectData.kNormal1);
    kCapsule.m_kSegment.m_kDirection = 4 * 
        m_kIntersectData.kNormal1;
    NiMeshPtr spNormal1 = NiDrawableBV::CreateFromCapsule(kCapsule,1.0f);

    // make the center black and the normal white
    NiVertexColorProperty* pkVC = NiNew NiVertexColorProperty;
    pkVC->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);
    pkVC->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    spCenter->AttachProperty(pkVC);
    spNormal0->AttachProperty(pkVC);
    spNormal1->AttachProperty(pkVC);

    NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
    pkMat->SetEmittance(NiColor::BLACK);
    spCenter->AttachProperty(pkMat);
    pkMat = NiNew NiMaterialProperty;
    pkMat->SetEmittance(NiColor(1,0,0));
    spNormal0->AttachProperty(pkMat);
    pkMat = NiNew NiMaterialProperty;
    pkMat->SetEmittance(NiColor::WHITE);
    spNormal1->AttachProperty(pkMat);

    NiPoint3 kTrans = m_kIntersectData.kPoint - pkScene->GetTranslate();
    spCenter->SetTranslate(kTrans);
    spNormal0->SetTranslate(kTrans);
    spNormal1->SetTranslate(kTrans);

    // Connect everything together
    pkDynamic->AttachChild(spCenter);
    pkDynamic->AttachChild(spNormal0);
    pkDynamic->AttachChild(spNormal1);

    RecursiveSetSelectives(pkDynamic,true,true,true,false);

    pkScene->UpdateProperties();
    pkScene->UpdateEffects();
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::HideWireFrames()
{
    NiNode* pkStatic = (NiNode*)FindObjectByName(STR_COLLISION_STATIC);

    if (pkStatic == NULL)
        return;

    NiAVObject* pkObj = FindObjectByName(STR_COLLISION_COLLIDER);
    RecursiveSetWireABV(pkObj, true, true, true, false);

    pkObj = FindObjectByName(STR_COLLISION_COLLIDEE);
    RecursiveSetWireABV(pkObj, true, true, true, false);

    unsigned int uiChildCnt = pkStatic->GetArrayCount();
    for(unsigned int ui=0; ui<uiChildCnt; ui++)
    {
        NiAVObject* pkChildObj = pkStatic->GetAt(ui);
    
        if (pkChildObj == NULL)
            continue;

        pkStatic->DetachChild(pkChildObj);
    }
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::CreateAttachTri( 
    const NiPoint3* apkTri, float fMult, NiColor& kColor,
    float fTime)
{
    NiNode* pkTriOBBNode = NULL;
    NiPoint3 kDelta;

    if (GetDynamicSim())
    {
        pkTriOBBNode = 
            (NiNode*)(FindObjectByName(STR_COLLISION_DYNAMIC));
        kDelta = m_kDirection * fTime;
    }
    else 
    {
        pkTriOBBNode =
            (NiNode*)(FindObjectByName(STR_COLLISION_STATIC));
        kDelta = NiPoint3::ZERO;
    }

    NiMesh* pkMesh = NiNew NiMesh();
    pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiDataStreamElementLock kIndexLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UINT16_1, 6,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        NiDataStream::USAGE_VERTEX_INDEX);

    NiTStridedRandomAccessIterator<NiUInt16> kIndexIter = 
        kIndexLock.begin<NiUInt16>();

    NiDataStreamElementLock kPositionLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3, 3,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        NiDataStream::USAGE_VERTEX);

    NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = 
        kPositionLock.begin<NiPoint3>();

    kPositionIter[0] = (apkTri[0] * fMult) + kDelta;
    kPositionIter[1] = (apkTri[1] * fMult) + kDelta;
    kPositionIter[2] = (apkTri[2] * fMult) + kDelta;

    kIndexIter[0] = 0;
    kIndexIter[1] = 1;
    kIndexIter[2] = 2;
    kIndexIter[3] = 2;
    kIndexIter[4] = 1;
    kIndexIter[5] = 0;

    NiBound kBound;
    kBound.ComputeFromData(3, apkTri);
    pkMesh->SetModelBound(kBound);

    pkMesh->SetName(STR_COLLISION_WIRETRIOBB);
    pkTriOBBNode->AttachChild(pkMesh);

    NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
    pkMat->SetEmittance(kColor);
    pkMesh->AttachProperty(pkMat);

    RecursiveSetSelectives(pkTriOBBNode,true,true,true,false);

    pkTriOBBNode->UpdateProperties();
    pkTriOBBNode->UpdateEffects();
}
//---------------------------------------------------------------------------
float NiCollisionLabInfo::GetSmallestObjRadius()
{
    NiAVObject* pkObj0 = m_kIntersectData.pkObj0;
    NiAVObject* pkObj1 = m_kIntersectData.pkObj1;

    if (pkObj0 == NULL || pkObj1 == NULL)
    {
        pkObj0 = FindObjectByName(STR_COLLISION_COLLIDER);
        pkObj1 = FindObjectByName(STR_COLLISION_COLLIDEE);

        NIASSERT(pkObj0);
        NIASSERT(pkObj1);
    }

    NiBound kBound0 = pkObj0->GetWorldBound();
    float fRadius0 = kBound0.GetRadius();

    NiBound kBound1 = pkObj1->GetWorldBound();
    float fRadius1 = kBound1.GetRadius();

    // Get smallest radius
    float fRadius = fRadius0;
    if (fRadius1 < fRadius)                   
        fRadius = fRadius1;

    return fRadius;
}
//---------------------------------------------------------------------------
int NiCollisionLabInfo::Callback(NiCollisionGroup::Intersect& kIntersect)
{
    // This is the function called when a collision is found between two
    // objects.  Within the kIntersect class, pkNode0 will be the collider, 
    // but either pkObj0 or pkObj1 could be pkNode's ancester, so don't rely
    // on any particular relationship.  This demo only indicates when static 
    // collisions occur, so fTime, kNormal and kPoint are not useful.

    CNifDoc* pkDoc = CNifDoc::GetDocument();
    pkDoc->Lock();

    bool bTriLevel = false;
    bool bSubstituted = false;

    // First retrieve the record used to store the collisions   
    NiCollisionData* pkData = NiGetCollisionData(kIntersect.pkRoot0);
    NIASSERT(pkData);

    NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;
    NIASSERT(pkLabInfo);

    // Store the intersect data
    if ( pkLabInfo->m_bCollided ) 
    {
        if (pkLabInfo->GetDynamicSim())
        {
            const float fEpsilon = 1e-06f;
            if (kIntersect.fTime + fEpsilon
                < pkLabInfo->m_kIntersectData.fTime)
            {
                // Choose a specific collider (or one of its component
                // triangles) by minimum collision time, if possible.
                bSubstituted = true;
            }
            else if (NiAbs(kIntersect.fTime - 
                pkLabInfo->m_kIntersectData.fTime) <= fEpsilon)
            {
                // If collision times are the same (within tolerance), choose
                // a specific collider (or one of its component triangles) by
                // the minimum angle between the velocity vector and the
                // collider's collision normal.
                float fDotNew = pkLabInfo->m_kDirection.Dot(
                    kIntersect.kNormal0);
                float fDotCur = pkLabInfo->m_kDirection.Dot(
                    pkLabInfo->m_kIntersectData.kNormal0);

                if (NiAbs(fDotNew) > NiAbs(fDotCur))
                    bSubstituted = true;
            }
        }
        else
        {
            bSubstituted = true;
        }
    }
    else
    {
        bSubstituted = true;
        pkLabInfo->m_bCollided = true;
    }

    if (bSubstituted)
        pkLabInfo->m_kIntersectData = kIntersect;
    
    NiNode* pkScene = pkDoc->GetSceneGraph();

    // Hide wireframe representations...
    NiAVObject* pkLab = pkLabInfo->FindObjectByName(STR_COLLISION_LAB);

    NiAVObject* pkObj = kIntersect.pkObj0;
    NiAVObject* pkWire = NULL;

    if (pkObj)
    {
        if (NiIsKindOf(NiNode,pkObj))
        {
            NiString strName = NULL;
            strName.Format("%s_%x", STR_COLLISION_WIREABV, pkObj);

            // Look for an immediate child with COLLISION_WIREABV_STING name. 
            unsigned int uiTotal = ((NiNode*)pkObj)->GetArrayCount();
            for (unsigned int ui=0;ui<uiTotal;ui++)
            {
                NiAVObject* pkChildObj = ((NiNode*)pkObj)->GetAt(ui);
            
                if (pkChildObj == NULL)
                    continue;

                if (pkChildObj->GetName())
                {
                    if (strcmp(pkChildObj->GetName(),(const char*)strName)==0)
                    {
                        pkWire = pkChildObj;
                        break;
                    }
                }
            }
        }
        else
        {
            // This is probably a trishape (obb/tri) and wire was placed
            // on the parent...
            NiNode* pkParent = pkObj->GetParent();
            NIASSERT(pkParent);
            NiString strName = NULL;
            strName.Format("%s_%x", STR_COLLISION_WIREABV, pkObj);
            pkWire = pkParent->GetObjectByName((const char*)strName);
        }
    }

    if (pkWire)
    {
        pkWire->SetAppCulled(false);
        pkWire->SetSelectiveUpdate(true);
        pkWire->SetSelectiveUpdateTransforms(true);
        pkWire->SetSelectiveUpdateRigid(false);
    }

    pkData = NiGetCollisionData(pkObj);
    if (pkData)
    {
        if (pkData->GetCollisionMode() == NiCollisionData::USE_TRI ||
            pkData->GetCollisionMode() == NiCollisionData::USE_OBB)
            bTriLevel = true;
    }

    pkObj = kIntersect.pkObj1;
    pkWire = NULL;

    if (pkObj)
    {
        if (NiIsKindOf(NiNode,pkObj))
        {
            NiString strName = NULL;
            strName.Format("%s_%x", STR_COLLISION_WIREABV, pkObj);

            // Look for an immediate child with COLLSION_WIREABV_STRING name.
            unsigned int uiTotal = ((NiNode*)pkObj)->GetArrayCount();
            for (unsigned int ui=0;ui<uiTotal;ui++)
            {
                NiAVObject* pkChildObj = ((NiNode*)pkObj)->GetAt(ui);
            
                if (pkChildObj == NULL)
                    continue;

                if (pkChildObj->GetName())
                {
                    if (strcmp(pkChildObj->GetName(),(const char*)strName)==0)
                    {
                        pkWire = pkChildObj;
                        break;
                    }
                }
            }
        }
        else
        {
            // This is probably a trishape (obb/tri) and wire was placed
            // on the parent...
            NiNode* pkParent = pkObj->GetParent();
            NIASSERT(pkParent);
            NiString strName = NULL;
            strName.Format("%s_%x", STR_COLLISION_WIREABV, pkObj);
            pkWire = pkParent->GetObjectByName((const char*)strName);
        }
    }

    if (pkWire)
    {
        pkWire->SetAppCulled(false);
        pkWire->SetSelectiveUpdate(true);
        pkWire->SetSelectiveUpdateTransforms(true);
        pkWire->SetSelectiveUpdateRigid(false);
    }

    pkData = NiGetCollisionData(pkObj);
    if (pkData)
    {
        if (pkData->GetCollisionMode() == NiCollisionData::USE_TRI ||
            pkData->GetCollisionMode() == NiCollisionData::USE_OBB)
            bTriLevel = true;
    }

    
    if (bTriLevel && bSubstituted)
    {
        // because intersect information is not valid outside of
        // callback, we save it. 
        if (kIntersect.appkTri1)
        {
            pkLabInfo->m_kTri1[0] = *kIntersect.appkTri1[0];
            pkLabInfo->m_kTri1[1] = *kIntersect.appkTri1[1];
            pkLabInfo->m_kTri1[2] = *kIntersect.appkTri1[2];
        }

        if (kIntersect.appkTri2)
        {
            pkLabInfo->m_kTri2[0] = *kIntersect.appkTri2[0];
            pkLabInfo->m_kTri2[1] = *kIntersect.appkTri2[1];
            pkLabInfo->m_kTri2[2] = *kIntersect.appkTri2[2];
        }

        if (pkLabInfo->DisplayTrianglesProcessed())
            pkLabInfo->CreateCollisionTriangles(false);
    }
    else
    {
        kIntersect.appkTri1 = NULL;
        kIntersect.appkTri2 = NULL;
    }
    
    pkDoc->UnLock();

    // If we are doing static collisions, then, we can break out 
    // as soon as 1 triangle is detected as colliding. We can
    // do this in static collision since our app doesn't care about
    // finding the first point of intersection, time, etc. This
    // is much faster as we don't have to process every triangle
    // every frame when objects are in a collision state.
    if (bTriLevel && pkLabInfo->GetDynamicSim()==false)
        return NiCollisionGroup::BREAKOUT_COLLISIONS;

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
void NiCollisionLabInfo::CreateCollisionTriangles(
    bool bFinalCallFromOutsideCallback)
{   
    NiNode* pkLab = (NiNode*)FindObjectByName(STR_COLLISION_LAB);

    NiColor kColor1 = NiColor(0.0f,0.0f,0.7f);
    NiColor kColor2 = NiColor(0.7f, 0.0f,0.0f);
    float fScale = 1.02f;

    if (bFinalCallFromOutsideCallback)
    {
        kColor1 = NiColor(0.5f,1.0f,1.0f);
        kColor2 = NiColor(0.7f,1.0f,1.0f);
        fScale = 1.025f;
    }

    if (GetDynamicSim())
    {
        NiNode* pkTriOBBNode = 
            (NiNode*)(FindObjectByName(STR_COLLISION_DYNAMIC));
        pkTriOBBNode->RemoveAllChildren();
    }
    else 
    {
        NiNode* pkTriOBBNode =
            (NiNode*)(FindObjectByName(STR_COLLISION_STATIC));
        pkTriOBBNode->RemoveAllChildren();
    }

    if (m_kIntersectData.appkTri1)
        CreateAttachTri( m_kTri1,fScale, kColor1, m_kIntersectData.fTime);

    if (m_kIntersectData.appkTri2)
        CreateAttachTri(m_kTri2, fScale, kColor2, 0.0f);

    if (bFinalCallFromOutsideCallback)
    {
        // From outside the callback, appkTri1 and appkTri2 are not valid.
        // However, we should creat triangles whenever the values are
        // non zero. Since we just created the triangles, we set appkTri1
        // and appkTri2 to NULL.
        m_kIntersectData.appkTri1 = NULL; 
        m_kIntersectData.appkTri2 = NULL;
    }
}
//---------------------------------------------------------------------------
bool NiCollisionLabInfo::AddCollisionLabNifFile(
    CString strPathName, int iLoc)
{
    // Load NIF file.
    CProgressBarDlg* pProgressBar = NiExternalNew CProgressBarDlg;
    pProgressBar->m_strMessage = "Loading NIF File\nPlease Wait...";
    pProgressBar->Create(IDD_PROGRESS_BAR);
    pProgressBar->ShowWindow(SW_SHOW);
    pProgressBar->UpdateWindow();
    pProgressBar->SetRange(0, 1);
    pProgressBar->SetPos(1);

    CNifDoc* pkDoc = CNifDoc::GetDocument();
    pkDoc->Lock();

    NiStream kStream;
    if (!kStream.Load(strPathName))
    {
        CString strMsg;
        strMsg.Format("Unable to open the specified NIF file: %s. "
            "Please try again.", kStream.GetLastErrorMessage());
        ::MessageBox(NULL, strMsg,
            "Error Loading NIF File", MB_OK | MB_ICONERROR);
        pProgressBar->DestroyWindow();
        pkDoc->UnLock();   
        return false;
    }
    pProgressBar->DestroyWindow();

    unsigned int uiObjCnt = kStream.GetObjectCount();

    if (uiObjCnt == 0)
    {
        ::MessageBox(NULL,
            "This file contains no top-level objects. It will "
            "not be loaded", "Invalid Object Error", MB_OK |
            MB_ICONEXCLAMATION);
        pkDoc->UnLock();   
        return false;
    }

    ClearIntersectData();
    ResetCollisionIndicator(false);
    ResetCollisionIndicator(true);

    for (unsigned int ui = 0; ui < uiObjCnt; ui++)
    {
        NiObject* pkBaseObj = kStream.GetObjectAt(ui);
        NiNode* pkObject = NiDynamicCast(NiNode, pkBaseObj);
        if (!pkObject)
        {
            continue;
        }

        if (NiCollisionTraversals::CheckForCollisionData(
            pkObject, true) == false)
        {
            // No collision data found... abort.
            ::MessageBox(NULL,
                "This file contains no NiCollisionData. "
                " Collision data will be created and triangle " \
                "level collision dectection will be used. ", 
                "User Warning", MB_OK |
                MB_ICONEXCLAMATION);

            NiCollisionTraversals::CreateCollisionData(pkObject,
                NiCollisionData::USE_TRI);
        }

        if (pkObject && !pkObject->GetParent())
        {
            NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;
            delete pkDoc->m_pkCollisionLabGrp;
            pkDoc->m_pkCollisionLabGrp = NULL;
            pkLabInfo->AddCollisionLabObject(pkObject, strPathName, iLoc);

            NiAVObject* pkCollider = FindObjectByName(
                STR_COLLISION_COLLIDER);

            NiAVObject* pkCollidee = FindObjectByName(
                STR_COLLISION_COLLIDEE);
            
            if (pkCollider && pkCollidee)
            {
                RecursiveSetSelectives(pkCollider,true,true,true,false);
                RecursiveSetSelectives(pkCollidee,true,true,true,false);

                // We have the right amount of objects...
                pkDoc->m_pkCollisionLabGrp = NiNew NiCollisionGroup;

                // Ensure both objects have collision data...
                NiCollisionData* pkCD0 = NiGetCollisionData(pkCollider);
                NiCollisionData* pkCD1 = NiGetCollisionData(pkCollidee);

                if (pkCD0 == NULL)
                    pkCD0 = NiNew NiCollisionData(pkCollider);

                if (pkCD1 == NULL)
                    pkCD1 = NiNew NiCollisionData(pkCollidee);

                pkDoc->m_pkCollisionLabGrp->AddCollider(pkCollider, false);
                pkDoc->m_pkCollisionLabGrp->AddCollider(pkCollidee, false);

                pkCD0->SetCollideCallback(Callback);
                pkCD0->SetCollideCallbackData(this);               

                pkLabInfo->SetAuxCallbacks(pkLabInfo->m_bAuxCallbacks);
            }

        }
        else if(NiIsKindOf(NiPixelData, pkBaseObj))
        {
            ::MessageBox(NULL, "This file contains a nif texture as a "
                "top-level object. The texture will not be loaded.",
                "NIF Texture Found Error", MB_OK | MB_ICONEXCLAMATION);
        }
        else if(ui == 0)
        {
            ::MessageBox(NULL, "This file contains an invalid top-level "
                "object. The object will not be loaded",
                "Invalid Object Error", MB_OK | MB_ICONEXCLAMATION);
        }
    }
    AfxGetApp()->AddToRecentFileList(strPathName);
    
    pkDoc->UnLock();   
    return true;
}
//---------------------------------------------------------------------------
NiNode* NiCollisionLabInfo::AddCollisionLabObject(NiAVObject* pkRootObject,
    const char* pcFilePath, int iLoc)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    pkDoc->Lock();

    NiNode* pkScene = pkDoc->GetSceneGraph();
    NiNode* pkLab = (NiNode*)FindObjectByName(STR_COLLISION_LAB);

    bool bAttachLab = false;

    if (pkLab == NULL)
    {
        pkLab = NiNew NiNode;
        pkLab->SetSelectiveUpdate(true);
        pkLab->SetSelectiveUpdateTransforms(true);
        pkLab->SetSelectiveUpdatePropertyControllers(true);
        pkLab->SetSelectiveUpdateRigid(false);
        pkLab->SetName(STR_COLLISION_LAB);
        bAttachLab = true;

        // Create Indicator slots...
        NiNode* pkIndicator = NiNew NiNode;
        pkIndicator->SetName(STR_COLLISION_INDICATORS);
        pkLab->SetAt(INDICATOR_POS,pkIndicator);

        NiNode* pkNode = NiNew NiNode;
        pkNode->SetName(STR_COLLISION_STATIC);
        pkIndicator->SetAt(INDICATOR_STATIC_POS, pkNode);

        // Create the Dynamic indicators node
        pkNode = NiNew NiNode;
        pkNode->SetName(STR_COLLISION_DYNAMIC);
        pkIndicator->SetAt(INDICATOR_DYNAMIC_POS, pkNode);

        NiWireframeProperty* pkWire = NiNew NiWireframeProperty;
        pkWire->SetWireframe(true);
        pkIndicator->AttachProperty(pkWire);

        NiVertexColorProperty* pkVC = NiNew NiVertexColorProperty;
        pkVC->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);
        pkVC->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
        pkIndicator->AttachProperty(pkVC);

        RecursiveSetSelectives(pkIndicator,true,true,true,false);

    }

    NiNode* pkNode = NiNew NiNode;
    pkNode->SetSelectiveUpdate(true);
    pkNode->SetSelectiveUpdateTransforms(true);
    pkNode->SetSelectiveUpdatePropertyControllers(true);
    pkNode->SetSelectiveUpdateRigid(false);
    const char* pcNifName = strrchr(pcFilePath, '\\') + 1;
    pkNode->SetName(pcNifName);
    pkNode->AttachChild(pkRootObject);

    pkLab->SetAt(iLoc,pkNode);

    // Add wire frames to the node...
    NiColor kWireColor;

    if (iLoc == 0)
        kWireColor = NiColor(1.0f,1.0f,0.0f);
    else
        kWireColor = NiColor(0.0f,1.0f,1.0f);

    pkNode->Update(0.0f);
    NiOptimizeABV::CreateWireABV(pkNode, kWireColor, 1.019f,
        STR_COLLISION_WIREABV,false,true); 
    bool bSelectiveUpdate, bRigid;
    pkNode->SetSelectiveUpdateFlags(bSelectiveUpdate,
        pkNode->GetSelectiveUpdateTransforms(), bRigid);

    unsigned int uiIndex = pkScene->GetChildCount();

    if (bAttachLab)
    {
        pkDoc->AddRoot(pkLab);

        CNifStatisticsManager::Lock();
        CNifStatisticsManager::AccessStatisticsManager()->AddRoot(pkLab);
        CNifStatisticsManager::UnLock();
    }

    pkScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkScene);
    pkScene->UpdateProperties();
    pkScene->UpdateEffects();
    pkScene->UpdateNodeBound();

    CNifTimeManager* pkTimeManager = pkDoc->GetTimeManager();
    CNifAnimationInfo* pkAnimInfo = pkDoc->GetAnimationInfo();
    
    pkAnimInfo->CollectData(pkScene);
    pkTimeManager->SetStartTime(pkAnimInfo->GetMinTime());
    pkTimeManager->SetEndTime(pkAnimInfo->GetMaxTime());
    if (pkTimeManager->GetStartTime() == pkTimeManager->GetEndTime())
    {
        pkTimeManager->SetTimeMode(CNifTimeManager::CONTINUOUS);
    }

    if (bAttachLab)
        pkDoc->GetCameraList()->AddCamerasForRoot(pkLab);

    pkDoc->UpdateAllViews(NULL, MAKELPARAM(NIF_ADDNIF, uiIndex), NULL);
    pkDoc->UpdateAllViews(NULL, MAKELPARAM(NIF_UPDATEDEFAULTNODE, 0), NULL);
    pkDoc->UpdateAllViews(NULL, MAKELPARAM(NIF_SCENECHANGED, 0), NULL);

    // Ensure children of AssetViewerCollisionWireABV flags correct.
    RecursiveSetWireABV(pkNode,false, true, true, false, true);

    if (AfxGetMainWnd())
    {
        CString strAppTitle;
        strAppTitle.LoadString(AFX_IDS_APP_TITLE);
        if (pkDoc->GetNumberOfRoots()> 2)
        {
            AfxGetMainWnd()->SetWindowText(strAppTitle +
                " - Multiple Files");
        }
        else
        {
            AfxGetMainWnd()->SetWindowText(strAppTitle + CString(" - ") +
                pcNifName);
        }
    }

    g_bNeedPromptOnDiscard = true;

    pkDoc->UnLock();

    return pkLab;
}
//---------------------------------------------------------------------------
NiAVObject* NiCollisionLabInfo::FindObjectByName(char* pcName)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();

    NiNode* pkScene = pkDoc->GetSceneGraph();

    NiAVObject* pkLabObj = NULL;

    unsigned int uiChildCnt = pkScene->GetArrayCount();

    for(unsigned int ui=0; ui < uiChildCnt; ui++)
    {
        NiAVObject* pkChild = pkScene->GetAt(ui);

        if (pkChild == NULL)
            continue;

        if (strcmp(pkChild->GetName(),STR_COLLISION_LAB)!=0)
            continue;

        pkLabObj = pkChild;
        break;
    }

    if (strcmp(pcName,STR_COLLISION_LAB)==0)
        return pkLabObj;

    if (pkLabObj == NULL)
        return NULL;

    if (strcmp(pcName,STR_COLLISION_COLLIDER)==0)
        return ((NiNode*)pkLabObj)->GetAt(COLLIDER_POS);

    if (strcmp(pcName,STR_COLLISION_COLLIDEE)==0)
        return ((NiNode*)pkLabObj)->GetAt(COLLIDEE_POS);

    NiAVObject* pkIndicator =
        ((NiNode*)pkLabObj)->GetAt(INDICATOR_POS);

    if (strcmp(pcName,STR_COLLISION_STATIC)==0)
        return ((NiNode*)pkIndicator)->GetAt(0);

    if (strcmp(pcName,STR_COLLISION_DYNAMIC)==0)
        return ((NiNode*)pkIndicator)->GetAt(1);

    return NULL;   
}
//---------------------------------------------------------------------------


