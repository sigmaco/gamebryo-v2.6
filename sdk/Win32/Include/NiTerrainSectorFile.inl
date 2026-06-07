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
inline NiUInt32 NiTerrainSectorFile::GetBlockID() const
{
    if (IsWritable())
    {
        return m_iCurrentBlockID + 1;
    }
    else
    {
        return m_iCurrentBlockID;
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorFile::GetBlockLevel() const
{
    return m_iCurrentBlockLevel;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorFile::SetHeightData(float* pfData, 
    NiUInt32 uiDataLength)
{
    SetStreamData(
        STREAM_HEIGHT, sizeof(float), pfData, uiDataLength);
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorFile::SetNormalData(float* pfData, 
    NiUInt32 uiDataLength)
{
    SetStreamData(
        STREAM_NORMAL, sizeof(float) * 2, pfData, uiDataLength);
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorFile::SetTangentData(float* pfData, NiUInt32 
    uiDataLength)
{
    SetStreamData(
        STREAM_TANGENT, sizeof(float) * 2, pfData, uiDataLength);
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorFile::SetMorphHeightData(float* pfData, 
    NiUInt32 uiDataLength)
{
    SetStreamData(
        STREAM_MORPH_HEIGHT, sizeof(float) * 1, pfData, uiDataLength);
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorFile::SetMorphNormalData(float* pfData, 
    NiUInt32 uiDataLength)
{
    SetStreamData(
        STREAM_MORPH_NORMAL, sizeof(float) * 2, pfData, uiDataLength);
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorFile::SetMorphTangentData(float* pfData, 
    NiUInt32 uiDataLength)
{
    SetStreamData(
        STREAM_MORPH_TANGENT, sizeof(float) * 2, pfData, uiDataLength);
}
//---------------------------------------------------------------------------
inline float* NiTerrainSectorFile::GetHeightData(NiUInt32& uiDataLength)
{
    return GetStreamData(STREAM_HEIGHT, uiDataLength);
}
//---------------------------------------------------------------------------
inline float* NiTerrainSectorFile::GetNormalData(NiUInt32& uiDataLength)
{
    return GetStreamData(STREAM_NORMAL, uiDataLength);
}
//---------------------------------------------------------------------------
inline float* NiTerrainSectorFile::GetTangentData(NiUInt32& uiDataLength)
{
    return GetStreamData(STREAM_TANGENT, uiDataLength);
}
//---------------------------------------------------------------------------
inline float* NiTerrainSectorFile::GetMorphHeightData(NiUInt32& uiDataLength)
{
    return GetStreamData(STREAM_MORPH_HEIGHT, uiDataLength);
}
//---------------------------------------------------------------------------
inline float* NiTerrainSectorFile::GetMorphNormalData(NiUInt32& uiDataLength)
{
    return GetStreamData(STREAM_MORPH_NORMAL, uiDataLength);
}
//---------------------------------------------------------------------------
inline float* NiTerrainSectorFile::GetMorphTangentData(NiUInt32& uiDataLength)
{
    return GetStreamData(STREAM_MORPH_TANGENT, uiDataLength);
}
//---------------------------------------------------------------------------
