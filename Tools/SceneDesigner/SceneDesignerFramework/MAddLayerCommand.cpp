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

#include "MAddLayerCommand.h"
#include "MEventManager.h"
#include "MUtility.h"
#include "MLayerManager.h"
#include "MFramework.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MAddLayerCommand::MAddLayerCommand(String* filename, MLayer* pmActiveLayer)
: m_filename(filename)
, m_pmLayers(0)
, m_pmActiveLayer(pmActiveLayer)
, m_bImportedLayer(false)
{
}
//---------------------------------------------------------------------------
void MAddLayerCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MAddLayerCommand::get_Name()
{
    MVerifyValidInstance;

    FileInfo* fi = new FileInfo(m_filename);
    String* strCommand = fi->Exists ? "Import" : "Create";
    return String::Format("{0} \"{1}\" layer", strCommand, m_filename);
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MAddLayerCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MAddLayerCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    MLayerManager* LayerManager = MLayerManager::Instance;

    // Execute command.
    MLayer* pmLayer = LayerManager->FindLayerByName(m_filename);
    if (pmLayer == NULL)
    {
        m_pmLayers = new List<MLayer*>();

        // this is a completely new layer
        FileInfo* fi = new FileInfo(m_filename);
        if (fi->Exists)
        {
            // if the file exists, import it
            ImportLayer(m_filename, MUtility::GetFormatFromFilename(
                m_filename));
            m_bImportedLayer = true;
        }
        else
        {
            // otherwise create a new empty one
            pmLayer = new MLayer(m_filename);
            NIASSERT(m_pmActiveLayer != NULL);
            LayerManager->AddLayer(pmLayer, m_pmActiveLayer, 
                MLayerManager::EnableEvents);
            m_pmActiveLayer->DoAddExplicitLayer(pmLayer);
            // save the file so the source filename will get set correctly
            pmLayer->Save(false);
            m_pmLayers->Add(pmLayer);
            m_bImportedLayer = false;
        }
    }
    else
    {
        // the layer exists, we're just adding making it explicit to a new
        // layer
        m_pmActiveLayer->DoAddExplicitLayer(pmLayer);
    }
    m_pmActiveLayer->Dirty = true;
}
//---------------------------------------------------------------------------
void MAddLayerCommand::ImportLayer(String* strFilename, String* strFormat)
{
    MVerifyValidInstance;

    MLayerManager* LayerManager = MLayerManager::Instance;
    // Verify that we are not importing a layer that already exists.
    MAssert(LayerManager->FindLayerByName(strFilename) == NULL,
        "Layer already exists!");

    // Before importing the layer, search the existing scene for a
    // terrain entity.
    bool bSceneHasTerrain = false;
    MEntity* amSceneEntities[] = MFramework::Instance->Scene->GetEntities();
    for (int i = 0; i < amSceneEntities->Length; ++i)
    {
        if (amSceneEntities[i]->HasProperty("Terrain Archive"))
        {
            bSceneHasTerrain = true;
            break;
        }
    }

    MFramework::Scenes* pmSceneList = MFramework::Instance->LoadScene(
        strFilename, strFormat, false);
    if (pmSceneList == NULL)
    {
        return;
    }

    NiScene* uberScene = MFramework::Instance->Scene->GetNiScene();

    bool bFirstScene = true;
    NIASSERT(m_pmActiveLayer != NULL);
    MLayer* pmParent = m_pmActiveLayer;

    MFramework::Scenes::Enumerator mEnum = pmSceneList->GetEnumerator();
    while (mEnum.MoveNext())
    {
        MScene* pmScene = mEnum.Current;
        if (LayerManager->GetLayerByName(pmScene->SourceFilename) == NULL)
        {
            MLayer* newLayer = new MLayer(pmScene, uberScene);
            LayerManager->AddLayer(newLayer, pmParent, 
                MLayerManager::EnableEvents);
            m_pmLayers->Add(newLayer);
            if (bFirstScene)
            {
                m_pmActiveLayer->DoAddExplicitLayer(newLayer);
                pmParent = newLayer;
                bFirstScene = false;
            }
        }
    }

    if (bSceneHasTerrain)
    {
        bool bLayersHaveTerrain = false;
        List<MLayer*>::Enumerator mEnum = m_pmLayers->GetEnumerator();
        while (!bLayersHaveTerrain && mEnum.MoveNext())
        {
            MEntity* amLayerEntities[] = mEnum.Current->GetEntities();
            for (int i = 0; i < amLayerEntities->Length; ++i)
            {
                if (amLayerEntities[i]->HasProperty("Terrain Archive"))
                {
                    bLayersHaveTerrain = true;
                    break;
                }
            }
        }

        if (bLayersHaveTerrain)
        {
            MessageBox::Show("The imported layer (or one of its dependencies) "
                "contains a\nterrain entity. A terrain entity already exists "
                "in the\nscene. Only one terrain entity is allowed per scene.",
                "Cannot Import Layer", MessageBoxButtons::OK,
                MessageBoxIcon::Error);

            RemoveLayers();
        }
    }
}
//---------------------------------------------------------------------------
void MAddLayerCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Undo command.
    MLayerManager* LayerManager = MLayerManager::Instance;

    if (m_bImportedLayer)
    {
        if (m_pmLayers != NULL)
        {
            RemoveLayers();
        }
    }
    else
    {
        // we just need to remove the filename from the active layer
        MLayer* pmLayer = LayerManager->FindLayerByName(m_filename);
        NIASSERT(pmLayer != NULL);
        m_pmActiveLayer->DoRemoveExplicitLayer(pmLayer);
    }
}
//---------------------------------------------------------------------------
void MAddLayerCommand::RemoveLayers()
{
    if (m_pmLayers != NULL)
    {
        MLayerManager* LayerManager = MLayerManager::Instance;
        List<MLayer*>::Enumerator en = m_pmLayers->GetEnumerator();
        while (en.MoveNext())
        {
            m_pmActiveLayer->DoRemoveExplicitLayer(en.Current);
            LayerManager->RemoveLayer(en.Current, false);
        }
        m_pmLayers = NULL;
    }
}
//---------------------------------------------------------------------------
