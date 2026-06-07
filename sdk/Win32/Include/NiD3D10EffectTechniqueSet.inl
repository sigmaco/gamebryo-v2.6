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
inline const NiFixedString& NiD3D10EffectTechniqueSet::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10EffectTechniqueSet::GetImplementationCount() const
{
    return m_kImplementations.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline NiD3D10EffectTechnique* NiD3D10EffectTechniqueSet::GetTechnique(
    NiUInt32 uiImplementation)
{
    if (uiImplementation < m_kImplementations.GetSize())
        return m_kImplementations.GetAt(uiImplementation);
    return NULL;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10EffectTechniqueSet::GetTechniqueArrayCount() const
{
    return m_kImplementations.GetSize();
}
//---------------------------------------------------------------------------
inline void NiD3D10EffectTechniqueSet::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
