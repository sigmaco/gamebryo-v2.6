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
#include "NiD3DRendererPCH.h"

#include "NiDX9DataStream.h"
#include "NiDX9DataStreamFactory.h"

//---------------------------------------------------------------------------
NiDataStream* NiDX9DataStreamFactory::CreateDataStreamImpl( 
    const NiDataStreamElementSet& kElements,   
    NiUInt32 uiCount,   
    NiUInt8 uiAccessMask,   
    NiDataStream::Usage eUsage)
{
    if (uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_MUTABLE)
    {
        return NiNew 
            NiDX9LockableDataStream<MutableLockPolicy<NiDX9DataStream> >
            (kElements, uiCount, uiAccessMask, eUsage);
    }
    else if (uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_VOLATILE)
    {
        return NiNew 
            NiDX9LockableDataStream<VolatileLockPolicy<NiDX9DataStream> >(
            kElements, uiCount, uiAccessMask, eUsage);
    }
    else
    {
        return NiNew 
            NiDX9LockableDataStream<StaticLockPolicy<NiDX9DataStream> >(
            kElements, uiCount, uiAccessMask, eUsage);
    }    
}
//---------------------------------------------------------------------------
NiDataStream* NiDX9DataStreamFactory::CreateDataStreamImpl( 
    NiUInt8 uiAccessMask,   
    NiDataStream::Usage eUsage,
    bool)
{
    if (uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_MUTABLE)
    {
        return NiNew 
            NiDX9LockableDataStream<MutableLockPolicy<NiDX9DataStream> >(
            uiAccessMask, eUsage);
    }
    else if (uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_VOLATILE)
    {
        return NiNew 
            NiDX9LockableDataStream<VolatileLockPolicy<NiDX9DataStream> >(
            uiAccessMask, eUsage);
    }
    else
    {
        return NiNew 
            NiDX9LockableDataStream<StaticLockPolicy<NiDX9DataStream> >(
            uiAccessMask, eUsage);
    }}
//---------------------------------------------------------------------------
