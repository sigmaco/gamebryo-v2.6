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

#ifndef NIPOSEBUFFERHANDLE_H
#define NIPOSEBUFFERHANDLE_H

#include "NiAnimationLibType.h"
#include "NiStream.h"
#include "NiPoseBufferChannelType.h"

class NIANIMATION_ENTRY NiPoseBufferHandle : public NiMemObject
{
public:
    // Constructors.
    NiPoseBufferHandle(NiPoseBufferChannelType ePBChannelType, 
        unsigned short usChannelIndex);
    NiPoseBufferHandle();

    // Functions which initialize the handle.
    void Init(NiPoseBufferChannelType ePBChannelType, 
        unsigned short usChannelIndex);
    void Invalidate();

    // Functions to obtain the handle data.
    bool IsValid() const;
    NiPoseBufferChannelType GetChannelType() const;
    unsigned short GetChannelIndex() const;

    // Comparison operators.
    bool operator== (const NiPoseBufferHandle& kOther) const;
    bool operator!= (const NiPoseBufferHandle& kOther) const;

    // *** begin Emergent internal use only ***
    NiPoseBufferChannelType GetChannelTypeNoValidation() const;

    // Streaming functions.
    void LoadBinary(NiStream& stream);
    void SaveBinary(NiStream& stream) const;
    // *** end Emergent internal use only ***

protected:
    unsigned short m_usPBChannelType;
    unsigned short m_usChannelIndex;
};

#include "NiPoseBufferHandle.inl"

#endif  // #ifndef NIPOSEBUFFERHANDLE_H
