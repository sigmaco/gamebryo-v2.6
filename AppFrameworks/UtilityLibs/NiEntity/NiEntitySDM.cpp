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
#include "NiEntityPCH.h"

#include "NiEntitySDM.h"
#include "NiEntity.h"

NiImplementSDMConstructor(NiEntity, "NiAnimation NiMesh NiFloodgate NiMain");

#ifdef NIENTITY_EXPORT
NiImplementDllMain(NiEntity);
#endif

//---------------------------------------------------------------------------
void NiEntitySDM::Init()
{
    NiImplementSDMInitCheck();

    NiGeneralEntity::_SDMInit();

    // Provided Factories
    NiFactories::_SDMInit();

    // General
    NiEntityCommandManager::_SDMInit();
    NiScene::_SDMInit();

    // Components
    NiActorComponent::_SDMInit();
    NiCameraComponent::_SDMInit();
    NiLightComponent::_SDMInit();
    NiPrefabComponent::_SDMInit();
    NiSceneGraphComponent::_SDMInit();
    NiShadowGeneratorComponent::_SDMInit();
    NiTransformationComponent::_SDMInit();
    NiGeneralComponent::_SDMInit();

    // Interfaces
    NiEntityPropertyInterface::_SDMInit();
    NiEntityInterface::_SDMInit();
    NiEntityComponentInterface::_SDMInit();
    
    // Handlers
    NiExternalAssetNIFHandler::_SDMInit();
    NiExternalAssetKFMHandler::_SDMInit();
    NiExternalAssetKFHandler::_SDMInit();

    // Streaming
    NiEntityStreamingAscii::_SDMInit();

    NiFactories::_SDMRegister();
}
//---------------------------------------------------------------------------
void NiEntitySDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiGeneralEntity::_SDMShutdown();

    // Provided Factories
    NiFactories::_SDMShutdown();

    // General
    NiEntityCommandManager::_SDMShutdown();
    NiScene::_SDMShutdown();

    // Components
    NiActorComponent::_SDMShutdown();
    NiCameraComponent::_SDMShutdown();
    NiLightComponent::_SDMShutdown();
    NiPrefabComponent::_SDMShutdown();
    NiSceneGraphComponent::_SDMShutdown();
    NiShadowGeneratorComponent::_SDMShutdown();
    NiTransformationComponent::_SDMShutdown();
    NiGeneralComponent::_SDMShutdown();

    // Interfaces
    NiEntityPropertyInterface::_SDMShutdown();
    NiEntityInterface::_SDMShutdown();
    NiEntityComponentInterface::_SDMShutdown();

    // Handlers
    NiExternalAssetNIFHandler::_SDMShutdown();
    NiExternalAssetKFMHandler::_SDMShutdown();
    NiExternalAssetKFHandler::_SDMShutdown();

    // Streaming
    NiEntityStreamingAscii::_SDMShutdown();
}
//---------------------------------------------------------------------------
