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

#ifndef NIPSYSPLANARCOLLIDER_H
#define NIPSYSPLANARCOLLIDER_H

#include "NiPSysCollider.h"
#include <NiPoint3.h>

class NiAVObject;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSPlanarCollider.
*/
class NIPARTICLE_ENTRY NiPSysPlanarCollider : public NiPSysCollider
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // ** begin Emergent internal use only ***
    NiAVObject* GetColliderObj() const;
    float GetWidth() const;
    float GetHeight() const;
    const NiPoint3& GetXAxis() const;
    const NiPoint3& GetYAxis() const;
    // ** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysPlanarCollider();

    NiAVObject* m_pkColliderObj;
    float m_fWidth;
    float m_fHeight;
    NiPoint3 m_kXAxis;
    NiPoint3 m_kYAxis;
};

NiSmartPointer(NiPSysPlanarCollider);

/// @endcond

#include "NiPSysPlanarCollider.inl"

#endif  // #ifndef NIPSYSPLANARCOLLIDER_H
