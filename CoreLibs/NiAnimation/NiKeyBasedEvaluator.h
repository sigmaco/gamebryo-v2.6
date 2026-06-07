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

#ifndef NIKEYBASEDEVALUATOR_H
#define NIKEYBASEDEVALUATOR_H

#include "NiAnimationLibType.h"
#include "NiEvaluator.h"
#include "NiAnimationKey.h"

class NIANIMATION_ENTRY NiKeyBasedEvaluator : public NiEvaluator
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiKeyBasedEvaluator);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    virtual ~NiKeyBasedEvaluator();

    // Pure virtual methods
    virtual unsigned short GetKeyChannelCount() const = 0;
    virtual unsigned int GetKeyCount(unsigned short usChannel) const = 0;
    virtual NiAnimationKey::KeyType GetKeyType(unsigned short usChannel) const
        = 0;
    virtual NiAnimationKey::KeyContent GetKeyContent(unsigned short usChannel)
        const = 0;
    virtual NiAnimationKey* GetKeyArray(unsigned short usChannel) const = 0;
    virtual unsigned char GetKeyStride(unsigned short usChannel) const = 0;

    // Class Methods
    unsigned int GetAllocatedSize(unsigned short usChannel) const;
    NiAnimationKey* GetKeyAt(unsigned int uiWhichKey, 
        unsigned short usChannel) const;

    // Legacy accessor for all data at once
    NiAnimationKey* GetAnim(unsigned int &uiNumKeys,
        NiAnimationKey::KeyType &eType, unsigned char &ucSize,
        unsigned short usChannel = 0) const;

    // *** begin Emergent internal use only ***
    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    // *** end Emergent internal use only ***

protected:
    NiKeyBasedEvaluator();
};

NiSmartPointer(NiKeyBasedEvaluator);

#endif  // #ifndef NIKEYBASEDEVALUATOR_H
