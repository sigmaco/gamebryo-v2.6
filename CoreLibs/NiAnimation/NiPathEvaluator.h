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

#ifndef NIPATHEVALUATOR_H
#define NIPATHEVALUATOR_H

#include <NiFlags.h>
#include "NiKeyBasedEvaluator.h"
#include "NiPosData.h"
#include "NiFloatData.h"
#include "NiScratchPad.h"

NiSmartPointer(NiPathEvaluator);

class NIANIMATION_ENTRY NiPathEvaluator : public NiKeyBasedEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiPathEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    enum KeyChannel
    {
        PATH = 0,
        PERCENTAGE
    };

    enum EvalChannel
    {
        POSITION = EVALPOSINDEX,
        ROTATION = EVALROTINDEX,
        SCALE = EVALSCALEINDEX // SCALE channel is never valid.
    };

    // *** begin Emergent internal use only ***
    static float PathLength(float fNorm, const NiPosKey* pkPosKey1, 
        const NiPosKey* pkPosKey2, NiPosKey::InterpFunction interpD1);
    static void CalcRefFrame(float fPct, float fNorm, 
        const NiScratchPad::PathFillData* pkPathFill, NiMatrix3& kRefFrame);
    // *** end Emergent internal use only ***

    NiPathEvaluator();
    NiPathEvaluator(NiPosData* pkPathData, NiFloatData* pkPctData);
    virtual ~NiPathEvaluator();

    // Key data access.
    void SetPathData(NiPosData* pkPathData);
    NiPosData* GetPathData() const;
    void SetPctData(NiFloatData* pkPctData);
    NiFloatData* GetPctData() const;

    // Path data access.
    NiPosKey* GetPathKeys(unsigned int& uiNumKeys, NiPosKey::KeyType& eType,
        unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The NiPosData object 
    // assumes ownership of the data array being passed in, and has the 
    // responsibility for deleting it when finished with it. The path keys
    // must have a time range starting at 0.0 and ending at 1.0.
    void ReplacePathKeys(NiPosKey* pkPathKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType ePathType);

    // Percent data access.
    NiFloatKey* GetPctKeys(unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The NiFloatData object 
    // assumes ownership of the data array being passed in, and has the 
    // responsibility for deleting it when finished with it.
    void ReplacePctKeys(NiFloatKey* pkPctKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // Control option data member access methods.
    void SetAllowFlip(bool bAllowFlip);
    bool GetAllowFlip() const;

    void SetBank(bool bBank);
    bool GetBank() const;

    enum BankDir { NEGATIVE = -1, POSITIVE = 1 };
    void SetBankDir(BankDir eDir);
    BankDir GetBankDir() const;

    void SetConstVelocity(bool bConstVelocity);
    bool GetConstVelocity() const;

    void SetFollow(bool bFollow);
    bool GetFollow() const;

    void SetMaxBankAngle(float fAngle);
    float GetMaxBankAngle() const;

    void SetSmoothing(float fSmooth);
    float GetSmoothing() const;

    void SetFollowAxis(short sAxis);
    short GetFollowAxis() const;

    void SetFlip(bool bFlip);
    bool GetFlip() const;

    bool GetCurveTypeOpen() const;

    float GetTotalPathLength() const;

    // Implemented from NiKeyBasedEvaluator.
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(unsigned short usChannel)
        const;
    virtual NiAnimationKey::KeyContent GetKeyContent(
        unsigned short usChannel) const;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const;

    // *** begin Emergent internal use only ***

    // Virtual function overrides from NiEvaluator.
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

    static bool PathFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);
    virtual NiEvaluator* GetSequenceEvaluator(float fStartTime,
        float fEndTime);
    virtual void ProcessClone(NiCloningProcess& kCloning);

    void SetCurveTypeOpen(bool bOpen);

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();

    // Flags.
    enum
    {
        CVDATAUPDATE_MASK   = 0x0001,
        OPENCURVE_MASK      = 0x0002,
        ALLOWFLIP_MASK      = 0x0004,
        BANK_MASK           = 0x0008,
        CONSTVELOCITY_MASK  = 0x0010,
        FOLLOW_MASK         = 0x0020,
        FLIP_MASK           = 0x0040
    };
    
    // Constant velocity related routines.
    void SetConstantVelocityData();
    float PathSpeed(int iI0, int iI1, float fTime) const;
    float PathLength(int iI0, int iI1, float fTime) const;

    void SetCVDataNeedsUpdate(bool bUpdate);
    bool GetCVDataNeedsUpdate() const;
 
    NiPosDataPtr m_spPathData;
    NiFloatDataPtr m_spPctData;

    // Reparameterization by arc length.
    mutable float* m_pfPartialLength;
    mutable float m_fTotalLength;

    float m_fMaxBankAngle;
    float m_fSmoothing;
    short m_sFollowAxis;
    float m_fMaxCurvature;
    BankDir m_eBankDir;
};

#include "NiPathEvaluator.inl"

#endif  // #ifndef NIPATHEVALUATOR_H
