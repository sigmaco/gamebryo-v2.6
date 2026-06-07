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

#ifndef NIDATASTREAMFACTORYPROXY_H
#define NIDATASTREAMFACTORYPROXY_H

#include "NiDataStreamFactory.h"
#include "NiTMap.h"

/**
    This class acts as a proxy to a data stream factory T.  T must
    have a default constructor, be derived from NiDataStreamFactory, and
    not be abstract.

    This proxy records the data streams that were created and can return the
    original access mask and usage semantics.  Additionally, it calls the
    ForceCPURead function itself *after* the NiDataStream::CreateDataStream
    function where the callback is usually made so that the virtual function
    here can get the original value.  Therefore, no callback should be set
    when using this class.
*/
template<class T>
class NiTDataStreamFactoryProxy : public T
{
protected:
    virtual NiDataStream* CreateDataStreamImpl(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage);

    virtual NiDataStream* CreateDataStreamImpl(NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage,
        bool bCanOverrideAccessMask);
};

#include "NiTDataStreamFactoryProxy.inl"

#endif // #ifndef NIDATASTREAMFACTORYPROXY_H