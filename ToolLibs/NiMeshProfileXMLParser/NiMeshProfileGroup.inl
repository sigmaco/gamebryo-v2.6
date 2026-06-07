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

//  NiMeshProfileGroup inline functions

//---------------------------------------------------------------------------
inline NiUInt32 NiMeshProfileGroup::GetProfileCount(
    NiSystemDesc::RendererID eRenderer) const
{
    return m_akProfiles[eRenderer].GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshProfileGroup::GetProfileCount() const
{
    NiUInt32 uiCount = 0;
    for (NiUInt32 ui = 0; ui < MESHPROFILE_PARSER_NUM_RENDERERS; ui++)
    {
        uiCount += m_akProfiles[ui].GetSize();
    }
    return uiCount;
}
//---------------------------------------------------------------------------
inline const NiMeshProfile* NiMeshProfileGroup::GetProfile(
    NiSystemDesc::RendererID eRenderer, NiUInt32 uiIndex) const
{
    return m_akProfiles[eRenderer][uiIndex];
}
