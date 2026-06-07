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
// NiD3DXEffectTechniqueSet inline functions
//---------------------------------------------------------------------------
inline const char* NiD3DXEffectTechniqueSet::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectTechniqueSet::GetImplementationCount() const
{
    return m_kImplementations.GetEffectiveSize();
}

//---------------------------------------------------------------------------
inline NiD3DXEffectTechnique* NiD3DXEffectTechniqueSet::GetTechnique(
    unsigned int uiImplementation)
{
    if (uiImplementation < m_kImplementations.GetSize())
        return m_kImplementations.GetAt(uiImplementation);
    return NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectTechniqueSet::GetTechniqueArrayCount() const
{
    return m_kImplementations.GetSize();
}
//---------------------------------------------------------------------------
