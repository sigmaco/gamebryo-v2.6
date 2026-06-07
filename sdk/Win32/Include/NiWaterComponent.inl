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

inline NiUInt32 NiWaterComponent::GetLength() const
{
    return m_uiLength;
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetLength(NiUInt32 uiLength)
{
    if (GetLength() != uiLength &&
        uiLength > 0)
    {
        m_uiLength = uiLength;
        m_bPropertyChanged = true;
        SetBit(true, PROP_LENGTH_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWaterComponent::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetWidth(NiUInt32 uiWidth)
{
    if (GetWidth() != uiWidth &&
        uiWidth > 0)
    {
        m_uiWidth = uiWidth;
        m_bPropertyChanged = true;
        SetBit(true, PROP_WIDTH_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWaterComponent::GetMaxVerticesPerSide() const
{
    return m_uiMaxVerticesPerSide;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWaterComponent::GetNumLengthVertices() const
{
    return m_uiNumVerticesLength;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWaterComponent::GetNumWidthVertices() const
{
    return m_uiNumVerticesWidth;
}
//---------------------------------------------------------------------------
inline NiColorA NiWaterComponent::GetShallowColor() const
{
    if (m_spMasterComponent && !GetBit(PROP_SHALLOW_COLOR_CHANGED))
    {
        return m_spMasterComponent->GetShallowColor();
    }
    else
    {
        return m_kShallowColor;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetShallowColor(NiColorA kColor)
{
    if (GetShallowColor() != kColor)
    {
        m_kShallowColor = kColor;
        m_bPropertyChanged = true;
        SetBit(true, PROP_SHALLOW_COLOR_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiColorA NiWaterComponent::GetDeepColor() const
{   
    if (m_spMasterComponent && !GetBit(PROP_DEEP_COLOR_CHANGED))
    {
        return m_spMasterComponent->GetDeepColor() ;
    }
    else
    {
        return m_kDeepColor;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetDeepColor(NiColorA kColor)
{
    if (GetDeepColor() != kColor)
    {
        m_kDeepColor = kColor;
        m_bPropertyChanged = true;
        SetBit(true, PROP_DEEP_COLOR_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWaterComponent::GetEnvMap() const
{
    if (m_spMasterComponent && !GetBit(PROP_ENVMAP_CHANGED))
    {
        return m_spMasterComponent->GetEnvMap();
    }
    else
    {
        return m_kEnvMap;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetEnvMap(NiFixedString kEnvMap)
{
    if (GetEnvMap() != kEnvMap)
    {
        m_kEnvMap = kEnvMap;
        m_bPropertyChanged = true;
        SetBit(true, PROP_ENVMAP_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetDepthThreshold() const
{
    if (m_spMasterComponent && !GetBit(PROP_DEPTH_CHANGED))
    {
        return m_spMasterComponent->GetDepthThreshold();
    }
    else
    {
        return m_fDepthThreshold;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetDepthThreshold(float fDepth)
{
    if (GetDepthThreshold() != fDepth)
    {
        m_fDepthThreshold = fDepth;
        m_bPropertyChanged = true;
        SetBit(true, PROP_DEPTH_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWaterComponent::GetNormalMapDir() const
{
    if (m_spMasterComponent && !GetBit(PROP_NORMAL_DIR_CHANGED))
    {
        return m_spMasterComponent->GetNormalMapDir();
    }
    else
    {
        return m_kNormalMapDir;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetNormalMapDir(NiFixedString kDir)
{
    if (GetNormalMapDir() != kDir)
    {
        m_kNormalMapDir = kDir;
        m_bPropertyChanged = true;
        SetBit(true, PROP_NORMAL_DIR_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWaterComponent::GetNormalFile() const
{
    if (m_spMasterComponent && !GetBit(PROP_NORMAL_NAME_CHANGED))
    {
        return m_spMasterComponent->GetNormalFile();
    }
    else
    {
        return m_kNormalMapFile;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetNormalFile(NiFixedString kFile)
{
    if (GetNormalFile() != kFile)
    {
        m_kNormalMapFile = kFile;
        m_bPropertyChanged = true;
        SetBit(true, PROP_NORMAL_NAME_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWaterComponent::GetTextureSizeRatio() const
{
    if (m_spMasterComponent && !GetBit(PROP_TEXTURE_SIZE_CHANGED))
    {
        return m_spMasterComponent->GetTextureSizeRatio();
    }
    else
    {
        return m_uiTextureSizeRatio;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetTextureSizeRatio(NiUInt32 uiSize)
{
    if (GetTextureSizeRatio() != uiSize)
    {
        m_uiTextureSizeRatio = uiSize;
        m_bPropertyChanged = true;
        SetBit(true, PROP_TEXTURE_SIZE_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiPoint2 NiWaterComponent::GetNormalVelocity() const
{
    if (m_spMasterComponent && !GetBit(PROP_NORMAL_VELOCITY_CHANGED))
    {
        return m_spMasterComponent->GetNormalVelocity() ;
    }
    else
    {
        return m_kTexTransformVelocity;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetNormalVelocity(NiPoint2 kVelocity)
{
    if (GetNormalVelocity() != kVelocity)
    {
        m_kTexTransformVelocity = kVelocity;
        m_bPropertyChanged = true;
        SetBit(true, PROP_NORMAL_VELOCITY_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetNormalLength() const
{
    if (m_spMasterComponent && !GetBit(PROP_NORMAL_LENGTH_CHANGED))
    {
        return m_spMasterComponent->GetNormalLength() ;
    }
    else
    {
        return m_fNormalLength;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetNormalLength(float fLength)
{
    if (GetNormalLength() != fLength)
    {
        m_fNormalLength = fLength;
        m_bPropertyChanged = true;
        SetBit(true, PROP_NORMAL_LENGTH_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetNormalWidth() const
{
    if (m_spMasterComponent && !GetBit(PROP_NORMAL_WIDTH_CHANGED))
    {
        return m_spMasterComponent->GetNormalWidth() ;
    }
    else
    {
        return m_fNormalWidth;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetNormalWidth(float fWidth)
{
    if (GetNormalWidth() != fWidth)
    {
        m_fNormalWidth = fWidth;
        m_bPropertyChanged = true;
        SetBit(true, PROP_NORMAL_WIDTH_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetReflectionFactor() const
{
    if (m_spMasterComponent && !GetBit(PROP_REFLECTION_FACTOR_CHANGED))
    {
        return m_spMasterComponent->GetReflectionFactor() ;
    }
    else
    {
        return m_fReflectionFactor;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetReflectionFactor(float fReflection)
{
    if (GetReflectionFactor() != fReflection)
    {
        m_fReflectionFactor = fReflection;
        m_bPropertyChanged = true;
        SetBit(true, PROP_REFLECTION_FACTOR_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiWaterComponent::GetAssociatedTerrain() const
{
    if (m_spMasterComponent && !GetBit(PROP_TERRAIN_CHANGED))
    {
        return m_spMasterComponent->GetAssociatedTerrain() ;
    }
    else
    {
        return m_spAssociatedTerrain;
    }
}
//---------------------------------------------------------------------------
inline NiTexture* NiWaterComponent::GetDynamicEnvMap() const
{
    if (m_spMasterComponent && !GetBit(PROP_DYNAMIC_ENVMAP))
    {
        return m_spMasterComponent->GetDynamicEnvMap() ;
    }
    else
    {
        return m_spRootNode->GetDynamicEnvMap();
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetDynamicEnvMap(NiTexture* pkTexture)
{
    if (GetDynamicEnvMap() != pkTexture)
    {
        m_spRootNode->SetDynamicEnvMap(pkTexture);
        SetBit(true, PROP_DYNAMIC_ENVMAP);
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWaterComponent::GetRefractionMap() const
{
    if (m_spMasterComponent && !GetBit(PROP_REFRACTION_MAP))
    {
        return m_spMasterComponent->GetRefractionMap() ;
    }
    else
    {
        return m_kRefractionMap;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetRefractionMap(NiFixedString kRefractionPath)
{
    if (GetRefractionMap() != kRefractionPath)
    {
        m_kRefractionMap = kRefractionPath;
        m_bPropertyChanged = true;
        SetBit(true, PROP_REFRACTION_MAP);
    }
}
//---------------------------------------------------------------------------
inline NiTexture* NiWaterComponent::GetDynamicRefractionMap() const
{
    if (m_spMasterComponent && !GetBit(PROP_DYNAMIC_REFRACTION_MAP))
    {
        return m_spMasterComponent->GetDynamicRefractionMap() ;
    }
    else
    {
        return m_spRootNode->GetDynamicRefractionMap();
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetDynamicRefractionMap(NiTexture* pkTexture)
{
    if (GetDynamicRefractionMap() != pkTexture)
    {
        m_spRootNode->SetDynamicRefractionMap(pkTexture);
        SetBit(true, PROP_DYNAMIC_REFRACTION_MAP);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetSpecularity() const
{
    if (m_spMasterComponent && !GetBit(PROP_SPECULAR_VALUE_CHANGED))
    {
        return m_spMasterComponent->GetSpecularity();
    }
    else
    {
        return m_fSpecularity;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetSpecularity(float fSpecularity)
{
    if (GetSpecularity() != fSpecularity)
    {
        m_fSpecularity = fSpecularity;
        m_bPropertyChanged = true;
        SetBit(true, PROP_SPECULAR_VALUE_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetDisturbance() const
{
    if (m_spMasterComponent && !GetBit(PROP_DISTURBANCE_VALUE_CHANGED))
    {
        return m_spMasterComponent->GetDisturbance();
    }
    else
    {
        return m_fDisturbance;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetDisturbance(float fDisturbance)
{
    if (GetDisturbance() != fDisturbance)
    {
        m_fDisturbance = fDisturbance;
        m_bPropertyChanged = true;
        SetBit(true, PROP_DISTURBANCE_VALUE_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetFogDensity() const
{
    if (m_spMasterComponent && !GetBit(PROP_FOGDENSITY_VALUE_CHANGED))
    {
        return m_spMasterComponent->GetFogDensity();
    }
    else
    {
        return m_fFogDensity;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetFogDensity(float fFogDensity)
{
    if (GetFogDensity() != fFogDensity)
    {
        m_fFogDensity = fFogDensity;
        m_bPropertyChanged = true;
        SetBit(true, PROP_FOGDENSITY_VALUE_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetFogDistance() const
{
    if (m_spMasterComponent && !GetBit(PROP_FOGDISTANCE_VALUE_CHANGED))
    {
        return m_spMasterComponent->GetFogDistance();
    }
    else
    {
        return m_fFogDistance;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetFogDistance(float fFogDistance)
{
    if (GetFogDistance() != fFogDistance)
    {
        m_fFogDistance = fFogDistance;
        m_bPropertyChanged = true;
        SetBit(true, PROP_FOGDISTANCE_VALUE_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetFogFallOff() const
{
    if (m_spMasterComponent && !GetBit(PROP_FOGFALLOFF_VALUE_CHANGED))
    {
        return m_spMasterComponent->GetFogFallOff();
    }
    else
    {
        return m_fFogFallOff;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetFogFallOff(float fFogFallOff)
{
    if (GetFogFallOff() != fFogFallOff)
    {
        m_fFogFallOff = fFogFallOff;
        m_bPropertyChanged = true;
        SetBit(true, PROP_FOGFALLOFF_VALUE_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline float NiWaterComponent::GetNormalAnimationSpeed() const
{
    if (m_spMasterComponent && !GetBit(PROP_NORMAL_ANIMATION_SPEED_CHANGED))
    {
        return m_spMasterComponent->m_fNormalAnimationSpeed;
    }
    else
    {
        return m_fNormalAnimationSpeed;
    }
}
//---------------------------------------------------------------------------
inline void NiWaterComponent::SetNormalAnimationSpeed(float fSpeed)
{
    if (GetNormalAnimationSpeed() != fSpeed && fSpeed >= 0.0f)
    {
        m_fNormalAnimationSpeed = fSpeed;
        m_bPropertyChanged = true;
        SetBit(true, PROP_NORMAL_ANIMATION_SPEED_CHANGED);
    }
}
//---------------------------------------------------------------------------
