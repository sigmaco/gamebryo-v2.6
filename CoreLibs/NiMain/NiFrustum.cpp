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

#include "NiFrustum.h"

//---------------------------------------------------------------------------
NiFrustum::NiFrustum(bool bOrtho) : m_bOrtho(bOrtho)
{
}
//---------------------------------------------------------------------------
NiFrustum::NiFrustum(float fLeft, float fRight, float fTop, float fBottom,
    float fNear, float fFar, bool bOrtho)
    :
    m_fLeft(fLeft),
    m_fRight(fRight),
    m_fTop(fTop),
    m_fBottom(fBottom),
    m_fNear(fNear),
    m_fFar(fFar),
    m_bOrtho(bOrtho)
{
}
#ifndef __SPU__
#include <NiSystem.h>
//---------------------------------------------------------------------------
char* NiFrustum::GetViewerString(const char* pcPrefix) const
{
    size_t stLen = strlen(pcPrefix) + 80;
    char* pcString = NiAlloc(char, stLen);

    NiSprintf(pcString, stLen, "%s = (N=%g,F=%g,L=%g,R=%g,T=%g,B=%g,O=%u)",
        pcPrefix, m_fNear, m_fFar, m_fLeft, m_fRight, m_fTop, m_fBottom, 
        m_bOrtho);

    return pcString;
}
//---------------------------------------------------------------------------
#endif
