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

#include "TerrainPluginPCH.h"

#include <NiTerrainInteractor.h>

#include "MTerrainPlugin.h"
#include "MTerrainPickPolicy.h"
#include "NiRay.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//---------------------------------------------------------------------------
MTerrainPickPolicy::MTerrainPickPolicy()
{
    // associate this pick algorithm with entities that have 
    // terrain sector components
    NiTerrainComponent* pkTempComponent = 
        NiNew NiTerrainComponent();
    mTemplateID = MUtility::IDToGuid(pkTempComponent->GetTemplateID());
    NiDelete pkTempComponent;
}
//---------------------------------------------------------------------------
bool MTerrainPickPolicy::PickOnEntity(MEntity* pmEntity, 
    const NiPoint3& kOrigin, const NiPoint3& kDir, 
    NiPick* pkPick, Hashtable* pmPickedObjectToEntity)
{
    NiTerrainInteractor* pkInteractor = 0;
    size_t fSize = 0;

    NiRay kRay = NiRay(kOrigin, kDir);

    if (pmEntity->GetNiEntityInterface()->
        GetPropertyData("Interactor", (void*&)pkInteractor, fSize))
    {
        if(pkInteractor->Collide(kRay))
        {
            // append the results to the provided NiPick object
            NiPoint3 kPosition;
            NiPoint3 kNormal;
            kRay.GetIntersection(kPosition, kNormal);
            NiAVObject* pkTarget = pmEntity->GetSceneRootPointer(0);

            NiPick::Record* pkRecord = pkPick->Add(pkTarget);
            pkRecord->SetIntersection(kPosition);
            pkRecord->SetNormal(kNormal);
            pkRecord->SetDistance((kPosition - kOrigin).Length());

            // associate the pick results with this entity
            Object* pmObj = __box(
                (unsigned int)PtrToUint(pkTarget));
            if (!pmPickedObjectToEntity->Contains(pmObj))
            {
                pmPickedObjectToEntity->Item[pmObj] = pmEntity;
            }
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
