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

#ifndef NIPSEMITPARTICLESCTLR_H
#define NIPSEMITPARTICLESCTLR_H

#include "NiParticleLibType.h"
#include "NiPSEmitterCtlr.h"

class NiFloatInterpolator;
class NiBoolInterpolator;

/**
    Animates particle emission and birth rate.

    This controller contains two interpolators that control when an
    NiPSEmitter should emit particles. The two interpolators hold an array of
    NiFloatKey objects that define the birth rate over time and an array of
    NiBoolKey objects that indicate whether or not the emitter is active over
    time. Sequence animation relies upon two evaluators instead of two 
    interpolators; however, their operation is similar.
    
    The Update and UpdateValue functions of this controller determine how 
    many new particles, if any, need to be emitted and then call 
    EmitParticles on its associated NiPSEmitter object.

    @note No particles will be emitted if the time range for this controller
        is zero. Even if both interpolators are posed, the time range for this
        controller should be set to a non-zero value, such as 1.0, in order
        for particles to be emitted properly.
*/
class NIPARTICLE_ENTRY NiPSEmitParticlesCtlr : public NiPSEmitterCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSEmitParticlesCtlr);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kEmitterName The name of the NiPSEmitter object that should be
            used to emit particles.
    */ 
    NiPSEmitParticlesCtlr(const NiFixedString& kEmitterName);

    /// @name Base Class Overrides
    //@{
    /// This function marks this controller as inactive.
    virtual void Stop();
    /// This function determines how many new particles, if any, need to be 
    /// emitted and then calls EmitParticles on the associated NiPSEmitter 
    /// object.
    virtual void Update(float fTime);
    /// This function sets the specified evaluator at the specified 
    /// evaluator index. After the BirthRate and EmitterActive 
    /// evaluators have both been set via this function for a given fTime,
    /// this function determines how many new particles, if any, need to be 
    /// emitted and then calls EmitParticles on the associated NiPSEmitter 
    /// object. ppkEvalSPData may be NULL when the evaluator contains 
    /// posed (i.e. constant) data.
    virtual bool UpdateValue(float fTime, 
        NiControllerSequence* pkControllerSequence, 
        NiEvaluator* pkEvaluator, 
        NiEvaluatorSPData** ppkEvalSPData,
        unsigned short usIndex = 0);
    //@}
    ///

    //@{
    /// Accesses the interpolator that animates the emission birth rate.
    NiFloatInterpolator* GetBirthRateInterpolator() const;
    void SetBirthRateInterpolator(NiFloatInterpolator* pkInterpolator);
    //@}

    //@{
    /// Accesses the interpolator that animates when particles should be
    /// emitted.
    NiBoolInterpolator* GetEmitterActiveInterpolator() const;
    void SetEmitterActiveInterpolator(NiBoolInterpolator* pkInterpolator);
    //@}
    
    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);
    virtual void ResetTimeExtrema();

    virtual unsigned short GetInterpolatorCount() const;
    virtual const char* GetInterpolatorID(unsigned short usIndex = 0);
    virtual unsigned short GetInterpolatorIndex(const char* pcID) const;
    
    virtual NiInterpolator* GetInterpolator(unsigned short usIndex = 0) const;
    virtual void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned short usIndex = 0);

    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) const;

    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSEmitParticlesCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;
    //@}

    /// Common functions that perform most of the particle update work.
    void UpdateParticleSystem(float fTime);
    void UpdateParticleSystemEval(float fTime);

    /**
        Helper function that computes the number of particles to emit based on
        the input parameters and calls EmitParticles on the NiPSEmitter.

        Requires that the input parameters meet the following constraints:
            - fBirthRate > 0.0f
            - fEmitStart < fEmitStop 
            - fCurTime > fEmitStart
            - fLastTime < fEmitStop

        @param fTime The time passed into the Update function.
        @param fCurTime The current time used to determine how many particles
            to emit.
        @param fLastTime The last time at which particles were emitted.
        @param fEmitStart The start time for particle emission.
        @param fEmitStop The stop time for particle emission.
        @param fBirthRate The birth rate to use when emitting particles.
    */
    void EmitParticles(
        float fTime,
        float fCurTime,
        float fLastTime,
        float fEmitStart,
        float fEmitStop,
        float fBirthRate);

    /// The interpolator that controls whether or not the emitter is active.
    NiInterpolatorPtr m_spEmitterActiveInterpolator;

    /// A cached last birth rate interpolator used to detect when the
    /// interpolator has been changed.
    NiInterpolator* m_pkLastBirthRateInterpolator;

    /// The last scaled time at which the controller was updated.
    float m_fLastScaledTime;

    /// The last computed active state for the emitter.
    bool m_bLastActive;

    /// An array of particle ages that gets passed to the emitter when
    /// emitting particles.
    NiFloatSet m_kParticleAges;

    /// Arrays to temporarily hold the evaluators and their 
    /// associated scratch pad data for UpdateValue.
    enum
    {
        BIRTH_RATE = 0,
        EMITTER_ACTIVE,
        NUM_EVALUATORS,
    };
    NiEvaluator* m_apkEvaluator[NUM_EVALUATORS];
    NiEvaluatorSPData* m_apkEvalSPData[NUM_EVALUATORS];

    /// A cached last birth rate evaluator and sequence activation ID 
    /// for detecting when the evaluator has been changed.
    NiEvaluator* m_pkLastBirthRateEvaluator;
    unsigned int m_uiLastSequenceActivationID;

    /// The last time and index that UpdateValue was called.
    float m_fLastUpdateValueTimeEval;
    unsigned short m_usLastUpdateValueIndexEval;

    //@{
    /// Name and index constants for use when attaching and detaching
    /// interpolators.
    static const char* ms_pcBirthRate;
    static const char* ms_pcEmitterActive;
    static const NiUInt16 ms_usBirthRateIndex;
    static const NiUInt16 ms_usEmitterActiveIndex;
    //@}
};

NiSmartPointer(NiPSEmitParticlesCtlr);

#endif  // #ifndef NIPSEMITPARTICLESCTLR_H
