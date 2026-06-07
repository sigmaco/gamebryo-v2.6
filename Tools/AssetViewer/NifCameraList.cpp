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

// NifCameraList.cpp

#include "stdafx.h"
#include "NifCameraList.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
const char* NiCameraInfo::ms_kDefaultCameraName = "Default Camera";


//---------------------------------------------------------------------------
NiCamera* CNifCameraList::GetCamera(NiCameraInfoIndex kIndex)
{
    NiCameraInfo* pkInfo = GetCameraInfo(kIndex);
    if(pkInfo != NULL)
        return pkInfo->m_spCam;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiCameraInfo* CNifCameraList::GetCameraInfo(NiCameraInfoIndex kIndex)
{
    if(kIndex.uiRootIndex < m_pkDoc->GetNumberOfRoots())
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCamList = 
            m_pkCameraList.GetAt(kIndex.uiRootIndex);

        if(pkCamList && kIndex.uiCameraIndex < pkCamList->GetSize())
        {
            return pkCamList->GetAt(kIndex.uiCameraIndex);
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiCamera* CNifCameraList::GetCameraFromOriginal(NiCamera* pkCamera) const
{
    if (!pkCamera)
        return NULL;

    for(unsigned int i = 0; i < m_pkCameraList.GetSize(); i++)
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCamList = m_pkCameraList.GetAt(i);
        if (!pkCamList)
            continue;

        for (unsigned int j = 0; j < pkCamList->GetSize(); j++)
        {
            NiCameraInfo* pkInfo = pkCamList->GetAt(j);
            if (pkInfo && pkInfo->m_spOriginalCam == pkCamera)
                return pkInfo->m_spCam;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
unsigned int CNifCameraList::NumCameras()
{
    unsigned int uiCount = 0;

    for(unsigned int ui = 0; ui < m_pkCameraList.GetSize(); ui++)
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCamList = m_pkCameraList.GetAt(ui);
        if(pkCamList)
        {
            uiCount += pkCamList->GetEffectiveSize();
        }
    }

    return uiCount;
}
//---------------------------------------------------------------------------
unsigned int CNifCameraList::NumCamerasForRoot(unsigned int uiWhichRoot)
{
    if(uiWhichRoot >= m_pkCameraList.GetEffectiveSize())
        return 0;
    else
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCamList =
            m_pkCameraList.GetAt(uiWhichRoot);
        if(pkCamList)
            return pkCamList->GetEffectiveSize();
    }
    return 0;
}
//---------------------------------------------------------------------------
void CNifCameraList::AddCamerasForRoot(NiNode* pkRoot)
{
    NiNode* pkNode = pkRoot;
    unsigned int uiWhichRoot = m_pkDoc->GetRootId(pkRoot);
    
    if(uiWhichRoot == (unsigned int) -1)
        return;

    if(uiWhichRoot >= m_pkCameraList.GetSize())
    {
        m_pkCameraList.SetSize(uiWhichRoot+1);
        NiTPrimitiveArray<NiCameraInfo*>* pkList = 
            NiNew NiTPrimitiveArray<NiCameraInfo*>(10);
        m_pkCameraList.SetAt(uiWhichRoot, pkList);
    }
    if(pkNode)
    {
        pkNode->Update(0.0f);
        
        NiMesh::CompleteSceneModifiers(pkNode);
    }

    FindAndAddCameras(uiWhichRoot, pkRoot);    
}
//---------------------------------------------------------------------------
void CNifCameraList::RemoveCamerasForRoot(NiNode* pkRoot, bool bCompact)
{
    NiNode* pkNode = pkRoot;
    unsigned int uiWhichRoot = m_pkDoc->GetRootId(pkRoot);
    if(uiWhichRoot < m_pkCameraList.GetSize())
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCameras =
            m_pkCameraList.GetAt(uiWhichRoot);
        if(pkCameras)
        {
            for(unsigned int ui = 0; ui < pkCameras->GetSize(); ui++)
            {
                NiCameraInfo* pkInfo = pkCameras->GetAt(ui);
                if(pkInfo)
                {
                    delete pkInfo;
                    pkCameras->SetAt(ui, NULL);
                }
            }
            delete pkCameras;
            m_pkCameraList.SetAt(uiWhichRoot, NULL);
            if(bCompact)
                m_pkCameraList.Compact();
        }
    }
}
//---------------------------------------------------------------------------
void CNifCameraList::FindAndAddCameras(unsigned int uiWhichRoot,
    NiNode* pkNode)
{
    if(pkNode == NULL)
        return;
    for(unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkObj = pkNode->GetAt(ui);
        if(NiIsKindOf(NiNode, pkObj))
            FindAndAddCameras(uiWhichRoot, (NiNode*) pkObj);
        else if(NiIsKindOf(NiCamera, pkObj))
            AddCamera(uiWhichRoot, (NiCamera*) pkObj);
    }
}
//---------------------------------------------------------------------------
NiCameraInfoIndex CNifCameraList::AddCamera(unsigned int uiWhichRoot,
    NiCamera* pkCamera)
{
    NiCameraInfoIndex kIndex;
    kIndex.uiCameraIndex = (unsigned int) -1;
    kIndex.uiRootIndex = (unsigned int) -1;
    if(pkCamera && uiWhichRoot < m_pkCameraList.GetEffectiveSize())
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCameras =
            m_pkCameraList.GetAt(uiWhichRoot);
        if(pkCameras)
        {
            NiAVObject* pkObj = (NiAVObject*)pkCamera;

            NiCameraInfo* pkInfo = NiNew NiCameraInfo;
            pkInfo->m_bIsAnimated = m_pkDoc->IsAnimated(pkCamera);
            pkInfo->m_kOriginalRotation = pkCamera->GetWorldRotate();
            pkInfo->m_kOriginalTranslation = pkCamera->GetWorldTranslate();
            
            if(!pkInfo->m_bIsAnimated)
            {
                pkInfo->m_spCam = (NiCamera*) pkCamera->Clone();
                pkInfo->m_spCam->SetRotate(pkInfo->m_kOriginalRotation);
                pkInfo->m_spCam->SetTranslate(pkInfo->m_kOriginalTranslation);
            }
            else
            {
                pkInfo->m_spCam = (NiCamera*) pkCamera;
            }
            
            pkInfo->m_spOriginalCam = pkCamera;
            
            if (pkCamera->GetName() == NiCameraInfo::ms_kDefaultCameraName)
            {
                pkInfo->m_bIsDefault = true;
            }
            else
            {
                pkInfo->m_bIsDefault = false;
            }
            
            NiOutputDebugString("Adding Camera: ");

            while(pkObj && !pkObj->GetName().Exists())
                pkObj = pkObj->GetParent();

            if (pkObj != NULL && !pkInfo->m_spCam->GetName().Exists())
            {
                NiOutputDebugString(pkObj->GetName());
                pkInfo->m_spCam->SetName(pkObj->GetName());
            }

            NiOutputDebugString("\n");

            kIndex.uiRootIndex = uiWhichRoot;
            kIndex.uiCameraIndex = pkCameras->Add(pkInfo);
        }
    }

    return kIndex;
}
//---------------------------------------------------------------------------
void CNifCameraList::RemoveCamera(unsigned int uiWhichRoot,
    NiCamera* pkCamera)
{
    if(pkCamera && uiWhichRoot < m_pkCameraList.GetEffectiveSize())
    {
        NiTPrimitiveArray<NiCameraInfo*>* pkCameras =
            m_pkCameraList.GetAt(uiWhichRoot);
        if(pkCameras)
        {
            for(unsigned int ui = 0; ui < pkCameras->GetSize(); ui++)
            {
                NiCameraInfo* pkInfo = pkCameras->GetAt(ui);
                if( pkInfo && (pkInfo->m_spCam == pkCamera || 
                    pkInfo->m_spOriginalCam == pkCamera))
                {
                    delete pkInfo;
                    pkCameras->SetAt(ui, NULL);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void CNifCameraList::CreateCameraList()
{
}
//---------------------------------------------------------------------------
void CNifCameraList::EmptyCameraList()
{
    if(!m_pkDoc)
        return;

    for(unsigned int ui = 0; ui < m_pkDoc->GetNumberOfRoots(); ui++)
    {
        RemoveCamerasForRoot(m_pkDoc->GetRoot(ui), false);
    }
}
//---------------------------------------------------------------------------
CNifCameraList::CNifCameraList(CNifDoc* pkDoc)
{
    NIASSERT(pkDoc);
    m_pkDoc = pkDoc;
}
//---------------------------------------------------------------------------
CNifCameraList::~CNifCameraList()
{

    EmptyCameraList();
    m_pkDoc = NULL;
}
//---------------------------------------------------------------------------
NiCameraInfoIndex CNifCameraList::GetNextCameraInfoIndex(
    NiCameraInfoIndex kIndex)
{
    bool bEnteredLoop = false;
    bool bKeepLooping = true;
    while(kIndex.uiCameraIndex+1 == 
          NumCamerasForRoot(kIndex.uiRootIndex) &&
          kIndex.uiRootIndex <= m_pkDoc->GetNumberOfRoots() &&
          bKeepLooping)
    {
        if(bEnteredLoop && 
           NumCamerasForRoot(kIndex.uiRootIndex) == 1)
            bKeepLooping = false;
        else
        {
            kIndex.uiCameraIndex = 0;
            kIndex.uiRootIndex++;
            while(NumCamerasForRoot(kIndex.uiRootIndex) == 0 &&
                kIndex.uiRootIndex < m_pkDoc->GetNumberOfRoots())
            {
                kIndex.uiRootIndex++;
            }
            bEnteredLoop = true;
        }
    }

    if(kIndex.uiRootIndex >= m_pkDoc->GetNumberOfRoots())
    {
        kIndex.uiCameraIndex = (unsigned int) -1;
        kIndex.uiRootIndex = (unsigned int) -1;
    }
    else if(kIndex.uiRootIndex <= m_pkDoc->GetNumberOfRoots() &&
        !bEnteredLoop)
    {
        kIndex.uiCameraIndex++;
    }
    else if(kIndex.uiCameraIndex >= NumCamerasForRoot(kIndex.uiRootIndex))
    {
        kIndex.uiCameraIndex = (unsigned int) -1;
        kIndex.uiRootIndex = (unsigned int) -1;
    }
    return kIndex;
}
//---------------------------------------------------------------------------
NiCameraInfoIndex CNifCameraList::GetPreviousCameraInfoIndex(
    NiCameraInfoIndex kIndex)
{
    if(kIndex.uiRootIndex == 0 && kIndex.uiCameraIndex == 0)
    {
        kIndex.uiCameraIndex = (unsigned int ) -1;
        kIndex.uiRootIndex = (unsigned int) -1;
    }

    bool bEnteredLoop = false;
    while(kIndex.uiCameraIndex == 0 &&
          kIndex.uiRootIndex != 0)
    {
        kIndex.uiRootIndex--;
        kIndex.uiCameraIndex = 
            NumCamerasForRoot(kIndex.uiRootIndex) - 1;
        bEnteredLoop = true;
    }

    if(kIndex.uiCameraIndex == (unsigned int) -1)
    {
    }
    else if(!bEnteredLoop && kIndex.uiCameraIndex != 0)
    {
        kIndex.uiCameraIndex--;
    }
    else if(!bEnteredLoop)
    {
        kIndex.uiRootIndex = m_pkDoc->GetNumberOfRoots() - 1;
        while(NumCamerasForRoot(kIndex.uiRootIndex) == 0)
            kIndex.uiRootIndex--;
        kIndex.uiCameraIndex = 
            NumCamerasForRoot(kIndex.uiRootIndex) - 1;
    }

    return kIndex;

}
//---------------------------------------------------------------------------
