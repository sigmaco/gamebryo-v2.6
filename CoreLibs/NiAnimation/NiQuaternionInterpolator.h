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

#ifndef NIQUATERNIONINTERPOLATOR_H
#define NIQUATERNIONINTERPOLATOR_H

#include "NiKeyBasedInterpolator.h"
#include "NiRotData.h"

class NIANIMATION_ENTRY NiQuaternionInterpolator :
    public NiKeyBasedInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiQuaternionInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiQuaternionInterpolator();
    NiQuaternionInterpolator(NiRotData* pkRotData);
    NiQuaternionInterpolator(const NiQuaternion& kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuaternion& kValue);
    virtual bool IsQuaternionValueSupported() const;

    inline NiRotData* GetQuaternionData() const;
    inline void SetQuaternionData(NiRotData* pkRotData);
    void SetPoseValue(const NiQuaternion& kPoseValue);

    inline NiRotKey* GetKeys(unsigned int& uiNumKeys, NiRotKey::KeyType& eType,
        unsigned char& ucSize) const;
    inline void ReplaceKeys(NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);
    inline void SetKeys(NiRotKey* pkKeys, unsigned int uiNumKeys,
        NiRotKey::KeyType eType);


    // Implemented from NiKeyBasedInterpolator
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(
        unsigned short usChannel) const;
    virtual NiAnimationKey::KeyContent GetKeyContent(unsigned short usChannel)
        const;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const;

    virtual bool GetChannelPosed(unsigned short usChannel) const;
    virtual void Collapse();

    // *** begin Emergent internal use only ***
    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiInterpolator* GetSequenceInterpolator(float fStartTime,
        float fEndTime);
    // *** end Emergent internal use only ***

protected:
    NiQuaternion m_kQuaternionValue;

    NiRotDataPtr m_spQuaternionData;
    unsigned int m_uiLastIdx;
};

NiSmartPointer(NiQuaternionInterpolator);

#include "NiQuaternionInterpolator.inl"

#endif  // #ifndef NIQUATERNIONINTERPOLATOR_H
