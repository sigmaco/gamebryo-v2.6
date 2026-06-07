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

#include <NiRTLib.h>
#include <NiStream.h>
#include "NiTextKey.h"

//---------------------------------------------------------------------------
NiTextKey::NiTextKey(float fTime, const NiFixedString& kText)
{
    m_fTime = fTime;
    m_kText = kText;
}
//---------------------------------------------------------------------------
NiTextKey::~NiTextKey()
{
}
//---------------------------------------------------------------------------
void NiTextKey::SetText(const NiFixedString& kText)
{
    m_kText = kText;
}
//---------------------------------------------------------------------------
NiTextKey* NiTextKey::Copy()
{
    NiTextKey* pCopy = NiNew NiTextKey();
    pCopy->SetTime(m_fTime);
    pCopy->SetText(m_kText);
    return pCopy;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiTextKey::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,m_fTime);
    if (stream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        stream.LoadCStringAsFixedString(m_kText);
    }
    else
    {
        stream.LoadFixedString(m_kText);
    }
}
//---------------------------------------------------------------------------
void NiTextKey::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,m_fTime);
    stream.SaveFixedString(m_kText);
}
//---------------------------------------------------------------------------
void NiTextKey::RegisterStreamables(NiStream& stream)
{
    stream.RegisterFixedString(m_kText);
}
//---------------------------------------------------------------------------
