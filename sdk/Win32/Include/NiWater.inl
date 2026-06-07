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
inline NiUInt32 NiWater::GetLength() const
{
    return m_uiLength;
}
//---------------------------------------------------------------------------
inline void NiWater::SetLength(NiUInt32 uiLength)
{
    if (GetLength() != uiLength &&
        uiLength > 0)
    {
        DetachChild(m_spWaterMesh);
        m_spWaterMesh = 0;
        m_spTextureMask = 0;
        m_uiLength = uiLength;
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWater::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline void NiWater::SetWidth(NiUInt32 uiWidth)
{
    if (GetWidth() != uiWidth &&
        uiWidth > 0)
    {
        DetachChild(m_spWaterMesh);
        m_spWaterMesh = 0;
        m_spTextureMask = 0;        
        m_uiWidth = uiWidth;
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWater::GetMaxVerticesPerSide() const
{
    return m_uiMaxVerticesPerSide;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWater::GetNumLengthVertices() const
{
    return m_uiNumVerticesLength;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWater::GetNumWidthVertices() const
{
    return m_uiNumVerticesWidth;
}
//---------------------------------------------------------------------------
inline NiColorA NiWater::GetShallowColor() const
{
    return m_kShallowColor;
}
//---------------------------------------------------------------------------
inline void NiWater::SetShallowColor(NiColorA kColor)
{
    if (GetShallowColor() != kColor)
    {
        m_bRedrawTexture = true;
        m_kShallowColor = kColor;
    }
}
//---------------------------------------------------------------------------
inline NiColorA NiWater::GetDeepColor() const
{   
    return m_kDeepColor;
}
//---------------------------------------------------------------------------
inline void NiWater::SetDeepColor(NiColorA kColor)
{
    if (GetDeepColor() != kColor)
    {
        m_bRedrawTexture = true;
        m_kDeepColor =  kColor;
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWater::GetEnvMap() const
{
    return m_kEnvMap;
}
//---------------------------------------------------------------------------
inline void NiWater::SetEnvMap(NiFixedString kEnvMap)
{
    if (GetEnvMap() != kEnvMap)
    {
        m_kEnvMap = kEnvMap;
        m_bReloadEnvMap = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetDepthThreshold() const
{
    return m_fDepthThreshold;
}
//---------------------------------------------------------------------------
inline void NiWater::SetDepthThreshold(float fDepth)
{
    if (GetDepthThreshold() != fDepth)
    {
        m_fDepthThreshold = fDepth;
        m_bRedrawTexture = true;
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWater::GetNormalMapDir() const
{
    return m_kNormalMapDir;
}
//---------------------------------------------------------------------------
inline bool NiWater::SetNormalMapDir(NiFixedString kDir)
{
    if (GetNormalMapDir() != kDir)
    {
        char acAbsPath[NI_MAX_PATH];
        if (NiPath::IsRelative(kDir))
        {
            char acDir[NI_MAX_PATH];
            NiPath::GetCurrentWorkingDirectory(acDir, NI_MAX_PATH);
            NiPath::ConvertToAbsolute(acAbsPath, NI_MAX_PATH, kDir, acDir);
        }
        else
        {
            NiStrcpy(acAbsPath, NI_MAX_PATH, kDir);
        }

        // We only return false if the directory does not exist
        if (NiFile::DirectoryExists(acAbsPath) == false)
            return false;

        m_kNormalMapDir = kDir;
        
        m_bReloadNormalMaps = true;

        if (m_kCurrentNormalMap == 0)
            m_kCurrentNormalMap = m_kNormalMapList.GetHeadPos();        
    }
    return true;
}
//---------------------------------------------------------------------------
inline NiFixedString NiWater::GetNormalFile() const
{
    return m_kNormalMapFile;
}
//---------------------------------------------------------------------------
inline void NiWater::SetNormalFile(NiFixedString kFile)
{
    if (GetNormalFile() != kFile)
    {
        m_kNormalMapFile = kFile;

        m_bReloadNormalMaps = true;

        if (m_kCurrentNormalMap == 0)
            m_kCurrentNormalMap = m_kNormalMapList.GetHeadPos();
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiWater::GetTextureSizeRatio() const
{
    return m_uiTextureSizeRatio;
}
//---------------------------------------------------------------------------
inline void NiWater::SetTextureSizeRatio(NiUInt32 uiSize)
{
    if (GetTextureSizeRatio() != uiSize)
    {
        m_bRedrawTexture = true;
        m_spTextureMask = 0;

        if (uiSize < 16)
        {
            m_uiTextureSizeRatio = 16;
        }
        else if (NiIsPowerOf2(uiSize))
        {
            m_uiTextureSizeRatio = uiSize;
        }
        else
        {
            // Find nearest power of two: 
            NiUInt32 lower = (NiUInt32) (uiSize / 2);
            NiUInt32 higher = (NiUInt32) (uiSize);

            lower--;
            for (NiUInt32 i = 1; i<sizeof(NiUInt32) * 8; i = i * 2)
                lower = lower | lower >> i;
            
            lower++;

            higher--;
            for (NiUInt32 i = 1; i < sizeof(NiUInt32) * 8; i = i * 2)
                higher = higher | higher >> i;
            higher++;

            if (uiSize - lower < higher - uiSize)
            {
                m_uiTextureSizeRatio = lower;
            }
            else
            {
                m_uiTextureSizeRatio = higher;
            }
            
        }
    }
}
//---------------------------------------------------------------------------
inline NiPoint2 NiWater::GetNormalVelocity() const
{
    return m_kTexTransformVelocity;
}
//---------------------------------------------------------------------------
inline void NiWater::SetNormalVelocity(NiPoint2 kVelocity)
{
    if (GetNormalVelocity() != kVelocity)
    {
        m_kTexTransformVelocity = kVelocity;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetNormalLength() const
{
    return m_fNormalLength;
}
//---------------------------------------------------------------------------
inline void NiWater::SetNormalLength(float fLength)
{
    if (GetNormalLength() != fLength)
    {
        m_fNormalLength = fLength;  
        m_bRedrawTexture = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetNormalWidth() const
{
    return m_fNormalWidth;
}
//---------------------------------------------------------------------------
inline void NiWater::SetNormalWidth(float fWidth)
{
    if (GetNormalWidth() != fWidth)
    {
        m_fNormalWidth = fWidth;
        m_bRedrawTexture = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetReflectionFactor() const
{
    return m_fReflectionFactor;
}
//---------------------------------------------------------------------------
inline void NiWater::SetReflectionFactor(float fReflection)
{
    if (GetReflectionFactor() != fReflection)
    {
        m_fReflectionFactor = fReflection;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline NiTerrain* NiWater::GetAssociatedTerrain() const
{
    return m_spAssociatedTerrain;
}
//---------------------------------------------------------------------------
inline void NiWater::SetAssociatedTerrain(NiTerrain* pkTerrain)
{
    if (GetAssociatedTerrain() != pkTerrain)
    {
        m_spAssociatedTerrain = pkTerrain;
        m_bRedrawTexture = true;
    }
}

//---------------------------------------------------------------------------
inline NiTexture* NiWater::GetDynamicEnvMap() const
{
    return m_spEnvironmentMap;
}
//---------------------------------------------------------------------------
inline void NiWater::SetDynamicEnvMap(NiTexture* pkTexture)
{
    if (GetDynamicEnvMap() != pkTexture)
    {
        m_spEnvironmentMap = 0;
        m_spEnvironmentMap = NiDynamicCast(NiRenderedTexture, pkTexture);
        
        if (NiIsKindOf(NiRenderedCubeMap, pkTexture))
        {
            SetReflectionMode(REFLECTION_MODE_CUBE_MAP);
        }
        else
        {
            SetReflectionMode(REFLECTION_MODE_PLANAR_PROJECTION);
        }
    }
}
//---------------------------------------------------------------------------
inline NiFixedString NiWater::GetRefractionMap() const
{
    return m_kRefractionMap;
}
//---------------------------------------------------------------------------
inline void NiWater::SetRefractionMap(NiFixedString kRefractionPath)
{
    if (GetRefractionMap() != kRefractionPath)
    {
        m_kRefractionMap = kRefractionPath;
        m_bReloadRefractionMap = true;
    }
}
//---------------------------------------------------------------------------
inline NiTexture* NiWater::GetDynamicRefractionMap() const
{
    return m_spRefractionMap;
}
//---------------------------------------------------------------------------
inline void NiWater::SetDynamicRefractionMap(NiTexture* pkTexture)
{
    if (GetDynamicRefractionMap() != pkTexture)
    {
        m_spRefractionMap = 0;
        m_spRefractionMap = NiDynamicCast(NiRenderedTexture, pkTexture);
    }
}
//---------------------------------------------------------------------------
inline NiUInt8 NiWater::GetReflectionMode() const
{
    return m_uiRefMode;
}
//---------------------------------------------------------------------------
inline void NiWater::SetReflectionMode(NiUInt8 uiRefMode)
{
    if (GetReflectionMode() != uiRefMode)
    {
        m_uiRefMode = uiRefMode;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetSpecularity() const
{
    return m_fSpecularity;
}
//---------------------------------------------------------------------------
inline void NiWater::SetSpecularity(float fSpecularity)
{
    if (GetSpecularity() != fSpecularity)
    {
        m_fSpecularity = fSpecularity;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetDisturbance() const
{
    return m_fDisturbance;
}
//---------------------------------------------------------------------------
inline void NiWater::SetDisturbance(float fDisturbance)
{
    if (GetDisturbance() != fDisturbance)
    {
        m_fDisturbance = fDisturbance;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetFogDensity() const
{
    return m_fFogDensity;
}
//---------------------------------------------------------------------------
inline void NiWater::SetFogDensity(float fFogDensity)
{
    if (GetFogDensity() != fFogDensity)
    {
        m_fFogDensity = fFogDensity;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetFogDistance() const
{
    return m_fFogDistance;
}
//---------------------------------------------------------------------------
inline void NiWater::SetFogDistance(float fFogDistance)
{
    if (GetFogDistance() != fFogDistance)
    {
        m_fFogDistance = fFogDistance;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetFogFallOff() const
{
    return m_fFogFallOff;
}
//---------------------------------------------------------------------------
inline void NiWater::SetFogFallOff(float fFogFallOff)
{
    if (GetFogFallOff() != fFogFallOff)
    {
        m_fFogFallOff = fFogFallOff;
        m_bUpdateExtraData = true;
    }
}
//---------------------------------------------------------------------------
inline float NiWater::GetNormalAnimationSpeed() const
{
    return m_fNormalAnimationSpeed;
}
//---------------------------------------------------------------------------
inline void NiWater::SetNormalAnimationSpeed(float fSpeed)
{
    if (m_fNormalAnimationSpeed != fSpeed && fSpeed >= 0.0f)
    {
        m_fNormalAnimationSpeed = fSpeed;
    }
}
//---------------------------------------------------------------------------
