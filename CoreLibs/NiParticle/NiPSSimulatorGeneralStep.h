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

#ifndef NIPSSIMULATORGENERALSTEP_H
#define NIPSSIMULATORGENERALSTEP_H

#include "NiPSSimulatorStep.h"
#include <NiPSSimulatorGeneralKernel.h>
#include <NiSPStream.h>
#include "NiPSKernelDefinitions.h"

/**
    Encapsulates a Floodgate kernel that updates particle grow/shrink, colors,
    and rotations.

    A grow time and a shrink time can be specified for this step. These
    control durations of time during which particles will grow from size 0.0
    to their specified radius and will shrink from their radius to 0.0. The
    grow and shrink generation values specify which particle generation to use
    for the growing and shrinking action. The most common case is for the grow
    generation to be 0 and the shrink generation to be the highest generation
    supported by the particle system (so that particles will grow when born
    and shrink when dying).

    Color keys can be specified that define color animation over the lifetime
    of each particle. The key time values represent a ratio through the
    lifetime of the particle and must start at 0.0 and end at 1.0.

    This step uses the NiPSSimulatorGeneralKernel Floodgate kernel. The kernel
    is only executed if one of the following conditions is met:
    
    - The grow time is greater than 0.0.
    - The shrink time is greater than 0.0.
    - The particle system has colors and color keys have been specified.
    - The particle system has rotations.
*/
class NIPARTICLE_ENTRY NiPSSimulatorGeneralStep : public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSimulatorGeneralStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSSimulatorGeneralStep();

    /// Virtual destructor.
    virtual ~NiPSSimulatorGeneralStep();
    //@}

    /// @name Base Class Overrides
    //@{
    /// Returns the Floodgate kernel associated with the step, which will
    /// be of type NiPSSimulatorGeneralKernel.
    virtual NiSPKernel* GetKernel();

    /// Returns the largest stride value of all the input streams in the step.
    virtual NiUInt16 GetLargestInputStride();

    /**
        Prepares the input stream with the specified semantic for execution.

        The following semantics are recognised:
        NiPSCommonSemantics::PARTICLELASTUPDATE,
        NiPSCommonSemantics::PARTICLEFLAGS,
        NiPSCommonSemantics::PARTICLEAGE,
        NiPSCommonSemantics::PARTICLELIFESPAN,
        NiPSCommonSemantics::PARTICLEROTANGLE and
        NiPSCommonSemantics::PARTICLEROTSPEED. Other semantics will assert
        in Debug builds.
    */
    virtual void PrepareInputStream(
        NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic,
        NiSPStream* pkStream);

    /**
        Prepares the output stream with the specified semantic for execution.

        The following semantics are recognised:
        NiPSCommonSemantics::PARTICLESIZE and
        NiPSCommonSemantics::PARTICLEROTANGLE. Other semantics will assert
        in Debug builds.
    */
    virtual void PrepareOutputStream(
        NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic,
        NiSPStream* pkStream);

    /**
        Creates a task and add several input and output streams, including
        the fixed inputs.
    */
    virtual NiSPTaskPtr Attach(NiPSParticleSystem* pkParticleSystem);

    /**
        Sets all pointers to data streams to NULL.
    */
    virtual void Detach(NiPSParticleSystem* pkParticleSystem);

    /**
        Update function called each frame.

        This function sets the block count on all streams. The function
        returns true if any of the following are true: the particles have
        non-zero grow time, the particles have non-zero fade time, the
        particles have colors and there are color keys, or the particles
        have rotations. If none are true, then the function returns false.
    */
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}

    /**
        Gets the color keys that have been assigned to the step.

        @param ucNumColorKeys The size of the returned color key array.
        @return A pointer to an array of color keys.
    */
    inline NiPSKernelColorKey* GetColorKeys(NiUInt8& ucNumColorKeys) const;

    /**
        Copies the input array of color keys, allocating aligned memory
        internally and copying the data.

        It is valid to specify NULL for pkColorKeys, but ucNumColorKeys must
        also be zero. If pkColorKeys is not NULL, ucNumColorKeys must be
        greater than zero.

        @note This function does not take ownership of pkColorKeys. The
            calling code is responsible for memory management of that data.
        
        @param pkColorKeys A pointer to the array of color keys to copy.
        @param ucNumColorKeys The number of color keys in the pkColorKeys
            array.
    */
    inline void CopyColorKeys(
        NiPSKernelColorKey* pkColorKeys,
        NiUInt8 ucNumColorKeys);

    //@{
    /**
        Accesses the grow time for particles, in seconds.

        The grow time for a particle is the amount of time over which its size
        is ramped from 0.0 to 1.0. The particle size is multiplied by the
        particle radius to compute the final size for the particle.
    */
    inline float GetGrowTime() const;
    inline void SetGrowTime(float fGrowTime);
    //@}

    //@{
    /**
        Accesses the shrink time for particles, in seconds.

        The shrink time for a particle is the amount of time over which its
        size is ramped from 1.0 to 0.0. The particle size is multiplied by the
        particle radius to compute the final size for the particle.
    */
    inline float GetShrinkTime() const;
    inline void SetShrinkTime(float fShrinkTime);
    //@}

    //@{
    /**
        Accesses the grow generation for particles.

        The grow generation specifies the particle generation to which the
        grow effect should be applied. This is usually generation 0, so that
        newly created particles will grow.
    */
    inline NiUInt16 GetGrowGeneration() const;
    inline void SetGrowGeneration(NiUInt16 usGrowGeneration);
    //@}

    //@{
    /**
        Accesses the shrink generation for particles.

        The shrink generation specifies the particle generation to which the
        shrink effect should be applied. This is usually the highest supported
        generation for the particle system, so that particles will shrink
        immediately before getting killed.
    */
    inline NiUInt16 GetShrinkGeneration() const;
    inline void SetShrinkGeneration(NiUInt16 usShrinkGeneration);
    //@}

protected:
    /// The Floodgate kernel used by the step.
    NiPSSimulatorGeneralKernel m_kKernel;

    /// A structure containing input data for the Floodgate kernel.
    NiPSSimulatorGeneralKernelStruct m_kInputStruct;

    /// @name Input Floodgate Streams
    //@{
    NiTSPFixedInput<NiPSSimulatorGeneralKernelStruct> m_kInputStructIS;
    NiSPStream* m_pkLastUpdateIS;
    NiSPStream* m_pkFlagsIS;
    NiSPStream* m_pkAgeIS;
    NiSPStream* m_pkLifeSpanIS;
    NiTSPFixedInput<NiPSKernelColorKey> m_kColorKeyIS;
    NiSPStream* m_pkRotAngleIS;
    NiSPStream* m_pkRotSpeedIS;
    //@}

    /// @name Output Floodgate Streams
    //@{
    NiSPStream* m_pkSizeOS;
    NiSPStream* m_pkColorOS;
    NiSPStream* m_pkRotAngleOS;
    //@}

    /// A pointer to an array of color keys to use for particle color
    /// animation.
    NiPSKernelColorKey* m_pkColorKeys;

    /// The number of color keys in the m_pkColorKeys array.
    NiUInt8 m_ucNumColorKeys;
};

NiSmartPointer(NiPSSimulatorGeneralStep);

#include "NiPSSimulatorGeneralStep.inl"

#endif  // #ifndef NIPSSIMULATORGENERALSTEP_H
