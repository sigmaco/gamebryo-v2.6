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

//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetFlipCtlrType()
{
    return ms_kFlipCtlrType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetGeomMorpherCtlrType()
{
    return ms_kGeomMorpherCtlrType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetMorphWeightsCtlrType()
{
    return ms_kMorphWeightsCtlrType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetPSEmitParticlesCtlrType()
{
    return ms_kPSEmitParticlesCtlrType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetTransformCtlrType()
{
    return ms_kTransformCtlrType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetBirthRateInterpID()
{
    return ms_kBirthRateInterpID;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetEmitterActiveInterpID()
{
    return ms_kEmitterActiveInterpID;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetNonAccumSuffix()
{
    return ms_kNonAccumSuffix;
}
//---------------------------------------------------------------------------
inline int NiAnimationConstants::GetNonAccumSuffixLength()
{
    return ms_iNonAccumSuffixLength;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetStartTextKey()
{
    return ms_kStartTextKey;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetEndTextKey()
{
    return ms_kEndTextKey;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAnimationConstants::GetMorphTextKey()
{
    return ms_kMorphTextKey;
}
//---------------------------------------------------------------------------
inline int NiAnimationConstants::GetMorphTextKeyLength()
{
    return ms_iMorphTextKeyLength;
}
//---------------------------------------------------------------------------
inline unsigned int NiAnimationConstants::GetPoseVersion()
{
    return NiStream::GetVersion(20, 5, 0, 1);
}
//---------------------------------------------------------------------------
inline unsigned int NiAnimationConstants::GetScratchPadVersion()
{
    return NiStream::GetVersion(20, 5, 0, 2);
}
//---------------------------------------------------------------------------
