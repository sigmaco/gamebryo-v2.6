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

#include "NiPalette.h"
#include "NiRenderer.h"

NiImplementRTTI(NiPalette, NiObject);

NiPalette* NiPalette::ms_pkHead = 0;
NiPalette* NiPalette::ms_pkTail = 0;
NiCriticalSection NiPalette::ms_kPaletteListCriticalSection;

//---------------------------------------------------------------------------
NiPalette::NiPalette()
{
    m_bAlpha = false;
    m_uiEntries = 0;    
    m_pkPalette = NULL;

    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;
    
    m_pkRendererData = 0;

    AddPaletteToList();
}
//---------------------------------------------------------------------------
NiPalette::NiPalette(const NiPalette& kPal)
: NiObject()
{
    m_bAlpha = kPal.m_bAlpha;
    m_uiEntries = kPal.m_uiEntries;

    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;

    m_pkPalette = NiNew PaletteEntry[m_uiEntries];
    unsigned int uiDestSize = m_uiEntries * sizeof(PaletteEntry);
    NiMemcpy(m_pkPalette, uiDestSize, kPal.m_pkPalette, kPal.m_uiEntries *
        sizeof(PaletteEntry));

    m_pkRendererData = 0;
    CreateRendererData();

    AddPaletteToList();
}
//---------------------------------------------------------------------------
NiPalette::NiPalette(bool bHasAlpha, unsigned int uiEntries)
{
    m_bAlpha = bHasAlpha;
    m_uiEntries = uiEntries;
    m_pkPalette = NiNew PaletteEntry[m_uiEntries];
    // Copy the given palette entries into the object.

    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;
    
    m_pkRendererData = 0;
    CreateRendererData();

    AddPaletteToList();
}
//---------------------------------------------------------------------------
NiPalette::NiPalette(bool bHasAlpha, unsigned int uiEntries,
    const PaletteEntry* pkEntries)
{
    // RevID's of 0 are reserved for use in the renderers, so we must 
    // initialize the RevID to 1
    m_uiRevID = 1;

    m_bAlpha = bHasAlpha;
    m_uiEntries = uiEntries;

    m_pkPalette = NiNew PaletteEntry[m_uiEntries];
    // Copy the given palette entries into the object.
    unsigned int uiDestSize = m_uiEntries * sizeof(PaletteEntry);
    NiMemcpy(m_pkPalette, pkEntries, uiDestSize);
    
    m_pkRendererData = 0;
    CreateRendererData();

    AddPaletteToList();
}
//---------------------------------------------------------------------------
NiPalette::~NiPalette()
{
    NiDelete[] m_pkPalette;

    NiDelete m_pkRendererData;

    RemovePaletteFromList();
}
//---------------------------------------------------------------------------
void NiPalette::CreateRendererData() 
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer)
        pkRenderer->CreatePaletteRendererData(this);
}
//---------------------------------------------------------------------------
NiPalette& NiPalette::operator=(const NiPalette& kPal)
{
    if (this == &kPal)
        return *this;

    // DO NOT copy the renderer-specific data
    if (m_pkRendererData)
    {
        NiDelete m_pkRendererData;
        m_pkRendererData = 0;
    }
    
    m_bAlpha = kPal.m_bAlpha;
    m_uiRevID = 1;

    if (m_uiEntries != kPal.m_uiEntries)
    {
        NiDelete [] m_pkPalette;

        m_uiEntries = kPal.m_uiEntries;

        m_pkPalette = NiNew PaletteEntry[m_uiEntries];
    }

    unsigned int uiDestSize = m_uiEntries * sizeof(PaletteEntry);

    NiMemcpy(m_pkPalette, uiDestSize, kPal.m_pkPalette, kPal.m_uiEntries *
        sizeof(PaletteEntry));

    return *this;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPalette);
//---------------------------------------------------------------------------
void NiPalette::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiBool bAlpha;
    NiStreamLoadBinary(kStream, bAlpha);
    m_bAlpha = (bAlpha != 0);

    // for future expansion - the number of palette entries
    unsigned int uiEntries;
    NiStreamLoadBinary(kStream, uiEntries);

    if (uiEntries != m_uiEntries)
    {
        NiDelete [] m_pkPalette;
        m_uiEntries = uiEntries;
        m_pkPalette = NiNew PaletteEntry[m_uiEntries];
    }

    NiStreamLoadBinary(kStream, (unsigned char*)m_pkPalette,
        m_uiEntries * sizeof(PaletteEntry));
 
    // Create renderer data only after palette data has been loaded
    CreateRendererData();
}
//---------------------------------------------------------------------------
void NiPalette::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPalette::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPalette::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, (NiBool) m_bAlpha);

    NiStreamSaveBinary(kStream, m_uiEntries);
 
    NiStreamSaveBinary(kStream, (unsigned char*)m_pkPalette,
        m_uiEntries * sizeof(PaletteEntry));
}
//---------------------------------------------------------------------------
bool NiPalette::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiPalette* pkPalette = (NiPalette*)pkObject;

    if (m_bAlpha != pkPalette->m_bAlpha)
        return false;

    if (m_uiEntries != pkPalette->m_uiEntries)
        return false;

    if (memcmp(m_pkPalette, pkPalette->m_pkPalette, 
        m_uiEntries * sizeof(PaletteEntry) != 0))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPalette::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPalette::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
