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
// NiD3DXEffectFile inline functions
//---------------------------------------------------------------------------
inline const char* NiD3DXEffectFile::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectFile::SetDXVersion(unsigned int uiDXVersion)
{
    m_uiDXVersion = uiDXVersion;
}
//---------------------------------------------------------------------------
inline LPD3DXBASEEFFECT NiD3DXEffectFile::GetEffect() const
{
    return m_pkD3DXEffect;
}
//---------------------------------------------------------------------------
inline LPD3DXEFFECT NiD3DXEffectFile::GetRenderableEffect() const
{
    if (m_bRenderableEffect)
        return (LPD3DXEFFECT)m_pkD3DXEffect;
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectFile::SetEffect(LPD3DXBASEEFFECT pkEffect)
{
    m_pkD3DXEffect = pkEffect;
}
//---------------------------------------------------------------------------
