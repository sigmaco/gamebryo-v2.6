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

#include "NiBrushCommandInterface.h"
#include "MTerrainPlugin.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

NiBrushCommandInterface::NiBrushCommandInterface()
{
    m_pkTerrainInteractor = 0;
    m_spActivePointSet = 0;
    m_pmActiveOperation = 0;
    m_fBrushSize = 0;
    
    m_spOriginalPointSet = NULL;
    m_pmActiveFalloff = 0;
    m_pkPassPointInfo = 0;

    m_kName = NiFixedString("Brush Operation");

    m_pkPreviousCommand = 0;
    m_pkNextCommand = 0;
    m_iCurrentIndex = 0;
    m_bSmartPainting = false;
}
//---------------------------------------------------------------------------
NiBrushCommandInterface::NiBrushCommandInterface(
    NiTerrainInteractor* pkTerrain, 
    NiDeformablePointSetPtr spPointSet,
    MBrushType* pmBrush,
    MBrushOperation* pmBrushOperation,
    MFalloff* pmActiveFalloff,
    NiBrushPassPointInfo* pkPointInfo,
    NiPoint3 kIntersection,
    float fBrushSize,
    float fTerrainScale,
    NiPoint3* pkListOfIntersections,
    int iNumberOfIntersections,
    int iCurrentIndex)
{
    m_pkTerrainInteractor = pkTerrain;
    m_spActivePointSet = spPointSet;
    m_spOriginalPointSet = 0;

    m_pmActiveBrush = pmBrush->CloneBrushType();
    m_pmActiveOperation = pmBrushOperation->CloneOperation();
    
    m_kIntersection = kIntersection;
    m_fBrushSize = fBrushSize;
    m_fTerrainScale = fTerrainScale;

    m_pmActiveFalloff = pmActiveFalloff;
    
    m_pkPassPointInfo = pkPointInfo;

    m_kName = NiFixedString("Brush Operation");

    m_pkListOfOrigins = NiNew NiPoint3[iNumberOfIntersections];
    m_iNumberOfOrigins = iNumberOfIntersections;
    
    for(int i = 0; i < iNumberOfIntersections; i++)
    {
        m_pkListOfOrigins[i] = pkListOfIntersections[i];
    }

    m_iCurrentIndex = iCurrentIndex;

    m_pkPreviousCommand = 0;
    m_pkNextCommand = 0;
    m_bSmartPainting = false;
}

//---------------------------------------------------------------------------
NiBrushCommandInterface::~NiBrushCommandInterface()
{
    m_pkTerrainInteractor->InvalidatePointSet(m_spActivePointSet);
    m_pkTerrainInteractor->InvalidatePointSet(m_spOriginalPointSet);

    m_pkTerrainInteractor = 0;
    m_spActivePointSet = 0;
    m_spOriginalPointSet = 0;
    
    m_pmActiveOperation->Dispose();    
    m_pmActiveFalloff = 0;
    m_pkPassPointInfo = 0;
    m_pmActiveBrush->Dispose();

    NiDelete [] m_pkListOfOrigins;
    m_iNumberOfOrigins = 0;
    m_iCurrentIndex = 0;

}
//---------------------------------------------------------------------------
void NiBrushCommandInterface::AddReference()
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiBrushCommandInterface::RemoveReference()
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiBrushCommandInterface::GetName()
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiBrushCommandInterface::DoCommand(
    NiEntityErrorInterface* pkErrors, bool bUndoable)
{
    NI_UNUSED_ARG(bUndoable);
    NI_UNUSED_ARG(pkErrors);

    // Initialize the the bound.
    NiBound kBound;
    kBound.SetCenterAndRadius(m_kIntersection, 
        m_pmActiveBrush->GetSizeOuter() * m_fTerrainScale);

    // Find all the deformable points.
    NiUInt32 uiNumPoints = 
        m_pmActiveOperation->m_pmElement->PreparePoints(kBound,
        m_pkTerrainInteractor, m_spActivePointSet);

    // Initialize all the points.
    m_pmActiveBrush->CategoriseVerticies(m_kIntersection, 
        uiNumPoints, m_spActivePointSet, m_pkPassPointInfo, 
        m_pmActiveFalloff, m_pkTerrainInteractor, m_fTerrainScale);

    // Applying the operation to the pointset
    if (m_spActivePointSet->IsDataValid() && uiNumPoints)
    {
        // Set the info to the active set.
        m_pkPassPointInfo->m_spPointSet = m_spActivePointSet;

        // Copy the current set before we modify it.
        m_spOriginalPointSet = m_spActivePointSet->Clone();
        
        // Set the name
        m_kName = MStringToCharPointer(m_pmActiveOperation->m_pmName);

        NiInt32 iMin = 0;
        NiInt32 iMax = 1;

        switch (m_spActivePointSet->GetPointType())
        {
        case NiDeformablePoint::VERTEX:
            {
                // Get the entity or the terrain.
                MEntity* pmEntity = 0;
                MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
                NIASSERT(pmEntity);
                NIASSERT(pmEntity->GetNiEntityInterface());
                NIVERIFY(pmEntity->GetNiEntityInterface()->GetPropertyData(
                    NiTerrainComponent::ms_kPropTerrainMinHeight, iMin));
                NIVERIFY(pmEntity->GetNiEntityInterface()->GetPropertyData(
                    NiTerrainComponent::ms_kPropTerrainMaxHeight, iMax));
            }
            break;

        case NiDeformablePoint::MASK_PIXEL:
            iMin = 0;
            iMax = 255;
            break;

        default:
            NIASSERT(!"Invalid deformation type");  
            break;
        }

        // Tell each operation to apply it's algorithm to the element
        m_pmActiveOperation->ApplyOperation(m_pkPassPointInfo, 
            &m_kIntersection, m_pmActiveBrush->GetSizeInner(), 
            (float)iMin, (float)iMax);

        if (m_pmActiveFalloff)
        {
            m_pmActiveOperation->ApplyFalloff(m_pkPassPointInfo, 
                (float)iMin, (float)iMax);
        }
        
        // Commit the changes to the terrain
        m_pmActiveOperation->m_pmElement->CommitPoints(
            m_pkTerrainInteractor, 
            m_spActivePointSet, m_kIntersection, 
            m_pmActiveBrush->GetSizeOuter());

        NiDefaultErrorHandler kErrors;
        m_pkTerrainInteractor->UpdateSectors(0.0f,
            &kErrors,
            MFramework::Instance->ExternalAssetManager);
    } 

    m_pkPassPointInfo->m_spPointSet = 0;
}

//---------------------------------------------------------------------------
void NiBrushCommandInterface::UndoCommand(
    NiEntityErrorInterface* pkErrors)
{
    NI_UNUSED_ARG(pkErrors);
    //Commit the changes to the terrain
    if (m_spOriginalPointSet)
    {
        m_pmActiveOperation->m_pmElement->CommitPoints(m_pkTerrainInteractor, 
            m_spOriginalPointSet, m_kIntersection, 
            m_pmActiveBrush->GetSizeOuter());
    }

    // Update the terrain
    MEntity* pmEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    pmEntity->Update(0.0, MFramework::Instance->ExternalAssetManager);

    m_pkPassPointInfo->m_spPointSet = 0;

}
//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetBrushElement(MBrushElement* pmBrushElement)
{
    NIASSERT(m_pmActiveOperation->m_pmElement == 0);
    m_pmActiveOperation->m_pmElement = pmBrushElement;
}

//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetBrushOperation(
    MBrushOperation* pmBrushOperation)
{
    NIASSERT(m_pmActiveOperation == 0);
    m_pmActiveOperation = pmBrushOperation;
}

//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetIntersection(NiPoint3 kIntersection)
{
    m_kIntersection = kIntersection;
}

//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetBrushSize(float fBrushSize)
{
    m_fBrushSize = fBrushSize;
}

//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetBrushStrength(float fBrushStrength)
{
    m_pmActiveOperation->m_fStrength = fBrushStrength;
}
//---------------------------------------------------------------------------
bool NiBrushCommandInterface::HasChanged(NiDeformablePoint* pkPoints)
{
    float fRadius = m_pmActiveBrush->GetSizeInner();
    int iIndex = m_iCurrentIndex;
        
    pkPoints->SetHasChanged(false);
    
    // Checks previous oepration done to see if the point was modified
    for(int i = 0; i <= m_iNumberOfOrigins; i++)
    {
        NiPoint2 kDistance(m_kIntersection.x - m_pkListOfOrigins[iIndex].x,
            m_kIntersection.y - m_pkListOfOrigins[iIndex].y);

        if(kDistance.Length() <= (fRadius*2))
        {
            NiPoint2 kPointDist(
            pkPoints->GetWorldLocation().x - m_pkListOfOrigins[iIndex].x,
            pkPoints->GetWorldLocation().y - m_pkListOfOrigins[iIndex].y);

            
            if(kPointDist.Length() <= fRadius)
            {
                pkPoints->SetHasChanged(true);
                return true;
            }
            
        }
        
        iIndex--;

        if(iIndex < 0)
            iIndex = m_iNumberOfOrigins;
    }

    
    return pkPoints->GetHasChanged();
}
//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetPreviousCommand(
    NiBrushCommandInterface* pkToSet)
{
    m_pkPreviousCommand = pkToSet;
}
//---------------------------------------------------------------------------
void NiBrushCommandInterface::SetNextCommand(NiBrushCommandInterface* pkToSet)
{
    m_pkNextCommand = pkToSet;
}
//---------------------------------------------------------------------------
NiBrushCommandInterface* NiBrushCommandInterface::GetPreviousCommand()
{
    return m_pkPreviousCommand;
}
//---------------------------------------------------------------------------
NiBrushCommandInterface* NiBrushCommandInterface::GetNextCommand()
{
    return m_pkNextCommand;
}
//---------------------------------------------------------------------------
NiUInt32 NiBrushCommandInterface::GetSize()
{
    if (m_spOriginalPointSet)
    {
        NiUInt32 uiSize = 0;
        switch (m_spOriginalPointSet->GetPointType())
        {
        case NiDeformablePoint::VERTEX:
            ((NiTQuickReleaseObjectPool<NiTerrainVertex>*)
                (m_spOriginalPointSet->GetOriginalPointPool()))->GetMaxSize(
                uiSize);
            break;

        case NiDeformablePoint::MASK_PIXEL:
            ((NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*)
                (m_spOriginalPointSet->GetOriginalPointPool()))->GetMaxSize(
                uiSize);
            break;

        default:
            NIASSERT(!"Invalid deformation type");  
            break;
        }

        return uiSize + m_spOriginalPointSet->GetNumPoints();
    }

    return 0;
}
