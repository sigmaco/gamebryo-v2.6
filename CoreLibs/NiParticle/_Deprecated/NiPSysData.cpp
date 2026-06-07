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

#include "NiPSysData.h"
#include <NiBool.h>

NiImplementRTTI(NiPSysData, NiParticlesData);

//---------------------------------------------------------------------------
NiPSysData::NiPSysData() :
    m_pkParticleInfo(NULL),
    m_pfRotationSpeeds(NULL),
    m_usNumAddedParticles(0),
    m_usAddedParticlesBase(0)
{
}
//---------------------------------------------------------------------------
NiPSysData::~NiPSysData()
{
    NiDelete[] m_pkParticleInfo;
    NiFree(m_pfRotationSpeeds);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysData);
//---------------------------------------------------------------------------
void NiPSysData::LoadBinary(NiStream& kStream)
{
    NiParticlesData::LoadBinary(kStream);

    m_pkParticleInfo = NiNew NiParticleInfo[m_usVertices];
    for (unsigned short us = 0; us < m_usVertices; us++)
    {
        m_pkParticleInfo[us].LoadBinary(kStream);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 0, 0, 2))
    {
        NiBool bHasRotationSpeeds;
        NiStreamLoadBinary(kStream, bHasRotationSpeeds);
        if (bHasRotationSpeeds)
        {
            m_pfRotationSpeeds = NiAlloc(float, m_usVertices);
            NiStreamLoadBinary(kStream, m_pfRotationSpeeds, m_usVertices);
        }
    }
    else
    {
        if (m_pfRotationAngles)
        {
            m_pfRotationSpeeds = NiAlloc(float, m_usVertices);
            memset(m_pfRotationSpeeds, 0, m_usVertices *
                sizeof(m_pfRotationSpeeds[0]));
        }
    }

    NiStreamLoadBinary(kStream, m_usNumAddedParticles);
    NiStreamLoadBinary(kStream, m_usAddedParticlesBase);
}
//---------------------------------------------------------------------------
void NiPSysData::LinkObject(NiStream& kStream)
{
    NiParticlesData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysData::RegisterStreamables(NiStream& kStream)
{
    return NiParticlesData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysData::SaveBinary(NiStream& kStream)
{
    NiParticlesData::SaveBinary(kStream);

    NIASSERT(m_pkParticleInfo);
    for (unsigned short us = 0; us < m_usVertices; us++)
    {
        m_pkParticleInfo[us].SaveBinary(kStream);
    }

    NiBool bHasRotationSpeeds = (m_pfRotationSpeeds != NULL);
    NiStreamSaveBinary(kStream, bHasRotationSpeeds);
    if (bHasRotationSpeeds)
    {
        NiStreamSaveBinary(kStream, m_pfRotationSpeeds, m_usVertices);
    }

    NiStreamSaveBinary(kStream, m_usNumAddedParticles);
    NiStreamSaveBinary(kStream, m_usAddedParticlesBase);
}
//---------------------------------------------------------------------------
bool NiPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiParticlesData::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysData* pkDest = (NiPSysData*) pkObject;

    if ((m_pfRotationSpeeds && !pkDest->m_pfRotationSpeeds) ||
        (!m_pfRotationSpeeds && pkDest->m_pfRotationSpeeds))
    {
        return false;
    }

    for (unsigned short us = 0; us < m_usActiveVertices; us++)
    {
        if (!m_pkParticleInfo[us].IsEqual(pkDest->m_pkParticleInfo[us]))
        {
            return false;
        }

        if (m_pfRotationSpeeds &&
            m_pfRotationSpeeds[us] != pkDest->m_pfRotationSpeeds[us])
        {
            return false;
        }
    }

    if (m_usNumAddedParticles != pkDest->m_usNumAddedParticles ||
        m_usAddedParticlesBase != pkDest->m_usAddedParticlesBase)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
