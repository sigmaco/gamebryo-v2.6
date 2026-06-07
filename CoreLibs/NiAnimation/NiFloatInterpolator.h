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

#ifndef NIFLOATINTERPOLATOR_H
#define NIFLOATINTERPOLATOR_H

#include "NiKeyBasedInterpolator.h"
#include "NiFloatData.h"

class NIANIMATION_ENTRY NiFloatInterpolator : public NiKeyBasedInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiFloatInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiFloatInterpolator();
    NiFloatInterpolator(NiFloatData* pkFloatData);
    NiFloatInterpolator(float fPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        float& fValue);
    virtual bool IsFloatValueSupported() const;

    inline NiFloatData* GetFloatData() const;
    inline void SetFloatData(NiFloatData* pkFloatData);
    void SetPoseValue(float fValue);

    inline NiFloatKey* GetKeys(unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType,
        unsigned char& ucSize) const;
    
    // For swapping animation sequences at runtime. The object 
    //   assumes ownership of the data array being passed in, and has the 
    //   responsibility for deleting it when finished with it.
    inline void ReplaceKeys(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // For swapping rotation animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    inline void SetKeys (NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    // Implemented from NiKeyBasedInterpolator
    virtual unsigned short GetKeyChannelCount() const ;
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
    float m_fFloatValue;

    NiFloatDataPtr m_spFloatData;
    unsigned int m_uiLastIdx;
};

NiSmartPointer(NiFloatInterpolator);

#include "NiFloatInterpolator.inl"

#endif  // #ifndef NIFLOATINTERPOLATOR_H
