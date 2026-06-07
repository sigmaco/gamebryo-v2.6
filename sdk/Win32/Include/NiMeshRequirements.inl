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
inline NiSystemDesc::RendererID NiMeshRequirements::GetRenderer() const
{
    return m_eRenderer;
}
//---------------------------------------------------------------------------
inline void NiMeshRequirements::SetRenderer(NiSystemDesc::RendererID eRenderer)
{
    m_eRenderer = eRenderer;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshRequirements::GetSetCount() const
{
    return m_kRequirementSets.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshRequirements::AddRequirement(NiUInt32 uiSet,
    SemanticRequirement* pkRequirement)
{
    NIASSERT(uiSet < GetSetCount());

    RequirementSet* pkSet = GetSetAt(uiSet);
    if (!pkSet)
        return UINT_MAX;

    return pkSet->Add(pkRequirement);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshRequirements::AddRequirement(NiUInt32 uiSet,
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex,
    NiMeshRequirements::StreamBehavior eStreamBehavior,
    NiUInt32 uiStreamID, NiDataStream::Usage eUsage, NiUInt8 uiAccess,
    NiDataStreamElement::Format eFormat)
{
    NIASSERT(uiSet < GetSetCount());
    RequirementSet* pkSet = GetSetAt(uiSet);
    NIASSERT(pkSet);
    if (!pkSet)
        return UINT_MAX;

    SemanticRequirement* pkReq = NULL;
    pkReq = NiNew SemanticRequirement(
        kSemantic, uiSemanticIndex, eStreamBehavior, uiStreamID, eUsage,
        uiAccess, eFormat);

    return pkSet->Add(pkReq);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshRequirements::GetRequirementCount(NiUInt32 uiSet) const
{
    NIASSERT(uiSet < GetSetCount());
    const RequirementSet* pkSet = GetSetAt(uiSet);

    if (!pkSet)
        return 0;

    return pkSet->GetSize();
}
//---------------------------------------------------------------------------
inline NiMeshRequirements::SemanticRequirement*
    NiMeshRequirements::GetRequirement(NiUInt32 uiSet, NiUInt32 uiIndex) const
{
    NIASSERT(uiSet < GetSetCount());
    const RequirementSet* pkSet = GetSetAt(uiSet);
    
    if (uiIndex >= pkSet->GetSize())
        return NULL;

    return pkSet->GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiMeshRequirements::RequirementSet* NiMeshRequirements::GetSetAt(
    NiUInt32 uiIndex) const
{
    if (uiIndex >= m_kRequirementSets.GetSize())
        return NULL;

    return m_kRequirementSets.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiMeshRequirements::AddRequirementSet(RequirementSet* pkReqSet)
{
    NIASSERT(pkReqSet);
    m_kRequirementSets.Add(pkReqSet);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshRequirements::CreateNewRequirementSet()
{
    RequirementSet* pkNewSet = NiNew RequirementSet;
    return m_kRequirementSets.AddFirstEmpty(pkNewSet);
}
//---------------------------------------------------------------------------
