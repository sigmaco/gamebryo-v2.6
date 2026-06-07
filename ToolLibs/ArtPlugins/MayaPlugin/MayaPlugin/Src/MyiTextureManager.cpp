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

MyiTextureManager* MyiTextureManager::ms_pkTextureManager = NULL;

//---------------------------------------------------------------------------
MyiTextureManager* MyiTextureManager::GetTextureManager()
{
    NIASSERT(ms_pkTextureManager != NULL);
    return ms_pkTextureManager;
}
//---------------------------------------------------------------------------
void MyiTextureManager::Create()
{
    ms_pkTextureManager = NiNew MyiTextureManager();
}
//---------------------------------------------------------------------------
void MyiTextureManager::Destroy()
{
    NiDelete ms_pkTextureManager;
    ms_pkTextureManager = NULL;
}
//---------------------------------------------------------------------------
// The MyiTextureManager class cycles through all the textures and
// stores them in an array.
//---------------------------------------------------------------------------
MyiTextureManager::MyiTextureManager( const char *pDirectory, 
    const char *pDrive)
{
    NI_UNUSED_ARG(pDrive);
    NI_UNUSED_ARG(pDirectory);
    LoadAllMaterialTextures();
    LoadAllParticleTextures();
    LoadAllTextureEffectTextures();
    LoadAllFlipBookTextures();
    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiTextureManager::~MyiTextureManager()
{

    // Delete all of the textures
    for(NiTListIterator kIter = m_kTextureList.GetHeadPos(); kIter != NULL; 
        kIter = m_kTextureList.GetNextPos(kIter) )
    {
        SourceTexture* pkSrcTexture = m_kTextureList.Get(kIter);
        NiDelete pkSrcTexture;
    }
}
//---------------------------------------------------------------------------
NiSourceTexturePtr MyiTextureManager::GetTexture(int iTextureID)
{
    SourceTexture* pTexture = FindTexture(iTextureID);

    if(pTexture != NULL)
        return pTexture->m_spNiSourceTexture;

    return NULL;
}
//---------------------------------------------------------------------------
bool MyiTextureManager::NeedsAlphaProperty(int iTextureID)
{
    SourceTexture* pTexture = FindTexture(iTextureID);

    if(pTexture != NULL)
        return pTexture->m_bNeedsAlpha;

    return false;
}
//---------------------------------------------------------------------------
void MyiTextureManager::LoadAllMaterialTextures()
{
    // Grab the number of textures for the entire scene.
    int iTextures;
    DtTextureGetSceneCount(&iTextures);

    if(DtExt_HaveGui())
    {
        // Update the progress window
        MProgressWindow::setProgressStatus(
            "Creating Gamebryo Material Textures");
        MProgressWindow::setProgressRange(0, iTextures);
        MProgressWindow::setProgress(0);
    }


    for (int i = 0; i < iTextures; i++)
    {
        if(DtExt_HaveGui())
        {
            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        // For every texture in the scene, register it!
        MyiTexture kMyiTexture( i );
        
        if (kMyiTexture.Error())
        {
            char* szFileName;
            DtTextureGetFileNameID( i, &szFileName );

            printf("ERROR - Failed To Load Texture: %s\n", szFileName);
            AddTextureToList(i, NULL, false, "FAILED TO LOAD", 
                MObject::kNullObj);
        }
        else
        {
            AddTextureToList(i, kMyiTexture.GetTexture(), 
                kMyiTexture.GetHasAlpha(),
                kMyiTexture.GetOriginalTextureFileName(), 
                kMyiTexture.GetTextureNode());
        }
    }
}
//---------------------------------------------------------------------------
void MyiTextureManager::LoadAllParticleTextures()
{
    if(DtExt_HaveGui())
    {
        // Update the progress window
        MProgressWindow::setProgressStatus(
            "Creating Gamebryo Material Textures");
        MProgressWindow::setProgressRange(0, 
            gParticleSystemManager.GetNumEmitters());
        MProgressWindow::setProgress(0);
    }

    // Get the texture for each emitter and add it if it has not already 
    // been loaded
    for(int iLoop = 0; iLoop < gParticleSystemManager.GetNumEmitters(); 
        iLoop++)
    {
        if(DtExt_HaveGui())
        {
            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        kMDtEmitter* pCurrentEmitter = 
            gParticleSystemManager.GetEmitter(iLoop);

            // Does this emittter have a texture for it's particles?
        if(pCurrentEmitter->m_szParticleTextureFileName != NULL)
        {

            SourceTexture* pTexture = 
                FindTexture( pCurrentEmitter->m_szParticleTextureFileName );

                // No texture match was found
                // create a new texture
            if(pTexture == NULL)
            {
                MyiTexture kNewParticleTexture(
                    pCurrentEmitter->m_szParticleTextureFileName); 

                pTexture = AddTextureToList( m_kTextureList.GetSize(),
                    kNewParticleTexture.GetTexture(), 
                    kNewParticleTexture.GetHasAlpha(),
                    pCurrentEmitter->m_szParticleTextureFileName, 
                    MObject::kNullObj );
            }

            // Set the Texture ID
            pCurrentEmitter->m_iTextureID = pTexture->m_iTextureID;
        }
    }
}
//---------------------------------------------------------------------------
void MyiTextureManager::LoadAllTextureEffectTextures()
{
    // Examine each of the lights looking for a Texture Effect
    int iLightCount;
    DtLightGetCount(&iLightCount);

    if(DtExt_HaveGui())
    {
        // Update the progress window
        MProgressWindow::setProgressStatus(
            "Creating Gamebryo Texture Effect Textures");
        MProgressWindow::setProgressRange(0, iLightCount);
        MProgressWindow::setProgress(0);
    }


    for( int iLightID = 0; iLightID < iLightCount; iLightID++)
    {
        if(DtExt_HaveGui())
        {
            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        if(IsTextureEffect(iLightID))
        {
            MObject Shape;
            DtExt_LightGetShapeNode(iLightID, Shape);
   
            MObject kTextureNode = FollowInputAttribute(Shape, 
                MString("color"));

            MyiTexture kNewTexture(kTextureNode); 

            if (kNewTexture.Succeeded())
            {
                AddTextureToList( m_kTextureList.GetSize(), 
                    kNewTexture.GetTexture(), kNewTexture.GetHasAlpha(), 
                    kNewTexture.GetOriginalTextureFileName(),
                    kNewTexture.GetTextureNode());
            }

        }
    }

}
//---------------------------------------------------------------------------
void MyiTextureManager::LoadAllFlipBookTextures()
{
    // Check for All of the textures looking for Flip Book Textures
    int iNumMaterials;
    DtMtlGetSceneCount(&iNumMaterials);

    if(DtExt_HaveGui())
    {
        // Update the progress window
        MProgressWindow::setProgressStatus(
            "Creating Gamebryo Flipbook Textures");
        MProgressWindow::setProgressRange(0, iNumMaterials);
        MProgressWindow::setProgress(0);
    }

    bool bFlipBook;
    short sStartFrame;
    short sEndFrame;
    float fFramesPerSecond;
    float fStartTime;
    char cCycleType;

    for (int iMaterialID = 0; iMaterialID < iNumMaterials; iMaterialID++)
    {
        if(DtExt_HaveGui())
        {
            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        // BASE MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, "color", 
            bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, fStartTime, 
            cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "color", sStartFrame, sEndFrame);
        }
        
        // DARK MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, "DarkMap",
            bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, fStartTime, 
            cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "DarkMap", sStartFrame, 
                sEndFrame);
        }
        
        // DETAIL MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, 
            "DetailMap", bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, 
            fStartTime, cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "DetailMap", sStartFrame, 
                sEndFrame);
        }
        
        // DECAL 1 MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, 
            "Decal1Map", bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, 
            fStartTime, cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "Decal1Map", sStartFrame, 
                sEndFrame);
        }
        
        // DECAL 2 MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, 
            "Decal2Map", bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, 
            fStartTime, cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "Decal2Map", sStartFrame, 
                sEndFrame);
        }
        
        // GLOW MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, "GlowMap", 
            bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, fStartTime, 
            cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "GlowMap", sStartFrame, 
                sEndFrame);
        }
        
        // GLOSS MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, 
            "GlossMap", bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, 
            fStartTime, cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "GlossMap", sStartFrame, 
                sEndFrame);
        }
        
        // BUMP MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, "BumpMap", 
            bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, fStartTime, 
            cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "BumpMap", sStartFrame, 
                sEndFrame);
        }

        // Normal MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, 
            "NormalMap", bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, 
            fStartTime, cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "NormalMap", sStartFrame, 
                sEndFrame);
        }

        // Parallax MAP
        if( DtTextureGetMaterialMapFlipBookProperties(iMaterialID, 
            "ParallaxMap", bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond,
            fStartTime, cCycleType) )
        {
            AddFlipBookTextures(iMaterialID, "ParallaxMap", sStartFrame, 
                sEndFrame);
        }
    }
}
//---------------------------------------------------------------------------
void MyiTextureManager::AddFlipBookTextures(int iMaterialID, char* szType, 
        short sStartFrame, short sEndFrame)
{

    int iTextureID;
    DtTextureGetIDMulti(iMaterialID, szType, &iTextureID);

    char *szFullPath = NULL;
    DtTextureGetFileNameID(iTextureID, &szFullPath);


    for(int iLoop = sStartFrame; iLoop <= sEndFrame; iLoop++)
    {
        char szFramePath[MAX_PATH];

        MyiTexture::CreateSequentialTexturePath(szFullPath, szFramePath, 
            MAX_PATH, iLoop, '/');

        //NiSourceTexture::Create(szFramePath);
        NiSourceTexturePtr spTexture = NULL; 
        int iWidth;
        int iHeight;

        // Check for an error loading the file
        if (!MyiTexture::CreateTextureFromFile(szFramePath, iWidth, iHeight, 
            spTexture))
        {
            char szBuffer[1024];
            NiSprintf(szBuffer, 1024,
                "Error:: Loading Flip Book Texture       Base "
                "Name ( %s ) Index:: %d Frame ( %s ) \n", szFullPath, iLoop, 
                szFramePath);
            DtExt_Err(szBuffer);
        }

        AddTextureToList(iTextureID, spTexture, true, szFramePath, 
            MObject::kNullObj);
    }
}
//---------------------------------------------------------------------------
SourceTexture* MyiTextureManager::AddTextureToList(int iTextureID, 
        NiSourceTexturePtr spTexture, bool bNeedsAlpha, char* szFileName,
        MObject kTextureNode)
{
    SourceTexture* pNewSource = NiNew SourceTexture();
    pNewSource->m_iTextureID = iTextureID;
    pNewSource->m_spNiSourceTexture = spTexture;
    pNewSource->m_bNeedsAlpha = bNeedsAlpha;
    pNewSource->m_kTextureNode = kTextureNode;

    // Copy the FileName
    if(szFileName != NULL)
    {
        size_t stLen = strlen(szFileName) + 1;
        pNewSource->m_szFileName = NiAlloc(char, stLen);
        NiStrcpy(pNewSource->m_szFileName, stLen, szFileName);
    }

    // Check for a duplicate using exactly the same file
    if(strcmp(szFileName,""))
    {
        SourceTexture* pkDup = FindTexture(pNewSource);
        if(pkDup != NULL)
        {
            pNewSource->m_spNiSourceTexture = pkDup->m_spNiSourceTexture;
        }
    }


    m_kTextureList.AddTail(pNewSource);

    return pNewSource;
}
//---------------------------------------------------------------------------
NiSourceTexturePtr MyiTextureManager::GetTexture(char* szFileName)
{
    SourceTexture* pTexture = FindTexture(szFileName);

    if(pTexture == NULL)
        return NULL;

    return pTexture->m_spNiSourceTexture;
}
//---------------------------------------------------------------------------
bool MyiTextureManager::NeedsAlphaProperty(char* szFileName)
{
    SourceTexture* pTexture = FindTexture(szFileName);

    if(pTexture == NULL)
        return false;

    return pTexture->m_bNeedsAlpha;
}
//---------------------------------------------------------------------------
NiSourceTexturePtr MyiTextureManager::GetTexture(MObject kTextureNode)
{

    SourceTexture* pTexture = FindTexture(kTextureNode);

    if(pTexture == NULL)
        return NULL;

    return pTexture->m_spNiSourceTexture;
}
//---------------------------------------------------------------------------
SourceTexture* MyiTextureManager::FindTexture(char* szFileName)
{
    NiTListIterator kIter = m_kTextureList.GetHeadPos();

    // Scan the list looking for the texture
    while(kIter != NULL)
    {
        if(strcmp(m_kTextureList.Get(kIter)->m_szFileName, szFileName) == 0)
        {
            return m_kTextureList.Get(kIter);
        }

        kIter = m_kTextureList.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
SourceTexture* MyiTextureManager::FindTexture(SourceTexture* pkTexture)
{
    NiTListIterator kIter = m_kTextureList.GetHeadPos();

    // Scan the list looking for the texture
    while(kIter != NULL)
    {
        if(strcmp(m_kTextureList.Get(kIter)->m_szFileName, 
            pkTexture->m_szFileName) == 0)
        {
            if (pkTexture && pkTexture->m_spNiSourceTexture)
            {
                if ((pkTexture->m_spNiSourceTexture->GetSourcePixelData() ==
                        NULL) &&
                    (m_kTextureList.Get(kIter)->m_spNiSourceTexture->
                        GetSourcePixelData() == NULL))
                {
                    return m_kTextureList.Get(kIter);
                }

                if( pkTexture->m_spNiSourceTexture->GetSourcePixelData() &&
                    m_kTextureList.Get(kIter)->m_spNiSourceTexture->
                        GetSourcePixelData() &&
                    (pkTexture->m_spNiSourceTexture->GetSourcePixelData()->
                        GetPixelFormat() ==
                    m_kTextureList.Get(kIter)->m_spNiSourceTexture->
                        GetSourcePixelData()->GetPixelFormat()))
                {
                    return m_kTextureList.Get(kIter);
                }
            }
        }

        kIter = m_kTextureList.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
SourceTexture* MyiTextureManager::FindTexture(int iTextureID)
{
    NiTListIterator kIter = m_kTextureList.GetHeadPos();

    // Scan the list looking for the texture
    while(kIter != NULL)
    {
        if(m_kTextureList.Get(kIter)->m_iTextureID == iTextureID)
        {
            return m_kTextureList.Get(kIter);
        }

        kIter = m_kTextureList.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
SourceTexture* MyiTextureManager::FindTexture(MObject kTextureNode)
{
    NiTListIterator kIter = m_kTextureList.GetHeadPos();

    // Scan the list looking for the texture
    while(kIter != NULL)
    {
        if(m_kTextureList.Get(kIter)->m_kTextureNode == kTextureNode)
        {
            return m_kTextureList.Get(kIter);
        }

        kIter = m_kTextureList.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MyiTextureManager::IsTextureEffect(int iLightID)
{
    MObject Transform;
    MFnDagNode dgNode;

    // Get the transform based on the LightID
    DtExt_LightGetTransform(iLightID, Transform);

    dgNode.setObject(Transform);

    // Check for the Extra attributes
    if(CheckForExtraAttribute(dgNode, "Ni_DynamicTextureMap", true))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SourceTexture::SourceTexture()
{
    m_iTextureID = -1;
    m_spNiSourceTexture = NULL;
    m_bNeedsAlpha = false;
    m_szFileName = NULL;
    m_kTextureNode = MObject::kNullObj;
}
//---------------------------------------------------------------------------
SourceTexture::~SourceTexture()
{
    m_iTextureID = -1;
    m_spNiSourceTexture = NULL;
    m_bNeedsAlpha = false;

        // Delete
    if(m_szFileName != NULL)
        NiFree(m_szFileName);

    m_szFileName = NULL;
}
//---------------------------------------------------------------------------
