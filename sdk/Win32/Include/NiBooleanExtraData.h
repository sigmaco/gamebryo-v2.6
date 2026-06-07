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

#ifndef NIBOOLEANEXTRADATA_H
#define NIBOOLEANEXTRADATA_H

#include "NiExtraData.h"

class NIMAIN_ENTRY NiBooleanExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiBooleanExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBooleanExtraData (const bool bValue);
    ~NiBooleanExtraData ();

    inline void SetValue(const bool bValue);
    inline bool GetValue() const;

protected:
    // support for streaming
    NiBooleanExtraData ();

    bool m_bValue;
};

NiSmartPointer(NiBooleanExtraData);

//---------------------------------------------------------------------------
//  Inline include
#include "NiBooleanExtraData.inl"

//---------------------------------------------------------------------------

#endif

