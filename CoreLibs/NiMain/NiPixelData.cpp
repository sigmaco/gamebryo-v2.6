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

#include "NiPixelData.h"

NiImplementRTTI(NiPixelData, NiObject);

//---------------------------------------------------------------------------
NiPixelData::NiPixelData()
{
    m_pucPixels = 0;
    m_puiWidth = 0;
    m_puiHeight = 0;
    m_puiOffsetInBytes = 0;
    m_uiMipmapLevels = 0;
    m_uiFaces = 0;
    m_uiPixelStride = 0;

    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;
}
//---------------------------------------------------------------------------
NiPixelData::NiPixelData(const NiPixelData& kSrc)
: NiObject()
{
    m_kPixelFormat = kSrc.m_kPixelFormat;

    if (kSrc.m_spPalette)
        m_spPalette = NiNew NiPalette(*kSrc.m_spPalette);

    m_uiMipmapLevels = kSrc.m_uiMipmapLevels;
    m_uiFaces = kSrc.m_uiFaces;
    m_uiPixelStride = kSrc.m_uiPixelStride;

    unsigned int auiWidth[16], auiHeight[16], auiOffsetInBytes[16];

    for (unsigned int i = 0; i < m_uiMipmapLevels; i++)
    {
        auiWidth[i] = kSrc.m_puiWidth[i];
        auiHeight[i] = kSrc.m_puiHeight[i];
        auiOffsetInBytes[i] = kSrc.m_puiOffsetInBytes[i];
    }

    unsigned int uiSizeInBytes = kSrc.m_puiOffsetInBytes[m_uiMipmapLevels];

    auiOffsetInBytes[m_uiMipmapLevels] = uiSizeInBytes;

    AllocateData(m_uiMipmapLevels, m_uiFaces, uiSizeInBytes);

    unsigned int uiDestSize = m_uiMipmapLevels << 2;
    NiMemcpy(m_puiWidth, &auiWidth, uiDestSize);
    NiMemcpy(m_puiHeight, &auiHeight, uiDestSize);
    
    uiDestSize = (m_uiMipmapLevels + 1) << 2;
    NiMemcpy(m_puiOffsetInBytes, &auiOffsetInBytes, uiDestSize);

    NiMemcpy(m_pucPixels, kSrc.m_pucPixels, uiSizeInBytes);

    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;
}
//---------------------------------------------------------------------------
NiPixelData::NiPixelData(unsigned int uiWidth, unsigned int uiHeight,
    const NiPixelFormat& kFormat, unsigned int uiMipmapLevels,
    unsigned int uiFaces)
{
    m_kPixelFormat = kFormat;

    NIASSERT(uiMipmapLevels <= ComputeMipmapLevels(uiWidth, uiHeight));

    if (uiMipmapLevels == 0)
        m_uiMipmapLevels = ComputeMipmapLevels(uiWidth, uiHeight);
    else
        m_uiMipmapLevels = uiMipmapLevels;
    m_uiFaces = uiFaces;

    unsigned int auiWidth[16], auiHeight[16], auiOffsetInBytes[16];
    
    auiOffsetInBytes[0] = 0;

    unsigned int i;
    if (!kFormat.GetCompressed())
    {
        m_uiPixelStride = kFormat.GetBitsPerPixel() >> 3;

        if (m_uiPixelStride)
        {
            for (i = 0; i < m_uiMipmapLevels; i++)
            {
                auiWidth[i] = uiWidth ? uiWidth : 1;
                auiHeight[i] = uiHeight ? uiHeight : 1;
                auiOffsetInBytes[i + 1] = auiOffsetInBytes[i] + 
                    m_uiPixelStride * auiWidth[i] * auiHeight[i];

                uiWidth >>= 1;
                uiHeight >>= 1;
            }
        }
        else
        {
            // Assume this is a 4 bit representation
            // since pixel stride came out to be zero.
            for (i = 0; i < m_uiMipmapLevels; i++)
            {
                auiWidth[i] = uiWidth ? uiWidth : 1;
                auiHeight[i] = uiHeight ? uiHeight : 1;
                auiOffsetInBytes[i + 1] = auiOffsetInBytes[i] + 
                    (auiWidth[i] * auiHeight[i] >> 1);

                uiWidth >>= 1;
                uiHeight >>= 1;
            }
        }

    }
    else
    {
        m_uiPixelStride = 0;

        // bytes per block
        unsigned int uiBPB;
        if (kFormat.GetFormat() == NiPixelFormat::FORMAT_DXT1)
            uiBPB = 8; // format uses 64 bits per 4x4 pixel block
        else
            uiBPB = 16; // format uses 128 bits per 4x4 pixel block

        for (i = 0; i < m_uiMipmapLevels; i++)
        {
            auiWidth[i] = uiWidth ? uiWidth : 1;
            auiHeight[i] = uiHeight ? uiHeight : 1;

            // Size of mipmap level is width rounded up to 4 times height
            // rounded up to 4 divided by 16 pixels per block times bits per 
            // block
            auiOffsetInBytes[i + 1] = auiOffsetInBytes[i] + 
                (((((auiWidth[i] + 3) & ~0x3) 
                * ((auiHeight[i] + 3) & ~0x3)) >> 4) * uiBPB);

            uiWidth >>= 1;
            uiHeight >>= 1;
        }
    }

    AllocateData(m_uiMipmapLevels, m_uiFaces,
        auiOffsetInBytes[m_uiMipmapLevels]);

    unsigned int uiDestSize = m_uiMipmapLevels << 2;
    NiMemcpy(m_puiWidth, &auiWidth, uiDestSize);
    NiMemcpy(m_puiHeight, &auiHeight, uiDestSize);

    uiDestSize = (m_uiMipmapLevels + 1) << 2;
    NiMemcpy(m_puiOffsetInBytes, &auiOffsetInBytes, uiDestSize);

    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;
}
//---------------------------------------------------------------------------
NiPixelData::~NiPixelData()
{
    m_spPalette = 0;
    FreeData();
}
//---------------------------------------------------------------------------
const unsigned char* NiPixelData::operator()(unsigned int uiX, 
    unsigned int uiY, unsigned int uiMipmapLevel, unsigned int uiFace) const
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    if (m_kPixelFormat.GetCompressed())
    {
        return 0;
    }
    else
    {
        return m_pucPixels + m_puiOffsetInBytes[uiMipmapLevel] +
            uiY * m_puiWidth[uiMipmapLevel] * m_uiPixelStride +
            uiX * m_uiPixelStride + 
            uiFace*m_puiOffsetInBytes[m_uiMipmapLevels];
    }
}
//---------------------------------------------------------------------------
unsigned char* NiPixelData::operator()(unsigned int uiX, unsigned int uiY, 
    unsigned int uiMipmapLevel, unsigned int uiFace)
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    if (m_kPixelFormat.GetCompressed())
    {
        return 0;
    }
    else
    {
        return m_pucPixels + m_puiOffsetInBytes[uiMipmapLevel] +
            uiY * m_puiWidth[uiMipmapLevel] * m_uiPixelStride +
            uiX * m_uiPixelStride + 
            uiFace*m_puiOffsetInBytes[m_uiMipmapLevels];
    }
}
//---------------------------------------------------------------------------
unsigned int NiPixelData::ComputeMipmapLevels(unsigned int uiWidth, 
    unsigned int uiHeight)
{
    // Returns 0 if the size is not a valid image (zeros)

    if (!uiWidth || !uiHeight)
        return 0;

    // Can have a mipmap with non-power-of-two dimensions if it is supplied
    // with the texture.  (Mipmaps for non-power-of-two dimensions will not be
    // generated automatically.)
    unsigned int uiWidthMipmapLevels = 1;
    while (uiWidth >> 1)
    {
        uiWidth >>= 1;
        uiWidthMipmapLevels++;
    }
    unsigned int uiHeightMipmapLevels = 1;
    while (uiHeight >> 1)
    {
        uiHeight >>= 1;
        uiHeightMipmapLevels++;
    }

    return (uiHeightMipmapLevels < uiWidthMipmapLevels) ? 
        uiWidthMipmapLevels : uiHeightMipmapLevels;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPixelData);
//---------------------------------------------------------------------------
void NiPixelData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    m_kPixelFormat.LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spPalette
    NiStreamLoadBinary(kStream, m_uiMipmapLevels);
    NiStreamLoadBinary(kStream, m_uiPixelStride);

    unsigned int auiWidth[16], auiHeight[16], auiOffsetInBytes[16];

    NIASSERT(m_uiMipmapLevels <= 16);
    for (unsigned int i = 0; i < m_uiMipmapLevels; i++)
    {
        NiStreamLoadBinary(kStream, auiWidth[i]);
        NiStreamLoadBinary(kStream, auiHeight[i]);
        NiStreamLoadBinary(kStream, auiOffsetInBytes[i]);
    }

    NiStreamLoadBinary(kStream, auiOffsetInBytes[m_uiMipmapLevels]);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 6))
    {
        m_uiFaces = 1;
    }
    else
    {
        NiStreamLoadBinary(kStream, m_uiFaces);
    }


    AllocateData(m_uiMipmapLevels, m_uiFaces, 
        auiOffsetInBytes[m_uiMipmapLevels]);
    
    unsigned int uiDestSize =  m_uiMipmapLevels << 2;
    NiMemcpy(m_puiWidth, &auiWidth, uiDestSize);
    NiMemcpy(m_puiHeight, &auiHeight, uiDestSize);

    uiDestSize = (m_uiMipmapLevels + 1) << 2;
    NiMemcpy(m_puiOffsetInBytes, &auiOffsetInBytes, uiDestSize);

    NiStreamLoadBinary(kStream, m_pucPixels, 
        m_puiOffsetInBytes[m_uiMipmapLevels]*m_uiFaces);
}
//---------------------------------------------------------------------------
void NiPixelData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_spPalette = (NiPalette*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPixelData::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    if (m_spPalette)
        m_spPalette->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPixelData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
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

    NiStreamSaveBinary(kStream, m_puiOffsetInBytes[m_uiMipmapLevels]);
    NiStreamSaveBinary(kStream, m_uiFaces);

    NiStreamSaveBinary(kStream, m_pucPixels, 
        m_puiOffsetInBytes[m_uiMipmapLevels]*m_uiFaces);
}
//---------------------------------------------------------------------------
bool NiPixelData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiPixelData* pkPixelData = (NiPixelData*)pkObject;

    if (m_kPixelFormat != pkPixelData->m_kPixelFormat)
        return false;

    if (m_spPalette && !m_spPalette->IsEqual(pkPixelData->m_spPalette))
        return false;

    if ((m_uiMipmapLevels != pkPixelData->m_uiMipmapLevels) ||
        (m_uiPixelStride != pkPixelData->m_uiPixelStride))
    {
        return false;
    }

    for (unsigned int i = 0; i < m_uiMipmapLevels; i++)
    {
        if ((m_puiWidth[i] != pkPixelData->m_puiWidth[i]) ||
            (m_puiHeight[i] != pkPixelData->m_puiHeight[i]) ||
            (m_puiOffsetInBytes[i] != pkPixelData->m_puiOffsetInBytes[i]))
        {
            return false;
        }

    }

    if (m_puiOffsetInBytes[m_uiMipmapLevels] != 
        pkPixelData->m_puiOffsetInBytes[m_uiMipmapLevels]) 
    {
        return false;
    }

    if (m_uiFaces != pkPixelData->m_uiFaces) 
    {
        return false;
    }

    if (memcmp(m_pucPixels, pkPixelData->m_pucPixels, 
        m_puiOffsetInBytes[m_uiMipmapLevels]) != 0)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPixelData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPixelData::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("Width", GetWidth()));
    pkStrings->Add(NiGetViewerString("Height", GetHeight()));
    pkStrings->Add(NiGetViewerString("m_uiMipmapLevels", m_uiMipmapLevels));
    pkStrings->Add(NiGetViewerString("m_uiFaces", m_uiFaces));
    pkStrings->Add(NiGetViewerString("TotalSizeInBytes", 
        (unsigned int)GetTotalSizeInBytes()));
    pkStrings->Add(NiGetViewerString("FaceSizeInBytes", 
        (unsigned int)GetFaceSizeInBytes()));
}
//---------------------------------------------------------------------------
bool NiPixelData::ConvertToOppositeEndian(void)
{
    // Verify that all components are the same size, or the total bpp is 16
    unsigned int uiComponentSize = 0;
    bool bCanConvert = true;
    for (unsigned int ui = 0; ui < m_kPixelFormat.GetNumComponents(); ++ui)
    {
        NiPixelFormat::Component eSrcComponent;
        NiPixelFormat::Representation eSrcRep;
        unsigned char ucSrcBPC;
        bool bSrcSigned;

        m_kPixelFormat.GetComponent(ui, eSrcComponent, eSrcRep, ucSrcBPC,
            bSrcSigned);

        if (eSrcComponent != NiPixelFormat::COMP_EMPTY)
        {
            if (uiComponentSize == 0)
                uiComponentSize = ucSrcBPC;

            if (uiComponentSize != ucSrcBPC)
                bCanConvert = false;
        }
    }

    if (m_kPixelFormat.GetBitsPerPixel() == 16)
    {
        bCanConvert = true;
        uiComponentSize = 16;
    }

    if (!bCanConvert)
    {
        return false;
    }

    // Verify that the size is one we can swap within
    switch (uiComponentSize)
    {
    case 16:
    case 32:
    case 64:
        break;
    default:
        return false;
    }

    // Do the endian conversion
    for (unsigned int uiFace = 0; uiFace < GetNumFaces(); ++uiFace)
    {
        for (unsigned int uiLevel = 0; uiLevel < GetNumMipmapLevels();
            ++uiLevel)
        {
            unsigned int uiSize = static_cast<unsigned int>(
                (GetSizeInBytes(uiLevel, uiFace)*8) / uiComponentSize);
            char * pcData = (char*)GetPixels(uiLevel, uiFace);

            switch (uiComponentSize)
            {
            case 16:
                NiEndian::Swap16(pcData, uiSize);
                break;
            case 32:
                NiEndian::Swap32(pcData, uiSize);
                break;
            case 64:
                NiEndian::Swap64(pcData, uiSize);
                break;
            }
        }
    }

    // Successfully toggled endianness
    m_kPixelFormat.SetLittleEndian(!m_kPixelFormat.GetLittleEndian());
    return true;
}
//---------------------------------------------------------------------------
