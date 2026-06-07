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

#include "NiRTLib.h"

//---------------------------------------------------------------------------
inline NiBoolKey::NiBoolKey()
{
}
//---------------------------------------------------------------------------
inline NiBoolKey::NiBoolKey(float fTime, const bool b)
    :
    NiAnimationKey(fTime)
{
    m_bBool = b;
}

//---------------------------------------------------------------------------
inline void NiBoolKey::SetBool(const bool b)
{
    m_bBool = b;
}
//---------------------------------------------------------------------------
inline const bool NiBoolKey::GetBool() const
{
    return m_bBool;
}
//---------------------------------------------------------------------------
inline void NiBoolKey::Interpolate(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(bool*)pResult = ms_bDefault;
}
//---------------------------------------------------------------------------
inline void NiBoolKey::InterpolateD1(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(bool*)pResult = ms_bDefault;
}
//---------------------------------------------------------------------------
inline void NiBoolKey::InterpolateD2(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(bool*)pResult = ms_bDefault;
}
//---------------------------------------------------------------------------
inline void NiBoolKey::FillDerivedVals(NiAnimationKey*, unsigned int, 
    unsigned char)
{
    // This routine should never be called.
    NIASSERT( false );
}
//---------------------------------------------------------------------------
inline float NiBoolKey::ComputeMaxCurvature(NiAnimationKey*, unsigned int)
{
    // This routine should never be called.
    NIASSERT( false );
    return 0.0f;
}
//---------------------------------------------------------------------------
inline void NiBoolKey::Copy(NiAnimationKey*, const NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
inline NiAnimationKey* NiBoolKey::CreateArray(unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
inline void NiBoolKey::DeleteArray(NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
inline NiBoolKey* NiBoolKey::GetKeyAt(unsigned int uiIndex, unsigned char 
    ucKeySize)
{
    return (NiBoolKey*) NiAnimationKey::GetKeyAt(uiIndex, ucKeySize);
}
//---------------------------------------------------------------------------
inline unsigned char NiBoolKey::GetKeySize(KeyType eType)
{
     return NiAnimationKey::GetKeySize(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::CreateFunction NiBoolKey::GetCreateFunction(KeyType eType)
{
    return NiAnimationKey::GetCreateFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::CopyFunction NiBoolKey::GetCopyFunction(KeyType eType)
{
    return NiAnimationKey::GetCopyFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::ArrayFunction NiBoolKey::GetArrayFunction(KeyType eType)
{
    return NiAnimationKey::GetArrayFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::DeleteFunction NiBoolKey::GetDeleteFunction(KeyType eType)
{
    return NiAnimationKey::GetDeleteFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::InsertFunction NiBoolKey::GetInsertFunction(KeyType eType)
{
    return NiAnimationKey::GetInsertFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::EqualFunction NiBoolKey::GetEqualFunction(KeyType eType)
{
    return NiAnimationKey::GetEqualFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::SaveFunction NiBoolKey::GetSaveFunction(KeyType eType)
{
    return NiAnimationKey::GetSaveFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::InterpFunction NiBoolKey::GetInterpFunction(KeyType eType)
{
    return NiAnimationKey::GetInterpFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::FillDerivedValsFunction NiBoolKey::GetFillDerivedFunction(
    KeyType eType)
{
    return NiAnimationKey::GetFillDerivedFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::CubicCoefsFunction NiBoolKey::GetCubicCoefsFunction(
    KeyType eType)
{
    return NiAnimationKey::GetCubicCoefsFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
inline NiBoolKey::IsPosedFunction NiBoolKey::GetIsPosedFunction(
    KeyType eType)
{
    return NiAnimationKey::GetIsPosedFunction(BOOLKEY, eType);
}
//---------------------------------------------------------------------------
