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
#include "MBrushOperationAlign.h"
#include "MBrushType.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushOperationAlign::MBrushOperationAlign() : MBrushOperation()
{
}
//---------------------------------------------------------------------------
MBrushOperationAlign::MBrushOperationAlign(MBrushOperationAlign* pkCopy) :
    MBrushOperation(pkCopy)
{
    m_fAlignValue = pkCopy->m_fAlignValue;
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationAlign::CloneOperation()
{
    MBrushOperation* toReturn = new MBrushOperationAlign(this);
    return toReturn;
}
//---------------------------------------------------------------------------
void MBrushOperationAlign::StartOperation(NiPoint3& kOrigin, 
    NiBrushPassPointInfo* pkActivePoints, 
    IInteractionMode::MouseButtonType eType)
{
    MEntity* pmEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    NiPoint3 kTerrainSpaceOrigin;
    NiPoint3 kTranslate;
    NiMatrix3 kRotate;
    float fScale;

    NIVERIFY(pmEntity->GetNiEntityInterface()->GetPropertyData(
        "Translation", kTranslate));

    NIVERIFY(pmEntity->GetNiEntityInterface()->GetPropertyData("Rotation",
        kRotate));

    NIVERIFY(pmEntity->GetNiEntityInterface()->GetPropertyData("Scale",
        fScale));

    kTerrainSpaceOrigin = kRotate.Inverse() * (
        (kOrigin - kTranslate) / fScale);

    switch (eType)
    { // get mouse button
        case IInteractionMode::MouseButtonType::LeftButton:
            // uses the point where the mouse is to flatten to
            m_fAlignValue = kTerrainSpaceOrigin.z;
            break;
        case IInteractionMode::MouseButtonType::RightButton:
            // uses the maximum value to flatten to
            m_fAlignValue = -FLT_MAX;
            for (unsigned int i=0; 
                i < pkActivePoints->m_kInnerPoints.GetSize(); i++)  
            {
                if (m_pmElement->GetValue(pkActivePoints->
                    m_kInnerPoints.GetAt(i)) > m_fAlignValue)
                {
                    m_fAlignValue = m_pmElement->GetValue
                        (pkActivePoints->m_kInnerPoints.GetAt(i));
                }
            }
            break;
        case IInteractionMode::MouseButtonType::MiddleButton:
            // uses the minimum value to flatten to
            m_fAlignValue = FLT_MAX;
            for (unsigned int i=0; 
                i<pkActivePoints->m_kInnerPoints.GetSize(); i++)  
            {
                if (m_pmElement->GetValue(pkActivePoints->
                    m_kInnerPoints.GetAt(i)) < m_fAlignValue)
                {
                    m_fAlignValue = m_pmElement->GetValue
                        (pkActivePoints->m_kInnerPoints.GetAt(i));
                }
            }
            break;
        default:
            // an unknown button
            return;
    }
}
//---------------------------------------------------------------------------
void MBrushOperationAlign::ApplyOperation(
    NiBrushPassPointInfo* pkActivePoints, NiPoint3* pkOrigin, float fRadius,
    float fMinHeight, float fMaxHeight)
{
    NI_UNUSED_ARG(fMaxHeight);
    NI_UNUSED_ARG(fMinHeight);
    NI_UNUSED_ARG(fRadius);
    NI_UNUSED_ARG(pkOrigin);
    NiDeformablePoint* pkPickWTV;

    // Apply the standard elevation algorithm to all inner points:
    for (unsigned int ui=0; ui<pkActivePoints->m_kInnerPoints.GetSize(); ui++)
    {
        pkPickWTV = pkActivePoints->m_kInnerPoints.GetAt(ui);
        if (pkPickWTV->IsActive())
        {
            m_pmElement->SetValue(pkPickWTV, m_fAlignValue);
        }
    }
}
//---------------------------------------------------------------------------
void MBrushOperationAlign::ApplyFalloff(NiBrushPassPointInfo* pkPoints,
    float fMinHeight, float fMaxHeight)
{
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
        
        fIncrement = m_fAlignValue - pkPoint->GetValue();

        fValue = pkPoint->GetValue() + 
            fIncrement * kFalloffData.m_fInterpolatedModifier;
      
        m_pmElement->SetValue(pkPoint, NiClamp(fValue, fMinHeight, 
            fMaxHeight));
        
    }
}
//---------------------------------------------------------------------------
void MBrushOperationAlign::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList
    )
{
    m_pmElement->GetOperationFullPanel(pmPanel, bInList,this);
}
