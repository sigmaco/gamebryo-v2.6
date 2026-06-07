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

#include "NiMeshPCH.h"

#include "NiRefObject.h"
#include "NiMeshRequirements.h"
#include "NiMeshErrorInterface.h"

//--------------------------------------------------------------------------
// SemanticRequirement
//--------------------------------------------------------------------------
NiMeshRequirements::SemanticRequirement::SemanticRequirement() :
    m_kSemantic(NULL),
    m_uiIndex(0),
    m_kUsage(NiDataStream::USAGE_MAX_TYPES),
    m_uiAccessFlags(0),
    m_eStreamBehavior(CAN_SPLIT),
    m_uiStreamID(0)
{
    // No work beyond member initialization
}
//---------------------------------------------------------------------------
NiMeshRequirements::SemanticRequirement::SemanticRequirement(
    const NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex, StreamBehavior eStreamBehavior,
    NiUInt32 uiStreamID, NiDataStream::Usage eUsage, NiUInt8 uiAccess,
    NiDataStreamElement::Format eFormat) :
    m_kSemantic(kSemantic),
    m_uiIndex(uiSemanticIndex),
    m_kUsage(eUsage),
    m_uiAccessFlags(uiAccess),
    m_eStreamBehavior(eStreamBehavior),
    m_uiStreamID(uiStreamID)
{
    m_kFormats.Add(eFormat);
}
//---------------------------------------------------------------------------
NiMeshRequirements::NiMeshRequirements(const char* pkRequirementName,
    NiSystemDesc::RendererID eRenderer) :
    m_eRenderer(eRenderer)
{
    m_kReqName = pkRequirementName;
}
//---------------------------------------------------------------------------
NiMeshRequirements::~NiMeshRequirements()
{
    for (NiUInt32 ui = 0; ui < m_kRequirementSets.GetSize(); ui++)
        NiDelete m_kRequirementSets.GetAt(ui);
}
//---------------------------------------------------------------------------
void NiMeshRequirements::ReleaseAllSemanticRequirements()
{
    NiUInt32 uiSets = GetSetCount();

    for(NiUInt32 uiSet=0; uiSet<uiSets; uiSet++)
        GetSetAt(uiSet)->RemoveAll();

    // The sets themselves still require deletion.
    for (NiUInt32 ui = 0; ui < m_kRequirementSets.GetSize(); ui++)
        NiDelete m_kRequirementSets.GetAt(ui);

    m_kRequirementSets.RemoveAll();
}
//---------------------------------------------------------------------------
NiMeshRequirements::SemanticRequirement& NiMeshRequirements::
    SemanticRequirement::operator= (const NiMeshRequirements::
    SemanticRequirement& r)
{
    m_kSemantic = r.m_kSemantic;
    m_uiIndex = r.m_uiIndex;
    m_kUsage = r.m_kUsage;
    m_uiAccessFlags = r.m_uiAccessFlags;

    m_kFormats.RemoveAll();
    m_kFormats.Realloc(r.m_kFormats.GetSize());

    for (NiUInt32 ui = 0; ui < r.m_kFormats.GetSize(); ui++)
        m_kFormats.Add(r.m_kFormats.GetAt(ui));

    m_uiStreamID = r.m_uiStreamID;
    m_eStreamBehavior = r.m_eStreamBehavior;

    return *this;
}
//---------------------------------------------------------------------------
NiMeshRequirements& NiMeshRequirements::operator= (const NiMeshRequirements& r)
{
    for (NiUInt32 ui = 0; ui < m_kRequirementSets.GetSize(); ui++)
        NiDelete m_kRequirementSets.GetAt(ui);
    m_kRequirementSets.RemoveAll();

    m_kRequirementSets.SetSize(r.m_kRequirementSets.GetSize());
    for (NiUInt32 ui = 0; ui < r.m_kRequirementSets.GetSize(); ui++)
    {
        RequirementSet* pkOldSet = r.m_kRequirementSets.GetAt(ui);
        RequirementSet* pkNewSet = NiNew RequirementSet();

        for(NiUInt32 uiSR = 0; uiSR < pkOldSet->GetSize(); uiSR++)
        {
            SemanticRequirement* pkNewSR = NiNew SemanticRequirement();
            SemanticRequirement* pkExistingSR = pkOldSet->GetAt(uiSR);
            *pkNewSR = *pkExistingSR;
            pkNewSet->Add(pkNewSR);
        }

        m_kRequirementSets.SetAt(ui, pkNewSet);
    }

    m_eRenderer = r.m_eRenderer;

    m_kReqName = r.m_kReqName;

    return *this;
}
//---------------------------------------------------------------------------
