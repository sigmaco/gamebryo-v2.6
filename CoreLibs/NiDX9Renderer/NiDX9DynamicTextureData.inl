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
inline D3DBaseTexturePtr NiDX9DynamicTextureData::GetSysMemTexture() const
{
    return m_pkD3DLockableSysMemTexture;
}
//---------------------------------------------------------------------------
inline bool NiDX9DynamicTextureData::IsLocked()
{
    return m_bTextureLocked;
}
//---------------------------------------------------------------------------
inline void NiDX9DynamicTextureData::SetLocked(bool bTextureLocked)
{
    m_bTextureLocked = bTextureLocked;
}
//---------------------------------------------------------------------------
