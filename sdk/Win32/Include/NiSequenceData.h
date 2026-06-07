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

#ifndef NISEQUENCEDATA_H
#define NISEQUENCEDATA_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiAnimState.h"
#include "NiInterpolator.h"
#include "NiEvaluator.h"
#include "NiTextKeyExtraData.h"
#include "NiStringPalette.h"

class NiTextKeyMatch;
class NiSkinningLODController;
class NiControllerSequence;
class NiControllerManager;

NiSmartPointer(NiSequenceData);
typedef NiTObjectArray<NiSequenceDataPtr> NiSequenceDataPointerArray;


class NIANIMATION_ENTRY NiSequenceData : public NiObject
{
public:
    NiDeclareRTTI;
    NiDeclareClone(NiSequenceData);
    NiDeclareStream;

    static const float INVALID_TIME;
    static const unsigned int INVALID_INDEX;

    // Callback object. ActivationChanged is called whenever the state
    // of this sequence changes. This is a pure virtual function, so 
    // feel free to use it with multiple inheritance.
    class NIANIMATION_ENTRY ActivationCallback
    {
    public:
        virtual ~ActivationCallback();
        virtual void ActivationChanged(NiControllerSequence* pkSequence,
            NiAnimState eState) = 0;
    };

    // Constructor and destructor.
    NiSequenceData(const NiFixedString& kName,
        unsigned int uiMaxEvaluators = 12);
    ~NiSequenceData();

    // Static functions for creating sequence datas from KF files.
    static NiSequenceDataPtr CreateSequenceDataFromFile(
        const char* pcFilename, const NiFixedString& kSequenceName);
    static NiSequenceDataPtr CreateSequenceDataFromFile(
        const NiStream& kStream, const NiFixedString& kSequenceName);
    static NiSequenceDataPtr CreateSequenceDataFromFile(
        const char* pcFilename, unsigned int uiIndex);
    static NiSequenceDataPtr CreateSequenceDataFromFile(
        const NiStream& kStream, unsigned int uiIndex);
    static bool CreateAllSequenceDatasFromFile(const char* pcFilename,
        NiSequenceDataPointerArray& kSeqDatas);
    static bool CreateAllSequenceDatasFromFile(const NiStream& kStream,
        NiSequenceDataPointerArray& kSeqDatas);

    // Functions for getting and setting the sequence data name.
    const NiFixedString& GetName() const;
    void SetName(const NiFixedString& kName);

    // Functions for getting and setting the text keys.
    NiTextKeyExtraData* GetTextKeys() const;
    void SetTextKeys(NiTextKeyExtraData* pkKeys);

    // Functions related to default callback objects.
    void AddDefaultActivationCallback(ActivationCallback* pkCallback, 
        NiControllerManager* pkManager = NULL);
    void RemoveDefaultActivationCallback(ActivationCallback* pkCallback,
        NiControllerManager* pkManager = NULL);
    unsigned int GetDefaultActivationCallbackCount() const;
    void GetDefaultActivationCallbackAt(unsigned int uiIndex,
        ActivationCallback*& pkCallback, NiControllerManager*& pkManager);

    // Functions for adding and removing evaluators (for use by animation tools).
    unsigned int GetMaxEvaluators() const;
    void SetMaxEvaluators(unsigned int uiMax);
    unsigned int AddEvaluator(NiEvaluator* pkEvaluator);
    NiEvaluatorPtr RemoveEvaluator(unsigned int uiIndex);

    // Functions for sorting evaluators (for use by animation tools).
    void SortEvaluatorsByLOD(NiSkinningLODController* pkLODController,
        NiAVObjectPalette* pkPalette);
    void SortEvaluatorsByPBChannelType();

    // Functions for retrieving evaluators.
    unsigned int GetNumEvaluators() const;
    NiEvaluator* GetEvaluatorAt(unsigned int uiIndex) const;

    // Function for looking up the key time for a particular text key.
    float GetKeyTimeAt(const NiFixedString& kTextKey) const;
    float GetKeyTimeAt(NiTextKeyMatch* pkMatchObject) const;

    // Adjusted functions adjust by frequency of sequence.
    float TimeDivFreq(float fTime) const;
    float TimeMultFreq(float fTime) const;

    // Functions for getting and setting timing variables.
    float GetDuration() const;
    float GetDurationDivFreq() const;
    void SetDuration(float fDuration); // For use by creation code and tools.
    NiTimeController::CycleType GetCycleType() const;
    bool SetCycleType(NiTimeController::CycleType eType);
    float GetFrequency() const;
    void SetFrequency(float fFrequency);

    // *** begin Emergent internal use only ***

    // This internal IDTag class has been deprecated.
    //
    // This class has been replaced by NiEvaluator::IDTag which performs
    // a similar role. Functionality exists here only to enable streaming 
    // of existing assets.
    class NIANIMATION_ENTRY IDTag : public NiMemObject
    {
    public:
        IDTag();

        const NiFixedString& GetAVObjectName() const;
        const NiFixedString& GetPropertyType() const;
        const NiFixedString& GetCtlrType() const;
        const NiFixedString& GetCtlrID() const;
        const NiFixedString& GetInterpolatorID() const;

        void LoadBinary(NiStream& kStream,
             void*& pvDeprecatedExtraData);
        void HandleDeprecatedExtraData(NiStringPalette* pkPalette,
            NiStream& kStream);

    protected:
        NiFixedString m_kAVObjectName;
        NiFixedString m_kPropertyType;
        NiFixedString m_kCtlrType;
        NiFixedString m_kCtlrID;
        NiFixedString m_kInterpolatorID;
    };

    // Functions for getting and setting whether this is a temporary pose.
    bool GetTempPose() const;
    void SetTempPose();

    // Functions related to synchronization and timing.
    float FindCorrespondingMorphFrame(NiSequenceData* pkSourceSeqData,
        float fSourceSeqDataTime);
    float ComputeScaledTime(float fTime);
    bool CanSyncTo(NiSequenceData* pkTargetSeqData);
    bool VerifyMatchingMorphKeys(NiControllerSequence* pkTimeSyncSeq);

    // Accumulation functions.
    void SetAccumRootName(const NiFixedString& kAccumRootName);
    const NiFixedString& GetAccumRootName() const;
    void SetAccumTranslation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ);
    void GetAccumTranslation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const;
    void SetAccumRotation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ);
    void GetAccumRotation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const;
    void SetAccumFrontFacing(const unsigned char ucAxis,
        const bool bIsNegative);
    void GetAccumFrontFacing(unsigned char& ucAxis, bool& bIsNegative) const;

    // This function is only called by the tools to post-process 
    // evaluators once they've been attached to a sequence.
    void ReplaceEvaluatorAt(NiEvaluator* pkEvaluator, 
        unsigned int uiIndex);

    // Returns the total scratch pad buffer size as well as the 
    // number of unique b-spline basis data objects.
    enum
    {
        MAXUNIQUEBASISDATAS = 16,
    };
    unsigned int GetScratchPadBufferSize(bool bCumulativeAnimations, 
        NiAVObjectPalette* pkPalette, unsigned int& uiNumUniqueBasisDatas) 
        const;

    // Cloning support.
    virtual void ProcessClone(NiCloningProcess& kCloning);
    virtual void PostLinkObject(NiStream& kStream);
    // *** end Emergent internal use only ***

protected:
    // For use with cloning and streaming only.
    NiSequenceData();

    // Internal storage structures.
    class ActivationCallbackItem : public NiMemObject
    {
    public:
        ActivationCallbackItem();

        void ClearValues();

        ActivationCallback* m_pkCallback;
        NiControllerManager* m_pkManager;   // NULL indicates all managers
    };

    // Compatibility with older versions of saved NiControllerSequence objects.
    void LoadPreSequenceDataBinary(NiStream& kStream);

    bool ReallocateArrays();
    void SetCallbackArraySize(unsigned int uiNewArraySize);

    bool VerifyMatchingMorphKeys(NiTextKeyExtraData* pkTimeSyncTextKeys);

    // The name of this sequence.
    NiFixedString m_kName;

    // Evaluator array.
    unsigned int m_uiMaxEvaluators;
    unsigned int m_uiNumEvaluators;
    NiEvaluatorPtr* m_pspEvaluatorArray;

    // Text keys which point to times in the animation
    NiTextKeyExtraDataPtr m_spTextKeys;

    // Timing variables.
    float m_fDuration;
    NiTimeController::CycleType m_eCycleType;
    float m_fFrequency;

    // Callback objects
    unsigned int m_uiCallbackArraySize;
    unsigned int m_uiNumCallbacks;
    ActivationCallbackItem* m_pkDefaultActivationCallbacks;

    // Variables used for animation accumulation.
    NiFixedString m_kAccumRootName;
    enum
    {
        // Translations that are accumulated
        ACCUM_X_TRANS   = 0x0001,
        ACCUM_Y_TRANS   = 0x0002,
        ACCUM_Z_TRANS   = 0x0004,
        
        // Rotation that is accumulated. Either one, none or all
        // of these may be specified.
        ACCUM_X_ROT     = 0x0008,
        ACCUM_Y_ROT     = 0x0010,
        ACCUM_Z_ROT     = 0x0020,

        // Front facing axis. Default is positive X.
        ACCUM_X_FRONT   = 0x0040,
        ACCUM_Y_FRONT   = 0x0080,
        ACCUM_Z_FRONT   = 0x0100,
        ACCUM_NEG_FRONT = 0x0200,

        // Additional flags.

        // Stores a temporary pose. Automatically removed on completion.
        FLAG_TEMP_POSE = 0x4000, 
    };
    NiDeclareFlags(unsigned int);

    // NiStringPalettes were used to allocate strings
    // for prior versions of the engine. This member
    // is here purely for successful streaming of old sequences
    // and will only be non-NULL if in streaming code.
    // Version begin use: 10, 1, 0, 113
    // Version end use:   20, 1, 0, 1
    NiStringPalettePtr m_spDeprecatedStringPalette;

    // Temporary data to convert interpolators to evaluators during streaming.
    class StreamedInterpArray : public NiMemObject
    {
    public:
        unsigned int m_uiArraySize;
        NiInterpolatorPtr* m_pspInterpArray;
        IDTag* m_pkIDTagArray;
    };
    StreamedInterpArray* m_pkStreamedInterpArray;
};

#include "NiSequenceData.inl"

#endif  // #ifndef NISEQUENCEDATA_H
