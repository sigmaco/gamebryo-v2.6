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
//  NiVectorExtraData inline functions

//---------------------------------------------------------------------------
inline NiVectorExtraData::~NiVectorExtraData()
{
}
//---------------------------------------------------------------------------
inline const float* NiVectorExtraData::GetValue() const
{
    return m_afVector;
}
//---------------------------------------------------------------------------
inline const NiPoint3 NiVectorExtraData::GetValueVector3() const
{
    NiPoint3 kVector3(m_afVector[0], m_afVector[1], m_afVector[2]);
    return kVector3;
}
//---------------------------------------------------------------------------
inline void NiVectorExtraData::SetX(const float fX)
{
    m_afVector[0] = fX;
}
//---------------------------------------------------------------------------
inline void NiVectorExtraData::SetY(const float fY)
{
    m_afVector[1] = fY;
}
//---------------------------------------------------------------------------
inline void NiVectorExtraData::SetZ(const float fZ)
{
    m_afVector[2] = fZ;
}
//---------------------------------------------------------------------------
inline void NiVectorExtraData::SetW(const float fW)
{
    m_afVector[3] = fW;
}
//---------------------------------------------------------------------------
inline float NiVectorExtraData::GetX() const
{
    return m_afVector[0];
}
//---------------------------------------------------------------------------
inline float NiVectorExtraData::GetY() const
{
    return m_afVector[1];
}
//---------------------------------------------------------------------------
inline float NiVectorExtraData::GetZ() const
{
    return m_afVector[2];
}
//---------------------------------------------------------------------------
inline float NiVectorExtraData::GetW() const
{
    return m_afVector[3];
}
//---------------------------------------------------------------------------
