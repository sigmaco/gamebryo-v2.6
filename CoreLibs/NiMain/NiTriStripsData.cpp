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

#include "NiTriStripsData.h"
#include "NiBool.h"

NiImplementRTTI(NiTriStripsData,NiTriBasedGeomData);

//---------------------------------------------------------------------------
NiTriStripsData::NiTriStripsData(unsigned short usVertices, 
    NiPoint3* pkVertex, NiPoint3* pkNormal, NiColorA* pkColor, 
    NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiShaderRequirementDesc::NBTFlags eNBTMethod,
    unsigned short usTriangles, 
    unsigned short usStrips, unsigned short* pusStripLengths, 
    unsigned short* pusStripLists) :
    NiTriBasedGeomData(usVertices, pkVertex, pkNormal, pkColor, pkTexture, 
        usNumTextureSets, eNBTMethod, usTriangles)
{
#ifdef NIDEBUG
    unsigned short usTriangleCount = 0;
    unsigned int i;

    for (i = 0; i < usStrips; i++)
    {
        usTriangleCount += pusStripLengths[i] - 2;
    }
    NIASSERT(usTriangleCount == usTriangles);
#endif

    NIMEMASSERT(NiVerifyAddress(pusStripLengths));
    NIMEMASSERT(NiVerifyAddress(pusStripLists));

    m_usStrips = usStrips;
    m_pusStripLengths = pusStripLengths;
    m_pusStripLists = pusStripLists;
}
//---------------------------------------------------------------------------
NiTriStripsData::NiTriStripsData()
{
    m_usStrips = 0;
    m_pusStripLengths = NULL;
    m_pusStripLists = NULL;
}
//---------------------------------------------------------------------------
NiTriStripsData::~NiTriStripsData()
{
    if (GetGroup())
    {
        GetGroup()->DecRefCount();
        GetGroup()->DecRefCount();
    }
    else
    {
        NiFree(m_pusStripLengths);
        NiFree(m_pusStripLists);
    }
}
//---------------------------------------------------------------------------
void NiTriStripsData::GetTriangleIndices(unsigned short i,
    unsigned short& i0, unsigned short& i1, unsigned short& i2) const
{
    // Call straight through to the non-virtual inline.
    GetTriangleIndices_Inline(i, i0, i1, i2);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTriStripsData);

//---------------------------------------------------------------------------
void NiTriStripsData::LoadBinary(NiStream& kStream)
{
    NiTriBasedGeomData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usStrips);
    if (m_usStrips > 0)
    {
        if (GetGroup())
        {
            m_pusStripLengths = (unsigned short*) GetGroup()->
                Allocate(m_usStrips * sizeof(unsigned short));
        }
        else
        {
            m_pusStripLengths = NiAlloc(unsigned short,
                m_usStrips);
        }
        NIASSERT(m_pusStripLengths != NULL);

        NiStreamLoadBinary(kStream, m_pusStripLengths, m_usStrips);

        NiBool bHasList;
        NiStreamLoadBinary(kStream, bHasList);

        if (bHasList)
        {
            unsigned int uiStripLengthSum = GetStripLengthSum();

            if (GetGroup())
            {
                m_pusStripLists = (unsigned short*) GetGroup()->
                    Allocate(uiStripLengthSum * sizeof(unsigned short));
            }
            else
            {
                m_pusStripLists = NiAlloc(unsigned short,
                    uiStripLengthSum);
            }
            NIASSERT(m_pusStripLists != NULL);
            
            NiStreamLoadBinary(kStream, m_pusStripLists, uiStripLengthSum);
        }
    }
}
//---------------------------------------------------------------------------
void NiTriStripsData::LinkObject(NiStream& kStream)
{
    NiTriBasedGeomData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriStripsData::RegisterStreamables(NiStream& kStream)
{
    return NiTriBasedGeomData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriStripsData::SaveBinary(NiStream& kStream)
{
    NiTriBasedGeomData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usStrips);
    if (m_usStrips > 0)
    {
        NiStreamSaveBinary(kStream, m_pusStripLengths, m_usStrips);

        NiBool bHasList = (m_pusStripLists != NULL);
        NiStreamSaveBinary(kStream, bHasList);

        if (bHasList)
        {
            NiStreamSaveBinary(kStream, m_pusStripLists, GetStripLengthSum());
        }
    }
}
//---------------------------------------------------------------------------
bool NiTriStripsData::IsEqual(NiObject* pkObject)
{
    if (!NiTriBasedGeomData::IsEqual(pkObject))
        return false;

    NiTriStripsData* pkData = (NiTriStripsData*)pkObject;

    if (m_usStrips != pkData->m_usStrips)
    {
        return false;
    }

    unsigned int i, uiStripLengthSum;

    for (i = 0; i < m_usStrips; i++)
    {
        if (m_pusStripLengths[i] != pkData->m_pusStripLengths[i])
        {
            return false;
        }
    }

    uiStripLengthSum = GetStripLengthSum();

    // if both are null, assume data has been precached,
    // and original data has been released properly
    if (!m_pusStripLists && !pkData->m_pusStripLists)
        return true;
    
    // Only do element-wise comparison if pointers are valid
    if (m_pusStripLists && pkData->m_pusStripLists)
    {
        for (i = 0; i < uiStripLengthSum; i++)
        {
            if (m_pusStripLists[i] != pkData->m_pusStripLists[i])
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiTriStripsData::GetBlockAllocationSize() const
{
    unsigned int uiSize = (m_usStrips + GetStripLengthSum()) * 
        sizeof(unsigned short);

    return NiTriBasedGeomData::GetBlockAllocationSize() + uiSize;
}
//---------------------------------------------------------------------------
