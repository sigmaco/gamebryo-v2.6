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


#ifndef NIVERTWEIGHTSEXTRADATA_H
#define NIVERTWEIGHTSEXTRADATA_H

#include "NiExtraData.h"

class  NIMAIN_ENTRY NiVertWeightsExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiVertWeightsExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
//    NiVertWeightsExtraData (unsigned short usSize, const float* pArray);
    ~NiVertWeightsExtraData ();

//    void SetWeights (unsigned short usSize, const float* pWeights);
//    const float* GetWeights () const;

//    unsigned short GetWeightsCount() const { return m_usWeightCount; }

protected:
    // support for streaming
    NiVertWeightsExtraData ();

//    unsigned short  m_usWeightCount;
//    float*          m_pWeights;
};
NiSmartPointer(NiVertWeightsExtraData);
//---------------------------------------------------------------------------
//  Inline include
#include "NiVertWeightsExtraData.inl"

//---------------------------------------------------------------------------

#endif 
