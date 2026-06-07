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

//---------------------------------------------------------------------------
inline void NiTerrainInteractor::SetTerrain(
    NiTerrain* pkTerrain)
{
    m_pkTerrain = pkTerrain;
}
//---------------------------------------------------------------------------
inline NiTerrain* NiTerrainInteractor::GetTerrain() const
{
    return m_pkTerrain;
}
//---------------------------------------------------------------------------
inline bool NiTerrainInteractor::InToolMode()
{
    return ms_bInTool;
}
//---------------------------------------------------------------------------
inline void NiTerrainInteractor::SetInToolMode(bool bInToolMode)
{
    ms_bInTool = bInToolMode;
}
//---------------------------------------------------------------------------
inline void NiTerrainInteractor::GetMetaDataFast(float fX, 
    float fMinY, float fMaxY,
    const NiTPrimitiveArray<const NiSurfaceMask*>& kMasks,
    const NiTPrimitiveArray<float>& kValues,
    float& fProbability)
{
    fProbability = 0;
    for (NiUInt32 ui = 0; ui < kMasks.GetSize(); ui++)
    {
        const NiSurfaceMask* pkCurrentMask = kMasks.GetAt(ui);
        
        NiUInt16 usMinX = (NiUInt16)(fX * float(pkCurrentMask->GetWidth()));
        NiUInt16 usMinY = (NiUInt16)(fMinY * float(pkCurrentMask->GetWidth()));
        NiUInt16 usMaxY = (NiUInt16)(fMaxY * float(pkCurrentMask->GetWidth()));

        const float fInf = 1 / 1020.0f;
        float fOpacity = (pkCurrentMask->GetAt(usMinX, usMinY) + 
            pkCurrentMask->GetAt(usMinX, usMaxY)) * fInf;
        
        fProbability += fOpacity * kValues[ui];
    }
}

