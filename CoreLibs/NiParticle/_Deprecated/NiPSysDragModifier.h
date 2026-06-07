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

#ifndef NIPSYSDRAGMODIFIER_H
#define NIPSYSDRAGMODIFIER_H

#include "NiPSysModifier.h"
#include <NiPoint3.h>

class NiAVObject;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSDragForce.
*/
class NIPARTICLE_ENTRY NiPSysDragModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    NiAVObject* GetDragObj() const;
    const NiPoint3& GetDragAxis() const;
    float GetPercentage() const;
    float GetRange() const;
    float GetRangeFalloff() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysDragModifier();

    NiAVObject* m_pkDragObj;
    NiPoint3 m_kDragAxis;
    float m_fPercentage;  
    float m_fRange;
    float m_fFalloff;
};

NiSmartPointer(NiPSysDragModifier);

/// @endcond

#include "NiPSysDragModifier.inl"

#endif  // #ifndef NIPSYSDRAGMODIFIER_H
