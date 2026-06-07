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

#include "NiShaderDescBase.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiShaderDescBase::NiShaderDescBase() :
    m_pcName(0), 
    m_pcDesc(0)
{
}
//---------------------------------------------------------------------------
NiShaderDescBase::~NiShaderDescBase()
{
    NiFree(m_pcName);
    NiFree(m_pcDesc);
}
//---------------------------------------------------------------------------
const char* NiShaderDescBase::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
const char* NiShaderDescBase::GetDescription() const
{
    return m_pcDesc;
}
//---------------------------------------------------------------------------
void NiShaderDescBase::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = 0;

    if (pcName && pcName[0] != '\0')
    {
        size_t stLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char,stLen);
        NIASSERT(m_pcName);
        NiStrcpy(m_pcName, stLen, pcName);
    }
}
//---------------------------------------------------------------------------
void NiShaderDescBase::SetDescription(const char* pcDesc)
{
    NiFree(m_pcDesc);
    m_pcDesc = 0;

    if (pcDesc && pcDesc[0] != '\0')
    {
        size_t stLen = strlen(pcDesc) + 1;
        m_pcDesc = NiAlloc(char,stLen);
        NIASSERT(m_pcDesc);
        NiStrcpy(m_pcDesc, stLen, pcDesc);
    }
}
//---------------------------------------------------------------------------
