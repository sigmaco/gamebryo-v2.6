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

#ifndef NIDX9DATASTREAMFACTORY_H
#define NIDX9DATASTREAMFACTORY_H

#include <NiDataStreamFactory.h>

class NIDX9RENDERER_ENTRY NiDX9DataStreamFactory : public NiDataStreamFactory
{
protected:
    virtual NiDataStream* CreateDataStreamImpl(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage);

    virtual NiDataStream* CreateDataStreamImpl(
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage,
        bool bCanOverrideAccessMask);

};

#endif
