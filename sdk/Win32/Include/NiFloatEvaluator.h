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

#ifndef NIFLOATEVALUATOR_H
#define NIFLOATEVALUATOR_H

#include "NiKeyBasedEvaluator.h"
#include "NiFloatData.h"

class NIANIMATION_ENTRY NiFloatEvaluator : public NiKeyBasedEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiFloatEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        FLOAT = EVALBASEINDEX
    };

    NiFloatEvaluator();
    NiFloatEvaluator(NiFloatData* pkFloatData);

    NiFloatData* GetFloatData() const;
    void SetFloatData(NiFloatData* pkFloatData);

    NiFloatKey* GetKeys(unsigned int& uiNumKeys, NiFloatKey::KeyType& eType,
        unsigned char& ucSize) const;
    
    // For swapping animation sequences at runtime. The object 
    //   assumes ownership of the data array being passed in, and has the 
    //   responsibility for deleting it when finished with it.
    void ReplaceKeys(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    void SetKeys(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

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

    static bool CubicFloatFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();
   
    NiFloatDataPtr m_spFloatData;
};

NiSmartPointer(NiFloatEvaluator);

#include "NiFloatEvaluator.inl"

#endif  // #ifndef NIFLOATEVALUATOR_H
