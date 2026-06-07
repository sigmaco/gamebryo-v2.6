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

#include "NiDefaultAVObjectPalette.h"
#include "NiCloningProcess.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
// Default implementation - NiDefaultAVObjectPalette
//---------------------------------------------------------------------------
NiImplementRTTI(NiDefaultAVObjectPalette, NiAVObjectPalette);

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiDefaultAVObjectPalette);
//---------------------------------------------------------------------------
void NiDefaultAVObjectPalette::CopyMembers(NiDefaultAVObjectPalette* pDest,
    NiCloningProcess& kCloning) 
{
    NiAVObjectPalette::CopyMembers(pDest, kCloning);
}
//---------------------------------------------------------------------------
void NiDefaultAVObjectPalette::ProcessClone(NiCloningProcess& kCloning)
{
    NiAVObjectPalette::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiDefaultAVObjectPalette* pkDest = (NiDefaultAVObjectPalette*) pkClone;

    if (m_pkScene)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkScene, pkClone);
        NIASSERT(bCloned);
        pkDest->m_pkScene = (NiAVObject*) pkClone;
    }

    if (!m_kHash.IsEmpty())
    {
        NiTMapIterator kIter = m_kHash.GetFirstPos();
        while (kIter != NULL)
        {
            const char* pcKey;
            NiAVObject* pkValue;
            m_kHash.GetNext(kIter, pcKey, pkValue);
            if (pcKey != NULL && pkValue != NULL)
            {
                NiObject* pkClonedValue;
                bCloned = kCloning.m_pkCloneMap->GetAt(pkValue, pkClonedValue);
                NIASSERT(bCloned);
                if (bCloned)
                    pkDest->SetAVObject(pcKey, (NiAVObject*)pkClonedValue);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiDefaultAVObjectPalette);
//---------------------------------------------------------------------------
void NiDefaultAVObjectPalette::LoadBinary(NiStream& stream)
{
    NiAVObjectPalette::LoadBinary(stream);

    m_pkScene = (NiAVObject*) stream.ResolveLinkID();

    unsigned int uiCount = 0;
    NiStreamLoadBinary(stream, uiCount);
    while (uiCount-- > 0)
    {
        char* pcString = NULL;
        stream.LoadCString(pcString);
        NiAVObject* pkObj = (NiAVObject*) stream.ResolveLinkID();

        if (pcString != NULL && pkObj != NULL)
        {
            SetAVObject(pcString, pkObj);
        }

        NiFree(pcString);
    }
}
//---------------------------------------------------------------------------
void NiDefaultAVObjectPalette::LinkObject(NiStream& stream)
{
    NiAVObjectPalette::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiDefaultAVObjectPalette::RegisterStreamables(NiStream& stream)
{
    return NiAVObjectPalette::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiDefaultAVObjectPalette::SaveBinary(NiStream& stream)
{
    NiAVObjectPalette::SaveBinary(stream);
    stream.SaveLinkID(m_pkScene);
    unsigned int uiCount = m_kHash.GetCount();
    NiStreamSaveBinary(stream, uiCount);
    if (!m_kHash.IsEmpty())
    {
        NiTMapIterator kIter = m_kHash.GetFirstPos();
        while (kIter != NULL)
        {
            const char* pcKey;
            NiAVObject* pkValue;
            m_kHash.GetNext(kIter, pcKey, pkValue);
            stream.SaveCString(pcKey);
            stream.SaveLinkID(pkValue);
        }
    }
}
//---------------------------------------------------------------------------
bool NiDefaultAVObjectPalette::IsEqual(NiObject* pObject)
{
    if (!NiAVObjectPalette::IsEqual(pObject))
        return false;

    NiDefaultAVObjectPalette* pkDest = (NiDefaultAVObjectPalette*) pObject;
    if (pkDest->m_kHash.GetCount() != m_kHash.GetCount())
        return false;

    if (!m_kHash.IsEmpty())
    {
        NiTMapIterator kIter = m_kHash.GetFirstPos();
        while (kIter != NULL)
        {
            const char* pcKey;
            NiAVObject* pkValue;
            m_kHash.GetNext(kIter, pcKey, pkValue);
            
            NiAVObject* pkDestValue = pkDest->GetAVObject(pcKey);
            if (pkDestValue == NULL)
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiDefaultAVObjectPalette::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiAVObjectPalette::GetViewerStrings(pStrings);

    pStrings->Add(
        NiGetViewerString(NiDefaultAVObjectPalette::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
