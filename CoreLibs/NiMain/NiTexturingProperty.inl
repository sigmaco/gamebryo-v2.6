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
//  NiTexturingProperty inline functions

//---------------------------------------------------------------------------
inline bool NiTexturingProperty::Map::operator!=(const Map& kObj) const
{
    return !(*this == kObj);
}
//---------------------------------------------------------------------------
inline NiTexture* NiTexturingProperty::Map::GetTexture() const
{ 
    return m_spTexture; 
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::Map::SetTexture(NiTexture* pkTexture)
{
    m_spTexture = pkTexture;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ClampMode
    NiTexturingProperty::Map::GetClampMode() const
{
    return (ClampMode)GetField(CLAMPMODE_MASK, CLAMPMODE_POS);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::Map::SetClampMode(
    NiTexturingProperty::ClampMode eClampMode)
{
    SetField(NiInt32ToUInt16(eClampMode), CLAMPMODE_MASK, CLAMPMODE_POS);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::FilterMode
    NiTexturingProperty::Map::GetFilterMode() const
{
    return (FilterMode)GetField(FILTERMODE_MASK, FILTERMODE_POS);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::Map::SetFilterMode(
    NiTexturingProperty::FilterMode eFilterMode)
{
    SetField(NiInt32ToUInt16(eFilterMode), FILTERMODE_MASK, FILTERMODE_POS);
}
//---------------------------------------------------------------------------
inline unsigned short NiTexturingProperty::Map::GetMaxAnisotropy() const
{
    return m_usMaxAnisotropy;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::Map::SetMaxAnisotropy(
    unsigned short usMaxAnisotropy)
{
    m_usMaxAnisotropy = usMaxAnisotropy;
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::Map::GetTextureIndex() const
{
    return GetField(TEXCOORD_MASK, TEXCOORD_POS);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::Map::SetTextureIndex(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= UCHAR_MAX);
    SetField(NiInt32ToUInt16(uiIndex), TEXCOORD_MASK, TEXCOORD_POS);
}
//---------------------------------------------------------------------------
inline NiTextureTransform* NiTexturingProperty::Map::GetTextureTransform()
{
    return m_pkTextureTransform;
}
//---------------------------------------------------------------------------
inline const NiTextureTransform* 
    NiTexturingProperty::Map::GetTextureTransform() const
{
    return m_pkTextureTransform;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::Map::SetTextureTransform(
    NiTextureTransform* pkTextureTransform)
{
    m_pkTextureTransform = pkTextureTransform;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::MapClassID 
    NiTexturingProperty::Map::GetClassID() const
{
    return NiTexturingProperty::MAP_CLASS_BASE;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::BumpMap::BumpMap() : Map()
{
    m_fLumaScale = 1.0f;
    m_fLumaOffset = 0.0f;
    m_fBumpMat00 = 0.5f;
    m_fBumpMat01 = 0.0f;
    m_fBumpMat10 = 0.0f;
    m_fBumpMat11 = 0.5f;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::BumpMap::BumpMap(NiTexture* pkTexture, 
    unsigned int uiIndex, ClampMode eClampMode, FilterMode eFilterMode,
    float fLumaScale, float fLumaOffset,
    float fBumpMat00, float fBumpMat01,  
    float fBumpMat10, float fBumpMat11)
: Map(pkTexture, uiIndex, eClampMode, eFilterMode)
{
    m_fLumaScale = fLumaScale;
    m_fLumaOffset = fLumaOffset;
    m_fBumpMat00 = fBumpMat00;
    m_fBumpMat01 = fBumpMat01;
    m_fBumpMat10 = fBumpMat10;
    m_fBumpMat11 = fBumpMat11;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::BumpMap::~BumpMap()
{
    /* */
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::BumpMap::GetLumaScale() const
{
    return m_fLumaScale;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::BumpMap::SetLumaScale(float fVal)
{
    m_fLumaScale = fVal;
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::BumpMap::GetLumaOffset() const
{
    return m_fLumaOffset;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::BumpMap::SetLumaOffset(float fVal)
{
    m_fLumaOffset = fVal;
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::BumpMap::GetBumpMat00() const
{
    return m_fBumpMat00;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::BumpMap::SetBumpMat00(float fVal)
{
    m_fBumpMat00 = fVal;
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::BumpMap::GetBumpMat01() const
{
    return m_fBumpMat01;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::BumpMap::SetBumpMat01(float fVal)
{
    m_fBumpMat01 = fVal;
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::BumpMap::GetBumpMat10() const
{
    return m_fBumpMat10;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::BumpMap::SetBumpMat10(float fVal)
{
    m_fBumpMat10 = fVal;
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::BumpMap::GetBumpMat11() const
{
    return m_fBumpMat11;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::BumpMap::SetBumpMat11(float fVal)
{
    m_fBumpMat11 = fVal;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::MapClassID 
    NiTexturingProperty::BumpMap::GetClassID() const
{
    return NiTexturingProperty::MAP_CLASS_BUMP;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ShaderMap::ShaderMap() : 
    Map(),
    m_uiID(0)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ShaderMap::ShaderMap(NiTexture* pkTexture, 
    unsigned int uiIndex, ClampMode eClampMode, FilterMode eFilterMode,
    unsigned int uiID) : 
    Map(pkTexture, uiIndex, eClampMode, eFilterMode),
    m_uiID(uiID)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ShaderMap::~ShaderMap()
{
    /* */
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::ShaderMap::GetID() const
{
    return m_uiID;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::ShaderMap::SetID(unsigned int uiID)
{
    m_uiID = uiID;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::MapClassID 
    NiTexturingProperty::ShaderMap::GetClassID() const
{
    return NiTexturingProperty::MAP_CLASS_SHADER;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ParallaxMap::ParallaxMap() : 
    Map(),
    m_fOffset(0.5f)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ParallaxMap::ParallaxMap(NiTexture* pkTexture, 
    unsigned int uiIndex, ClampMode eClampMode, FilterMode eFilterMode,
    float fOffset) : 
    Map(pkTexture, uiIndex, eClampMode, eFilterMode),
    m_fOffset(fOffset)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ParallaxMap::~ParallaxMap()
{
    /* */
}
//---------------------------------------------------------------------------
inline float NiTexturingProperty::ParallaxMap::GetOffset() const
{
    return m_fOffset;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::ParallaxMap::SetOffset(float fOffset)
{
    m_fOffset = fOffset;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::MapClassID 
    NiTexturingProperty::ParallaxMap::GetClassID() const
{
    return NiTexturingProperty::MAP_CLASS_PARALLAX;
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
    NiTexturingProperty::GetBaseMap() const
{
    return m_kMaps.GetAt(BASE_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetBaseMap()
{
    return m_kMaps.GetAt(BASE_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBaseMap(Map* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(BASE_INDEX);
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(BASE_INDEX, pkMap);
    }
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
    NiTexturingProperty::GetDarkMap() const
{
    return m_kMaps.GetAt(DARK_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetDarkMap()
{
    return m_kMaps.GetAt(DARK_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetDarkMap(Map* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(DARK_INDEX);
    if(pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(DARK_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
    NiTexturingProperty::GetDetailMap() const
{
    return m_kMaps.GetAt(DETAIL_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetDetailMap()
{
    return m_kMaps.GetAt(DETAIL_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetDetailMap(Map* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(DETAIL_INDEX);
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(DETAIL_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
    NiTexturingProperty::GetGlossMap() const
{
    return m_kMaps.GetAt(GLOSS_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetGlossMap()
{
    return m_kMaps.GetAt(GLOSS_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetGlossMap(Map* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(GLOSS_INDEX);
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(GLOSS_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
    NiTexturingProperty::GetGlowMap() const
{
    return m_kMaps.GetAt(GLOW_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetGlowMap()
{
    return m_kMaps.GetAt(GLOW_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetGlowMap(Map* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(GLOW_INDEX);
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(GLOW_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::BumpMap* 
    NiTexturingProperty::GetBumpMap() const
{
    // Item in the BUMP_MAP slot is always of the BumpMap subclass
    return (BumpMap*)(m_kMaps.GetAt(BUMP_INDEX));
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::BumpMap* NiTexturingProperty::GetBumpMap()
{
    // Item in the BUMP_MAP slot is always of the BumpMap subclass
    return (BumpMap*)(m_kMaps.GetAt(BUMP_INDEX));
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBumpMap(BumpMap* pkMap)
{
    // Item in the BUMP_MAP slot is always of the BumpMap subclass
    BumpMap* pkOld = (BumpMap*)(m_kMaps.GetAt(BUMP_INDEX));
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(BUMP_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::ParallaxMap* 
NiTexturingProperty::GetParallaxMap() const
{
    return (const NiTexturingProperty::ParallaxMap*)
        m_kMaps.GetAt(PARALLAX_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ParallaxMap* NiTexturingProperty::GetParallaxMap()
{
    return (NiTexturingProperty::ParallaxMap*)
        m_kMaps.GetAt(PARALLAX_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetParallaxMap(ParallaxMap* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(PARALLAX_INDEX);
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(PARALLAX_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
NiTexturingProperty::GetNormalMap() const
{
    return m_kMaps.GetAt(NORMAL_INDEX);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetNormalMap()
{
    return m_kMaps.GetAt(NORMAL_INDEX);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetNormalMap(Map* pkMap)
{
    Map* pkOld = m_kMaps.GetAt(NORMAL_INDEX);
    if (pkMap != pkOld)
    {
        NiDelete pkOld;
        m_kMaps.SetAt(NORMAL_INDEX, pkMap);
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::Map* 
    NiTexturingProperty::GetDecalMap(unsigned int uiIndex) const
{
    NIASSERT((uiIndex + DECAL_BASE) < m_kMaps.GetSize());

    return m_kMaps.GetAt(uiIndex + DECAL_BASE);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTexturingProperty::GetDecalMap(
    unsigned int uiIndex)
{
    NIASSERT((uiIndex + DECAL_BASE) < m_kMaps.GetSize());

    return m_kMaps.GetAt(uiIndex + DECAL_BASE);
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::GetDecalArrayCount() const
{
    return m_kMaps.GetSize() - DECAL_BASE;
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::GetDecalMapCount() const
{
    return GetField(DECALCOUNT_MASK, DECALCOUNT_POS);
}
//---------------------------------------------------------------------------
inline const NiTexturingProperty::ShaderMap* 
    NiTexturingProperty::GetShaderMap(unsigned int uiIndex) const
{
    if (!m_pkShaderMaps || (uiIndex >= m_pkShaderMaps->GetSize()))
        return 0;

    return m_pkShaderMaps->GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ShaderMap* 
    NiTexturingProperty::GetShaderMap(unsigned int uiIndex)
{    
    if (!m_pkShaderMaps || (uiIndex >= m_pkShaderMaps->GetSize()))
        return 0;

    return m_pkShaderMaps->GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::GetShaderArrayCount() const
{
    if (!m_pkShaderMaps)
        return 0;

    return m_pkShaderMaps->GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::GetShaderMapCount() const
{
    if (!m_pkShaderMaps)
        return 0;

    return m_pkShaderMaps->GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ApplyMode NiTexturingProperty::GetApplyMode()
    const
{
    return (ApplyMode)GetField(APPLYMODE_MASK, APPLYMODE_POS);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetApplyMode(
    NiTexturingProperty::ApplyMode eMode)
{
    NIASSERT(eMode < APPLY_MAX_MODES &&
        eMode != APPLY_DEPRECATED &&
        eMode != APPLY_DEPRECATED2);

    SetField(NiInt32ToUInt16(eMode), APPLYMODE_MASK, APPLYMODE_POS);
}
//---------------------------------------------------------------------------
inline bool NiTexturingProperty::IsMultiTexture() const
{
    return GetBit(MULTITEXTURE_MASK);
}
//---------------------------------------------------------------------------
inline int NiTexturingProperty::Type() const
{ 
    return NiProperty::TEXTURING; 
}
//---------------------------------------------------------------------------
inline int NiTexturingProperty::GetType()
{
    return NiProperty::TEXTURING;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty *NiTexturingProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
inline bool NiTexturingProperty::IsEqualFast(
    const NiTexturingProperty& kProp) const
{
    // For now, return that any two distinct properties are different.
    return this == &kProp;
}
//---------------------------------------------------------------------------
inline const NiTPrimitiveArray<NiTexturingProperty::Map*>& 
    NiTexturingProperty::GetMaps() const
{
    return m_kMaps;
}
//---------------------------------------------------------------------------
inline NiTexture* NiTexturingProperty::GetBaseTexture() const
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (pkBaseMap)
        return pkBaseMap->GetTexture();
    else
        return 0;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBaseTexture(NiTexture* pkTexture)
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (!pkBaseMap)
    {
        pkBaseMap = NiNew Map;
        m_kMaps.SetAt(0, pkBaseMap);
    }

    pkBaseMap->SetTexture(pkTexture);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ClampMode NiTexturingProperty::GetBaseClampMode() 
    const
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (pkBaseMap)
        return pkBaseMap->GetClampMode();
    else
        return WRAP_S_WRAP_T;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBaseClampMode(
    NiTexturingProperty::ClampMode eMode)
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (!pkBaseMap)
    {
        pkBaseMap = NiNew Map;
        m_kMaps.SetAt(0, pkBaseMap);
    }

    pkBaseMap->SetClampMode(eMode);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::FilterMode 
    NiTexturingProperty::GetBaseFilterMode() const
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (pkBaseMap)
        return pkBaseMap->GetFilterMode();
    else
        return FILTER_NEAREST;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBaseFilterMode(
    NiTexturingProperty::FilterMode eMode)
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (!pkBaseMap)
    {
        pkBaseMap = NiNew Map;
        m_kMaps.SetAt(0, pkBaseMap);
    }

    pkBaseMap->SetFilterMode(eMode);
}
//---------------------------------------------------------------------------
inline unsigned int NiTexturingProperty::GetBaseTextureIndex() const
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (pkBaseMap)
        return pkBaseMap->GetTextureIndex();
    else
        return 0;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBaseTextureIndex(unsigned int uiIndex)
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (!pkBaseMap)
    {
        pkBaseMap = NiNew Map;
        m_kMaps.SetAt(0, pkBaseMap);
    }

    pkBaseMap->SetTextureIndex(uiIndex);
}
//---------------------------------------------------------------------------
inline NiTextureTransform* NiTexturingProperty::GetBaseTextureTransform() 
    const
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (pkBaseMap)
        return pkBaseMap->GetTextureTransform();
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::SetBaseTextureTransform(
    NiTextureTransform* pkTransform)
{
    Map* pkBaseMap = m_kMaps.GetAt(0);
    if (!pkBaseMap)
    {
        pkBaseMap = NiNew Map;
        m_kMaps.SetAt(0, pkBaseMap);
    }

    pkBaseMap->SetTextureTransform(pkTransform);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::InitializeDecalCount()
{
    SetField(0, DECALCOUNT_MASK, DECALCOUNT_POS);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::IncrementDecalCount()
{
    unsigned int uiCount = GetDecalMapCount();
    NIASSERT(uiCount < UCHAR_MAX);
    SetField(NiInt32ToUInt16(++uiCount), DECALCOUNT_MASK, DECALCOUNT_POS);
}
//---------------------------------------------------------------------------
inline void NiTexturingProperty::DecrementDecalCount()
{
    unsigned int uiCount = GetDecalMapCount();
    NIASSERT(uiCount < UCHAR_MAX);
    SetField(NiInt32ToUInt16(--uiCount), DECALCOUNT_MASK, DECALCOUNT_POS);
}
//---------------------------------------------------------------------------
inline bool NiTexturingProperty::ValidMapExists(MapEnum eMap, 
    unsigned int uiWhichIndex) const
{
    const NiTexturingProperty::Map* pkMap = NULL;
    if (eMap == DECAL_BASE)
    {
        if (m_kMaps.GetSize() > (unsigned int)eMap + uiWhichIndex)
            pkMap = m_kMaps.GetAt((unsigned int)eMap + uiWhichIndex);
    }
    else if (eMap == SHADER_BASE)
    {
        if (m_pkShaderMaps != NULL && m_pkShaderMaps->GetSize() > uiWhichIndex)
        {
            pkMap = m_pkShaderMaps->GetAt(uiWhichIndex);
        }
    }
    else
    {
        NIASSERT(uiWhichIndex == 0);
        pkMap = m_kMaps.GetAt((unsigned int)eMap);
    }

    if (!pkMap)
        return false;

    if (!pkMap->GetTexture())
        return false;

    return true;

}
//---------------------------------------------------------------------------
