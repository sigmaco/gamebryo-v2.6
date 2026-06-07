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
#include "MTerrainPlugin.h"

#include <NiRay.h>
#include "MBrushElementSculpt.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushElementSculpt::MBrushElementSculpt()
{
    m_fMinHeight = -100.0f;
    m_fMaxHeight = 100.0f;
    m_fMinSlope = 0.0f;
    m_fMaxSlope = 90.0f;
    m_bUseSlope = false;
    m_bUseHeight = false;
}
//---------------------------------------------------------------------------
MBrushElementSculpt::MBrushElementSculpt(MBrushElementSculpt* pmToCopy)
{
    m_pmName = pmToCopy->m_pmName;
    m_fMinimumValue = 0;
    m_fMaximumValue = 0;
    m_fMinHeight = pmToCopy->m_fMinHeight;
    m_fMaxHeight = pmToCopy->m_fMaxHeight;
    m_fMinSlope = pmToCopy->m_fMinSlope;
    m_fMaxSlope = pmToCopy->m_fMaxSlope;
    m_bUseSlope = pmToCopy->m_bUseSlope;
    m_bUseHeight = pmToCopy->m_bUseHeight;
}
//---------------------------------------------------------------------------
MBrushElement* MBrushElementSculpt::CloneOperation()
{
    MBrushElementSculpt* pkClone = new MBrushElementSculpt(this);
    
    return pkClone;
}
//---------------------------------------------------------------------------
float MBrushElementSculpt::GetValue(NiTerrainInteractor* pkTerrain, 
    const NiPoint3& kWorldLocation)
{
    NiRay kRay(
        NiPoint3(kWorldLocation.x, kWorldLocation.y, FLT_MAX),
        NiPoint3(0.0f, 0.0f, -1.0f));
    
    if (pkTerrain->Collide(kRay))
    {
        NiPoint3 kIntersection, kNormal;
        kRay.GetIntersection(kIntersection, kNormal);
        return kIntersection.z;
    }
    else
    {
        return FLT_MIN;
    }
}
//---------------------------------------------------------------------------
void MBrushElementSculpt::CommitPoints(NiTerrainInteractor* pkTerrain,
            NiDeformablePointSet* pkPoints, 
            const NiPoint3& kOrigin, const float& fRadius)
{
    NI_UNUSED_ARG(fRadius);
    NIASSERT(pkPoints != NULL);
    NiRay kRay = NiRay(kOrigin, NiPoint3(0,0,-1));
    pkTerrain->CommitChanges(pkPoints); 
}
//---------------------------------------------------------------------------
