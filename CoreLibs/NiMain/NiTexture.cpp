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
#include "NiTexture.h"

#include "NiImageConverter.h"
#include "NiPixelData.h"

NiImplementRTTI(NiTexture, NiObjectNET);

NiTexture* NiTexture::ms_pkHead = 0;
NiTexture* NiTexture::ms_pkTail = 0;
NiCriticalSection NiTexture::ms_kTextureListCriticalSection;

bool NiTexture::ms_bMipmapByDefault = false;

NiTexture::RendererData::TEXTUREFAILUREFUNC 
    NiTexture::RendererData::ms_pfnTextureFailureCallback = 
    NiTexture::RendererData::DefaultTextureFailureCallback;

NiPixelDataPtr NiTexture::RendererData::ms_aspReplacementData[
    NiTexture::RendererData::CREATEFAIL_COUNT];

NiPixelFormat NiTexture::RendererData::ms_kReplacementFormat;

//---------------------------------------------------------------------------
void NiTexture::RendererData::_SDMInit()
{
    ms_kReplacementFormat = NiPixelFormat::RGBA32;
}
//---------------------------------------------------------------------------
void NiTexture::RendererData::_SDMShutdown()
{
    // Clear out the replacement data so that the object report is correct
    // at exit.
    for (unsigned int ui = 0; 
        ui < NiTexture::RendererData::CREATEFAIL_COUNT; ui++)
    {
        ms_aspReplacementData[ui] = 0;
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiTexture::RendererData::GetReplacementData(
    NiTexture* pkTexture, NiTexture::RendererData::CreationFailure eFailure, 
#if defined(NIDEBUG)
    bool bReport)
#else
    bool)
#endif
{
#if defined(NIDEBUG)
    if (bReport)
        ReportDataCreationError(eFailure);
#endif

    if ((eFailure & 0x80000000) == 0)
        return 0;

    if (ms_pfnTextureFailureCallback == NULL)
        SetTextureFailureCallback(DefaultTextureFailureCallback);

    NIASSERT(ms_pfnTextureFailureCallback);
    return (*ms_pfnTextureFailureCallback)(pkTexture, eFailure);
}
//---------------------------------------------------------------------------
NiPixelData* NiTexture::RendererData::DefaultTextureFailureCallback(
    NiTexture*, NiTexture::RendererData::CreationFailure eFailure)
{
    unsigned int uiIndex = (eFailure & 0x7fffffff);
    if (uiIndex >= CREATEFAIL_COUNT)
        return 0;

    unsigned int uiNumFaces = 1;
    if (ms_aspReplacementData[uiIndex] == 0)
    {
        NiColorA kTopLeft, kBottomLeft;

        switch (eFailure)
        {
        case NO2DMATCH:
            // COMP_BLUE & yellow
            kTopLeft = NiColorA(0.0f, 0.0f, 1.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 1.0f, 0.0f, 1.0f);
            break;
        case NOCUBEMAPMATCH:
            // COMP_GREEN & COMP_RED
            kTopLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            uiNumFaces = 6;
            break;
        case NOCONVERTER:
            // Cyan & yellow
            kTopLeft = NiColorA(0.0f, 1.0f, 1.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 1.0f, 0.0f, 1.0f);
            break;
        case FAILEDCONVERT:
            // COMP_RED & yellow
            kTopLeft = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 1.0f, 0.0f, 1.0f);
            break;
        case BADDIMENSIONS:
            // Cyan
            kTopLeft = NiColorA(0.0f, 1.0f, 1.0f, 1.0f);
            kBottomLeft = NiColorA(0.0f, 1.0f, 1.0f, 1.0f);
            break;
        case FILEFORMATNOTSUPPORTED:
            // COMP_RED & COMP_BLUE
            kTopLeft = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(0.0f, 0.0f, 1.0f, 1.0f);
            break;
        case FILELOADFAILED:
            // COMP_RED
            kTopLeft = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        case CUBEMAPFAILEDCONVERT:
            // COMP_GREEN & yellow
            kTopLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 1.0f, 0.0f, 1.0f);
            uiNumFaces = 6;
            break;
        case CUBEMAPFILENOTFOUND:
            // COMP_GREEN & Magenta
            kTopLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 0.0f, 1.0f, 1.0f);
            uiNumFaces = 6;
            break;
        case CUBEMAPBADDIMENSIONS:
            // COMP_GREEN & Cyan
            kTopLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(0.0f, 1.0f, 1.0f, 1.0f);
            uiNumFaces = 6;
            break;
        case CUBEMAPFILEFORMATNOTSUPPORTED:
            // COMP_GREEN & COMP_BLUE
            kTopLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(0.0f, 0.0f, 1.0f, 1.0f);
            uiNumFaces = 6;
            break;
        case CUBEMAPFILELOADFAILED:
            // COMP_GREEN
            kTopLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            kBottomLeft = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
            uiNumFaces = 6;
            break;
        case FILENOTFOUND:
        case GENERIC:
        default:
            // Magenta
            kTopLeft = NiColorA(1.0f, 0.0f, 1.0f, 1.0f);
            kBottomLeft = NiColorA(1.0f, 0.0f, 1.0f, 1.0f);
            break;
        }
            
        ms_aspReplacementData[uiIndex] = MakeDataFromColors(kTopLeft, 
            kBottomLeft, ms_kReplacementFormat, uiNumFaces);
    }

    return ms_aspReplacementData[uiIndex];
}
//---------------------------------------------------------------------------
void NiTexture::RendererData::SetTextureReplacementFormat(
    const NiPixelFormat& kFormat)
{
    ms_kReplacementFormat = kFormat;

    // Clear any existing data objects
    for (unsigned int ui = 0; 
        ui < NiTexture::RendererData::CREATEFAIL_COUNT; ui++)
    {
        ms_aspReplacementData[ui] = 0;
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiTexture::RendererData::MakeDataFromColors(
    const NiColorA& kTopLeftColor, const NiColorA& kBottomLeftColor,
    const NiPixelFormat& kDesc, unsigned int uiNumFaces)
{
    NIASSERT(kDesc.GetFormat() == NiPixelFormat::FORMAT_RGBA || 
        kDesc.GetFormat() == NiPixelFormat::FORMAT_RGB);

    enum
    {
        ERR_WIDTH = 32,
        ERR_HEIGHT = 32
    };

    NiPixelData* pkData = NiNew NiPixelData(ERR_WIDTH, ERR_HEIGHT,
        kDesc, 1, uiNumFaces);
    NIASSERT(pkData);

    unsigned int auiColors[2];
    auiColors[0] = (((int)(255.0f * kTopLeftColor.r) << 
        kDesc.GetShift(NiPixelFormat::COMP_RED)) & 
        kDesc.GetMask(NiPixelFormat::COMP_RED));
    auiColors[0] |= (((int)(255.0f * kTopLeftColor.g) << 
        kDesc.GetShift(NiPixelFormat::COMP_GREEN)) & 
        kDesc.GetMask(NiPixelFormat::COMP_GREEN));
    auiColors[0] |= (((int)(255.0f * kTopLeftColor.b) << 
        kDesc.GetShift(NiPixelFormat::COMP_BLUE)) & 
        kDesc.GetMask(NiPixelFormat::COMP_BLUE));
    auiColors[0] |= (((int)(255.0f * kTopLeftColor.a) << 
        kDesc.GetShift(NiPixelFormat::COMP_ALPHA)) & 
        kDesc.GetMask(NiPixelFormat::COMP_ALPHA));

    auiColors[1] = (((int)(255.0f * kBottomLeftColor.r) << 
        kDesc.GetShift(NiPixelFormat::COMP_RED)) & 
        kDesc.GetMask(NiPixelFormat::COMP_RED));
    auiColors[1] |= (((int)(255.0f * kBottomLeftColor.g) << 
        kDesc.GetShift(NiPixelFormat::COMP_GREEN)) & 
        kDesc.GetMask(NiPixelFormat::COMP_GREEN));
    auiColors[1] |= (((int)(255.0f * kBottomLeftColor.b) << 
        kDesc.GetShift(NiPixelFormat::COMP_BLUE)) & 
        kDesc.GetMask(NiPixelFormat::COMP_BLUE));
    auiColors[1] |= (((int)(255.0f * kBottomLeftColor.a) << 
        kDesc.GetShift(NiPixelFormat::COMP_ALPHA)) & 
        kDesc.GetMask(NiPixelFormat::COMP_ALPHA));

    for (unsigned int uiFace = 0; uiFace < uiNumFaces; uiFace++)
    {
        unsigned char* pucPixels = pkData->GetPixels(0, uiFace);

        unsigned char ucBytesPP = kDesc.GetBitsPerPixel() / 8;

        for (unsigned int i = 0; i < ERR_HEIGHT; i++)
        {
            unsigned int uiQuadrantI = i / (ERR_HEIGHT / 2);

            for (unsigned int j = 0; j < ERR_WIDTH; j++)
            {
                unsigned int uiQuadrantJ = j / (ERR_WIDTH / 2);

                unsigned int uiColor = (uiQuadrantI == uiQuadrantJ ? 0 : 1);

                NiMemcpy(pucPixels, auiColors + uiColor, ucBytesPP);
                
                pucPixels += ucBytesPP;
            }
        }
    }

    return pkData;
}
//---------------------------------------------------------------------------
void NiTexture::RendererData::ReportDataCreationError(
    NiTexture::RendererData::CreationFailure eFailure)
{
    const char* pacError;
    const char* pacColor;

    switch (eFailure)
    {
    case GENERIC:
        pacError = "Generic";
        pacColor = "Magenta";
        break;
    case NO2DMATCH:
        pacError = "No 2D match";
        pacColor = "COMP_BLUE & Yellow";
        break;
    case NOCUBEMAPMATCH:
        pacError = "No CubeMap match";
        pacColor = "COMP_GREEN & COMP_RED";
        break;
    case NOCONVERTER:
        pacError = "No converter";
        pacColor = "Cyan & Yellow";
        break;
    case FAILEDCONVERT:
        pacError = "Failed convert";
        pacColor = "COMP_RED & Yellow";
        break;
    case FILENOTFOUND:
        pacError = "File not found";
        pacColor = "Magenta";
        break;
    case BADDIMENSIONS:
        pacError = "Bad dimensions";
        pacColor = "Cyan";
        break;
    case FILEFORMATNOTSUPPORTED:
        pacError = "File format not supported";
        pacColor = "COMP_RED & COMP_BLUE";
        break;
    case FILELOADFAILED:
        pacError = "File load failed";
        pacColor = "COMP_RED";
        break;
    case CUBEMAPFAILEDCONVERT:
        pacError = "Cube map failed convert";
        pacColor = "COMP_GREEN & yellow";
        break;
    case CUBEMAPFILENOTFOUND:
        pacError = "Cube map file not found";
        pacColor = "COMP_GREEN & Magenta";
        break;
    case CUBEMAPBADDIMENSIONS:
        pacError = "Cube map bad dimensions";
        pacColor = "COMP_GREEN & Cyan";
        break;
    case CUBEMAPFILEFORMATNOTSUPPORTED:
        pacError = "Cube map file format not supported";
        pacColor = "COMP_GREEN & COMP_BLUE";
        break;
    case CUBEMAPFILELOADFAILED:
        pacError = "Cube map file load failed";
        pacColor = "COMP_GREEN";
        break;
    default:
        pacError = "UNKNOWN";
        pacColor = "Magenta";
        break;
    }

    NiOutputDebugString("TextureRendererData error> ");
    NiOutputDebugString(pacError);
    NiOutputDebugString(" - Using ");
    NiOutputDebugString(pacColor);
    NiOutputDebugString(" texture.\n");
}
//---------------------------------------------------------------------------
void NiTexture::RendererData::SetTextureFailureCallback(
    NiTexture::RendererData::TEXTUREFAILUREFUNC pfnFunc)
{
    ms_pfnTextureFailureCallback = pfnFunc;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiTexture::LoadBinary(NiStream& kStream)
{
    NiObjectNET::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTexture::LinkObject(NiStream& kStream)
{
    NiObjectNET::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTexture::RegisterStreamables(NiStream& kStream)
{
    return NiObjectNET::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTexture::SaveBinary(NiStream& kStream)
{
    NiObjectNET::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTexture::IsEqual(NiObject* pkObject)
{
    return NiObjectNET::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
void NiTexture::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObjectNET::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiTexture::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
