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


#include "NiMainPCH.h"  // Precompiled Header

#include "NiPersistentSrcTextureRendererData.h"
#include "NiSourceTexture.h"
#include "NiSourceCubeMap.h"

NiImplementRTTI(NiPersistentSrcTextureRendererData, NiTexture::RendererData);

bool NiPersistentSrcTextureRendererData::ms_bToolModeStreaming = false;

//---------------------------------------------------------------------------
NiPersistentSrcTextureRendererData::NiPersistentSrcTextureRendererData() :
    m_pucPixels(0),
    m_puiWidth(0),
    m_puiHeight(0),
    m_puiOffsetInBytes(0),
    m_uiPadOffsetInBytes(0),
    m_uiMipmapLevels(0),
    m_uiPixelStride(0),
    m_uiFaces(0),
    m_eTargetPlatform(NI_ANY),  // Generic case (default).
    m_pucPristinePixels(NULL),
    m_uiPristineMaxOffsetInBytes(0),
    m_uiPristinePadOffsetInBytes(0)
{
    m_pkTexture = NULL;
    m_uiWidth = 0;
    m_uiHeight = 0;
}
//---------------------------------------------------------------------------
NiPersistentSrcTextureRendererData::NiPersistentSrcTextureRendererData(
    NiSourceTexture* pkTexture, unsigned int uiWidth, unsigned int uiHeight,
    unsigned int uiFaces, unsigned int uiMipmapLevels,
    unsigned int uiPixelStride, NiPixelFormat& kPixelFormat, NiPalette* pkPal,
    PlatformID eTargetPlatform) :
    m_pucPixels(0),
    m_puiWidth(0),
    m_puiHeight(0),
    m_puiOffsetInBytes(0),
    m_uiPadOffsetInBytes(0),
    m_uiMipmapLevels(uiMipmapLevels),
    m_uiPixelStride(uiPixelStride),
    m_uiFaces(uiFaces),
    m_eTargetPlatform(eTargetPlatform),
    m_spPalette(pkPal)
{
    m_pkTexture = (NiTexture*)pkTexture;
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
    m_kPixelFormat = kPixelFormat;
}
//---------------------------------------------------------------------------
// Create a persistent (streamable) copy of the data
// (NiPersistentSourceTextureRendererData) using NiPixelData, and then remove
// NiPixelData.
NiPersistentSrcTextureRendererData* NiPersistentSrcTextureRendererData::Create(
    NiSourceTexture* pkTexture, const PlatformID eTargetPlatform)
{
    // Early out if NiPersistentSrcTextureRendererData exists already.
    if (pkTexture->GetPersistentSourceRendererData())
        return NULL; 

    // Create NiPersistentSrcTextureRendererData for eTargetPlatform.
    NiPersistentSrcTextureRendererData* pkThis =
        NiNew NiPersistentSrcTextureRendererData();
    if (!pkThis)
        return NULL;

    // It is safe to use our own regular pointers, as we know that the
    // image converter functions never assign incoming or outgoing data to
    // smart pointers.
    NiPixelData* pkSrcPixels = pkTexture->GetSourcePixelData();

    // We don't attempt to load a texure file if there is no NiPixelData, since
    // NiPersistentSrcTextureRendererData applies to embedded texures.
    if (!pkSrcPixels)
    {
        NiDelete pkThis;
        return NULL;
    }

    // This smart pointer exists to allow pkSrcPixels to be deleted if it needs
    // to be.
    NiPixelDataPtr spSrcPixels = pkSrcPixels;

    const NiTexture::FormatPrefs& kPrefs = pkTexture->GetFormatPreferences();
    const NiPixelFormat& kSrcFmt = pkSrcPixels->GetPixelFormat();

    NiDevImageConverter* pkConverter = NiDynamicCast(NiDevImageConverter,
        NiImageConverter::GetImageConverter());
    NIASSERT(pkConverter);
    if (!pkConverter)
    {
        NiDelete pkThis;
        return NULL;
    }

    const NiPixelFormat* pkDestFmt = pkThis->FindClosestPixelFormat(
        eTargetPlatform, kPrefs, kSrcFmt, pkConverter);
    NIASSERT(pkDestFmt);
    if (!pkDestFmt)
    {
        NiDelete pkThis;
        return NULL;
    }

    NiPixelData* pkFormatted = pkConverter->ConvertPixelData(*pkSrcPixels,
        *pkDestFmt, pkSrcPixels, (pkSrcPixels->GetNumMipmapLevels() > 1));
    if (!pkFormatted)
    {
        NiDelete pkThis;
        return NULL;
    }

    // Update texture attributes in the NiPersistentSrcRendererData.
    pkThis->m_kPixelFormat = pkFormatted->GetPixelFormat();

    pkThis->m_eTargetPlatform = eTargetPlatform;
    pkThis->m_uiWidth = pkFormatted->GetWidth();
    pkThis->m_uiHeight = pkFormatted->GetHeight();
    pkThis->m_uiMipmapLevels = pkFormatted->GetNumMipmapLevels();
    pkThis->m_uiPixelStride = pkFormatted->GetPixelStride();
    pkThis->m_uiFaces = pkFormatted->GetNumFaces();

    // Attach palette if the texture is palettized.  Cube maps only support a
    // single palette shared by all faces.  Since all faces of the NiPixelData
    // object share a single palette, use the NiPixelData palette in that case.
    pkThis->m_spPalette = pkFormatted->GetPalette();

    unsigned int auiWidth[16], auiHeight[16], auiOffsetInBytes[16];
    for (unsigned int i = 0; i < pkThis->m_uiMipmapLevels; i++)
    {
        auiWidth[i] = pkFormatted->GetWidth(i, 0);
        auiHeight[i] = pkFormatted->GetHeight(i, 0);
        auiOffsetInBytes[i] = (unsigned int)(pkFormatted->GetOffsetInBytes(i, 0));
    }

    unsigned int uiFaceSizeInBytes = 
        (unsigned int)(pkFormatted->GetFaceSizeInBytes());
    auiOffsetInBytes[pkThis->m_uiMipmapLevels] = uiFaceSizeInBytes;
    pkThis->m_uiPadOffsetInBytes = uiFaceSizeInBytes;

    pkThis->AllocateData(pkThis->m_uiMipmapLevels, pkThis->m_uiFaces,
        uiFaceSizeInBytes);

    unsigned int uiDestSize = pkThis->m_uiMipmapLevels << 2;

    NiMemcpy(pkThis->m_puiWidth, &auiWidth, uiDestSize);
    NiMemcpy(pkThis->m_puiHeight, &auiHeight, uiDestSize);
    
    uiDestSize = (pkThis->m_uiMipmapLevels + 1) << 2;

    NiMemcpy(pkThis->m_puiOffsetInBytes, &auiOffsetInBytes, uiDestSize);

    for (unsigned int uiFace=0; uiFace < pkThis->m_uiFaces; uiFace++)
    {
        NiMemcpy((pkThis->m_pucPixels + (uiFace * uiFaceSizeInBytes)),
            pkFormatted->GetPixels(0, uiFace), uiFaceSizeInBytes);
    }

    pkThis->SetTexture(pkTexture);

    pkTexture->SetPersistentSourceRendererData(pkThis);
    pkTexture->SetSourceRendererDataIsPersistent(true);

    // There's no longer a need to hold on to the NiPixelData.  Clean up here
    // so it won't be streamed out.
    pkTexture->DestroyAppPixelData();
    if (pkFormatted->GetRefCount() == 0)
        NiDelete(pkFormatted);

    return pkThis;
}
//---------------------------------------------------------------------------
NiPersistentSrcTextureRendererData::~NiPersistentSrcTextureRendererData()
{
    m_spPalette = 0;
    FreeData();

    // If in "tool mode streaming" mode, pristine copies of data must be freed.
    if (ms_bToolModeStreaming)
        if (m_pucPristinePixels)
            NiFree(m_pucPristinePixels);
}
//---------------------------------------------------------------------------
// Determine desired pixel format by platform.
const NiPixelFormat*
    NiPersistentSrcTextureRendererData::FindClosestPixelFormat(
    const PlatformID eTargetPlatform, const NiTexture::FormatPrefs& kPrefs,
    const NiPixelFormat& kSrcFmt, NiDevImageConverter* pkConverter)
{
    switch (eTargetPlatform)
    {
        case NI_XENON:
            return pkConverter->FindClosestPixelFormat(
                NiDevImageConverter::XENON, kPrefs, kSrcFmt, kSrcFmt);
        case NI_PS3:
            return pkConverter->FindClosestPixelFormat(
                NiDevImageConverter::PLAYSTATION3, kPrefs, kSrcFmt, kSrcFmt);
        case NI_WII:
            return pkConverter->FindClosestPixelFormat(
                NiDevImageConverter::WII, kPrefs, kSrcFmt, kSrcFmt);
        case NI_D3D10:
            {
            NiDevImageConverter::Platform ePlatform = NiDevImageConverter::D3D10;
            bool bSrcAlpha = kSrcFmt.GetPalettized() ?
                kSrcFmt.GetPaletteHasAlpha() :
                kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0;

            if (kSrcFmt.GetBitsPerPixel() >= 64)
            {
                return pkConverter->FindClosestPixelFormat(ePlatform, kPrefs,
                    kSrcFmt, kSrcFmt);
            }
            else if (kSrcFmt.GetBitsPerPixel() == 16)
            {
                // On D3D10, 16 bit means either 5551 or 565. 
                if (kSrcFmt.GetCompressed() || bSrcAlpha)
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::RGBA5551);
                }
                else
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::BGR565);
                }
            }
            else
            {
                if (kSrcFmt.GetCompressed())
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, kSrcFmt);
                }
                else
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::BGRA8888);
                }
            }
            }
            break;
        case NI_DX9:
            {
            NiDevImageConverter::Platform ePlatform = NiDevImageConverter::DX9;
            bool bSrcAlpha = kSrcFmt.GetPalettized() ?
                kSrcFmt.GetPaletteHasAlpha() :
                kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0;

            if (kSrcFmt.GetBitsPerPixel() >= 64)
            {
                return pkConverter->FindClosestPixelFormat(ePlatform, kPrefs,
                    kSrcFmt, kSrcFmt);
            }
            else if (kSrcFmt.GetBitsPerPixel() == 16)
            {
                // Need to distinguish BGRA4444 from BGRX4444 for DX9, but
                // BGRX4444 is preferable to alternatives for virtually all
                // graphics cards.
                if (kSrcFmt.GetCompressed() || bSrcAlpha)
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::BGRA4444);
                }
                else
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::BGRX4444);
                }
            }
            else
            {
                if (kSrcFmt.GetCompressed())
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, kSrcFmt);
                }
                // Need to distinguish BGRA8888 from BGRX8888 for DX9, but
                // BGRX8888 is preferable to BGR888 for virtually all graphics
                // cards.
                else if (bSrcAlpha)
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::BGRA8888);
                }
                else
                {
                    return pkConverter->FindClosestPixelFormat(ePlatform,
                        kPrefs, kSrcFmt, NiPixelFormat::BGRX8888);
                }
            }
            }
        default:
            break;
    }

    return NULL;
}
//---------------------------------------------------------------------------
// This base class generic version of AllocateData() is platform-agnostic.
void NiPersistentSrcTextureRendererData::AllocateData(unsigned int uiLevels,
    unsigned int uiFaces, unsigned int uiBytes)
{
    NIASSERT(uiFaces > 0);
    NIASSERT(uiLevels > 0);

    // Rounding up for multiple of 4 for pointer allignment.
    uiBytes = (uiBytes + 3) & ~3;

    // uiLevels * 3     (Levels for each:  Width, Height, OffsetInBytes)
    // << 2             ( * 4 to make room for unsigned int )
    // + uiBytes        ( + the space allocated for pixels)
    // * uiFaces        ( * the number of faces for this pixel data)
#if defined(_WII)
    // Textures have to be 32-byte aligned on the Wii.
    m_pucPixels = NiAlignedAlloc2(unsigned char,
        ((uiLevels * 3 + 1) << 2) + (uiBytes * uiFaces), 32, NiMemHint::TEXTURE);
#else
    m_pucPixels = NiAlloc2(unsigned char,
        ((uiLevels * 3 + 1) << 2) + (uiBytes * uiFaces), NiMemHint::TEXTURE);
#endif
    NIASSERT(m_pucPixels);

    m_puiWidth = (unsigned int*)(m_pucPixels + (uiBytes * uiFaces));
    m_puiHeight = m_puiWidth + uiLevels;
    m_puiOffsetInBytes = m_puiHeight + uiLevels;
}
//---------------------------------------------------------------------------
void NiPersistentSrcTextureRendererData::FreeData()
{
    if (!m_pucPixels)
        return;

#if defined(_WII)
    // Textures have to be 32-byte aligned on the Wii.
    NiAlignedFree(m_pucPixels);
#else
    NiFree(m_pucPixels);
#endif
    m_puiOffsetInBytes = 0;
    m_puiHeight = 0;
    m_puiWidth = 0;
    m_pucPixels = 0;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPersistentSrcTextureRendererData);
//---------------------------------------------------------------------------
void NiPersistentSrcTextureRendererData::LoadBinary(NiStream& kStream)
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
        m_pucPristinePixels = NiAlloc(unsigned char, uiTotalPixelMemory);
        NIASSERT(m_pucPristinePixels);
        NiMemcpy(m_pucPristinePixels, m_pucPixels, uiTotalPixelMemory);
    }
}
//---------------------------------------------------------------------------
void NiPersistentSrcTextureRendererData::LinkObject(NiStream& kStream)
{
    NiTexture::RendererData::LinkObject(kStream);

    m_spPalette = (NiPalette*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPersistentSrcTextureRendererData::RegisterStreamables(NiStream& kStream)
{
    if (!NiTexture::RendererData::RegisterStreamables(kStream))
        return false;

    if (m_spPalette)
        m_spPalette->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPersistentSrcTextureRendererData::SaveBinary(NiStream& kStream)
{
    NiTexture::RendererData::SaveBinary(kStream);

    bool bToolModeStreamingValid = ms_bToolModeStreaming && 
        (m_uiPristineMaxOffsetInBytes != 0) &&
        (m_uiPristinePadOffsetInBytes != 0) && (m_pucPristinePixels != NULL);

    m_kPixelFormat.SaveBinary(kStream);

    kStream.SaveLinkID(m_spPalette);
    NiStreamSaveBinary(kStream, m_uiMipmapLevels);
    NiStreamSaveBinary(kStream, m_uiPixelStride);

    for (unsigned int i = 0; i < m_uiMipmapLevels; i++)
    {
        NiStreamSaveBinary(kStream, m_puiWidth[i]);
        NiStreamSaveBinary(kStream, m_puiHeight[i]);
        NiStreamSaveBinary(kStream, m_puiOffsetInBytes[i]);
    }

    // If in "tool mode streaming" mode, pristine copies of data must be saved
    // to guarantee what will be streamed out matches what has been streamed
    // in.
    if (bToolModeStreamingValid)
    {
        NiStreamSaveBinary(kStream, m_uiPristineMaxOffsetInBytes);
        NiStreamSaveBinary(kStream, m_uiPristinePadOffsetInBytes);
    }
    else
    {
        NiStreamSaveBinary(kStream, m_puiOffsetInBytes[m_uiMipmapLevels]);
        NiStreamSaveBinary(kStream, m_uiPadOffsetInBytes);
    }

    NiStreamSaveBinary(kStream, m_uiFaces);

    NiStreamSaveEnum(kStream, m_eTargetPlatform);

    if (bToolModeStreamingValid)
    {
        NiStreamSaveBinary(kStream, m_pucPristinePixels, 
            m_uiPristineMaxOffsetInBytes * m_uiFaces);
    }
    else
    {
        NiStreamSaveBinary(kStream, m_pucPixels, 
            m_puiOffsetInBytes[m_uiMipmapLevels] * m_uiFaces);
    }
}
//---------------------------------------------------------------------------
bool NiPersistentSrcTextureRendererData::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPersistentSrcTextureRendererData, pkObject));
    if (!NiIsKindOf(NiPersistentSrcTextureRendererData, pkObject))
        return false;

    if (!NiObject::IsEqual(pkObject))
        return false;

    NiPersistentSrcTextureRendererData* pkPersistentSrcTextureRendererData =
        (NiPersistentSrcTextureRendererData*) pkObject;

    if (m_kPixelFormat != pkPersistentSrcTextureRendererData->m_kPixelFormat)
        return false;

    if (m_spPalette &&
        !m_spPalette->IsEqual(pkPersistentSrcTextureRendererData->m_spPalette))
    {
        return false;
    }

    if ((m_uiMipmapLevels !=
        pkPersistentSrcTextureRendererData->m_uiMipmapLevels) ||
        (m_uiPixelStride !=
        pkPersistentSrcTextureRendererData->m_uiPixelStride))
    {
        return false;
    }

    for (unsigned int i = 0; i < m_uiMipmapLevels; i++)
    {
        if ((m_puiWidth[i] !=
            pkPersistentSrcTextureRendererData->m_puiWidth[i]) ||
            (m_puiHeight[i] !=
            pkPersistentSrcTextureRendererData->m_puiHeight[i]) ||
            (m_puiOffsetInBytes[i] !=
            pkPersistentSrcTextureRendererData->m_puiOffsetInBytes[i]))
        {
            return false;
        }
    }

    if (m_puiOffsetInBytes[m_uiMipmapLevels] !=
        pkPersistentSrcTextureRendererData->
        m_puiOffsetInBytes[m_uiMipmapLevels]) 
    {
        return false;
    }

    if (m_uiPadOffsetInBytes !=
        pkPersistentSrcTextureRendererData->m_uiPadOffsetInBytes) 
    {
        return false;
    }

    if (m_uiFaces != pkPersistentSrcTextureRendererData->m_uiFaces) 
        return false;

    if (m_eTargetPlatform !=
        pkPersistentSrcTextureRendererData->m_eTargetPlatform) 
    {
        return false;
    }

    if (m_pucPixels && pkPersistentSrcTextureRendererData->m_pucPixels)
    {
        if (memcmp(m_pucPixels,
            pkPersistentSrcTextureRendererData->m_pucPixels,
            m_puiOffsetInBytes[m_uiMipmapLevels]) != 0)
        {
            return false;
        }
    }

    // If in "tool mode streaming" mode, pristine copies of data must be saved
    // to guarantee what will be streamed out matches what has been streamed
    // in.  If that step has been done, IsEqual() needs to consider it, too.
    if (ms_bToolModeStreaming)
    {
        if (m_pucPristinePixels &&
            pkPersistentSrcTextureRendererData->m_pucPristinePixels)
        {
            if (memcmp(m_pucPristinePixels,
                pkPersistentSrcTextureRendererData->m_pucPristinePixels,
                m_uiPristineMaxOffsetInBytes) != 0)
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPersistentSrcTextureRendererData::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    ///NiObject::GetViewerStrings(pkStrings);
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPersistentSrcTextureRendererData::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("Width", GetWidth()));
    pkStrings->Add(NiGetViewerString("Height", GetHeight()));
    pkStrings->Add(NiGetViewerString("MipmapLevels", m_uiMipmapLevels));
    pkStrings->Add(NiGetViewerString("Faces", m_uiFaces));
    pkStrings->Add(NiGetViewerString("TotalSizeInBytes",
        (unsigned int)GetTotalSizeInBytes()));
    pkStrings->Add(NiGetViewerString("FaceSizeInBytes", 
        (unsigned int)GetFaceSizeInBytes()));

    switch (m_eTargetPlatform)
    {
        case NI_ANY:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "NI_ANY - Generic (default)"));
            break;
        case NI_DX9:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "NI_DX9"));
            break;
        case NI_D3D10:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "NI_D3D10"));
            break;
        case NI_XENON:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "NI_XENON"));
            break;
        case NI_PS3:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "NI_PS3"));
            break;
        case NI_WII:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "NI_WII"));
            break;
        default:
            pkStrings->Add(NiGetViewerString("TargetPlatform",
                "UNKNOWN!"));
            break;
    }
}
//---------------------------------------------------------------------------
void NiPersistentSrcTextureRendererData::ResetStreamingFunctions()
{
    NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
    NiStream::RegisterLoader("NiPersistentSrcTextureRendererData", 
        NiPersistentSrcTextureRendererData::CreateObject);
}
//---------------------------------------------------------------------------


