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

#include "NiFlipController.h"
#include "NiFloatInterpolator.h"
#include "NiBlendFloatInterpolator.h"
#include "NiStepFloatKey.h"
#include "NiEvaluator.h"
#include "NiControllerSequence.h"

NiImplementRTTI(NiFlipController, NiFloatInterpController);
#define SHADER_MAP_OFFSET 1024
//---------------------------------------------------------------------------
NiFlipController::NiFlipController()
    :
    m_kTextures(1,1),
    m_uiIndex(0),
    m_uiAffectedMap(0),
    m_pcCtlrID(NULL)
{ 
}
//---------------------------------------------------------------------------
NiFlipController::~NiFlipController()
{ 
    ResetCtlrID();
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiFlipController::CreateBlendInterpolator(
    unsigned short usIndex,
    bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NI_UNUSED_ARG(usIndex);
     NIASSERT(usIndex == 0);
     NiBlendInterpolator* pkInterp = NiNew NiBlendFloatInterpolator(
        bManagerControlled, fWeightThreshold, ucArraySize);
     pkInterp->SetOnlyUseHighestWeight(true);
     return pkInterp;
}
//---------------------------------------------------------------------------
bool NiFlipController::SetAffectedMap(NiTexturingProperty::Map* pkMap)
{
    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)m_pkTarget;
    if (pkTexProp && pkMap)
    {
        const NiTexturingProperty::NiMapArray& kMaps = 
            pkTexProp->GetMaps();
        unsigned int uiSize = kMaps.GetSize();
        for (unsigned int i = 0; i < uiSize; i++)
        {
            if (kMaps.GetAt(i) == pkMap)
            {
                m_uiAffectedMap = i;
                ResetCtlrID();
                return true;
            }
        }
        for (unsigned int ui =0; ui < pkTexProp->GetShaderArrayCount(); ui++)
        {
            if (pkTexProp->GetShaderMap(ui) == pkMap)
            {
                m_uiAffectedMap = SHADER_MAP_OFFSET + ui;
                ResetCtlrID();
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
NiTexturingProperty::Map* NiFlipController::GetAffectedMap() const
{
    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)m_pkTarget;
    if (pkTexProp)
    {
        const NiTexturingProperty::NiMapArray& kMaps =
            pkTexProp->GetMaps();
        if (m_uiAffectedMap < kMaps.GetSize())
        {
            return kMaps.GetAt(m_uiAffectedMap);
        }
        else
        {
            return pkTexProp->GetShaderMap(m_uiAffectedMap -
                SHADER_MAP_OFFSET);             
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiFlipController::AttachTexture(NiTexture* pkTexture, 
    unsigned int uiIndex)
{
    if ( m_kTextures.GetSize() <= uiIndex )
        m_kTextures.SetSize(uiIndex+1);
        
    m_kTextures.SetAtGrow(uiIndex, pkTexture);
}
//---------------------------------------------------------------------------
NiTexturePtr NiFlipController::DetachTexture(unsigned int uiIndex)
{
    NiTexturePtr spTexture = m_kTextures.RemoveAt(uiIndex);
    return spTexture;
}
//---------------------------------------------------------------------------
NiTexture* NiFlipController::GetTextureAt(unsigned int uiIndex)
{
    if (uiIndex < m_kTextures.GetSize())
        return m_kTextures.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiFlipController::GetTargetFloatValue(float& fValue)
{
    fValue = (float) m_uiIndex;
}
//---------------------------------------------------------------------------
void NiFlipController::Update(float fTime)
{
    if (GetManagerControlled())
    {
        return;
    }

    if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        float fValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, fValue))
        {
            UpdateValue(fTime, fValue, 0);
        }
    }
}
//---------------------------------------------------------------------------
bool NiFlipController::UpdateValue(float, float fFloat, 
    unsigned short)
{
    unsigned int uiMaxIndex = m_kTextures.GetSize();
    NIASSERT(uiMaxIndex != 0);
    // Add 0.01f to the value to make sure we round down correctly in
    // the int conversion
    m_uiIndex = (int)(fFloat + 0.01f);
    if (m_uiIndex >= uiMaxIndex)
        m_uiIndex = uiMaxIndex - 1;

    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)m_pkTarget;
    if (pkTexProp)
    {
        const NiTexturingProperty::NiMapArray& kMaps =
            pkTexProp->GetMaps();
        if (m_uiAffectedMap < kMaps.GetSize())
        {
            if (kMaps.GetAt(m_uiAffectedMap) != NULL)
            {
                kMaps.GetAt(m_uiAffectedMap)->SetTexture(GetTexture());
            }
        }
        else
        {
            unsigned int uiAffectedMap = m_uiAffectedMap -
                SHADER_MAP_OFFSET;
            if (uiAffectedMap < pkTexProp->GetShaderArrayCount())
            {
                if (pkTexProp->GetShaderMap(uiAffectedMap) != NULL)
                {
                    pkTexProp->GetShaderMap(uiAffectedMap)->SetTexture(
                        GetTexture());
                }
            }
            else
            {
                NIASSERT(m_uiAffectedMap < kMaps.GetSize());
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiFlipController::UpdateValue(float fTime, 
    NiControllerSequence* pkControllerSequence, 
    NiEvaluator* pkEvaluator, 
    NiEvaluatorSPData** ppkEvalSPData,
    unsigned short)
{
    NIASSERT(pkControllerSequence);
    NIASSERT(pkEvaluator);

    m_fScaledTime = pkControllerSequence->GetLastScaledTime();

    float fValue;
    if (pkEvaluator->GetChannelPosedValue(0, &fValue))
    {
        UpdateValue(fTime, fValue, 0);
    }
    else
    {
        NIASSERT(ppkEvalSPData);
        NIASSERT(ppkEvalSPData[0]);
        if (pkEvaluator->UpdateChannel(fTime, 0, ppkEvalSPData[0], &fValue))
        {
            UpdateValue(fTime, fValue, 0);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
const char* NiFlipController::GetCtlrID()
{
    if (m_pcCtlrID)
        return m_pcCtlrID;

    m_pcCtlrID = NiAlloc(char,15);
    NiSprintf(m_pcCtlrID, 15, "%d", m_uiAffectedMap);
    return m_pcCtlrID;
}
//---------------------------------------------------------------------------
void NiFlipController::ResetCtlrID()
{
    NiFree(m_pcCtlrID);
    m_pcCtlrID = NULL;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFlipController);
//---------------------------------------------------------------------------
void NiFlipController::CopyMembers(NiFlipController* pkDest,
    NiCloningProcess& kCloning)
{
    NiFloatInterpController::CopyMembers(pkDest, kCloning);

    pkDest->m_uiAffectedMap = m_uiAffectedMap;
    

    for (unsigned int uiI = 0; uiI < GetTextureArraySize(); uiI++)
    {
        NiTexture* pkTexture = GetTextureAt(uiI);
        pkDest->AttachTexture(pkTexture,uiI);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFlipController);

//---------------------------------------------------------------------------
void NiFlipController::LoadBinary(NiStream& kStream)
{
    NiFloatInterpController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream,m_uiAffectedMap);
    kStream.ReadMultipleLinkIDs();  // m_kTextures
}
//---------------------------------------------------------------------------
void NiFlipController::LinkObject(NiStream& kStream)
{
    NiFloatInterpController::LinkObject(kStream);

    if (m_pkTarget)
    {
        unsigned int uiSize = kStream.GetNumberOfLinkIDs();
        m_kTextures.SetSize(uiSize);
        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            m_kTextures.SetAt(uiI,(NiTexture*)kStream.GetObjectFromLinkID());
        }
    }
}
//---------------------------------------------------------------------------
bool NiFlipController::RegisterStreamables(NiStream& kStream)
{
    if (!NiFloatInterpController::RegisterStreamables(kStream))
        return false;

    unsigned int uiSize = m_kTextures.GetSize();
    for (unsigned int uiI = 0; uiI < uiSize; uiI++)
    {
        NiTexture* pkTexture = m_kTextures.GetAt(uiI);
        if (pkTexture)
            pkTexture->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiFlipController::SaveBinary(NiStream& kStream)
{
    NiFloatInterpController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream,m_uiAffectedMap);

    unsigned int uiSize = m_kTextures.GetSize();
    NiStreamSaveBinary(kStream,uiSize);
    for (unsigned int uiI = 0; uiI < uiSize; uiI++)
    {
        NiTexture* pkTexture = m_kTextures.GetAt(uiI);
        kStream.SaveLinkID(pkTexture);
    }
}
//---------------------------------------------------------------------------
bool NiFlipController::IsEqual(NiObject* pkObject)
{
    if (!NiFloatInterpController::IsEqual(pkObject))
        return false;

    NiFlipController* pkFlipController = (NiFlipController*) pkObject;

    if (m_uiAffectedMap != pkFlipController->m_uiAffectedMap)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
