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

#ifndef NIENTITY_H
#define NIENTITY_H

#include "NiEntityLibType.h"

#include "NiDefaultErrorHandler.h"
#include "NiEntityCloneHelper.h"
#include "NiEntityCommandManager.h"
#include "NiEntityRenderingContext.h"
#include "NiEntitySelectionSet.h"
#include "NiFactories.h"
#include "NiGeneralEntity.h"
#include "NiInterfaceSmartPointer.h"
#include "NiScene.h"
#include "NiSceneRenderView.h"
#include "NiTFactory.h"
#include "NiUniqueID.h"

// Components
#include "NiActorComponent.h"
#include "NiCameraComponent.h"
#include "NiGeneralComponent.h"
#include "NiGeneralComponentProperty.h"
#include "NiLightComponent.h"
#include "NiPrefabComponent.h"
#include "NiSceneGraphComponent.h"
#include "NiShadowGeneratorComponent.h"
#include "NiTransformationComponent.h"

// Interfaces
#include "NiEntityCommandInterface.h"
#include "NiEntityComponentInterface.h"
#include "NiEntityErrorInterface.h"
#include "NiEntityInterface.h"
#include "NiEntityPropertyInterface.h"

// Commands
#include "NiAddRemoveComponentCommand.h"
#include "NiAddRemoveEntityCommand.h"
#include "NiAddRemovePropertyCommand.h"
#include "NiAddRemoveSelectionSetCommand.h"
#include "NiBatchCommand.h"
#include "NiChangeEntityTagsCommand.h"
#include "NiChangeHiddenStateCommand.h"
#include "NiChangePropertyCommand.h"
#include "NiMakePropertyUniqueCommand.h"
#include "NiRenameEntityCommand.h"
#include "NiRenameSelectionSetCommand.h"
#include "NiResetPropertyDataCommandFactory.h"
#include "NiSelectEntitiesCommand.h"
#include "NiSetElementCountCommand.h"

// Change Property Data Commands
#include "NiChangeBoolDataCommand.h"
#include "NiChangeColorADataCommand.h"
#include "NiChangeColorDataCommand.h"
#include "NiChangeEntityPointerDataCommand.h"
#include "NiChangeFloatDataCommand.h"
#include "NiChangeIntDataCommand.h"
#include "NiChangeMatrix3DataCommand.h"
#include "NiChangeNiObjectPointerDataCommand.h"
#include "NiChangePoint2DataCommand.h"
#include "NiChangePoint3DataCommand.h"
#include "NiChangeQuaternionDataCommand.h"
#include "NiChangeShortDataCommand.h"
#include "NiChangeStringDataCommand.h"
#include "NiChangeUIntDataCommand.h"
#include "NiChangeUShortDataCommand.h"

// Reset Property Data Commands
#include "NiResetBoolDataCommand.h"
#include "NiResetColorADataCommand.h"
#include "NiResetColorDataCommand.h"
#include "NiResetEntityPointerDataCommand.h"
#include "NiResetFloatDataCommand.h"
#include "NiResetIntDataCommand.h"
#include "NiResetMatrix3DataCommand.h"
#include "NiResetNiObjectPointerDataCommand.h"
#include "NiResetPoint2DataCommand.h"
#include "NiResetPoint3DataCommand.h"
#include "NiResetQuaternionDataCommand.h"
#include "NiResetShortDataCommand.h"
#include "NiResetStringDataCommand.h"
#include "NiResetUIntDataCommand.h"
#include "NiResetUShortDataCommand.h"

// Streaming
#include "NiBase64.h"
#include "NiDOMTool.h"
#include "NiEntityStreaming.h"
#include "NiEntityStreamingAscii.h"
#include "NiExternalAssetHandler.h"
#include "NiExternalAssetKFHandler.h"
#include "NiExternalAssetKFMHandler.h"
#include "NiExternalAssetManager.h"
#include "NiExternalAssetNIFHandler.h"
#include "NiParamsKF.h"
#include "NiParamsKFM.h"
#include "NiParamsNIF.h"

#include "NiEntitySDM.h"

static NiEntitySDM NiEntitySDMObject;

#endif // NIENTITY_H
