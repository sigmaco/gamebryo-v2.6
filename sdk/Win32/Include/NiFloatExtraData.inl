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
//  NiFloatExtraData inline functions

//---------------------------------------------------------------------------
inline NiFloatExtraData::NiFloatExtraData()
{
    m_fValue = 0;
}
//---------------------------------------------------------------------------
inline NiFloatExtraData::NiFloatExtraData(const float fValue)
{
    m_fValue = fValue;
}
//---------------------------------------------------------------------------
inline NiFloatExtraData::~NiFloatExtraData ()
{
}
//---------------------------------------------------------------------------
inline float NiFloatExtraData::GetValue() const
{
    return m_fValue;
}
//---------------------------------------------------------------------------
inline void NiFloatExtraData::SetValue(const float fValue)
{
    m_fValue = fValue;
}
