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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiRenderObject.h"
#include "NiObjectNET.h"
#include "NiTimeSyncController.h"
#include "NiShaderConstantMap.h"

NiImplementRTTI(NiTimeSyncController,NiShaderTimeController);

//---------------------------------------------------------------------------
NiTimeSyncController::~NiTimeSyncController()
{
    m_spExtraData = NULL;
}
//---------------------------------------------------------------------------
void NiTimeSyncController::Update (float fTime)
{
    if (!m_pkTarget)
        return;

    if (!m_pkTarget || !m_spExtraData || DontDoUpdate(fTime))
    {
        return;
    }

    m_spExtraData->SetValue(fTime);
}
//---------------------------------------------------------------------------
void NiTimeSyncController::GuaranteeKeysAtStartAndEnd(float, float)
{
    // There are no keys, so don't do anything.
}
//---------------------------------------------------------------------------
bool NiTimeSyncController::IsStreamable() const
{
    return false;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTimeSyncController);
//---------------------------------------------------------------------------
void NiTimeSyncController::CopyMembers(
    NiTimeSyncController* pkDest,
    NiCloningProcess& kCloning)
{
    NiShaderTimeController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTimeSyncController);
//---------------------------------------------------------------------------
void NiTimeSyncController::LoadBinary(NiStream&)
{
    // This should never get called, since these objects are not streamable
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiTimeSyncController::LinkObject(NiStream&)
{
    // This should never get called, since these objects are not streamable
    NIASSERT(false);
}
//---------------------------------------------------------------------------
bool NiTimeSyncController::RegisterStreamables(
    NiStream& kStream)
{
    // Do *NOT* call NiShaderTimeController::RegisterStreamables, since we
    // explicitly do not want to stream this object. Instead, continue 
    // on to the next NiTimeController on the list.

    if (m_spNext)
        m_spNext->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiTimeSyncController::SaveBinary(NiStream&)
{
    // This should never get called, since these objects are not streamable
    NIASSERT(false);
}
//---------------------------------------------------------------------------
bool NiTimeSyncController::IsEqual(NiObject* pObject)
{
    if (!NiShaderTimeController::IsEqual(pObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTimeSyncController::GetViewerStrings(
    NiViewerStringsArray* pStrings)
{
    pStrings->Add(NiGetViewerString(
        NiTimeSyncController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
void NiTimeSyncController::SetTarget(NiObjectNET* pkTarget)
{
    NiShaderTimeController::SetTarget(pkTarget);

    if (pkTarget != NULL)
    {
        NiExtraData* pkPrevED = pkTarget->GetExtraData(
            NiShaderConstantMap::GetTimeExtraDataName());
        if (pkPrevED != NULL && NiIsKindOf(NiFloatExtraData, pkPrevED))
        {
            m_spExtraData = (NiFloatExtraData*) pkPrevED;
        }
        else
        {
            m_spExtraData = NiNew NiFloatExtraData(0.0f);
            m_spExtraData->SetName(
                NiShaderConstantMap::GetTimeExtraDataName());
            pkTarget->AddExtraData(m_spExtraData);
        }
    }
    
}
//---------------------------------------------------------------------------
bool NiTimeSyncController::TargetIsRequiredType() const
{
    if (!NiIsKindOf(NiRenderObject, m_pkTarget))
    {
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
