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
#include "NiTQuickReleasePool.h"
#include "MBrushOperationSmooth.h"
#include "MBrushElementSculpt.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//---------------------------------------------------------------------------
MBrushOperationSmooth::MBrushOperationSmooth() : MBrushOperation()
{
}
//---------------------------------------------------------------------------
MBrushOperationSmooth::MBrushOperationSmooth(MBrushOperationSmooth* pmCopy):
    MBrushOperation(pmCopy)
{
}
//---------------------------------------------------------------------------
MBrushOperationSmooth::~MBrushOperationSmooth()
{   
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationSmooth::CloneOperation()
{
    return new MBrushOperationSmooth(this);
}
//---------------------------------------------------------------------------
void MBrushOperationSmooth::ApplyOperation(
    NiBrushPassPointInfo* pkActivePoints, NiPoint3* pkOrigin, float fRadius,
    float fMinHeight, float fMaxHeight)
{
    NI_UNUSED_ARG(fRadius);
    NI_UNUSED_ARG(pkOrigin);
    NiDeformablePoint* pkPoint;    
    NiUInt32 uiNumPoints;
    float fAverage = 0.0f;
    
    // Get the terrain interactor
    MEntity* pmEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    NiTerrainInteractor* pkInteractor;
    size_t fSizet;
    pmEntity->GetNiEntityInterface()->GetPropertyData("Interactor",
        (void*&)pkInteractor, fSizet, 0);
       
    // Inner radius, max strength
    uiNumPoints = pkActivePoints->m_kInnerPoints.GetSize();
    for(unsigned int ui = 0; ui < uiNumPoints; ui++)
    {

        pkPoint = pkActivePoints->m_kInnerPoints.GetAt(ui);
        if (!pkPoint->IsActive())
            continue;

        fAverage = m_pmElement->GetSmoothedValue(pkInteractor,
            pkActivePoints->m_spPointSet, pkPoint);
       
        m_pmElement->SetValue(pkPoint, NiClamp(fAverage, fMinHeight, 
            fMaxHeight)); 
    }
       
    // Outer radius, interpolate strength
    float fOriginal = 0.0f;
    float fDiff = 0.0f;
    uiNumPoints = pkActivePoints->m_kOuterPoints.GetSize();
    for(unsigned int ui = 0; ui < uiNumPoints; ui++)
    {        
        const NiBrushPassPointInfo::FalloffData& kFalloffData = 
            pkActivePoints->m_kOuterPoints.GetAt(ui);        
        pkPoint = kFalloffData.m_pkActual;
        if (!pkPoint->IsActive())
            continue;

        fOriginal = m_pmElement->GetValue(pkPoint);
        fAverage = m_pmElement->GetSmoothedValue(pkInteractor,
            pkActivePoints->m_spPointSet, pkPoint);

        // Interpolate for falloff
        fDiff = (fAverage - fOriginal) * kFalloffData.m_fInterpolatedModifier;
       
        m_pmElement->SetValue(pkPoint, NiClamp(fOriginal + fDiff, fMinHeight,
            fMaxHeight)); 
    }
}
//---------------------------------------------------------------------------
void MBrushOperationSmooth::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList)
{
    m_pmElement->GetOperationFullPanel(pmPanel, bInList, this);
}
//---------------------------------------------------------------------------
void MBrushOperationSmooth::Do_Dispose(bool bDisposing) 
{    
    MBrushOperation::Do_Dispose(bDisposing);
}
