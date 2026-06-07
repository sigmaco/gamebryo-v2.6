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

#include "NiTextureTransformController.h"
#include <NiBool.h>
#include "NiFloatData.h"
#include "NiFloatInterpolator.h"

NiImplementRTTI(NiTextureTransformController, NiFloatInterpController);

//---------------------------------------------------------------------------
NiTextureTransformController::NiTextureTransformController(
    NiTexturingProperty* pkTarget, NiTexturingProperty::Map* pkMap, 
    TransformMember eMember): 
    m_eMember(eMember), m_pcCtlrID(NULL)
{
    m_uiLastIdx = 0;
    m_bShaderMap = false;
    m_uiMapIndex = 0;

    SetTarget(pkTarget);

    // Set the Shader Map and Map Index
    SetMap(pkMap);
}
//---------------------------------------------------------------------------
NiTextureTransformController::~NiTextureTransformController()
{
    ResetCtlrID();
}
//---------------------------------------------------------------------------
bool NiTextureTransformController::SetMap(
        NiTexturingProperty::Map* pkMap, NiTexturingProperty* pkTarget)
{
    m_pkMap = pkMap;

    // Check for clearing the Map
    if (pkMap == NULL)
    {   
        ResetCtlrID();
        return true;
    }


    // We must have a target to propey set the map
    if ((pkTarget == NULL) && (m_pkTarget == NULL))
    {
        return false;
    }

    // Use the target passed in if we don't already have one.
    if (m_pkTarget == NULL)
    {
        SetTarget(pkTarget);
    }


    unsigned int uiIndex;
    NiTexturingProperty* pkTexture = (NiTexturingProperty*)m_pkTarget;
    const NiTexturingProperty::NiMapArray& kMaps = pkTexture->GetMaps();

    // Look for a regular map
    for( uiIndex = 0; uiIndex < kMaps.GetSize(); uiIndex++)
    {
        if (kMaps.GetAt(uiIndex) == pkMap)
        {
            m_bShaderMap = false;
            m_uiMapIndex = uiIndex;
            ResetCtlrID();
            NIVERIFY(SetInternalMap());
            return true;
        }
    }

    // Look for a Shader map
    for( uiIndex = 0; uiIndex < pkTexture->GetShaderArrayCount(); uiIndex++)
    {
        if (pkTexture->GetShaderMap(uiIndex) == pkMap)
        {
            m_bShaderMap = true;
            m_uiMapIndex = uiIndex;
            ResetCtlrID();            
            NIVERIFY(SetInternalMap());
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiTextureTransformController::Update(float fTime)
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
bool NiTextureTransformController::UpdateValue(float, float fFloat, 
    unsigned short)
{
    if (!SetInternalMap())
    {
        NIASSERT(false);  // you should never get here
        return true;
    }

    // Get the Transform for this map
    NiTextureTransform* pkTransform = m_pkMap->GetTextureTransform();

    // Automatically create an Identity matrix if one isn't already
    // there
    if (pkTransform == NULL)
    {
        pkTransform = NiNew NiTextureTransform(NiPoint2(0.0f, 0.0f),
            0.0f, NiPoint2(1.0f, 1.0f), NiPoint2(0.5f, 0.5f),
            NiTextureTransform::MAYA_TRANSFORM);
        m_pkMap->SetTextureTransform(pkTransform);
    }

    NiPoint2 kPoint;

    switch(m_eMember)
    {
    case TT_TRANSLATE_U:
        kPoint = pkTransform->GetTranslate();
        kPoint.x = fFloat;
        pkTransform->SetTranslate(kPoint);
        break;
    case TT_TRANSLATE_V:
        kPoint = pkTransform->GetTranslate();
        kPoint.y = fFloat;
        pkTransform->SetTranslate(kPoint);
        break;
    case TT_ROTATE:
        pkTransform->SetRotate(fFloat);
        break;
    case TT_SCALE_U:
        kPoint = pkTransform->GetScale();
        kPoint.x = fFloat;
        pkTransform->SetScale(kPoint);
        break;
    case TT_SCALE_V:
        kPoint = pkTransform->GetScale();
        kPoint.y = fFloat;
        pkTransform->SetScale(kPoint);
        break;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTextureTransformController::GetTargetFloatValue(float& fValue)
{
    if (!SetInternalMap())
    {
        NIASSERT(false);  // you should never get here
        return;
    }

    // Get the Transform for this map
    NiTextureTransform* pkTransform = m_pkMap->GetTextureTransform();

    // Automatically create an Identity matrix if one isn't already there
    if (pkTransform == NULL)
    {
        NIASSERT(pkTransform != NULL);
        fValue = 0.0f;
        return;
    }

    NiPoint2 kPoint;

    switch(m_eMember)
    {
    case TT_TRANSLATE_U:
        kPoint = pkTransform->GetTranslate();
        fValue = kPoint.x;
        break;
    case TT_TRANSLATE_V:
        kPoint = pkTransform->GetTranslate();
        fValue = kPoint.y;
        break;
    case TT_ROTATE:
        fValue = pkTransform->GetRotate();
        break;
    case TT_SCALE_U:
        kPoint = pkTransform->GetScale();
        fValue = kPoint.x;
        break;
    case TT_SCALE_V:
        kPoint = pkTransform->GetScale();
        fValue = kPoint.y;
        break;
    }
}
//---------------------------------------------------------------------------
const char* NiTextureTransformController::GetCtlrID()
{
    if (m_pcCtlrID)
        return m_pcCtlrID;

    m_pcCtlrID = NiAlloc(char,50);
    switch(m_eMember)
    {
    case TT_TRANSLATE_U:
        NiSprintf(m_pcCtlrID, 50, "%d-%d-TT_TRANSLATE_U", (int)m_bShaderMap,
            m_uiMapIndex);
        break;
    case TT_TRANSLATE_V:
        NiSprintf(m_pcCtlrID, 50, "%d-%d-TT_TRANSLATE_V", (int)m_bShaderMap,
            m_uiMapIndex);
        break;
    case TT_ROTATE:
        NiSprintf(m_pcCtlrID, 50, "%d-%d-TT_ROTATE", (int)m_bShaderMap,
            m_uiMapIndex);
        break;
    case TT_SCALE_U:
        NiSprintf(m_pcCtlrID, 50, "%d-%d-TT_SCALE_U", (int)m_bShaderMap,
            m_uiMapIndex);
        break;
    case TT_SCALE_V:
        NiSprintf(m_pcCtlrID, 50, "%d-%d-TT_SCALE_V", (int)m_bShaderMap,
            m_uiMapIndex);
        break;
    }    
    return m_pcCtlrID;
}
//---------------------------------------------------------------------------
void NiTextureTransformController::ResetCtlrID()
{
    NiFree(m_pcCtlrID);
    m_pcCtlrID = NULL;
}
//---------------------------------------------------------------------------
void NiTextureTransformController::SetTarget(NiObjectNET* pkTarget)
{
    if (m_pkTarget != pkTarget)
    {
        // Clear out cached map pointer
        m_pkMap = NULL;
    }

    NiTimeController::SetTarget(pkTarget);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTextureTransformController);
//---------------------------------------------------------------------------
void NiTextureTransformController::CopyMembers(
    NiTextureTransformController* pkDest,
    NiCloningProcess& kCloning)
{
    NiFloatInterpController::CopyMembers(pkDest, kCloning);

    pkDest->m_uiLastIdx = m_uiLastIdx;
    pkDest->m_bShaderMap = m_bShaderMap;
    pkDest->m_uiMapIndex = m_uiMapIndex;
    pkDest->m_eMember = m_eMember;

    // pkDest->m_pkMap is purposely not set here because it will be set
    // during the Update call in SetInternalMap. It is initialized to NULL in
    // the constructor.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTextureTransformController);
//---------------------------------------------------------------------------
void NiTextureTransformController::LoadBinary(NiStream& kStream)
{
    NiFloatInterpController::LoadBinary(kStream);

    NiBool kTemp;
    NiStreamLoadBinary(kStream, kTemp);
    m_bShaderMap = (kTemp != 0);

    NiStreamLoadBinary(kStream, m_uiMapIndex);
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 5))
    {
        // Decal map moved up two slots - must increment
        if (m_uiMapIndex >= NiTexturingProperty::NORMAL_INDEX)
            m_uiMapIndex += 2;
    }

    NiStreamLoadEnum(kStream, m_eMember);
}
//---------------------------------------------------------------------------
void NiTextureTransformController::LinkObject(NiStream& kStream)
{
    NiFloatInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTextureTransformController::RegisterStreamables(NiStream& kStream)
{
    if (!NiFloatInterpController::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiTextureTransformController::SaveBinary(NiStream& kStream)
{
    NiFloatInterpController::SaveBinary(kStream);

    NiBool kTemp = m_bShaderMap;

    NiStreamSaveBinary(kStream, kTemp);
    NiStreamSaveBinary(kStream, m_uiMapIndex);
    NiStreamSaveEnum(kStream, m_eMember);

}
//---------------------------------------------------------------------------
bool NiTextureTransformController::IsEqual(NiObject* pkObject)
{
    if (!NiFloatInterpController::IsEqual(pkObject))
        return false;

    NiTextureTransformController* pkCtrl = 
        (NiTextureTransformController*)pkObject;

    if (pkCtrl->m_uiLastIdx != m_uiLastIdx ||
        pkCtrl->m_bShaderMap != m_bShaderMap ||
        pkCtrl->m_uiMapIndex != m_uiMapIndex ||
        pkCtrl->m_eMember != m_eMember)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTextureTransformController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiFloatInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiTextureTransformController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

