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

#ifndef NICOLOREVALUATOR_H
#define NICOLOREVALUATOR_H

#include "NiKeyBasedEvaluator.h"
#include "NiColorData.h"

class NIANIMATION_ENTRY NiColorEvaluator : public NiKeyBasedEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiColorEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        COLORA = EVALBASEINDEX
    };

    NiColorEvaluator();
    NiColorEvaluator(NiColorData* pkColorData);

    NiColorData* GetColorData() const;
    void SetColorData(NiColorData* pkColorData);

    NiColorKey* GetKeys(unsigned int& uiNumKeys, NiColorKey::KeyType& eType,
        unsigned char& ucSize) const;
    void ReplaceKeys(NiColorKey* pkKeys, unsigned int uiNumKeys,
        NiColorKey::KeyType eType);
    void SetKeys(NiColorKey* pkKeys, unsigned int uiNumKeys,
        NiColorKey::KeyType eType);

    // Implemented from NiKeyBasedEvaluator.
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(
        unsigned short usChannel) const;
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

    static bool LinearColorFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();

    NiColorDataPtr m_spColorData;
};

NiSmartPointer(NiColorEvaluator);

#include "NiColorEvaluator.inl"

#endif  // #ifndef NICOLOREVALUATOR_H
