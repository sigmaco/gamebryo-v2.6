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

#include "NiSceneGraphSharedData.h"
#include "NiTimerSharedData.h"
#include "NiSharedDataList.h"
#include "NiRendererSharedData.h"
#include <NiMesh.h>

NiImplementRTTI(NiSceneGraphSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiSceneGraphSharedData::NiSceneGraphSharedData() 
{
    m_spMainRoot = NiNew NiNode;
    m_spMainRoot->SetSelectiveUpdate(true);
    m_spMainRoot->SetSelectiveUpdateTransforms(true);
    m_spMainRoot->SetSelectiveUpdatePropertyControllers(true);
    m_spMainRoot->SetSelectiveUpdateRigid(false);

    m_bPreserveEndian = false;
    m_bLittleEndian = true;

    m_pkMeshProfileProcessor = NULL;

    UpdateAssociatedSharedData();
}
//---------------------------------------------------------------------------
NiSceneGraphSharedData::~NiSceneGraphSharedData()
{
    NiSharedDataList::LockSharedData();
    NiSharedDataList* pkSharedData = NiSharedDataList::GetInstance();
    if (!pkSharedData)
    {
        return;
    }

    NiRendererSharedData* pkRendererData = (NiRendererSharedData*) 
        pkSharedData->Get(NiGetSharedDataType(NiRendererSharedData));

    if (pkRendererData)
        pkRendererData->PurgeRendererData(m_spMainRoot);
    NiSharedDataList::UnlockSharedData();

    m_spMainRoot = NULL;
    m_kSceneRoots.RemoveAll();
    m_kNodeInfoMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiSceneGraphSharedData::AddRoot(NiNode* pkRoot, const char* pcName,
    bool bStreamable)
{
    if (pkRoot)
    {
        m_spMainRoot->AttachChild(pkRoot);
        m_kSceneRoots.Add(pkRoot);
        NodeInfo* pkInfo = NiNew NodeInfo(pcName, bStreamable);
        m_kNodeInfoMap.SetAt(pkRoot, pkInfo);

        pkRoot->Update(GetLastUpdateTime(pkRoot));
        NiMesh::CompleteSceneModifiers(pkRoot);
        pkRoot->UpdateProperties();
        pkRoot->UpdateEffects();

        UpdateAssociatedSharedData();
    }
}
//---------------------------------------------------------------------------
NiNodePtr NiSceneGraphSharedData::RemoveRoot(NiNode* pkRoot)
{
    if (!pkRoot)
    {
        return 0;
    }

    NiNodePtr spOriginalRoot = 0;

    for (unsigned int ui = 0; ui < m_kSceneRoots.GetSize(); ui++)
    {
        if (m_kSceneRoots.GetAt(ui) == pkRoot)
        {
            NiRenderer* pkRenderer = NiRenderer::GetRenderer();
            if (pkRenderer)
                pkRenderer->PurgeAllRendererData(pkRoot);

            spOriginalRoot = pkRoot;
            m_spMainRoot->DetachChild(pkRoot);
            m_kSceneRoots.OrderedRemoveAt(ui);
            m_kNodeInfoMap.RemoveAt(pkRoot);
            break;
        }
    }

    return spOriginalRoot;
}
//---------------------------------------------------------------------------
void NiSceneGraphSharedData::RemoveAllRoots()
{
    for (unsigned int ui = 0; ui < m_kSceneRoots.GetSize(); ui++)
    {
        NiNode* pkRoot = m_kSceneRoots.GetAt(ui);
        RemoveRoot(pkRoot);
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphSharedData::UpdateAll(float fTime)
{
    for (unsigned int ui = 0; ui < m_kSceneRoots.GetSize(); ui++)
    {
        NiNode* pkRoot = m_kSceneRoots.GetAt(ui);
        UpdateRoot(pkRoot, fTime);
    }

    NiSharedDataList* pkSharedData = NiSharedDataList::GetInstance();
    if (!pkSharedData)
    {
        return;
    }

    NiTimerSharedData* pkTimerData = (NiTimerSharedData*) 
        pkSharedData->Get(NiGetSharedDataType(NiTimerSharedData));
    if (!pkTimerData)
    {
        pkTimerData = NiNew NiTimerSharedData;
        pkSharedData->Insert(pkTimerData);
    }

    pkTimerData->SetCurrentTime(fTime);
}
//---------------------------------------------------------------------------
void NiSceneGraphSharedData::UpdateRoot(NiNode* pkRoot, float fTime)
{
    pkRoot->Update(fTime);
    NiMesh::CompleteSceneModifiers(pkRoot);

    NodeInfoPtr spInfo;
    if (m_kNodeInfoMap.GetAt(pkRoot, spInfo))
    {
        spInfo->m_fLastUpdateTime = fTime;
    }
}
//---------------------------------------------------------------------------
unsigned int NiSceneGraphSharedData::GetRootCount()
{
    return m_kSceneRoots.GetSize();
}
//---------------------------------------------------------------------------
NiNode* NiSceneGraphSharedData::GetRootAt(unsigned int uiIndex)
{
    if (uiIndex >= m_kSceneRoots.GetSize())
    {
        return NULL;
    }

    return m_kSceneRoots.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiString NiSceneGraphSharedData::GetRootName(NiNode* pkRoot)
{
    NiString strRootName;

    if (!pkRoot)
    {
        return strRootName;
    }

    NodeInfoPtr spInfo;
    if (m_kNodeInfoMap.GetAt(pkRoot, spInfo))
    {
        strRootName = spInfo->m_strName;
    }

    return strRootName;
}
//---------------------------------------------------------------------------
bool NiSceneGraphSharedData::IsRootStreamable(NiNode* pkRoot)
{
    if (!pkRoot)
    {
        return false;
    }

    NodeInfoPtr spInfo;
    if (m_kNodeInfoMap.GetAt(pkRoot, spInfo))
    {
        return spInfo->m_bStreamable;
    }

    return false;
}
//---------------------------------------------------------------------------
float NiSceneGraphSharedData::GetLastUpdateTime(NiNode* pkRoot)
{
    if (!pkRoot)
    {
        return -1.0f;
    }

    NodeInfoPtr spInfo;
    if (m_kNodeInfoMap.GetAt(pkRoot, spInfo))
    {
        return spInfo->m_fLastUpdateTime;
    }

    return -1.0f;
}
//---------------------------------------------------------------------------
NiNodePtr NiSceneGraphSharedData::GetFullSceneGraph()
{
    return m_spMainRoot;
}
//---------------------------------------------------------------------------
void NiSceneGraphSharedData::UpdateAssociatedSharedData()
{
    NiSharedDataList* pkSharedData = NiSharedDataList::GetInstance();
    if (!pkSharedData)
    {
        return;
    }

    NiTimerSharedData* pkTimerData = (NiTimerSharedData*) 
        pkSharedData->Get(NiGetSharedDataType(NiTimerSharedData));
    if (pkTimerData)
    {
        pkTimerData->CollectData(GetFullSceneGraph());
    }

}
//---------------------------------------------------------------------------
bool NiSceneGraphSharedData::GetPreserveEndianness()
{
    return m_bPreserveEndian;
}
//---------------------------------------------------------------------------
bool NiSceneGraphSharedData::GetSourceLittleEndian()
{
    return m_bLittleEndian;
}
//---------------------------------------------------------------------------
void NiSceneGraphSharedData::SetSourceLittleEndian(bool bLittleEndian, 
    bool bPreserve)
{
    m_bLittleEndian = bLittleEndian;
    m_bPreserveEndian = bPreserve;
}
//---------------------------------------------------------------------------