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
// http://www.emergent.net1

//---------------------------------------------------------------------------
inline NiAnimationKey::NiAnimationKey()
{
}
//---------------------------------------------------------------------------
inline NiAnimationKey::~NiAnimationKey()
{
}
//---------------------------------------------------------------------------
inline NiAnimationKey::NiAnimationKey(float fTime)
{
    m_fTime = fTime;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetTime(float fTime)
{
    m_fTime = fTime;
}
//---------------------------------------------------------------------------
inline float NiAnimationKey::GetTime() const
{
    return m_fTime;
}
//---------------------------------------------------------------------------
inline NiAnimationKey* NiAnimationKey::GetKeyAt(unsigned int uiIndex,
    unsigned char ucKeySize) const
{
    return (NiAnimationKey*) ((char*) this + uiIndex * ucKeySize);
}
//---------------------------------------------------------------------------
inline NiAnimationKey::CreateFunction NiAnimationKey::GetCreateFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_loaders[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::CurvatureFunction NiAnimationKey::GetCurvatureFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_curves[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::InterpFunction NiAnimationKey::GetInterpFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_interps[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::InterpFunction NiAnimationKey::GetInterpD1Function(
    KeyContent eContent, KeyType eType)
{
    return ms_interpd1s[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::InterpFunction NiAnimationKey::GetInterpD2Function(
    KeyContent eContent, KeyType eType)
{
    return ms_interpd2s[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::EqualFunction NiAnimationKey::GetEqualFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_equals[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::CopyFunction NiAnimationKey::GetCopyFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_copies[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::ArrayFunction NiAnimationKey::GetArrayFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_arrays[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::DeleteFunction NiAnimationKey::GetDeleteFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_deletes[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::InsertFunction NiAnimationKey::GetInsertFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_inserts[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::SaveFunction NiAnimationKey::GetSaveFunction(
    KeyContent eContent, KeyType eType)
{
    return ms_saves[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::FillDerivedValsFunction 
    NiAnimationKey::GetFillDerivedFunction(KeyContent eContent, KeyType eType)
{
    return ms_fillderived[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::CubicCoefsFunction 
    NiAnimationKey::GetCubicCoefsFunction(KeyContent eContent, KeyType eType)
{
    return ms_cubiccoefs[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline NiAnimationKey::IsPosedFunction 
    NiAnimationKey::GetIsPosedFunction(KeyContent eContent, KeyType eType)
{
    return ms_isposed[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetCreateFunction(KeyContent eContent, 
    KeyType eType, CreateFunction pCreate)
{
    ms_loaders[eContent * NUMKEYTYPES + eType] = pCreate;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetCurvatureFunction(KeyContent eContent, 
    KeyType eType, CurvatureFunction pCurve)
{
    ms_curves[eContent * NUMKEYTYPES + eType] = pCurve;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetInterpFunction(KeyContent eContent, 
    KeyType eType, InterpFunction pInterp)
{
    ms_interps[eContent * NUMKEYTYPES + eType] = pInterp;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetInterpD1Function(KeyContent eContent, 
    KeyType eType, InterpFunction pInterp)
{
    ms_interpd1s[eContent * NUMKEYTYPES + eType] = pInterp;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetInterpD2Function(KeyContent eContent, 
    KeyType eType, InterpFunction pInterp)
{
    ms_interpd2s[eContent * NUMKEYTYPES + eType] = pInterp;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetEqualFunction(KeyContent eContent, 
    KeyType eType, EqualFunction pEqual)
{
    ms_equals[eContent * NUMKEYTYPES + eType] = pEqual;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetCopyFunction(KeyContent eContent,
    KeyType eType, CopyFunction pCopy)
{
    ms_copies[eContent * NUMKEYTYPES + eType] = pCopy;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetArrayFunction(KeyContent eContent,
    KeyType eType, ArrayFunction pArray)
{
    ms_arrays[eContent * NUMKEYTYPES + eType] = pArray;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetDeleteFunction(KeyContent eContent,
    KeyType eType, DeleteFunction pDelete)
{
    ms_deletes[eContent * NUMKEYTYPES + eType] = pDelete;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetInsertFunction(KeyContent eContent,
    KeyType eType, InsertFunction pInsert)
{
    ms_inserts[eContent * NUMKEYTYPES + eType] = pInsert;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetSaveFunction(KeyContent eContent,
    KeyType eType, SaveFunction pSave)
{
    ms_saves[eContent * NUMKEYTYPES + eType] = pSave;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetFillDerivedValsFunction(KeyContent eContent, 
    KeyType eType, FillDerivedValsFunction pFillDerivedVals)
{
    ms_fillderived[eContent * NUMKEYTYPES + eType] = pFillDerivedVals;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetCubicCoefsFunction(KeyContent eContent, 
    KeyType eType, CubicCoefsFunction pCubicCoefs)
{
    ms_cubiccoefs[eContent * NUMKEYTYPES + eType] = pCubicCoefs;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetIsPosedFunction(KeyContent eContent, 
    KeyType eType, IsPosedFunction pIsPosed)
{
    ms_isposed[eContent * NUMKEYTYPES + eType] = pIsPosed;
}
//---------------------------------------------------------------------------
inline void NiAnimationKey::SetKeySize(KeyContent eContent, 
    KeyType eType, unsigned char ucKeySize)
{
    ms_keysizes[eContent * NUMKEYTYPES + eType] = ucKeySize;
}
//---------------------------------------------------------------------------
inline unsigned char NiAnimationKey::GetKeySize(KeyContent eContent, 
    KeyType eType)
{
    return ms_keysizes[eContent * NUMKEYTYPES + eType];
}
//---------------------------------------------------------------------------
