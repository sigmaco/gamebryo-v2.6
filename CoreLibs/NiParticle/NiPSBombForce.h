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

#ifndef NIPSBOMBFORCE_H
#define NIPSBOMBFORCE_H

#include "NiPSForce.h"

/**
    Applies an explosive force to particles.

    The position and orientation of the bomb are determined from the bomb
    object in the scene graph and the bomb axis. The bomb axis specifies which
    axis of the bomb object represents the direction it is facing, which is
    used primarily for planar and spherical bombs.
    
    The bomb has several parameters, including decay, acceleration,
    a decay type, and a symmetry type.
*/
class NIPARTICLE_ENTRY NiPSBombForce : public NiPSForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSBombForce);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the force.
        @param pkBombObj The object in the scene graph whose world-space
            transformations will be used to represent the position and
            orientation of the bomb.
        @param kBombAxis The axis of the bomb object's transforms to use as a
            direction vector for planar and spherical bombs.
        @param fDecay How the bomb's force will decrease with distance.
        @param fDeltaV The acceleration the bomb will apply to the particles.
        @param eDecayType The type of decay used by the bomb.
        @param eSymmType The type of symmetry employed by the bomb.
    */
    NiPSBombForce(
        const NiFixedString& kName,
        NiAVObject* pkBombObj = NULL,
        NiPoint3 kBombAxis = NiPoint3::UNIT_X,
        float fDecay = 0.0f,
        float fDeltaV = 0.0f,
        NiPSForceDefinitions::BombData::DecayType eDecayType =
            NiPSForceDefinitions::BombData::DECAY_NONE,
        NiPSForceDefinitions::BombData::SymmType eSymmType =
            NiPSForceDefinitions::BombData::SYMM_SPHERICAL);
    //@}

    /// @name Base Class Overrides
    //@{
    virtual size_t GetDataSize() const;
    virtual void* GetData() const;
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}
    ///

    //@{
    /// Accesses the object in the scene graph whose world-space
    /// transformations will be used to represent the position and orientation
    /// of the bomb.
    inline NiAVObject* GetBombObj() const;
    inline void SetBombObj(NiAVObject* pkBombObj);
    //@}

    //@{
    /// Accesses the axis of the bomb object's transforms to use as a
    /// direction vector for planar and spherical bombs.
    inline const NiPoint3& GetBombAxis() const;
    inline void SetBombAxis(const NiPoint3& kBombAxis);
    //@}

    //@{
    /**
        Accesses how the bomb's force will decrease with distance.

        This value is only used with the LINEAR and EXPONENTIAL decay types.
        No attenuation of the force will occur within fDecay of the bomb.
    */
    inline float GetDecay() const;
    inline void SetDecay(float fDecay);
    //@}

    //@{
    /// Accesses the acceleration the bomb will apply to the particles.
    inline float GetDeltaV() const;
    inline void SetDeltaV(float fDeltaV);
    //@}

    //@{
    /// Accesses the type of decay used by the bomb.
    inline NiPSForceDefinitions::BombData::DecayType GetDecayType() const;
    inline void SetDecayType(
        NiPSForceDefinitions::BombData::DecayType eDecayType);
    //@}

    //@{
    /// Accesses the type of symmetry employed by the bomb.
    inline NiPSForceDefinitions::BombData::SymmType GetSymmType() const;
    inline void SetSymmType(
        NiPSForceDefinitions::BombData::SymmType eSymmType);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSBombForce();
    //@}
    ///

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::BombData m_kInputData;

    /// A pointer to the bomb object in the scene graph.
    NiAVObject* m_pkBombObj;
};

NiSmartPointer(NiPSBombForce);

#include "NiPSBombForce.inl"

#endif  // #ifndef NIPSBOMBFORCE_H
