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
inline NiTexture::NiTexture()
{
    m_pkRendererData = 0;
    AddTextureToList();
}
//---------------------------------------------------------------------------
inline NiTexture::~NiTexture()
{
    NiDelete m_pkRendererData;
    RemoveTextureFromList();
}
//---------------------------------------------------------------------------
inline NiTexture::FormatPrefs::FormatPrefs() 
{
    m_ePixelLayout = PIX_DEFAULT;
    m_eAlphaFmt = ALPHA_DEFAULT;
    m_eMipMapped = MIP_DEFAULT;
}
//---------------------------------------------------------------------------
inline const NiTexture::FormatPrefs& NiTexture::GetFormatPreferences() const
{
    return m_kFormatPrefs;
}
//---------------------------------------------------------------------------
inline void NiTexture::SetRendererData(RendererData* pkRendererData)
{
    m_pkRendererData = pkRendererData;
}
//---------------------------------------------------------------------------
inline NiTexture* NiTexture::GetListNext()
{
    return m_pkNext;
}
//---------------------------------------------------------------------------
inline NiTexture* NiTexture::GetListPrev()
{
    return m_pkPrev;
}
//---------------------------------------------------------------------------
inline NiTexture::RendererData* NiTexture::GetRendererData() const
{
    return m_pkRendererData;
}
//---------------------------------------------------------------------------
inline bool NiTexture::GetMipmapByDefault()
{
    return ms_bMipmapByDefault;
}
//---------------------------------------------------------------------------
inline void NiTexture::SetMipmapByDefault(bool bMipmap)
{
    ms_bMipmapByDefault = bMipmap;
}
//---------------------------------------------------------------------------
inline NiTexture::RendererData::RendererData(NiTexture* pkTexture) : 
    m_pkTexture(pkTexture), m_uiWidth(0), m_uiHeight(0)
{
}
//---------------------------------------------------------------------------
inline NiTexture::RendererData::~RendererData()
{
    /* */
}
//---------------------------------------------------------------------------
inline NiTexture* NiTexture::GetListHead()
{
    return ms_pkHead;
}
//---------------------------------------------------------------------------
inline NiTexture* NiTexture::GetListTail()
{
    return ms_pkTail;
}
//---------------------------------------------------------------------------
inline void NiTexture::AddTextureToList()
{
    ms_kTextureListCriticalSection.Lock();
    if (!ms_pkHead)
        ms_pkHead = this;
    if (ms_pkTail)
    {
        ms_pkTail->m_pkNext = this;
        m_pkPrev = ms_pkTail;
    }
    else
    {
        m_pkPrev = 0;
    }
    ms_pkTail = this;
    m_pkNext = 0;
    ms_kTextureListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
inline void NiTexture::RemoveTextureFromList()
{
    ms_kTextureListCriticalSection.Lock();
    if (ms_pkHead == this)
        ms_pkHead = m_pkNext;
    if (ms_pkTail == this)
        ms_pkTail = m_pkPrev;

    if (m_pkPrev)
        m_pkPrev->m_pkNext = m_pkNext;
    if (m_pkNext)
        m_pkNext->m_pkPrev = m_pkPrev;
    ms_kTextureListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
inline void NiTexture::LockTextureList()
{
    ms_kTextureListCriticalSection.Lock();
}
//---------------------------------------------------------------------------
inline void NiTexture::UnlockTextureList()
{
    ms_kTextureListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
inline const NiPixelFormat* NiTexture::GetPixelFormat() const
{
    // Get a pointer to a pixel descriptor that matches the format that will
    // be used by the renderer.
    if (m_pkRendererData)
        return m_pkRendererData->GetPixelFormat();
    else
        return 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiTexture::RendererData::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiTexture::RendererData::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline const NiPixelFormat* NiTexture::RendererData::GetPixelFormat() const
{
    return &m_kPixelFormat;
}
//---------------------------------------------------------------------------
