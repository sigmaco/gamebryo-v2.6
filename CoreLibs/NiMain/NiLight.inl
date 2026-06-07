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
//  NiLight inline functions

//---------------------------------------------------------------------------
inline void NiLight::SetDimmer(float fDimmer)
{
    m_fDimmer = fDimmer;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiLight::GetDimmer() const
{
    return m_fDimmer;
}
//---------------------------------------------------------------------------
inline void NiLight::SetAmbientColor(const NiColor& color)
{
    m_kAmb = color;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline const NiColor& NiLight::GetAmbientColor() const
{
    return m_kAmb;
}
//---------------------------------------------------------------------------
inline void NiLight::SetDiffuseColor(const NiColor& color)
{
    m_kDiff = color;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline const NiColor& NiLight::GetDiffuseColor() const
{
    return m_kDiff;
}
//---------------------------------------------------------------------------
inline void NiLight::SetSpecularColor(const NiColor& color)
{
    m_kSpec = color;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline const NiColor& NiLight::GetSpecularColor() const
{
    return m_kSpec;
}
//---------------------------------------------------------------------------
#if defined(WIN32) || defined(_XENON) || defined(_WII)
inline void* NiLight::GetRendererData() const
{
    return m_pvRendererData;
}
//---------------------------------------------------------------------------
inline void NiLight::SetRendererData(void* pvRendererData)
{
    m_pvRendererData = pvRendererData;
}
//---------------------------------------------------------------------------
#endif // defined(WIN32) || defined(_XENON) || defined(_WII)

