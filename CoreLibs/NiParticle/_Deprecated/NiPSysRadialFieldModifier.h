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

#ifndef NIPSYSRADIALFIELDMODIFIER_H
#define NIPSYSRADIALFIELDMODIFIER_H

#include "NiPSysFieldModifier.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSRadialFieldForce.
*/
class NIPARTICLE_ENTRY NiPSysRadialFieldModifier : public NiPSysFieldModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    float GetRadialType() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysRadialFieldModifier();

    float m_fRadialType;
};

NiSmartPointer(NiPSysRadialFieldModifier);

/// @endcond

#include "NiPSysRadialFieldModifier.inl"

#endif  // #ifndef NIPSYSRADIALFIELDMODIFIER_H
