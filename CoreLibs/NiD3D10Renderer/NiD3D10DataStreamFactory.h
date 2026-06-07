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

#ifndef NID3D10DATASTREAMFACTORY_H
#define NID3D10DATASTREAMFACTORY_H

#include "NiD3D10RendererLibType.h"

#include <NiDataStreamFactory.h>

#include "NiD3D10Headers.h"
#include "NiD3D10DataStream.h"

class NID3D10RENDERER_ENTRY NiD3D10DataStreamFactory : 
    public NiDataStreamFactory
{
public:
    NiD3D10DataStreamFactory();
    ~NiD3D10DataStreamFactory();

protected:
    virtual NiDataStream* CreateDataStreamImpl(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage);

    virtual NiDataStream* CreateDataStreamImpl(NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage,
        bool bCanOverrideAccessMask);

    friend NiD3D10DataStream* NiD3D10DataStream::Create(
        ID3D10Buffer* pkBuffer);
};

#endif  //#ifndef NID3D10DATASTREAMFACTORY_H
