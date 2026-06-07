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
inline const NiFixedString& NiShadowTechnique::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiShadowTechnique::
    GetReadFragmentName(NiStandardMaterial::LightType eType) const
{
    return m_kReadFragmentName[eType];
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiShadowTechnique::
    GetWriteFragmentName(NiStandardMaterial::LightType eType) const
{
    return m_kWriteFragmentName[eType];
}
//---------------------------------------------------------------------------
inline bool NiShadowTechnique::IsWriteBatchable()
{
    return m_bWriteBatchable;
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetWriteBatchable(bool bBatchable)
{
    m_bWriteBatchable = bBatchable;
}
//---------------------------------------------------------------------------
inline unsigned short NiShadowTechnique::GetTechniqueID()
{
    return m_usTechniqueID;
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetTechniqueID(unsigned short usID)
{
    m_usTechniqueID = usID;
}
//---------------------------------------------------------------------------
inline unsigned short NiShadowTechnique::GetActiveTechniqueSlot()
{
    return m_uiActiveTechniqueSlot;
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetActiveTechniqueSlot(unsigned short usSlot)
{
    // Ensure that the active shadow technique slot we are telling this 
    // NiShadowTechique it is assigned to, is really the slot it is assigned.
    NIASSERT(NiShadowManager::GetActiveShadowTechnique(usSlot) == this);

    m_uiActiveTechniqueSlot = usSlot;
}
//---------------------------------------------------------------------------
inline bool NiShadowTechnique::GetUseCubeMapForPointLight()
{
    return m_bUseCubeMapForPointLight;
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetUseCubeMapForPointLight(bool bUseCubeMap)
{
    m_bUseCubeMapForPointLight = bUseCubeMap;
}
//---------------------------------------------------------------------------
inline NiTexture::FormatPrefs& NiShadowTechnique::GetTextureFormatPrefs()
{
    return m_kFormatPrefs;
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetTextureFormatPrefs(
    NiTexture::FormatPrefs& pkPrefs)
{
    m_kFormatPrefs = pkPrefs;
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::AppendPostProcessRenderClicks(
    NiShadowMap*, NiShadowClickGenerator*)
{
    /* */
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::PrepareShadowMap(NiShadowMap* pkShadowMap)
{
    pkShadowMap->SetFilterMode(NiTexturingProperty::FILTER_NEAREST);
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetDefaultDepthBias(
    NiStandardMaterial::LightType eType, bool bRenderBackfaces,
    float fDepthBias)
{
    NIASSERT(eType < NiStandardMaterial::LIGHT_MAX);
    int iIndex = NiSelect(bRenderBackfaces, eType,
        eType + NiStandardMaterial::LIGHT_MAX);
    m_afDefaultDepthBias[iIndex] = fDepthBias;
}
//---------------------------------------------------------------------------
inline float NiShadowTechnique::GetDefaultDepthBias(
    NiStandardMaterial::LightType eType, bool bRenderBackfaces)
{
    NIASSERT(eType < NiStandardMaterial::LIGHT_MAX);
    int iIndex = NiSelect(bRenderBackfaces, eType,
        eType + NiStandardMaterial::LIGHT_MAX);
    return m_afDefaultDepthBias[iIndex];
}
//---------------------------------------------------------------------------
inline void NiShadowTechnique::SetDepthFormat(
    const NiPixelFormat* pkDepthFormat)
{
    m_pkDepthFormat = pkDepthFormat;
}
//---------------------------------------------------------------------------
inline const NiPixelFormat* NiShadowTechnique::GetDepthFormat() const
{
    return m_pkDepthFormat;
}
//---------------------------------------------------------------------------
