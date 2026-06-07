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
inline const char* NSBTexture::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline unsigned int NSBTexture::GetStage()
{
    return m_uiStage;
}
//---------------------------------------------------------------------------
inline void NSBTexture::SetStage(unsigned int uiStage)
{
    m_uiStage = uiStage;
}
//---------------------------------------------------------------------------
inline unsigned int NSBTexture::GetTextureFlags()
{
    return m_uiTextureFlags;
}
//---------------------------------------------------------------------------
inline void NSBTexture::SetSource(unsigned int uiNDLMap)
{
    // Clear the DECAL and SHADER modes
    m_uiTextureFlags &= ~NiTextureStage::TSTF_MAP_MASK;

    // Set the given map flag
    m_uiTextureFlags |= uiNDLMap;
}
//---------------------------------------------------------------------------
inline void NSBTexture::SetSourceDecal(unsigned int uiDecal)
{
    // Clear the Map and Gamebryo sections
    m_uiTextureFlags &= ~(NiTextureStage::TSTF_MAP_MASK | 
        NiTextureStage::TSTF_NDL_TYPEMASK);

    // Set decal mode and index
    m_uiTextureFlags |= (NiTextureStage::TSTF_MAP_DECAL | uiDecal);
}
//---------------------------------------------------------------------------
inline void NSBTexture::SetSourceShader(unsigned int uiShaderMap)
{
    // Clear the Map and Gamebryo sections
    m_uiTextureFlags &= ~(NiTextureStage::TSTF_MAP_MASK | 
        NiTextureStage::TSTF_NDL_TYPEMASK);

    // Set shader mode and index
    m_uiTextureFlags |= (NiTextureStage::TSTF_MAP_SHADER | uiShaderMap);
}
//---------------------------------------------------------------------------
inline void NSBTexture::SetSourceObject(
    NiShaderAttributeDesc::ObjectType eObjectType,
    unsigned int uiObjectIndex)
{
    m_usObjTextureFlags = (unsigned short)(uiObjectIndex |
        (((unsigned int)eObjectType) << NiTextureStage::TSOTF_TYPE_SHIFT));
}
//---------------------------------------------------------------------------
