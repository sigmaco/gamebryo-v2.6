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

#include "NiTerrainPCH.h"

#include "NiDynamicStreamCache.h"

//---------------------------------------------------------------------------
NiDynamicStreamCache::NiDynamicStreamCache(NiUInt32 uiVertsPerBlock) :
    m_uiStreamSize(uiVertsPerBlock) 
{
    for (NiUInt8 ucPos = 0; ucPos < MAX_TYPE; ++ucPos)
        m_auiCurrentPosition[ucPos] = 0;
}
//---------------------------------------------------------------------------
NiDynamicStreamCache::~NiDynamicStreamCache()
{
    for (NiUInt8 ucStreamType = 0; ucStreamType < MAX_TYPE; ++ucStreamType)
    {
        for (NiUInt32 ui = 0; ui < m_akStreams[ucStreamType].GetSize(); ++ui)
            m_akStreams[ucStreamType].SetAt(ui, 0);
    }
}
//---------------------------------------------------------------------------
NiDataStream* NiDynamicStreamCache::RequestStream(STREAM_TYPES eStreamType)
{
    // Do we need to grow?
    if (m_auiCurrentPosition[eStreamType] ==m_akStreams[eStreamType].GetSize())
    {
        // Now initialize the array of streams
        NiDataStream* pkStream;
        m_akStreams[eStreamType].SetSize(m_auiCurrentPosition[eStreamType] + 
            m_akStreams[eStreamType].GetGrowBy());

        for (NiUInt32 ui = m_auiCurrentPosition[eStreamType]; 
            ui < m_akStreams[eStreamType].GetAllocatedSize(); ++ui)
        {
            pkStream = NiDataStream::CreateDataStream(
                m_akElements[eStreamType], 
                m_uiStreamSize, 
                m_aucAccessMask[eStreamType], 
                m_aeUsage[eStreamType], 
                false);

            m_akStreams[eStreamType].SetAt(ui, pkStream);
        }
    }

    return m_akStreams[eStreamType][m_auiCurrentPosition[eStreamType]++];
}
//---------------------------------------------------------------------------
bool NiDynamicStreamCache::InitializeStreamCache(STREAM_TYPES eStreamType, 
    NiUInt32 uiInitialSize, NiUInt32 uiGrowBy,
    const NiDataStreamElementSet& kElements,
    NiUInt8 ucAccessMask, NiDataStream::Usage eUsage)
{
    NIASSERT(m_akStreams[eStreamType].GetSize() == 0);

    // Elements
    for (NiUInt8 ui = 0; ui < kElements.GetSize(); ++ui)
    {
        NiDataStreamElement kElement = kElements.GetAt(ui);
        m_akElements[eStreamType].Add(kElement);
        m_akElements[eStreamType].m_uiStride = kElements.m_uiStride;
    }

    // access mask
    m_aucAccessMask[eStreamType] = ucAccessMask;

    // Usage
    m_aeUsage[eStreamType] = eUsage;

    // Now initialise the array of streams
    NiDataStream* pkStream;
    m_akStreams[eStreamType].SetSize(uiInitialSize);
    m_akStreams[eStreamType].SetGrowBy(uiGrowBy);
    for (NiUInt32 ui = 0; ui < uiInitialSize; ++ui)
    {
        pkStream = NiDataStream::CreateDataStream(
            kElements, m_uiStreamSize, ucAccessMask, eUsage, false);

        if (!pkStream || !pkStream->GetSize())
        {
            // Could not allocate a data stream, may be out of memory?
            m_akStreams[eStreamType].SetSize(ui);
            return false;
        }

        m_akStreams[eStreamType].SetAt(ui, pkStream);
    }

    // All streams allocated successfully
    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiDynamicStreamCache::GetCurrentSize(STREAM_TYPES eStreamType)
{
    return m_auiCurrentPosition[eStreamType];
}
//---------------------------------------------------------------------------
NiUInt32 NiDynamicStreamCache::GetMaxSize(STREAM_TYPES eStreamType)
{
    return m_akStreams[eStreamType].GetSize();
}
//---------------------------------------------------------------------------
NiUInt32 NiDynamicStreamCache::GetNumAvailable(STREAM_TYPES eStreamType)
{
    return m_akStreams[eStreamType].GetSize() - 
        m_auiCurrentPosition[eStreamType];
}
