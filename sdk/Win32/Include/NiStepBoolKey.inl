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
inline NiStepBoolKey::NiStepBoolKey()
{
}
//---------------------------------------------------------------------------
inline NiStepBoolKey::NiStepBoolKey(float fTime, const bool bBool)
    :
    NiBoolKey(fTime,bBool)
{
}
//---------------------------------------------------------------------------
inline bool NiStepBoolKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    return NiBoolKey::Equal(key0,key1);
}
//---------------------------------------------------------------------------
inline void NiStepBoolKey::FillDerivedVals(NiAnimationKey*, unsigned int, 
    unsigned char)
{
    // no derived values to fill in
}
//---------------------------------------------------------------------------
