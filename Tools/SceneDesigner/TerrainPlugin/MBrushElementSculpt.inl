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

inline float MBrushElementSculpt::GetValue(const NiDeformablePoint* pkPoint)
{
    return pkPoint->GetValue();
}
//---------------------------------------------------------------------------
inline void MBrushElementSculpt::SetValue(NiDeformablePoint* pkPoint, 
    float fValue)
{
    // Check the procedural filters
    if (CheckProceduralFilters(pkPoint))
    {
        pkPoint->SetValue(fValue);
    }
}
//---------------------------------------------------------------------------
inline unsigned int MBrushElementSculpt::PreparePoints(const NiBound& kBound, 
    NiTerrainInteractor* pkTerrain, NiDeformablePointSet* pkPoints)
{
    pkPoints->SetPointType(NiDeformablePoint::VERTEX);
    return pkTerrain->GetVerticesInBound2D(kBound, pkPoints);
}
//---------------------------------------------------------------------------
inline void MBrushElementSculpt::Do_Dispose(bool)
{
    
}
//---------------------------------------------------------------------------
inline void MBrushElementSculpt::GetPointSpacing(float& spacing)
{
    MEntity* pEntity;
    NiTerrainInteractor* pInteractor;
    size_t size = 0;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pEntity);

    pEntity->GetNiEntityInterface()->GetPropertyData("Interactor",
        (void*&)pInteractor, size, 0);

    pInteractor->GetPointSpacing(NiDeformablePoint::VERTEX, 0, spacing);
}