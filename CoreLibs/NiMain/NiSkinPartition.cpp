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

#include "NiSkinPartition.h"

#include "NiBool.h"
#include "NiSkinData.h"
#include "NiRenderer.h"

#include <NiSystem.h>

NiImplementRTTI(NiSkinPartition, NiObject);

//---------------------------------------------------------------------------
// NiSkinPartition::Partition member functions

//---------------------------------------------------------------------------
NiSkinPartition::Partition::Partition()
{
    m_pusBones = NULL;
    m_pfWeights = NULL;
    m_pusVertexMap = NULL;
    m_pucBonePalette = NULL;
    m_pusTriList = NULL;
    m_pusStripLengths = NULL;
    m_usStrips = 0;
}

//---------------------------------------------------------------------------
NiSkinPartition::Partition::~Partition()
{
    NiFree(m_pusBones);
    NiFree(m_pfWeights);
    NiFree(m_pusVertexMap);
    NiFree(m_pucBonePalette);
    NiFree(m_pusTriList);
    NiFree(m_pusStripLengths);
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::PostPrecache()
{
    // Delete everything except strip lengths and bone indices.

    NiFree(m_pfWeights);
    m_pfWeights = NULL;
    NiFree(m_pusVertexMap);
    m_pusVertexMap = NULL;
    NiFree(m_pucBonePalette);
    m_pucBonePalette = NULL;
    NiFree(m_pusTriList);
    m_pusTriList = NULL;
}

//---------------------------------------------------------------------------
unsigned int NiSkinPartition::Partition::GetStripLengthSum() const
{
    unsigned int uiStripLengthSum = 0;

    for (unsigned int i = 0; i < m_usStrips; i++)
    {
        uiStripLengthSum += m_pusStripLengths[i];
    }

    return uiStripLengthSum;
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::LoadBinary(NiStream& kStream)
{
    NiStreamLoadBinary(kStream, m_usVertices);
    NiStreamLoadBinary(kStream, m_usTriangles);
    NiStreamLoadBinary(kStream, m_usBones);
    NiStreamLoadBinary(kStream, m_usStrips);
    NiStreamLoadBinary(kStream, m_usBonesPerVertex);
    
    m_pusBones = NiAlloc(unsigned short, m_usBones);
    NiStreamLoadBinary(kStream, m_pusBones, m_usBones);
    
    NiBool bHasMap;
    NiStreamLoadBinary(kStream, bHasMap);
    
    if (bHasMap)
    {
        m_pusVertexMap = NiAlloc(unsigned short, m_usVertices);
        NiStreamLoadBinary(kStream, m_pusVertexMap, m_usVertices);
    }
    
    unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;

    NiBool bHasWeights;
    NiStreamLoadBinary(kStream, bHasWeights);

    if (bHasWeights)
    {
        m_pfWeights = NiAlloc(float, uiInteractions);
        NiStreamLoadBinary(kStream, m_pfWeights, uiInteractions);
    }

    unsigned int uiListLen;

    if (m_usStrips == 0)
    {
        uiListLen = 3 * m_usTriangles;
    }
    else
    {
        m_pusStripLengths = NiAlloc(unsigned short, m_usStrips);
        NiStreamLoadBinary(kStream, m_pusStripLengths, m_usStrips);
        uiListLen = GetStripLengthSum();
    }

    NiBool bHasList;
    NiStreamLoadBinary(kStream, bHasList);

    if (bHasList)
    {
        m_pusTriList = NiAlloc(unsigned short, uiListLen);
        NiStreamLoadBinary(kStream, m_pusTriList, uiListLen);
    }

    NiBool bHasPalette;    
    NiStreamLoadBinary(kStream, bHasPalette);
    if (bHasPalette)
    {
        m_pucBonePalette = NiAlloc(unsigned char, uiInteractions);
        NiStreamLoadBinary(kStream, m_pucBonePalette, uiInteractions);
    }
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::SaveBinary(NiStream& kStream)
{
    NiStreamSaveBinary(kStream, m_usVertices);
    NiStreamSaveBinary(kStream, m_usTriangles);
    NiStreamSaveBinary(kStream, m_usBones);
    NiStreamSaveBinary(kStream, m_usStrips);
    NiStreamSaveBinary(kStream, m_usBonesPerVertex);
    
    NiStreamSaveBinary(kStream, m_pusBones, m_usBones);

    NiBool bHasMap = (m_pusVertexMap != NULL);
    NiStreamSaveBinary(kStream, bHasMap);
    if (bHasMap)
    {
        NiStreamSaveBinary(kStream, m_pusVertexMap, m_usVertices);
    }

    unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;

    NiBool bHasWeights = (m_pfWeights != NULL);
    NiStreamSaveBinary(kStream, bHasWeights);
    if (bHasWeights)
    {
        NiStreamSaveBinary(kStream, m_pfWeights, uiInteractions);
    }

    unsigned int uiListLen;
    
    if (m_usStrips == 0)
    {
        uiListLen = 3 * m_usTriangles;
    }
    else
    {
        NiStreamSaveBinary(kStream, m_pusStripLengths, m_usStrips);
        unsigned int uiStripLengthSum = GetStripLengthSum();
        uiListLen = uiStripLengthSum;
    }

    NiBool bHasList = (m_pusTriList != NULL);
    NiStreamSaveBinary(kStream, bHasList);
    if (bHasList)
    {
        NiStreamSaveBinary(kStream, m_pusTriList, uiListLen);
    }

    NiBool bHasPalette = (m_pucBonePalette != NULL);
    NiStreamSaveBinary(kStream, bHasPalette);
    if (bHasPalette)
    {
        NiStreamSaveBinary(kStream, m_pucBonePalette, uiInteractions);
    }
}

//---------------------------------------------------------------------------
bool NiSkinPartition::Partition::IsEqual(const Partition& kPartition)
{
    if (m_usVertices != kPartition.m_usVertices)
        return false;
    if (m_usTriangles != kPartition.m_usTriangles)
        return false;
    if (m_usBones != kPartition.m_usBones)
        return false;
    if (m_usStrips != kPartition.m_usStrips)
        return false;
    if (m_usBonesPerVertex != kPartition.m_usBonesPerVertex)
        return false;
    
    if (memcmp(m_pusBones, kPartition.m_pusBones,
        sizeof(m_pusBones[0]) * m_usBones))
    {
        return false;
    }

    if (memcmp(m_pusVertexMap, kPartition.m_pusVertexMap,
        sizeof(m_pusVertexMap[0]) * m_usVertices))
    {
        return false;
    }

    unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;

    if (memcmp(m_pfWeights, kPartition.m_pfWeights,
        sizeof(m_pfWeights[0]) * uiInteractions))
    {
        return false;
    }

    if (m_usStrips == 0)
    {
        if (memcmp(m_pusTriList, kPartition.m_pusTriList,
            sizeof(m_pusTriList[0]) * 3 * m_usTriangles))
        {
            return false;
        }
    }
    else
    {
        if (memcmp(m_pusStripLengths, kPartition.m_pusStripLengths,
            sizeof(m_pusStripLengths[0]) * m_usStrips))
        {
            return false;
        }

        unsigned int uiStripLengthSum = GetStripLengthSum();
        if (memcmp(m_pusTriList, kPartition.m_pusTriList,
            sizeof(m_pusTriList[0]) * uiStripLengthSum))
        {
            return false;
        }
    }

    if ((m_pucBonePalette == NULL) != (kPartition.m_pucBonePalette == NULL))
        return false;

    if (m_pucBonePalette != NULL)
    {
        if (memcmp(m_pucBonePalette, kPartition.m_pucBonePalette,
            sizeof(m_pucBonePalette[0]) * uiInteractions))
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
// NiSkinPartition member functions

//---------------------------------------------------------------------------
NiSkinPartition::NiSkinPartition()
{
    m_pkPartitions = NULL;
}

//---------------------------------------------------------------------------
NiSkinPartition::~NiSkinPartition()
{
    NiDelete[] m_pkPartitions;
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinPartition);
//---------------------------------------------------------------------------
void NiSkinPartition::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiPartitions);

    m_pkPartitions = NiNew Partition[m_uiPartitions];

    for (unsigned int i = 0; i < m_uiPartitions; i++)
    {
        m_pkPartitions[i].LoadBinary(kStream);
    }
}
//---------------------------------------------------------------------------
void NiSkinPartition::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiSkinPartition::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiSkinPartition::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiPartitions);

    for (unsigned int i = 0; i < m_uiPartitions; i++)
    {
        m_pkPartitions[i].SaveBinary(kStream);
    }
}
//---------------------------------------------------------------------------
bool NiSkinPartition::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiSkinPartition* pkSkinPartition = (NiSkinPartition*) pkObject;

    if (m_uiPartitions != pkSkinPartition->m_uiPartitions)
    {
        return false;
    }

    for (unsigned int i = 0; i < m_uiPartitions; i++)
    {
        if (!m_pkPartitions[i].IsEqual(pkSkinPartition->m_pkPartitions[i]))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
