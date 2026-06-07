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

//---------------------------------------------------------------------------
inline void NiTimeController::SetAnimType(AnimType eType)
{
    SetField(NiInt32ToUInt16(eType), ANIMTYPE_MASK, ANIMTYPE_POS);

    // APP_TIME animations are considered to be always active
    SetActive(eType == APP_TIME);
}
//---------------------------------------------------------------------------
inline NiTimeController::AnimType NiTimeController::GetAnimType() const
{ 
    return (AnimType)GetField(ANIMTYPE_MASK, ANIMTYPE_POS);
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetCycleType(CycleType eType)
{
    SetField(NiInt32ToUInt16(eType), CYCLETYPE_MASK, CYCLETYPE_POS); 
}
//---------------------------------------------------------------------------
inline NiTimeController::CycleType NiTimeController::GetCycleType() const
{ 
    return (CycleType)GetField(CYCLETYPE_MASK, CYCLETYPE_POS); 
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetPlayBackwards(bool bPlayBackwards)
{
    SetBit(bPlayBackwards, DIRECTION_MASK);
}
//---------------------------------------------------------------------------
inline bool NiTimeController::GetPlayBackwards() const
{
    return GetBit(DIRECTION_MASK);
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetManagerControlledBit(bool bManagerControlled)
{
    SetBit(bManagerControlled, MANAGERCONTROLLED_MASK);
}
//---------------------------------------------------------------------------
inline bool NiTimeController::GetManagerControlledBit() const
{
    return GetBit(MANAGERCONTROLLED_MASK);
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetFrequency(float fFreq)
{
    m_fFrequency = fFreq;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetFrequency() const
{
    return m_fFrequency;
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetPhase(float fPhase)
{
    m_fPhase = fPhase;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetPhase() const
{
    return m_fPhase;
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetBeginKeyTime(float fBegin)
{
    m_fLoKeyTime = fBegin;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetBeginKeyTime() const
{
    return m_fLoKeyTime;
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetEndKeyTime(float fEnd)
{
    m_fHiKeyTime = fEnd;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetEndKeyTime() const
{
    return m_fHiKeyTime;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetStartTime() const
{
    // GetStartTime is for advanced users only.  The NiAnimBlender class
    // makes use of it to control animation blending & morphing.
    return m_fStartTime;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetLastTime() const
{
    return m_fLastTime;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetLastScaledTime() const
{
    return m_fScaledTime;
}
//---------------------------------------------------------------------------
inline NiObjectNET* NiTimeController::GetTarget() const
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetNext(NiTimeController* pkNext)
{
    m_spNext = pkNext;
}
//---------------------------------------------------------------------------
inline NiTimeController* NiTimeController::GetNext() const
{
    return m_spNext;
}
//---------------------------------------------------------------------------
inline bool NiTimeController::DontDoUpdate(float fTime)
{
    if (!GetActive() || (m_fLastTime == fTime && !GetForceUpdate()))
        return true;

    if (GetComputeScaledTime())
    {
        float fScaledTime = ComputeScaledTime(fTime);
        if (m_fScaledTime == fScaledTime && !GetForceUpdate())
            return true;

        m_fScaledTime = fScaledTime;
    }

    SetForceUpdate(false);    
    return false;
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetActive(bool bActive)
{
    SetBit(bActive, ACTIVE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiTimeController::GetActive() const
{
    return GetBit(ACTIVE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTimeController::OnPreDisplayIterate(
    NiTimeController* pkController)
{
    while (pkController != NULL)
    {
        pkController->OnPreDisplay();
        pkController = pkController->m_spNext;
    }
}
//---------------------------------------------------------------------------
inline bool NiTimeController::IsTransformController() const
{
    return false;
}
//---------------------------------------------------------------------------
inline void NiTimeController::ForceNextUpdate()
{
    SetForceUpdate(true);
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetForceUpdate(bool bForceUpdate)
{
    SetBit(bForceUpdate, FORCEUDPATE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiTimeController::GetForceUpdate() const
{
    return GetBit(FORCEUDPATE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTimeController::SetComputeScaledTime(bool bComputeScaledTime)
{
    SetBit(bComputeScaledTime, COMPUTESCALEDTIME_MASK);
}
//---------------------------------------------------------------------------
inline bool NiTimeController::GetComputeScaledTime() const
{
    return GetBit(COMPUTESCALEDTIME_MASK);
}
//---------------------------------------------------------------------------
inline bool NiTimeController::IsStreamable() const
{
    return true;
}
//---------------------------------------------------------------------------
inline float NiTimeController::GetMinBeginKeyTime(NiAVObject* pkObject)
{
    float fMinBeginKeyTime = NI_INFINITY;
    GetMinBeginKeyTimeRecursive(pkObject, fMinBeginKeyTime);
    if (fMinBeginKeyTime == NI_INFINITY)
    {
        fMinBeginKeyTime = 0.0f;
    }
    return fMinBeginKeyTime;
}
//---------------------------------------------------------------------------
