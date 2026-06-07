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

// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10Texture.h"
#include "NSBD3D10TextureStage.h"
#include "NSBD3D10Utility.h"

//---------------------------------------------------------------------------
NSBD3D10Texture::NSBD3D10Texture() :
    m_pcName(NULL),
    m_uiStage(0),
    m_uiTextureFlags(0),
    m_usObjTextureFlags(0)
{
    /* */
}
//---------------------------------------------------------------------------
NSBD3D10Texture::~NSBD3D10Texture()
{
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
void NSBD3D10Texture::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
bool NSBD3D10Texture::SetupTextureStage(NiD3D10Pass& kPass)
{
    unsigned int uiGBMap = 0;
    unsigned int uiGBMapInstance = 0;
    // Must reinterpret the texture flags because the D3D10 enumerations 
    // do not match the DX9 enumerations.
    bool bSuccess = NSBD3D10TextureStage::InterpretFlags(m_uiTextureFlags, 
        uiGBMap, uiGBMapInstance);
    if (!bSuccess)
        return false;

    unsigned int uiTextureId = kPass.GetFirstUnassignedTexture();

    kPass.SetGamebryoMap(uiTextureId, m_pcName, uiGBMap, 
        (unsigned short)uiGBMapInstance,
        m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Texture::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    NiStreamSaveBinary(kStream, m_uiStage);
    NiStreamSaveBinary(kStream, m_uiTextureFlags);
    NiStreamSaveBinary(kStream, m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Texture::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    NiStreamLoadBinary(kStream, m_uiStage);
    NiStreamLoadBinary(kStream, m_uiTextureFlags);
    NiStreamLoadBinary(kStream, m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Texture::CreateFromTextureStage(
    NSBD3D10TextureStage* pkTextureStage, const char* pcTextureName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcTextureName);
    m_uiStage = pkTextureStage->GetStage();
    m_uiTextureFlags = pkTextureStage->GetTextureFlags();
    m_usObjTextureFlags = pkTextureStage->GetObjTextureSettings();

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10Texture::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "Texture %s\n", m_pcName);
    NSBD3D10Utility::IndentInsert();

    NSBD3D10Utility::Dump(pf, true, "Stage         = %d\n", m_uiStage);
    NSBD3D10Utility::Dump(pf, true, "Texture Flags = 0x%08x\n", 
        m_uiTextureFlags);
    NSBD3D10Utility::Dump(pf, true, "Object Flags = 0x%04x\n", 
        m_usObjTextureFlags);

    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
