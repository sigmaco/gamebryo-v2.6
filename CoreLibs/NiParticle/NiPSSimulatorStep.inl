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
inline const NiTObjectSet<NiFixedString>&
    NiPSSimulatorStep::GetInputSemantics() const
{
    return m_kInputSemantics;
}
//---------------------------------------------------------------------------
inline const NiTObjectSet<NiFixedString>&
    NiPSSimulatorStep::GetOutputSemantics() const
{
    return m_kOutputSemantics;
}
//---------------------------------------------------------------------------
inline const NiTPrimitiveSet<NiSPStream*>&
    NiPSSimulatorStep::GetInputStreams() const
{
    return m_kInputStreams;
}
//---------------------------------------------------------------------------
inline const NiTPrimitiveSet<NiSPStream*>&
    NiPSSimulatorStep::GetOutputStreams() const
{
    return m_kOutputStreams;
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorStep::SetInputCount(NiUInt32 uiCount)
{
    m_kInputSemantics.ReallocNoShrink(uiCount);
    m_kInputStreams.ReallocNoShrink(uiCount);
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorStep::SetOutputCount(NiUInt32 uiCount)
{
    m_kOutputSemantics.ReallocNoShrink(uiCount);
    m_kOutputStreams.ReallocNoShrink(uiCount);
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorStep::AddInput(const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    m_kInputSemantics.Add(kSemantic);
    m_kInputStreams.Add(pkStream);
}
//---------------------------------------------------------------------------
inline void NiPSSimulatorStep::AddOutput(const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    m_kOutputSemantics.Add(kSemantic);
    m_kOutputStreams.Add(pkStream);
}
//---------------------------------------------------------------------------
