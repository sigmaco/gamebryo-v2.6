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
inline float* NiParticlesData::GetRadii()
{
    return m_pfRadii;
}
//---------------------------------------------------------------------------
inline const float* NiParticlesData::GetRadii() const
{
    return m_pfRadii;
}
//---------------------------------------------------------------------------
inline float* NiParticlesData::GetSizes()
{
    return m_pfSizes;
}
//---------------------------------------------------------------------------
inline const float* NiParticlesData::GetSizes() const
{
    return m_pfSizes;
}
//---------------------------------------------------------------------------
inline NiQuaternion* NiParticlesData::GetRotations()
{
    return m_pkRotations;
}
//---------------------------------------------------------------------------
inline const NiQuaternion* NiParticlesData::GetRotations() const
{
    return m_pkRotations;
}
//---------------------------------------------------------------------------
inline float* NiParticlesData::GetRotationAngles()
{
    return m_pfRotationAngles;
}
//---------------------------------------------------------------------------
inline const float* NiParticlesData::GetRotationAngles() const
{
    return m_pfRotationAngles;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiParticlesData::GetRotationAxes()
{
    return m_pkRotationAxes;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiParticlesData::GetRotationAxes() const
{
    return m_pkRotationAxes;
}
//---------------------------------------------------------------------------
