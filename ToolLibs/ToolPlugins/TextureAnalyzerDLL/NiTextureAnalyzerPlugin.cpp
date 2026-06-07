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

#include "stdafx.h"
#include "resource.h"
#include "NiTextureAnalyzerPlugin.h"
#include <NiXMLLogger.h>


NiImplementRTTI(NiTextureAnalyzerPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiTextureAnalyzerPlugin::NiTextureAnalyzerPlugin() :
    NiPlugin("Texture Analyzer", "1.0", 
        "Displays Texture partitioning statistics",
        "Searches the scene graph for Texturened objects, "
        "printing Texture partition information.")
{
}
//---------------------------------------------------------------------------
bool NiTextureAnalyzerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiTextureAnalyzerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiString GetPixelFormatString(NiPixelFormat kFormat)
{
    NiString strFormat;
    switch(kFormat.GetFormat())
    {
        case NiPixelFormat::FORMAT_RGB:
            strFormat += "RGB";
            break;
        case NiPixelFormat::FORMAT_RGBA:
            strFormat += "RGBA";
            break;
        case NiPixelFormat::FORMAT_PAL:
            strFormat += "PAL";
            break;
        case NiPixelFormat::FORMAT_PALALPHA:
            strFormat += "PALALPHA";
            break;
        case NiPixelFormat::FORMAT_DXT1:
            strFormat += "DXT1";
            break;
        case NiPixelFormat::FORMAT_DXT3:
            strFormat += "DXT3";
            break;
        case NiPixelFormat::FORMAT_DXT5:
            strFormat += "DXT5";
            break;
        case NiPixelFormat::FORMAT_RGB24NONINTERLEAVED:
            strFormat += "RGB24NONINTERLEAVED";
            break;
        case NiPixelFormat::FORMAT_BUMP:
            strFormat += "BUMP";
            break;
        case NiPixelFormat::FORMAT_BUMPLUMA:
            strFormat += "BUMPLUMA";
            break;
        default:
            strFormat += "Unknown Format #" + 
                NiString::FromInt(kFormat.GetFormat());
            break;
    }

    strFormat += " (Bits Per Pixel: " + 
        NiString::FromInt(kFormat.GetBitsPerPixel()) + ")";
    return strFormat;
}
//---------------------------------------------------------------------------
unsigned int CalcMaskSize(unsigned int uiMask)
{
    NiUInt32 uiTotal = 0;
    while (uiMask != 0)
    {
        uiTotal += (uiMask & 1);
        uiMask = uiMask >> 1;
    }
    return uiTotal;
    
}
//---------------------------------------------------------------------------
NiString GetPixelMaskString(NiPixelFormat kFormat)
{
    NiString kString;  
    NiPixelFormat::Component aeWhichOrder[4];
    memset(aeWhichOrder, 0, 4 * sizeof(NiPixelFormat::Component));

    NiUInt32 uiCount = 0;
    for (NiUInt32 ui = 0; ui < 4; ui++)
    {
        NiPixelFormat::Component eComp0 = (NiPixelFormat::Component) ui;
        unsigned int uiMask0 = kFormat.GetMask(eComp0);

        NiUInt32 uiIndex = uiCount;
        for (NiUInt32 uj = 0; uj < uiCount; uj++)
        {
            NiPixelFormat::Component eComp1 = aeWhichOrder[uj];
            unsigned int uiMask1 = kFormat.GetMask(eComp1);

            if (uiMask0 < uiMask1)
            {
                for (NiInt32 ix = uiCount; ix > (int)uj; ix--)
                {
                    aeWhichOrder[ix] = aeWhichOrder[ix-1];
                }
                uiIndex = uj;
                break;
            }
        }

        aeWhichOrder[uiIndex] = eComp0;
        uiCount++;
    }
        
    for (NiInt32 iIndex = 3; iIndex >= 0; iIndex--)
    {
        NiPixelFormat::Component eComp = aeWhichOrder[iIndex];
        unsigned int uiMask = kFormat.GetMask(eComp); 
        char cChannel = '\0';
        if (eComp == NiPixelFormat::COMP_RED)
            cChannel = 'R';
        else if (eComp == NiPixelFormat::COMP_GREEN)
            cChannel = 'G';
        else if (eComp == NiPixelFormat::COMP_BLUE)
            cChannel = 'B';
        else if (eComp == NiPixelFormat::COMP_ALPHA)
            cChannel = 'A';

        kString += cChannel + NiString::FromInt(CalcMaskSize(uiMask));
    }

    return kString;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiTextureAnalyzerPlugin::Execute(
    const NiPluginInfo*)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiTexture* pkTexture = NiTexture::GetListHead();
    
    m_strMessage.Empty();
    unsigned int uiTotalKnownMemSize = 0;

    while(pkTexture)
    {
        char acString[256];
        NiSprintf(acString, 256, "<Ptr to %lX>", PtrToUlong(pkTexture));
        const char* pcName = acString;
        const char* pcClassName = pkTexture->GetRTTI()->GetName();
        NiString strSubMsg;
        NiUInt32 uiWidth = pkTexture->GetWidth();
        NiUInt32 uiHeight = pkTexture->GetHeight();

        if (NiIsKindOf(NiSourceTexture, pkTexture))
        {
            NiSourceTexture* pkSrcTex = (NiSourceTexture*) pkTexture;
            NiPixelData* pkPixels = pkSrcTex->GetSourcePixelData();
            if (pkSrcTex->GetFilename())
                pcName = pkSrcTex->GetFilename();

            NiString strAncillaryText = "";
            if (pkPixels)
            {
                NiPixelFormat kFormat = pkPixels->GetPixelFormat();
                NiPalette* pkPalette = pkPixels->GetPalette();
                NiUInt32 uiTotalSizeInBytes = (NiUInt32)pkPixels->GetTotalSizeInBytes();
                NiUInt32 uiNumMipMapLevels = pkPixels->GetNumMipmapLevels();
                uiTotalKnownMemSize += uiTotalSizeInBytes;
                float fSizeInKB = ((float) uiTotalSizeInBytes) / 1024.0f;

                strAncillaryText += 
                    "     Pixel Format: " + 
                    GetPixelFormatString(kFormat) + "\r\r\n";
                strAncillaryText +=
                    "     Pixel Mask:   " + 
                    GetPixelMaskString(kFormat) + "\r\r\n";
                strAncillaryText += 
                    "     Palette Exists: " + 
                    NiString::FromBool(pkPalette != NULL) + "\r\r\n";
                strAncillaryText +=
                    "     Total Size: " + 
                    NiString::FromFloat(fSizeInKB) + " kb\r\r\n";
                strAncillaryText += 
                    "     MipMap Levels: " + 
                    NiString::FromInt(uiNumMipMapLevels) + "\r\r\n";
            }
            else
            {
                 strAncillaryText += "     External Texture";
            }
        
            strSubMsg.Format(
                "%s Object: \"%s\" (%d x %d)\r\r\n%s", 
                pcClassName, pcName, uiWidth, 
                uiHeight, (const char*) strAncillaryText);
        
        }
        else
        {
            strSubMsg.Format(
                "%s Object: (%d x %d)\r\r\n", pcClassName, 
                uiWidth, uiHeight);
        }

        strSubMsg += "\r\r\n";
        m_strMessage += strSubMsg;
        
        pkTexture = pkTexture->GetListNext();
    }

    float fSizeInKB = ((float) uiTotalKnownMemSize) / 1024.0f;
    float fSizeInMB = fSizeInKB / 1024.0f;
    NiString strSubMsg;
    strSubMsg.Format(
        "================================================================"
        "\r\r\nTotal Known Texture Size:                         %f"
        " MB\r\r\n", fSizeInMB);
    
    m_strMessage += strSubMsg;

    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if ((bWriteResultsToLog) && (pkLogger))
    {
        pkLogger->LogElement("TextureAnalysisResults", m_strMessage);
    }
    else
    {
        NiInfoDialog kDlg("Texture Analysis Results:");
        kDlg.SetText(m_strMessage);
        kDlg.DoModal();
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiTextureAnalyzerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiTextureAnalyzerPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiTextureAnalyzerPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiTextureAnalyzerPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
