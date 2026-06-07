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
inline NiColorKey::NiColorKey()
{
}
//---------------------------------------------------------------------------
inline NiColorKey::NiColorKey(float fTime, const NiColorA& color)
    :
    NiAnimationKey(fTime)
{
    m_Color = color;
}
//---------------------------------------------------------------------------
inline NiColorKey::~NiColorKey ()
{
}
//---------------------------------------------------------------------------
inline void NiColorKey::SetColor(const NiColorA& color)
{
    m_Color = color;
}
//---------------------------------------------------------------------------
inline const NiColorA& NiColorKey::GetColor() const
{
    return m_Color;
}
//---------------------------------------------------------------------------
inline void NiColorKey::Copy(NiAnimationKey*, const NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
inline NiAnimationKey* NiColorKey::CreateArray(unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
inline void NiColorKey::DeleteArray(NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
inline void NiColorKey::Interpolate(float, const NiAnimationKey*,
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(NiColorA*)pResult = NiColorA::BLACK;
}
//---------------------------------------------------------------------------
inline NiColorKey* NiColorKey::GetKeyAt(unsigned int uiIndex, 
    unsigned char ucSize)
{
    return (NiColorKey*) NiAnimationKey::GetKeyAt(uiIndex, ucSize);
}
//---------------------------------------------------------------------------
inline unsigned char NiColorKey::GetKeySize(KeyType eType) 
{
     return NiAnimationKey::GetKeySize(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::CreateFunction NiColorKey::GetCreateFunction(KeyType eType)
{
    return NiAnimationKey::GetCreateFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::SaveFunction NiColorKey::GetSaveFunction(KeyType eType)
{
    return NiAnimationKey::GetSaveFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::EqualFunction NiColorKey::GetEqualFunction(KeyType eType)
{
    return NiAnimationKey::GetEqualFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::CopyFunction NiColorKey::GetCopyFunction(KeyType eType)
{
    return NiAnimationKey::GetCopyFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::ArrayFunction NiColorKey::GetArrayFunction(KeyType eType)
{
    return NiAnimationKey::GetArrayFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::DeleteFunction NiColorKey::GetDeleteFunction(KeyType eType)
{
    return NiAnimationKey::GetDeleteFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::InsertFunction NiColorKey::
    GetInsertFunction(KeyType eType)
{
    return NiAnimationKey::GetInsertFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::InterpFunction NiColorKey::GetInterpFunction(KeyType eType)
{
    return NiAnimationKey::GetInterpFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline void NiColorKey::SetDefault(const NiColorA& kDefault)
{
    ms_kDefault = kDefault;
}
//---------------------------------------------------------------------------
inline NiColorKey::CubicCoefsFunction NiColorKey::GetCubicCoefsFunction(
    KeyType eType)
{
    return NiAnimationKey::GetCubicCoefsFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
inline NiColorKey::IsPosedFunction NiColorKey::GetIsPosedFunction(
    KeyType eType)
{
    return NiAnimationKey::GetIsPosedFunction(COLORKEY, eType);
}
//---------------------------------------------------------------------------
