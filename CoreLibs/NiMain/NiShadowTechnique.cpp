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
#include "NiMainPCH.h"
#include "NiShadowGenerator.h"
#include "NiShadowTechnique.h"

NiImplementRootRTTI(NiShadowTechnique);

//---------------------------------------------------------------------------
NiShadowTechnique::NiShadowTechnique(const NiFixedString& kName,
    const NiFixedString kReadFragmentName, 
    const NiFixedString kWriteFragmentName, bool, bool)
{
    NiShadowTechnique(kName, 
        kReadFragmentName, kReadFragmentName, kReadFragmentName,
        kWriteFragmentName, kWriteFragmentName, kWriteFragmentName,
        m_bUseCubeMapForPointLight, m_bWriteBatchable);
}
//---------------------------------------------------------------------------
NiShadowTechnique::NiShadowTechnique(const NiFixedString& kName, 
    const NiFixedString kDirReadFragmentName, 
    const NiFixedString kPointReadFragmentName, 
    const NiFixedString kSpotReadFragmentName, 
    const NiFixedString kDirWriteFragmentName, 
    const NiFixedString kPointWriteFragmentName, 
    const NiFixedString kSpotWriteFragmentName, 
    bool bUseCubeMapForPointLight,
    bool bWriteBatchable) : 
    m_uiActiveTechniqueSlot(INVALID_SHADOWTECHNIQUE_ID), 
    m_usTechniqueID(INVALID_SHADOWTECHNIQUE_ID),
    m_bWriteBatchable(bWriteBatchable),
    m_bUseCubeMapForPointLight(bUseCubeMapForPointLight),
    m_pkDepthFormat(0)
{
    m_kName = kName;
    m_kFormatPrefs.m_ePixelLayout = NiTexture::FormatPrefs::SINGLE_COLOR_32;
    m_kFormatPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;

    for (int iIndex = 0; iIndex < NiStandardMaterial::LIGHT_MAX * 2; iIndex++)
    {
        m_afDefaultDepthBias[iIndex] = 0.0f;
    }

    m_kReadFragmentName[NiStandardMaterial::LIGHT_DIR] = 
        kDirReadFragmentName;
    m_kReadFragmentName[NiStandardMaterial::LIGHT_POINT] = 
        kPointReadFragmentName;
    m_kReadFragmentName[NiStandardMaterial::LIGHT_SPOT] = 
        kSpotReadFragmentName;

    m_kWriteFragmentName[NiStandardMaterial::LIGHT_DIR] = 
        kDirWriteFragmentName;
    m_kWriteFragmentName[NiStandardMaterial::LIGHT_POINT] = 
        kPointWriteFragmentName;
    m_kWriteFragmentName[NiStandardMaterial::LIGHT_SPOT] = 
        kSpotWriteFragmentName;
}
//---------------------------------------------------------------------------
