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

// MyiTexture.h

#ifndef MYITEXTURE_H
#define MYITEXTURE_H

#include <NiSmartPointer.h>
#include <NiPixelData.h>
#include <NiSourceTexture.h>
#include <NiTexture.h>

#include "MyiObject.h"


class MyiTexture : public MyiObject
{
public:
    //,const char *pDirectory = NULL, const char *pDrive = NULL);
    MyiTexture(int iTextureID); 
    MyiTexture(char* szFileName); 
    MyiTexture(MObject kTexture); 
    
    ~MyiTexture();
    inline NiSourceTexturePtr GetTexture();
    
    inline bool GetHasAlpha();
    inline char* GetOriginalTextureFileName();
    inline MObject GetTextureNode();
    
    static  void ReplaceChar(char *szPath, char cFindChar, char cReplaceChar);
    static  void DuplicateChar(char* szPath, unsigned int uiBuffLen, 
        char cDupChar);
    static  void GetFileNameFromPath(char* szTextureFilePath, 
        char* szTextureFileName, unsigned int uiFileNameLen, 
        char* szOriginalTextureFileName, unsigned int uiOrigFileNameLen, 
        char cPathSeperator='\\');
    static  void GetDirectoryPathFromFilePath(char* szTextureFilePath, 
        char* szDirectoryPath, unsigned int uiBufferLen,
        char cPathSeperator='\\');
    static  void CreateSequentialTexturePath(char* szOriginalPath, 
        char* szNewPath, unsigned int uiBufferLen, int iSequenceNumber, 
        char cPathSeperator='\\');
    
    static  bool CreateTextureFromFile(char *szFileName, int &iWidth,
        int &iHeight, NiSourceTexturePtr &spTexture);
    
    static void CheckPowerOfTwoTexture( char* szTextureName, 
        int iWidth, int iHeight);

    static bool IsCubeMap(const char* pcFilePath);


    
    
protected:

  enum
    {
        PLATFORM_SIZE_REDUCTION_PLATFORM_DEFAULT = 0,
            PLATFORM_SIZE_REDUCTION_TEXTURE_DEFAULT,
            PLATFORM_SIZE_REDUCTION_1_2,
            PLATFORM_SIZE_REDUCTION_1_4,
            PLATFORM_SIZE_REDUCTION_1_8,
            PLATFORM_SIZE_REDUCTION_1_16,
            PLATFORM_SIZE_REDUCTION_1_32,
            PLATFORM_SIZE_REDUCTION_1_64
    };
    
    
    
    
    enum
    {
        PLATFORM_TEXTURE_LOCATION_DEFAULT = 0,
            PLATFORM_TEXTURE_LOCATION_INTERNAL,
            PLATFORM_TEXTURE_LOCATION_EXTERNAL_NIF,
            PLATFORM_TEXTURE_LOCATION_EXTERNAL_ORIGINAL
    };
    
    enum MayaSelfLoadingTextureFormat 
    {
        NOT_SELF_LOADING = 0, 
            TARGA, 
            BMP, 
            RGBA, 
            DDS,
            EXR,
            HDR
    };
    static  MyiTexture::MayaSelfLoadingTextureFormat IsSelfLoadTexture(
        char *szFileName);
    
    void Initialize();
    bool GetAttributes( int iTextureID );
    bool GetAttributes(MObject kTextureNode);
    void GetPlatformAttributes(MObject kTextureNode);
    
    NiPixelData *CreatePixelData();
    
    bool CreateTextureFile(int iTextureID, const char *pDrive, 
        const char *pDirectory, const char *pName,  const char *pExt);
    bool CreateNiTexture();
    
    bool LoadTextureFromFile(char *szFileName);
    
    NiTexture::FormatPrefs CreatePlatformFormatPrefs();
    
    NiSourceTexturePtr CreateExternalNIFSaveFile(NiPixelData* 
        pPixelData, NiTexture::FormatPrefs& kPrefs);
    
    NiSourceTexturePtr CreateExternalTextureOriginal(
        NiTexture::FormatPrefs& kPrefs);
    
    int GetTextureLocation(MayaSelfLoadingTextureFormat eTextureFormat);
    
    void CreatePlatformSpecificSubDirectoryAndSetConverter();
    
    void CreateDirectoryStructure(char *pDirectory);
    
    bool CreateCubeMap();
    bool GetCubeMapFileName(MObject kTextureNode, const char* pcAttribute,
        char* pcFilePath, unsigned int uiBuffSize);
    bool VerifyCubeMapFormats(NiPixelData* pkPosXRaw, NiPixelData* pkNegXRaw, 
        NiPixelData* pkPosYRaw, NiPixelData* pkNegYRaw, 
        NiPixelData* pkPosZRaw, NiPixelData* pkNegZRaw, const char* pacPosX, 
        const char* pacNegX, const char* pacPosY, const char* pacNegY, 
        const char* pacPosZ, const char* pacNegZ);
    
    bool CopyToDestinationDirectory(char* pcFilePath);
    
    void ConvertPixelData(NiPixelDataPtr& spPixelData);

    void SavePixelDataAsNIF(NiPixelData* pPixelData, char* pcFilePath);

    void GetFormatAttribute(MFnDependencyNode& dgNode, 
        const char* pcTextureName, const char* pcFormatName, 
        int& iTextureFormat);

    NiSourceTexturePtr m_spTexture;
    char *m_pName;
    char *m_pFileName;
    char *m_pDir;
    char *m_pDrive;
    char *m_pExt;
    char m_szPlatformDirectory[_MAX_PATH];
    char m_szTextureFileName[_MAX_PATH];
    char m_szOriginalTextureFileName[_MAX_PATH];
    char m_szFullTexturePath[_MAX_PATH];
    
    unsigned char *m_pBuffer;
    int m_iWidth;
    int m_iHeight;
    int m_iComponents;
    bool m_bHasAlpha;
    bool m_bCubeMap;
    bool m_bBumpMap;
    bool m_bMipMap;
    MObject m_kTextureNode;
    NiTexture::FormatPrefs m_kFormatPrefs;
    
    int m_iPlatformFormat;
    int m_iPlatformSizeReduction;
    int m_iPlatformTextureLocation;
    bool m_bPlaformLoadDirectlyToRenderer;
};

//---------------------------------------------------------------------------
inline NiSourceTexturePtr MyiTexture::GetTexture()
{
    return m_spTexture;
}
//---------------------------------------------------------------------------
inline bool MyiTexture::GetHasAlpha()
{ 
    return m_bHasAlpha; 
}
//---------------------------------------------------------------------------
inline char* MyiTexture::GetOriginalTextureFileName()
{ 
    return m_szOriginalTextureFileName; 
}
//---------------------------------------------------------------------------
inline MObject MyiTexture::GetTextureNode()
{
    return m_kTextureNode;
}
//---------------------------------------------------------------------------

#endif
