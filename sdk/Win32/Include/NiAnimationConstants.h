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

#ifndef NIANIMATIONCONSTANTS_H
#define NIANIMATIONCONSTANTS_H

#include "NiAnimationLibType.h"
#include <NiStream.h>
#include <NiFixedString.h>


class NIANIMATION_ENTRY NiAnimationConstants
{
public:
    static const NiFixedString& GetFlipCtlrType();
    static const NiFixedString& GetGeomMorpherCtlrType();
    static const NiFixedString& GetMorphWeightsCtlrType();
    static const NiFixedString& GetPSEmitParticlesCtlrType();
    static const NiFixedString& GetTransformCtlrType();

    static const NiFixedString& GetBirthRateInterpID();
    static const NiFixedString& GetEmitterActiveInterpID();

    static const NiFixedString& GetNonAccumSuffix();
    static int GetNonAccumSuffixLength();

    static const NiFixedString& GetStartTextKey();
    static const NiFixedString& GetEndTextKey();
    static const NiFixedString& GetMorphTextKey();
    static int GetMorphTextKeyLength();

    // *** begin Emergent internal use only ***
    static unsigned int GetPoseVersion();
    static unsigned int GetScratchPadVersion();

    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    static NiFixedString ms_kFlipCtlrType;
    static NiFixedString ms_kGeomMorpherCtlrType;
    static NiFixedString ms_kMorphWeightsCtlrType;
    static NiFixedString ms_kPSEmitParticlesCtlrType;
    static NiFixedString ms_kTransformCtlrType;

    static NiFixedString ms_kBirthRateInterpID;
    static NiFixedString ms_kEmitterActiveInterpID;

    static NiFixedString ms_kNonAccumSuffix;
    static int ms_iNonAccumSuffixLength;

    static NiFixedString ms_kStartTextKey;
    static NiFixedString ms_kEndTextKey;
    static NiFixedString ms_kMorphTextKey;
    static int ms_iMorphTextKeyLength;
};

#include "NiAnimationConstants.inl"

#endif //#ifndef NIANIMATIONCONSTANTS_H
