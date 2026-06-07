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

#ifndef NICOLORINTERPOLATOR_H
#define NICOLORINTERPOLATOR_H

#include "NiKeyBasedInterpolator.h"
#include "NiColorData.h"

class NIANIMATION_ENTRY NiColorInterpolator : public NiKeyBasedInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiColorInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiColorInterpolator();
    NiColorInterpolator(NiColorData* pkColorData);
    NiColorInterpolator(const NiColorA& kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiColorA& kValue);
    virtual bool IsColorAValueSupported() const;

    inline NiColorData* GetColorData() const;
    inline void SetColorData(NiColorData* pkColorData);
    void SetPoseValue(const NiColorA& kPoseValue);

    inline NiColorKey* GetKeys(unsigned int& uiNumKeys,
        NiColorKey::KeyType& eType,
        unsigned char& ucSize) const;
    inline void ReplaceKeys(NiColorKey* pkKeys, unsigned int uiNumKeys,
        NiColorKey::KeyType eType);
    inline void SetKeys(NiColorKey* pkKeys, unsigned int uiNumKeys,
        NiColorKey::KeyType eType);

    // Implemented from 
    virtual unsigned short GetKeyChannelCount() const;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const;
    virtual NiAnimationKey::KeyType GetKeyType(
        unsigned short usChannel) const;
    virtual NiAnimationKey::KeyContent GetKeyContent(
        unsigned short usChannel) const;
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
    NiColorA m_kColorValue;

    NiColorDataPtr m_spColorData;
    unsigned int m_uiLastIdx;
};

NiSmartPointer(NiColorInterpolator);

#include "NiColorInterpolator.inl"

#endif  // #ifndef NICOLORINTERPOLATOR_H
