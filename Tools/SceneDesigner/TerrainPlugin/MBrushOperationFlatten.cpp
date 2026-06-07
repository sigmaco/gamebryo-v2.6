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

#include "MBrushOperationFlatten.h"
#include "MBrushType.h"


using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushOperationFlatten::MBrushOperationFlatten() : MBrushOperation()
{
}
//---------------------------------------------------------------------------
MBrushOperationFlatten::MBrushOperationFlatten(
    MBrushOperationFlatten* pkCopy) :
    MBrushOperation(pkCopy)
{
    m_fFlattenValue = pkCopy->m_fFlattenValue;
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationFlatten::CloneOperation()
{
    MBrushOperation* pmResult = new MBrushOperationFlatten(this);
    return pmResult;
}
//---------------------------------------------------------------------------
void MBrushOperationFlatten::StartOperation(NiPoint3& kOrigin, 
    NiBrushPassPointInfo*, 
    IInteractionMode::MouseButtonType)
{
    MEntity* pmEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    NiPoint3 kTerrainSpaceOrigin;
    NiPoint3 kTranslate;
    NiMatrix3 kRotate;
    float fScale;

    pmEntity->GetNiEntityInterface()->GetPropertyData(
        "Translation", kTranslate);
    pmEntity->GetNiEntityInterface()->GetPropertyData("Rotation", kRotate);
    pmEntity->GetNiEntityInterface()->GetPropertyData("Scale", fScale);

    kTerrainSpaceOrigin = kRotate.Inverse() * (
        (kOrigin - kTranslate) / fScale);
    // uses the point where the mouse is to flatten to
    m_fFlattenValue = kTerrainSpaceOrigin.z;
}
//---------------------------------------------------------------------------
void MBrushOperationFlatten::ApplyOperation(NiBrushPassPointInfo* 
    pkActivePoints, NiPoint3* pkOrigin, float fRadius, float fMinHeight, 
    float fMaxHeight)
{
    NI_UNUSED_ARG(fMaxHeight);
    NI_UNUSED_ARG(fMinHeight);
    NI_UNUSED_ARG(fRadius);
    NI_UNUSED_ARG(pkOrigin);
    NiDeformablePoint* pkPickWTV;

    //Apply the standard elevation algorithm to all inner points:
    unsigned int uiSize = pkActivePoints->m_kInnerPoints.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ++ui)  
    {
        pkPickWTV = pkActivePoints->m_kInnerPoints.GetAt(ui);
        if (pkPickWTV->IsActive())
        {
            m_pmElement->SetValue(pkPickWTV, m_fFlattenValue);
        }
    }
}
//---------------------------------------------------------------------------
void MBrushOperationFlatten::ApplyFalloff(NiBrushPassPointInfo* pkPoints,
    float fMinHeight, float fMaxHeight)
{
    NI_UNUSED_ARG(fMaxHeight);
    NI_UNUSED_ARG(fMinHeight);
    unsigned int uiNumPoints = pkPoints->m_kOuterPoints.GetSize();
    NiBrushPassPointInfo::FalloffData kFalloffData;
    float fValue;
    float fIncrement;
           
    NiDeformablePoint* pkPoint;

    for (unsigned int ui = 0; ui < uiNumPoints; ++ui)
    {
        kFalloffData = pkPoints->m_kOuterPoints.GetAt(ui);
        
        pkPoint = kFalloffData.m_pkActual;
        if (!pkPoint->IsActive())
            continue;

        fIncrement = m_fFlattenValue - pkPoint->GetValue();

        fValue = pkPoint->GetValue() + 
            fIncrement * kFalloffData.m_fInterpolatedModifier;

        m_pmElement->SetValue(pkPoint, fValue);  
        
    }
}
//---------------------------------------------------------------------------
void MBrushOperationFlatten::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList
    )
{
    m_pmElement->GetOperationFullPanel(pmPanel,bInList,this);
}
