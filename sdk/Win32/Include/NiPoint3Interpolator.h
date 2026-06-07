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

#ifndef NIPOINT3INTERPOLATOR_H
#define NIPOINT3INTERPOLATOR_H

#include "NiKeyBasedInterpolator.h"
#include "NiPosData.h"

class NIANIMATION_ENTRY NiPoint3Interpolator : public NiKeyBasedInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiPoint3Interpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiPoint3Interpolator();
    NiPoint3Interpolator(NiPosData* pkPosData);
    NiPoint3Interpolator(const NiPoint3& kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiPoint3& kValue);
    virtual bool IsPoint3ValueSupported() const;

    inline NiPosData* GetPoint3Data() const;
    inline void SetPoint3Data(NiPosData* pkPosData);
    void SetPoseValue(const NiPoint3& kPoseValue);

    inline NiPosKey* GetKeys(unsigned int& uiNumKeys, NiPosKey::KeyType& eType,
        unsigned char& ucSize) const;
    inline void ReplaceKeys(NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);
    inline void SetKeys(NiPosKey* pkKeys, unsigned int uiNumKeys,
        NiPosKey::KeyType eType);

    // Implemented from NiKeyBasedInterpolator
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(unsigned short usChannel)
        const;
    virtual NiAnimationKey::KeyContent GetKeyContent(
        unsigned short usChannel ) const;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const;

    virtual bool GetChannelPosed(unsigned short usChannel) const;
    virtual void Collapse();

    // *** begin Emergent internal use only ***
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiInterpolator* GetSequenceInterpolator(float fStartTime,
        float fEndTime);
    // *** end Emergent internal use only ***

protected:
    NiPoint3 m_kPoint3Value;

    NiPosDataPtr m_spPoint3Data;
    unsigned int m_uiLastIdx;
};

NiSmartPointer(NiPoint3Interpolator);

#include "NiPoint3Interpolator.inl"

#endif  // #ifndef NIPOINT3INTERPOLATOR_H
