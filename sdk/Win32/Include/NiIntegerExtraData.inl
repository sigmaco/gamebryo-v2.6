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
//  NiIntegerExtraData inline functions

//---------------------------------------------------------------------------
inline NiIntegerExtraData::NiIntegerExtraData()
{
    m_iValue = 0;
}
//---------------------------------------------------------------------------
inline NiIntegerExtraData::NiIntegerExtraData(const int iValue)
{
    m_iValue = iValue;
}
//---------------------------------------------------------------------------
inline NiIntegerExtraData::~NiIntegerExtraData()
{
}
//---------------------------------------------------------------------------
inline int NiIntegerExtraData::GetValue() const
{
    return m_iValue;
}
//---------------------------------------------------------------------------
inline void NiIntegerExtraData::SetValue(const int iValue)
{
    m_iValue = iValue;
}
