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
inline NiSourceTexture::NiSourceTexture() :
    m_bStatic(true),
    m_bLoadDirectToRendererHint(false),
    m_bSrcRendererDataIsPersistent(false)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiSourceTexture::~NiSourceTexture()
{
    m_spSrcPixelData = 0;
    m_spPersistentSrcRendererData = 0;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSourceTexture::GetFilename() const
{
    return m_kFilename;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSourceTexture::GetPlatformSpecificFilename()
    const
{
    return m_kPlatformFilename;
}
//---------------------------------------------------------------------------
inline NiPixelData* NiSourceTexture::GetSourcePixelData()
{
    return m_spSrcPixelData;
}
//---------------------------------------------------------------------------
inline const NiPixelData* NiSourceTexture::GetSourcePixelData() const
{
    return m_spSrcPixelData;
}
//---------------------------------------------------------------------------
inline NiPersistentSrcTextureRendererData*
    NiSourceTexture::GetPersistentSourceRendererData()
{
    return m_spPersistentSrcRendererData;
}
//---------------------------------------------------------------------------
inline const NiPersistentSrcTextureRendererData*
    NiSourceTexture::GetPersistentSourceRendererData() const
{
    return m_spPersistentSrcRendererData;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetPersistentSourceRendererData(
    NiPersistentSrcTextureRendererData* pkPersistentSrcRendererData)
{
    m_spPersistentSrcRendererData = pkPersistentSrcRendererData;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetStatic(bool bStatic)
{
    m_bStatic = bStatic;
}
//---------------------------------------------------------------------------
inline bool NiSourceTexture::GetStatic() const
{
    return m_bStatic;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetLoadDirectToRendererHint(bool bDirect)
{
    m_bLoadDirectToRendererHint = bDirect;
}
//---------------------------------------------------------------------------
inline bool NiSourceTexture::GetLoadDirectToRendererHint() const
{
    return m_bLoadDirectToRendererHint;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetSourceRendererDataIsPersistent(
    bool bSrcRendererDataIsPersistent)
{
    m_bSrcRendererDataIsPersistent = bSrcRendererDataIsPersistent;
}
//---------------------------------------------------------------------------
inline bool NiSourceTexture::GetSourceRendererDataIsPersistent() const
{
    return m_bSrcRendererDataIsPersistent;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetUsePreloading(bool bPreload)
{
    ms_bPreload = bPreload;
}
//---------------------------------------------------------------------------
inline bool NiSourceTexture::GetUsePreloading()
{
    return ms_bPreload;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetUseMipmapping(bool bMipmap)
{
    ms_bMipmap = bMipmap;
}
//---------------------------------------------------------------------------
inline bool NiSourceTexture::GetUseMipmapping()
{
    return ms_bMipmap;
}
//---------------------------------------------------------------------------
inline void NiSourceTexture::SetDestroyAppDataFlag(bool bDestroy)
{
    ms_bDestroyAppData = bDestroy;
}
//---------------------------------------------------------------------------
inline bool NiSourceTexture::GetDestroyAppDataFlag()
{
    return ms_bDestroyAppData;
}
//---------------------------------------------------------------------------
