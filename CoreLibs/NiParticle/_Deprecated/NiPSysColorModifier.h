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

#ifndef NIPSYSCOLORMODIFIER_H
#define NIPSYSCOLORMODIFIER_H

#include "NiPSysModifier.h"
#include <NiColorData.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSSimulatorGeneralStep.
*/
class NIPARTICLE_ENTRY NiPSysColorModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    NiColorData* GetColorData() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysColorModifier();

    NiColorDataPtr m_spColorData;
};

NiSmartPointer(NiPSysColorModifier);

/// @endcond

#include "NiPSysColorModifier.inl"

#endif  // #ifndef NIPSYSCOLORMODIFIER_H
