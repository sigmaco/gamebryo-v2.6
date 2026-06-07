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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiVertexColorProperty.h"

NiImplementRTTI(NiVertexColorProperty,NiProperty);

NiVertexColorPropertyPtr NiVertexColorProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiVertexColorProperty);
//---------------------------------------------------------------------------
void NiVertexColorProperty::CopyMembers(NiVertexColorProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiVertexColorProperty);

//---------------------------------------------------------------------------
void NiVertexColorProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 2))
    {
        // In 20.1.0.2 and later, these values are located in the flags. 
        SourceVertexMode eSource;
        NiStreamLoadEnum(kStream, eSource);
        SetSourceMode(eSource);

        LightingMode eLighting;
        NiStreamLoadEnum(kStream, eLighting);
        SetLightingMode(eLighting);
    }
}
//---------------------------------------------------------------------------
void NiVertexColorProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiVertexColorProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiVertexColorProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiVertexColorProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiVertexColorProperty* pkVC = (NiVertexColorProperty*) pkObject;

    if (GetSourceMode() != pkVC->GetSourceMode() ||
        GetLightingMode() != pkVC->GetLightingMode())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiVertexColorProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiVertexColorProperty::ms_RTTI.GetName()));

    pkStrings->Add(GetViewerString("Source Mode", GetSourceMode()));
    pkStrings->Add(GetViewerString("Lighting Mode", GetLightingMode()));
}
//---------------------------------------------------------------------------
char* NiVertexColorProperty::GetViewerString(const char* pcPrefix,
    SourceVertexMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 26;
    char* pcString = NiAlloc(char,stLen);

    switch (eMode)
    {
    case SOURCE_IGNORE:
        NiSprintf(pcString, stLen, "%s = SOURCE_IGNORE", pcPrefix);
        break;
    case SOURCE_EMISSIVE:
        NiSprintf(pcString, stLen, "%s = SOURCE_EMISSIVE", pcPrefix);
        break;
    case SOURCE_AMB_DIFF:
        NiSprintf(pcString, stLen, "%s = SOURCE_AMB_DIFF", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------
char* NiVertexColorProperty::GetViewerString(const char* pcPrefix,
    LightingMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 26;
    char* pcString = NiAlloc(char,stLen);

    switch (eMode)
    {
    case LIGHTING_E:
        NiSprintf(pcString, stLen, "%s = LIGHTING_E", pcPrefix);
        break;
    case LIGHTING_E_A_D:
        NiSprintf(pcString, stLen, "%s = LIGHTING_E_A_D", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiVertexColorProperty::_SDMInit()
{
    ms_spDefault = NiNew NiVertexColorProperty;
}
//---------------------------------------------------------------------------
    
void NiVertexColorProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
