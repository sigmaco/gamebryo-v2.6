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
inline NiUInt32 NiPSSimulatorForcesStep::GetForcesCount() const
{
    return m_kForces.GetSize();
}
//---------------------------------------------------------------------------
inline NiPSForce* NiPSSimulatorForcesStep::GetForceAt(NiUInt32 uiIndex) const
{
    NIASSERT(uiIndex < m_kForces.GetSize());

    return m_kForces.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiPSForce* NiPSSimulatorForcesStep::GetForceByName(
    const NiFixedString& kName) const
{
    const NiUInt32 uiForceCount = m_kForces.GetSize();
    for (NiUInt32 ui = 0; ui < uiForceCount; ++ui)
    {
        NiPSForce* pkForce = m_kForces.GetAt(ui);
        if (pkForce->GetName() == kName)
        {
            return pkForce;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorForcesStep::AddForce(NiPSForce* pkForce)
{
    NIASSERT(pkForce);

    m_kForces.Add(pkForce);
    m_bShouldAllocateDataBuffer = true;
}
//---------------------------------------------------------------------------
inline NiPSForcePtr NiPSSimulatorForcesStep::SetForceAt(
    NiUInt32 uiIndex,
    NiPSForce* pkForce)
{
    NIASSERT(uiIndex < m_kForces.GetSize());
    NIASSERT(pkForce);

    NiPSForcePtr spOldForce = m_kForces.GetAt(uiIndex);
    m_kForces.ReplaceAt(uiIndex, pkForce);
    m_bShouldAllocateDataBuffer = true;

    return spOldForce;
}
//---------------------------------------------------------------------------
inline bool NiPSSimulatorForcesStep::RemoveForce(NiPSForce* pkForce)
{
    NIASSERT(pkForce);

    NiInt32 iIndex = m_kForces.Find(pkForce);
    if (iIndex > -1)
    {
        m_kForces.OrderedRemoveAt((NiUInt32) iIndex);
        m_bShouldAllocateDataBuffer = true;
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiPSForcePtr NiPSSimulatorForcesStep::RemoveForceAt(NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kForces.GetSize());

    NiPSForcePtr spOldForce = m_kForces.GetAt(uiIndex);
    m_kForces.OrderedRemoveAt(uiIndex);
    m_bShouldAllocateDataBuffer = true;

    return spOldForce;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorForcesStep::RemoveAllForces()
{
    m_kForces.RemoveAll();
    m_bShouldAllocateDataBuffer = true;
}
//---------------------------------------------------------------------------
