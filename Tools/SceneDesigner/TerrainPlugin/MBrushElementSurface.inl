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

inline const NiSurface* MBrushElementSurface::GetSurface()
{
    return m_pkCurSurface;
}
//---------------------------------------------------------------------------
inline void MBrushElementSurface::SetSurface(const NiSurface* pkSurface)
{
    if (m_pkCurSurface == pkSurface)
        return;

    if (pkSurface && m_pmPackageBox && m_pmPackageBox->Items->Count)
    {
        m_bPreventUpdate = true;
        m_pmPackageBox->SelectedItem = new String(
            pkSurface->GetPackage()->GetName());
        m_pmSurfaceBox->SelectedItem = new String(pkSurface->GetName());
        m_bPreventUpdate = false;
    }
    else
    {
        m_pkCurSurface = pkSurface;
    }
}
//---------------------------------------------------------------------------
inline MBrushElement* MBrushElementSurface::CloneOperation()
{
    MBrushElementSurface* pkClone = new MBrushElementSurface(this);
    return pkClone;
}
//---------------------------------------------------------------------------
inline float MBrushElementSurface::GetValue(const NiDeformablePoint* pkPoint)
{
    return pkPoint->GetValue();
}
//---------------------------------------------------------------------------
inline float MBrushElementSurface::GetSmoothedValue(
    NiTerrainInteractor* pkInteractor, NiDeformablePointSet* pkPointSet,
    const NiDeformablePoint* pkPoint)
{
    float fValue;
    pkInteractor->GetSmoothedValue(pkPointSet, pkPoint, fValue, GetSurface());
    return fValue;
}
//---------------------------------------------------------------------------
inline void MBrushElementSurface::SetValue(NiDeformablePoint* pkPoint, 
    float fValue)
{
    // Check the procedural filters
    if (CheckProceduralFilters(pkPoint))
    {
        pkPoint->SetValue(fValue);  

        // Set the point map to remember the value of the modifier for this point
        if (m_pkPointMap)
        {
            float fBlendModifier = 1.0f - (fValue / 255.0f);
            m_pkPointMap->SetAt(&pkPoint->GetWorldLocation(), fBlendModifier);
        }
    }
}
//---------------------------------------------------------------------------
inline void MBrushElementSurface::CommitPoints(NiTerrainInteractor* pkTerrain,
    NiDeformablePointSet* pkPoints, 
    const NiPoint3&, const float&)
{
    NIASSERT(pkPoints != NULL);

    // Change the value of the other masks according to the map modifier.
    if (m_pkPointMap)
    {
        NiUInt32 uiNumPoints = pkPoints->GetNumPoints();
        for (NiUInt32 ui = 0; ui < uiNumPoints; ui++)  
        {       
            NiDeformablePoint* pkPoint = pkPoints->GetAt(ui);
           
            if (!pkPoint->IsActive() && pkPoint->GetValue() != 0.0f)
            {
               float fModifier = 1;
               m_pkPointMap->GetAt(&pkPoint->GetWorldLocation(), fModifier);
                
               pkPoint->SetValue(NiClamp(pkPoint->GetValue() * fModifier, 
                   1.0f, 255.0f));      
               pkPoint->SetActive(true);
            }
        }

        m_pkPointMap->RemoveAll();
        NiDelete m_pkPointMap;
        m_pkPointMap = 0;
    }

    pkTerrain->CommitChanges(pkPoints);    
}
