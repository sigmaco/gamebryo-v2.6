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

#include "NiCodeBlock.h"
#include "NiTSet.h"

//---------------------------------------------------------------------------
NiCodeBlock::NiCodeBlock()
{
}
//---------------------------------------------------------------------------
NiCodeBlock::~NiCodeBlock()
{
}
//---------------------------------------------------------------------------
const NiFixedString& NiCodeBlock::GetLanguage() const
{
    return m_kLanguage;
}
//---------------------------------------------------------------------------
void NiCodeBlock::SetLanguage(const NiFixedString& kStr)
{
    m_kLanguage = kStr;
}
//---------------------------------------------------------------------------
const NiFixedString& NiCodeBlock::GetPlatform() const
{
    return m_kPlatform;
}
//---------------------------------------------------------------------------
void NiCodeBlock::SetPlatform(const NiFixedString& kStr)
{
    m_kPlatform = kStr;
}
//---------------------------------------------------------------------------
const NiFixedString& NiCodeBlock::GetTarget() const
{
    return m_kTarget;
}
//---------------------------------------------------------------------------
void NiCodeBlock::SetTarget(const NiFixedString& kStr)
{
    m_kTarget = kStr;
}
//---------------------------------------------------------------------------
const NiString& NiCodeBlock::GetText()  const
{
    return m_kText;
}
//---------------------------------------------------------------------------
void NiCodeBlock::SetText(const NiString&  kText)
{
    m_kText = kText;
}
//---------------------------------------------------------------------------
