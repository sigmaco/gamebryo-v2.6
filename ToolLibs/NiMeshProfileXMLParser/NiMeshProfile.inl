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

//  NiMeshProfile inline functions

//---------------------------------------------------------------------------
inline const NiFixedString& NiMeshProfile::GetName() const
{
    return m_strName;
}
//---------------------------------------------------------------------------
inline const char* NiMeshProfile::GetDescription() const
{
    return m_strDescription;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfile::GetRecomputeNormals() const
{
    return m_bRecomputeNormals;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfile::GetKeepUnspecifiedFinalizeStreams() const
{
    return m_bKeepUnspecifiedFinalizeStreams;
}
//---------------------------------------------------------------------------
inline unsigned int NiMeshProfile::GetFinalizeStreamCount() const
{
    return m_kFinalizeStreams.GetSize();
}
//---------------------------------------------------------------------------
inline const NiMeshProfileStream* NiMeshProfile::GetFinalizeStream(
    unsigned int uiIndex) const
{
    return m_kFinalizeStreams[uiIndex];
}
//---------------------------------------------------------------------------
inline const NiTObjectArray <NiMeshProfileStreamPtr>* 
    NiMeshProfile::GetFinalizeStreams() const
{
    return &m_kFinalizeStreams;
}

