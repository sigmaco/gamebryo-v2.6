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

#ifndef NITRANSFORMEVALUATOR_H
#define NITRANSFORMEVALUATOR_H

#include "NiKeyBasedEvaluator.h"
#include "NiQuatTransform.h"
#include "NiTransformData.h"

class NIANIMATION_ENTRY NiTransformEvaluator : public NiKeyBasedEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiTransformEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        POSITION = EVALPOSINDEX,
        ROTATION = EVALROTINDEX,
        SCALE = EVALSCALEINDEX
    };

    NiTransformEvaluator();
    NiTransformEvaluator(NiTransformData* pkData);
    NiTransformEvaluator(const NiQuatTransform& kPoseValue);

    // Data access.
    NiTransformData* GetTransformData() const;
    void SetTransformData(NiTransformData* pkData);

    // Translation keys.
    unsigned int GetNumPosKeys() const;
    NiPosKey* GetPosData(unsigned int& uiNumKeys, NiPosKey::KeyType& eType, 
        unsigned char& ucSize) const;
    void ReplacePosData(NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);
    void SetPoseTranslate(const NiPoint3& kTranslate);

    // Rotation keys.
    unsigned int GetNumRotKeys() const;
    NiRotKey* GetRotData(unsigned int& uiNumKeys, NiRotKey::KeyType& eType,
        unsigned char& ucSize) const;
    void ReplaceRotData(NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);
    void SetPoseRotate(const NiQuaternion& kRotate);

    // Scale keys.
    unsigned int GetNumScaleKeys() const;
    NiFloatKey* GetScaleData(unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType, unsigned char& ucSize) const;
    void ReplaceScaleData(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);
    void SetPoseScale(float fScale);

    void SetPoseValue(const NiQuatTransform& kPoseValue);

    // Implemented from NiKeyBasedEvaluator
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(unsigned short usChannel)
        const;
    virtual NiAnimationKey::KeyContent GetKeyContent(
        unsigned short usChannel) const;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const;

    // Virtual function overrides from NiEvaluator.
    virtual void Collapse();

    // *** begin Emergent internal use only ***

    virtual bool GetChannelPosedValue(unsigned int uiChannel, 
        void* pvResult) const;
    virtual bool UpdateChannel(float fTime, unsigned int uiChannel,
        NiEvaluatorSPData* pkEvalSPData, void* pvResult) const;
    virtual bool GetChannelScratchPadInfo(unsigned int uiChannel, 
        bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
        unsigned int& uiFillSize, bool& bSharedFillData, 
        NiScratchPadBlock& eSPBSegmentData, 
        NiBSplineBasisData*& pkBasisData) const;
    virtual bool InitChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
        bool bInitSharedData, NiAVObjectPalette* pkPalette, 
        NiPoseBufferHandle kPBHandle) const;

    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();

    NiQuatTransform m_kTransformValue;

    NiTransformDataPtr m_spData;
};

NiSmartPointer(NiTransformEvaluator);

#include "NiTransformEvaluator.inl"

#endif  // #ifndef NITRANSFORMEVALUATOR_H
