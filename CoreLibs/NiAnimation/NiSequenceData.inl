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

#include <NiAVObject.h>

//---------------------------------------------------------------------------
// NiSequenceData::IDTag
//---------------------------------------------------------------------------
inline NiSequenceData::IDTag::IDTag() :
    m_kAVObjectName(NULL), m_kPropertyType(NULL), m_kCtlrType(NULL), 
    m_kCtlrID(NULL), m_kInterpolatorID(NULL)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::IDTag::GetAVObjectName() const
{
    return m_kAVObjectName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::IDTag::GetPropertyType() const
{
    return m_kPropertyType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::IDTag::GetCtlrType() const
{
    return m_kCtlrType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::IDTag::GetCtlrID() const
{
    return m_kCtlrID;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::IDTag::GetInterpolatorID() const
{
    return m_kInterpolatorID;
}

//---------------------------------------------------------------------------
// NiSequenceData::ActivationCallbackItem
//---------------------------------------------------------------------------
inline NiSequenceData::ActivationCallbackItem::ActivationCallbackItem() :
    m_pkCallback(NULL), m_pkManager(NULL)
{
}
//---------------------------------------------------------------------------
inline void NiSequenceData::ActivationCallbackItem::ClearValues()
{
    m_pkCallback = NULL;
    m_pkManager = NULL;
}

//---------------------------------------------------------------------------
// NiSequenceData
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NiTextKeyExtraData* NiSequenceData::GetTextKeys() const
{
    return m_spTextKeys;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetTextKeys(NiTextKeyExtraData* pkKeys)
{
    m_spTextKeys = pkKeys;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::AddDefaultActivationCallback(
    ActivationCallback* pkCallback, NiControllerManager* pkManager)
{
    NIASSERT(pkCallback);

    // Is current array big enough?
    if (m_uiNumCallbacks >= m_uiCallbackArraySize)
    {
        SetCallbackArraySize(m_uiNumCallbacks + 3);
    }

    // Add new callback.
    ActivationCallbackItem& kItem = 
        m_pkDefaultActivationCallbacks[m_uiNumCallbacks];
    kItem.m_pkCallback = pkCallback;
    kItem.m_pkManager = pkManager;
    m_uiNumCallbacks++;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::RemoveDefaultActivationCallback(
    ActivationCallback* pkCallback, NiControllerManager* pkManager)
{
    // Remove all matching instances.
    unsigned int ui = 0;
    while (ui < m_uiNumCallbacks)
    {
        ActivationCallbackItem& kItem = m_pkDefaultActivationCallbacks[ui];
        if (kItem.m_pkManager == pkManager &&
            (!pkCallback || kItem.m_pkCallback == pkCallback))
        {
            // Replace removed item with last item in array.
            kItem = m_pkDefaultActivationCallbacks[m_uiNumCallbacks - 1];
            m_pkDefaultActivationCallbacks[m_uiNumCallbacks - 1].ClearValues();
            m_uiNumCallbacks--;
        }
        else
        {
            // Advance to the next callback.
            ui++;
        }
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiSequenceData::GetDefaultActivationCallbackCount(
    ) const
{
    return m_uiNumCallbacks;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::GetDefaultActivationCallbackAt(
    unsigned int uiIndex, ActivationCallback*& pkCallback, 
    NiControllerManager*& pkManager)
{
    NIASSERT(uiIndex < m_uiNumCallbacks);

    ActivationCallbackItem& kItem = m_pkDefaultActivationCallbacks[uiIndex];
    pkCallback = kItem.m_pkCallback;
    pkManager = kItem.m_pkManager;
}
//---------------------------------------------------------------------------
inline unsigned int NiSequenceData::GetMaxEvaluators() const
{
    return m_uiMaxEvaluators;
}
//---------------------------------------------------------------------------
inline unsigned int NiSequenceData::GetNumEvaluators() const
{
    return m_uiNumEvaluators;
}
//---------------------------------------------------------------------------
inline NiEvaluator* NiSequenceData::GetEvaluatorAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_uiNumEvaluators);
    return m_pspEvaluatorArray[uiIndex];
}
//---------------------------------------------------------------------------
inline float NiSequenceData::TimeDivFreq(float fTime) const
{
    return (fTime / m_fFrequency);
}
//---------------------------------------------------------------------------
inline float NiSequenceData::TimeMultFreq(float fTime) const
{
    return (fTime * m_fFrequency);
}
//---------------------------------------------------------------------------
inline float NiSequenceData::GetDuration() const
{
    return m_fDuration;
}
//---------------------------------------------------------------------------
inline float NiSequenceData::GetDurationDivFreq() const
{
    return m_fDuration / m_fFrequency;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetDuration(float fDuration)
{
    m_fDuration = fDuration;
}
//---------------------------------------------------------------------------
inline NiTimeController::CycleType NiSequenceData::GetCycleType()
    const
{
    return m_eCycleType;
}
//---------------------------------------------------------------------------
inline bool NiSequenceData::SetCycleType(
    NiTimeController::CycleType eCycleType)
{
    if (eCycleType == NiTimeController::CLAMP ||
        eCycleType == NiTimeController::LOOP)
    {
        m_eCycleType = eCycleType;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline float NiSequenceData::GetFrequency() const
{
    return m_fFrequency;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetFrequency(float fFrequency)
{
    NIASSERT(m_fFrequency > 0.0f);
    m_fFrequency = fFrequency;
}
//---------------------------------------------------------------------------
inline bool NiSequenceData::GetTempPose() const
{
    return GetBit(FLAG_TEMP_POSE);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetTempPose()
{
    // NiSequenceData objects marked as temporary poses are automatically 
    // removed from the NiControllerManager when their associated active 
    // NiControllerSequence runs to completion.
    SetBit(true, FLAG_TEMP_POSE);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetAccumRootName(const NiFixedString&
    kAccumRootName)
{   
    m_kAccumRootName = kAccumRootName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSequenceData::GetAccumRootName() const
{
    return m_kAccumRootName;
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetAccumTranslation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ)
{
    SetBit(bAccumX, ACCUM_X_TRANS);
    SetBit(bAccumY, ACCUM_Y_TRANS);
    SetBit(bAccumZ, ACCUM_Z_TRANS);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::GetAccumTranslation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const
{
    bAccumX = GetBit(ACCUM_X_TRANS);
    bAccumY = GetBit(ACCUM_Y_TRANS);
    bAccumZ = GetBit(ACCUM_Z_TRANS);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetAccumRotation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ)
{
    SetBit(bAccumX, ACCUM_X_ROT);
    SetBit(bAccumY, ACCUM_Y_ROT);
    SetBit(bAccumZ, ACCUM_Z_ROT);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::GetAccumRotation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const
{
    bAccumX = GetBit(ACCUM_X_ROT);
    bAccumY = GetBit(ACCUM_Y_ROT);
    bAccumZ = GetBit(ACCUM_Z_ROT);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::SetAccumFrontFacing(
    const unsigned char ucAxis, const bool bIsNegative)
{
    switch (ucAxis)
    {
        case 0:
            SetBit(true, ACCUM_X_FRONT);
            SetBit(false, ACCUM_Y_FRONT);
            SetBit(false, ACCUM_Z_FRONT);
            break;
        case 1:
            SetBit(false, ACCUM_X_FRONT);
            SetBit(true, ACCUM_Y_FRONT);
            SetBit(false, ACCUM_Z_FRONT);
            break;
        case 2:
            SetBit(false, ACCUM_X_FRONT);
            SetBit(false, ACCUM_Y_FRONT);
            SetBit(true, ACCUM_Z_FRONT);
            break;
    }
    SetBit(bIsNegative, ACCUM_NEG_FRONT);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::GetAccumFrontFacing(
    unsigned char& ucAxis, bool& bIsNegative) const
{
    if (GetBit(ACCUM_X_FRONT))
    {
        ucAxis = 0;
    }
    else if (GetBit(ACCUM_Y_FRONT))
    {
        ucAxis = 1;
    }
    else
    {
        ucAxis = 2;
    }
    bIsNegative = GetBit(ACCUM_NEG_FRONT);
}
//---------------------------------------------------------------------------
inline void NiSequenceData::ReplaceEvaluatorAt(NiEvaluator* pkEvaluator, 
    unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiNumEvaluators);
    m_pspEvaluatorArray[uiIndex] = pkEvaluator;
}
//---------------------------------------------------------------------------
