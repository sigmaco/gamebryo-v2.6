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

//  NiOpProfile inline functions

//---------------------------------------------------------------------------
inline const NiFixedString& NiOpProfile::GetName() const
{
    return m_strName;
}
//---------------------------------------------------------------------------

inline const char* NiOpProfile::GetDescription() const
{
    return m_strDescription;
}
//---------------------------------------------------------------------------
inline NiSystemDesc::RendererID NiOpProfile::GetRenderer() const
{
    return m_eRenderer;
}
//---------------------------------------------------------------------------
inline void NiOpProfile::SetName(const char* pkName)
{
    m_strName = pkName;
}
//---------------------------------------------------------------------------
inline void NiOpProfile::SetDescription(const char* pkDescription)
{
    m_strDescription = pkDescription;
}
//---------------------------------------------------------------------------
inline void NiOpProfile::SetRenderer(NiSystemDesc::RendererID
    eRenderer)
{
    m_eRenderer = eRenderer;
}
//---------------------------------------------------------------------------
inline bool NiOpProfile::GetKeepUnspecifiedFinalizeStreams() const
{
    return m_bKeepUnspecifiedFinalizeStreams;
}
//---------------------------------------------------------------------------
inline void NiOpProfile::SetKeepUnspecifiedFinalizeStreams(bool bValue)
{
    m_bKeepUnspecifiedFinalizeStreams = bValue;
}
//---------------------------------------------------------------------------
inline NiTObjectArray <NiOpStartPtr>& NiOpProfile::GetFinalizeStartNodes()
{
    return m_kStartFinalizeNodes;
}
//---------------------------------------------------------------------------
inline NiTObjectArray <NiOpTerminatorPtr>&
    NiOpProfile::GetFinalizeTerminatorNodes()
{
    return m_kTerminatorFinalizeNodes;
}
//---------------------------------------------------------------------------
inline void NiOpProfile::SetPostResolveOp(const PostResolveOp eOp, 
    const bool bData)
{
    m_abPostResolveOps[eOp] = bData;
}
//---------------------------------------------------------------------------
inline NiOpTerminator* NiOpProfile::FindFinalizeTerminatorNode(
    const NiFixedString& kSemantic, const NiUInt8 uiSemanticIndex) const
{
    for (NiUInt32 ui = 0; ui < m_kTerminatorFinalizeNodes.GetSize(); ui++)
    {
        NiFixedString kCurrentSemantic;
        NiUInt8 uiCurrentSemanticIndex;
        m_kTerminatorFinalizeNodes[ui]->GetSemantic(kCurrentSemantic, 
            uiCurrentSemanticIndex);

        if (kSemantic == kCurrentSemantic &&
            uiSemanticIndex == uiCurrentSemanticIndex)
        {
            return m_kTerminatorFinalizeNodes[ui];
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------

