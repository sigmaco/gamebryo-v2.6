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

#ifndef NIFLOATEXTRADATA_H
#define NIFLOATEXTRADATA_H

#include "NiExtraData.h"

class NIMAIN_ENTRY NiFloatExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiFloatExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiFloatExtraData (const float fValue);
    ~NiFloatExtraData ();

    inline void SetValue (const float fValue);
    inline float GetValue () const;

protected:
    // support for streaming
    NiFloatExtraData ();

    float m_fValue;
};

NiSmartPointer(NiFloatExtraData);

//---------------------------------------------------------------------------
//  Inline include
#include "NiFloatExtraData.inl"

//---------------------------------------------------------------------------

#endif

