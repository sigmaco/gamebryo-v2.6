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
inline NiSPWorkload::NiSPWorkload() : m_pkInputsOutputs(0), m_uiInOutCount(0),
    m_uiInputCount(0), m_uiOutputCount(0), m_uiBlockCount(0),
    m_uiRangeStart(0), m_pkKernel(0)
{
}
//---------------------------------------------------------------------------
inline NiSPWorkload::~NiSPWorkload()
{
    Clear();
}
//---------------------------------------------------------------------------
template <class T>
inline T* NiSPWorkload::GetInput(NiUInt16 uiInputIndex)
{
    NiSPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];

    return (T*)((char*)kHeader.m_pvBaseAddress + kHeader.m_uiDataOffset);
}
//---------------------------------------------------------------------------
template <class T> 
inline T* NiSPWorkload::GetInputBase(NiUInt16 uiInputIndex)
{
    NiSPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];

    return (T*)kHeader.m_pvBaseAddress;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPWorkload::GetInputStride(NiUInt16 uiInputIndex)
{
    NiSPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];
    return kHeader.m_uiStride;
}
//---------------------------------------------------------------------------
template <class T>
inline T* NiSPWorkload::GetOutput(NiUInt16 uiOutputIndex)
{
    NiUInt32 uiIndex = uiOutputIndex + m_uiInputCount;
    NiSPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];

    return (T*)((char*)kHeader.m_pvBaseAddress + kHeader.m_uiDataOffset);
}
//---------------------------------------------------------------------------
template <class T> 
inline T* NiSPWorkload::GetOutputBase(NiUInt16 uiOutputIndex)
{
    NiUInt32 uiIndex = uiOutputIndex + m_uiInputCount;
    NiSPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];

    return (T*)kHeader.m_pvBaseAddress;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPWorkload::GetOutputStride(NiUInt16 uiOutputIndex)
{
    NiUInt32 uiIndex = uiOutputIndex + m_uiInputCount;
    NiSPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];
    return kHeader.m_uiStride;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPWorkload::GetBlockCount()
{
    return m_uiBlockCount;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiSPWorkload::GetInputCount()
{
    return m_uiInputCount;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiSPWorkload::GetOutputCount()
{
    return m_uiOutputCount;
}
//---------------------------------------------------------------------------
inline NiSPKernel* NiSPWorkload::GetKernel()
{
    return m_pkKernel;
}
//---------------------------------------------------------------------------
inline void NiSPWorkload::SetKernel(NiSPKernel* pkKernel)
{
    m_pkKernel = pkKernel;
}
//---------------------------------------------------------------------------
inline void NiSPWorkload::Clear()
{
    NiFree(m_pkInputsOutputs);
    m_pkInputsOutputs = 0;
    m_uiInOutCount = 0;
    m_uiInputCount = 0;
    m_uiOutputCount = 0;
    m_uiBlockCount = 0;
    m_uiRangeStart = 0;
}
//---------------------------------------------------------------------------
inline void NiSPWorkload::Reset(NiUInt32 uiInputCount, NiUInt32 uiOutputCount,
    bool bCompact)
{
    NiUInt32 uiInOutCount = uiInputCount + uiOutputCount;
    if (m_uiInOutCount < uiInOutCount ||
        (bCompact && m_uiInOutCount != uiInOutCount))
    {
        m_pkInputsOutputs = (Header*)
            NiRealloc(m_pkInputsOutputs, sizeof(Header) * uiInOutCount);
        m_uiInOutCount = uiInOutCount;
    }
    m_uiInputCount = (NiUInt16) uiInputCount;
    m_uiOutputCount = (NiUInt16) uiOutputCount;

    m_uiBlockCount = 0;
    m_uiRangeStart = 0;
}
//---------------------------------------------------------------------------
inline void NiSPWorkload::Preload()
{
    // No-Op.
}
//---------------------------------------------------------------------------