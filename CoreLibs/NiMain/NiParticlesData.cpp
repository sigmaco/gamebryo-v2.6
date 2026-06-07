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

#include "NiParticlesData.h"
#include "NiBool.h"

NiImplementRTTI(NiParticlesData, NiGeometryData);

//---------------------------------------------------------------------------
NiParticlesData::NiParticlesData() :
    m_pfRadii(NULL),
    m_pfSizes(NULL),
    m_pkRotations(NULL),
    m_pfRotationAngles(NULL),
    m_pkRotationAxes(NULL)
{
    SetConsistency(NiGeometryData::VOLATILE);
}
//---------------------------------------------------------------------------
NiParticlesData::~NiParticlesData()
{
    NiFree(m_pfRadii);
    NiFree(m_pfSizes);
    NiDelete[] m_pkRotations;
    NiFree(m_pfRotationAngles);
    NiDelete[] m_pkRotationAxes;
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiParticlesData);
//---------------------------------------------------------------------------
void NiParticlesData::LoadBinary(NiStream& kStream)
{
    NiGeometryData::LoadBinary(kStream);

    m_pfRadii = NiAlloc(float,m_usVertices);
    NiBool bRadii;
    NiStreamLoadBinary(kStream, bRadii);
    if (bRadii)
    {
        NiStreamLoadBinary(kStream, m_pfRadii, m_usVertices);
    }
    else
    {
        for (unsigned short us = 0; us < m_usVertices; us++)
        {
            m_pfRadii[us] = 1.0f;
        }
    }

    NiStreamLoadBinary(kStream, m_usActiveVertices);

    NiBool bSize;
    NiStreamLoadBinary(kStream, bSize);

    m_pfSizes = NiAlloc(float,m_usVertices);

    if (bSize)
    {
        NiStreamLoadBinary(kStream,m_pfSizes,m_usVertices);
    }
    else
    {
        for (unsigned short i = 0; i < m_usVertices; i++)
            m_pfSizes[i] = 1.0f;
    }

    NiBool bHasRotation;
    NiStreamLoadBinary(kStream, bHasRotation);

    if (bHasRotation)
    {
        m_pkRotations = NiNew NiQuaternion[m_usVertices];
        for (unsigned short us = 0; us < m_usVertices; us++)
        {
            m_pkRotations[us].LoadBinary(kStream);
        }
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 5))
    {
        if (m_pkRotations)
        {
            m_pfRotationAngles = NiAlloc(float, m_usVertices);
            m_pkRotationAxes = NiNew NiPoint3[m_usVertices];
            for (unsigned short us = 0; us < m_usVertices; us++)
            {
                m_pkRotations[us].ToAngleAxis(m_pfRotationAngles[us],
                    m_pkRotationAxes[us]);
            }
            NiDelete[] m_pkRotations;
            m_pkRotations = NULL;

        }
    }
    else
    {
        NiBool bHasRotationAngles;
        NiStreamLoadBinary(kStream, bHasRotationAngles);
        if (bHasRotationAngles)
        {
            m_pfRotationAngles = NiAlloc(float, m_usVertices);
            for (unsigned short us = 0; us < m_usVertices; us++)
            {
                NiStreamLoadBinary(kStream, m_pfRotationAngles[us]);
            }
        }

        NiBool bHasRotationAxes;
        NiStreamLoadBinary(kStream, bHasRotationAxes);
        if (bHasRotationAxes)
        {
            m_pkRotationAxes = NiNew NiPoint3[m_usVertices];
            for (unsigned short us = 0; us < m_usVertices; us++)
            {
                m_pkRotationAxes[us].LoadBinary(kStream);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiParticlesData::LinkObject(NiStream& kStream)
{
    NiGeometryData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiParticlesData::RegisterStreamables(NiStream& kStream)
{
    return NiGeometryData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiParticlesData::SaveBinary(NiStream& kStream)
{
    NiGeometryData::SaveBinary(kStream);

    NiBool bRadii = (m_pfRadii != NULL);
    NiStreamSaveBinary(kStream, bRadii);
    if (bRadii)
    {
        NiStreamSaveBinary(kStream, m_pfRadii, m_usVertices);
    }

    NiStreamSaveBinary(kStream, m_usActiveVertices);

    NiBool bSize = (m_pfSizes != NULL);
    NiStreamSaveBinary(kStream, bSize);
    if (bSize)
    {
        NiStreamSaveBinary(kStream, m_pfSizes, m_usVertices);
    }

    NiBool bHasRotation = (m_pkRotations != NULL);
    NiStreamSaveBinary(kStream, bHasRotation);
    if (bHasRotation)
    {
        for (unsigned int i = 0; i < m_usVertices; i++)
            m_pkRotations[i].SaveBinary(kStream);
    }

    NiBool bHasRotationAngles = (m_pfRotationAngles != NULL);
    NiStreamSaveBinary(kStream, bHasRotationAngles);
    if (bHasRotationAngles)
    {
        for (unsigned int ui = 0; ui < m_usVertices; ui++)
        {
            NiStreamSaveBinary(kStream, m_pfRotationAngles[ui]);
        }
    }

    NiBool bHasRotationAxes = (m_pkRotationAxes != NULL);
    NiStreamSaveBinary(kStream, bHasRotationAxes);
    if (bHasRotationAxes)
    {
        for (unsigned int ui = 0; ui < m_usVertices; ui++)
        {
            m_pkRotationAxes[ui].SaveBinary(kStream);
        }
    }
}
//---------------------------------------------------------------------------
bool NiParticlesData::IsEqual(NiObject* pkObject)
{
    if (!NiGeometryData::IsEqual(pkObject))
        return false;

    NiParticlesData* pkPart = (NiParticlesData*) pkObject;

    if ((m_pfRadii && !pkPart->m_pfRadii) ||
        (!m_pfRadii && pkPart->m_pfRadii))
    {
        return false;
    }
    if (m_pfRadii)
    {
        for (unsigned short us = 0; us < m_usVertices; us++)
        {
            if (m_pfRadii[us] != pkPart->m_pfRadii[us])
            {
                return false;
            }
        }
    }

    if ((m_pfSizes && !pkPart->m_pfSizes) || (!m_pfSizes && pkPart->m_pfSizes))
        return false;

    if (m_pfSizes)
    {
        for (unsigned short i = 0; i < m_usActiveVertices; i++)
        {
            if (m_pfSizes[i] != pkPart->m_pfSizes[i])
                return false;
        }
    }

    if ((m_pkRotations && !pkPart->m_pkRotations) || 
        (!m_pkRotations && pkPart->m_pkRotations))
    {
        return false;
    }

    if (m_pkRotations)
    {
        for (unsigned int i = 0; i < m_usActiveVertices; i++)
        {
            if (m_pkRotations[i] != pkPart->m_pkRotations[i])
                return false;
        }
    }

    if (m_pfRotationAngles)
    {
        for (unsigned int ui = 0; ui < m_usActiveVertices; ui++)
        {
            if (m_pfRotationAngles[ui] != pkPart->m_pfRotationAngles[ui])
            {
                return false;
            }
        }
    }

    if (m_pkRotationAxes)
    {
        for (unsigned int ui = 0; ui < m_usActiveVertices; ui++)
        {
            if (m_pkRotationAxes[ui] != pkPart->m_pkRotationAxes[ui])
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
