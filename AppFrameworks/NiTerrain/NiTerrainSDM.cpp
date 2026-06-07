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

#include "NiTerrainPCH.h"
#include "NiTerrainSDM.h"

#include "NiTerrainSector.h"
#include "NiTerrainComponent.h"
#include "NiTerrainSectorComponent.h"
#include "NiWaterComponent.h"
#include "NiSurface.h"
#include "NiTerrain.h"

#include <NiFactories.h>

NiImplementSDMConstructor(NiTerrain, "NiMesh NiFloodgate NiMain NiEntity");

#ifdef NITERRAIN_EXPORT
NiImplementDllMain(NiTerrain);
#endif

NiFactoryDeclarePropIntf(NiTerrainComponent);
NiFactoryDeclarePropIntf(NiTerrainSectorComponent);
NiFactoryDeclarePropIntf(NiWaterComponent);

//---------------------------------------------------------------------------
void NiTerrainSDM::Init()
{
    NiImplementSDMInitCheck();

    // Register to GameBryo factory system:
    NiFactoryRegisterPropIntf(NiTerrainComponent);
    NiFactoryRegisterPropIntf(NiTerrainSectorComponent);
    NiFactoryRegisterPropIntf(NiWaterComponent);

    NiSurfacePalette::CreateInstance();
    NiMetaDataStore::CreateInstance();

    NiTerrain::_SDMInit();
    NiTerrainComponent::_SDMInit();
    NiTerrainSectorComponent::_SDMInit();
    NiWaterComponent::_SDMInit();   
    NiSurface::_SDMInit();
}
//---------------------------------------------------------------------------
void NiTerrainSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiSurfacePalette::DestroyInstance();
    NiMetaDataStore::DestroyInstance();

    NiTerrain::_SDMShutdown();
    NiTerrainComponent::_SDMShutdown();
    NiTerrainSectorComponent::_SDMShutdown();
    NiWaterComponent::_SDMShutdown();   
    NiSurface::_SDMShutdown();
}
//---------------------------------------------------------------------------
