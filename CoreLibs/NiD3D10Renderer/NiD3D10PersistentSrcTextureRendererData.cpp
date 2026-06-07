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
#include "NiD3D10RendererPCH.h"

#include <NiImageConverter.h>

#include "NiD3D10PersistentSrcTextureRendererData.h"
#include "NiD3D10Renderer.h"

NiImplementRTTI(NiD3D10PersistentSrcTextureRendererData,
    NiPersistentSrcTextureRendererData);

//---------------------------------------------------------------------------
NiD3D10PersistentSrcTextureRendererData::NiD3D10PersistentSrcTextureRendererData()
{
}
//---------------------------------------------------------------------------
NiD3D10PersistentSrcTextureRendererData::~NiD3D10PersistentSrcTextureRendererData()
{
    // Defer to base class.
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiD3D10PersistentSrcTextureRendererData);
//---------------------------------------------------------------------------
void NiD3D10PersistentSrcTextureRendererData::LoadBinary(NiStream& kStream)
{
    
    

    NiTexture::RendererData::LoadBinary(kStream);
    
    m_kPixelFormat.LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spPalette
    NiStreamLoadBinary(kStream, m_uiMipmapLevels);
    NiStreamLoadBinary(kStream, m_uiPixelStride);

    unsigned int auiWidth[16], auiHeight[16], auiOffsetInBytes[16];

    for (unsigned int i = 0; i < m_uiMipmapLevels; i++)
    {
        NiStreamLoadBinary(kStream, auiWidth[i]);
        NiStreamLoadBinary(kStream, auiHeight[i]);
        NiStreamLoadBinary(kStream, auiOffsetInBytes[i]);
    }

    NiStreamLoadBinary(kStream, auiOffsetInBytes[m_uiMipmapLevels]);

    if ((kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 6)))
    {
        m_uiPadOffsetInBytes = 0;
        NiStreamLoadBinary(kStream, m_uiPadOffsetInBytes);
    }
    else
    {
        NiMemcpy(&m_uiPadOffsetInBytes, &auiOffsetInBytes[m_uiMipmapLevels],
            4);
    }

    NiStreamLoadBinary(kStream, m_uiFaces);

    NiStreamLoadEnum(kStream, m_eTargetPlatform);
    
    AllocateData(m_uiMipmapLevels, m_uiFaces,
        auiOffsetInBytes[m_uiMipmapLevels]);

    unsigned int uiDestSize =  m_uiMipmapLevels << 2;
    NiMemcpy(m_puiWidth, &auiWidth, uiDestSize);
    NiMemcpy(m_puiHeight, &auiHeight, uiDestSize);

    uiDestSize = (m_uiMipmapLevels + 1) << 2;
    NiMemcpy(m_puiOffsetInBytes, &auiOffsetInBytes, uiDestSize);

    NiStreamLoadBinary(kStream, m_pucPixels, 
        m_puiOffsetInBytes[m_uiMipmapLevels] * m_uiFaces);

    // If in "tool mode streaming" mode, pristine copies of data must be saved
    // to guarantee what will be streamed out matches what has been streamed
    // in.
    if (ms_bToolModeStreaming)
    {
        m_uiPristineMaxOffsetInBytes = m_puiOffsetInBytes[m_uiMipmapLevels];
        m_uiPristinePadOffsetInBytes = m_uiPadOffsetInBytes;
        unsigned int uiTotalPixelMemory =
            m_uiPristineMaxOffsetInBytes * m_uiFaces;
        m_pucPristinePixels = NiAlloc2(unsigned char, uiTotalPixelMemory,NiMemHint::TEXTURE);
        NIASSERT(m_pucPristinePixels);
        NiMemcpy(m_pucPristinePixels, m_pucPixels, uiTotalPixelMemory);
    }
}
//---------------------------------------------------------------------------
void NiD3D10PersistentSrcTextureRendererData::LinkObject(NiStream& kStream)
{
    
    
    NiPersistentSrcTextureRendererData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiD3D10PersistentSrcTextureRendererData::RegisterStreamables(
    NiStream& kStream)
{
    
    
    if (!NiPersistentSrcTextureRendererData::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10PersistentSrcTextureRendererData::SaveBinary(NiStream& kStream)
{
    
    
    NiPersistentSrcTextureRendererData::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiD3D10PersistentSrcTextureRendererData::GetStreamableRTTIName(char* acName,
    unsigned int uiMaxSize) const
{
    
    
    return NiPersistentSrcTextureRendererData::GetRTTI()->CopyName(acName,
        uiMaxSize);
}
//---------------------------------------------------------------------------
bool NiD3D10PersistentSrcTextureRendererData::IsEqual(NiObject* pkObject)
{
    
    
    NIASSERT(NiIsKindOf(NiD3D10PersistentSrcTextureRendererData, pkObject));
    if (!NiIsKindOf(NiD3D10PersistentSrcTextureRendererData, pkObject))
        return false;

    if (!NiPersistentSrcTextureRendererData::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10PersistentSrcTextureRendererData::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    
    
    NiPersistentSrcTextureRendererData::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiD3D10PersistentSrcTextureRendererData::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
