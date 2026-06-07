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


#ifndef NIPHYSXSDKDESC_H
#define NIPHYSXSDKDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXSDKDesc objects are for caching and streaming the complete
    parameter set in a NxPhysicsSDK object.
    
    A parameter is anything set though a NxPhysicsSDK::setParameter call.
    It also maintains a NxPhysicsSDKDesc for the hardware parameter data
    used in creating the SDK. Applications can create and stream as many
    of these as desired. They are particularly useful for storing sets of
    debug visualization settings, as these can otherwise be unwieldy to
    manage. 

    @note Insert one of these into the top-level object list of an NiStream
        object to stream it, or look for one in the top-level objects of a
        loaded stream. The class is not automatically inserted into the
        stream by any other object, so an application must manage it
        explicitly. 
*/
class NIPHYSX_ENTRY NiPhysXSDKDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    /// @endcond

public:
    /// Captures the state of the PhysX SDK object held by the NiPhysXManager
    /// object.
    void FromSDK();

    /**
        Pushes the cached state into the PhysX SDK object held by the
        NiPhysXManager object.
        
        Basically, it loops over all the parameters calling
        NxPhysicsSDK::setParameter on each.
    */
    void ToSDK() const;
    
    /**
        Get an individual parameter.
        
        This function operates on the internal values only - it does not
        get the value from the SDK.
    */
    NxReal GetParameter(const NxParameter kIndex) const;

    /**
        Set an individual parameter.
        
        This function operates on the internal values only - it does not
        push the value to the SDK.
    */
    void SetParameter(const NxParameter kIndex, const NxReal kVal);

    /// Set the internal copy of the NxPhysicsSDKDesc.
    void SetSDKDesc(const NxPhysicsSDKDesc& kSDKDesc);

    /// Get the internal copy of the NxPhysicsSDKDesc.
    void GetSDKDesc(NxPhysicsSDKDesc& kSDKDesc) const;

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    NxPhysicsSDKDesc m_kSDKDescriptor;
    NxReal m_afSDKParams[NX_PARAMS_NUM_VALUES];
};

NiSmartPointer(NiPhysXSDKDesc);

#endif // #ifdef NIPHYSXSDKDESC_H

