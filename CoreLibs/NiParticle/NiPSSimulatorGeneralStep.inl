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
inline NiPSKernelColorKey* NiPSSimulatorGeneralStep::GetColorKeys(
    NiUInt8& ucNumColorKeys) const
{
    ucNumColorKeys = m_ucNumColorKeys;
    return m_pkColorKeys;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorGeneralStep::CopyColorKeys(
    NiPSKernelColorKey* pkColorKeys,
    NiUInt8 ucNumColorKeys)
{
    NIASSERT((!pkColorKeys && ucNumColorKeys == 0) ||
        (pkColorKeys && ucNumColorKeys > 0));
    NIASSERT(!pkColorKeys || pkColorKeys[0].m_fTime == 0.0f);
    NIASSERT(!pkColorKeys || pkColorKeys[ucNumColorKeys - 1].m_fTime == 1.0f);

    m_ucNumColorKeys = ucNumColorKeys;
    NiAlignedFree(m_pkColorKeys);
    m_pkColorKeys = NULL;
    if (pkColorKeys)
    {
        m_pkColorKeys = NiAlignedAlloc(NiPSKernelColorKey, m_ucNumColorKeys,
            NIPSKERNEL_ALIGNMENT);
        NiMemcpy(m_pkColorKeys, m_ucNumColorKeys * sizeof(NiPSKernelColorKey),
            pkColorKeys, ucNumColorKeys * sizeof(NiPSKernelColorKey));
    }
}
//---------------------------------------------------------------------------
inline float NiPSSimulatorGeneralStep::GetGrowTime() const
{
    return m_kInputStruct.m_fGrowTime;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorGeneralStep::SetGrowTime(float fGrowTime)
{
    m_kInputStruct.m_fGrowTime = fGrowTime;
}
//---------------------------------------------------------------------------
inline float NiPSSimulatorGeneralStep::GetShrinkTime() const
{
    return m_kInputStruct.m_fShrinkTime;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorGeneralStep::SetShrinkTime(float fShrinkTime)
{
    m_kInputStruct.m_fShrinkTime = fShrinkTime;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPSSimulatorGeneralStep::GetGrowGeneration() const
{
    return m_kInputStruct.m_usGrowGeneration;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorGeneralStep::SetGrowGeneration(
    NiUInt16 usGrowGeneration)
{
    m_kInputStruct.m_usGrowGeneration = usGrowGeneration;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPSSimulatorGeneralStep::GetShrinkGeneration() const
{
    return m_kInputStruct.m_usShrinkGeneration;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorGeneralStep::SetShrinkGeneration(
    NiUInt16 usShrinkGeneration)
{
    m_kInputStruct.m_usShrinkGeneration = usShrinkGeneration;
}
//---------------------------------------------------------------------------
