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

#ifndef NIINTERPSCALAR_H
#define NIINTERPSCALAR_H

#include "NiAnimationLibType.h"

// A static class defining scalar interpolation functions

class NIANIMATION_ENTRY NiInterpScalar
{
public:
    // *** begin Emergent internal use only ***

    static float Linear (float fTime, float fP, float fQ);
    static float Bezier (float fTime, float fP, float fDP, 
        float fQ, float fDQ);
    static float TCB (float fTime, float fP, float fPDD, 
        float fQ, float fQDS);

    static void AdjustBezier(float fLastValue, float fLastTime, 
        float& fLastOut, float fNextValue, float fNextTime, float& fNextIn, 
        float fNewTime, float fNewValue, float& fNewIn, float& fNewOut);

    // *** end Emergent internal use only ***

};


#endif

