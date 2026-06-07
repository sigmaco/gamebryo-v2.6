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

#ifndef NIPSYSMESHUPDATEMODIFIER_H
#define NIPSYSMESHUPDATEMODIFIER_H

#include "NiPSysModifier.h"
#include <NiAVObject.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSMeshParticleSystem.
*/
class NIPARTICLE_ENTRY NiPSysMeshUpdateModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    NiAVObject* GetMeshAt(unsigned int uiIndex) const;
    unsigned int GetMeshCount() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysMeshUpdateModifier();

    NiTObjectArray<NiAVObjectPtr> m_kMeshes;
};

NiSmartPointer(NiPSysMeshUpdateModifier);

/// @endcond

#include "NiPSysMeshUpdateModifier.inl"

#endif  // #ifndef NIPSYSMESHUPDATEMODIFIER_H
