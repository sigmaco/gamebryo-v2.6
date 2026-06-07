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
inline void NiD3D10EffectFile::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiD3D10EffectFile::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline ID3D10Effect* NiD3D10EffectFile::GetEffect() const
{
    return m_pkD3D10Effect;
}
//---------------------------------------------------------------------------
inline void* NiD3D10EffectFile::GetCompiledEffectData() const
{
    void* pvReturn = NULL;
    if (m_pkCompiledEffect)
        pvReturn = m_pkCompiledEffect->GetBufferPointer();

    return pvReturn;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10EffectFile::GetCompiledEffectDataSize() const
{
    NiUInt32 uiReturn = 0;
    if (m_pkCompiledEffect)
        uiReturn = (NiUInt32)m_pkCompiledEffect->GetBufferSize();

    return uiReturn;
}
//---------------------------------------------------------------------------
