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

#ifndef NILOOKATEVALUATOR_H
#define NILOOKATEVALUATOR_H

#include <NiFlags.h>
#include "NiEvaluator.h"

class NIANIMATION_ENTRY NiLookAtEvaluator : public NiEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiLookAtEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    enum Axis
    {
        X,
        Y,
        Z
    };

    enum Channel
    {
        POSITION = EVALPOSINDEX,
        ROTATION = EVALROTINDEX,
        SCALE = EVALSCALEINDEX
    };

    NiLookAtEvaluator();
    NiLookAtEvaluator(const NiFixedString& kLookAtObjectName, 
        const NiFixedString& kDrivenObjectName, Axis eAxis = X, 
        bool bFlip = false);
    ~NiLookAtEvaluator();

    const NiFixedString& GetLookAtObjectName() const;
    void SetLookAtObjectName(const NiFixedString& kName);

    const NiFixedString& GetDrivenObjectName() const;
    void SetDrivenObjectName(const NiFixedString& kName);

    bool GetFlip() const;
    void SetFlip(bool bFlip);

    Axis GetAxis() const;
    void SetAxis(Axis eAxis);

    NiEvaluator* GetTranslateEvaluator();
    void SetTranslateEvaluator(NiEvaluator* pkEval);
    
    NiEvaluator* GetRollEvaluator();
    void SetRollEvaluator(NiEvaluator* pkEval);
    
    NiEvaluator* GetScaleEvaluator();
    void SetScaleEvaluator(NiEvaluator* pkEval);

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
    virtual void ShutdownChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData) const;

    static bool LookAtFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    virtual void GetActiveTimeRange(float& fBeginKeyTime, 
        float& fEndKeyTime) const;
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();

    // Flags.
    enum
    {
        FLIP_MASK   = 0x01,
        AXIS_MASK   = 0x06,
        AXIS_POS    = 1,
        SHUTDOWNCALLED = 0x80,
    };

    // Indices to held evaluators.
    enum
    {
        TRANSLATEEVAL = 0,
        ROLLEVAL = 1,
        SCALEEVAL = 2,
        MAXEVALUATORS = 3
    };

    NiFixedString m_kLookAtObjectName;
    NiFixedString m_kDrivenObjectName;
    NiEvaluatorPtr m_aspEvaluators[MAXEVALUATORS];
};

NiSmartPointer(NiLookAtEvaluator);

#include "NiLookAtEvaluator.inl"

#endif  // #ifndef NILOOKATEVALUATOR_H
