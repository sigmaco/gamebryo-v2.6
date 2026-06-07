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

#ifndef NIPSYSBOUNDUPDATEMODIFIER_H
#define NIPSYSBOUNDUPDATEMODIFIER_H

#include "NiPSysModifier.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSBoundUpdater.
*/
class NIPARTICLE_ENTRY NiPSysBoundUpdateModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    short GetUpdateSkip() const;
    void SetUpdateSkip(short sUpdateSkip);
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysBoundUpdateModifier();

    short m_sUpdateSkip;
};

NiSmartPointer(NiPSysBoundUpdateModifier);

/// @endcond

#include "NiPSysBoundUpdateModifier.inl"

#endif  // #ifndef NIPSYSBOUNDUPDATEMODIFIER_H
