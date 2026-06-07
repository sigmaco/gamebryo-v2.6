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

#ifndef NIDATASTREAMFACTORY_H
#define NIDATASTREAMFACTORY_H

#include "NiDataStream.h"

/**
    Interface that allows a renderer (or some other object)
    to create data streams.
*/
class NIMESH_ENTRY NiDataStreamFactory : public NiMemObject
{
public:

    /// Virtual destructor.
    inline virtual ~NiDataStreamFactory(){}

    /// Identical to NiDataStream::CreateDataStream.
    static NiDataStream* CreateDataStream(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage,
        bool bForceCreateToolDS = false);

    /// @cond EMERGENT_INTERNAL

    /**
        Identical to protected NiDataStream::CreateDataStream used by 
        streaming, with one additional parameter.

        @param bCanOverrideAccessMask Flag that indicates whether the function 
            is allowed to change the provided access mask.
    */
    static NiDataStream* CreateDataStream(
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage,
        bool bForceCreateToolDS,
        bool bCanOverrideAccessMask = true);

    /// @endcond

    /// Callback function called during stream creation.
    typedef void (*NiDataStreamFactoryCallback)(
        NiUInt8& uiAccessMask, NiDataStream::Usage eUsage);

    /// Sets the Callback function called during stream creation.
    inline void SetCallback(NiDataStreamFactoryCallback pfnCallback)
        { m_pfnCallback = pfnCallback; }

    /// Gets the Callback function called during stream creation.
    inline NiDataStreamFactoryCallback GetCallback() const
        { return m_pfnCallback; }

    /// Callback function that forces all datastreams to be compliant
    /// with CPU Read access.
    static void ForceCPUReadAccessCallback(
        NiUInt8& uiAccessMask, NiDataStream::Usage eUsage);

protected:

    /// Hidden constructor.
    NiDataStreamFactory() : m_pfnCallback(0) {}

    /**
        Creates a data stream given the specified set of inputs.

        @param kElements Elements encoded in the NiDataStream.
        @param uiCount Number of elements in the NiDataStream.
        @param uiAccessMask Access mask to be enforced by the NiDataStream.
        @param eUsage Usage type for the NiDataStream.

        @return Returns a new NiDataStream subclass or NULL if unable to
            create the instance.
    
        @note Derived classes must implement both versions of 
            CreateDataStreamImpl.

        @note These methods are expected to call the callback function 
            if one is set.
    */
    virtual NiDataStream* CreateDataStreamImpl(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage) = 0;

    /**
        Creates a data stream given the specified set of inputs.

        @param uiAccessMask Access mask to be enforced by the NiDataStream.
        @param eUsage Usage type for the NiDataStream.
        @param bCanOverrideAccessMask Flag that indicates whether the function 
        is allowed to change the provided access mask.

        @return Returns a new NiDataStream subclass or NULL if unable to
            create the instance.
    
        @note Derived classes must implement both versions of 
            CreateDataStreamImpl.

        @note These methods are expected to call the callback function 
            if one is set.
    */
    virtual NiDataStream* CreateDataStreamImpl(NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage,
        bool bCanOverrideAccessMask) = 0;

    /// Callback function called during stream creation.
    NiDataStreamFactoryCallback m_pfnCallback;
};

#endif // #ifndef NIDATASTREAMFACTORY_H
