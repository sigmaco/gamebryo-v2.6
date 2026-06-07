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
#include "NiBinaryShaderLibPCH.h"

#include <NiShaderFactory.h>

#include "NSBRenderStates.h"
#include "NSBStateGroup.h"
#include "NSBUtility.h"

#if defined(NIDEBUG)
#include "NSBStageAndSamplerStates.h"
#endif  //#if defined(NIDEBUG)

//---------------------------------------------------------------------------
void NSBStateGroup::NSBSGEntry::SetAttribute(const char* pcAttribute)
{
    NiRendererUtility::SetString(m_pcAttribute, 0, pcAttribute);
}
//---------------------------------------------------------------------------
bool NSBStateGroup::NSBSGEntry::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiFlags);
    NiStreamSaveBinary(kStream, m_uiState);
    NiStreamSaveBinary(kStream, m_uiValue);
    kStream.WriteCString(m_pcAttribute);

    return true;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::NSBSGEntry::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiState);
    NiStreamLoadBinary(kStream, m_uiValue);
    m_pcAttribute = kStream.ReadCString();

    return true;
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBStateGroup()
{
    m_kStateList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBStateGroup::~NSBStateGroup()
{
    // Clean up the list
    NSBSGEntry* pkEntry;

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
void NSBStateGroup::SetState(unsigned int uiState, unsigned int uiValue, 
    bool bSave, bool bUseMapValue)
{
    NSBSGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        NILOG(NIMESSAGE_GENERAL_1, 
            "Warning: NSBStateGroup::SetState> Overwriting "
            " State 0x%08x - original value 0x%08x - NiNew value 0x%08x\n",
            uiState, pkEntry->GetValue(), uiValue);

        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* WARNING: NSBStateGroup::SetState\n"
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
    pkEntry = NiNew NSBSGEntry();
    NIASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetValue(uiValue);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(false);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}
//---------------------------------------------------------------------------
void NSBStateGroup::SetState(unsigned int uiState, const char* pcAttribute, 
    bool bSave, bool bUseMapValue)
{
    NSBSGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        // We may want to generate a warning about this.
        pkEntry->SetAttribute(pcAttribute);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(true);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBSGEntry();
    NIASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetAttribute(pcAttribute);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(true);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}
//---------------------------------------------------------------------------
unsigned int NSBStateGroup::GetStateCount()
{
    return m_kStateList.GetSize();
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::GetFirstState()
{
    NSBSGEntry* pkEntry = 0;

    m_kStateIter = m_kStateList.GetHeadPos();
    if (m_kStateIter)
        pkEntry = m_kStateList.GetNext(m_kStateIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::GetNextState()
{
    NSBSGEntry* pkEntry = 0;

    if (m_kStateIter)
        pkEntry = m_kStateList.GetNext(m_kStateIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::SaveBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kStateList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);
    
    unsigned int uiTestCount = 0;

    NSBSGEntry* pkEntry = GetFirstState();
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
bool NSBStateGroup::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBSGEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBSGEntry();
        NIASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kStateList.AddTail(pkEntry);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::FindStateInList(
    unsigned int uiState)
{
    NSBSGEntry* pkEntry = GetFirstState();
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
void NSBStateGroup::Dump(FILE* pf, DumpMode eMode)
{
    NSBUtility::Dump(pf, true, "Entry Count = %d\n", GetStateCount());
    
    NSBUtility::IndentInsert();

    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (eMode == DUMP_RENDERSTATES)
            DumpEntryAsRenderState(pf, pkEntry);
        else
        if (eMode == DUMP_STAGESTATES)
            DumpEntryAsStageState(pf, pkEntry);
        else
        if (eMode == DUMP_SAMPLERSTATES)
            DumpEntryAsSamplerState(pf, pkEntry);
        else
            DumpEntryAsUnknown(pf, pkEntry);

        pkEntry = GetNextState();
    }

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsUnknown(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    NSBUtility::Dump(pf, true, "0x%08x = 0x%08x %s\n",
        pkEntry->GetState(), pkEntry->GetValue(),
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsRenderState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = NSBRenderStates::LookupRenderStateString(
        (NSBRenderStates::NiD3DRenderState)pkEntry->GetState());
    NSBUtility::Dump(pf, true, "%32s = 0x%08x %s\n", pcState, 
        pkEntry->GetValue(), pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsStageState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = 
        NSBStageAndSamplerStates::LookupTextureStageString(
            (NSBStageAndSamplerStates::NiD3DTextureStageState)
            pkEntry->GetState());
    const char* pcValue = 
        NSBStageAndSamplerStates::LookupTextureStageValueString(
            (NSBStageAndSamplerStates::NiD3DTextureStageState)pkEntry->
            GetState(), pkEntry->GetValue());
            
    NSBUtility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue, 
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsSamplerState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = 
        NSBStageAndSamplerStates::LookupTextureSamplerString(
        (NSBStageAndSamplerStates::NiD3DTextureSamplerState)
            pkEntry->GetState());
    const char* pcValue = 
        NSBStageAndSamplerStates::LookupTextureSamplerValueString(
            (NSBStageAndSamplerStates::NiD3DTextureSamplerState)pkEntry->
            GetState(), pkEntry->GetValue());
            
    NSBUtility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue, 
        pkEntry->IsSaved() ? "SAVED" : "");
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
