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

#ifndef NISTRINGEXTRADATA_H
#define NISTRINGEXTRADATA_H

#include "NiExtraData.h"

class NIMAIN_ENTRY NiStringExtraData : public NiExtraData
{
    NiDeclareRTTI;
    NiDeclareClone(NiStringExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiStringExtraData (const NiFixedString& kString);
    ~NiStringExtraData ();

    void SetValue(const NiFixedString& kString);
    inline const NiFixedString& GetValue() const;

protected:
    // support for streaming
    NiStringExtraData ();

    NiFixedString m_kString;
};

NiSmartPointer(NiStringExtraData);
//---------------------------------------------------------------------------
//  Inline include
#include "NiStringExtraData.inl"

//---------------------------------------------------------------------------

#endif

