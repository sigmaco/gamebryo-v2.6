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
inline const char* NSBD3D10Texture::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Texture::GetStage()
{
    return m_uiStage;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Texture::SetStage(unsigned int uiStage)
{
    m_uiStage = uiStage;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Texture::GetTextureFlags()
{
    return m_uiTextureFlags;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Texture::SetSource(unsigned int uiNDLMap)
{
    // Clear the DECAL and SHADER modes
    m_uiTextureFlags &= ~NiD3D10Pass::GB_MAP_TYPEMASK;

    // Set the given map flag
    m_uiTextureFlags |= uiNDLMap;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Texture::SetSourceDecal(unsigned int uiDecal)
{
    // Clear the Map and Gamebryo sections
    m_uiTextureFlags &= ~NiD3D10Pass::GB_MAP_TYPEMASK;

    // Set decal mode and index.
    m_uiTextureFlags |= (NiD3D10Pass::GB_MAP_DECAL | uiDecal);
}
//---------------------------------------------------------------------------
inline void NSBD3D10Texture::SetSourceShader(unsigned int uiShaderMap)
{
    // Clear the Map and Gamebryo sections
    m_uiTextureFlags &= ~NiD3D10Pass::GB_MAP_TYPEMASK;

    // Set shader mode and index
    m_uiTextureFlags |= (NiD3D10Pass::GB_MAP_SHADER | uiShaderMap);
}
//---------------------------------------------------------------------------
inline void NSBD3D10Texture::SetSourceObject(
    NiShaderAttributeDesc::ObjectType eObjectType,
    unsigned int uiObjectIndex)
{
    m_usObjTextureFlags = (unsigned short)(((unsigned short) eObjectType) <<
        NiD3D10Pass::OTF_TYPE_SHIFT) | ((unsigned short) uiObjectIndex);
}
//---------------------------------------------------------------------------
