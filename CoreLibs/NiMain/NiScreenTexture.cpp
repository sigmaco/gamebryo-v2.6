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

#include "NiScreenTexture.h"

NiImplementRTTI(NiScreenTexture, NiObject);

// construction and destruction
//---------------------------------------------------------------------------
NiScreenTexture::NiScreenTexture()
{
}
//---------------------------------------------------------------------------
NiScreenTexture::~NiScreenTexture()
{
}
//---------------------------------------------------------------------------
const NiScreenTexture::ScreenRect& NiScreenTexture::GetScreenRect(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kScreenRects.GetSize());

    return m_kScreenRects.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiScreenTexture::ScreenRect& NiScreenTexture::GetScreenRect(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kScreenRects.GetSize());

    return m_kScreenRects.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NiScreenTexture::GetNumScreenRects() const
{
    return m_kScreenRects.GetSize();
}
//---------------------------------------------------------------------------
NiTexturingProperty* NiScreenTexture::GetTexturingProperty() const
{
    return m_spTexProp;
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenTexture);
//---------------------------------------------------------------------------
void NiScreenTexture::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    unsigned int uiNumRects;
    NiStreamLoadBinary(kStream, uiNumRects);

    for (unsigned int i = 0; i < uiNumRects; i++)
    {
        ScreenRect kRect;
        NiStreamLoadBinary(kStream, kRect.m_sPixTop);
        NiStreamLoadBinary(kStream, kRect.m_sPixLeft);
        NiStreamLoadBinary(kStream, kRect.m_usPixWidth);
        NiStreamLoadBinary(kStream, kRect.m_usPixHeight);
        NiStreamLoadBinary(kStream, kRect.m_usTexTop);
        NiStreamLoadBinary(kStream, kRect.m_usTexLeft);
        kRect.m_kColor.LoadBinary(kStream);

        m_kScreenRects.Add(kRect);
    }

    kStream.ReadLinkID();   // m_spTexProp
}
//---------------------------------------------------------------------------
void NiScreenTexture::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_spTexProp = (NiTexturingProperty*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiScreenTexture::RegisterStreamables(NiStream& kStream)
{
    if(!NiObject::RegisterStreamables(kStream))
        return false;

    m_spTexProp->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiScreenTexture::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    unsigned int uiNumRects = m_kScreenRects.GetSize();
    NiStreamSaveBinary(kStream, uiNumRects);

    for (unsigned int i = 0; i < uiNumRects; i++)
    {
        const ScreenRect& kRect = m_kScreenRects.GetAt(i);
        NiStreamSaveBinary(kStream, kRect.m_sPixTop);
        NiStreamSaveBinary(kStream, kRect.m_sPixLeft);
        NiStreamSaveBinary(kStream, kRect.m_usPixWidth);
        NiStreamSaveBinary(kStream, kRect.m_usPixHeight);
        NiStreamSaveBinary(kStream, kRect.m_usTexTop);
        NiStreamSaveBinary(kStream, kRect.m_usTexLeft);
        NiColorA kColor = kRect.m_kColor;
        kColor.SaveBinary(kStream);
    }

    kStream.SaveLinkID(m_spTexProp);
}
//---------------------------------------------------------------------------
bool NiScreenTexture::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiScreenTexture* pkTexture = (NiScreenTexture*) pkObject;

    unsigned int uiNumRects = m_kScreenRects.GetSize();
    if (uiNumRects != pkTexture->m_kScreenRects.GetSize())
        return false;

    for (unsigned int i = 0; i < uiNumRects; i++)
    {
        const ScreenRect& kRect = m_kScreenRects.GetAt(i);
        const ScreenRect& kOtherRect = pkTexture->m_kScreenRects.GetAt(i);

        if (kRect.m_sPixTop != kOtherRect.m_sPixTop &&
            kRect.m_sPixLeft != kOtherRect.m_sPixLeft &&
            kRect.m_usPixWidth != kOtherRect.m_usPixWidth &&
            kRect.m_usPixHeight != kOtherRect.m_usPixHeight &&
            kRect.m_usTexTop != kOtherRect.m_usTexTop &&
            kRect.m_usTexLeft != kOtherRect.m_usTexLeft &&
            kRect.m_kColor != kOtherRect.m_kColor)
        {
            return false;
        }
    }

    if (!m_spTexProp->IsEqual(pkTexture->m_spTexProp))
        return false;

    return true;
}
//---------------------------------------------------------------------------
