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

#ifndef NICONTROLLERMANAGER_H
#define NICONTROLLERMANAGER_H

#include <NiTimeController.h>
#include "NiAnimationLibType.h"
#include <NiAVObject.h>
#include "NiSequenceData.h"
#include "NiControllerSequence.h"
#include "NiPoseBinding.h"
#include "NiPoseBlender.h"
#include "NiMultiTargetPoseHandler.h"

class NiAVObjectPalette;
class NiQuatTransform;
class NiSkinningLODController;


class NIANIMATION_ENTRY NiControllerManager : public NiTimeController
{
public:
    NiDeclareRTTI;
    NiDeclareClone(NiControllerManager);
    NiDeclareStream;

    static const float SEQUENCE_DATA_FREQUENCY;

    static NiControllerManager* FindControllerManager(NiAVObject* pkObject);
    static NiAVObject* FindAccumRoot(NiAVObject* pkObject);

    // Constructor and destructor.
    NiControllerManager(NiAVObject* pkTarget,
        bool bCumulativeAnimations = false, 
        NiPoseBinding* pkPoseBinding = NULL, 
        bool bMoveAccumTransformToChild = true);
    virtual ~NiControllerManager();

    // Virtual functions inherited from NiTimeController.
    inline virtual void Start(float){}
    inline virtual void Start(){}
    inline virtual void Stop(){}
    inline virtual void SetTarget(NiObjectNET*){}
    virtual void Update(float fTime);

    // Functions to access held objects.
    NiAVObjectPalette* GetObjectPalette() const;
    NiPoseBinding* GetPoseBinding() const;
    NiPoseBlender* GetPoseBlender() const;
    NiMultiTargetPoseHandler* GetMultiTargetPoseHandler() const;
    NiSkinningLODController* GetSkinningLODController() const;

    // Functions for adding sequence datas to the manager.
    NiSequenceData* AddSequenceDataFromFile(const char* pcFilename,
        unsigned int uiIndex);
    NiSequenceData* AddSequenceDataFromFile(const NiStream& kStream,
        unsigned int uiIndex);
    bool AddAllSequenceDatasFromFile(const char* pcFilename);
    bool AddSequenceData(NiSequenceData* pkSeqData);
    NiSequenceData* AddPoseSequenceData(
        const NiSequenceData* pkSeqDataToSynchronize, 
        bool bTempPose = true, const NiFixedString& kName = NULL);

    // Functions for removing sequence datas from the manager.
    NiSequenceDataPtr RemoveSequenceData(const NiFixedString& kName);
    NiSequenceDataPtr RemoveSequenceData(NiSequenceData* pkSeqData);
    void RemoveAllSequenceDatas();

    // Functions for retrieving sequence datas from the manager.
    unsigned int GetSequenceDataCount() const;
    NiSequenceData* GetSequenceDataByName(const NiFixedString& pcName) const;
    NiSequenceData* GetSequenceDataAt(unsigned int uiIndex) const;

    // Functions for activating sequences.
    NiControllerSequence* ActivateSequence(
        const NiFixedString& kSequenceDataName,
        int iPriority = 0, float fWeight = 1.0f, float fEaseInTime = 0.0f, 
        NiControllerSequence* pkTimeSyncSeq = NULL, 
        float fFrequency = SEQUENCE_DATA_FREQUENCY, 
        float fStartFrame = 0.0f);
    NiControllerSequence* ActivateSequence(
        NiSequenceData* pkSeqData,
        int iPriority = 0, float fWeight = 1.0f, float fEaseInTime = 0.0f, 
        NiControllerSequence* pkTimeSyncSeq = NULL, 
        float fFrequency = SEQUENCE_DATA_FREQUENCY,
        float fStartFrame = 0.0f);

    // Functions for deactivating sequences.
    inline bool DeactivateSequence(const NiFixedString& kSequenceName,
        float fEaseOutTime = 0.0f);
    inline bool DeactivateSequence(NiControllerSequence* pkSequence,
        float fEaseOutTime = 0.0f);
    inline void DeactivateAll(float fEaseOutTime = 0.0f);

    // Functions for retrieving sequences from the manager.
    unsigned int GetSequenceCount() const;
    inline NiControllerSequence* GetSequenceByName(
        const NiFixedString& pcName) const;
    NiControllerSequence* GetSequenceBySequenceData(
        const NiSequenceData* pkSeqData) const;
    NiControllerSequence* GetSequenceAt(unsigned int uiIndex) const;

    // Functions for removing sequences.
    void RemoveAllInactiveSequences(bool bIgnoreRefCount = false);
    void RemoveAllSequences();

    // Functions for setting sequence weight.
    inline bool SetSequenceWeight(const NiFixedString& kSequenceName,
        float fWeight);
    inline bool SetSequenceWeight(NiControllerSequence* pkSequence, 
        float fWeight);

    // Functions for cross fading an active sequence with a 
    // sequence to be activated.
    NiControllerSequence* CrossFade(const NiFixedString& kSourceSequenceName,
        const NiFixedString& kDestSeqDataName, float fDuration,
        int iPriority = 0, float fDestWeight = 1.0f,
        const NiFixedString& kDestTimeSyncSeqName = NULL, 
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    NiControllerSequence* CrossFade(NiControllerSequence* pkSourceSequence,
        NiSequenceData* pkDestSeqData, float fDuration,
        int iPriority = 0, float fDestWeight = 1.0f,
        NiControllerSequence* pkDestTimeSyncSeq = NULL,
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);

    // Functions for performing blend transitions from an active 
    // sequence to a sequence to be activated.
    NiControllerSequence* BlendFromSequence(
        const NiFixedString& kSourceSequenceName,
        const NiFixedString& kDestSeqDataName, float fDuration,
        const NiFixedString& kDestTextKey, int iPriority = 0,
        float fSourceWeight = 1.0f, float fDestWeight = 1.0f,
        const NiFixedString& kDestTimeSyncSeqName = NULL,
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    NiControllerSequence* BlendFromSequence(
        const NiFixedString& kSourceSequenceName,
        const NiFixedString& kDestSeqDataName, float fDuration,
        float fDestFrame = 0.0f, int iPriority = 0,
        float fSourceWeight = 1.0f, float fDestWeight = 1.0f,
        const NiFixedString& kDestTimeSyncSeqName = NULL,
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    NiControllerSequence* BlendFromSequence(
        NiControllerSequence* pkSourceSequence,
        NiSequenceData* pkDestSeqData, float fDuration,
        const NiFixedString& kDestTextKey, int iPriority = 0,
        float fSourceWeight = 1.0f, float fDestWeight = 1.0f,
        NiControllerSequence* pkDestTimeSyncSeq = NULL,
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    NiControllerSequence* BlendFromSequence(
        NiControllerSequence* pkSourceSequence,
        NiSequenceData* pkDestSeqData, float fDuration,
        float fDestFrame = 0.0f, int iPriority = 0,
        float fSourceWeight = 1.0f, float fDestWeight = 1.0f,
        NiControllerSequence* pkDestTimeSyncSeq = NULL,
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    bool StopBlendFromSequence(const NiFixedString& kSourceSequenceName,
        const NiFixedString& kDestSequenceName, float fEaseOutTime = 0.0f);
    bool StopBlendFromSequence(NiControllerSequence* pkSourceSequence,
        NiControllerSequence* pkDestSequence, float fEaseOutTime = 0.0f);

    // Functions for performing morph transitions between 
    // an active sequence and a sequence data.
    NiControllerSequence* Morph(const NiFixedString& kSourceSequenceName,
        const NiFixedString& kDestSeqDataName, 
        float fDuration, int iPriority = 0, float fSourceWeight = 1.0f, 
        float fDestWeight = 1.0f, 
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    NiControllerSequence* Morph(NiControllerSequence* pkSourceSequence,
        NiSequenceData* pkDestSeqData, 
        float fDuration, int iPriority = 0, float fSourceWeight = 1.0f, 
        float fDestWeight = 1.0f, 
        float fDestFrequency = SEQUENCE_DATA_FREQUENCY);
    bool StopMorph(const NiFixedString& kSourceSequenceName,
        const NiFixedString& kDestSequenceName, float fEaseOutTime = 0.0f);
    bool StopMorph(NiControllerSequence* pkSourceSequence,
        NiControllerSequence* pkDestSequence, float fEaseOutTime = 0.0f);

    // Functions for performing blend transitions from the current state of
    // the scene graph to a destination sequence. When possible,
    // BlendFromSequence should be used instead of this, as BlendFromPose is
    // not as efficient at runtime.
    bool BlendFromPose(const NiFixedString& kDestSeqDataName, 
        const NiFixedString& kDestTextKey, float fDuration, int iPriority,
        const NiFixedString& kSeqDataToSynchronize,
        NiControllerSequence*& pkPoseSequence, 
        NiControllerSequence*& pkDestSequence);
    bool BlendFromPose(const NiFixedString& kDestSeqDataName, 
        float fDestFrame, float fDuration, int iPriority, 
        const NiFixedString& kSeqDataToSynchronize,
        NiControllerSequence*& pkPoseSequence, 
        NiControllerSequence*& pkDestSequence);
    bool BlendFromPose(NiSequenceData* pkDestSeqData,
        const NiFixedString& kDestTextKey, float fDuration, int iPriority,
        NiSequenceData* pkSeqDataToSynchronize,
        NiControllerSequence*& pkPoseSequence, 
        NiControllerSequence*& pkDestSequence);
    bool BlendFromPose(NiSequenceData* pkDestSeqData, 
        float fDestFrame, float fDuration, int iPriority,
        NiSequenceData* pkSeqDataToSynchronize,
        NiControllerSequence*& pkPoseSequence, 
        NiControllerSequence*& pkDestSequence);

    // Functions pertaining to transformation accumulation.
    inline bool GetCumulativeAnimations() const;
    const NiFixedString& GetAccumRootName() const;
    NiAVObject* GetAccumRoot() const;
    bool GetAccumulatedTransform(NiQuatTransform& kTransform) const;
    void SetAccumulatedTransform(const NiQuatTransform& kTransform);
    void ClearCumulativeAnimations();

    // Functions for recycling NiControllerSequence objects.
    unsigned short GetNumRecycledSequences() const;
    unsigned short GetMaxRecycledSequences() const;
    void SetMaxRecycledSequences(unsigned short usMaxRecycledSequences);
    void RemoveAllRecycledSequences();

    // *** Begin Emergent internal use only ***
    inline NiControllerSequence* ActivateSequence(
        NiSequenceData* pkSeqData,
        int iPriority, float fWeight, float fEaseInTime, 
        NiControllerSequence* pkTimeSyncSeq, float fFrequency, 
        float fStartFrame, bool bTransition, bool bHoldTransitionFrame);
    inline unsigned int GetNextActivationID();

    inline void AddSequence(NiControllerSequence* pkSequence);
    inline void RemoveSequence(NiControllerSequence* pkSequence);

    bool IsKnownSequenceData(const NiSequenceData* pkSeqData) const;

    unsigned short GetMaxEvaluators() const;
    unsigned short GetMaxUniqueBasisDatas() const;
    unsigned int GetMaxScratchPadBufferSize() const;
    NiControllerSequencePtr GetRecycledSequence();

    // cloning support
    virtual void ProcessClone(
        NiCloningProcess& kCloning);

    // streaming support
    virtual void PostLinkObject(NiStream& kStream);
    // *** End Emergent internal use only ***

protected:
    // Default constructor for use in cloning and streaming only.
    NiControllerManager();

    void Init(NiAVObject* pkTarget, bool bCumulativeAnimations, 
        NiPoseBinding* pkPoseBinding, bool bMoveAccumTransformToChild);
    void Shutdown();

    bool RecycleSequence(NiControllerSequence* pkSequence);

    // Virtual functions inherited from NiTimeController.
    virtual bool TargetIsRequiredType() const;

    NiSequenceDataPointerArray m_kSeqDataArray;
    NiTStringPointerMap<NiSequenceData*> m_kSeqDataMap;

    NiControllerSequencePointerArray m_kSequenceArray;

    bool m_bCumulative;

    // NiAVObject palette for aiding in target lookup.
    NiAVObjectPalettePtr m_spObjectPalette;
    NiPoseBindingPtr m_spPoseBinding;
    NiPoseBlenderPtr m_spPoseBlender;
    NiMultiTargetPoseHandlerPtr m_spMTPoseHandler;
    NiSkinningLODController* m_pkLODController;

    // Recycle NiControllerSequence objects to reduce memory fragmentation.
    unsigned short m_usMaxEvaluators;
    unsigned short m_usMaxUniqueBasisDatas;
    unsigned int m_uiMaxScratchPadBufferSize;
    unsigned short m_usMaxRecycledSequences;
    unsigned short m_usUnusedPadding;
    NiControllerSequencePointerArray m_kRecycledSequenceArray;

    // ID to differentiate between different instances of a recycled sequence.
    unsigned int m_uiActivationID;
};

NiSmartPointer(NiControllerManager);

#include "NiControllerManager.inl"

#endif  // #ifndef NICONTROLLERMANAGER_H
