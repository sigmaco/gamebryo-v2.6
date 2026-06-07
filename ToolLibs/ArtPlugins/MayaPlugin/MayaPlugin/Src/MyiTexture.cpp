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

// Precompiled Headers
#include "MayaPluginPCH.h"


#include "NiDevImageQuantizer.h"

#include <fstream>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

// The purpose of the MyiTexture class is simply to create a structure to
// temporarly housed a newly allocated NiTextureProperty as well as some
// useful info on the texture such as the width, height, pixeldepth, etc.

//---------------------------------------------------------------------------
MyiTexture::~MyiTexture()
{
    // m_pFileName is an internal buffer - not to be freed by us.
    // m_pName is an internal buffer - not to be freed by us.
    // m_pBuffer is an internal buffer - not to be freed by us.
    // pDirectory & pDrive reference an outside char.
    m_spTexture = NULL;
    NiFree(m_pExt);
    NiFree(m_pName);
}
//---------------------------------------------------------------------------
MyiTexture::MyiTexture(int iTextureID) 
{
    Initialize();

    // Get the Attributes for this texture.
    if (!GetAttributes(iTextureID))
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    if (!CreateNiTexture())
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    // Ensure our Ni texture was create properly
    if (m_spTexture == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }
    
    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiTexture::MyiTexture(char* szFileName) 
{
    Initialize();

    NiStrcpy(m_szFullTexturePath, _MAX_PATH, szFileName);

    MyiTexture::MayaSelfLoadingTextureFormat eTextureFormat = 
        IsSelfLoadTexture(m_szFullTexturePath);

    if (eTextureFormat == NOT_SELF_LOADING)
    {
        DtExt_Err("ERROR: with Texture %s Only .TGA and .BMP files are "
            "supported.\n", szFileName);

        SetStatus(MYIOBJ_ERROR);
        return;
    }

    // Ensure our Ni texture was create properly
    if (!LoadTextureFromFile(m_szFullTexturePath))
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiTexture::MyiTexture(MObject kTexture)
{
    Initialize();

    // Get the Attributes for this texture.
    if (!GetAttributes(kTexture))
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    if (!CreateNiTexture())
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    // Ensure our Ni texture was create properly
    if (m_spTexture == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
void MyiTexture::Initialize()
{
    m_pDir          = NULL; 
    m_pDrive        = NULL; 
    m_pExt          = NULL;
    m_pName         = NULL;
    m_pFileName     = NULL;
    m_pBuffer       = NULL;
    m_bHasAlpha     = false;
    m_iWidth        = 1;
    m_iHeight       = 1;
    m_iComponents   = 0;
    m_bCubeMap      = false;
    m_bBumpMap      = false;
    m_bMipMap       = false;
    m_kTextureNode = MObject::kNullObj;

    m_szTextureFileName[0] = '\0';
    m_szPlatformDirectory[0] = '\0';
    m_szFullTexturePath[0] = '\0';

    m_pExt = NiAlloc(char, _MAX_EXT);
    NiStrcpy(m_pExt, _MAX_EXT, "NIF");

    DtGetDirectory(&m_pDir);
    DtSceneGetName(&m_pFileName);

    m_iPlatformFormat = MyiExportGlobals::PLATFORM_DEFAULT;
    m_iPlatformSizeReduction = PLATFORM_SIZE_REDUCTION_PLATFORM_DEFAULT;
    m_iPlatformTextureLocation = PLATFORM_TEXTURE_LOCATION_DEFAULT;
    m_bPlaformLoadDirectlyToRenderer = false;

    // Apply the Platform Defaults
    switch(NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_GENERIC:
        m_iPlatformSizeReduction = gExport.m_iGenericTextureReduction;
        break;
    case NiSystemDesc::RENDERER_XENON:
        m_iPlatformSizeReduction = gExport.m_iXenonTextureReduction;
        break;
    case NiSystemDesc::RENDERER_DX9:
        m_iPlatformSizeReduction = gExport.m_iDX9TextureReduction;
        break;
    case NiSystemDesc::RENDERER_D3D10:
        m_iPlatformSizeReduction = gExport.m_iD3D10TextureReduction;
        break;
    case NiSystemDesc::RENDERER_PS3:
        m_iPlatformSizeReduction = gExport.m_iPS3TextureReduction;
        break;
    case NiSystemDesc::RENDERER_WII:
        m_iPlatformSizeReduction = gExport.m_iWiiTextureReduction;
        break;
    }
}
//---------------------------------------------------------------------------
bool MyiTexture::CreateTextureFile(
    int iTextureID, const char *pDrive, const char *pDirectory, 
    const char *pName, const char *pExt)
{
    char path[_MAX_PATH];
    char szTextureFileFullPath[_MAX_PATH];
    bool bResult = false;

    MyiTexture::MayaSelfLoadingTextureFormat eTextureFormat = 
        IsSelfLoadTexture(m_szFullTexturePath);
    
    if (eTextureFormat != NOT_SELF_LOADING)
    {
        // SAVE THE FILE NAME BRIEFLY
        NiStrcpy(szTextureFileFullPath, _MAX_PATH, m_szFullTexturePath);

        ReplaceChar(szTextureFileFullPath, '/', '\\');
        DuplicateChar(szTextureFileFullPath, _MAX_PATH, '\\');

            // GET THE TEXTURE FILE NAME AND BUILD THE PATH
        GetFileNameFromPath( szTextureFileFullPath, m_szTextureFileName, 
            _MAX_PATH, m_szOriginalTextureFileName, _MAX_PATH );

#if defined(_MSC_VER) && _MSC_VER >= 1400
        _makepath_s(path, _MAX_PATH, pDrive, pDirectory, m_szTextureFileName, 
            pExt);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        _makepath(path, pDrive, pDirectory, m_szTextureFileName, pExt);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

        ReplaceChar(path, '/', '\\');
        DuplicateChar(path, _MAX_PATH, '\\');


        // EXTERNAL TEXTURE FILE ie .TGA
        const unsigned int uiLen = _MAX_PATH + _MAX_PATH + 25;
        char szCommand[uiLen];

            // MOVE THE TEXTURE TO THE DESTINATION DIRECTORY
        NiSprintf( szCommand, uiLen, "system(\"copy \\\"%s\\\" \\\"%s\\\"\");",
            szTextureFileFullPath, path);

        bResult = MGlobal::executeCommand( szCommand ) == MS::kSuccess;
    }
    else
    {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        _makepath_s(path, _MAX_PATH, pDrive, pDirectory, pName, pExt);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        _makepath(path, pDrive, pDirectory, pName, pExt);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

        ReplaceChar(path, '/', '\\');

        // Write image to the file.
        bResult = DtImageLibFileSave(iTextureID, path);
    }

    NIASSERT(bResult);
    return bResult;
}
//---------------------------------------------------------------------------
bool MyiTexture::CreateNiTexture()
{
    NiPixelData *pPixelData = NULL;

    MayaSelfLoadingTextureFormat eTextureFormat = 
        IsSelfLoadTexture(m_szFullTexturePath);

    // Create the Texture as External Original Format
    if (GetTextureLocation(eTextureFormat) == 
            TEXTURE_SAVE_EXTERNAL_AS_ORIGINAL)
    {

        if (eTextureFormat == NOT_SELF_LOADING)
        {
            // If it isn't self loading we assume an alpha property
            // because we can't tell.
            m_bHasAlpha = true;
        }
        else
        {
             // We need to determine if this texture needs an alpha property.
            NiSourceTexturePtr spTexture;
            int iWidth;
            int iHeight;

            CreateTextureFromFile(m_szFullTexturePath, iWidth, iHeight, 
                spTexture);

            NIASSERT(spTexture->GetSourcePixelData());

            m_bHasAlpha = NiDevImageQuantizer::ImageHasAlpha(
                spTexture->GetSourcePixelData());
        }

        if (!CreateCubeMap())
        {
            m_kFormatPrefs.m_ePixelLayout = 
                NiTexture::FormatPrefs::PIX_DEFAULT;

            m_kFormatPrefs.m_eMipMapped = 
                NiTexture::FormatPrefs::MIP_DEFAULT;

            m_spTexture = CreateExternalTextureOriginal(m_kFormatPrefs);
        }

        // Verify the texture is a power of 2
        CheckPowerOfTwoTexture(m_pName, m_iWidth, m_iHeight);

        return true;
    }

    // Create the Cube Map
    if (CreateCubeMap())
    {
        return true;
    }

    // Create a Regular Texture
    if (eTextureFormat != NOT_SELF_LOADING)
    {
        // CLEAN UP THE PATH
        char szTextureFileFullPath[_MAX_PATH];
        NiStrcpy(szTextureFileFullPath, _MAX_PATH, m_szFullTexturePath);
        
        // CLEAN UP THE PATH SEPARATOR
        ReplaceChar(szTextureFileFullPath, '/', '\\');
        
        if (!LoadTextureFromFile(szTextureFileFullPath))
        {
            return false;
        }
        pPixelData = m_spTexture->GetSourcePixelData();
    }
    else
    {   
        // CREATE THE IMAGE FROM WHAT MAYA HAS
        pPixelData = CreatePixelData(); 
        if (pPixelData == NULL) 
        {
            return false;
        }
    }

    // Verify the texture is a power of 2
    CheckPowerOfTwoTexture(m_pName, m_iWidth, m_iHeight);
    
    // Convert for pixel options
    NiPixelDataPtr spPixelData = pPixelData; 
    ConvertPixelData(spPixelData);
    
    switch(GetTextureLocation(eTextureFormat))
    {
    case TEXTURE_SAVE_INTERNAL:
        {
            // Create the Texture
            if (pPixelData->GetNumFaces() == 6)
            {
                m_spTexture = NiSourceCubeMap::Create(spPixelData, NULL,
                    m_kFormatPrefs);
            }
            else
            {
                m_spTexture = NiSourceTexture::Create(spPixelData, 
                    m_kFormatPrefs);
            }
            
            // Check for failure to create texture
            if (m_spTexture == NULL)
                return false;

            // Set the Textures File Name
            if ( strcmp(m_szTextureFileName,"")==0)
                m_spTexture->SetFilename(m_pName);
            else
                m_spTexture->SetFilename(m_szOriginalTextureFileName);
            
        }
        break;
    case TEXTURE_SAVE_EXTERNAL_AS_NIF:
        {
            // Load the texture from that file
            m_kFormatPrefs.m_ePixelLayout = 
                NiTexture::FormatPrefs::PIX_DEFAULT;

            m_kFormatPrefs.m_eMipMapped = 
                NiTexture::FormatPrefs::MIP_DEFAULT;

            m_kFormatPrefs.m_eAlphaFmt = 
                NiTexture::FormatPrefs::ALPHA_DEFAULT;

            m_spTexture = 
                CreateExternalNIFSaveFile(spPixelData, m_kFormatPrefs);
        }
        break;
    default:
        NIASSERT(false);  
        break;
    }

    return true;
}
//---------------------------------------------------------------------------
NiTexture::FormatPrefs MyiTexture::CreatePlatformFormatPrefs()
{
    NiTexture::FormatPrefs kPrefs;

    // Setup MipMaping
    if ((NiTexturingProperty::FilterMode)gExport.m_iFilterType == 
        NiTexturingProperty::FILTER_TRILERP)
    {
        kPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
    }
    else
    {
        kPrefs.m_eMipMapped = NiTexture::FormatPrefs::MIP_DEFAULT;
    }

    // Set the Texture Quality
    if (m_bBumpMap)
        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::BUMPMAP;
    else
        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::PIX_DEFAULT;


    // Set the Alpha Preferences
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::ALPHA_DEFAULT;

    return kPrefs;
}
//---------------------------------------------------------------------------
NiSourceTexturePtr MyiTexture::CreateExternalNIFSaveFile(
    NiPixelData* pPixelData, NiTexture::FormatPrefs& kPrefs)
{
    // Check for NULL PIXEL DATA
    if (pPixelData == NULL)
    {
        return NULL;
    }

    // Create the file with the correct Nif Extension
    char szFullFileName[_MAX_PATH];
    
    if ( strcmp(m_szTextureFileName,"")==0)
        NiSprintf(szFullFileName, _MAX_PATH, "%s.NIF", m_pName);
    else
        NiSprintf(szFullFileName, _MAX_PATH, "%s.NIF", m_szTextureFileName);
    
    // Try and create the platform specific directory
    CreatePlatformSpecificSubDirectoryAndSetConverter();
    
    // Create the stream and save the texture
    NiStream TextureStream;
    TextureStream.InsertObject(pPixelData); 

    // Set the Endianness
    // Use a switch because the platform enums are in a different order
    switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_GENERIC:
        TextureStream.SetSaveAsLittleEndian(
            NiDevImageConverter::IsLittleEndian(NiDevImageConverter::ANY));
        break;
    case NiSystemDesc::RENDERER_XENON:
        TextureStream.SetSaveAsLittleEndian(
            NiDevImageConverter::IsLittleEndian(NiDevImageConverter::XENON));
        break;
    case NiSystemDesc::RENDERER_PS3:
        TextureStream.SetSaveAsLittleEndian(
            NiDevImageConverter::IsLittleEndian(NiDevImageConverter::PLAYSTATION3));
        break;
    case NiSystemDesc::RENDERER_WII:
        TextureStream.SetSaveAsLittleEndian(
            NiDevImageConverter::IsLittleEndian(NiDevImageConverter::WII));
        break;
    case NiSystemDesc::RENDERER_DX9:
        TextureStream.SetSaveAsLittleEndian(
            NiDevImageConverter::IsLittleEndian(NiDevImageConverter::DX9));
        break;
    case NiSystemDesc::RENDERER_D3D10:
        TextureStream.SetSaveAsLittleEndian(
            NiDevImageConverter::IsLittleEndian(NiDevImageConverter::D3D10));
        break;
    default:
        NIASSERT(0);
        break;
    }
    
    char* szPlatformName =
        NiImageConverter::ConvertFilenameToPlatformSpecific(szFullFileName);
    TextureStream.Save( szPlatformName );
    NiFree( szPlatformName );

    NiSourceTexture* pNewTexture = NULL;
    
    if (pPixelData->GetNumFaces() == 6)
    {
        bool bUsePreload = NiSourceTexture::GetUsePreloading();
        NiSourceTexture::SetUsePreloading(false);

        pNewTexture = NiSourceCubeMap::Create(szFullFileName, NULL, kPrefs);

        NiSourceTexture::SetUsePreloading(bUsePreload);
    }
    else
    {
        pNewTexture = NiSourceTexture::Create(szFullFileName, kPrefs);
    }

    // Create just the file name again
    if ( strcmp(m_szTextureFileName,"")==0)
        NiSprintf(szFullFileName, _MAX_PATH, "%s.NIF", m_pName);
    else
        NiSprintf(szFullFileName, _MAX_PATH, "%s.NIF", m_szTextureFileName);

    // Put the File name in the Source Texture
    pNewTexture->SetFilename(szFullFileName);

    return pNewTexture;
}
//---------------------------------------------------------------------------
NiPixelData *MyiTexture::CreatePixelData(void) 
{
    NiPixelData *pPixelData;
    unsigned int uiPixelSize;
    int i, j;

    // In Maya, the RGBA values for these images always will be one
    // byte (8 bits) for each component.

    // At present, RGBA component depth is assumed.
    uiPixelSize = m_iComponents;

    if (m_pBuffer == NULL)
    {
        // if the texture was not found in the MDt portion of the export, we
        // can't find it. Warn and exit
        DtExt_Err("ERROR: The texture %s was not able to have pixel data "
            "created for it. Try setting it to a natively supported texture "
            "type.", m_pName);
        return NULL;
    }

    pPixelData = NiNew NiPixelData(m_iWidth, m_iHeight, NiPixelFormat::RGBA32);
    if (pPixelData == NULL)  
        return NULL;
    
    unsigned char *pDst, *pSrc, *pImage;

    pSrc = m_pBuffer;
    pImage = pPixelData->GetPixels();

    if (gExport.m_bFlipTexturesInsteadOfUVs)
    {
        // Textures are already flipped as they come from Maya so
        // if we are flipping textures instead of UV's then we DO NOT need  
        // to flip Maya textures

        for (i=0; i<m_iHeight;i++)
        {
            pDst = &pImage[i * m_iWidth * uiPixelSize];
            for (j = 0; j < m_iWidth; j++)
            {
                // For each component...(component is 3 or 4)
                
                // MAYA stores it's values as BGRA and not RGBA
                // So we must convert.
                pDst[0] = pSrc[2];
                pDst[1] = pSrc[1];
                pDst[2] = pSrc[0];
                pDst[3] = pSrc[3];
                               
                pDst += uiPixelSize;
                pSrc += 4;
            }
        }
    }
    else
    {
        // Textures are already flipped as they come from Maya so
        // if we are NOT flipping textures instead of UV's then we MUST  
        // flip Maya textures.

        for (i=0; i<m_iHeight;i++)
        {
            pDst = &pImage[ (m_iHeight - i - 1) * m_iWidth * uiPixelSize];
            for (j = 0; j < m_iWidth; j++)
            {
                // For each component...(component is 3 or 4)
                
                // MAYA stores it's values as BGRA and not RGBA
                // So we must convert.
                pDst[0] = pSrc[2];
                pDst[1] = pSrc[1];
                pDst[2] = pSrc[0];
                pDst[3] = pSrc[3];
                             
                pDst += uiPixelSize;
                pSrc += 4;
            }
        }
    }

    // Scan the image looking for a non opaque alpha value
    m_bHasAlpha = NiDevImageQuantizer::ImageHasAlpha(pPixelData);

    return pPixelData;
}
//---------------------------------------------------------------------------
bool MyiTexture::GetAttributes(int iTextureID)
{
    char* pName;
    DtTextureGetNameID(iTextureID, &pName);

    // Allocate the name;
    size_t stLen = strlen(pName) + 1;   
    m_pName = NiAlloc(char, stLen);
    NiStrcpy(m_pName, stLen, pName);

    DtTextureGetImageSizeByID( iTextureID, &m_iWidth, &m_iHeight, 
        &m_iComponents);

    DtTextureGetImageByID( iTextureID, &m_pBuffer );

    // Get the texture file name
    char *pTempFileFullPath;
    DtTextureGetFileNameID( iTextureID, &pTempFileFullPath );

    GetFileNameFromPath(pTempFileFullPath, m_szTextureFileName,
        _MAX_PATH, m_szOriginalTextureFileName, _MAX_PATH, '/');

    // Save the Textures Full Path
    NiStrcpy(m_szFullTexturePath, _MAX_PATH, pTempFileFullPath);

    // Fail if we don't have a texture size
    if (((m_iWidth == 0) || (m_iHeight == 0)) &&
        (IsSelfLoadTexture(m_szFullTexturePath) < MyiTexture::RGBA))
        return false;

    m_bBumpMap = DtTextureGetBumpMapID(iTextureID);

    m_bMipMap = DtTextureNeedsMipMap(iTextureID);

    // Get the Texture Node
    DtTextureGetTextureNodeID(iTextureID, m_kTextureNode);

    // Look for any Platform Texture Properties
    GetPlatformAttributes(m_kTextureNode);

    m_kFormatPrefs = CreatePlatformFormatPrefs(); 
    return true;
}
//---------------------------------------------------------------------------
bool MyiTexture::GetAttributes(MObject kTextureNode)
{
    // Save the Texture Node
    m_kTextureNode = kTextureNode;

    // Get the Dependency Node
    MStatus kStatus;
    MFnDependencyNode dgNode;
    kStatus = dgNode.setObject(m_kTextureNode);

    // Get the Node Name
    size_t stLen = strlen(dgNode.name().asChar()) + 1;
    m_pName = NiAlloc(char, stLen);
    NiStrcpy(m_pName, stLen, dgNode.name().asChar());

    // Get the File name
    MPlug kPlug = dgNode.findPlug("fileTextureName", &kStatus);
    if (kStatus == MS::kSuccess)
    {
        MString kFileName;
        kStatus = kPlug.getValue(kFileName);
   
        if (kStatus != MS::kSuccess)
            return false;

        GetFileNameFromPath((char*)kFileName.asChar(), m_szTextureFileName, 
            _MAX_PATH, m_szOriginalTextureFileName, _MAX_PATH, '/');

        // Save the Textures Full Path
        NiStrcpy(m_szFullTexturePath, _MAX_PATH, kFileName.asChar());
    }
    else
    {
        kPlug = dgNode.findPlug("front", &kStatus);

        if (kStatus != MS::kSuccess)
            return false;

        NiStrcpy(m_szTextureFileName, _MAX_PATH, "Environmental Cube Map");
        NiStrcpy(m_szOriginalTextureFileName, _MAX_PATH, 
            "Environmental Cube Map");

        NiStrcpy(m_szFullTexturePath, _MAX_PATH,  "Environmental Cube Map");
    }
    
    // Look for any Platform Texture Properties
    GetPlatformAttributes(m_kTextureNode);

    m_kFormatPrefs = CreatePlatformFormatPrefs(); 
    return true;
}
//---------------------------------------------------------------------------
void MyiTexture::GetFormatAttribute(MFnDependencyNode& dgNode, 
    const char* pcTextureName, const char* pcFormatName, int& iTextureFormat)
{
    MStatus kStat = MStatus::kSuccess;

    bool bHasMaya = dgNode.hasAttribute(pcFormatName, &kStat);

    if(!bHasMaya)
    {
        //early out if attribute has not been added to texture
        return;
    }

    MString kTextureName = dgNode.name(&kStat);
    if(kStat != MStatus::kSuccess)
    {
        NIASSERT(!"Unable to get texture format");
        DtExt_Err("Unable to get texture format for texture \"%s\"",
            pcTextureName);
        iTextureFormat = MyiExportGlobals::PLATFORM_DEFAULT;
        return;

    }

    MString cmd = "getAttr -asString " + kTextureName + "." + pcFormatName;

    MString kResult;
    kStat = MGlobal::executeCommand(cmd, kResult);
    if(kStat != MStatus::kSuccess)
    {
        NIASSERT(!"Unable to get texture format");
        DtExt_Err("Unable to get texture format for texture \"%s\"",
            pcTextureName);
        iTextureFormat = MyiExportGlobals::PLATFORM_DEFAULT;
        return;
    }

    if(!DtConvertTextureFormatNameToEnum(kResult, iTextureFormat))
    {
        //uh oh
        NIASSERT(!"Unsupported texture format option");
        DtExt_Err("\nUnsupported texture format option \"%s\" for texture \""
            "%s\"\n", pcTextureName, pcFormatName);
        
        iTextureFormat = MyiExportGlobals::PLATFORM_DEFAULT;
    }

}

//---------------------------------------------------------------------------
void MyiTexture::GetPlatformAttributes(MObject kTextureNode)
{
    // Get the Dependency Node
    MStatus kStatus;
    MFnDependencyNode dgNode;
    kStatus = dgNode.setObject(kTextureNode);

    NIASSERT(kStatus == MS::kSuccess);

    int iTextureFormat = MyiExportGlobals::PLATFORM_DEFAULT;
    int iTextureSizeReduction = PLATFORM_SIZE_REDUCTION_PLATFORM_DEFAULT;
    int iTextureLocation = PLATFORM_TEXTURE_LOCATION_DEFAULT;

    switch(NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_GENERIC:
        // Check for old texture options
        GetExtraAttribute(dgNode, "Ni_PC_Format", true, iTextureFormat);
        GetExtraAttribute(dgNode, "Ni_PC_Texture_Size", true, 
            iTextureSizeReduction);

        // Check for old texture options
        GetExtraAttribute(dgNode, "Ni_PC_Location", true, iTextureLocation);
        GetExtraAttribute(dgNode, "Ni_PC_Load_Directly_To_Renderer", true, 
            m_bPlaformLoadDirectlyToRenderer);

        GetFormatAttribute(dgNode, m_pName, "Ni_Generic_Format",
            iTextureFormat);

        GetExtraAttribute(dgNode, "Ni_Generic_Texture_Size", true, 
            iTextureSizeReduction);

        GetExtraAttribute(dgNode, "Ni_Generic_Location", true,
            iTextureLocation);
        GetExtraAttribute(dgNode, "Ni_Generic_Load_Directly_To_Renderer", true,
            m_bPlaformLoadDirectlyToRenderer);
        break;
    case NiSystemDesc::RENDERER_XENON:
        GetFormatAttribute(dgNode, m_pName, "Ni_Next_Gen_XBox_Format", 
            iTextureFormat);

        GetExtraAttribute(dgNode, "Ni_Next_Gen_XBox_Texture_Size", true, 
            iTextureSizeReduction);

        GetExtraAttribute(dgNode, "Ni_Next_Gen_XBox_Location", true, 
            iTextureLocation);

        GetExtraAttribute(dgNode, "Ni_Next_Gen_XBox_Load_Directly_To_Renderer",
            true, m_bPlaformLoadDirectlyToRenderer);
        break;
    case NiSystemDesc::RENDERER_DX9:
        GetFormatAttribute(dgNode, m_pName, "Ni_DX9_Format", iTextureFormat);

        GetExtraAttribute(dgNode, "Ni_DX9_Texture_Size", true, 
            iTextureSizeReduction);

        GetExtraAttribute(dgNode, "Ni_DX9_Location", true, 
            iTextureLocation);

        GetExtraAttribute(dgNode, "Ni_DX9_Load_Directly_To_Renderer",
            true, m_bPlaformLoadDirectlyToRenderer);
        break;
    case NiSystemDesc::RENDERER_D3D10:
        GetFormatAttribute(dgNode, m_pName, "Ni_D3D10_Format", iTextureFormat);

        GetExtraAttribute(dgNode, "Ni_D3D10_Texture_Size", true, 
            iTextureSizeReduction);

        GetExtraAttribute(dgNode, "Ni_D3D10_Location", true, 
            iTextureLocation);

        GetExtraAttribute(dgNode, "Ni_D3D10_Load_Directly_To_Renderer",
            true, m_bPlaformLoadDirectlyToRenderer);
        break;
    case NiSystemDesc::RENDERER_PS3:
        GetFormatAttribute(dgNode, m_pName, "Ni_PS3_Format", iTextureFormat);

        GetExtraAttribute(dgNode, "Ni_PS3_Texture_Size", true, 
            iTextureSizeReduction);
        GetExtraAttribute(dgNode, "Ni_PS3_Location", true, 
            iTextureLocation);
        break;
    case NiSystemDesc::RENDERER_WII:
        GetFormatAttribute(dgNode, m_pName, "Ni_Wii_Format", iTextureFormat);
        
        GetExtraAttribute(dgNode, "Ni_Wii_Texture_Size", true, 
            iTextureSizeReduction);
        GetExtraAttribute(dgNode, "Ni_Wii_Location", true, 
            iTextureLocation);
        break;
    default:
        NIASSERT(false);  // You should never get here
        break;
    }

    m_iPlatformFormat = iTextureFormat;

    if (iTextureSizeReduction != PLATFORM_SIZE_REDUCTION_PLATFORM_DEFAULT)
    {
        m_iPlatformSizeReduction = iTextureSizeReduction - 1;
    }

    if (iTextureLocation != PLATFORM_TEXTURE_LOCATION_DEFAULT)
        m_iPlatformTextureLocation = iTextureLocation;

}
//---------------------------------------------------------------------------
MyiTexture::MayaSelfLoadingTextureFormat 
MyiTexture::IsSelfLoadTexture(char *szFileName)
{

    // Ensure filename exists
    if (szFileName == NULL)
        return NOT_SELF_LOADING;

    // Ensure that we have a non-empty string
    if (strcmp(szFileName, "")==0)
        return NOT_SELF_LOADING;

    // Find the beginning of the extension
    char *pCurrent = strrchr(szFileName, '.');

    if (pCurrent == NULL)
        return NOT_SELF_LOADING;

    // Is it a Targa file
    if (NiStricmp( pCurrent, ".TGA" ) == 0 || 
        NiStricmp( pCurrent, ".TARGA" ) == 0)
        return TARGA;

    // Is it a BMP file?
    if (NiStricmp( pCurrent, ".BMP" )==0)
        return BMP;

    // Is it a RGB file?
    if ((NiStricmp(pCurrent, ".SGI") == 0) || 
       (NiStricmp(pCurrent, ".RGB") == 0) || 
       (NiStricmp(pCurrent, ".RGBA") == 0) || 
       (NiStricmp(pCurrent, ".INT") == 0) || 
       (NiStricmp(pCurrent, ".INTA") == 0))
        return RGBA;

    // Is it a DDS file?
    if (NiStricmp( pCurrent, ".DDS" )==0)
        return DDS;

    // Is it a EXR file?
    if (NiStricmp( pCurrent, ".EXR") == 0)
        return EXR;

    // Is it a HDR file?
    if (NiStricmp( pCurrent, ".HDR") == 0)
        return HDR;

    // It must be a non-supported file format
    return NOT_SELF_LOADING;
}
//---------------------------------------------------------------------------
void MyiTexture::ReplaceChar(char *szPath, char cFindChar, char cReplaceChar)
{
    char *pCurrent;

    // Replace all back slaches with forward slashes
    for(pCurrent = szPath; *pCurrent != '\0'; pCurrent++)
    {
        if (*pCurrent == cFindChar)
            *pCurrent = cReplaceChar;
    }

}
//---------------------------------------------------------------------------
void MyiTexture::DuplicateChar(char* szPath, unsigned int uiBuffLen, 
    char cDupChar)
{
    char szTempPath[_MAX_PATH];
    char *pCurrentSrc = szPath;
    char *pCurrentDest = szTempPath;

    // Copy to the temp path duplicating the dup character
    while(*pCurrentSrc != '\0')
    {
        if (*pCurrentSrc == cDupChar)
            *pCurrentDest++ = cDupChar;

        *pCurrentDest++ = *pCurrentSrc++;
    }

    // Place the NULL terminator
    *pCurrentDest = '\0';

    NiStrcpy(szPath, uiBuffLen, szTempPath);
}
//---------------------------------------------------------------------------
void MyiTexture::GetFileNameFromPath(char* szTextureFilePath, 
    char* szTextureFileName, unsigned int uiFileNameLen,
    char* szOriginalTextureFileName, unsigned int uiOrigFileNameLen,
    char cPathSeperator/*='\\'*/)

{
    // Return the string before the last
    char *pCurrent = strrchr(szTextureFilePath, cPathSeperator);

    // If the separator is not found, copy the entire string.
    if (pCurrent == NULL)
    {
        pCurrent = szTextureFilePath;
    }
    else
    {   // Move past the separator
        pCurrent++;
    }

    // Save Original File Name
    NiStrcpy(szOriginalTextureFileName, uiOrigFileNameLen, pCurrent);
    NiStrcpy(szTextureFileName, uiFileNameLen, pCurrent);

    // Remove Any Extension
    pCurrent = strrchr(szTextureFileName, '.');

    if (pCurrent != NULL)
        *pCurrent = '\0';
}
//---------------------------------------------------------------------------
void MyiTexture::GetDirectoryPathFromFilePath(char* szTextureFilePath, 
    char* szDirectoryPath, unsigned int uiBufferLen, 
    char cPathSeperator/*='\\'*/)
{
    // Return the string before the last
    const char *pCurrent = strrchr(szTextureFilePath, cPathSeperator);

    // If the separator is not found, copy the entire string.
    if (pCurrent != NULL)
    {
        NiStrncpy(szDirectoryPath, uiBufferLen, szTextureFilePath, 
            pCurrent - szTextureFilePath);
    }
}
//---------------------------------------------------------------------------
void MyiTexture::CreateSequentialTexturePath(char* szOriginalPath, 
    char* szNewPath, unsigned int uiBufferLen, int iSequenceNumber, 
    char cPathSeperator/*='\\'*/)
{
    NI_UNUSED_ARG(cPathSeperator);

    char szDirectoryPath[MAX_PATH] = "";
    char szFileName[MAX_PATH] = "";;
    char szExtension[MAX_PATH] = "";
    char szOriginalTextureFileName[MAX_PATH] = "";
    char* pCurrent = NULL;

    // Save the Extension
    pCurrent = strrchr(szOriginalPath, '.');
    if (pCurrent != NULL)
        NiStrcpy(szExtension, MAX_PATH, pCurrent);

    // Get Just the FileName
    MyiTexture::GetFileNameFromPath(szOriginalPath, szFileName, _MAX_PATH, 
        szOriginalTextureFileName, _MAX_PATH, '/');
    MyiTexture::GetDirectoryPathFromFilePath(szOriginalPath, szDirectoryPath, 
        MAX_PATH, '/');

    // Strip off the Numbers
    pCurrent = szFileName + strlen(szFileName) - 1;

    int iDigits = 0;

    while((pCurrent >= szFileName) &&
          (*pCurrent >= '0') &&
          (*pCurrent <= '9') )
    {
        *pCurrent-- = '\0';
        iDigits++;
    }

    iDigits = NiMax(1, iDigits);
    char acFormat[128];
    NiSprintf(acFormat, 128, "%%s/%%s%%0%dd%%s", iDigits);

    // "%s/%s%03d%s"
    NiSprintf(szNewPath, uiBufferLen, acFormat, szDirectoryPath, szFileName, 
        iSequenceNumber, szExtension);
}
//---------------------------------------------------------------------------
void MyiTexture::CheckPowerOfTwoTexture( char* szTextureName, 
    int iWidth, int iHeight)
{

    // Can Maya read the foramt?
    if ((iWidth == 0) && (iHeight == 0))
        return;

    int iWidthValue = 1;
    int iHeightValue = 1;

    // Verify that the width and heigh are powers of 2
    for( iWidthValue = 1; iWidthValue <= 0x8000000; iWidthValue <<= 1)
    {
        if (iWidth <= iWidthValue)
            break;
    }

    for( iHeightValue = 1; iHeightValue <= 0x8000000; iHeightValue <<= 1)
    {
        if ( iHeight <= iHeightValue )
            break;
    }

    if ((iWidth != iWidthValue) || (iHeight != iHeightValue))
    {
        // Echo to the Output Window
        DtExt_Err("Warning: The texture %s is not a power of 2.  Width=%d "
            "Height=%d\n There are significant restriction on all but the "
            "latest video hardware. See the documentation for more details.", 
            szTextureName, iWidth, iHeight );
    }
}
//---------------------------------------------------------------------------
bool MyiTexture::LoadTextureFromFile(char *szFileName)
{
    if (CreateTextureFromFile(szFileName, m_iWidth, m_iHeight, m_spTexture))
    {
        // Scan the image looking for a non opaque alpha value
        m_bHasAlpha = NiDevImageQuantizer::ImageHasAlpha(
            m_spTexture->GetSourcePixelData());

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool MyiTexture::CreateTextureFromFile(char *szFileName, int &iWidth, 
    int &iHeight, NiSourceTexturePtr &spTexture)
{
    // *******************************************
    // *******************************************
    // THIS IS A WORK AROUND TO DEAL WITH MAYA
    // MAYA FLIPS ALL OF ITS TEXTURES INTERNALLY
    // AS A WORK AROUND WE WILL FLIP THE TEXTURE MAP
    // TO MAINTAIN CONSISTANCY WITH WHAT THE ARTIST
    // SEES IN MAYA
    // *******************************************
    // *******************************************

    NiImageConverter* pNiIC = NiImageConverter::GetImageConverter();

    NiPixelData *pPixelData = NULL;
    pPixelData = pNiIC->ReadImageFile(szFileName,pPixelData);
    if (pPixelData==NULL)
    {
        return false;
    }

    // set width and height based on the file we loaded
    iWidth = pPixelData->GetWidth(); 
    iHeight = pPixelData->GetHeight();  

    // Flip if requested and NOT a compressed format
    if ( gExport.m_bFlipTexturesInsteadOfUVs &&
        (pPixelData->GetPixelFormat().GetFormat() != 
            NiPixelFormat::FORMAT_DXT1) &&
        (pPixelData->GetPixelFormat().GetFormat() != 
            NiPixelFormat::FORMAT_DXT3) &&
        (pPixelData->GetPixelFormat().GetFormat() != 
            NiPixelFormat::FORMAT_DXT5))
    {
        unsigned char* pCurrentTop;
        unsigned char* pCurrentBottom;
        unsigned char HoldChar;
        
        for( int iImageHeight = 0; iImageHeight < 
            (int)(pPixelData->GetHeight()/2); iImageHeight++)  
        {
            pCurrentTop = (*pPixelData)(0,iImageHeight);
            pCurrentBottom = (*pPixelData)(0,pPixelData->GetHeight() - 
                iImageHeight -1);
            
            for( int iWidth = 0; iWidth < (int)(pPixelData->GetWidth() * 
                pPixelData->GetPixelStride()); iWidth++ )
            {
                HoldChar = *pCurrentTop;
                *pCurrentTop = *pCurrentBottom;
                *pCurrentBottom = HoldChar;
                
                pCurrentTop++;
                pCurrentBottom++;
            }
        }
    }
    
    if (pPixelData->GetNumFaces() == 6)
        spTexture = NiSourceCubeMap::Create(pPixelData, NULL);
    else
        spTexture = NiSourceTexture::Create(pPixelData);

    if (spTexture == NULL)
    {
        // Echo to the Output Window
        DtExt_Err("Failed to Load Texture:%s\n", szFileName);

        return false;
    }

    // Verify the texture is a power of 2
    CheckPowerOfTwoTexture(szFileName, iWidth, iHeight);

    return true;
}
//---------------------------------------------------------------------------
NiSourceTexturePtr MyiTexture::CreateExternalTextureOriginal(
    NiTexture::FormatPrefs& kPrefs  )
{
    char szDestinationDir[_MAX_PATH];
    char szTextureFileFullPath[_MAX_PATH];
    char szDestinationFullPath[_MAX_PATH];
    bool bResult = false;
    
    // Get the texture file name and build the path
    NiStrcpy(szDestinationDir, _MAX_PATH, m_pDir);

    CreatePlatformSpecificSubDirectoryAndSetConverter();

    NiStrcat(szDestinationDir, _MAX_PATH, "\\");
    NiStrcat(szDestinationDir, _MAX_PATH, m_szPlatformDirectory);

    // Create the Destination Full Path for loading the texture
    NiStrcpy(szDestinationFullPath, _MAX_PATH, szDestinationDir);
    NiStrcat(szDestinationFullPath, _MAX_PATH, "\\");
    NiStrcat(szDestinationFullPath, _MAX_PATH, m_szOriginalTextureFileName);

    ReplaceChar(szDestinationFullPath, '/', '\\');
    DuplicateChar(szDestinationFullPath, _MAX_PATH, '\\');
       
    // Save the file name briefly
    NiStrcpy(szTextureFileFullPath, _MAX_PATH, m_szFullTexturePath);
    
    // Add in the seperators for MEL
    ReplaceChar(szTextureFileFullPath, '/', '\\');
    DuplicateChar(szTextureFileFullPath, _MAX_PATH, '\\');

    ReplaceChar(szDestinationDir, '/', '\\');
    DuplicateChar(szDestinationDir, _MAX_PATH, '\\');
    
    // External texture file, ie .TGA
    const unsigned int uiLen = _MAX_PATH + _MAX_PATH + 25;
    char szCommand[uiLen];
    
    // Move the texture to the destination directory
    NiSprintf( szCommand, uiLen, "system(\"copy \\\"%s\\\" \\\"%s\\\"\");", 
        szTextureFileFullPath, szDestinationDir);
    
    bResult = MGlobal::executeCommand( szCommand ) == MS::kSuccess;

    NiSourceTexture* pkNewTexture = NULL;
    if (IsCubeMap(szDestinationFullPath))
    {
        bool bUsePreload = NiSourceTexture::GetUsePreloading();
        NiSourceTexture::SetUsePreloading(false);

        pkNewTexture = (NiSourceTexture*)NiSourceCubeMap::Create(
            szDestinationFullPath, NULL, kPrefs);

        NiSourceTexture::SetUsePreloading(bUsePreload);
    }
    else
    {
        pkNewTexture = NiSourceTexture::Create(szDestinationFullPath, kPrefs);
    }

    // Only Use the File name don't use the Full Path.
    pkNewTexture->SetFilename(m_szOriginalTextureFileName);

    // Set the Hint if the texture should be loaded directly the Renderer
    pkNewTexture->SetLoadDirectToRendererHint(
        m_bPlaformLoadDirectlyToRenderer);
    return pkNewTexture;
}
//---------------------------------------------------------------------------
int MyiTexture::GetTextureLocation(
    MayaSelfLoadingTextureFormat eTextureFormat)
{
    // Get the Global Location Setting
    int iTextureSaveLocation = gExport.m_iTextureSaveLocation;

    // Get the platform texture location if Any
    if (m_iPlatformTextureLocation != PLATFORM_TEXTURE_LOCATION_DEFAULT)
    {
        // Convert from the platform enum to the global enum
        iTextureSaveLocation = (m_iPlatformTextureLocation - 1);
    }

    // Non Self Loading textures can't be saved as external original
    // it is saved externally as a NIF
    if ((iTextureSaveLocation == TEXTURE_SAVE_EXTERNAL_AS_ORIGINAL) &&
       (eTextureFormat == NOT_SELF_LOADING))
    {
        iTextureSaveLocation = TEXTURE_SAVE_EXTERNAL_AS_NIF;
    }

    // Setup NifViewerMode to be Save Internal
    char* pcFileName;
    DtSceneGetName(&pcFileName);
    
    if (strcmp(pcFileName, "NifViewerMode") == 0)
    {
        iTextureSaveLocation = TEXTURE_SAVE_INTERNAL;
    }

    return iTextureSaveLocation;
}
//---------------------------------------------------------------------------
void MyiTexture::CreatePlatformSpecificSubDirectoryAndSetConverter()
{
    // Try and create the platform specific directory
    switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_GENERIC:
        if (gExport.m_pcGenericTextureSaveDirectory != NULL)
        {
            char szNewDir[_MAX_PATH];
        
            NiSprintf(szNewDir, _MAX_PATH, "%s\\%s", m_pDir, 
                gExport.m_pcGenericTextureSaveDirectory);
            ReplaceChar(szNewDir, '/', '\\');

            CreateDirectoryStructure(szNewDir);

            NiStrcpy(m_szPlatformDirectory, _MAX_PATH, 
                gExport.m_pcGenericTextureSaveDirectory);
        }

        // Setup the Image converter to use the platform specific SubDirectory
        NiImageConverter::SetPlatformSpecificSubdirectory(
                gExport.m_pcGenericTextureSaveDirectory);
        break;
    case NiSystemDesc::RENDERER_XENON:
        if (gExport.m_pcXenonTextureSaveDirectory != NULL)
        {
            char szNewDir[_MAX_PATH];
        
            NiSprintf(szNewDir, _MAX_PATH, "%s\\%s", m_pDir, 
                gExport.m_pcXenonTextureSaveDirectory);

            ReplaceChar(szNewDir, '/', '\\');
        
            CreateDirectoryStructure(szNewDir);

            NiStrcpy(m_szPlatformDirectory, _MAX_PATH,
                gExport.m_pcXenonTextureSaveDirectory);
        }

        // Setup the Image converter to use the platform specific SubDirectory
        NiImageConverter::SetPlatformSpecificSubdirectory(
            gExport.m_pcXenonTextureSaveDirectory);
        break;
    case NiSystemDesc::RENDERER_DX9:
        if (gExport.m_pcDX9TextureSaveDirectory != NULL)
        {
            char szNewDir[_MAX_PATH];
        
            NiSprintf(szNewDir, _MAX_PATH, "%s\\%s", m_pDir, 
                gExport.m_pcDX9TextureSaveDirectory);
            ReplaceChar(szNewDir, '/', '\\');
        
            CreateDirectoryStructure(szNewDir);

            NiStrcpy(m_szPlatformDirectory, _MAX_PATH,
                gExport.m_pcDX9TextureSaveDirectory);
        }

        // Setup the Image converter to use the platform specific SubDirectory
        NiImageConverter::SetPlatformSpecificSubdirectory(
            gExport.m_pcDX9TextureSaveDirectory);

        break;
    case NiSystemDesc::RENDERER_D3D10:
        if (gExport.m_pcD3D10TextureSaveDirectory != NULL)
        {
            char szNewDir[_MAX_PATH];
        
            NiSprintf(szNewDir, _MAX_PATH, "%s\\%s", m_pDir, 
                gExport.m_pcD3D10TextureSaveDirectory);
            ReplaceChar(szNewDir, '/', '\\');
        
            CreateDirectoryStructure(szNewDir);

            NiStrcpy(m_szPlatformDirectory, _MAX_PATH,
                gExport.m_pcD3D10TextureSaveDirectory);
        }

        // Setup the Image converter to use the platform specific SubDirectory
        NiImageConverter::SetPlatformSpecificSubdirectory(
            gExport.m_pcD3D10TextureSaveDirectory);

        break;
    case NiSystemDesc::RENDERER_PS3:
        if (gExport.m_pcPS3TextureSaveDirectory != NULL)
        {
            char szNewDir[_MAX_PATH];
        
            NiSprintf(szNewDir, _MAX_PATH, "%s\\%s", m_pDir, 
                gExport.m_pcPS3TextureSaveDirectory);
            ReplaceChar(szNewDir, '/', '\\');
        
            CreateDirectoryStructure(szNewDir);

            NiStrcpy(m_szPlatformDirectory, _MAX_PATH,
                gExport.m_pcPS3TextureSaveDirectory);
        }

        // Setup the Image converter to use the platform specific SubDirectory
        NiImageConverter::SetPlatformSpecificSubdirectory(
            gExport.m_pcPS3TextureSaveDirectory);

        break;
    case NiSystemDesc::RENDERER_WII:
        if (gExport.m_pcWiiTextureSaveDirectory != NULL)
        {
            char szNewDir[_MAX_PATH];
        
            NiSprintf(szNewDir, _MAX_PATH, "%s\\%s", m_pDir, 
                gExport.m_pcWiiTextureSaveDirectory);
            ReplaceChar(szNewDir, '/', '\\');
        
            CreateDirectoryStructure(szNewDir);

            NiStrcpy(m_szPlatformDirectory, _MAX_PATH,
                gExport.m_pcWiiTextureSaveDirectory);
        }

        // Setup the Image converter to use the platform specific SubDirectory
        NiImageConverter::SetPlatformSpecificSubdirectory(
            gExport.m_pcWiiTextureSaveDirectory);

        break;
    }
}
//---------------------------------------------------------------------------
void MyiTexture::CreateDirectoryStructure(char *pDirectory)
{
    char* pEnd = strstr(pDirectory, "\\");

    if (pEnd == NULL)
        return;

    // Skip the first slash
    pEnd = strstr(++pEnd, "\\");

    // Keep Building until you have made full directory path
    while(pEnd != NULL)
    {
        // Set this slash to NULL
        *pEnd = 0;

        _mkdir(pDirectory);

        // Reset back to a slash
        *pEnd = '\\';

        // move beyond the slash and find the next one
        pEnd = strstr(++pEnd, "\\");
    }


    // Try and make the last directory
    _mkdir(pDirectory);
}
//---------------------------------------------------------------------------
bool MyiTexture::CreateCubeMap()
{
    char acPosX[MAX_PATH];
    char acNegX[MAX_PATH];
    char acPosY[MAX_PATH];
    char acNegY[MAX_PATH];
    char acPosZ[MAX_PATH];
    char acNegZ[MAX_PATH];
    char acTextureFileName[_MAX_PATH];
    char acOriginalTextureFileName[_MAX_PATH];


    if (!GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Positive_X", acPosX,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Right", acPosX,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "right", acPosX,
            MAX_PATH))
    {
        return false;
    }

    if (!GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Negative_X", acNegX,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Left", acNegX,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "left", acNegX,
            MAX_PATH))
    {
        return false;
    }

    if (!GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Positive_Y", acPosY,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Top", acPosY,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "top", acPosY,
            MAX_PATH))
    {
        return false;
    }

    if (!GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Negative_Y", acNegY,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Bottom", acNegY,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "bottom", acNegY,
            MAX_PATH))
    {
        return false;
    }

    if (!GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Positive_Z", acPosZ,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Front", acPosZ,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "front", acPosZ,
            MAX_PATH))
    {
        return false;
    }

    if (!GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Negative_Z", acNegZ,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "Ni_CubeMap_Back", acNegZ,
            MAX_PATH) &&
        !GetCubeMapFileName(m_kTextureNode, "back", acNegZ,
            MAX_PATH))
    {
        return false;
    }

    ReplaceChar(acPosX, '/', '\\');
    ReplaceChar(acNegX, '/', '\\');
    ReplaceChar(acPosY, '/', '\\');
    ReplaceChar(acNegY, '/', '\\');
    ReplaceChar(acPosZ, '/', '\\');
    ReplaceChar(acNegZ, '/', '\\');

    // Verify these are self loading textures
    if ((IsSelfLoadTexture(acPosX) == NOT_SELF_LOADING) ||
        (IsSelfLoadTexture(acNegX) == NOT_SELF_LOADING) ||
        (IsSelfLoadTexture(acPosY) == NOT_SELF_LOADING) ||
        (IsSelfLoadTexture(acNegY) == NOT_SELF_LOADING) ||
        (IsSelfLoadTexture(acPosZ) == NOT_SELF_LOADING) ||
        (IsSelfLoadTexture(acNegZ) == NOT_SELF_LOADING))
    {
        DtExt_Err(
            "ERROR: Texture %s contains an unsupported texture format.\n",
            m_pName);

        return false;
    }


    // Adjust the Original file name of the cube map because it is nolonger 
    // valid.
    NiStrcpy(m_szOriginalTextureFileName, _MAX_PATH, "CubeMap:");

    // Strip to just the file name
    GetFileNameFromPath(acPosX, acTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName, _MAX_PATH, '\\');
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName);

    GetFileNameFromPath(acNegX, acTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName, _MAX_PATH, '\\');
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, ";");
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName);

    GetFileNameFromPath(acPosY, acTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName, _MAX_PATH, '\\');
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, ";");
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName);

    GetFileNameFromPath(acNegY, acTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName, _MAX_PATH, '\\');
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, ";");
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName);

    GetFileNameFromPath(acPosZ, acTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName, _MAX_PATH, '\\');
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, ";");
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName);

    GetFileNameFromPath(acNegZ, acTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName, _MAX_PATH, '\\');
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, ";");
    NiStrcat(m_szOriginalTextureFileName, _MAX_PATH, 
        acOriginalTextureFileName);


    switch(GetTextureLocation(TARGA))
    {
    case TEXTURE_SAVE_EXTERNAL_AS_ORIGINAL:
        CopyToDestinationDirectory(acPosX);
        CopyToDestinationDirectory(acNegX);
        CopyToDestinationDirectory(acPosY);
        CopyToDestinationDirectory(acNegY);
        CopyToDestinationDirectory(acPosZ);
        CopyToDestinationDirectory(acNegZ);

        // Strip to just the file name
        GetFileNameFromPath(acPosX, acTextureFileName, _MAX_PATH, 
            acOriginalTextureFileName, _MAX_PATH, '\\');
        NiStrcpy(acPosX, _MAX_PATH, acOriginalTextureFileName);

        GetFileNameFromPath(acNegX, acTextureFileName, _MAX_PATH, 
            acOriginalTextureFileName, _MAX_PATH, '\\');
        NiStrcpy(acNegX, _MAX_PATH, acOriginalTextureFileName);

        GetFileNameFromPath(acPosY, acTextureFileName, _MAX_PATH, 
            acOriginalTextureFileName, _MAX_PATH, '\\');
        NiStrcpy(acPosY, _MAX_PATH, acOriginalTextureFileName);

        GetFileNameFromPath(acNegY, acTextureFileName, _MAX_PATH, 
            acOriginalTextureFileName, _MAX_PATH, '\\');
        NiStrcpy(acNegY, _MAX_PATH, acOriginalTextureFileName);

        GetFileNameFromPath(acPosZ, acTextureFileName, _MAX_PATH, 
            acOriginalTextureFileName, _MAX_PATH, '\\');
        NiStrcpy(acPosZ, _MAX_PATH, acOriginalTextureFileName);

        GetFileNameFromPath(acNegZ, acTextureFileName, _MAX_PATH, 
            acOriginalTextureFileName, _MAX_PATH, '\\');
        NiStrcpy(acNegZ, _MAX_PATH, acOriginalTextureFileName);

        // Create the Cube Map
        m_spTexture = NiSourceCubeMap::Create(acPosX, acNegX, acPosY, acNegY, 
            acPosZ, acNegZ, NULL );

        break;
    case TEXTURE_SAVE_INTERNAL:
    case TEXTURE_SAVE_EXTERNAL_AS_NIF:
        {
            NiImageConverter* pNiIC = NiImageConverter::GetImageConverter();
            NiPixelDataPtr spPX = pNiIC->ReadImageFile(acPosX, NULL);
            NiPixelDataPtr spNX = pNiIC->ReadImageFile(acNegX, NULL);
            NiPixelDataPtr spPY = pNiIC->ReadImageFile(acPosY, NULL);
            NiPixelDataPtr spNY = pNiIC->ReadImageFile(acNegY, NULL);
            NiPixelDataPtr spPZ = pNiIC->ReadImageFile(acPosZ, NULL);
            NiPixelDataPtr spNZ = pNiIC->ReadImageFile(acNegZ, NULL);

            // Check for any of the textures failing to load
            if (!spPX || !spNX || !spPY || !spNY || !spPZ || !spNZ)
                return false;

            // Convert the Pixel Data
            ConvertPixelData(spPX);
            ConvertPixelData(spNX);
            ConvertPixelData(spPY);
            ConvertPixelData(spNY);
            ConvertPixelData(spPZ);
            ConvertPixelData(spNZ);

            // Verify the different faces of the cube map.
            if (VerifyCubeMapFormats(spPX, spNX, spPY, spNY, spPZ, spNZ, 
                acPosX, acNegX, acPosY, acNegY, acPosZ, acNegZ))
            {
                return false;
            }

            // Create the NIF files
            if (GetTextureLocation(TARGA) == TEXTURE_SAVE_EXTERNAL_AS_NIF)
            {
                SavePixelDataAsNIF(spPX, acPosX);
                SavePixelDataAsNIF(spNX, acNegX);
                SavePixelDataAsNIF(spPY, acPosY);
                SavePixelDataAsNIF(spNY, acNegY);
                SavePixelDataAsNIF(spPZ, acPosZ);
                SavePixelDataAsNIF(spNZ, acNegZ);

                // Strip to just the file name
                GetFileNameFromPath(acPosX, acTextureFileName, _MAX_PATH, 
                    acOriginalTextureFileName, _MAX_PATH, '\\');
                NiSprintf(acPosX, _MAX_PATH, "%s.nif", acTextureFileName);
                
                GetFileNameFromPath(acNegX, acTextureFileName, _MAX_PATH, 
                    acOriginalTextureFileName, _MAX_PATH, '\\');
                NiSprintf(acNegX, _MAX_PATH, "%s.nif", acTextureFileName);
                
                GetFileNameFromPath(acPosY, acTextureFileName, _MAX_PATH, 
                    acOriginalTextureFileName, _MAX_PATH, '\\');
                NiSprintf(acPosY, _MAX_PATH, "%s.nif", acTextureFileName);
                
                GetFileNameFromPath(acNegY, acTextureFileName, _MAX_PATH, 
                    acOriginalTextureFileName, _MAX_PATH, '\\');
                NiSprintf(acNegY, _MAX_PATH, "%s.nif", acTextureFileName);
                
                GetFileNameFromPath(acPosZ, acTextureFileName, _MAX_PATH, 
                    acOriginalTextureFileName, _MAX_PATH, '\\');
                NiSprintf(acPosZ, _MAX_PATH, "%s.nif", acTextureFileName);
                
                GetFileNameFromPath(acNegZ, acTextureFileName, _MAX_PATH, 
                    acOriginalTextureFileName, _MAX_PATH, '\\');
                NiSprintf(acNegZ, _MAX_PATH, "%s.nif", acTextureFileName);

                m_spTexture = NiSourceCubeMap::Create(acPosX, acNegX, acPosY, 
                    acNegY, acPosZ, acNegZ, NULL );
            }
            else
            {
                m_spTexture = NiSourceCubeMap::Create(spPX, spNX, spPY, spNY, 
                    spPZ, spNZ, NULL);
            }
        }
        break;
    }

    if (m_spTexture == NULL)
        return false;

    m_iWidth = 1;
    m_iHeight = 1;
    m_bCubeMap = true;

    return true;
}
//---------------------------------------------------------------------------
bool MyiTexture::GetCubeMapFileName(MObject kTextureNode, 
    const char* pcAttribute, char* pcFilePath, unsigned int uiBuffSize)
{
    // Find what the color is connected to 
    MObject kFaceMap = FollowInputAttribute(kTextureNode, 
        MString(pcAttribute));

    if (kFaceMap == MObject::kNullObj)
        return false;

    MFnDependencyNode dgNode(kFaceMap);

    // Create the plug for this attribute
    MStatus kStatus;
    MPlug kPlug;

    kPlug = dgNode.findPlug(MString("fileTextureName"), &kStatus);

    if (kStatus != MS::kSuccess)
        return false;

    MString kFilePath;
    kStatus = kPlug.getValue(kFilePath);

    NiStrcpy(pcFilePath, uiBuffSize, kFilePath.asChar());

    return true;
}
//---------------------------------------------------------------------------
bool MyiTexture::VerifyCubeMapFormats(NiPixelData* pkPosXRaw, 
    NiPixelData* pkNegXRaw, NiPixelData* pkPosYRaw, 
    NiPixelData* pkNegYRaw, NiPixelData* pkPosZRaw, 
    NiPixelData* pkNegZRaw, const char* pacPosX, const char* pacNegX,
    const char* pacPosY, const char* pacNegY, const char* pacPosZ,
    const char* pacNegZ)
{
    bool bError = false;

    // All Faces of a cube map must be square and the same size
    if (pkPosXRaw->GetWidth() != pkPosXRaw->GetHeight())
    {
        DtExt_Err("CUBE MAP: %s :: Positive X,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacPosX);
        bError = true;
    }

    if (pkPosXRaw->GetWidth() != pkNegXRaw->GetWidth())
    {
        DtExt_Err("CUBE MAP: %s :: Negative X,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacNegX);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkNegXRaw->GetHeight())
    {
        DtExt_Err("CUBE MAP: %s :: Negative X,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacNegX);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkPosYRaw->GetWidth())
    {
        DtExt_Err("CUBE MAP: %s :: Positive Y,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacPosY);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkPosYRaw->GetHeight())
    {
        DtExt_Err("CUBE MAP: %s :: Positive Y,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacPosY);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkNegYRaw->GetWidth())
    {
        DtExt_Err("CUBE MAP: %s :: Negative Y,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacNegY);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkNegYRaw->GetHeight())
    {
        DtExt_Err("CUBE MAP: %s :: Negative Y,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacNegY);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkPosZRaw->GetWidth())
    {
        DtExt_Err("CUBE MAP: %s :: Positive Z,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacPosZ);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkPosZRaw->GetHeight())
    {
        DtExt_Err("CUBE MAP: %s :: Positive Z,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacPosZ);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkNegZRaw->GetWidth())
    {
        DtExt_Err("CUBE MAP: %s :: Negative Z,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacNegZ);
        bError = true;
    }
    if (pkPosXRaw->GetWidth() != pkNegZRaw->GetHeight())
    {
        DtExt_Err("CUBE MAP: %s :: Negative Z,  All Textures of a cube map "
            "must be square and the same size.  %s is Invalid\n", 
            m_pName, pacNegZ);
        bError = true;
    }

    // Make Sure That Non of the Formats are Palletized
    if (pkPosXRaw->GetPixelFormat().GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        DtExt_Err("CUBE MAP: %s :: Positive X,  All Textures of a cube map "
            "must NOT be Palletized.  %s is Invalid\n", m_pName, pacPosX);
        bError = true;
    }

    if (pkNegXRaw->GetPixelFormat().GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        DtExt_Err("CUBE MAP: %s :: Negative X,  All Textures of a cube map "
            "must NOT be Palletized.  %s is Invalid\n",  m_pName, pacNegX);
        bError = true;
    }

    if (pkPosYRaw->GetPixelFormat().GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        DtExt_Err("CUBE MAP: %s :: Positive Y,  All Textures of a cube map "
            "must NOT be Palletized.  %s is Invalid\n", m_pName, pacPosY);
        bError = true;
    }

    if (pkNegYRaw->GetPixelFormat().GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        DtExt_Err("CUBE MAP: %s :: Negative Y,  All Textures of a cube map "
            "must NOT be Palletized.  %s is Invalid\n", m_pName, pacNegY);
        bError = true;
    }

    if (pkPosZRaw->GetPixelFormat().GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        DtExt_Err("CUBE MAP: %s :: Positive Z,  All Textures of a cube map "
            "must NOT be Palletized.  %s is Invalid\n", m_pName, pacPosZ);
        bError = true;
    }

    if (pkNegZRaw->GetPixelFormat().GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        DtExt_Err("CUBE MAP: %s :: Negative Z,  All Textures of a cube map "
            "must NOT be Palletized.  %s is Invalid\n", m_pName, pacNegZ);
        bError = true;
    }

    // All Faces of a cube map must be the same format
    if (pkPosXRaw->GetPixelFormat() != pkNegXRaw->GetPixelFormat())
    {
        DtExt_Err("CUBE MAP: %s :: Negative X,  All Textures of a cube map "
            "must be the same format.  %s is Invalid\n", m_pName, pacNegX);
        bError = true;
    }

    if (pkPosXRaw->GetPixelFormat() != pkPosYRaw->GetPixelFormat())
    {
        DtExt_Err("CUBE MAP: %s :: Positive Y,  All Textures of a cube map "
            "must be the same format.  %s is Invalid\n", m_pName, pacPosY);
        bError = true;
    }

    if (pkPosXRaw->GetPixelFormat() != pkNegYRaw->GetPixelFormat())
    {
        DtExt_Err("CUBE MAP: %s :: Negative Y,  All Textures of a cube map "
            "must be the same format.  %s is Invalid\n", m_pName, pacNegY);
        bError = true;
    }

    if (pkPosXRaw->GetPixelFormat() != pkPosZRaw->GetPixelFormat())
    {
        DtExt_Err("CUBE MAP: %s :: Positive Z,  All Textures of a cube map "
            "must be the same format.  %s is Invalid\n", m_pName, pacPosZ);
        bError = true;
    }

    if (pkPosXRaw->GetPixelFormat() != pkNegZRaw->GetPixelFormat())
    {
        DtExt_Err("CUBE MAP: %s :: Negative Z,  All Textures of a cube map "
            "must be the same format.  %s is Invalid\n", m_pName, pacNegZ);
        bError = true;
    }

    return bError;
}
//---------------------------------------------------------------------------
bool MyiTexture::CopyToDestinationDirectory(char* pcFilePath)
{
    char szDestinationDir[_MAX_PATH];
    char szTextureFileFullPath[_MAX_PATH];
    char szDestinationFullPath[_MAX_PATH];
    char szTextureFileName[_MAX_PATH];
    char szOriginalTextureFileName[_MAX_PATH];
    
    // Get the texture file name and build the path
    NiStrcpy(szDestinationDir, _MAX_PATH, m_pDir);

    CreatePlatformSpecificSubDirectoryAndSetConverter();

    NiStrcat(szDestinationDir, _MAX_PATH, "\\");
    NiStrcat(szDestinationDir, _MAX_PATH, m_szPlatformDirectory);

    // Get the File Name from the Path
    GetFileNameFromPath(pcFilePath, szTextureFileName, _MAX_PATH, 
        szOriginalTextureFileName, _MAX_PATH, '\\');


    // Create the Destination Full Path for loading the texture
    NiStrcpy(szDestinationFullPath, _MAX_PATH, szDestinationDir);
    NiStrcat(szDestinationFullPath, _MAX_PATH, "\\");
    NiStrcat(szDestinationFullPath, _MAX_PATH, szOriginalTextureFileName);
    ReplaceChar(szDestinationFullPath, '/', '\\');
    
    // Save the file name briefly
    NiStrcpy(szTextureFileFullPath, _MAX_PATH, pcFilePath);
    
    // Add in the seperators for MEL
    ReplaceChar(szTextureFileFullPath, '/', '\\');
    DuplicateChar(szTextureFileFullPath, _MAX_PATH, '\\');

    ReplaceChar(szDestinationDir, '/', '\\');
    DuplicateChar(szDestinationDir, _MAX_PATH, '\\');
    
    // External texture file ie, .TGA
    const unsigned int uiLen = _MAX_PATH + _MAX_PATH + 25;
    char szCommand[uiLen];
    
    // Move the texture to the destination directory
    NiSprintf( szCommand, uiLen, "system(\"copy \\\"%s\\\" \\\"%s\\\"\");", 
        szTextureFileFullPath, szDestinationDir);
    
    return(MGlobal::executeCommand( szCommand ) == MS::kSuccess);
}

//---------------------------------------------------------------------------
void MyiTexture::ConvertPixelData(NiPixelDataPtr& spPixelData)
{
    NiDevImageConverter::Platform ePlatform;

    switch(NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_GENERIC:
        ePlatform = NiDevImageConverter::ANY;
        break;
    case NiSystemDesc::RENDERER_XENON:
        ePlatform = NiDevImageConverter::XENON;
        break;
    case NiSystemDesc::RENDERER_PS3:
        ePlatform = NiDevImageConverter::PLAYSTATION3;
        break;
    case NiSystemDesc::RENDERER_WII:
        ePlatform = NiDevImageConverter::WII;
        break;
    case NiSystemDesc::RENDERER_DX9:
        ePlatform = NiDevImageConverter::DX9;
        break;
    case NiSystemDesc::RENDERER_D3D10:
        ePlatform = NiDevImageConverter::D3D10;
        break;
    default:
        ePlatform = NiDevImageConverter::ANY;
    }

    NiPixelFormat kFormat;
    if(m_iPlatformFormat == MyiExportGlobals::PLATFORM_DEFAULT)
    {
        // Apply the Platform Defaults
        switch(NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
        {
        case NiSystemDesc::RENDERER_GENERIC:
            m_iPlatformFormat = gExport.m_iGenericTextureFormat;
            break;
        case NiSystemDesc::RENDERER_XENON:
            m_iPlatformFormat = gExport.m_iXenonTextureFormat;
            break;
        case NiSystemDesc::RENDERER_DX9:
            m_iPlatformFormat = gExport.m_iDX9TextureFormat;
            break;
        case NiSystemDesc::RENDERER_D3D10:
            m_iPlatformFormat = gExport.m_iD3D10TextureFormat;
            break;
        case NiSystemDesc::RENDERER_PS3:
            m_iPlatformFormat = gExport.m_iPS3TextureFormat;
            break;
        case NiSystemDesc::RENDERER_WII:
            m_iPlatformFormat = gExport.m_iWiiTextureFormat;
            break;
        }
    }

    switch(m_iPlatformFormat)
    {
    case MyiExportGlobals::TEXTURE_DEFAULT:
        kFormat = spPixelData->GetPixelFormat();
        break;
    case MyiExportGlobals::TRUE_COLOR:
        if (NiDevImageQuantizer::ImageHasAlpha(spPixelData))
            kFormat = NiPixelFormat::RGBA32;
        else
            kFormat = NiPixelFormat::RGB24;
        break;
    case MyiExportGlobals::HIGH_COLOR:
        kFormat = NiPixelFormat::RGBA16;
        break;
    case MyiExportGlobals::PALETTE_8:
        if (NiDevImageQuantizer::ImageHasAlpha(spPixelData))
            kFormat = NiPixelFormat::PALA8;
        else
            kFormat = NiPixelFormat::PAL8;
        break;

    case MyiExportGlobals::COMPRESS1:
        kFormat = NiPixelFormat::DXT1;
        break;
    case MyiExportGlobals::COMPRESS3:
        kFormat = NiPixelFormat::DXT3;
        break;
    case MyiExportGlobals::COMPRESS5:
        kFormat = NiPixelFormat::DXT5;
        break;
    }

    // Lets check that we are not going over the global limit.
    int iSizeReduction = 0;
    unsigned int uiWidth = spPixelData->GetWidth();
    unsigned int uiHeight = spPixelData->GetHeight();
    unsigned int uiWidthMax = DtExt_MaxXTextureRes();
    unsigned int uiHeightMax = DtExt_MaxXTextureRes();
    while ((uiWidth > uiWidthMax || uiHeight > uiHeightMax) &&
        (uiWidth > 1 && uiHeight > 1))
    {
        uiWidth /= 2;
        uiHeight /= 2;
        iSizeReduction++;
    }

    if (iSizeReduction > m_iPlatformSizeReduction)
    {
        m_iPlatformSizeReduction = iSizeReduction;
    }

    unsigned int uiSizeDivideBy = 
        (unsigned int)NiPow(2.0f, (float)m_iPlatformSizeReduction);

    if(uiWidth != uiHeight && uiSizeDivideBy != 1)
    {
        DtExt_Err("\nTexture %s could not be reduced to a smaller size, "
            "because it is non-square.\n", m_pName);
    }

    char pcErrorBuffer[256] = "";

    NiPixelDataPtr spFinalPixels = NiDevImageQuantizer::ProcessPixelData(
        spPixelData, m_kFormatPrefs, m_bMipMap, true, kFormat, uiSizeDivideBy,
        ePlatform, m_szOriginalTextureFileName, pcErrorBuffer, 256);

    // Did the conversion fail?
    if (!spFinalPixels)
    {
        DtExt_Err(pcErrorBuffer);

        // Check for conversion failure
        spPixelData = NiNew NiPixelData(1, 1, NiPixelFormat::RGBA32);
        unsigned char* pPixels = spPixelData->GetPixels();
        
        *((unsigned int*)pPixels) = 0xffff00ff;

        return;
    }
    // Was a warning issued during conversion?
    else if (strlen(pcErrorBuffer) > 0)
    {
        // Technically not an error, but DtExt_Msg() doesn't always display
        // messages based on a debug flag.
        DtExt_Err(pcErrorBuffer);
    }

    spPixelData = spFinalPixels;
}
//---------------------------------------------------------------------------
void MyiTexture::SavePixelDataAsNIF(NiPixelData* pPixelData, char* pcFilePath)
{
    NIASSERT(pPixelData != NULL);

    // Try and create the platform specific directory
    CreatePlatformSpecificSubDirectoryAndSetConverter();
    
    char szTextureFileName[_MAX_PATH];
    char szOriginalTextureFileName[_MAX_PATH];

    // Get the File Name from the Path
    GetFileNameFromPath(pcFilePath, szTextureFileName, _MAX_PATH, 
        szOriginalTextureFileName, _MAX_PATH, '\\');

    // append on the NIF
    NiStrcat(szTextureFileName, _MAX_PATH, ".NIF");

    // Create the stream and save the texture
    NiStream TextureStream;
    TextureStream.InsertObject(pPixelData); 
 
    char* szPlatformName = NiImageConverter::
        ConvertFilenameToPlatformSpecific(szTextureFileName);
    TextureStream.Save( szPlatformName );
    NiFree( szPlatformName );
}
//---------------------------------------------------------------------------
bool MyiTexture::IsCubeMap(const char* pcFilePath)
{
    char acStandardFilename[NI_MAX_PATH];
    NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilePath);
    NiPath::Standardize(acStandardFilename);

    // check if the image is a DDS cubemap
    unsigned int uiFaceCount = 1;
    unsigned int uiW, uiH;
    NiPixelFormat kFmt;
    bool bMipmap;
    if (NiDevImageConverter::GetImageConverter()->ReadImageFileInfo(
        (const char*)&acStandardFilename, kFmt, bMipmap, uiW, uiH, uiFaceCount)
        == false)
    {
        return false;
    }

    if (uiFaceCount == 6)
        return true;
        
    return false;
}
//---------------------------------------------------------------------------
