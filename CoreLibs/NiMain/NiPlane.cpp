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

#include "NiPlane.h"

//---------------------------------------------------------------------------
NiPlane::NiPlane()
{
    m_kNormal = NiPoint3::ZERO;
    m_fConstant = 0.0f;
}
//---------------------------------------------------------------------------
NiPlane::NiPlane(const NiPoint3& kNormal, float fConstant)
{
    m_kNormal = kNormal;
    m_fConstant = fConstant;
}
//---------------------------------------------------------------------------
NiPlane::NiPlane(const NiPoint3& kNormal, const NiPoint3& kPoint)
{
    m_kNormal = kNormal;
    m_fConstant = kNormal * kPoint;
}
//---------------------------------------------------------------------------
NiPlane::NiPlane(const NiPoint3& kP0, const NiPoint3& kP1,const NiPoint3& kP2)
{
    NiPoint3 kDif1 = kP1 - kP0;
    NiPoint3 kDif2 = kP2 - kP1;
    m_kNormal = kDif1.UnitCross(kDif2);
    m_fConstant = m_kNormal * kP0; 
}
//---------------------------------------------------------------------------
#ifndef __SPU__
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiPlane::LoadBinary(NiStream& stream)
{
    m_kNormal.LoadBinary(stream);
    NiStreamLoadBinary(stream,m_fConstant);
}
//---------------------------------------------------------------------------
void NiPlane::SaveBinary(NiStream& stream)
{
    m_kNormal.SaveBinary(stream);
    NiStreamSaveBinary(stream,m_fConstant);
}
//---------------------------------------------------------------------------
char* NiPlane::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 56;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "(%g,%g,%g), %g", m_kNormal.x, m_kNormal.y,
        m_kNormal.z, m_fConstant);

    return pString;
}
//---------------------------------------------------------------------------
#endif
