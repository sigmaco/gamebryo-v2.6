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
#include <NiParticlePCH.h>

#include "NiParticleInfo.h"
#include <NiStream.h>

//---------------------------------------------------------------------------
NiParticleInfo::NiParticleInfo() :
    m_kVelocity(NiPoint3::ZERO),
    m_fAge(0.0f),
    m_fLifeSpan(0.0f),
    m_fLastUpdate(0.0f),
    m_usGeneration(0),
    m_usCode(0)
{
}
//---------------------------------------------------------------------------
void NiParticleInfo::LoadBinary(NiStream& kStream)
{
    m_kVelocity.LoadBinary(kStream);
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 5))
    {
        // The rotation axis is no longer a member of NiParticleInfo, so
        // this value is ignored.
        NiPoint3 kRotationAxis;
        kRotationAxis.LoadBinary(kStream);
    }

    NiStreamLoadBinary(kStream, m_fAge);
    NiStreamLoadBinary(kStream, m_fLifeSpan);

    NiStreamLoadBinary(kStream, m_fLastUpdate);

    NiStreamLoadBinary(kStream, m_usGeneration);
    NiStreamLoadBinary(kStream, m_usCode);
}
//---------------------------------------------------------------------------
void NiParticleInfo::SaveBinary(NiStream& kStream)
{
    m_kVelocity.SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fAge);
    NiStreamSaveBinary(kStream, m_fLifeSpan);

    NiStreamSaveBinary(kStream, m_fLastUpdate);

    NiStreamSaveBinary(kStream, m_usGeneration);
    NiStreamSaveBinary(kStream, m_usCode);
}
//---------------------------------------------------------------------------
bool NiParticleInfo::IsEqual(const NiParticleInfo& kData)
{
    if (m_kVelocity != kData.m_kVelocity ||
        m_fAge != kData.m_fAge ||
        m_fLifeSpan != kData.m_fLifeSpan ||
        m_fLastUpdate != kData.m_fLastUpdate ||
        m_usGeneration != kData.m_usGeneration ||
        m_usCode != kData.m_usCode)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
