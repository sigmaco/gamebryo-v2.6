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

#ifndef NIPSFORCE_H
#define NIPSFORCE_H

#include <NiObject.h>
#include "NiPSForceDefinitions.h"

class NiPSParticleSystem;

/**
    The abstract base class for all particle forces.

    Multiple forces can be owned by an NiPSSimulatorForcesStep object, which
    is attached to the NiPSSimulator mesh modifier of a particle system. Each
    force provides a block of memory that contains the data it needs to
    properly execute. In its Update function, it updates that data as
    necessary. The NiPSSimulatorForcesStep object will concatenate the data
    from all its forces into a single data buffer that gets sent to a
    Floodgate kernel.

    In order to properly modify the particle velocity each frame, a function
    needs to be added for each force to the NiPSForceHelpers class. The
    NiPSSimulatorForcesKernel Floodgate kernel loops over all the forces,
    calling the helper function for each and providing that force's data
    buffer to that function. Each force's helper function is provided with the
    array of particle data and is expected to modify the velocity of each
    particle as necessary.

    The only information stored by this base class are the force name, the
    force type, and whether or not the force is active. It contains three pure
    virtual functions used to access the block of data for the force and to
    update the force parameters prior to execution in a Floodgate kernel.
*/
class NIPARTICLE_ENTRY NiPSForce : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSForce);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    //@{
    /**
        Accesses the name of the force.

        The force name is used by time controllers to resolve which force
        should be animated by that controller.
        
        @note The force name should not be programmatically changed unless no
            time controllers exist that reference the force.
    */
    inline const NiFixedString& GetName() const;
    inline void SetName(const NiFixedString& kName);
    //@}

    /**
        Accesses the type of the force.

        The force type is set by each derived class and cannot be changed.
    */
    inline NiPSForceDefinitions::ForceType GetType() const;

    //@{
    /**
        Accesses whether or not the force is active.

        Inactive forces will not be included during kernel execution.
    */
    inline bool GetActive() const;
    inline void SetActive(bool bActive);
    //@}

    /// Returns the size in bytes of the data block provided by the force.
    virtual size_t GetDataSize() const = 0;

    /// Returns a pointer to the data block provided by the force.
    virtual void* GetData() const = 0;

    /// Updates the data for the force.
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime) = 0;

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the force.
        @param eType The type of the force.
    */
    NiPSForce(
        const NiFixedString& kName,
        NiPSForceDefinitions::ForceType eType);

    /// Protected default constructor for cloning and streaming only.
    NiPSForce();
    //@}

private:
    /// The name of the force.
    NiFixedString m_kName;

    /// The type of the force.
    NiPSForceDefinitions::ForceType m_eType;

    /// Whether or not the force is active.
    bool m_bActive;
};

NiSmartPointer(NiPSForce);

#include "NiPSForce.inl"

#endif  // #ifndef NIPSFORCE_H
