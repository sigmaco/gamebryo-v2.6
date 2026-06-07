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

#ifndef NIQUATERNIONEVALUATOR_H
#define NIQUATERNIONEVALUATOR_H

#include "NiKeyBasedEvaluator.h"
#include "NiRotData.h"

class NIANIMATION_ENTRY NiQuaternionEvaluator : public NiKeyBasedEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiQuaternionEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        ROTATION = EVALBASEINDEX
    };

    NiQuaternionEvaluator();
    NiQuaternionEvaluator(NiRotData* pkRotData);

    NiRotData* GetQuaternionData() const;
    void SetQuaternionData(NiRotData* pkRotData);

    NiRotKey* GetKeys(unsigned int& uiNumKeys, NiRotKey::KeyType& eType,
        unsigned char& ucSize) const;
    void ReplaceKeys(NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);
    void SetKeys(NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);

    // Implemented from NiKeyBasedEvaluator
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(
        unsigned short usChannel) const;
    virtual NiAnimationKey::KeyContent GetKeyContent(unsigned short usChannel)
        const;
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

    static bool SlerpRotFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    static bool SquadRotFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    static bool EulerRotFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();

    NiRotDataPtr m_spQuaternionData;
};

NiSmartPointer(NiQuaternionEvaluator);

#include "NiQuaternionEvaluator.inl"

#endif  // #ifndef NIQUATERNIONEVALUATOR_H
