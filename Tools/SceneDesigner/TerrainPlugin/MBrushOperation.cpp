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

#include "MBrushOperation.h"
#include "MTerrainPlugin.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushOperation::MBrushOperation()
{
    m_bUseAllVertices = true;
    m_fStrength = 0;
    m_iNextPropertyX = 0;
    m_iNextPropertyY = 0;
    m_pmElement = 0;
    m_bInList = false;
    m_pmName = S"";
    m_pmOperationIcon = 0;

    Update(false);
}
//---------------------------------------------------------------------------
MBrushOperation::MBrushOperation(MBrushOperation* pmCopy)
{
    m_fStrength = pmCopy->m_fStrength;
    m_fPercent = pmCopy->m_fPercent;
    m_pmName = pmCopy->m_pmName;    
    m_pmElement = pmCopy->m_pmElement->CloneOperation();
    m_pmOperationIcon = pmCopy->m_pmOperationIcon;
    m_pmElement->SetAssociatedOperation(this);
}
//---------------------------------------------------------------------------
void MBrushOperation::ApplyFalloff(NiBrushPassPointInfo* pkPoints, 
    float fMinHeight, float fMaxHeight)
{
    unsigned int uiNumPoints = pkPoints->m_kOuterPoints.GetSize();
    NiBrushPassPointInfo::FalloffData kFalloffData;
    float fValue;
    float fDiff;
   
    NiDeformablePoint* pkPoint;

    for (unsigned int ui = 0; ui < uiNumPoints; ++ui)
    {
        kFalloffData = pkPoints->m_kOuterPoints.GetAt(ui);

        pkPoint = kFalloffData.m_pkActual;
        if (!pkPoint->IsActive())
            continue;
        
        fDiff = kFalloffData.m_fInnerBoundaryValue - 
            kFalloffData.m_fOuterBoundaryValue;
        
        fValue = kFalloffData.m_fOuterBoundaryValue + (
            kFalloffData.m_fInterpolatedModifier * fDiff
            );
       
        m_pmElement->SetValue(pkPoint, NiClamp(fValue, fMinHeight, 
            fMaxHeight));
        
    }
}
//---------------------------------------------------------------------------
void MBrushOperation::GetOptionInformation(
    System::Windows::Forms::Label* pmElementDesc)
{
    pmElementDesc->Text = "";
    m_pmElement->GetOptionInformation(pmElementDesc);
}
//---------------------------------------------------------------------------
void MBrushOperation::ReadValues(MBrushOperation* pmOperation)
{
    m_fStrength = pmOperation->m_fStrength;
    m_fPercent = pmOperation->m_fPercent;
    m_bUseAllVertices = pmOperation->m_bUseAllVertices;
}
//---------------------------------------------------------------------------
void MBrushOperation::Update(bool bUpdateElement)
{
    if (bUpdateElement && m_pmElement)
        m_pmElement->Update(this);
}
//---------------------------------------------------------------------------
void MBrushOperation::Do_Dispose(bool)
{
    m_pmElement = NULL;
}
