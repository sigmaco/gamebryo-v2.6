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


#ifndef NIPHYSXMATERIALDESC_H
#define NIPHYSXMATERIALDESC_H

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
    NiPhysXMaterialDesc objects are for caching snapshots of NxMaterialDesc
    objects, used for streaming, cloning and restoring simulation state.
    
    Applications will typically modify these objects between streaming in
    PhysX content and instantiating it, or if implementing cloning for content
    created in code. Multiple states can be stored corresponding to different
    snapshots of PhysX state.
*/
class NIPHYSX_ENTRY NiPhysXMaterialDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXMaterialDesc);
    /// @endcond

public:
    /// The costructor optionally takes the index of this material in the
    /// PhysX SDK's set of materials. 
    NiPhysXMaterialDesc(const NiUInt16 usIndex = 0);

    /// Destructor frees all allocated memory.
    virtual ~NiPhysXMaterialDesc();

    /// Push the cached state from snapshot into a PhysX material descriptor. 
    ///
    /// The requested state index is not range checked.
    virtual void ToMaterialDesc(NxMaterialDesc& kMaterialDesc,
        const NiUInt32 uiIndex = 0) const;

    /// Push the cached state from snapshot into a PhysX material object.
    ///
    /// The requested state index is not range checked.
    virtual void ToMaterial(NxMaterial* pkMaterial,
        const NiUInt32 uiIndex = 0) const;

    /// Captures state from a PhysX material descriptor and stores it in
    /// state uiIndex (allocating more space if required).    
    virtual void FromMaterialDesc(const NxMaterialDesc& kMaterialDesc,
        const NiUInt32 uiIndex = 0);

    /**
        Captures state from a PhysX material object and stores it in state
        uiIndex.
        
        This simply saves the given material to a descriptor and calls
        FromMaterialDesc. 
    */
    virtual void FromMaterial(NxMaterial* pkMaterial,
        const NiUInt32 uiIndex = 0);
    
    /// Get the material index associated with this material.
    NiUInt16 GetIndex() const;

    /// Set the material index associated with this material.
    void SetIndex(const NiUInt16);

    /**
        Returns true if the material data at the given index is equal to the
        given descriptor.

        Equivalent to a operator==() on NxMaterialDesc, which is not defined
        by PhysX.
    */
    bool IsDataEqual(const NxMaterialDesc& kMaterialDesc,
        const NiUInt32 uiIndex = 0);
        
    /// Remove all the stored materials starting at and beyond the given index.
    virtual void RemoveStatesBeyond(const NiUInt32 uiState);

    /// Remove the stored material at the given index, and shuffle down other
    /// states to fill any hole in stored state indexes.
    virtual void RemoveState(const NiUInt32 uiState);

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    NiUInt16 m_usIndex;
    NiUInt32 m_uiNumStates;
    NxMaterialDesc* m_pkMaterialDescs;
};

NiSmartPointer(NiPhysXMaterialDesc);

#include "NiPhysXMaterialDesc.inl"

#endif // #ifdef NIPHYSXMATERIALDESC_H

