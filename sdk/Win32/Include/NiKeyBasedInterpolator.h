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

#ifndef NIKEYBASEDINTERPOLATOR_H
#define NIKEYBASEDINTERPOLATOR_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include <NiBool.h>
#include "NiInterpolator.h"
#include "NiAnimationKey.h"

class NIANIMATION_ENTRY NiKeyBasedInterpolator : public NiInterpolator
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiKeyBasedInterpolator);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    virtual ~NiKeyBasedInterpolator();

    // Pure virtual methods
    virtual unsigned short GetKeyChannelCount() const = 0;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const = 0;
    virtual NiAnimationKey::KeyType GetKeyType(unsigned short usChannel) const
        = 0;
    virtual NiAnimationKey::KeyContent GetKeyContent(unsigned short usChannel)
        const = 0;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const = 0;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const = 0;
    
    virtual bool GetChannelPosed(unsigned short usChannel) const = 0;
    
    // Class Methods
    unsigned int GetAllocatedSize(unsigned short usChannel) const;
    NiAnimationKey* GetKeyAt(unsigned int uiWhichKey, 
        unsigned short usChannel) const;

    void FillDerivedValues(unsigned short usChannel);
    
    // Legacy accessor for all data at once
    NiAnimationKey* GetAnim(unsigned int &uiNumKeys,
        NiAnimationKey::KeyType &eType, unsigned char &ucSize,
        unsigned short usChannel = 0) const;

    // *** begin Emergent internal use only ***
    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    // *** end Emergent internal use only ***

protected:
    NiKeyBasedInterpolator();
};

NiSmartPointer(NiKeyBasedInterpolator);

#endif  // #ifndef NIINTERPOLATOR_H
