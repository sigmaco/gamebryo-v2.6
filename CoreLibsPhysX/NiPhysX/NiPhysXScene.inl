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

#include "NiPhysXSceneDesc.h"

//---------------------------------------------------------------------------
inline NxScene* NiPhysXScene::GetPhysXScene() const
{
    return m_pkPhysXScene;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiPhysXScene::GetSceneXform() const
{
    return m_kSceneXform;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetSceneXform(const NiTransform& kXform)
{
    m_kSceneXform = kXform;
    NotifyAllSceneChanged();
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetScaleFactor() const
{
    return m_fScalePToW;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetScaleFactor(const float fScale)
{
    m_fScalePToW = fScale;
    NotifyAllSceneChanged();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXScene::GetPropCount() const
{
    return m_kProps.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXProp* NiPhysXScene::GetPropAt(const NiUInt32 uiIndex)
{
    return m_kProps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NxCompartment* NiPhysXScene::GetCompartmentAt(NiUInt32 uiID)
{
    NxCompartment* pkResult;
    if (m_kCompartmentMap.GetAt(uiID, pkResult))
        return pkResult;
        
    return 0;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::AddCompartment(NiUInt32 uiID,
    NxCompartment* pkCompartment)
{
    NxCompartment* pkResult;
    if (m_kCompartmentMap.GetAt(uiID, pkResult))
        return false;

    m_kCompartmentMap.SetAt(uiID, pkCompartment);
    
    return true;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetUpdateSrc(void) const
{
    return GetBit(UPDATE_SRC_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetUpdateSrc(const bool bActive)
{
    SetBit(bActive, UPDATE_SRC_MASK);
    NotifyAllSceneChanged();
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetUpdateDest(void) const
{
    return GetBit(UPDATE_DEST_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetUpdateDest(const bool bActive)
{
    SetBit(bActive, UPDATE_DEST_MASK);
    NotifyAllSceneChanged();
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetDoFixedStep(void) const
{
    return GetBit(FIXED_TIMESTEP_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetDoFixedStep(const bool bActive)
{
    SetBit(bActive, FIXED_TIMESTEP_MASK);
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetTimestep(void) const
{
    return m_fTimestep;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetTimestep(const float fStep)
{
    m_fTimestep = fStep;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXScene::GetNumSubSteps() const
{
    return m_uiNumSubSteps;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetNumSubSteps(const NiUInt32 uiNumSteps)
{
    NIASSERT(uiNumSteps > 0);
    m_uiNumSubSteps = uiNumSteps;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXScene::GetMaxSubSteps() const
{
    return m_uiMaxSubSteps;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetMaxSubSteps(const NiUInt32 uiMaxSteps)
{
    NIASSERT(uiMaxSteps >= m_uiNumSubSteps);
    m_uiMaxSubSteps = uiMaxSteps;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetAsynchronousSimulation() const
{
    return GetBit(SIM_PARALLEL_RENDER_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetAsynchronousSimulation(const bool bActive)
{
    SetBit(bActive, SIM_PARALLEL_RENDER_MASK);
    NotifyAllSceneChanged();
}
//---------------------------------------------------------------------------
inline NiSPWorkflowManager* NiPhysXScene::GetWorkflowManager() const
{
    return m_pkWorkflowManager;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetWorkflowManager(
    NiSPWorkflowManager* pkWorkflowManager)
{
    if (GetBit(OWN_MANAGER_MASK))
    {
        NiDelete m_pkWorkflowManager;
        m_pkWorkflowManager = 0;
        SetBit(false, OWN_MANAGER_MASK);
    }
    m_pkWorkflowManager = pkWorkflowManager;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetDebugRender(void) const
{
    return GetBit(DEBUG_RENDER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetInSimFetch() const
{
    return m_bInSimFetch;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetFetchTime(const float fCurrentTime)
{
    m_fPrevFetchTime = fCurrentTime;
    m_fNextFetchTime = fCurrentTime;
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetPrevFetchTime() const
{
    return m_fPrevFetchTime;
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetNextFetchTime() const
{
    return m_fNextFetchTime;
}
//---------------------------------------------------------------------------
inline NiPhysXSceneDesc* NiPhysXScene::GetSnapshot()
{
    return m_spSnapshot;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetSnapshot(NiPhysXSceneDesc* pkSnapshot)
{
    m_spSnapshot = pkSnapshot;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::ReleaseSnapshot()
{
    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::AddModifiedMesh(NiMesh* pkMesh)
{
    m_kModifiedMeshes.AddFirstEmpty(pkMesh);
    NotifySceneChanged(pkMesh);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::DeleteModifiedMesh(NiMesh* pkTarget)
{
    for (NiUInt32 i = 0; i < m_kModifiedMeshes.GetSize(); i++)
        if (m_kModifiedMeshes.GetAt(i) == pkTarget)
        {
            m_kModifiedMeshes.RemoveAtAndFill(i);
            NotifySceneChanged(pkTarget, true);
        }
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::AddSource(NiPhysXSrc* pkSrc)
{
    m_kSources.AddFirstEmpty(pkSrc);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::DeleteSource(NiPhysXSrc* pkTarget)
{
    for (NiUInt32 i = 0; i < m_kSources.GetSize(); i++)
        if (m_kSources.GetAt(i) == pkTarget)
        {
            m_kSources.RemoveAtAndFill(i);
        }
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::AddDestination(NiPhysXDest* pkDest)
{
    m_kDestinations.AddFirstEmpty(pkDest);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::DeleteDestination(NiPhysXDest* pkTarget)
{
    for (NiUInt32 i = 0; i < m_kDestinations.GetSize(); i++)
        if (m_kDestinations.GetAt(i) == pkTarget)
        {
            m_kDestinations.RemoveAtAndFill(i);
        }
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
