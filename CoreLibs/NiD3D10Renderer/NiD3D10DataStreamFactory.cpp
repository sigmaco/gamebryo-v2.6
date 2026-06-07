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

// Precompiled Header
#include "NiD3D10RendererPCH.h"

#include "NiD3D10DataStreamFactory.h"

//---------------------------------------------------------------------------
NiD3D10DataStreamFactory::NiD3D10DataStreamFactory() :
    NiDataStreamFactory()
{
    NiDataStream::SetFactory(this);
}
//---------------------------------------------------------------------------
NiD3D10DataStreamFactory::~NiD3D10DataStreamFactory()
{
    NiDataStream::SetFactory(NULL);
}
//---------------------------------------------------------------------------
NiDataStream* NiD3D10DataStreamFactory::CreateDataStreamImpl(
    const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage)
{
    D3D10_USAGE eD3D10Usage; 
    NiUInt32 uiBindFlags;
    NiUInt32 uiCPUAccessFlags;

    if (!NiD3D10DataStream::InterpretDataStreamFlags(uiAccessMask, eUsage, 
        eD3D10Usage, uiBindFlags, uiCPUAccessFlags))
    {
        return NULL;
    }

    if (uiAccessMask == (NiDataStream::ACCESS_GPU_READ | 
         NiDataStream::ACCESS_GPU_WRITE))
    {
        // This is the case used for Stream Output.
        // Use a NON-lockable data stream here; if they want to access the
        // contents (which is very slow), they should use another path.
        NIASSERT((uiAccessMask & (NiDataStream::ACCESS_CPU_WRITE_ANY)) == 0);
        return NiNew NiD3D10NonLockableDataStream(kElements, uiCount, 
            uiAccessMask, eUsage, eD3D10Usage, uiBindFlags, uiCPUAccessFlags);
    }
    else if ((uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_MUTABLE) != 0)
    {
        return NiNew 
            NiD3D10LockableDataStream<MutableLockPolicy<NiD3D10DataStream> >(
            kElements, uiCount, uiAccessMask, eUsage, eD3D10Usage, 
            uiBindFlags, uiCPUAccessFlags);
    }
    else if ((uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_VOLATILE) != 0)
    {
        return NiNew 
            NiD3D10LockableDataStream<VolatileLockPolicy<NiD3D10DataStream> >(
            kElements, uiCount, uiAccessMask, eUsage, eD3D10Usage, 
            uiBindFlags, uiCPUAccessFlags);
    }
    else 
    {
        NIASSERT((uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_STATIC) != 0)
        return NiNew 
            NiD3D10LockableDataStream<StaticLockPolicy<NiD3D10DataStream> >(
            kElements, uiCount, uiAccessMask, eUsage, eD3D10Usage, 
            uiBindFlags, uiCPUAccessFlags);
    }
}
//---------------------------------------------------------------------------
NiDataStream* NiD3D10DataStreamFactory::CreateDataStreamImpl(
    NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage,
    bool)
{
    D3D10_USAGE eD3D10Usage; 
    NiUInt32 uiBindFlags;
    NiUInt32 uiCPUAccessFlags;

    if (!NiD3D10DataStream::InterpretDataStreamFlags(uiAccessMask, eUsage, 
        eD3D10Usage, uiBindFlags, uiCPUAccessFlags))
    {
        return NULL;
    }

    if ((uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_MUTABLE) != 0)
    {
        return NiNew 
            NiD3D10LockableDataStream<MutableLockPolicy<NiD3D10DataStream> >(
            uiAccessMask, eUsage, eD3D10Usage, uiBindFlags, uiCPUAccessFlags);
    }
    else if ((uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_VOLATILE) != 0)
    {
        return NiNew 
            NiD3D10LockableDataStream<VolatileLockPolicy<NiD3D10DataStream> >(
            uiAccessMask, eUsage, eD3D10Usage, uiBindFlags, uiCPUAccessFlags);
    }
    else 
    {
        NIASSERT((uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_STATIC) != 0);
        return NiNew 
            NiD3D10LockableDataStream<StaticLockPolicy<NiD3D10DataStream> >(
            uiAccessMask, eUsage, eD3D10Usage, uiBindFlags, uiCPUAccessFlags);
    }
}
//---------------------------------------------------------------------------
