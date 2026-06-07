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

#ifndef NIBEZFLOATKEY_H
#define NIBEZFLOATKEY_H

#include "NiFloatKey.h"

class NIANIMATION_ENTRY NiBezFloatKey : public NiFloatKey
{
    NiDeclareAnimationStream;
public: 
    // construction
    NiBezFloatKey ();
    NiBezFloatKey (float fTime, float fVal, float fInTan, float fOutTan);
    
    // attributes
    inline void SetInTan (float fInTan);
    inline void SetOutTan (float fOutTan);
    inline float GetInTan () const;
    inline float GetOutTan () const;

protected:
    float m_fInTan;
    float m_fOutTan;
};

NiRegisterAnimationStream(NiBezFloatKey);

#include "NiBezFloatKey.inl"

#endif

