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

#ifndef NIINTEGEREXTRADATA_H
#define NIINTEGEREXTRADATA_H

#include "NiExtraData.h"

class NIMAIN_ENTRY NiIntegerExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiIntegerExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiIntegerExtraData (const int iValue);
    ~NiIntegerExtraData ();

    inline void SetValue(const int iValue);
    inline int GetValue() const;

protected:
    // support for streaming
    NiIntegerExtraData ();

    int m_iValue;
};

NiSmartPointer(NiIntegerExtraData);
//---------------------------------------------------------------------------
//  Inline include
#include "NiIntegerExtraData.inl"

//---------------------------------------------------------------------------

#endif

