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
inline NiEvaluatorSPData::NiEvaluatorSPData() : 
    m_pkEvaluator(NULL), m_sLOD(0), 
    m_ucEvalChannelIndex(EVALBASEINDEX), m_ucFlags(0), 
    m_fSPSegmentMinTime(NI_INFINITY), m_fSPSegmentMaxTime(-NI_INFINITY), 
    m_pfnSPFillFunc(NULL), m_pvSPFillData(NULL), m_pvSPSegmentData(NULL)
{
}
//---------------------------------------------------------------------------
inline NiEvaluator* NiEvaluatorSPData::GetEvaluator() const
{
    return m_pkEvaluator;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetEvaluator(NiEvaluator* pkEvaluator)
{
    m_pkEvaluator = pkEvaluator;
}
//---------------------------------------------------------------------------
inline NiInt16 NiEvaluatorSPData::GetLOD() const
{
    return m_sLOD;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetLOD(NiInt16 sLOD)
{
    m_sLOD = sLOD;
}
//---------------------------------------------------------------------------
inline NiEvaluatorSPData::EvalChannelIndex 
    NiEvaluatorSPData::GetEvalChannelIndex() const
{
    return (EvalChannelIndex)m_ucEvalChannelIndex;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetEvalChannelIndex(
    EvalChannelIndex eEvalChannelIndex)
{
    m_ucEvalChannelIndex = (NiUInt8)eEvalChannelIndex;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiEvaluatorSPData::GetFlags() const
{
    return m_ucFlags;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetFlags(NiUInt8 ucFlags)
{
    m_ucFlags = ucFlags;
}
//---------------------------------------------------------------------------
inline bool NiEvaluatorSPData::AlwaysUpdate() const
{
    return (m_ucFlags & FLAGALWAYSUPDATE) != 0;
}
//---------------------------------------------------------------------------
inline bool NiEvaluatorSPData::IsStepFunction() const
{
    return (m_ucFlags & FLAGSTEPFUNCTION) != 0;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetStepFunction(bool bEnabled)
{
    if (bEnabled)
    {
        m_ucFlags |= FLAGSTEPFUNCTION;
    }
    else
    {
        m_ucFlags &= ~FLAGSTEPFUNCTION;
    }
}
//---------------------------------------------------------------------------
inline bool NiEvaluatorSPData::IsAlternateSPData() const
{
    return (m_ucFlags & FLAGALTERNATESPDATA) != 0;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetAlternateSPData(bool bEnabled)
{
    if (bEnabled)
    {
        m_ucFlags |= FLAGALTERNATESPDATA;
    }
    else
    {
        m_ucFlags &= ~FLAGALTERNATESPDATA;
    }
}
//---------------------------------------------------------------------------
inline float NiEvaluatorSPData::GetSPSegmentMinTime() const
{
    return m_fSPSegmentMinTime;
}
//---------------------------------------------------------------------------
inline float NiEvaluatorSPData::GetSPSegmentMaxTime() const
{
    return m_fSPSegmentMaxTime;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetSPSegmentTimeRange(
    float fMinTime, float fMaxTime)
{
    m_fSPSegmentMinTime = fMinTime;
    m_fSPSegmentMaxTime = fMaxTime;
}
//---------------------------------------------------------------------------
inline bool NiEvaluatorSPData::IsSPSegmentDataValid(float fTime) const
{
    return (fTime >= m_fSPSegmentMinTime && fTime <= m_fSPSegmentMaxTime);
}
//---------------------------------------------------------------------------
inline NiScratchPadFillFunc NiEvaluatorSPData::GetSPFillFunc() const
{
    return m_pfnSPFillFunc;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetSPFillFunc(
    NiScratchPadFillFunc pfnSPFillFunc)
{
    m_pfnSPFillFunc = pfnSPFillFunc;
}
//---------------------------------------------------------------------------
inline void* NiEvaluatorSPData::GetSPFillData() const
{
    return m_pvSPFillData;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetSPFillData(void* pvSPFillData)
{
    m_pvSPFillData = pvSPFillData;
}
//---------------------------------------------------------------------------
inline void* NiEvaluatorSPData::GetSPSegmentData() const
{
    return m_pvSPSegmentData;
}
//---------------------------------------------------------------------------
inline void NiEvaluatorSPData::SetSPSegmentData(void* pvSPSegmentData)
{
    m_pvSPSegmentData = pvSPSegmentData;
}
//---------------------------------------------------------------------------
