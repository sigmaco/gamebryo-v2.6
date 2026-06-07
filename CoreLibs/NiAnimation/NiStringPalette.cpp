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
#include "NiAnimationPCH.h"

#include <NiStream.h>

#include "NiStringPalette.h"

NiImplementRTTI(NiStringPalette, NiObject);

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiStringPalette);
//---------------------------------------------------------------------------
bool NiStringPalette::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiStringPalette::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    // Since string palettes are rarely changed after being loaded, stream
    // out the end of buffer as the entire allocated size.
    unsigned int uiStreamedSize = m_uiEndOfBuffer;
    NiStreamSaveBinary(kStream, uiStreamedSize);
    if (uiStreamedSize > 0)
    {
        NIASSERT(m_pcBuffer);
        NiStreamSaveBinary(kStream, m_pcBuffer, uiStreamedSize);
    }
    NiStreamSaveBinary(kStream, m_uiEndOfBuffer);
}
//---------------------------------------------------------------------------
void NiStringPalette::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiAllocatedSize);
    if (m_uiAllocatedSize > 0)
    {
        m_pcBuffer = NiAlloc(char, m_uiAllocatedSize);
        NiStreamLoadBinary(kStream, m_pcBuffer, m_uiAllocatedSize);
    }
    NiStreamLoadBinary(kStream, m_uiEndOfBuffer);
}
//---------------------------------------------------------------------------
void NiStringPalette::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiStringPalette::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiStringPalette* pkDest = (NiStringPalette*) pkObject;

    if (pkDest->m_uiAllocatedSize != m_uiAllocatedSize ||
        pkDest->m_uiEndOfBuffer != m_uiEndOfBuffer)
    {
        return false;
    }

    if (m_uiAllocatedSize > 0)
    {
        if (memcmp(pkDest->m_pcBuffer, m_pcBuffer, m_uiAllocatedSize *
            sizeof(char)) != 0)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
