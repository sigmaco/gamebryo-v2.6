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
#include "SceneDesignerFrameworkPCH.h"

#include "MPaletteManager.h"
#include "MEventManager.h"
#include "MFramework.h"
#include "MUtility.h"

using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MPaletteManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MPaletteManager();
    }
}
//---------------------------------------------------------------------------
void MPaletteManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MPaletteManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MPaletteManager* MPaletteManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MPaletteManager::MPaletteManager()
{
    m_pmPalettes = new ArrayList();
    m_strPaletteFolder = String::Empty;
    m_strPaletteFileExtension = "pal";
}
//---------------------------------------------------------------------------
void MPaletteManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        RemoveAllPalettes();
    }
}
//---------------------------------------------------------------------------
unsigned int MPaletteManager::get_PaletteCount()
{
    MVerifyValidInstance;

    return m_pmPalettes->Count;
}
//---------------------------------------------------------------------------
MPalette* MPaletteManager::GetPalettes()[]
{
    MVerifyValidInstance;

    return dynamic_cast<MPalette*[]>(m_pmPalettes->ToArray(
        __typeof(MPalette)));
}
//---------------------------------------------------------------------------
MPalette* MPaletteManager::GetPaletteByName(String* strName)
{
    MVerifyValidInstance;

    for (int i = 0; i < m_pmPalettes->Count; i++)
    {
        MPalette* pmPalette = dynamic_cast<MPalette*>(m_pmPalettes->Item[i]);
        MAssert(pmPalette != NULL, "Invalid palette array!");
        if (pmPalette->Name->Equals(strName))
        {
            return pmPalette;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MPaletteManager::AddPalette(MPalette* pmPalette)
{
    MVerifyValidInstance;

    if (m_pmPalettes->Contains(pmPalette))
    {
        return false;
    }

    m_pmPalettes->Add(pmPalette);
    MEventManager::Instance->RaisePaletteAdded(pmPalette);

    return true;
}
//---------------------------------------------------------------------------
void MPaletteManager::RemovePalette(MPalette* pmPalette)
{
    MVerifyValidInstance;

    if (pmPalette == m_pmActivePalette)
    {
        set_ActivePalette(NULL);
    }
    m_pmPalettes->Remove(pmPalette);
    MEventManager::Instance->RaisePaletteRemoved(pmPalette);
    pmPalette->Dispose();
}
//---------------------------------------------------------------------------
void MPaletteManager::RemoveAllPalettes()
{
    MVerifyValidInstance;

    for (int i = 0; i < m_pmPalettes->Count; i++)
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            m_pmPalettes->Item[i]);
        //MEventManager::Instance->RaisePaletteRemoved(
        //    dynamic_cast<MPalette*>(m_pmPalettes->Item[i]));
        if (pmDisposable)
        {
            pmDisposable->Dispose();
        }
    }
    m_pmPalettes->Clear();
}
//---------------------------------------------------------------------------
MPalette* MPaletteManager::get_ActivePalette()
{
    MVerifyValidInstance;

    return m_pmActivePalette;
}
//---------------------------------------------------------------------------
void MPaletteManager::set_ActivePalette(MPalette* pmActivePalette)
{
    MVerifyValidInstance;

    if (pmActivePalette == NULL || m_pmPalettes->Contains(pmActivePalette))
    {
        MPalette* pmOldActivePalette = m_pmActivePalette;
        m_pmActivePalette = pmActivePalette;
        MEventManager::Instance->RaiseActivePaletteChanged(m_pmActivePalette,
            pmOldActivePalette);
    }
}
//---------------------------------------------------------------------------
String* MPaletteManager::get_PaletteFolder()
{
    MVerifyValidInstance;

    return m_strPaletteFolder;
}
//---------------------------------------------------------------------------
void MPaletteManager::set_PaletteFolder(String* strPath)
{
    MVerifyValidInstance;

    m_strPaletteFolder = strPath;
}
//---------------------------------------------------------------------------
String* MPaletteManager::get_PaletteFileExtension()
{
    return m_strPaletteFileExtension;
}
//---------------------------------------------------------------------------
MPalette* MPaletteManager::LoadPalette(String* strPath)
{
    MVerifyValidInstance;

    FileInfo* pmFileInfo = new FileInfo(strPath);
    String* strFileName = pmFileInfo->Name;
    String* strPaletteName = strFileName->Substring(0, 
        strFileName->LastIndexOf("."));

    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent("GSA");

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Load the scene
    const char* pcFilename = MStringToCharPointer(strPath);
    bool bSuccess = NIBOOL_IS_TRUE(pkEntityStreaming->Load(pcFilename, false, 
        true));

    // Report errors.
    MUtility::AddErrorInterfaceMessages(spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    if (!bSuccess)
        return NULL;
    
    NIASSERT(pkEntityStreaming->GetSceneCount() == 1);
    if (pkEntityStreaming->GetSceneCount() != 1)
        return NULL;

    MScene* pmScene = MSceneFactory::Instance->Get(
        pkEntityStreaming->GetSceneAt(0));

    pmScene->Name = strPaletteName;

    MPalette* pmPalette = new MPalette(pmScene);
    
    // Mark palette scene and entities as clean after being converted.
    pmPalette->Dirty = false;
    MEntity* amEntities[] = pmPalette->GetEntities();
    for (int i = 0; i < amEntities->Length; ++i)
    {
        amEntities[i]->Dirty = false;
    }

    // If the palette file was converted from an old file or modified at 
    // load time, mark it as dirty.
    if (pkEntityStreaming->HasBeenConverted(pmScene->GetNiScene()) ||
        pkEntityStreaming->HasBeenModified(pmScene->GetNiScene()))
    {
        pmPalette->Dirty = true;
    }

    pkEntityStreaming->RemoveAllScenes(); // Because static instance

    return pmPalette;
}
//---------------------------------------------------------------------------
void MPaletteManager::SavePalette(MPalette* pmPalette, String* strFilename,
    String* strFormat)
{
    MVerifyValidInstance;

    MAssert(pmPalette->Scene->GetNiScene() != NULL, 
        "Null scene passed to Palette Manager");

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    
    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Insert desired scenes
    pkEntityStreaming->InsertScene(pmPalette->Scene->GetNiScene());

    // Save the scene
    const char* pcFilename = MStringToCharPointer(strFilename);
    char acAbsPath[NI_MAX_PATH];
    NiStrcpy(acAbsPath, NI_MAX_PATH, pcFilename);
    MFreeCharPointer(pcFilename);

    NIASSERT(!NiPath::IsRelative(acAbsPath));
    if (!NiPath::IsUniqueAbsolute(acAbsPath))
    {
        NiPath::RemoveDotDots(acAbsPath);
    }

    pkEntityStreaming->Save(acAbsPath);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    pkEntityStreaming->RemoveAllScenes(); // Because static instance
}
//---------------------------------------------------------------------------
