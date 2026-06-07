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

#include "NiAlphaProperty.h"
#include "NiBool.h"

NiImplementRTTI(NiAlphaProperty,NiProperty);

NiAlphaPropertyPtr NiAlphaProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiAlphaProperty);
//---------------------------------------------------------------------------
void NiAlphaProperty::CopyMembers(NiAlphaProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;

    pkDest->m_ucAlphaTestRef = m_ucAlphaTestRef;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiAlphaProperty);
//---------------------------------------------------------------------------
void NiAlphaProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadBinary(kStream, m_ucAlphaTestRef);
}
//---------------------------------------------------------------------------
void NiAlphaProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiAlphaProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiAlphaProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    NiStreamSaveBinary(kStream, m_ucAlphaTestRef);
}
//---------------------------------------------------------------------------
bool NiAlphaProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiAlphaProperty* pkAlpha = (NiAlphaProperty*) pkObject;

    if (GetAlphaBlending() != pkAlpha->GetAlphaBlending() ||
        GetSrcBlendMode() != pkAlpha->GetSrcBlendMode() ||
        GetDestBlendMode() != pkAlpha->GetDestBlendMode() || 
        GetAlphaTesting() != pkAlpha->GetAlphaTesting() ||
        GetTestMode() != pkAlpha->GetTestMode() ||
        GetTestRef() != pkAlpha->GetTestRef() ||
        GetNoSorter() != pkAlpha->GetNoSorter())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiAlphaProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiAlphaProperty::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bAlpha",  GetAlphaBlending()));
    pkStrings->Add(GetViewerString("m_eSrcBlend", GetSrcBlendMode()));
    pkStrings->Add(GetViewerString("m_eDestBlend", GetDestBlendMode()));
    pkStrings->Add(NiGetViewerString("m_bAlphaTest", GetAlphaTesting()));
    pkStrings->Add(GetViewerString("m_eTestMode", GetTestMode()));
    pkStrings->Add(NiGetViewerString("m_ucTestRef", GetTestRef()));
    pkStrings->Add(NiGetViewerString("m_bNoSorter", GetNoSorter()));
}
//---------------------------------------------------------------------------
char* NiAlphaProperty::GetViewerString(const char* pcPrefix,
    AlphaFunction eFunc)
{
    size_t stLen = strlen(pcPrefix) + 22;
    char* pcString = NiAlloc(char, stLen);

    switch (eFunc)
    {
    case ALPHA_ONE:
        NiSprintf(pcString, stLen, "%s = ALPHA_ONE", pcPrefix);
        break;
    case ALPHA_ZERO:
        NiSprintf(pcString, stLen, "%s = ALPHA_ZERO", pcPrefix);
        break;
    case ALPHA_SRCCOLOR:
        NiSprintf(pcString, stLen, "%s = ALPHA_SRCCOLOR", pcPrefix);
        break;
    case ALPHA_INVSRCCOLOR:
        NiSprintf(pcString, stLen, "%s = ALPHA_INVSRCCOLOR", pcPrefix);
        break;
    case ALPHA_DESTCOLOR:
        NiSprintf(pcString, stLen, "%s = ALPHA_DESTCOLOR", pcPrefix);
        break;
    case ALPHA_INVDESTCOLOR:
        NiSprintf(pcString, stLen, "%s = ALPHA_INVDESTCOLOR", pcPrefix);
        break;
    case ALPHA_SRCALPHA:
        NiSprintf(pcString, stLen, "%s = ALPHA_SRCALPHA", pcPrefix);
        break;
    case ALPHA_INVSRCALPHA:
        NiSprintf(pcString, stLen, "%s = ALPHA_INVSRCALPHA", pcPrefix);
        break;
    case ALPHA_DESTALPHA:
        NiSprintf(pcString, stLen, "%s = ALPHA_DESTALPHA", pcPrefix);
        break;
    case ALPHA_INVDESTALPHA:
        NiSprintf(pcString, stLen, "%s = ALPHA_INVDESTALPHA", pcPrefix);
        break;
    case ALPHA_SRCALPHASAT:
        NiSprintf(pcString, stLen, "%s = ALPHA_SRCALPHASAT", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;

    }

    return pcString;
}
//---------------------------------------------------------------------------
char* NiAlphaProperty::GetViewerString(const char* pcPrefix,
    TestFunction eFunc)
{
    size_t stLen = strlen(pcPrefix) + 22;
    char* pcString = NiAlloc(char, stLen);

    switch (eFunc)
    {
    case TEST_ALWAYS:
        NiSprintf(pcString, stLen, "%s = TEST_ALWAYS", pcPrefix);
        break;
    case TEST_LESS:
        NiSprintf(pcString, stLen, "%s = TEST_LESS", pcPrefix);
        break;
    case TEST_EQUAL:
        NiSprintf(pcString, stLen, "%s = TEST_EQUAL", pcPrefix);
        break;
    case TEST_LESSEQUAL:
        NiSprintf(pcString, stLen, "%s = TEST_LESSEQUAL", pcPrefix);
        break;
    case TEST_GREATER:
        NiSprintf(pcString, stLen, "%s = TEST_GREATER", pcPrefix);
        break;
    case TEST_NOTEQUAL:
        NiSprintf(pcString, stLen, "%s = TEST_NOTEQUAL", pcPrefix);
        break;
    case TEST_GREATEREQUAL:
        NiSprintf(pcString, stLen, "%s = TEST_GREATEREQUAL", pcPrefix);
        break;
    case TEST_NEVER:
        NiSprintf(pcString, stLen, "%s = TEST_NEVER", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiAlphaProperty::_SDMInit()
{
    ms_spDefault = NiNew NiAlphaProperty;
}
//---------------------------------------------------------------------------
    
void NiAlphaProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
