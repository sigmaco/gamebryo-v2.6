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

#include "NiBool.h"
#include "NiZBufferProperty.h"

NiImplementRTTI(NiZBufferProperty, NiProperty);

NiZBufferPropertyPtr NiZBufferProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiZBufferProperty);
//---------------------------------------------------------------------------
void NiZBufferProperty::CopyMembers(NiZBufferProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiZBufferProperty);

//---------------------------------------------------------------------------
void NiZBufferProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 2))
    {
        // In 20.1.0.2 and later, this value is located in the flags. 
        TestFunction eTest;
        NiStreamLoadEnum(kStream, eTest);
        SetTestFunction(eTest);
    }
}
//---------------------------------------------------------------------------
void NiZBufferProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiZBufferProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiZBufferProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiZBufferProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiZBufferProperty* pkZBuffer = (NiZBufferProperty*) pkObject;

    if (GetZBufferTest() != pkZBuffer->GetZBufferTest() ||
        GetZBufferWrite() != pkZBuffer->GetZBufferWrite() ||
        GetTestFunction() != pkZBuffer->GetTestFunction())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiZBufferProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiZBufferProperty::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bZTest", GetZBufferTest()));
    pkStrings->Add(NiGetViewerString("m_bZWrite", GetZBufferWrite()));

    const char* apcTests[TEST_MAX_MODES] = 
    {
        "TEST_ALWAYS",
        "TEST_LESS",
        "TEST_EQUAL",
        "TEST_LESSEQUAL",
        "TEST_GREATER",
        "TEST_NOTEQUAL",
        "TEST_GREATEREQUAL",
        "TEST_NEVER",
    };

    pkStrings->Add(NiGetViewerString("Test Function", 
        apcTests[(unsigned int)GetTestFunction()]));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiZBufferProperty::_SDMInit()
{
    ms_spDefault = NiNew NiZBufferProperty;
}
//---------------------------------------------------------------------------
    
void NiZBufferProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
