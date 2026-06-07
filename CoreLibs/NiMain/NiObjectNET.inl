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
inline void NiObjectNET::SetDefaultEqualityType(EqualityType eEqualityType)
{
    ms_eEqualityType = eEqualityType;
}
//---------------------------------------------------------------------------
inline NiObjectNET::EqualityType NiObjectNET::GetDefaultEqualityType()
{
    return ms_eEqualityType;
}
//---------------------------------------------------------------------------
inline void NiObjectNET::SetDefaultCopyType(CopyType eCopyType)
{
    ms_eCopyType = eCopyType;
}
//---------------------------------------------------------------------------
inline NiObjectNET::CopyType NiObjectNET::GetDefaultCopyType()
{
    return ms_eCopyType;
}
//---------------------------------------------------------------------------
inline void NiObjectNET::SetDefaultAppendCharacter(char cAppendCharacter)
{
    ms_cAppendCharacter = cAppendCharacter;
}
//---------------------------------------------------------------------------
inline char NiObjectNET::GetDefaultAppendCharacter()
{
    return ms_cAppendCharacter;
}
//---------------------------------------------------------------------------
inline unsigned short NiObjectNET::GetExtraDataSize() const
{
    return m_usExtraDataSize;
}
//---------------------------------------------------------------------------
inline NiExtraData* NiObjectNET::GetExtraDataAt(
    unsigned short usExtraDataIndex) const
{
    NIASSERT(usExtraDataIndex < m_usExtraDataSize);

    return m_ppkExtra[usExtraDataIndex];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiObjectNET::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline NiTimeController* NiObjectNET::GetControllers() const
{
    return m_spControllers;
}
//---------------------------------------------------------------------------
#define NiGetController(classname, pkObjectNET) \
    ((classname*) pkObjectNET->GetController(&classname::ms_RTTI))
//---------------------------------------------------------------------------
