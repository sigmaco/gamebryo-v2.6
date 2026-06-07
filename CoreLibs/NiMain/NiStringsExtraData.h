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

#ifndef NISTRINGSEXTRADATA_H
#define NISTRINGSEXTRADATA_H

#include "NiExtraData.h"

class NIMAIN_ENTRY NiStringsExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiStringsExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiStringsExtraData(const unsigned int uiSize, const char** ppcValue);
    ~NiStringsExtraData();

    void SetArray(const unsigned int uiSize, const char** ppcValue);
    inline void GetArray(unsigned int &uiSize, char** &ppcValue) const;

    bool SetValue(const unsigned int uiIndex, char* pcValue);
    char* GetValue(const unsigned int uiIndex) const;

protected:
    // support for streaming
    NiStringsExtraData ();

    unsigned int m_uiSize;
    char** m_ppcValue;
};

NiSmartPointer(NiStringsExtraData);
//---------------------------------------------------------------------------
//  Inline include
#include "NiStringsExtraData.inl"

//---------------------------------------------------------------------------

#endif

