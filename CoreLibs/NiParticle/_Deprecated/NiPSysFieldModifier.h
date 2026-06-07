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

#ifndef NIPSYSFIELDMODIFIER_H
#define NIPSYSFIELDMODIFIER_H

#include "NiPSysModifier.h"

class NiAVObject;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSFieldForce.
*/
class NIPARTICLE_ENTRY NiPSysFieldModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    NiAVObject* GetFieldObj() const;
    float GetMagnitude() const;
    float GetAttenuation() const;
    bool GetUseMaxDistance() const;
    float GetMaxDistance() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysFieldModifier();

    NiAVObject* m_pkFieldObj;
    float m_fMagnitude;
    float m_fAttenuation;
    bool m_bUseMaxDistance;
    float m_fMaxDistance;
};

NiSmartPointer(NiPSysFieldModifier);

/// @endcond

#include "NiPSysFieldModifier.inl"

#endif  // #ifndef NIPSYSFIELDMODIFIER_H
