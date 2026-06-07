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
inline NiStepRotKey::NiStepRotKey()
{
}
//---------------------------------------------------------------------------
inline NiStepRotKey::NiStepRotKey(float fTime, const NiQuaternion& quat)
    :
    NiRotKey(fTime,quat)
{
}
//---------------------------------------------------------------------------
inline NiStepRotKey::NiStepRotKey(float fTime, float fAngle,
    const NiPoint3& axis)
    :
    NiRotKey(fTime,fAngle,axis)
{
}
//---------------------------------------------------------------------------
inline bool NiStepRotKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    return NiRotKey::Equal(key0,key1);
}
//---------------------------------------------------------------------------
