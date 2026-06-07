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

#ifndef NIPHYSXMESHPSYSPROP_H
#define NIPHYSXMESHPSYSPROP_H

#include "NiPhysXParticleLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXProp.h"

class NiPhysXMeshParticleSystem;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXPSMeshParticleSystemProp class.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXMeshPSysProp : public NiPhysXProp
{
    /// @cond EMERGENT_INTERNAL
    
    NiDeclareStream;
    NiDeclareRTTI;

    /// @endcond

public:
    /// Destructor.
    ~NiPhysXMeshPSysProp();
    
    /// Get the number of particle systems found in this prop.
    NiUInt32 GetSystemCount() const;

    /// Get a particle system by index.
    NiPhysXMeshParticleSystem* GetSystemAt(NiUInt32 uiIndex);
       
    /// @cond EMERGENT_INTERNAL
    
    NiPhysXMeshPSysProp();

    void AddSystem(NiPhysXMeshParticleSystem* pkSystem);

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    NiTPrimitiveArray<NiPhysXMeshParticleSystem*> m_kSystems;
};

NiSmartPointer(NiPhysXMeshPSysProp);

/// @endcond

#endif  // #ifndef NIPHYSXMESHPSYSPROP_H

