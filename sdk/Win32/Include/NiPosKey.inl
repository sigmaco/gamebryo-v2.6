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
inline NiPosKey::NiPosKey()
{
}
//---------------------------------------------------------------------------
inline NiPosKey::NiPosKey(float fTime, const NiPoint3& pos)
    :
    NiAnimationKey(fTime)
{
    m_Pos = pos;
}
//---------------------------------------------------------------------------
inline NiPosKey::~NiPosKey ()
{
}
//---------------------------------------------------------------------------
inline void NiPosKey::SetPos(const NiPoint3& pos)
{
    m_Pos = pos;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPosKey::GetPos() const
{
    return m_Pos;
}
//---------------------------------------------------------------------------
inline void NiPosKey::Interpolate(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(NiPoint3*)pResult = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
inline void NiPosKey::InterpolateD1(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(NiPoint3*)pResult = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
inline void NiPosKey::InterpolateD2(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(NiPoint3*)pResult = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
inline void NiPosKey::FillDerivedVals(NiAnimationKey*, unsigned int, 
    unsigned char)
{
    // This routine should never be called.
    NIASSERT( false );
}
//---------------------------------------------------------------------------
inline float NiPosKey::ComputeMaxCurvature(NiAnimationKey*, unsigned int)
{
    // This routine should never be called.
    NIASSERT( false );
    return 0.0f;
}
//---------------------------------------------------------------------------
inline void NiPosKey::Copy(NiAnimationKey*, const NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
inline NiAnimationKey* NiPosKey::CreateArray(unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
inline void NiPosKey::DeleteArray(NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
inline void NiPosKey::SetMaxCurvatureSampleSize(int iSampleSize)
{
    ms_iSampleSize = iSampleSize;
}
//---------------------------------------------------------------------------
inline float NiPosKey::GetMaxCurvatureSampleDelta()
{
    return ms_fSampleDelta;
}
//---------------------------------------------------------------------------
inline NiPosKey* NiPosKey::GetKeyAt(unsigned int uiIndex, unsigned char 
    ucKeySize)
{
    return (NiPosKey*) NiAnimationKey::GetKeyAt(uiIndex, ucKeySize);
}
//---------------------------------------------------------------------------
inline unsigned char NiPosKey::GetKeySize(KeyType eType) 
{
     return NiAnimationKey::GetKeySize(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::CreateFunction NiPosKey::GetCreateFunction(KeyType eType)
{
    return NiAnimationKey::GetCreateFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::CopyFunction NiPosKey::GetCopyFunction(KeyType eType)
{
    return NiAnimationKey::GetCopyFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::ArrayFunction NiPosKey::GetArrayFunction(KeyType eType)
{
    return NiAnimationKey::GetArrayFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::DeleteFunction NiPosKey::GetDeleteFunction(KeyType eType)
{
    return NiAnimationKey::GetDeleteFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::InsertFunction NiPosKey::GetInsertFunction(KeyType eType)
{
    return NiAnimationKey::GetInsertFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::EqualFunction NiPosKey::GetEqualFunction(KeyType eType)
{
    return NiAnimationKey::GetEqualFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::SaveFunction NiPosKey::GetSaveFunction(KeyType eType)
{
    return NiAnimationKey::GetSaveFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::CurvatureFunction NiPosKey::GetCurvatureFunction(
    KeyType eType)
{
    return NiAnimationKey::GetCurvatureFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::InterpFunction NiPosKey::GetInterpFunction(KeyType eType)
{
    return NiAnimationKey::GetInterpFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::InterpFunction NiPosKey::GetInterpD1Function(KeyType eType)
{
    return NiAnimationKey::GetInterpD1Function(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::InterpFunction NiPosKey::GetInterpD2Function(KeyType eType)
{
    return NiAnimationKey::GetInterpD2Function(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::FillDerivedValsFunction NiPosKey::GetFillDerivedFunction(
    KeyType eType)
{
    return NiAnimationKey::GetFillDerivedFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::CubicCoefsFunction NiPosKey::GetCubicCoefsFunction(
    KeyType eType)
{
    return NiAnimationKey::GetCubicCoefsFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
inline NiPosKey::IsPosedFunction NiPosKey::GetIsPosedFunction(
    KeyType eType)
{
    return NiAnimationKey::GetIsPosedFunction(POSKEY, eType);
}
//---------------------------------------------------------------------------
