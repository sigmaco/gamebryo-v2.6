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

//  **  Deprecated class - no longer load/save the weight info.  **

// Precompiled Header
#include "NiMainPCH.h"

#include "NiVertWeightsExtraData.h"

NiImplementRTTI(NiVertWeightsExtraData,NiExtraData);

//---------------------------------------------------------------------------
//NiVertWeightsExtraData::NiVertWeightsExtraData(
//    unsigned short  usSize,
//    const float*    pArray)
//{
//   m_pWeights       = 0;    
//   m_usWeightCount  = 0;
//   SetWeights(usSize, pArray);
//}
//---------------------------------------------------------------------------
//void NiVertWeightsExtraData::SetWeights(
//    unsigned short  usSize, 
//    const float*    pWeights)
//{
//    delete[] m_pWeights;
//    m_pWeights = NULL;
//    m_usWeightCount = 0;
//    if ( pWeights && usSize )
//    {
//        m_pWeights  = new float[usSize];
//        NIASSERT( m_pWeights );
//        NiMemcpy( m_pWeights, usSize * sizeof(float), pWeights,
//              usSize * sizeof(float));
//        m_usWeightCount = usSize;
//
//        // array is written as array size followed 
//        // by array of floats 
//        m_uiSize = sizeof(unsigned short) + m_usWeightCount * sizeof(float);
//    }
// }
 
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiVertWeightsExtraData);
//---------------------------------------------------------------------------
void NiVertWeightsExtraData::CopyMembers(NiVertWeightsExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

//    pDest->m_usWeightCount = m_usWeightCount;
//    if ( m_usWeightCount )
//        pDest->m_pWeights = new float[m_usWeightCount];
//    else
//        pDest->m_pWeights = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiVertWeightsExtraData);
//---------------------------------------------------------------------------

// Deprecated class - no longer load the weight info into member variables.

void NiVertWeightsExtraData::LoadBinary(NiStream&)
{
    //NiStreamLoadBinary(kStream, m_usWeightCount);
    //m_pWeights = new float[m_usWeightCount];
    //NIASSERT( m_pWeights );
    //for (int i = 0; i < m_usWeightCount; i++)
    //    NiStreamLoadBinary(stream, m_pWeights[i]);
}
//---------------------------------------------------------------------------
void NiVertWeightsExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiVertWeightsExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------

// Deprecated class - no longer save the weight info.

void NiVertWeightsExtraData::SaveBinary(NiStream&)
{
    //NiExtraData::SaveBinary(kStream);

    //NiStreamSaveBinary(stream, m_usWeightCount);
    //for (int i = 0; i < m_usWeightCount; i++)
    //    NiStreamSaveBinary(stream, m_pWeights[i]);
}
//---------------------------------------------------------------------------
bool NiVertWeightsExtraData::IsEqual(NiObject* pObject)
{
    if ( !NiExtraData::IsEqual(pObject) )
        return false;

//    NiVertWeightsExtraData *pExtra = (NiVertWeightsExtraData*) pObject;
//    if ( m_usWeightCount != pExtra->m_usWeightCount )
//        return false;
//    for (unsigned short usWeight = 0; usWeight < m_usWeightCount; usWeight++)
//    {
//        if ( m_pWeights[usWeight] != pExtra->m_pWeights[usWeight] )
//            return false;
//    }

    return true;
}
//---------------------------------------------------------------------------
void NiVertWeightsExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(
        NiVertWeightsExtraData::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
