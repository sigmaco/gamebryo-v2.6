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

// MyiTextureManager.h

#ifndef MYITEXTUREMANAGER_H
#define MYITEXTUREMANAGER_H

#include <NiSourceTexture.h>
#include "MyiObject.h"
#include <NiTList.h>

NiSmartPointer(NiSourceTexture);



class SourceTexture : public NiMemObject
{
public:
    SourceTexture();
    ~SourceTexture();

    int m_iTextureID;
    char* m_szFileName;
    MObject m_kTextureNode;
    NiSourceTexturePtr m_spNiSourceTexture;
    bool m_bNeedsAlpha;
};


class MyiTextureManager : public MyiObject
{
public:
    MyiTextureManager(const char *pDirectory = NULL, 
        const char *pDrive = NULL);

    ~MyiTextureManager();
    NiSourceTexturePtr GetTexture(int iTextureID);

    bool NeedsAlphaProperty(int iTextureID);

    NiSourceTexturePtr GetTexture(char* szFileName);
    bool NeedsAlphaProperty(char* szFileName);

    NiSourceTexturePtr GetTexture(MObject kTextureNode);

    static MyiTextureManager* GetTextureManager();
    static void Create();
    static void Destroy();

protected:

    void LoadAllMaterialTextures();
    void LoadAllParticleTextures();
    void LoadAllTextureEffectTextures();
    void LoadAllFlipBookTextures();

    void AddFlipBookTextures(int iMaterialID, char* szType, 
        short sStartFrame, short sEndFrame);

    SourceTexture* AddTextureToList(int iTextureID, 
        NiSourceTexturePtr spTexture, bool bNeedsAlpha, char* szFileName, 
        MObject kTextureNode);

    SourceTexture* FindTexture(char* szFileName);
    SourceTexture* FindTexture(SourceTexture* pkTexture);
    SourceTexture* FindTexture(int iTextureID);
    SourceTexture* FindTexture(MObject kTextureNode);


    bool IsTextureEffect(int iLightID);

    NiTList<SourceTexture*> m_kTextureList;

    static MyiTextureManager* ms_pkTextureManager;
};

#endif