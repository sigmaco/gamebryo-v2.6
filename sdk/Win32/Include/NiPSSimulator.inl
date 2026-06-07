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
inline NiUInt32 NiPSSimulator::GetStepCount() const
{
    return m_kSteps.GetSize();
}
//---------------------------------------------------------------------------
inline NiPSSimulatorStep* NiPSSimulator::GetStepAt(NiUInt32 uiIndex) const
{
    NIASSERT(uiIndex < m_kSteps.GetSize());
    return m_kSteps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSSimulator::AddStep(NiPSSimulatorStep* pkStep)
{
    NIASSERT(pkStep);

    if (m_bCanChangeSteps)
    {
        m_kSteps.Add(pkStep);
        return (m_kSteps.GetSize() - 1);
    }
    else
    {
        return INVALID_INDEX;
    }
}
//---------------------------------------------------------------------------
inline NiPSSimulatorStepPtr NiPSSimulator::SetStepAt(
    NiUInt32 uiIndex,
    NiPSSimulatorStep* pkStep)
{
    NIASSERT(uiIndex < m_kSteps.GetSize());
    NIASSERT(pkStep);

    if (m_bCanChangeSteps)
    {
        NiPSSimulatorStepPtr spOldStep = m_kSteps.GetAt(uiIndex);
        m_kSteps.ReplaceAt(uiIndex, pkStep);
        return spOldStep;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline bool NiPSSimulator::RemoveStep(NiPSSimulatorStep* pkStep)
{
    NIASSERT(pkStep);

    if (m_bCanChangeSteps)
    {
        NiInt32 iIndex = m_kSteps.Find(pkStep);
        if (iIndex > -1)
        {
            m_kSteps.OrderedRemoveAt((NiUInt32) iIndex);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiPSSimulatorStepPtr NiPSSimulator::RemoveStepAt(NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kSteps.GetSize());

    if (m_bCanChangeSteps)
    {
        NiPSSimulatorStepPtr spOldStep = m_kSteps.GetAt(uiIndex);
        m_kSteps.OrderedRemoveAt(uiIndex);
        return spOldStep;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline bool NiPSSimulator::RemoveAllSteps()
{
    if (m_bCanChangeSteps)
    {
        m_kSteps.RemoveAll();
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiPSSimulatorStep* NiPSSimulator::GetSimulatorStepByType(
    const NiRTTI* pkRTTI) const
{
    const NiUInt32 uiStepCount = m_kSteps.GetSize();
    for (NiUInt32 ui = 0; ui < uiStepCount; ++ui)
    {
        NiPSSimulatorStep* pkStep = m_kSteps.GetAt(ui);
        if (pkStep->IsKindOf(pkRTTI))
        {
            return pkStep;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline void NiPSSimulator::DeleteInputAndOutputStreams()
{
    for (NiUInt32 ui = 0; ui < m_kInputStreams.GetSize(); ui++)
    {
        NiDelete m_kInputStreams.GetAt(ui)->m_pkStream;
        NiDelete m_kInputStreams.GetAt(ui);
    }
    m_kInputStreams.RemoveAll();

    for (NiUInt32 ui = 0; ui < m_kOutputStreams.GetSize(); ui++)
    {
        NiDelete m_kOutputStreams.GetAt(ui)->m_pkStream;
        NiDelete m_kOutputStreams.GetAt(ui);
    }
    m_kOutputStreams.RemoveAll();
    
    m_kDummyStream.ClearTaskArrays();
}
//---------------------------------------------------------------------------
