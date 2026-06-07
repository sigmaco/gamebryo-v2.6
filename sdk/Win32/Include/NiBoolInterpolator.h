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

#ifndef NIBOOLINTERPOLATOR_H
#define NIBOOLINTERPOLATOR_H

#include "NiKeyBasedInterpolator.h"
#include "NiBoolData.h"

class NIANIMATION_ENTRY NiBoolInterpolator : public NiKeyBasedInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBoolInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBoolInterpolator();
    NiBoolInterpolator(NiBoolData* pkBoolData);
    NiBoolInterpolator(bool bPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        bool& bValue);
    virtual bool IsBoolValueSupported() const;

    inline NiBoolData* GetBoolData() const;
    inline void SetBoolData(NiBoolData* pkBoolData);
    void SetPoseValue(bool bPoseValue);

    inline NiBoolKey* GetKeys(unsigned int& uiNumKeys,
        NiBoolKey::KeyType& eType, unsigned char& ucSize)
        const;
    void ReplaceKeys(NiBoolKey* pkKeys, unsigned int uiNumKeys, 
        NiBoolKey::KeyType eType);
    void SetKeys(NiBoolKey* pkKeys, unsigned int uiNumKeys, 
        NiBoolKey::KeyType eType);

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
    virtual void Collapse();

    // *** begin Emergent internal use only ***
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiInterpolator* GetSequenceInterpolator(float fStartTime,
        float fEndTime);
    // *** end Emergent internal use only ***

protected:
    NiBool m_bBoolValue;

    NiBoolDataPtr m_spBoolData;
    unsigned int m_uiLastIdx;
};

NiSmartPointer(NiBoolInterpolator);

#include "NiBoolInterpolator.inl"

#endif  // #ifndef NIFLOATINTERPOLATOR_H
