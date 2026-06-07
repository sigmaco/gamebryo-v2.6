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

#include "NiAnimationPCH.h"
#include "NiAnimationConstants.h"

NiFixedString NiAnimationConstants::ms_kFlipCtlrType;
NiFixedString NiAnimationConstants::ms_kGeomMorpherCtlrType;
NiFixedString NiAnimationConstants::ms_kMorphWeightsCtlrType;
NiFixedString NiAnimationConstants::ms_kPSEmitParticlesCtlrType;
NiFixedString NiAnimationConstants::ms_kTransformCtlrType;

NiFixedString NiAnimationConstants::ms_kBirthRateInterpID;
NiFixedString NiAnimationConstants::ms_kEmitterActiveInterpID;

NiFixedString NiAnimationConstants::ms_kNonAccumSuffix;
int NiAnimationConstants::ms_iNonAccumSuffixLength;

NiFixedString NiAnimationConstants::ms_kStartTextKey;
NiFixedString NiAnimationConstants::ms_kEndTextKey;
NiFixedString NiAnimationConstants::ms_kMorphTextKey;
int NiAnimationConstants::ms_iMorphTextKeyLength;


//---------------------------------------------------------------------------
void NiAnimationConstants::_SDMInit()
{
    ms_kFlipCtlrType = "NiFlipController";
    ms_kGeomMorpherCtlrType = "NiGeomMorpherController";
    ms_kMorphWeightsCtlrType = "NiMorphWeightsController";
    ms_kPSEmitParticlesCtlrType = "NiPSEmitParticlesCtlr";
    ms_kTransformCtlrType = "NiTransformController";

    ms_kBirthRateInterpID = "BirthRate";
    ms_kEmitterActiveInterpID = "EmitterActive";

    ms_kNonAccumSuffix = " NonAccum";
    ms_iNonAccumSuffixLength = (int)strlen(ms_kNonAccumSuffix);

    ms_kStartTextKey = "start";
    ms_kEndTextKey = "end";
    ms_kMorphTextKey = "morph:";
    ms_iMorphTextKeyLength = (int)strlen(ms_kMorphTextKey);
}
//---------------------------------------------------------------------------
void NiAnimationConstants::_SDMShutdown()
{
    ms_kFlipCtlrType = NULL;
    ms_kGeomMorpherCtlrType = NULL;
    ms_kMorphWeightsCtlrType = NULL;
    ms_kPSEmitParticlesCtlrType = NULL;
    ms_kTransformCtlrType = NULL;

    ms_kBirthRateInterpID = NULL;
    ms_kEmitterActiveInterpID = NULL;

    ms_kNonAccumSuffix = NULL;
    ms_iNonAccumSuffixLength = 0;

    ms_kStartTextKey = NULL;
    ms_kEndTextKey = NULL;
    ms_kMorphTextKey = NULL;
    ms_iMorphTextKeyLength = 0;
}
