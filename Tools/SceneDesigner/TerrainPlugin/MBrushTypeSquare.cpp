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

#include "MBrushTypeSquare.h"


using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushTypeSquare::MBrushTypeSquare()
{
    m_pmBrushTypeName = S"Square";
}
//---------------------------------------------------------------------------
MBrushTypeSquare::MBrushTypeSquare(MBrushTypeSquare* pmCopy)
{
    this->m_fSizeInner = pmCopy->m_fSizeInner;
    this->m_fSizeInnerSqr = pmCopy->m_fSizeInnerSqr;
    this->m_fSizeOuter = pmCopy->m_fSizeOuter;
    this->m_fSizeOuterSqr = pmCopy->m_fSizeOuterSqr;
    this->m_pkBrushOverlay = pmCopy->m_pkBrushOverlay;
    m_pmBrushTypeName = S"Square";
}
//---------------------------------------------------------------------------
bool MBrushTypeSquare::SetupBrushOverlay()
{
    return false;
}
//---------------------------------------------------------------------------
void MBrushTypeSquare::UpdateBrushOverlay(NiPoint3& kCenter, 
    NiTerrainInteractor* pkTerrainComponent, 
    NiBrushPassPointInfo* pkCategorisedPoints
    )
{
    NI_UNUSED_ARG(kCenter);
    NI_UNUSED_ARG(pkCategorisedPoints);
    NI_UNUSED_ARG(pkTerrainComponent);
}
//---------------------------------------------------------------------------
bool MBrushTypeSquare::ResizeBrushOverlay(float)
{
    return true;
}
//---------------------------------------------------------------------------
void MBrushTypeSquare::CategoriseVerticies(NiPoint3& kIntersection, 
    unsigned int uiNumVerticies, 
    NiDeformablePointSetPtr spActivePoints, 
    NiBrushPassPointInfo* pkCategorisedPoints, 
    MFalloff* pmFalloffFunction, 
    NiTerrainInteractor* pkTerrainComponent,
    float fTerrainScale)
{
    NI_UNUSED_ARG(fTerrainScale);
    NI_UNUSED_ARG(kIntersection);
    NI_UNUSED_ARG(pkCategorisedPoints);
    NI_UNUSED_ARG(pkTerrainComponent);
    NI_UNUSED_ARG(pmFalloffFunction);
    NI_UNUSED_ARG(uiNumVerticies);
}
//---------------------------------------------------------------------------
void MBrushTypeSquare::CalculateBoundaryValues(
    NiPoint3 kIntersection, float fInnerRadius, float fOuterRadius,
    NiBrushPassPointInfo* pkPassPointInfo, 
    NiTerrainInteractor* pkTerrainComponent,
    MBrushElement* pmActiveElement)
{
    NI_UNUSED_ARG(fInnerRadius);
    NI_UNUSED_ARG(fOuterRadius);
    NI_UNUSED_ARG(kIntersection);
    NI_UNUSED_ARG(pkPassPointInfo);
    NI_UNUSED_ARG(pkTerrainComponent);
    NI_UNUSED_ARG(pmActiveElement);
}
//---------------------------------------------------------------------------
MBrushType* MBrushTypeSquare::CloneBrushType()
{
    MBrushType* pmToReturn = new MBrushTypeSquare(this);
    return pmToReturn;
}
