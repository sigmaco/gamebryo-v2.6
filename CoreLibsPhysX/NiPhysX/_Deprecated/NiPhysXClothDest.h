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

#ifndef NIPHYSXCLOTHDEST_H
#define NIPHYSXCLOTHDEST_H

#include "NiPhysXLibType.h"

#include <NiTriShape.h>
#include <NiPhysXDest.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXClothModifier class.
*/

class NIPHYSX_ENTRY NiPhysXClothDest : public NiPhysXDest
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    /// Destructor clears all smart pointers.
    ~NiPhysXClothDest();

    /// Obtain the target mesh that was streamed in.
    NiTriShape* GetTarget();

    /// Obtain the mesh growth factor that was streamed in.
    float GetMeshGrowFactor() const;

    /// Obtain the flag controlling transformations when the cloth is sleeping.
    bool GetDoSleepXforms() const;
    
    /// Obtain the NBT set to update as the cloth deforms.
    unsigned short GetNBTSet() const;

    /// Update function inherited from NiPhysXDest does nothing.
    virtual void UpdateFromActors(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// Update function inherited from NiPhysXDest does nothing.
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// @cond EMERGENT_INTERNAL

    /// Returns true because we consider it OK to ignore this class when its
    /// loader is unavailable.
    virtual bool StreamCanSkip();

    /// @endcond
    
protected:
    /// Protected constructor so applications cannot create an instance.
    NiPhysXClothDest();

    /// The node that has the geometry data that we are driving.
    NiTriShapePtr m_spTarget;
    
    /// The amount by which the number of vertices is allowed to grow as
    /// cloth tears.
    float m_fMeshGrowFactor;

    /// The texture set to use for NBT generation, if required
    unsigned short m_usNBTSet;

    /// Whether or not to update transforms when the cloth is sleeping.
    bool m_bDoSleepXforms;
};

NiSmartPointer(NiPhysXClothDest);

/// @endcond

#endif  // #ifndef NIPHYSXCLOTHDEST_H
