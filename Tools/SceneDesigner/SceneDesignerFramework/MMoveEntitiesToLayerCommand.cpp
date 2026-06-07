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

#include "MSetActiveLayerCommand.h"
#include "MEventManager.h"
#include "MUtility.h"
#include "MLayer.h"
#include "MMoveEntitiesToLayerCommand.h"
#include "MEventManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MMoveEntitiesToLayerCommand::MMoveEntitiesToLayerCommand(
    MLayer* pmDestination)
: m_pmDestination(pmDestination)
, m_pmLayers(new Layers)
{
}
//---------------------------------------------------------------------------
void MMoveEntitiesToLayerCommand::AddEntity(MEntity* pmEntity)
{
    Entities* entities = NULL;
    if (!m_pmLayers->Contains(pmEntity->Layer))
    {
        entities = new Entities;
        m_pmLayers->Item[pmEntity->Layer] = entities;
    } else
    {
        entities = m_pmLayers->Item[pmEntity->Layer];
    }
    entities->Add(pmEntity);
}
//---------------------------------------------------------------------------
void MMoveEntitiesToLayerCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
String* MMoveEntitiesToLayerCommand::get_Name()
{
    MVerifyValidInstance;

    return String::Format("Move selected entites to \"{0}\" layer", 
        m_pmDestination->Name);
}
//---------------------------------------------------------------------------
NiEntityCommandInterface* 
   MMoveEntitiesToLayerCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MMoveEntitiesToLayerCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    MEventManager* ev = MEventManager::Instance;

    Layers::Enumerator layers = m_pmLayers->GetEnumerator();
    while (layers.MoveNext())
    {
        Entities* entities = layers.Current.Value;
        assert(entities);
        Entities::Enumerator en = entities->GetEnumerator();
        MLayer* srcLayer = layers.Current.Key;
        while (en.MoveNext())
        {
            bool frozen = en.Current->Frozen;
            bool hidden = en.Current->Hidden;
            srcLayer->RemoveEntity(en.Current, true, false);
            m_pmDestination->AddEntity(en.Current, true, false);
            if (frozen != en.Current->Frozen)
            {
                ev->RaiseEntityFrozenStateChanged(en.Current, !frozen);
            }
            if (hidden != en.Current->Hidden)
            {
                ev->RaiseEntityHiddenStateChanged(en.Current, !hidden);
            }
        }
        ev->RaiseEntitiesMovedToNewLayer(srcLayer, m_pmDestination, 
            entities->ToArray());
    }
}
//---------------------------------------------------------------------------
void MMoveEntitiesToLayerCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    Layers::Enumerator layers = m_pmLayers->GetEnumerator();
    while (layers.MoveNext())
    {
        Entities* entities = layers.Current.Value;
        assert(entities);
        Entities::Enumerator en = entities->GetEnumerator();
        MLayer* srcLayer = layers.Current.Key;
        while (en.MoveNext())
        {
            m_pmDestination->RemoveEntity(en.Current, true, false);
            srcLayer->AddEntity(en.Current, true, false);
        }
        // pass src and dst reversed since we are undoing
        MEventManager::Instance->
            RaiseEntitiesMovedToNewLayer(m_pmDestination, srcLayer, 
                entities->ToArray());
    }
}
//---------------------------------------------------------------------------
