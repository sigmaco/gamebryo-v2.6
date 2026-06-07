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

#include "MaxImmerse.h"
#include "NiMAXTextureConverter.h"
#include "NiMAXCustAttribConverter.h"
#include "MiUtils.h"

#include "NiPNGReader.h"
#include "NiFilename.h"
#define EXT_LEN 4
#define CODE_LEN 2

bool HashEq(const void *pSearch, const void *p, const void *p2Search, 
    const void *p2);
void HashFree(const void *p2, NiTexturePtr &spTex);
void CubeHashFree(const void *p2, NiSourceCubeMapPtr &spTex);

//---------------------------------------------------------------------------
NiTHash<NiTexturePtr>* NiMAXTextureConverter::ms_pkTexHTab = NULL;
NiTHash<NiSourceCubeMapPtr>* NiMAXTextureConverter::ms_pkCubeHTab = NULL;

bool NiMAXTextureConverter::ms_bSaveTexturesSeparately = true;
bool NiMAXTextureConverter::ms_bUseOrigExt = false;
bool NiMAXTextureConverter::ms_bUseOrigExtVRAM = false;
bool NiMAXTextureConverter::ms_bPerTextureOverride = false;
NiDevImageConverter::Platform NiMAXTextureConverter::ms_ePlatform = 
    NiDevImageConverter::ANY;
NiTexture::FormatPrefs::PixelLayout NiMAXTextureConverter::ms_ePixelLayout = 
    NiTexture::FormatPrefs::PIX_DEFAULT;
NiTexture::FormatPrefs::PixelLayout NiMAXTextureConverter::
    ms_eLocalPixelLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
void *NiMAXTextureConverter::ms_pvUserData = NULL;
bool NiMAXTextureConverter::ms_bDevImageQuantizerFound = false;
NiDDSReader* NiMAXTextureConverter::ms_pkDDSReader = NULL;

//---------------------------------------------------------------------------
NiMAXTextureConverter::NiMAXTextureConverter(
    TimeValue kAnimStart,
    TimeValue kAnimEnd)
{
    NIASSERT(NiDevImageConverter::NUM_PLATFORMS == 8);

    m_kAnimStart = kAnimStart;
    m_kAnimEnd = kAnimEnd;
}
//---------------------------------------------------------------------------

bool HashEq(const void *pSearch, const void *p, const void *, 
    const void *)
{
    // p2Search and p2 aren't used
    if (!strcmp((const char *) pSearch, (const char *) p))
        return(true);

    return(false);
}
//---------------------------------------------------------------------------

void HashFree(const void *, NiTexturePtr &spTex)
{
    spTex = NULL;
    // p2 isn't used
}
//---------------------------------------------------------------------------

void CubeHashFree(const void *, NiSourceCubeMapPtr &spTex)
{
    spTex = NULL;
}
//---------------------------------------------------------------------------
// For a given "uiSize" determine the largest power of two smaller than it.
static int NfResizeSub(unsigned int uiSize)
{
    unsigned int i;

    for (i = 2; uiSize >= i; i *= 2)
        ;

    i /= 2;
    if (i < 2)
        i = 2;

    return (i);
}

//---------------------------------------------------------------------------
// For a given "uiSize" determine the largest power of two smaller than it.
void NfResize(unsigned int uiXSize, unsigned int uiYSize, int *pXResult,
    int *pYResult)
{
    CHECK_MEMORY();
    unsigned int iXNew, iYNew;

    iXNew = NfResizeSub(uiXSize);
    iYNew = NfResizeSub(uiYSize);

    *pXResult = iXNew;
    *pYResult = iYNew;
}
//---------------------------------------------------------------------------
// Is "uiSize" a power of two
static bool NfIsPowerOfTwo(unsigned int uiSize)
{
    unsigned int i;

    if (uiSize != 1 && uiSize % 2)     // powers of two are always even
        return(false);

    for (i = 1; uiSize >= i; i *= 2)
    {
        if (uiSize == i)
            return(true);
    }

    return(false);
}
//---------------------------------------------------------------------------
#pragma optimize( "", off )
NiPixelData *NiMAXTextureConverter::ConvertColor64(
    Bitmap *pkSourceBitMap, 
    NiPixelFormat eFormat)
{
    CHECK_MEMORY();
    NiPixelData* pkPixelData;
    BMM_Color_64* pkPixels;
    int iX, iY; 

    pkPixelData = NiNew NiPixelData(
                            pkSourceBitMap->Width(),
                            pkSourceBitMap->Height(),
                            eFormat);
    if (pkPixelData == NULL)
        return(NULL);

    pkPixels = NiExternalNew BMM_Color_64[pkSourceBitMap->Width()];
    if (pkPixels == NULL)
    {
        NiDelete pkPixelData;
        return(NULL);
    }
    
    for (iY = 0; iY < pkSourceBitMap->Height(); iY++)
    {
        pkSourceBitMap->GetPixels(0, iY, 
                                 pkSourceBitMap->Width(), 
                                 pkPixels);
        for (iX = 0; iX < pkSourceBitMap->Width(); iX++)
        {
            unsigned char *pucTmp = (*pkPixelData)(iX, iY);
            float r,g,b,a;
            r = pkPixels[iX].r;
            g = pkPixels[iX].g;
            b = pkPixels[iX].b;
            a = pkPixels[iX].a;
            r = r / 257.0f;
            g = g / 257.0f;
            b = b / 257.0f;
            a = a / 257.0f;

            // 255.0f * r / 65535.0f
            pucTmp[0] = unsigned char (r);  
            pucTmp[1] = unsigned char (g);
            pucTmp[2] = unsigned char (b);
            if (FormatSupportsAlpha(eFormat))
                pucTmp[3] = unsigned char (a);
        }
    }
    
    NiDelete [] pkPixels;

    CHECK_MEMORY();
    return pkPixelData;
}
//---------------------------------------------------------------------------
NiPixelData *NiMAXTextureConverter::ConvertHDR(Bitmap *pkSourceBitMap)
{
    CHECK_MEMORY();
    NiPixelData* pkPixelData;
    BMM_Color_fl* pkPixels;
    int iX, iY; 

    pkPixelData = NiNew NiPixelData(pkSourceBitMap->Width(),
        pkSourceBitMap->Height(), NiPixelFormat::RGBA128);
    if (pkPixelData == NULL)
        return NULL;

    pkPixels = NiExternalNew BMM_Color_fl[pkSourceBitMap->Width()];
    if (pkPixels == NULL)
    {
        NiDelete pkPixelData;
        return NULL;
    }
    
    for (iY = 0; iY < pkSourceBitMap->Height(); iY++)
    {
        pkSourceBitMap->GetPixels(0, iY, pkSourceBitMap->Width(), pkPixels);
        for (iX = 0; iX < pkSourceBitMap->Width(); iX++)
        {
            unsigned char *pucTmp = (*pkPixelData)(iX, iY);
            float *pfTmp = (float*)pucTmp;
            pfTmp[0] = pkPixels[iX].r;
            pfTmp[1] = pkPixels[iX].g;
            pfTmp[2] = pkPixels[iX].b;
            pfTmp[3] = pkPixels[iX].a;
        }
    }
    
    NiDelete [] pkPixels;
    CHECK_MEMORY();

    
    return pkPixelData;
}
//---------------------------------------------------------------------------
NiPixelData *NiMAXTextureConverter::ShrinkColor64(
    Bitmap *pkSourceBitMap, 
    NiPixelFormat eFormat)
{
    CHECK_MEMORY();
    int iNewX, iNewY, iX, iY, iXSample, iYSample;
    NiPixelData *pkPixelData;
    BMM_Color_64 *pkPixels;
    float fXTerm, fYTerm;
    unsigned char *pucTmp;

    NfResize(pkSourceBitMap->Width(), pkSourceBitMap->Height(),
             &iNewX, &iNewY);

    fXTerm = (pkSourceBitMap->Width()-1.0f)/(iNewX - 1.0f);
    fYTerm = (pkSourceBitMap->Height()-1.0f)/(iNewY - 1.0f);

    pkPixelData = NiNew NiPixelData(iNewX, iNewY, eFormat);
    if (pkPixelData == NULL)
        return(NULL);

    pkPixels = NiExternalNew BMM_Color_64[pkSourceBitMap->Width()];
    if (pkPixels == NULL)
    {
        NiDelete pkPixelData;
        return(NULL);
    }

    for (iY = 0; iY < iNewY; iY++)
    {
        iYSample = int(iY * fYTerm);

        pkSourceBitMap->GetPixels(0, iYSample, 
                                 pkSourceBitMap->Width(), 
                                 pkPixels);
        for (iX = 0; iX < iNewX; iX++)
        {
            pucTmp = (*pkPixelData)(iX, iY);
            
            iXSample = int(iX * fXTerm);

            NIASSERT(iXSample < pkSourceBitMap->Width());

            // 255 * r / 65535
            pucTmp[0] = unsigned char (pkPixels[iXSample].r / 257.0f); 
            pucTmp[1] = unsigned char (pkPixels[iXSample].g / 257.0f);
            pucTmp[2] = unsigned char (pkPixels[iXSample].b / 257.0f);
            if (FormatSupportsAlpha(eFormat))
                pucTmp[3] = unsigned char (pkPixels[iXSample].a / 257.0f);
        }
    }
    
    NiDelete [] pkPixels;

    CHECK_MEMORY();
    return pkPixelData;
}
//---------------------------------------------------------------------------
NiPixelData *NiMAXTextureConverter::ShrinkHDR(Bitmap *pkSourceBitMap)
{
    // This method handles non-power-of-two High Dynamic Range textures
    // they are sampled to the largest power of two texture that is smaller
    // than the original.
    CHECK_MEMORY();
    int iNewX, iNewY, iX, iY, iXSample, iYSample;
    NiPixelData *pkPixelData;   // our pixel data output to be filled
    BMM_Color_fl *pkPixels;     // max's HDR pixel format
    float fXTerm, fYTerm;       // the ratio of old to new image dimensions
    unsigned char *pucTmp;      // stores the byte data of the source bitmap
    float *pfTmp;               // assigns float data to the address of pucTmp

    NfResize(pkSourceBitMap->Width(), pkSourceBitMap->Height(),
             &iNewX, &iNewY);

    fXTerm = (pkSourceBitMap->Width() - 1.0f) / (iNewX - 1.0f);
    fYTerm = (pkSourceBitMap->Height() - 1.0f) / (iNewY - 1.0f);

    pkPixelData = NiNew NiPixelData(iNewX, iNewY, NiPixelFormat::RGBA128);
    if (pkPixelData == NULL)
        return NULL;

    pkPixels = NiExternalNew BMM_Color_fl[pkSourceBitMap->Width()];
    if (pkPixels == NULL)
    {
        NiDelete pkPixelData;
        return NULL;
    }

    for (iY = 0; iY < iNewY; iY++)
    {
        iYSample = int(iY * fYTerm);

        pkSourceBitMap->GetPixels(0, iYSample, pkSourceBitMap->Width(), 
            pkPixels);
        for (iX = 0; iX < iNewX; iX++)
        {
            pucTmp = (*pkPixelData)(iX, iY);
            pfTmp = (float*)pucTmp;
            
            iXSample = int(iX * fXTerm);

            pfTmp[0] = pkPixels[iXSample].r; 
            pfTmp[1] = pkPixels[iXSample].g;
            pfTmp[2] = pkPixels[iXSample].b;
            pfTmp[3] = pkPixels[iXSample].a;
        }
    }
    NiDelete [] pkPixels;

    CHECK_MEMORY();
    return pkPixelData;
}
#pragma optimize( "", on ) 
//---------------------------------------------------------------------------
int NiMAXTextureConverter::ConvertBitmap(
    Bitmap* pkSourceBitMap, 
    BitmapInfo* pkBI,
    const TCHAR* pcFilePath,
    const TCHAR* pcFileName,
    NiTexture::FormatPrefs& kPrefs,
    NiTexture*& pkTexture, 
    NiPixelFormat eFormat,
    unsigned int& uiSizeDivideBy, 
    NiTObjectArray<NiString>*& pkFilenames,
    bool bSaveTexturesSeparately, 
    bool bUseOrigExt,
    bool bUseOrigExtVRAM,
    bool bAutoWriteTextures,
    bool bUseAlpha)
{
    CHECK_MEMORY();
    pkTexture = NULL;
    if(NiMAXConverter::GetViewerRun())
    {
        bSaveTexturesSeparately = false;
        bUseOrigExt = false;
    }

    bool bAlreadyConverted = false;

    if (!(NfIsPowerOfTwo(pkBI->Width()) && NfIsPowerOfTwo(pkBI->Height()))
        && !(NiMAXOptions::GetBool(NI_ALLOW_NPOT_TEXTURES)))
    {    
        char acString[1024];
        NiSprintf(acString, 1024,
            "Texture \"%s\" is not a power of two texture.\n"
            "A sampled version will be exported.", 
            pkBI->Filename());
        NILOGWARNING(acString);
    }
    else
    {
        bAlreadyConverted = ConvertBitmapByKnownDevice(pkSourceBitMap, pkBI, 
            pcFilePath, pcFileName, kPrefs,
            pkTexture, eFormat, uiSizeDivideBy, pkFilenames, 
            bSaveTexturesSeparately, 
            bUseOrigExt, bUseOrigExtVRAM, bAutoWriteTextures, bUseAlpha);

        if (bAlreadyConverted)
            return (W3D_STAT_OK);
    }

    bAlreadyConverted = ConvertBitmapByUnknownDevice(pkSourceBitMap, pkBI,
        pcFilePath, pcFileName, kPrefs, pkTexture, eFormat, uiSizeDivideBy,
        pkFilenames, bSaveTexturesSeparately, bUseOrigExt, bUseOrigExtVRAM,
        bAutoWriteTextures, bUseAlpha);

    if (bAlreadyConverted)
    {
        return (W3D_STAT_OK);
    }
    else
    {
        char acString[1024];
        NiSprintf(acString, 1024, "Failed sampling the Max version of the "
            "texture, \"%s\"."
            " It will not be exported.", 
            pkBI->Filename());
        NILOGWARNING(acString);

        NiDelete pkTexture;
        pkTexture = NULL;
        return (W3D_STAT_FAILED);
    }
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::SupportLegacyMipMapSpecification(
    Bitmap* pkSourceBitMap,
    BitmapInfo* pkBI,
    NiTexture::FormatPrefs&,
    NiTexture*&,  
    NiPixelFormat,
    unsigned int&, 
    NiTObjectArray<NiString>*&,
    bool&, 
    bool&,
    bool&)
{
    int iNumPalColors = pkSourceBitMap->Paletted();
        
    NiImageConverter::GetImageConverter();
    // calling pkSourceBitMap->Paletted() isn't always sufficient
    if (iNumPalColors == 0 && pkBI->Type() == BMM_PALETTED)
        iNumPalColors = 256;

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::ConvertBitmapByKnownDevice(
    Bitmap* pkSourceBitMap,
    BitmapInfo* pkBI,
    const TCHAR* pcFilePath,
    const TCHAR* pcFileName,
    NiTexture::FormatPrefs& kPrefs,
    NiTexture*& pkTexture,
    NiPixelFormat kDestFormat,
    unsigned int& uiSizeDivideBy, 
    NiTObjectArray<NiString>*& pkFilenames,
    bool bSaveTexturesSeparately, 
    bool bUseOrigExt,
    bool bUseOrigExtVRAM,
    bool bAutoWriteTextures,
    bool bUseAlpha)
{
    if (BMMGetFullFilename(pkBI) == TRUE)
    {
        if ((pcFilePath == NULL) || (pcFileName == NULL) || 
            (pcFilePath[0] == 0) || (pcFileName[0] == 0))
        {
            pcFilePath = pkBI->Name();
            pcFileName = pkBI->Filename();
        }

        NiDevImageQuantizer* pkConverter = 
            (NiDevImageQuantizer*) NiImageConverter::GetImageConverter();
        NIASSERT(pkConverter);
        if (pkConverter->CanReadImageFile(pcFilePath))
        {
            NiOutputDebugString("Convert Bitmap By Device..\n\t");
            NiOutputDebugString(pcFilePath);
            NiOutputDebugString("\n");        

            NiPixelFormat kOriginalFormat;
            bool bMipmap; 
            unsigned int uiWidth; 
            unsigned int uiHeight;
            pkTexture = NULL;
            bool bRequiresConversion = false;
            unsigned int uiNumFaces;
#ifdef NIDEBUG
            bool bSuccess = 
#endif
                pkConverter->ReadImageFileInfo(pcFilePath, 
                kOriginalFormat, bMipmap, uiWidth, uiHeight, uiNumFaces);
            NIASSERT(bSuccess);

            if (pkFilenames == NULL)
            {
                SupportLegacyMipMapSpecification(pkSourceBitMap, pkBI, kPrefs,
                    pkTexture, kDestFormat, uiSizeDivideBy, pkFilenames, 
                    bSaveTexturesSeparately, bUseOrigExt, bUseOrigExtVRAM);
            }

            if (kOriginalFormat.GetFormat() == NiPixelFormat::FORMAT_RGBA && 
                kDestFormat.GetFormat() == NiPixelFormat::FORMAT_PAL)
            {
                kDestFormat = NiPixelFormat::PALA8;
            }

            if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP &&
                bUseOrigExt)
            {
                kDestFormat = kOriginalFormat;                
            }
            else if (kPrefs.m_ePixelLayout != 
                NiTexture::FormatPrefs::PIX_DEFAULT && 
                kDestFormat != kOriginalFormat)
            {
                bRequiresConversion = true;
            }
            else if (kPrefs.m_ePixelLayout == 
                NiTexture::FormatPrefs::PIX_DEFAULT)
            {
                kDestFormat = kOriginalFormat;                
            }

            if (uiSizeDivideBy != 1)
                bRequiresConversion = true;

            if (bRequiresConversion && (bUseOrigExt || bUseOrigExtVRAM))
            {
                char acString[1024];
                NiSprintf(acString, 1024, 
                    "Texture conversion has been requested"
                    " for image, \"%s\",\n "
                    "but you have also specified using the original file"
                    " when loading."
                    "\nIn order to convert this texture, it will no longer"
                    " use the original file upon export\n", 
                    pkBI->Filename());
                NILOGWARNING(acString);

                bUseOrigExt = false;
            }

            if (bUseOrigExt || bUseOrigExtVRAM)
            {
                // check if the image is a DDS cubemap
                unsigned int uiFaceCount = 1;
                // get filename extension
                char acStandardFilename[NI_MAX_PATH];
                NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilePath);
                NiPath::Standardize(acStandardFilename);
                NiFilename kFileName(acStandardFilename);
                const char* pcExt = kFileName.GetExt();

                if (ms_pkDDSReader->CanReadFile(pcExt))
                {
                    NiFile* pkFile = NiFile::GetFile(acStandardFilename, 
                        NiFile::READ_ONLY);
                    unsigned int uiW;
                    unsigned int uiH;
                    bool bMipMap;
                    NiPixelFormat kFormat;
                    ms_pkDDSReader->ReadHeader(*pkFile, uiW, uiH, kFormat, 
                        bMipMap, uiFaceCount);
                    NiDelete pkFile;
                }

                bool bUsePreload = NiSourceTexture::GetUsePreloading();
                NiSourceTexture::SetUsePreloading(false);
                if (uiFaceCount == 6)
                {
                    pkTexture = NiSourceCubeMap::Create(pcFileName, NULL, 
                        kPrefs);
                }
                else
                {
                    pkTexture = NiSourceTexture::Create(pcFileName, kPrefs);
                }
                NiSourceTexture::SetUsePreloading(bUsePreload);

                if (bUseOrigExtVRAM && pkTexture != NULL)
                {
                    ((NiSourceTexture*)pkTexture)->
                        SetLoadDirectToRendererHint(true);
                }

                return true;
            }
            NiPixelDataPtr spPixelData =  NULL;
            
            // Generate the mip-map pyramid from a group of files
            if (pkFilenames != NULL && kPrefs.m_eMipMapped == 
                NiTexture::FormatPrefs::YES)
            {
                const char* apcFilenames[12];
                unsigned int uiNumLevels = 
                    NiPixelData::ComputeMipmapLevels(pkBI->Width(), 
                    pkBI->Height());

                NIASSERT(uiNumLevels > 1);
                unsigned int ui = 0;
                unsigned int uj = 0;

                // make sure the current bitmap is mip level 0
                if (!pkFilenames->GetAt(ui).EqualsNoCase(pcFilePath))
                {
                    apcFilenames[ui] = pcFilePath;
                    ui++;
                }

                // grab the filenames
                for (; ui < uiNumLevels; ui++, uj++)
                {
                    NiString strMipName = pkFilenames->GetAt(uj);
                    if (strMipName == "")                   
                        break;
                    apcFilenames[ui] = strMipName;
                }

                spPixelData = pkConverter->GenerateMipmapPyramidFromFiles(
                    (const char**)apcFilenames, 
                    uiNumLevels);
    
                NIASSERT(spPixelData);
                if (!spPixelData)
                    return false;
            }
            else 
            {
                spPixelData = pkConverter->ReadImageFile(pcFilePath, NULL);
                NIASSERT(spPixelData);
                if (!spPixelData)
                    return false;
            }

            bool bMipMapOn = false;
            
            if (kPrefs.m_eMipMapped == NiTexture::FormatPrefs::YES && 
                spPixelData && NiPixelData::ComputeMipmapLevels(
                spPixelData->GetWidth(), spPixelData->GetHeight()) > 1)
            {
                bMipMapOn = true;
            }

            TSTR strSaveName;
            TSTR strName = pcFileName;
            TSTR strNameMinusExt;
            TSTR strExt;
            SplitFilename(strName, NULL, &strNameMinusExt, &strExt);
            TSTR strFullName = pkBI->Filename();
 
            char acErrorBuff[1024];
            acErrorBuff[0] = '\0';
            spPixelData = NiDevImageQuantizer::ProcessPixelData(spPixelData, 
                kPrefs, bMipMapOn, bUseAlpha, kDestFormat, uiSizeDivideBy, 
                ms_ePlatform, strFullName, acErrorBuff, 1024);

            if (strlen(acErrorBuff) > 0)
                NILOGWARNING(acErrorBuff);
                  
            if (!spPixelData)
                return false;
            
            bool bCubeMap = spPixelData->GetNumFaces() == 6;

            if (bSaveTexturesSeparately && spPixelData)
            {
                NiOutputDebugString("Saving external nif..");
                NiOutputDebugString(strName);
                NiOutputDebugString("\n");
                
                // stream external nif
                strName = strNameMinusExt + _T(".nif");
                strSaveName = 
                    NiImageConverter::ConvertFilenameToPlatformSpecific(
                    strName);

                if (bAutoWriteTextures)
                {
                    NiStream stream;
                    stream.InsertObject(spPixelData);
                    stream.SetSaveAsLittleEndian(
                        NiDevImageConverter::IsLittleEndian(ms_ePlatform));
                    stream.Save(strSaveName);
                }

                // create the source texture to hold the nif texture
                bool bUsePreload = NiSourceTexture::GetUsePreloading();
                NiSourceTexture::SetUsePreloading(false);
                if (bCubeMap)
                {
                    pkTexture = NiSourceCubeMap::Create((const char*)strName,
                        NULL, kPrefs);
                }
                else
                {
                    pkTexture = NiSourceTexture::Create(
                        (const char*)strName, kPrefs);
                }
                NiSourceTexture::SetUsePreloading(bUsePreload);
                NIASSERT(pkTexture);
                return true;
            }
            else if (spPixelData)
            {
                NiOutputDebugString("Saving internal nif texture..");
                NiOutputDebugString(strName);
                NiOutputDebugString("\n");

                // create the source texture to hold the nif texture
                bool bUsePreload = NiSourceTexture::GetUsePreloading();
                NiSourceTexture::SetUsePreloading(false);
                if (bCubeMap)
                {
                    pkTexture = NiSourceCubeMap::Create(spPixelData,
                        NULL, kPrefs);
                }
                else
                {
                    pkTexture = NiSourceTexture::Create(spPixelData, kPrefs);
                }
                NIASSERT(pkTexture);
                ((NiSourceTexture*) pkTexture)->SetFilename((const char*)
                    strName);
                NiSourceTexture::SetUsePreloading(bUsePreload);    
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::ConvertBitmapByUnknownDevice(
    Bitmap* pkSourceBitMap,
    BitmapInfo* pkBI,
    const TCHAR*,
    const TCHAR* pcFileName,
    NiTexture::FormatPrefs& kPrefs,
    NiTexture*& pkTexture,
    NiPixelFormat kDestFormat,
    unsigned int& uiSizeDivideBy, 
    NiTObjectArray<NiString>*&,
    bool bSaveTexturesSeparately, 
    bool bUseOrigExt,
    bool,
    bool bAutoWriteTextures,
    bool bUseAlpha)
{
    bUseOrigExt = false;
    NiPixelDataPtr spPixelData = NULL;
    if (NfIsPowerOfTwo(pkBI->Width()) && NfIsPowerOfTwo(pkBI->Height()))
    {
        int iNumPalColors = pkSourceBitMap->Paletted();
            
        // calling pkSourceBitMap->Paletted() isn't always sufficient
        if (iNumPalColors == 0 && pkBI->Type() == BMM_PALETTED)
            iNumPalColors = 256;

        if (iNumPalColors)
        {
            BMM_Color_48 *pkPal;
            BYTE *pkPixels;
            int iX, iY;

            if (FormatSupportsAlpha(kDestFormat))
            {
                // If NOT png, then set spPixelData
                spPixelData = NiNew NiPixelData(pkBI->Width(), pkBI->Height(),
                    NiPixelFormat::PALA8);
            }
            else
            {
                spPixelData = NiNew NiPixelData(pkBI->Width(), pkBI->Height(),
                    NiPixelFormat::PAL8);
            }

            if (spPixelData == NULL)
                return false;

            pkPal = NiExternalNew BMM_Color_48[iNumPalColors];
            if (pkPal == NULL)
            {
                
                return false;
            }

            pkPixels = NiAlloc(BYTE, pkBI->Width());
            if (pkPixels == NULL)
            {
                NiDelete pkPal;
                
                return false;
            }
        
            pkSourceBitMap->GetPalette(0, iNumPalColors, pkPal);

            NiPalette *pkPalette = NiNew NiPalette(FormatSupportsAlpha(
                kDestFormat), iNumPalColors);
        
            NIASSERT(pkPalette);
            NiPalette::PaletteEntry* pEntry = pkPalette->GetEntries();
            for (iY = 0; iY < iNumPalColors; iY++, pEntry++)
            {
                // 255 * r / 65535f
                pEntry->m_ucRed   = unsigned char (pkPal[iY].r / 257.0f); 
                pEntry->m_ucGreen = unsigned char (pkPal[iY].g / 257.0f);
                pEntry->m_ucBlue  = unsigned char (pkPal[iY].b / 257.0f); 
                pEntry->m_ucAlpha = 255;
            }

            NiDelete [] pkPal;
        
            for (iY = 0; iY < pkBI->Height(); iY++)
            {
                pkSourceBitMap->GetIndexPixels(0, iY, 
                                              pkBI->Width(), 
                                              pkPixels);
                for (iX = 0; iX < pkBI->Width(); iX++)
                    *(*spPixelData)(iX, iY) = pkPixels[iX];
            }
          
            NiFree(pkPixels);

            spPixelData->SetPalette(pkPalette);
            kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::PALETTIZED_8;
        }
        else if (pkSourceBitMap->IsHighDynamicRange())
        {
            spPixelData = ConvertHDR(pkSourceBitMap);
            kDestFormat = NiPixelFormat::RGBA64;
            if (spPixelData == NULL)
                return false;
        }
        else
        {
            spPixelData = ConvertColor64(pkSourceBitMap, 
                NiPixelFormat::RGBA32);
            if (spPixelData == NULL)
                return false;
        }
    }
    else
    {
        if (pkSourceBitMap->IsHighDynamicRange())
        {
            spPixelData = ShrinkHDR(pkSourceBitMap);
            if (spPixelData == NULL)
                return false;
        }
        else
        {
            spPixelData = ShrinkColor64(pkSourceBitMap, NiPixelFormat::RGBA32);
            if (spPixelData == NULL)
                return false;
        }
    }

    NiDevImageConverter* pkConverter;
    pkConverter = 
        (NiDevImageConverter*)NiImageConverter::GetImageConverter();

    bool bMipMapOn = kPrefs.m_eMipMapped == NiTexture::FormatPrefs::YES;
    // construct filename to save
    TSTR strSaveName;
    TSTR strName = pcFileName;
    TSTR strNameMinusExt;
    TSTR strExt;
    TSTR strFullName = pkBI->Filename();

    char acErrorBuff[1024];
    acErrorBuff[0] = '\0';
    spPixelData = NiDevImageQuantizer::ProcessPixelData(spPixelData, kPrefs, 
        bMipMapOn, bUseAlpha, kDestFormat, uiSizeDivideBy, ms_ePlatform, 
        strName, acErrorBuff, 1024);

    if (strlen(acErrorBuff) > 0)
        NILOGWARNING(acErrorBuff);

    if (pkBI->NumberFrames() > 1 )
    {
        // animated bitmap
        strName = GetFrameFilename(strFullName, pkBI->CurrentFrame());
    }
    else
    {
            char acString[1024];
            NiSprintf(acString, 1024, "Texture \"%s\" is not a commonly "
                "supported Gamebryo type.\n"
                "Resorting to sampling the Max version of the texture.\n", 
                pcFileName);
            NILOGWARNING(acString);
    }

    SplitFilename(strName, NULL, &strNameMinusExt, &strExt);
    
    // create NiTexture from NiPixelData using kPrefs.
    if (!bSaveTexturesSeparately)
    {
        pkTexture = NiSourceTexture::Create(spPixelData, kPrefs);
        ((NiSourceTexture*)pkTexture)->SetFilename(
            (const char*)(strNameMinusExt + strExt));
    }
    else if (bSaveTexturesSeparately)
    {
         // stream external nif
        strName = strNameMinusExt + _T(".nif");
        strSaveName = 
            NiImageConverter::ConvertFilenameToPlatformSpecific(strName);
        
        if (bAutoWriteTextures)
        {
            NiStream stream;
            stream.InsertObject(spPixelData);
            stream.SetSaveAsLittleEndian(
                NiDevImageConverter::IsLittleEndian(ms_ePlatform));
            stream.Save(strSaveName);
        }

        bool bUsePreload = NiSourceTexture::GetUsePreloading();
        NiSourceTexture::SetUsePreloading(false);
        pkTexture = NiSourceTexture::Create((const char*)strName, kPrefs);
        NiSourceTexture::SetUsePreloading(bUsePreload);
    }

    CHECK_MEMORY();
    return true;
}
//---------------------------------------------------------------------------
TSTR NiMAXTextureConverter::GetFrameFilename(TSTR kFullname, int iFrame)
{
    // note: returns no path or extension

    CHECK_MEMORY();
    TSTR kName;
    SplitFilename(kFullname, NULL, &kName, NULL);
    char acNum[5];
    NiSprintf(acNum, 5, "_%02d", iFrame);

    return kName + _T(acNum);
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
int NiMAXTextureConverter::GetTexture(
    Bitmap* pkBitMap,
    BitmapInfo& kBI,
    const TCHAR* pcFilePath,
    const TCHAR* pcFileName,
    NiTexture*& pkTex, 
    NiTexture::FormatPrefs& kPrefs,
    NiPixelFormat eFormat, 
    unsigned int& uiSizeDivideBy,
    NiTObjectArray<NiString>*& pkFilenames,
    bool bSaveTexturesSeparately,
    bool bUseOrigExt,
    bool bUseOrigExtVRAM,
    bool bAutoWriteTextures,
    bool bUseAlpha)
{
    CHECK_MEMORY();
    
    void* pvUnused = 0;
    pkTex = ms_pkTexHTab->NameRetrieve((char*)pcFileName, pvUnused);
    if (!pkTex)
    {
        if (!pkBitMap)
            pkBitMap = TheManager->Load(&kBI);

        if (!pkBitMap)
            return(W3D_STAT_FAILED);

        int iStatus = ConvertBitmap(pkBitMap, &kBI, pcFilePath, pcFileName,
            kPrefs, pkTex, eFormat, uiSizeDivideBy, pkFilenames, 
            bSaveTexturesSeparately, bUseOrigExt, bUseOrigExtVRAM,
            bAutoWriteTextures, bUseAlpha);
        
        if (iStatus)
            return(iStatus);

        ms_pkTexHTab->NameInsert((char*)pcFileName, pvUnused, pkTex);
    }

    CHECK_MEMORY();
    if (!pkTex)
        return(W3D_STAT_NO_MEMORY);
    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXTextureConverter::GetAnimatedTexture(
    Bitmap* pkBitMap, 
    BitmapInfo& kBI,
    NiTexture*&,
    NiTexture::FormatPrefs& kPrefs, 
    NiPixelFormat eFormat,
    NiFlipController*& pkFlipper, 
    float fStart, 
    float fRate,
    int iOOB,
    unsigned int& uiSizeDivideBy, 
    NiTObjectArray<NiString>*& pkFilenames,
    bool bSaveTexturesSeparately,
    bool bUseOrigExt,
    bool bUseOrigExtVRAM,
    bool bAutoWriteTextures,
    bool bUseAlpha)
{
    bool bIFL = false;
    if (!_tcscmp(kBI.Device(), _T("IFL Image File")))
        bIFL = true;

    NIASSERT(kBI.NumberFrames() > 1); // animated bitmap

    kBI.GetCustomStep();
    int iTotalNumFrames = kBI.NumberFrames();
    int iStart = kBI.FirstFrame();
    int iEnd = kBI.LastFrame();
    int *piConversion = NiAlloc(int, iTotalNumFrames);
    unsigned int uiKeyTotal = 0;
    TSTR *strLoadedNames = NiExternalNew TSTR[iTotalNumFrames];
    
    pkFlipper = NiNew NiFlipController;

    if (NiMAXConverter::GetUseAppTime())
        pkFlipper->SetAnimType(NiTimeController::APP_TIME);
    else
        pkFlipper->SetAnimType(NiTimeController::APP_INIT);

    NiTimeController::CycleType eCycleType = NiTimeController::CLAMP;
    switch (iOOB)
    {
        case END_PINGPONG:
            eCycleType = NiTimeController::REVERSE;
            break;
        case END_HOLD:
            eCycleType = NiTimeController::CLAMP;
            break;
        case END_LOOP:
        default:
            eCycleType = NiTimeController::LOOP;
            break;
    }
    
    pkFlipper->SetCycleType(eCycleType);
    float fSecsPerFrame = GetTicksPerFrame()/(fRate * TICKSPERSECOND);

    TSTR pHashLastName = "";
    unsigned int uiUnique = 0;
    int iNumFrames = 0;

    for (int i = iStart;  i <= iEnd; i++, iNumFrames++)
    {
        kBI.SetCurrentFrame(i);
        BMMRES kRes;             
        
        // IFLs are handled slightly differently than
        // AVIs or other flipbooks.
        if (bIFL)
            kRes = TheManager->LoadInto(&kBI, &pkBitMap, TRUE);
        else
            kRes = pkBitMap->GoTo(&kBI);

        if (kRes != BMMRES_SUCCESS)
            break;   // error

        BitmapInfo kSubBI = pkBitMap->Storage()->bi;

        const TCHAR* pcFilePath = NULL;
        if (BMMGetFullFilename(&kSubBI) == TRUE)
        {
            pcFilePath = kSubBI.Name();
        }

        // construct string to look up shared NiTexture
        TSTR pHashName;
        if (bIFL)
            pHashName = kSubBI.Filename();
        else
            pHashName = GetFrameFilename(kBI.Filename(), kBI.CurrentFrame());

        bool bAlreadyLoaded = false;
        for (unsigned int uiCounter = 0; uiCounter < uiUnique; uiCounter++)
        {
            if (strLoadedNames[uiCounter] == pHashName)
            {
                piConversion[i] = uiCounter;
                bAlreadyLoaded = true;
                break;
            }
        }
        if (pHashName != pHashLastName)
            uiKeyTotal++;
        pHashLastName = pHashName;
        if (bAlreadyLoaded)
            continue;

        strLoadedNames[uiUnique] = pHashName;
        piConversion[i] = uiUnique;
        
        NiTexture* pkTex;
        int iStatus = GetTexture(pkBitMap, kBI, pcFilePath, pHashName,  
            pkTex, kPrefs, eFormat, uiSizeDivideBy, pkFilenames, 
            bSaveTexturesSeparately, bUseOrigExt, bUseOrigExtVRAM, 
            bAutoWriteTextures, bUseAlpha);
         
        if (iStatus)
            break;

        pkFlipper->AttachTexture(pkTex, uiUnique);
        uiUnique++;
    }

    NiStepFloatKey* pkFloatKeys = NiNew NiStepFloatKey[uiKeyTotal + 1];
    unsigned int uiKeyCount = 0;
    if (bIFL)
    {
        for (unsigned int ui = 0; ui < (unsigned int)iNumFrames; ui++)
        {
            if ((uiKeyCount == 0) || 
                (pkFloatKeys[uiKeyCount - 1].GetValue() != piConversion[ui]))
            {
                pkFloatKeys[uiKeyCount].SetTime(fStart + (float)ui * 
                    fSecsPerFrame);
                pkFloatKeys[uiKeyCount].SetValue((float) piConversion[ui]);
                uiKeyCount++;
            }
        }
    }
    else
    {
        for (unsigned int ui = 0; ui < uiKeyTotal; ui++)
        {
            pkFloatKeys[ui].SetTime(fStart + (float)ui * fSecsPerFrame);
            pkFloatKeys[ui].SetValue((float) ui);
        }
    }

    // We have to add in one last key that forms our end point,
    // it must be a duplicate of the last texture index to ensure
    // clamping or looping works correctly
    pkFloatKeys[uiKeyTotal].SetTime(fStart + (float)(iNumFrames) 
        * fSecsPerFrame);
    pkFloatKeys[uiKeyTotal].SetValue((float) piConversion[iNumFrames - 1]);
    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator;
    pkInterp->SetKeys(pkFloatKeys, uiKeyTotal + 1, NiFloatKey::STEPKEY);
    pkFlipper->SetInterpolator(pkInterp);
    pkFlipper->ResetTimeExtrema();

    NiDelete [] strLoadedNames;
    NiFree(piConversion);
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
// Convert a texture map from "pkTm" into an NiTexturingProperty::Map.
// 
// A non-zero return value indicates an error.
// A return value of W3D_STAT_OK indicates successful translation.
int NiMAXTextureConverter::Convert(Texmap* pkTm, 
    NiTexturingProperty::Map*& pkMap, 
    NiTexturingProperty::MapClassID eMapClassID,
    NiFlipController*& pkFlipper, 
    NiTexture::FormatPrefs& kPrefs, 
    unsigned int& uiFlags,
    unsigned int& uiWidth, 
    unsigned int& uiHeight,
    bool bIsntReflectionMap)
{
    CHECK_MEMORY();
    uiFlags = 0;
    pkFlipper = 0;
    pkMap = 0;
    uiWidth = 0;
    uiHeight = 0;

    // We only handle standard materials.
    if (!pkTm)
        return(W3D_STAT_FAILED);

    // check for vertex color map
    if (bIsntReflectionMap && 
        (pkTm->ClassID() == Class_ID(VCOL_CLASS_ID, 0) ||
         pkTm->ClassID() == PARTAGE_CLASSID))
    {
        uiFlags |= VERTCOL_MAP;
        return(W3D_STAT_OK);
    }

    // We only handle bitmap textures
    if (pkTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
    {
        BitmapTex* pkBm = (BitmapTex*) pkTm;

        if (!_tcscmp(pkBm->GetMapName(), _T("")))
            return(W3D_STAT_FAILED);

        return ConvertBitmapTexture(pkBm, pkMap, eMapClassID, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
    }
    else if (pkTm->ClassID() == Class_ID(ACUBIC_CLASS_ID,0))
    {
        return ConvertReflectionMapTexture(pkTm, pkMap, eMapClassID,
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
    }
    else
    {
        return ConvertSampledTexture(pkTm, pkMap, eMapClassID,
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
    }
    
}
//---------------------------------------------------------------------------
// Convert a bitmap texture map from "pkTm" into an NiTexturingProperty::Map.
// 
// A non-zero return value indicates an error.
// A return value of W3D_STAT_OK indicates successful translation.
int NiMAXTextureConverter::ConvertBitmapTexture(BitmapTex* pkBm, 
    NiTexturingProperty::Map*& pkMap, 
    NiTexturingProperty::MapClassID eMapClassID,
    NiFlipController*& pkFlipper, 
    NiTexture::FormatPrefs& kPrefs, 
    unsigned int& uiFlags,
    unsigned int& uiWidth, 
    unsigned int& uiHeight)
{
    // filter mode
    NiTexturingProperty::FilterMode eFilterMode;
    if (pkBm->GetFilterType() == FILTER_PYR)
        eFilterMode = NiTexturingProperty::FILTER_TRILERP;
    else if (pkBm->GetFilterType() == FILTER_SAT)
        eFilterMode = NiTexturingProperty::FILTER_BILERP;
    else
        eFilterMode = NiTexturingProperty::FILTER_NEAREST;

    unsigned short usMaxAnisotropy = 1;

    // set up image mipmapping prefs
    // m_eAlphaFmt should have already been set by the shader.
    kPrefs.m_eMipMapped = 
        (eFilterMode == NiTexturingProperty::FILTER_TRILERP) ? 
        NiTexture::FormatPrefs::YES : NiTexture::FormatPrefs::NO;

    // figure out the clamp mode this texture uses
    int iClamp = pkBm->GetUVGen()->GetTextureTiling();
    bool bURepeat, bVRepeat;
    if ((iClamp & U_MIRROR) || (iClamp & U_WRAP))
        bURepeat = true;
    else
        bURepeat = false;
    
    if ((iClamp & V_MIRROR) || (iClamp & V_WRAP))
        bVRepeat = true;
    else
        bVRepeat = false;
    
    // clamp mode
    NiTexturingProperty::ClampMode eClampMode;
    if (bURepeat && bVRepeat)
        eClampMode = NiTexturingProperty::WRAP_S_WRAP_T;
    else if (bURepeat)
        eClampMode = NiTexturingProperty::WRAP_S_CLAMP_T;
    else if (bVRepeat)
        eClampMode = NiTexturingProperty::CLAMP_S_WRAP_T;
    else
        eClampMode = NiTexturingProperty::CLAMP_S_CLAMP_T;

    // Use the texture to determine whether or not to use alpha?
    bool bUseImageAlphaIfAvailable = (pkBm->GetAlphaSource() == ALPHA_FILE);

    // GetBitmap is where the MAX bitmap manager accumulates the list of
    // missing maps
    Bitmap* pkBitMap = pkBm->GetBitmap(m_kAnimStart);
    if (!pkBitMap)
        return(W3D_STAT_FAILED);

    // create flipbook controller
    float fStart = pkBm->GetStartTime() * SECONDSPERTICK;
    float fRate = pkBm->GetPlaybackRate();
    int iOOB = pkBm->GetEndCondition();

    BitmapInfo kBI;
    const char* pcMapName = pkBm->GetMapName();
    TheManager->GetImageInfo(&kBI, pcMapName);
    uiWidth = kBI.Width();
    uiHeight = kBI.Height();
    const TCHAR* pcFilePath = NULL;
    const TCHAR* pcFileName = NULL;
    if (BMMGetFullFilename(&kBI) == TRUE)
    {
        if (pcFilePath == NULL || pcFileName == NULL)
        {
            pcFilePath = kBI.Name();
            pcFileName = kBI.Filename();
        }
    }
    
    if ((!NfIsPowerOfTwo(uiWidth) || !NfIsPowerOfTwo(uiHeight)) &&
        eFilterMode == NiTexturingProperty::FILTER_TRILERP &&
        NiMAXOptions::GetBool(NI_ALLOW_NPOT_TEXTURES))
    {
        eFilterMode = NiTexturingProperty::FILTER_BILERP;
        kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    }

    // Non-power of two textures are limited on several cards to
    // clamped texture coordinate addressing. We should warn
    // the user that they have entered into this perilous situation.
    // We shouldn't change the value for them, as they may want to only
    // support those high-level cards.
    if (!(NfIsPowerOfTwo(uiWidth) && NfIsPowerOfTwo(uiWidth))
        && (NiMAXOptions::GetBool(NI_ALLOW_NPOT_TEXTURES)) && 
        eClampMode != NiTexturingProperty::CLAMP_S_CLAMP_T)
    {    
        char acString[1024];
        NiSprintf(acString, 1024,
            "Texture \"%s\" is not a power of two texture.\n"
            "The texture address mode has been set to a value other than "
            "CLAMP in U and V.\n"
            "This is not supported on a large number of cards.\n"
            "It is suggested that you reset the texture address mode.\n", 
            pcFileName);
        NILOGWARNING(acString);
    }

    NiTexture* pkTex = NULL;
    int iStatus;

    // Handle custom attribute settings
    NiTObjectArray<NiString>* pkFilenames = NULL;  
    bool bExternalImages = ms_bSaveTexturesSeparately; 
    bool bOriginalImages = ms_bUseOrigExt;
    bool bOriginalImagesVRAM = ms_bUseOrigExtVRAM;
    unsigned int uiSizeDivideBy = 1;
    NiPixelFormat eFormat = NiPixelFormat::RGBA32;
    

    ComputeDefaultPixelLayout(pkBm, kPrefs, 
        uiSizeDivideBy, pkFilenames, bExternalImages, 
        bOriginalImages, bOriginalImagesVRAM, 
        eFilterMode, eFormat, bUseImageAlphaIfAvailable);

    NiMAXCustAttribConverter kAttribConvert(m_kAnimStart, m_kAnimEnd);
    kAttribConvert.ConvertTexmap(
        pkBm, 
        kPrefs, 
        uiSizeDivideBy, 
        pkFilenames, 
        bExternalImages, 
        bOriginalImages, 
        bOriginalImagesVRAM, 
        eFilterMode, 
        eFormat, 
        bUseImageAlphaIfAvailable,
        usMaxAnisotropy);
    
    if (NiMAXConverter::GetViewerRun())
    {
        bExternalImages = false;
        bOriginalImages = false;
        bOriginalImagesVRAM = false;
    }

    if (kBI.NumberFrames() > 1)
    {
        iStatus = GetAnimatedTexture(pkBitMap, kBI, pkTex, kPrefs, 
            eFormat, pkFlipper, fStart, fRate, iOOB, uiSizeDivideBy, 
            pkFilenames, bExternalImages, bOriginalImages, 
            bOriginalImagesVRAM, true, bUseImageAlphaIfAvailable);
        
        if (iStatus)
        {
            if (pkFilenames)
                NiDelete pkFilenames;
            return iStatus;
        }
        NIASSERT(pkFlipper);
        if (eMapClassID == NiTexturingProperty::MAP_CLASS_BUMP)
        {
            pkMap = NiNew NiTexturingProperty::BumpMap(pkTex, 0,
                eClampMode, eFilterMode);
        }
        else if (eMapClassID == NiTexturingProperty::MAP_CLASS_PARALLAX)
        {
            pkMap = NiNew NiTexturingProperty::ParallaxMap(pkTex, 0,
                eClampMode, eFilterMode);
        }
        else if (eMapClassID == NiTexturingProperty::MAP_CLASS_SHADER)
        {
            pkMap = NiNew NiTexturingProperty::ShaderMap(pkTex, 0,
                eClampMode, eFilterMode);
        }
        else
        {
            pkMap = NiNew NiTexturingProperty::Map(pkFlipper->GetTextureAt(0),
                0, eClampMode, eFilterMode);
        }

        if (bUseImageAlphaIfAvailable)
        {
            unsigned int uiNumTextures = pkFlipper->GetTextureArraySize();
            for (unsigned int uiTexIndex = 0; uiTexIndex < uiNumTextures; 
                ++uiTexIndex)
            {
                if (TextureHasAlpha(pkFlipper->GetTextureAt(uiTexIndex)))
                {
                    uiFlags |= HAS_ALPHA;
                    break;
                }
            }
        }
    }
    else
    {
        iStatus = GetTexture(pkBitMap, kBI, pcFilePath, pcFileName, pkTex, 
            kPrefs, eFormat, uiSizeDivideBy, pkFilenames, bExternalImages, 
            bOriginalImages, bOriginalImagesVRAM, true, 
            bUseImageAlphaIfAvailable);
        
        if (iStatus)
        {
            if (pkFilenames)
                NiDelete pkFilenames;
            return iStatus;    
        }

        if (eMapClassID == NiTexturingProperty::MAP_CLASS_BUMP)
        {
            pkMap = NiNew NiTexturingProperty::BumpMap(pkTex, 0,
                eClampMode, eFilterMode);
        }
        else if (eMapClassID == NiTexturingProperty::MAP_CLASS_PARALLAX)
        {
            pkMap = NiNew NiTexturingProperty::ParallaxMap(pkTex, 0,
                eClampMode, eFilterMode);
        }
        else if (eMapClassID == NiTexturingProperty::MAP_CLASS_SHADER)
        {
            pkMap = NiNew NiTexturingProperty::ShaderMap(pkTex, 0,
                eClampMode, eFilterMode);
        }
        else
        {
            pkMap = NiNew NiTexturingProperty::Map(pkTex, 0, eClampMode,
                eFilterMode);
        }

        if (bUseImageAlphaIfAvailable && TextureHasAlpha(pkTex))
        {
            uiFlags |= HAS_ALPHA;
        }
    }

    if (pkMap && eFilterMode == NiTexturingProperty::FILTER_ANISOTROPIC)
    {
        pkMap->SetMaxAnisotropy(usMaxAnisotropy);
    }

    if (pkFilenames)
        NiDelete pkFilenames;

    if (!pkMap)
    {
        return(W3D_STAT_NO_MEMORY);
    }

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::TextureHasAlpha(const NiTexture* pkTex)
{
    // Get access to PixelData to see if any texels have alpha
    NiSourceTexture* pkSrcTex = NiDynamicCast(NiSourceTexture, pkTex);

    // If something other than a NiSourceTexture (or a descendant) is
    // returned in the future, the code below should be modified to 
    // properly handle it.
    NIASSERT(pkSrcTex); 

    if (pkSrcTex)
    {
        return NiDevImageQuantizer::ImageHasAlpha(
            pkSrcTex->GetSourcePixelData());
    }

    // Err on the side of caution and enable alpha blending if we can't 
    // determine whether the image has alpha. This was the default behavior
    // in the past.
    return true;
}
//---------------------------------------------------------------------------
int NiMAXTextureConverter::ConvertSampledTexture(
    Texmap* pkTm, 
    NiTexturingProperty::Map*& pkMap, 
    NiTexturingProperty::MapClassID eMapClassID,
    NiFlipController*&, 
    NiTexture::FormatPrefs& kPrefs,
    unsigned int&, 
    unsigned int& uiWidth,
    unsigned int& uiHeight)
{
    uiWidth = 256;
    uiHeight = 256;

    BitmapInfo kInfo;
    kInfo.SetType(BMM_TRUE_32);
    kInfo.SetFlags(MAP_HAS_ALPHA);
    kInfo.SetCustomFlag(0);
    kInfo.SetWidth((WORD)uiWidth);
    kInfo.SetHeight((WORD)uiHeight);
    Bitmap * pkBitMap = TheManager->Create(&kInfo);

    if (pkBitMap)
    {
        GetCOREInterface()->RenderTexmap(pkTm, pkBitMap);
        // filter mode
        NiTexturingProperty::FilterMode eFilterMode = 
            eFilterMode = NiTexturingProperty::FILTER_TRILERP;

        // max anisotropy
        unsigned short usMaxAnisotropy = 1;

        // set up image mipmapping prefs
        // m_eAlphaFmt should have already been set by the shader.
        kPrefs.m_eMipMapped = 
            (eFilterMode == NiTexturingProperty::FILTER_TRILERP) ? 
            NiTexture::FormatPrefs::YES : NiTexture::FormatPrefs::NO;

        UVGen * pkUVs = pkTm->GetTheUVGen();
        int iClamp = U_WRAP|V_WRAP;
        if (pkUVs)
            iClamp = pkUVs->GetTextureTiling();
        bool bURepeat = true;
        bool bVRepeat = true;

        if ((iClamp & U_MIRROR) || (iClamp & U_WRAP))
            bURepeat = true;
        else
            bURepeat = false;
    
        if ((iClamp & V_MIRROR) || (iClamp & V_WRAP))
            bVRepeat = true;
        else
            bVRepeat = false;

        // clamp mode
        NiTexturingProperty::ClampMode eClampMode;
        // figure out the clamp mode this texture uses
    
        if (bURepeat && bVRepeat)
            eClampMode = NiTexturingProperty::WRAP_S_WRAP_T;
        else if (bURepeat)
            eClampMode = NiTexturingProperty::WRAP_S_CLAMP_T;
        else if (bVRepeat)
            eClampMode = NiTexturingProperty::CLAMP_S_WRAP_T;
        else
            eClampMode = NiTexturingProperty::CLAMP_S_CLAMP_T;

        // Handle custom attribute settings
        NiTObjectArray<NiString>* pkFilenames = NULL;
        bool bExternalImages = ms_bSaveTexturesSeparately; 
        bool bOriginalImages = false;
        bool bOriginalImagesVRAM = false;
        unsigned int uiSizeDivideBy = 1;
        NiPixelFormat eFormat = NiPixelFormat::RGB24;

        ComputeDefaultPixelLayout(pkTm, kPrefs, 
            uiSizeDivideBy, pkFilenames, bExternalImages, bOriginalImages, 
            bOriginalImagesVRAM, eFilterMode, eFormat, true);

        NiMAXCustAttribConverter kAttribConvert(m_kAnimStart, m_kAnimEnd);
        kAttribConvert.ConvertTexmap(
            pkTm, 
            kPrefs, 
            uiSizeDivideBy, 
            pkFilenames, 
            bExternalImages, 
            bOriginalImages, 
            bOriginalImagesVRAM, 
            eFilterMode, 
            eFormat, 
            true,
            usMaxAnisotropy);

        char acMapPath[1024];
        char acMapName[1024];
        NiSprintf(acMapName, 1024, "%s_%lx_Sampled.nif", pkTm->GetName(),
            (unsigned int)PtrToUint(pkTm));
        NiSprintf(acMapPath, 1024, "%s_%lx_Sampled.nif", pkTm->GetName(), 
            (unsigned int)PtrToUint(pkTm));
        NiTexture* pkTex;
        int iStatus;

        iStatus = GetTexture(pkBitMap, kInfo, acMapPath, acMapName, pkTex, 
            kPrefs, eFormat, uiSizeDivideBy, pkFilenames,
            bExternalImages, false, false, true, true);

        if (iStatus)
        {
            if (pkFilenames)
                NiDelete pkFilenames;
            return iStatus;
        }

        if (eMapClassID == NiTexturingProperty::MAP_CLASS_BUMP)
        {
            pkMap = NiNew NiTexturingProperty::BumpMap(
                pkTex, 
                0,
                eClampMode, 
                eFilterMode);
        }
        else if (eMapClassID == NiTexturingProperty::MAP_CLASS_PARALLAX)
        {
            pkMap = NiNew NiTexturingProperty::ParallaxMap(
                pkTex, 
                0,
                eClampMode, 
                eFilterMode);
        }
        else if (eMapClassID == NiTexturingProperty::MAP_CLASS_SHADER)
        {
            pkMap = NiNew NiTexturingProperty::ShaderMap(
                pkTex, 
                0,
                eClampMode, 
                eFilterMode);
        }
        else
        {
            pkMap = NiNew NiTexturingProperty::Map(
                pkTex, 
                0, 
                eClampMode,
                eFilterMode);
        }

        if (pkMap && eFilterMode == NiTexturingProperty::FILTER_ANISOTROPIC)
        {
            pkMap->SetMaxAnisotropy(usMaxAnisotropy);
        }

        pkBitMap->DeleteThis();
        if (pkFilenames)
            NiDelete pkFilenames;

        if (!pkMap)
            return(W3D_STAT_NO_MEMORY);

        return (W3D_STAT_OK);
    }
    else
    {
        return(W3D_STAT_FAILED);
    }

}

// Define the indices for the max cube map
#define MAX_CUBE_UP     0
#define MAX_CUBE_DN     1
#define MAX_CUBE_LF     2
#define MAX_CUBE_RT     3
#define MAX_CUBE_FR     4
#define MAX_CUBE_BK     5

// Determine the relationship between max cube maps and 
// Gamebryo cube maps.
#define CUBE_X_POS      MAX_CUBE_RT 
#define CUBE_X_NEG      MAX_CUBE_LF 

#define CUBE_Y_POS      MAX_CUBE_BK
#define CUBE_Y_NEG      MAX_CUBE_FR

#define CUBE_Z_POS      MAX_CUBE_UP
#define CUBE_Z_NEG      MAX_CUBE_DN

//---------------------------------------------------------------------------
int NiMAXTextureConverter::ConvertReflectionMapTexture(
    Texmap* pkTm, 
    NiTexturingProperty::Map*& pkMap, 
    NiTexturingProperty::MapClassID eMapClassID,
    NiFlipController*& pkFlipper, 
    NiTexture::FormatPrefs& kPrefs, 
    unsigned int& uiFlags,
    unsigned int& uiWidth, 
    unsigned int& uiHeight)
{
    NI_UNUSED_ARG(uiFlags);
    NI_UNUSED_ARG(pkFlipper);

    ACubic* pkCubicMap = (ACubic*) pkTm;
    NiTObjectArray<NiString>* pkFilenames = NULL;  
    // Cubemaps with 6 individual textures must be stored internally
    bool bExternalImages = false; 
    bool bOriginalImages = false;
    bool bOriginalImagesVRAM = false;
    unsigned int uiSizeDivideBy = 1;
    NiPixelFormat kFormat = NiPixelFormat::RGB24;
    // filter mode
    NiTexturingProperty::FilterMode eFilterMode = 
        eFilterMode = NiTexturingProperty::FILTER_TRILERP;
    // max anisotropy
    unsigned short usMaxAnisotropy = 1;

    ComputeDefaultPixelLayout(pkTm, kPrefs, 
        uiSizeDivideBy, pkFilenames, bExternalImages, bOriginalImages, 
        bOriginalImagesVRAM, eFilterMode, kFormat, true);

    NiMAXCustAttribConverter kAttribConvert(m_kAnimStart, m_kAnimEnd);
    kAttribConvert.ConvertTexmap(
        pkTm, 
        kPrefs, 
        uiSizeDivideBy, 
        pkFilenames, 
        bExternalImages, 
        bOriginalImages, 
        bOriginalImagesVRAM, 
        eFilterMode, 
        kFormat, 
        true,
        usMaxAnisotropy);

    // Issue a warning if they are choosing to save external textures
    if (ms_bSaveTexturesSeparately || ms_bUseOrigExt || ms_bUseOrigExtVRAM)
    {
        NILOGWARNING("The scene contains a Cubemap and also is set to use "
            "external textures. The cubemap will be stored internally.");
    }

    // Cube maps should never be palettized unless all the cube map faces are
    // palettized at the same time, since they all need to share the same 
    // palette. Therefore, if the requested format is palettized, disable it.
    if (kFormat.GetPalettized())
    {
        // Replacement will be in the original texture format.
        // If that's palettized, then the cube map will fail to load
        // on platforms that support palettized textures and cube maps
        kFormat = NiPixelFormat::RGBA32;
        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::PIX_DEFAULT;

        NILOGWARNING("Attempting to create a palettized cube map!\n"
            "This is not valid; cube map will not have its "
            "format converted.");
    }

    IParamBlock2* plock = pkCubicMap->GetParamBlock(0);
    if (plock)
    {

        TCHAR * pcCubeMaps[6];
        NiPixelDataPtr aspCubeMapPixelData[6];
        NiString astrCubeMapFilenames[6];
        
        int iSource = plock->GetInt(acubic_source, 0);
        int iUseAtmospheric = plock->GetInt(acubic_useatmospheric, 0);

        TCHAR* up = plock->GetStr(acubic_bitmap_names, 0, 0); 
        TCHAR* down = plock->GetStr(acubic_bitmap_names, 0, 1); 
        TCHAR* left = plock->GetStr(acubic_bitmap_names, 0, 2); 
        TCHAR* right = plock->GetStr(acubic_bitmap_names, 0, 3); 
        TCHAR* front = plock->GetStr(acubic_bitmap_names, 0, 4); 
        TCHAR* back = plock->GetStr(acubic_bitmap_names, 0, 5); 
    
        pcCubeMaps[0] = up;
        pcCubeMaps[1] = down;
        pcCubeMaps[2] = left;
        pcCubeMaps[3] = right;
        pcCubeMaps[4] = front;
        pcCubeMaps[5] = back;

        if (iSource != 1)
        {
            NILOGWARNING("Attempting to sample a reflection map.\n"
                "Max will not properly sample this type. Try using the \""
                "From File\" option instead\n");

            if (pkFilenames)
                NiDelete pkFilenames;

            return W3D_STAT_OK;
        }


        NiPixelFormat kUnifiedFormat;
        unsigned int uiUnifiedWidth = 0;
        unsigned int uiUnifiedHeight = 0;
        NiImageConverter* pkImageConverter = 
            NiImageConverter::GetImageConverter();

        for (unsigned int ui = 0; ui < 6; ui++)
        {
            uiWidth = 256;
            uiHeight = 256;

            BitmapInfo kInfo;
            kInfo.SetType(BMM_TRUE_32);
            kInfo.SetFlags(MAP_HAS_ALPHA);
            kInfo.SetCustomFlag(0);
            kInfo.SetWidth((WORD)uiWidth);
            kInfo.SetHeight((WORD)uiHeight);
            BMMRES kRes = TheManager->GetImageInfo(&kInfo, pcCubeMaps[ui]);
            
            if (kRes != BMMRES_SUCCESS)
            {
                if (pkFilenames)
                    NiDelete pkFilenames;

                return(W3D_STAT_FAILED);
            }

            const char* pcMapName = pcCubeMaps[ui];
            NiTexture* pkTex;
            Bitmap* pkBitMap = NULL;
            
            const char* pcFilePath = NULL;
            if (BMMGetFullFilename(&kInfo) == TRUE)
            {
                pcFilePath = kInfo.Name();
            }

            int iStatus = GetTexture(pkBitMap, kInfo, pcFilePath, 
                pcMapName, pkTex, kPrefs, kFormat, uiSizeDivideBy, 
                pkFilenames, bExternalImages, bOriginalImagesVRAM,  
                bOriginalImages, true, false);

            //Let everything clean up once we're done
            NiTexturePtr spTex = pkTex; 

            if (iStatus != W3D_STAT_OK)
            {
                if (pkFilenames)
                    NiDelete pkFilenames;

                return(iStatus);
            }

            if (pkTex != NULL && NiIsKindOf(NiSourceTexture, pkTex))
            {
                aspCubeMapPixelData[ui] = 
                    ((NiSourceTexture*)pkTex)->GetSourcePixelData();
                NIASSERT(aspCubeMapPixelData[ui] != NULL);
                astrCubeMapFilenames[ui] = 
                    ((NiSourceTexture*)pkTex)->GetFilename();
                NIASSERT(astrCubeMapFilenames[ui] != NULL);
            }
            else
            {
                if (pkFilenames)
                    NiDelete pkFilenames;

                return (W3D_STAT_FAILED);
            }

            // Make sure that the images all match width/height/pixel
            //  format. Otherwise fail the export.
            if (ui == 0)
            {
                bool bMipmap;
                unsigned int uiNumFaces;
                if (!pkImageConverter->ReadImageFileInfo(pcFilePath, 
                    kUnifiedFormat, bMipmap, uiUnifiedWidth, 
                    uiUnifiedHeight, uiNumFaces))
                {
                    if (aspCubeMapPixelData[ui] == NULL)
                    {
                        NiString strMessage = "Cannot export "
                            "the Reflection Map texture \"";
                        strMessage += pcFilePath;
                        strMessage += "\"";
                        NILOGWARNING((const char*)strMessage);
                
                        if (pkFilenames)
                            NiDelete pkFilenames;

                        return (W3D_STAT_FAILED);
                    }
                    else
                    {
                        kUnifiedFormat = 
                            aspCubeMapPixelData[ui]->GetPixelFormat();
                        uiUnifiedWidth =
                            aspCubeMapPixelData[ui]->GetWidth();
                        uiUnifiedHeight = 
                            aspCubeMapPixelData[ui]->GetHeight();
                    }
                }
            }
            else
            {
                NiPixelFormat kThisFormat;
                unsigned int uiThisWidth, uiThisHeight;
                bool bMipmap;
                unsigned int uiNumFaces;
                if (!pkImageConverter->ReadImageFileInfo(pcFilePath, 
                    kThisFormat, bMipmap, uiThisWidth, 
                    uiThisHeight, uiNumFaces))
                {
                    if (aspCubeMapPixelData[ui] == NULL)
                    {
                        NiString strMessage = "Cannot export the"
                            " Reflection Map texture \"";
                        strMessage += pcFilePath;
                        strMessage += "\"";
                        NILOGWARNING((const char*)strMessage);
                
                        if (pkFilenames)
                            NiDelete pkFilenames;

                        return (W3D_STAT_FAILED);
                    }
                    else
                    {
                        kThisFormat = 
                            aspCubeMapPixelData[ui]->GetPixelFormat();
                        uiThisWidth =
                            aspCubeMapPixelData[ui]->GetWidth();
                        uiThisHeight =
                            aspCubeMapPixelData[ui]->GetHeight();
                    }
                }

                NiString strMessage = "Cannot export the"
                    " Reflection Map texture \"";
                strMessage += pcFilePath;
                strMessage += "\" because\n";

                if (kThisFormat != kUnifiedFormat)
                {
                    strMessage += "the pixel format doesn't match"
                        " the other maps in the cube map.";
                    NILOGWARNING((const char*)strMessage);
                    if (pkFilenames)
                        NiDelete pkFilenames;

                    return (W3D_STAT_FAILED);
                }

                if (uiThisWidth != uiUnifiedWidth)
                {
                    strMessage += "the width doesn't match "
                        "the other maps in the cube map.";
                    NILOGWARNING((const char*)strMessage);
                    if (pkFilenames)
                        NiDelete pkFilenames;

                    return (W3D_STAT_FAILED);
                }

                if (uiThisHeight != uiUnifiedHeight)
                {
                    strMessage += "the height doesn't match"
                        " the other maps in the cube map.";
                    NILOGWARNING((const char*)strMessage);
                    if (pkFilenames)
                        NiDelete pkFilenames;

                    return (W3D_STAT_FAILED);
                }
            }
        }

        NiSourceCubeMap* pkSrcCubeMap = NULL;

        // remove the path from the filenames and concatenate them to a single
        // string that we can key into the hash table from.
        NiString kFileName;
        NiString kKeyString;
        int iChopPos = astrCubeMapFilenames[CUBE_X_POS].FindReverse('\\');
        kFileName = astrCubeMapFilenames[CUBE_X_POS].GetSubstring(iChopPos + 1,
            astrCubeMapFilenames[CUBE_X_POS].Length());
        kKeyString.Concatenate(kFileName);
        iChopPos = astrCubeMapFilenames[CUBE_X_NEG].FindReverse('\\');
        kFileName = astrCubeMapFilenames[CUBE_X_NEG].GetSubstring(iChopPos + 1,
            astrCubeMapFilenames[CUBE_X_NEG].Length());
        kKeyString.Concatenate(kFileName);
        iChopPos = astrCubeMapFilenames[CUBE_Y_POS].FindReverse('\\');
        kFileName = astrCubeMapFilenames[CUBE_Y_POS].GetSubstring(iChopPos + 1,
            astrCubeMapFilenames[CUBE_Y_POS].Length());
        kKeyString.Concatenate(kFileName);
        iChopPos = astrCubeMapFilenames[CUBE_Y_NEG].FindReverse('\\');
        kFileName = astrCubeMapFilenames[CUBE_Y_NEG].GetSubstring(iChopPos + 1,
            astrCubeMapFilenames[CUBE_Y_NEG].Length());
        kKeyString.Concatenate(kFileName);
        iChopPos = astrCubeMapFilenames[CUBE_Z_POS].FindReverse('\\');
        kFileName = astrCubeMapFilenames[CUBE_Z_POS].GetSubstring(iChopPos + 1,
            astrCubeMapFilenames[CUBE_Z_POS].Length());
        kKeyString.Concatenate(kFileName);
        iChopPos = astrCubeMapFilenames[CUBE_Z_NEG].FindReverse('\\');
        kFileName = astrCubeMapFilenames[CUBE_Z_NEG].GetSubstring(iChopPos + 1,
            astrCubeMapFilenames[CUBE_Z_NEG].Length());
        kKeyString.Concatenate(kFileName);

        // In order to convert into DX space,
        // we must change the order in which we pass in the images
        // Instead of XXYYZZ, the format will be XXZZYY
        // We will later have to pair this with a projective 
        // texture transform.
        void *pvUnused = 0;
        pkSrcCubeMap = ms_pkCubeHTab->NameRetrieve(
            (const char*)kKeyString, pvUnused);
        if (!pkSrcCubeMap)
        {
            pkSrcCubeMap = NiSourceCubeMap::Create(
                aspCubeMapPixelData[CUBE_X_POS], 
                aspCubeMapPixelData[CUBE_X_NEG], 
                aspCubeMapPixelData[CUBE_Z_POS],
                aspCubeMapPixelData[CUBE_Z_NEG],
                aspCubeMapPixelData[CUBE_Y_POS], 
                aspCubeMapPixelData[CUBE_Y_NEG],
                NULL);
            ms_pkCubeHTab->NameInsert((const char*)kKeyString, pvUnused,
                pkSrcCubeMap);
        }

        if (pkSrcCubeMap != NULL)
        {
            NIASSERT((eMapClassID == NiTexturingProperty::MAP_CLASS_BASE) ||
                (eMapClassID == NiTexturingProperty::MAP_CLASS_SHADER));
            if (eMapClassID == NiTexturingProperty::MAP_CLASS_BASE)
            {
                pkMap = NiNew NiTexturingProperty::Map;
            }
            else if (eMapClassID == NiTexturingProperty::MAP_CLASS_SHADER)
            {
                pkMap = NiNew NiTexturingProperty::ShaderMap;
            }

            pkMap->SetTexture(pkSrcCubeMap);
            NiTextureEffect::CoordGenType eCoordGenMethod;

            // We have to know how to set up the texture effect later...
            if (iUseAtmospheric == FALSE)
                eCoordGenMethod = NiTextureEffect::DIFFUSE_CUBE_MAP;
            else
                eCoordGenMethod = NiTextureEffect::SPECULAR_CUBE_MAP;

            NiIntegerExtraData* pkCoordGenED = 
                NiNew NiIntegerExtraData((int) eCoordGenMethod); 
            pkCoordGenED->SetName(NI_TEXTURE_COORD_GEN_KEY);
            pkSrcCubeMap->AddExtraData("TextureCoordGen", pkCoordGenED);
            pkSrcCubeMap->SetCurrentCubeFace(NiSourceCubeMap::FACE_POS_X);

            if (pkFilenames)
                NiDelete pkFilenames;

            return(W3D_STAT_OK);
        }
        else
        {
            if (pkFilenames)
                NiDelete pkFilenames;

            return(W3D_STAT_FAILED);
        }
    }
    else
    {
        if (pkFilenames)
            NiDelete pkFilenames;

        return(W3D_STAT_FAILED);
    }
}
//---------------------------------------------------------------------------
void NiMAXTextureConverter::Init()
{
    CHECK_MEMORY();
    NIASSERT(!ms_pkTexHTab);
    ms_pkTexHTab = NiNew NiTHash<NiTexturePtr>(199, HashEq, 
        HashFree);

    NIASSERT(!ms_pkCubeHTab);
    ms_pkCubeHTab = NiNew NiTHash<NiSourceCubeMapPtr>(19, HashEq,
        CubeHashFree);

    NIASSERT(ms_pkTexHTab->GetNumHashed() == 0);
    NIASSERT(ms_pkCubeHTab->GetNumHashed() == 0);

    NiDevImageQuantizer* pkCompress = NiDevImageQuantizer::Create();
    NIASSERT(pkCompress);

    if (pkCompress)
    {
        NiOutputDebugString("Runtime-compressor created\n");
        NiImageConverter::SetImageConverter(pkCompress);
        ms_bDevImageQuantizerFound = true;
    }
    else
    {
        NILOGERROR("Cannot create runtime compressor\n");
        NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
        ms_bDevImageQuantizerFound = false;
    }

    ms_pkDDSReader = NiNew NiDDSReader;

    // Begin accumulating a list of files that didn't load,
    // and don't put up load errors in the meantime.
    TheManager->BeginSavingLoadErrorFiles();  
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXTextureConverter::Shutdown()
{

    CHECK_MEMORY();
    // Get list of names of files not found
    NameTab& kFileList = TheManager->GetLoadErrorFileList();
    if (kFileList.Count() > 0)
    {
        unsigned int uiLen = kFileList.Count() * MAX_PATH;
        char* pcFileList = NiAlloc(char, uiLen);
        pcFileList[0] = 0;
        for (int i = 0; i< kFileList.Count(); i++)
        {
            NiStrcat(pcFileList, uiLen, (char *)kFileList[i]);
            NiStrcat(pcFileList, uiLen, "\n");
        }
        MessageBox(GetActiveWindow(), pcFileList, "Missing Map Files", 
            MB_OK | MB_ICONEXCLAMATION);
        NiFree(pcFileList);
    }

    TheManager->EndSavingLoadErrorFiles();  
    CHECK_MEMORY();

    NiDelete ms_pkTexHTab;
    ms_pkTexHTab = NULL;
    NiDelete ms_pkCubeHTab;
    ms_pkCubeHTab = NULL;

    NiDelete ms_pkDDSReader;

    ms_bSaveTexturesSeparately = true;
}
//---------------------------------------------------------------------------
void NiMAXTextureConverter::SetPerTextureOverride(bool override)
{
    ms_bPerTextureOverride = override;
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::GetPerTextureOverride()
{
    return ms_bPerTextureOverride;
}
//---------------------------------------------------------------------------
void NiMAXTextureConverter::SetLocalPixelLayout(
    NiTexture::FormatPrefs::PixelLayout ePixelLayout)
{
    ms_eLocalPixelLayout = ePixelLayout;
}
//---------------------------------------------------------------------------
NiTexture::FormatPrefs::PixelLayout 
NiMAXTextureConverter::GetLocalPixelLayout()
{
    return ms_eLocalPixelLayout;
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::FormatSupportsAlpha(NiPixelFormat eFormat)
{
    switch(eFormat.GetFormat())
    {
        case NiPixelFormat::FORMAT_RGBA:
        case NiPixelFormat::FORMAT_PALALPHA:
        case NiPixelFormat::FORMAT_DXT1:
        case NiPixelFormat::FORMAT_DXT3:
        case NiPixelFormat::FORMAT_DXT5:
            return true;
        default:
        case NiPixelFormat::FORMAT_RGB:
        case NiPixelFormat::FORMAT_PAL:
        case NiPixelFormat::FORMAT_RGB24NONINTERLEAVED:
        case NiPixelFormat::FORMAT_BUMP:
        case NiPixelFormat::FORMAT_BUMPLUMA:
            return false;
    }
}
//---------------------------------------------------------------------------
bool NiMAXTextureConverter::ComputeDefaultPixelLayout(
    Texmap*, 
    NiTexture::FormatPrefs& kPrefs,
    unsigned int&, 
    NiTObjectArray<NiString>*&,
    bool&,
    bool&,
    bool&,
    NiTexturingProperty::FilterMode&, 
    NiPixelFormat& ePixelFormat, 
    bool bImageHasAlpha)
{
    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
        return false;

    NiTexture::FormatPrefs::PixelLayout eLocalPixLayout = 
        GetLocalPixelLayout();
    NiTexture::FormatPrefs::PixelLayout eGlobalPixLayout = GetPixelLayout();

    NiTexture::FormatPrefs::PixelLayout eSelectedLayout;
    if (GetPerTextureOverride())
        eSelectedLayout = eGlobalPixLayout;
    else
        eSelectedLayout = eLocalPixLayout;

    kPrefs.m_ePixelLayout = eSelectedLayout;
    if (eSelectedLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
    {
        return false;
    }
    else if (eSelectedLayout == NiTexture::FormatPrefs::PALETTIZED_8)
    {
        if (bImageHasAlpha)
            ePixelFormat = NiPixelFormat::PALA8;
        else
            ePixelFormat = NiPixelFormat::PAL8;
        return true;
    }
    else if (eSelectedLayout == NiTexture::FormatPrefs::HIGH_COLOR_16)
    {
        if (bImageHasAlpha)
            ePixelFormat = NiPixelFormat::RGBA16;
        else
            ePixelFormat = NiPixelFormat::RGBA16;
        return true;
    }
    else if (eSelectedLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
    {
        if (bImageHasAlpha)
            ePixelFormat = NiPixelFormat::RGB24;
        else
            ePixelFormat = NiPixelFormat::RGBA32;
        return  true;
    }
    else if (eSelectedLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (bImageHasAlpha)
            ePixelFormat = NiPixelFormat::DXT1;
        else
            ePixelFormat = NiPixelFormat::DXT1;
        return  true;
    }
    else if (eSelectedLayout == NiTexture::FormatPrefs::PALETTIZED_4)
    {
        if (bImageHasAlpha)
            ePixelFormat = NiPixelFormat::PALA4;
        else
            ePixelFormat = NiPixelFormat::PAL4;
        return  true;
    }
    
    return false;
}
