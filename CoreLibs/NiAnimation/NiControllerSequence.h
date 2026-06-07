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

#ifndef NICONTROLLERSEQUENCE_H
#define NICONTROLLERSEQUENCE_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiAnimState.h"
#include "NiTextKeyMatch.h"
#include "NiPoseBuffer.h"
#include "NiSequenceData.h"
#include "NiScratchPad.h"

class NiControllerManager;

NiSmartPointer(NiControllerSequence);
typedef NiTObjectArray<NiControllerSequencePtr> 
    NiControllerSequencePointerArray;


class NIANIMATION_ENTRY NiControllerSequence : public NiObject
{
public:
    NiDeclareRTTI;

public:
    static const float INVALID_TIME;
    static const float CURRENT_FRAME;
    static const float SEQUENCE_DATA_FREQUENCY;

    // Destructor.
    ~NiControllerSequence();

    // Functions for getting the sequence name.
    inline const NiFixedString& GetName() const;

    // Function that returns a "unique" ID for differentiating between 
    // different instances of a recycled sequence.
    inline unsigned int GetActivationID() const;

    // Functions to get associated objects
    NiSequenceData* GetSequenceData() const;
    inline NiTextKeyExtraData* GetTextKeys() const;
    NiControllerSequence* GetTimeSyncSequence() const;
    NiPoseBuffer* GetPoseBuffer() const;
    NiScratchPad* GetScratchPad() const;

    // Function to reset the sequence to the beginning.
    inline void ResetSequence();

    // Function that returns the animation state for this sequence.
    NiAnimState GetState() const;

    // Functions for getting and setting the offset for this sequence.
    inline float GetOffset() const;
    inline void SetOffset(float fOffset);

    // Functions for getting and setting the sequence priority and weight.
    int GetPriority() const;
    float GetWeight() const;
    void SetWeight(float fWeight);

    // Functions for getting and setting timing information.
    float GetDuration() const;
    float GetDurationDivFreq() const;
    NiTimeController::CycleType GetCycleType() const;
    bool SetCycleType(NiTimeController::CycleType eType);
    float GetFrequency() const;

    // Functions to adjust time by frequency of sequence.
    float TimeDivFreq(float fTime) const;
    float TimeMultFreq(float fTime) const;

    // Functions to provide easy access to the last stored time values.
    inline float GetLastTime() const;
    inline float GetLastScaledTime() const;

    // Function for looking up the time of a text key.
    float GetTimeAt(const NiFixedString& kTextKey, float fCurrentTime = 0.0f);
    float GetTimeAt(NiTextKeyMatch* pkMatchObject, float fCurrentTime = 0.0f);

    // Function for looking up the key time for a particular text key.
    float GetKeyTimeAt(const NiFixedString& kTextKey) const;
    float GetKeyTimeAt(NiTextKeyMatch* pkMatchObject) const;

    // Functions dealing with evaluators.
    unsigned int GetNumEvaluators() const;
    NiEvaluator* GetEvaluatorAt(unsigned int uiIndex) const;
    void DisableEvaluatorAt(unsigned int uiIndex, bool bInvalidatePBData);
    void DisableEvaluatorChannelAt(unsigned int uiEvaluatorIndex, 
        unsigned int uiChannelIndex, bool bInvalidatePBData);

    // Functions related to callback objects.
    // Initialized to default list in NiSequenceData upon activation.
    void AddActivationCallback(
        NiSequenceData::ActivationCallback* pkCallback);
    void RemoveActivationCallback(
        NiSequenceData::ActivationCallback* pkCallback);
    unsigned int GetActivationCallbackCount() const;
    NiSequenceData::ActivationCallback* GetActivationCallbackAt(
        unsigned int uiIndex);

    // *** begin Emergent internal use only ***
    enum
    {
        LODSKIPEVALUATOR = -2,
        EVALMAXCHANNELS = NiEvaluator::EVALMAXCHANNELS,
    };

    NiControllerSequence();

    // Functions for getting and setting the controller manager on which
    // this active sequence is being played.
    inline NiControllerManager* GetOwner() const;
    inline bool SetOwner(NiControllerManager* pkOwner);

    bool SetSequenceData(NiSequenceData* pkSequenceData);

    void Update(float fTime, int iLOD, bool bUpdateEvaluators);

    static NiControllerSequence* Activate(NiControllerManager* pkOwner, 
        NiSequenceData* pkSeqData, int iPriority, 
        float fWeight, float fEaseInTime, 
        NiControllerSequence* pkTimeSyncSeq = NULL, 
        float fFrequency = SEQUENCE_DATA_FREQUENCY,
        float fStartFrame = 0.0f, 
        bool bTransition = false, 
        bool bHoldTransitionFrame = false);
    bool Reactivate(int iPriority, 
        float fWeight, float fEaseInTime, 
        NiControllerSequence* pkTimeSyncSeq = NULL, 
        float fStartFrame = CURRENT_FRAME, 
        bool bTransition = false, 
        bool bProcessCallbacks = true);
    bool Deactivate(float fEaseOutTime, 
        bool bTransition = false, 
        bool bHoldTransitionFrame = false, 
        bool bProcessCallbacks = true);

    NiControllerSequence* StartBlend(NiSequenceData* pkDestSeqData, 
        float fDuration, const char* pcDestTextKey, int iPriority, 
        float fSourceWeight, float fDestWeight, 
        NiControllerSequence* pkDestTimeSyncSeq = NULL, 
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    NiControllerSequence* StartBlend(NiSequenceData* pkDestSeqData, 
        float fDuration, float fDestFrame, int iPriority, 
        float fSourceWeight, float fDestWeight, 
        NiControllerSequence* pkDestTimeSyncSeq = NULL, 
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);

    NiControllerSequence* StartMorph(NiSequenceData* pkDestSeqData, 
        float fDuration, int iPriority, float fSourceWeight, 
        float fDestWeight, 
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);

    float FindCorrespondingMorphFrame(NiControllerSequence* pkSourceSequence,
        float fSourceSequenceTime);
    float ComputeScaledTime(float fTime, bool bStoreLastTime = true);

    float GetSpinnerScaledWeight() const;
    float GetEaseSpinner() const;
    inline float GetEaseEndTime() const;
    inline float GetDestFrame() const;
    bool IsMoreImportantThan(const NiControllerSequence* pkSequence) const;

    void PrepareForRecycling();

    void SetLastAccumTransform(const NiQuatTransform& kTransform);
    void ClearAccumTransformData();

    // *** end Emergent internal use only ***

protected:
    bool VerifyDependencies(NiControllerSequence* pkSequence);
    bool VerifyMatchingMorphKeys(NiControllerSequence* pkTimeSyncSeq);

    // ID to differentiate between different instances of a recycled sequence.
    unsigned int m_uiActivationID;

    // The controller manager that owns this sequence.
    NiControllerManager* m_pkOwner;

    // The buffer to which the animated pose is written on each update call.
    NiPoseBufferPtr m_spPoseBuffer;
    unsigned short m_usNumPoseBufferItems;
    short m_sLastPoseBufferUpdateLOD;
    float m_fLastPoseBufferUpdateTime;

    // The scratch pad which holds instance data for the evaluators.
    NiScratchPadPtr m_spScratchPad;

    // The shared sequence data containing the source animation content.
    NiSequenceDataPtr m_spSeqData;

    // Text keys which point to times in the animation.
    NiTextKeyExtraDataPtr m_spTextKeys;

    // Priority and weights of the sequence.
    int m_iPriority;
    float m_fWeight;
    float m_fTransSpinner;
    float m_fEaseSpinner;

    // Timing variables.
    float m_fDuration;
    NiTimeController::CycleType m_eCycleType;
    float m_fFrequency;
    float m_fLastTime;
    float m_fWeightedLastTime;
    float m_fLastScaledTime;

    // The current state of the sequence.
    NiAnimState m_eState;

    // Variables used for time measurement.
    float m_fOffset;
    float m_fStartTime;
    float m_fEndTime;
    float m_fDestFrame;
    bool m_bHoldDestFrame;

    // Variables used for time synchronization.
    NiControllerSequencePtr m_spPartnerSequence;

    // Callback objects.
    NiTPrimitiveArray<NiSequenceData::ActivationCallback*> 
        m_kActivationCallbacks;
};

#include "NiControllerSequence.inl"

#endif  // #ifndef NICONTROLLERSEQUENCE_H
