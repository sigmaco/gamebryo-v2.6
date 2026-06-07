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
#include "NiAnimationPCH.h"

#include "NiBlendFloatInterpolator.h"
#include "NiFloatInterpolator.h"
#include "NiConstFloatEvaluator.h"
#include "NiMorphWeightsController.h"
#include <NiCloningProcess.h>
#include <NiMesh.h>

NiImplementRTTI(NiMorphWeightsController, NiInterpController);

//---------------------------------------------------------------------------
NiMorphWeightsController::NiMorphWeightsController(NiUInt32 uiNumTargets)
    : m_kInterpolators(uiNumTargets), m_kTargetNames(uiNumTargets),
    m_pkWeightsStream(NULL), m_pkModifier(NULL), m_uiNumTargets(uiNumTargets),
    m_fLastUpdateValueTime(-NI_INFINITY), 
    m_usUpdateValueEndIndex(INVALID_INDEX), 
    m_pfUpdateValueWeightData(NULL)
{
    ReallocUpdateValueWeightData();
}
//---------------------------------------------------------------------------
NiMorphWeightsController::NiMorphWeightsController()
    : m_pkWeightsStream(NULL), m_pkModifier(NULL), m_uiNumTargets(0),
    m_fLastUpdateValueTime(-NI_INFINITY), 
    m_usUpdateValueEndIndex(INVALID_INDEX), 
    m_pfUpdateValueWeightData(NULL)
{
}
//---------------------------------------------------------------------------
NiMorphWeightsController::~NiMorphWeightsController()
{
    m_kInterpolators.RemoveAll();

    NiFree(m_pfUpdateValueWeightData);
    m_pfUpdateValueWeightData = NULL;
}
//---------------------------------------------------------------------------
bool NiMorphWeightsController::GetMeshProperties()
{
    NIASSERT(m_pkTarget && NiIsKindOf(NiMesh, m_pkTarget));
    NiMesh* pkMesh = (NiMesh*)m_pkTarget;

    NiDataStreamRef* pkMWRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::MORPHWEIGHTS());   

    if (!pkMWRef)
    {
        NiOutputDebugString("NiMorphWeightsController Error: "
            "NiMesh doesn't contain a streamref MORPHWEIGHTS\n");
        return false;
    }
    
    m_pkWeightsStream = pkMWRef->GetDataStream();

    m_pkModifier = (NiMorphMeshModifier*)
        pkMesh->GetModifierByType(&NiMorphMeshModifier::ms_RTTI);
        
    if (!m_pkModifier)
    {
        NiOutputDebugString("NiMorphWeightsController Error: "
            "NiMesh doesn't contain a NiMorphMeshModifier\n");
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::GetTargetName(const NiUInt16 uiIndex,
    NiFixedString& kName) const
{
    NIASSERT(uiIndex < GetInterpolatorCount());
    if (uiIndex < m_kTargetNames.GetSize())
        kName = m_kTargetNames.GetAt(uiIndex);
    else
        kName = NULL;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::SetTargetName(const NiUInt16 uiIndex,
    const NiFixedString& kName)
{
    NIASSERT(uiIndex < GetInterpolatorCount());
    m_kTargetNames.SetAtGrow(uiIndex, kName);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiInterpController methods
//---------------------------------------------------------------------------
void NiMorphWeightsController::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    m_fLoKeyTime = NI_INFINITY;
    m_fHiKeyTime = -NI_INFINITY;

    NiUInt32 uiCount = m_kInterpolators.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
        if (pkInterp)
        {
            float fHi = -NI_INFINITY;
            float fLo = NI_INFINITY;
            pkInterp->GetActiveTimeRange(fLo, fHi);
            
            if (fLo < m_fLoKeyTime)
                m_fLoKeyTime = fLo;
            if (fHi > m_fHiKeyTime)
                m_fHiKeyTime = fHi;
        }
    }
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::Update(float fTime) 
{
    if (GetManagerControlled() ||
        ((!m_pkWeightsStream || !m_pkModifier) && !GetMeshProperties()))
    {
        return;
    }

    bool bAlwaysUpdate = m_pkModifier->GetAlwaysUpdate();
    bool bRelative = m_pkModifier->GetRelativeTargets();

    if (!DontDoUpdate(fTime) || bAlwaysUpdate)
    {
        m_pkModifier->SetNeedsUpdate();
    }
    else
    {
        // If we don't need to update simply return.
        return;
    }

    // Ensure that the morph weights are not in use by the modifier
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    m_pkModifier->CompleteTasks((NiMesh*)m_pkTarget, &kSyncArgs);
    
    float* pfWeights = (float*)m_pkWeightsStream->Lock(
        NiDataStream::LOCK_WRITE);

    for (NiUInt32 ui = 0; ui < m_uiNumTargets; ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
        
        float fFrac = pfWeights[ui];
        if (ui == 0 && bRelative)
        {
            fFrac = 1.0f;
        }
        else if (pkInterp)
        {
            if (!pkInterp->Update(m_fScaledTime, m_pkTarget, fFrac))
            {
                // If the interpolator update failed for whatever reason,
                // leave the target weight alone.
                continue;
            }
        }
        else
        {
            continue;
        }

        pfWeights[ui] = fFrac;
    }

    m_pkWeightsStream->Unlock(NiDataStream::LOCK_WRITE);
}
//---------------------------------------------------------------------------
bool NiMorphWeightsController::UpdateValue(float fTime, float fFloat, 
    unsigned short usIndex)
{
    NIASSERT(GetManagerControlled());
    NIASSERT(usIndex < m_uiNumTargets);

    if ((!m_pkWeightsStream || !m_pkModifier) && !GetMeshProperties())
    {
        return true;
    }

    // This implementation requires UpdateValue be called in order
    // by index value, either from 0 to (m_uiNumTargets - 1) or from
    // (m_uiNumTargets - 1) to 0, for the current time.

    // Store the value in the local weight array.
    NIASSERT(m_pfUpdateValueWeightData);
    m_pfUpdateValueWeightData[usIndex] = fFloat;

    // Check if this is a new update time.
    if (fTime != m_fLastUpdateValueTime)
    {
        m_fLastUpdateValueTime = fTime;
        if (usIndex == 0)
        {
            m_usUpdateValueEndIndex = (unsigned short)(m_uiNumTargets - 1);
        }
        else
        {
            NIASSERT(usIndex == m_uiNumTargets - 1);
            m_usUpdateValueEndIndex = 0;
        }
    }

    // Check if all the values in the local weight array have been updated.
    if (usIndex == m_usUpdateValueEndIndex)
    {
        m_pkModifier->SetNeedsUpdate();

        // Check for use of relative targets.
        if (m_pkModifier->GetRelativeTargets())
        {
            m_pfUpdateValueWeightData[0] = 1.0f;
        }

        // Ensure that the morph weights are not in use by the modifier
        NiSyncArgs kSyncArgs;
        kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
        kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
        m_pkModifier->CompleteTasks((NiMesh*)m_pkTarget, &kSyncArgs);
        
        // Copy the local weight array.
        float* pfWeights = (float*)m_pkWeightsStream->Lock(
            NiDataStream::LOCK_WRITE);
        for (NiUInt32 ui = 0; ui < m_uiNumTargets; ui++)
        {
            pfWeights[ui] = m_pfUpdateValueWeightData[ui];
        }
        m_pkWeightsStream->Unlock(NiDataStream::LOCK_WRITE);

        // Reset the last update time to prevent another update
        // on the next end index at this same time.
        m_fLastUpdateValueTime = -NI_INFINITY;
        m_usUpdateValueEndIndex = INVALID_INDEX;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::SetTarget(NiObjectNET* pkTarget)
{
    if (NiIsKindOf(NiMesh, pkTarget))
    {
        NiMesh* pkMesh = (NiMesh*)pkTarget;

        NiDataStreamRef* pkMWRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::MORPHWEIGHTS());
            
        m_pkModifier = NiGetModifier(NiMorphMeshModifier, pkMesh);
        
        if (pkMWRef)
        {
            if (pkMWRef->GetTotalCount() != m_uiNumTargets)
            {
                NiOutputDebugString(
                    "NiMorphWeightsController::SetTarget Error: Mesh "
                    "morph weights stream has too few entries.");
                NiTimeController::SetTarget(NULL);
                return;
            }
            m_pkWeightsStream = pkMWRef->GetDataStream();
        }
        else
        {
            m_pkWeightsStream = 0;
        }
        
        NiTimeController::SetTarget(pkTarget);
    }
    else
    {
        NiOutputDebugString("NiMorphWeightsController Error> "
            "Target is not an NiMesh.");
        NiTimeController::SetTarget(NULL);
    }
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::GuaranteeTimeRange(float fStartTime,
    float fEndTime)
{
    NiUInt32 uiCount = m_kInterpolators.GetSize();
    for (NiUInt32 uiTarget = 0; uiTarget < uiCount;  uiTarget++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(uiTarget);
        if (pkInterp)
            pkInterp->GuaranteeTimeRange(fStartTime, fEndTime);
    }
}
//---------------------------------------------------------------------------
unsigned short NiMorphWeightsController::GetInterpolatorCount() const
{
    return (unsigned short)m_uiNumTargets;
}
//---------------------------------------------------------------------------
const char* NiMorphWeightsController::GetInterpolatorID(unsigned short 
    usIndex) 
{
    NIASSERT(usIndex < GetInterpolatorCount());

    if (usIndex < m_kTargetNames.GetSize())
        return (const char*)m_kTargetNames.GetAt(usIndex);
        
    return NULL;
}
//---------------------------------------------------------------------------
unsigned short NiMorphWeightsController::GetInterpolatorIndex(
    const char* pcID) const
{
    if (pcID == NULL)
        return INVALID_INDEX;

    NiUInt16 uiCount = (NiUInt16) m_kTargetNames.GetSize();
    for (NiUInt16 ui = 0; ui < uiCount; ui++)
    {
        NiFixedString kName;

        GetTargetName(ui, kName);
        
        if (!kName.Exists())
            continue;
        
        if (!NiStricmp(pcID, (const char*)kName))
        {
            return ui;
        }
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiInterpolator* NiMorphWeightsController::GetInterpolator(
    unsigned short usIndex) const
{
    NIASSERT(usIndex < GetInterpolatorCount());
    if (usIndex < m_kInterpolators.GetSize())
        return m_kInterpolators.GetAt(usIndex);
    return NULL;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::SetInterpolator(NiInterpolator* pkInterpolator,
    unsigned short usIndex)
{
    NIASSERT(usIndex < GetInterpolatorCount());
    NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
        usIndex));
    m_kInterpolators.SetAtGrow(usIndex, pkInterpolator);
}
//---------------------------------------------------------------------------
NiEvaluator* NiMorphWeightsController::CreatePoseEvaluator(
    unsigned short usIndex)
{
    NIASSERT(m_pkWeightsStream);
    NIASSERT(usIndex < m_pkWeightsStream->GetTotalCount());
    
    const float* pfWeights = 
        (const float*)m_pkWeightsStream->Lock(NiDataStream::LOCK_READ);
    float fValue = pfWeights[usIndex];
    m_pkWeightsStream->Unlock(NiDataStream::LOCK_READ);

    return NiNew NiConstFloatEvaluator(fValue);
}
//---------------------------------------------------------------------------
NiInterpolator* NiMorphWeightsController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(m_pkWeightsStream);
    NIASSERT(usIndex < m_pkWeightsStream->GetTotalCount());
    
    const float* pfWeights = 
        (const float*)m_pkWeightsStream->Lock(NiDataStream::LOCK_READ);
    float fValue = pfWeights[usIndex];
    m_pkWeightsStream->Unlock(NiDataStream::LOCK_READ);

    NiFloatInterpolator* pkPoseInterp = NiNew NiFloatInterpolator(fValue);
    return pkPoseInterp;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::SynchronizePoseInterpolator(
    NiInterpolator* pkInterp, unsigned short usIndex)
{
    NIASSERT(m_pkWeightsStream);
    NIASSERT(usIndex < m_pkWeightsStream->GetTotalCount());
    
    const float* pfWeights = 
        (const float*)m_pkWeightsStream->Lock(NiDataStream::LOCK_READ);
    float fValue = pfWeights[usIndex];
    m_pkWeightsStream->Unlock(NiDataStream::LOCK_READ);

    NiFloatInterpolator* pkPoseInterp = 
        NiDynamicCast(NiFloatInterpolator, pkInterp);
    NIASSERT(pkPoseInterp);
    pkPoseInterp->SetPoseValue(fValue);
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiMorphWeightsController::CreateBlendInterpolator(
    unsigned short, bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    return NiNew NiBlendFloatInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
bool NiMorphWeightsController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short) const
{
    NIASSERT(pkInterpolator);
    return pkInterpolator->IsFloatValueSupported();
}
//---------------------------------------------------------------------------
bool NiMorphWeightsController::TargetIsRequiredType() const
{
    if (!NiIsKindOf(NiMesh, m_pkTarget))
        return false;
        
    NiMesh* pkMesh = (NiMesh*)m_pkTarget;
    const NiDataStreamRef* pkDSRef =
        pkMesh->FindStreamRef(NiCommonSemantics::MORPHWEIGHTS());
    return pkDSRef != NULL;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::ResetModifierData()
{
    // Set pointers to NULL causing later re-evaluation.
    m_pkWeightsStream = NULL;
    m_pkModifier = NULL;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMorphWeightsController);
//---------------------------------------------------------------------------
void NiMorphWeightsController::CopyMembers(NiMorphWeightsController* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpController::CopyMembers(pkDest, kCloning);
    
    pkDest->m_uiNumTargets = m_uiNumTargets;
    pkDest->m_kInterpolators.SetSize(m_uiNumTargets);
    pkDest->m_kTargetNames.SetSize(m_uiNumTargets);
    pkDest->ReallocUpdateValueWeightData();
    
    pkDest->m_pkWeightsStream = 0;
    pkDest->m_pkModifier = 0;

    NiUInt32 uiCount = m_kInterpolators.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
        if (pkInterp)
        {
            pkDest->m_kInterpolators.Add(
                (NiInterpolator*)pkInterp->CreateSharedClone(kCloning));
        }
        else
        {
            pkDest->m_kInterpolators.Add(0);
        }
    }

    uiCount = m_kTargetNames.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        pkDest->m_kTargetNames.Add(m_kTargetNames.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::ProcessClone(NiCloningProcess& kCloning)
{
    NiInterpController::ProcessClone(kCloning);

    // Get the clone
    NiObject* pkCloneObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkCloneObject));
    
    NiUInt32 uiCount = m_kInterpolators.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
        if (pkInterp)
        {
            pkInterp->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMorphWeightsController);
//---------------------------------------------------------------------------
void NiMorphWeightsController::LoadBinary(NiStream& kStream)
{
    NiInterpController::LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_uiNumTargets);
    m_kInterpolators.SetSize(m_uiNumTargets);
    m_kTargetNames.SetSize(m_uiNumTargets);
    ReallocUpdateValueWeightData();

    NiUInt32 uiCount = 0;
    NiStreamLoadBinary(kStream, uiCount);
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        m_kInterpolators.Add((NiInterpolator*)kStream.ResolveLinkID());
    }

    NiStreamLoadBinary(kStream, uiCount);
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiFixedString kTargetName;
        kStream.LoadFixedString(kTargetName);
        m_kTargetNames.Add(kTargetName);
    }
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::LinkObject(NiStream& kStream)
{
    NiInterpController::LinkObject(kStream);

    if (NiIsKindOf(NiMesh, m_pkTarget))
    {
        ResetModifierData();
    }
}
//---------------------------------------------------------------------------
bool NiMorphWeightsController::RegisterStreamables(NiStream& kStream)
{
    if (!NiInterpController::RegisterStreamables(kStream))
        return false;

    for (NiUInt32 ui = 0; ui < m_kInterpolators.GetSize(); ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
        if (pkInterp)
            pkInterp->RegisterStreamables(kStream);
    }

    for (NiUInt32 ui = 0; ui < m_kTargetNames.GetSize(); ui++)
    {
        kStream.RegisterFixedString(m_kTargetNames.GetAt(ui));
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::SaveBinary(NiStream& kStream)
{
    NiInterpController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiNumTargets);

    NiStreamSaveBinary(kStream, m_kInterpolators.GetSize());
    for (unsigned int ui = 0; ui < m_kInterpolators.GetSize(); ui++)
    {
        kStream.SaveLinkID(m_kInterpolators.GetAt(ui));
    }

    NiStreamSaveBinary(kStream, m_kTargetNames.GetSize());
    for (unsigned int ui = 0; ui < m_kTargetNames.GetSize(); ui++)
    {
        kStream.SaveFixedString(m_kTargetNames.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiMorphWeightsController::IsEqual(NiObject* pkObject)
{
    if (!NiInterpController::IsEqual(pkObject))
        return false;

    NiMorphWeightsController* pkMorph = (NiMorphWeightsController*) pkObject;

    if (m_uiNumTargets != pkMorph->m_uiNumTargets)
        return false;

    if (m_kInterpolators.GetSize() != pkMorph->m_kInterpolators.GetSize())
        return false;
        
    for (NiUInt32 ui = 0; ui < m_kInterpolators.GetSize(); ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
        NiInterpolator* pkOtherInterp = pkMorph->m_kInterpolators.GetAt(ui);
        if ((pkInterp && !pkOtherInterp) ||
            (!pkInterp && pkOtherInterp) ||
            (pkInterp && !pkInterp->IsEqual(pkOtherInterp)))
        {
            return false;
        }
    }

    if (pkMorph->m_kTargetNames.GetSize() != m_kTargetNames.GetSize())
        return false;

    for (NiUInt32 ui = 0; ui < m_kTargetNames.GetSize(); ui++)
    {
        if (m_kTargetNames.GetAt(ui) != pkMorph->m_kTargetNames.GetAt(ui))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiMorphWeightsController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(
        NiGetViewerString(NiMorphWeightsController::ms_RTTI.GetName()));
        
    pkStrings->Add(NiGetViewerString("Modifier ", (void*)m_pkModifier));
    pkStrings->Add(
        NiGetViewerString("Weights Stream ", (void*)m_pkWeightsStream));
        
    pkStrings->Add(NiGetViewerString("Num Targets ", m_uiNumTargets));
    for (NiUInt32 ui = 0; ui < m_uiNumTargets; ui++)
    {
        NiInterpolator* pkInterp = m_kInterpolators.GetAt(ui);
            
        char pcPrefix[64];
        NiSprintf(pcPrefix, 64, "Interpolator %d", ui);
        pkStrings->Add(NiGetViewerString(pcPrefix, (void*)pkInterp));
        
        NiSprintf(pcPrefix, 64, "Name %d", ui);
        if (ui < m_kTargetNames.GetSize())
        {
            pkStrings->Add(
                NiGetViewerString(pcPrefix, m_kTargetNames.GetAt(ui)));
        }
        else
        {
            pkStrings->Add(NiGetViewerString(pcPrefix, "NULL"));
        }
    }
}
//---------------------------------------------------------------------------
void NiMorphWeightsController::ReallocUpdateValueWeightData()
{
    NiFree(m_pfUpdateValueWeightData);
    m_pfUpdateValueWeightData = NULL;

    if (m_uiNumTargets > 0)
    {
        m_pfUpdateValueWeightData = NiAlloc(float, m_uiNumTargets);
    }
}
//---------------------------------------------------------------------------
