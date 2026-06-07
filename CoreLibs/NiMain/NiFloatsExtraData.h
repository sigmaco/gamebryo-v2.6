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

#ifndef NIFLOATSEXTRADATA_H
#define NIFLOATSEXTRADATA_H

#include "NiExtraData.h"

class NIMAIN_ENTRY NiFloatsExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiFloatsExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiFloatsExtraData(const unsigned int uiSize, const float* pfValue);
    ~NiFloatsExtraData();

    void SetArray(const unsigned int uiSize, const float* pfValue);
    inline void GetArray(unsigned int &uiSize, float* &pfValue) const;

    bool SetValue(const unsigned int uiIndex, float fValue);
    float GetValue(const unsigned int uiIndex) const;

protected:
    // support for streaming
    NiFloatsExtraData();

    unsigned int m_uiSize;
    float* m_pfValue;
};
NiSmartPointer(NiFloatsExtraData);

//---------------------------------------------------------------------------
//  Inline include
#include "NiFloatsExtraData.inl"

//---------------------------------------------------------------------------

#endif

