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

#include "NiPhysXSharedData.h"

NiImplementRTTI(NiPhysXSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiPhysXSharedData::NiPhysXSharedData()
{
    m_spScene = 0; 
}
//---------------------------------------------------------------------------
NiPhysXSharedData::~NiPhysXSharedData()
{
    m_kProps.RemoveAll();
    m_spScene = 0;

    NiUInt32 uiCount = m_kStates.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        StateInfo* pkState = m_kStates.GetAt(ui);
        NiDelete pkState;
    }
}
//---------------------------------------------------------------------------
unsigned int NiPhysXSharedData::GetPropCount()
{
    return m_kProps.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXProp* NiPhysXSharedData::GetPropAt(unsigned int uiIndex)
{
    return m_kProps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::AddProp(NiPhysXProp* pkProp)
{
    m_kProps.Add(pkProp);
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::RemoveProp(NiPhysXProp* pkProp)
{
    m_kProps.Remove(pkProp);
}
//---------------------------------------------------------------------------
NiPhysXScene* NiPhysXSharedData::GetScene()
{
    return m_spScene;
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::SetScene(NiPhysXScene* pkScene)
{
    NIASSERT(pkScene);
    m_spScene = pkScene;
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXSharedData::GetStateCount()
{
    return m_kStates.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXSharedData::StateInfo& NiPhysXSharedData::GetStateAt(NiUInt32 uiIndex)
{
    return *(m_kStates.GetAt(uiIndex));
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::AddState(const NiPhysXSharedData::StateInfo& kState)
{
    StateInfo* pkState = NiNew StateInfo;
    pkState->m_fStateTime = kState.m_fStateTime;
    pkState->m_kStateName = kState.m_kStateName;
    m_kStates.Add(pkState);
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::RemoveState(const NiPhysXSharedData::StateInfo& kState)
{
    NiUInt32 uiCount = m_kStates.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        StateInfo* pkThisState = m_kStates.GetAt(ui);
        if (pkThisState->m_kStateName == kState.m_kStateName)
        {
            m_kStates.RemoveAtAndFill(ui);
            return;
        }
    }
}
//---------------------------------------------------------------------------
