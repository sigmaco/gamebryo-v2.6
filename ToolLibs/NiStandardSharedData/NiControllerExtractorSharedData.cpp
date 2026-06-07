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

#include "NiControllerExtractorSharedData.h"

NiImplementRTTI(NiControllerExtractorSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiControllerExtractorSharedData::NiControllerExtractorSharedData()
{
    m_kInfoArray.SetGrowBy(3);
}
//---------------------------------------------------------------------------
NiControllerExtractorSharedData::~NiControllerExtractorSharedData()
{
    m_kInfoArray.RemoveAll();
}
//---------------------------------------------------------------------------
NiControllerExtractor::InfoArray& NiControllerExtractorSharedData::
    GetInfoArray()
{
    return m_kInfoArray;
}
//---------------------------------------------------------------------------
void NiControllerExtractorSharedData::AddInfo(
    NiControllerExtractor::SequenceInfo* pkInfo)
{
    m_kInfoArray.Add(pkInfo);
}
//---------------------------------------------------------------------------
NiControllerExtractor::NiCCAGlobalInfoArray& NiControllerExtractorSharedData::
    GetCCAGlobalInfoArray()
{
    return m_kCCAGlobalArray;
}
//---------------------------------------------------------------------------
void NiControllerExtractorSharedData::AddCCAGlobalInfo(
    NiControllerExtractor::NiCCAGlobalInfo* pkInfo)
{
    m_kCCAGlobalArray.Add(pkInfo);
}
//---------------------------------------------------------------------------
NiControllerExtractor::NiCCAInfoArray& NiControllerExtractorSharedData::
    GetCCAOverrideInfoArray()
{
    return m_kCCAOverrideArray;
}
//---------------------------------------------------------------------------
void NiControllerExtractorSharedData::NiAddCCAOverrideInfo(
    NiControllerExtractor::NiCCAInfo* pkInfo)
{
    m_kCCAOverrideArray.Add(pkInfo);
}
//---------------------------------------------------------------------------
