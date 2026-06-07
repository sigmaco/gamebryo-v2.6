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
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10StateGroup.h"

#if defined(NIDEBUG)
#include "NSBD3D10Utility.h"
#endif  //#if defined(NIDEBUG)

#include <NiD3D10RenderStateGroup.h>
#include <NiD3D10ShaderFactory.h>

//---------------------------------------------------------------------------
void NSBD3D10StateGroup::NSBD3D10SGEntry::SetAttribute(
    const char* pcAttribute)
{
    NSBD3D10Utility::SetString(m_pcAttribute, 0, pcAttribute);
}
//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::NSBD3D10SGEntry::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiFlags);
    NiStreamSaveBinary(kStream, m_uiState);
    NiStreamSaveBinary(kStream, m_uiValue);
    kStream.WriteCString(m_pcAttribute);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::NSBD3D10SGEntry::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiState);
    NiStreamLoadBinary(kStream, m_uiValue);
    m_pcAttribute = kStream.ReadCString();

    return true;
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup::NSBD3D10StateGroup()
{
    m_kStateList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup::~NSBD3D10StateGroup()
{
    // Clean up the list
    NSBD3D10SGEntry* pkEntry;

    m_kStateIter = m_kStateList.GetHeadPos();
    while (m_kStateIter)
    {
        pkEntry = m_kStateList.GetNext(m_kStateIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kStateList.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBD3D10StateGroup::SetState(unsigned int uiState, unsigned int uiValue,
    bool bSave, bool bUseMapValue)
{
    NSBD3D10SGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        NSBD3D10Utility::Log("Warning: NSBD3D10StateGroup::SetState>  "
            "Overwriting state 0x%08x - original value 0x%08x - NiNew value "
            "0x%08x\n", uiState, pkEntry->GetValue(), uiValue);

        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* WARNING: NSBD3D10StateGroup::SetState\n"
            "    Overwriting State 0x%08x\n"
            "    Original Value = 0x%08x\n"
            "    New Value      = 0x%08x\n", 
            uiState, pkEntry->GetValue(), uiValue);

        pkEntry->SetValue(uiValue);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(false);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBD3D10SGEntry();
    NIASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetValue(uiValue);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(false);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}
//---------------------------------------------------------------------------
void NSBD3D10StateGroup::SetState(unsigned int uiState,
    const char* pcAttribute, bool bSave, bool bUseMapValue)
{
    NSBD3D10SGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        // We may want to generate a warning about this.
        pkEntry->SetAttribute(pcAttribute);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(true);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBD3D10SGEntry();
    NIASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetAttribute(pcAttribute);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(true);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10StateGroup::GetStateCount()
{
    return m_kStateList.GetSize();
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup::NSBD3D10SGEntry* NSBD3D10StateGroup::GetFirstState()
{
    NSBD3D10SGEntry* pkEntry = 0;

    m_kStateIter = m_kStateList.GetHeadPos();
    if (m_kStateIter)
        pkEntry = m_kStateList.GetNext(m_kStateIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup::NSBD3D10SGEntry* NSBD3D10StateGroup::GetNextState()
{
    NSBD3D10SGEntry* pkEntry = 0;

    if (m_kStateIter)
        pkEntry = m_kStateList.GetNext(m_kStateIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::SetupRenderStateGroup(
    NiD3D10RenderStateGroup& kRSGroup)
{
    return SetupRenderStateGroup_D3D10(kRSGroup);
}
//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::SetupTextureSamplerGroup(
    NiD3D10RenderStateGroup& kRSGroup, unsigned int uiStage)
{
    return SetupTextureSamplerGroup_D3D10(kRSGroup, uiStage);
}
//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::SaveBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kStateList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);
    
    unsigned int uiTestCount = 0;

    NSBD3D10SGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextState();
    }

    if (uiTestCount != uiCount)
        return false;
    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBD3D10SGEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBD3D10SGEntry();
        NIASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kStateList.AddTail(pkEntry);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup::NSBD3D10SGEntry* NSBD3D10StateGroup::FindStateInList(
    unsigned int uiState)
{
    NSBD3D10SGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (pkEntry->GetState() == uiState)
            return pkEntry;

        pkEntry = GetNextState();
    }

    return 0;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10StateGroup::Dump(FILE* pf, DumpMode eMode)
{
    NSBD3D10Utility::Dump(pf, true, "Entry Count = %d\n", GetStateCount());
    
    NSBD3D10Utility::IndentInsert();

    NSBD3D10SGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (eMode == DUMP_RENDERSTATES)
            DumpEntryAsRenderState(pf, pkEntry);
        else if (eMode == DUMP_SAMPLERSTATES)
            DumpEntryAsSamplerState(pf, pkEntry);
        else
            DumpEntryAsUnknown(pf, pkEntry);

        pkEntry = GetNextState();
    }

    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
void NSBD3D10StateGroup::DumpEntryAsUnknown(FILE* pf,
    NSBD3D10SGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    NSBD3D10Utility::Dump(pf, true, "0x%08x = 0x%08x %s\n",
        pkEntry->GetState(), pkEntry->GetValue(),
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBD3D10StateGroup::DumpEntryAsRenderState(FILE* pf,
    NSBD3D10SGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = NSBD3D10RenderStates::LookupRenderStateString(
        (NSBD3D10RenderStates::NiD3D10RenderState)pkEntry->GetState());

    NSBD3D10Utility::Dump(pf, true, "%32s = 0x%08x %s\n", pcState, 
        pkEntry->GetValue(), pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBD3D10StateGroup::DumpEntryAsSamplerState(FILE* pf,
    NSBD3D10SGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = 
        NSBD3D10StageAndSamplerStates::LookupTextureSamplerString(
        (NSBD3D10StageAndSamplerStates::NiD3D10TextureSamplerState)
            pkEntry->GetState());
    const char* pcValue = 
        NSBD3D10StageAndSamplerStates::LookupTextureSamplerValueString(
            (NSBD3D10StageAndSamplerStates::NiD3D10TextureSamplerState)
            pkEntry->GetState(), pkEntry->GetValue());
            
    NSBD3D10Utility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue, 
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
