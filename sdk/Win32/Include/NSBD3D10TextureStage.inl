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
#include "NSBD3D10Utility.h"

//---------------------------------------------------------------------------
// NSBD3D10TextureStage inline functions
//---------------------------------------------------------------------------
inline const char* NSBD3D10TextureStage::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10TextureStage::GetStage()
{
    return m_uiStage;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetStage(unsigned int uiStage)
{
    m_uiStage = uiStage;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10TextureStage::GetTextureFlags()
{
    return m_uiTextureFlags;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetNDLMap(unsigned int uiNDLMap)
{
    // Clear the DECAL and SHADER modes
    m_uiTextureFlags &= ~NiD3D10Pass::GB_MAP_TYPEMASK;

    // Set the given map flag
    m_uiTextureFlags |= uiNDLMap;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetDecalMap(unsigned int uiDecal)
{
    // Clear the Map and Gamebryo sections
    m_uiTextureFlags &= ~NiD3D10Pass::GB_MAP_TYPEMASK;

    // Set decal mode and index.
    m_uiTextureFlags |= (NiD3D10Pass::GB_MAP_DECAL | uiDecal);
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetShaderMap(unsigned int uiShaderMap)
{
    // Clear the Map and Gamebryo sections
    m_uiTextureFlags &= ~NiD3D10Pass::GB_MAP_TYPEMASK;

    // Set shader mode and index
    m_uiTextureFlags |= (NiD3D10Pass::GB_MAP_SHADER | uiShaderMap);
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetUseIndexFromMap(bool bUse)
{
    if (bUse)
        m_uiTextureFlags |= MAP_USE_INDEX;
    else
        m_uiTextureFlags &= ~MAP_USE_INDEX;
}
//---------------------------------------------------------------------------
inline bool NSBD3D10TextureStage::GetUseTextureTransformation()
{
    return m_bTextureTransform;
}
//---------------------------------------------------------------------------
inline D3DMATRIX& NSBD3D10TextureStage::GetTextureTransformation()
{
    return m_kTextureTransformation;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetUseTextureTransformation(bool bUse)
{
    m_bTextureTransform = bUse;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetTextureTransformation(D3DMATRIX& kTrans)
{
    m_kTextureTransformation = kTrans;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10TextureStage::GetTextureTransformFlags()
{
    return m_uiTextureTransformFlags;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetTextureTransformFlags(
    unsigned int uiFlags)
{
    m_uiTextureTransformFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10TextureStage::GetGlobalName()
{
    return m_pcGlobalEntry;
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetGlobalName(const char* pcGlobalName)
{
    NSBD3D10Utility::SetString(m_pcGlobalEntry, 0, pcGlobalName);
}
//---------------------------------------------------------------------------
inline void NSBD3D10TextureStage::SetObjTextureSettings(
    NiShaderAttributeDesc::ObjectType eObjectType,
    unsigned int uiObjectIndex)
{
    m_usObjTextureFlags = (unsigned short)(((unsigned short) eObjectType) <<
        NiD3D10Pass::OTF_TYPE_SHIFT) | ((unsigned short) uiObjectIndex);
}
//---------------------------------------------------------------------------
inline unsigned short NSBD3D10TextureStage::GetObjTextureSettings()
{
    return m_usObjTextureFlags;
}
//---------------------------------------------------------------------------
