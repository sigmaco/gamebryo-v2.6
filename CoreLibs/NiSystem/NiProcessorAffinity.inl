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
inline NiProcessorAffinity::NiProcessorAffinity() :
    m_eIdealProcessor(PROCESSOR_DONT_CARE),
    m_uiAffinityMask((unsigned int) PROCESSOR_DONT_CARE)
{
}
//---------------------------------------------------------------------------
inline NiProcessorAffinity::NiProcessorAffinity(Processor eIdealProcessor,
    unsigned int uiAffinityMask) :
    m_eIdealProcessor(eIdealProcessor), m_uiAffinityMask(uiAffinityMask)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiProcessorAffinity::GetAffinityMask() const
{
    return m_uiAffinityMask;
}
//---------------------------------------------------------------------------
inline void NiProcessorAffinity::SetAffinityMask(unsigned int uiAffinityMask)
{
    m_uiAffinityMask = uiAffinityMask;
}
//---------------------------------------------------------------------------
inline NiProcessorAffinity::Processor 
    NiProcessorAffinity::GetIdealProcessor() const
{
    return m_eIdealProcessor;
}
//---------------------------------------------------------------------------
inline void NiProcessorAffinity::SetIdealProcessor(
    NiProcessorAffinity::Processor eProcessor)
{
    m_eIdealProcessor = eProcessor;
}
//---------------------------------------------------------------------------
