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

#include "stdafx.h"
#include "MOrbitPoint.h"
#include "MSharedData.h"
#include "MFramework.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MOrbitPoint::MOrbitPoint()
{
    m_bFollow = true;
    m_pkFollowObj = NULL;
    m_pkOrbitRoot = NiNew NiNode();
    m_pkOrbitRoot->SetSelectiveUpdate(true);
    m_pkOrbitRoot->SetSelectiveUpdateTransforms(true);
    m_pkOrbitRoot->SetSelectiveUpdatePropertyControllers(true);
    m_pkOrbitRoot->SetSelectiveUpdateRigid(false);
    m_pkOrbitRoot->SetTranslate(NiPoint3::ZERO);
    m_pkOrbitRoot->SetRotate(NiMatrix3::IDENTITY);
    m_pkOrbitRoot->SetScale(1.0f);

    LoadOrbitScene();
    
    MSceneObject::SetToolOnlyObject(m_pkOrbitRoot, true);
    if (m_pkOrbitScene != NULL)
    {
        m_pkOrbitRoot->AttachChild(m_pkOrbitScene);
    }

    m_pkOrbitRoot->UpdateProperties();
    m_pkOrbitRoot->UpdateEffects();

    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    pkSharedData->SetScene(MSharedData::ORBIT_INDEX, m_pkOrbitRoot);
    pkSharedData->Unlock();

}
//---------------------------------------------------------------------------
MOrbitPoint::~MOrbitPoint()
{
    
}
//---------------------------------------------------------------------------
void MOrbitPoint::DeleteContents()
{
    if (m_pkOrbitScene)
    {
        m_pkOrbitScene->DecRefCount();
    }
    
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    pkSharedData->RemoveScene(MSharedData::ORBIT_INDEX);
    pkSharedData->Unlock();

    m_pkOrbitRoot = NULL;
    m_pkOrbitScene = NULL;
    
}
//---------------------------------------------------------------------------
void MOrbitPoint::ResetOrbitPoint()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    NiActorManager* pkActorManager = pkSharedData->GetActorManager();
    
    if (pkActorManager)
    {
        NiAVObject* pkModelRoot = pkActorManager->GetNIFRoot();
        NiAVObject* pkCharacter = pkActorManager->GetActorRoot();
        NiAVObject* pkAccumRoot = pkActorManager->GetAccumRoot();
        NiAVObject* pkNonAccumRoot = NULL;

        if (pkAccumRoot)
        {
            char acString[1024];
            NiSprintf(acString, 1024, "%s NonAccum", pkAccumRoot->GetName());
            pkNonAccumRoot = pkAccumRoot->GetObjectByName(acString);
        }

        if (pkNonAccumRoot == NULL)
        {
            pkNonAccumRoot = pkAccumRoot;
        }

        if (pkNonAccumRoot == NULL)
        {
            pkNonAccumRoot = pkCharacter;
        }

        if (pkNonAccumRoot == NULL)
        {
            pkNonAccumRoot = pkModelRoot;
        }
        
        Initialize(pkModelRoot, pkNonAccumRoot);
    }
    else
    {
        m_pkFollowObj = NULL;
    }
    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
void MOrbitPoint::LoadOrbitScene()
{
    NiStream kStream;
    String* strPath = MFramework::Instance->AppStartupPath;
    String* strOrbitFile = NiExternalNew String("\\..\\..\\OrbitPoint.nif");
    strPath = strPath->Concat(strPath, strOrbitFile);
    const char* pcPath = MStringToCharPointer(strPath);

    if (NiVirtualBoolBugWrapper::NiStream_Load(kStream, pcPath) != 0)
    {
        NiNode* pkNode = NiDynamicCast(NiNode, kStream.GetObjectAt(0));
        if (pkNode)
        {
            NiAVObject* pkAVObj = pkNode->GetObjectByName("Origin");
            m_pkOrbitScene = NiDynamicCast(NiNode, pkAVObj);
            if (m_pkOrbitScene)
            {
                m_pkOrbitScene->IncRefCount();
                m_pkOrbitScene->Update(0.0f);
                NiMesh::CompleteSceneModifiers(m_pkOrbitScene);
                m_pkOrbitScene->UpdateProperties();
                m_pkOrbitScene->UpdateEffects();
                m_pkOrbitScene->UpdateNodeBound();
                m_pkOrbitScene->SetAppCulled(true);

                MSceneObject::SetToolOnlyObject(m_pkOrbitScene, true);
            }
        }
    }
    else
    {       
        MessageBox(NULL, pcPath, "Missing File Error", MB_OK);
    }


    MFreeCharPointer(pcPath);
}
//---------------------------------------------------------------------------
void MOrbitPoint::Initialize(NiAVObject* pkSceneToFit,
            NiAVObject* pkTransformRelativeTo)
{
    if (m_pkOrbitScene == NULL)
        return;

    m_pkOrbitRoot->SetTranslate(NiPoint3::ZERO);
    m_pkOrbitRoot->SetRotate(NiMatrix3::IDENTITY);
    m_pkOrbitRoot->SetScale(1.0f);

    m_pkOrbitScene->SetRotate(NiMatrix3::IDENTITY);
    m_pkOrbitScene->SetTranslate(NiPoint3::ZERO);
    m_pkOrbitScene->SetScale(1.0f);
    m_pkOrbitRoot->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkOrbitRoot);
    m_pkOrbitRoot->UpdateNodeBound();
    MUtility::ScaleToFit(m_pkOrbitScene, pkSceneToFit, 0.05f, true, false);
    NiBound kBound;
    
    MUtility::GetWorldBounds(pkSceneToFit, kBound, false);
    NiPoint3 kSceneCenter = kBound.GetCenter();
    NiTransform kInvParentWorld;
    
    pkTransformRelativeTo->GetWorldTransform().Invert(kInvParentWorld);
    m_pkOrbitScene->SetTranslate(kInvParentWorld * kSceneCenter);

    m_pkFollowObj = pkTransformRelativeTo;
        
    Update(0.0f);

}
//---------------------------------------------------------------------------
void MOrbitPoint::GetOrbitBound(NiBound& kBound)
{
    kBound.SetCenter(NiPoint3::ZERO);
    kBound.SetRadius(0.0f);
    if (m_pkOrbitScene)
    {
        for (unsigned int ui = 0; ui < m_pkOrbitScene->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = m_pkOrbitScene->GetAt(ui);
            if (pkChild)
            {
                if (kBound.GetRadius() == 0)
                    kBound = pkChild->GetWorldBound();
                else
                    kBound.Merge(&pkChild->GetWorldBound());
            }
        }
    }
}
//---------------------------------------------------------------------------
void MOrbitPoint::GetParentWorldTransform(NiTransform& kTransform)
{
    kTransform.m_Translate = m_pkOrbitRoot->GetTranslate();
    kTransform.m_Rotate = m_pkOrbitRoot->GetRotate();
    kTransform.m_fScale = m_pkOrbitRoot->GetScale();
}
//---------------------------------------------------------------------------
void MOrbitPoint::SetParentWorldTransform(NiTransform kTransform)
{
    m_pkOrbitRoot->SetTranslate(kTransform.m_Translate);
    m_pkOrbitRoot->SetRotate(kTransform.m_Rotate);
    m_pkOrbitRoot->SetScale(kTransform.m_fScale);
}
//---------------------------------------------------------------------------
void MOrbitPoint::SetTranslate(NiPoint3 kPt)
{
    m_pkOrbitScene->SetTranslate(kPt);
}
//---------------------------------------------------------------------------
void MOrbitPoint::GetTranslate(NiPoint3 kPt)
{
    kPt = m_pkOrbitScene->GetTranslate();
}
//---------------------------------------------------------------------------
void MOrbitPoint::GetWorldTranslate(NiPoint3& kPt)
{
    kPt = m_pkOrbitScene->GetWorldTranslate();
}
//---------------------------------------------------------------------------
void MOrbitPoint::Update(float fTime)
{
    if (m_pkOrbitRoot != NULL && m_pkFollowObj != NULL && m_bFollow)
    {
        NiTransform kTransform;
        kTransform.m_fScale = 1.0f;
        kTransform.m_Rotate = NiMatrix3::IDENTITY;
        kTransform.m_Translate = m_pkFollowObj->GetWorldTranslate();
        SetParentWorldTransform(kTransform);
    }
    
    m_pkOrbitRoot->Update(fTime);
    NiMesh::CompleteSceneModifiers(m_pkOrbitRoot);
}
//---------------------------------------------------------------------------
bool MOrbitPoint::get_AppCulled()
{
    if (m_pkOrbitScene == NULL)
        return NULL;

    return m_pkOrbitScene->GetAppCulled();
}
//---------------------------------------------------------------------------
void MOrbitPoint::set_AppCulled(bool bAppCulled)
{
    if (m_pkOrbitScene)
        m_pkOrbitScene->SetAppCulled(bAppCulled);
}
//---------------------------------------------------------------------------
bool MOrbitPoint::get_Follow()
{
    return m_bFollow;
}
//---------------------------------------------------------------------------
void MOrbitPoint::set_Follow(bool bFollow)
{
    m_bFollow = bFollow;
}
//---------------------------------------------------------------------------
bool MOrbitPoint::get_Active()
{
    return m_pkFollowObj != NULL;
}
//---------------------------------------------------------------------------
