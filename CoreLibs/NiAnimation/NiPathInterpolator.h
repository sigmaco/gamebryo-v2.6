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

#ifndef NIPATHINTERPOLATOR_H
#define NIPATHINTERPOLATOR_H

#include <NiAVObject.h>
#include <NiKeyBasedInterpolator.h>
#include "NiPosData.h"
#include "NiFloatData.h"

NiSmartPointer(NiPathInterpolator);

class NIANIMATION_ENTRY NiPathInterpolator : public NiKeyBasedInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiPathInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiPathInterpolator();
    NiPathInterpolator(NiPosData* pkPathData, NiFloatData* pkPctData);
    virtual ~NiPathInterpolator();

    enum Channel
    {
        PATH = 0,
        PERCENTAGE
    };

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);
    virtual bool IsTransformValueSupported() const;

    // shared data access
    inline void SetPathData(NiPosData* pkPathData);
    inline NiPosData* GetPathData() const;
    inline void SetPctData(NiFloatData* pkPctData);
    inline NiFloatData* GetPctData() const;

    // path data access
    inline NiPosKey* GetPathKeys(unsigned int& uiNumKeys,
        NiPosKey::KeyType& eType,
        unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The NiPosData object 
    // assumes ownership of the data array being passed in, and has the 
    // responsibility for deleting it when finished with it. The path keys
    // must have a time range starting at 0.0 and ending at 1.0.
    void ReplacePathKeys(NiPosKey* pkPathKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType ePathType);

    // percent data access
    inline NiFloatKey* GetPctKeys(unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping animation sequences at runtime. The NiFloatData object 
    // assumes ownership of the data array being passed in, and has the 
    // responsibility for deleting it when finished with it.
    void ReplacePctKeys(NiFloatKey* pkPctKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // control option data member access methods
    inline void SetAllowFlip(bool bAllowFlip);
    inline bool GetAllowFlip() const;

    inline void SetBank(bool bBank);
    inline bool GetBank() const;

    enum BankDir { NEGATIVE = -1, POSITIVE = 1 };
    inline void SetBankDir(BankDir eDir);
    BankDir GetBankDir() const;

    inline void SetConstVelocity(bool bConstVelocity);
    inline bool GetConstVelocity() const;

    inline void SetFollow(bool bFollow);
    inline bool GetFollow() const;

    inline void SetMaxBankAngle(float fAngle);
    inline float GetMaxBankAngle() const;

    inline void SetSmoothing(float fSmooth);
    inline float GetSmoothing() const;

    inline void SetFollowAxis(short sAxis);
    inline short GetFollowAxis() const;

    inline void SetFlip(bool bFlip);
    inline bool GetFlip() const;

    inline bool GetCurveTypeOpen() const;

    float GetTotalPathLength() const;

    // Implemented from NiKeyBasedInterpolator
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(unsigned short usChannel)
        const;
    virtual NiAnimationKey::KeyContent GetKeyContent(
        unsigned short usChannel) const;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const;

    virtual bool GetChannelPosed(unsigned short usChannel) const;

    // *** begin Emergent internal use only ***
    inline void SetCurveTypeOpen(bool bOpen);
    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);
    virtual NiInterpolator* GetSequenceInterpolator(float fStartTime,
        float fEndTime);
    virtual void ProcessClone(NiCloningProcess& kCloning);
    // *** end Emergent internal use only ***

protected:
    // flags
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

    
    NiPoint3 GenPathInterp(int iI1, int iI2, float fNormTime);
    void CalcRefFrame(unsigned int uiI1, unsigned int uiI2, float fNormTime,
        NiMatrix3& refFrame);
    void GetPercentLoc(float fPercent, unsigned int& uiI1,
        unsigned int& uiI2, float& fNormTime);
    void GetLoc(float fTime, unsigned int& uiI1, unsigned int& uiI2,
        float& fNormTime);

    // constant velocity related routines
    void SetConstantVelocityData();
    float PathSpeed(int iI0, int iI1, float fTime) const;
    float PathLength(int iI0, int iI1, float fTime) const;

    inline void SetCVDataNeedsUpdate(bool bUpdate);
    inline bool GetCVDataNeedsUpdate() const;
 
    unsigned int m_uiLastPathIdx;
    unsigned int m_uiLastPctIdx;

    NiPosDataPtr m_spPathData;
    NiFloatDataPtr m_spPctData;

    // reparameterization by arc length
    mutable float* m_pfPartialLength;
    mutable float m_fTotalLength;

    float m_fMaxBankAngle;
    float m_fSmoothing;
    short m_sFollowAxis;
    float m_fMaxCurvature;
    BankDir m_eBankDir;

    NiQuaternion m_kRotation;
    NiPoint3 m_kPosition;
    float m_fLastTime;
};

#include "NiPathInterpolator.inl"

#endif
