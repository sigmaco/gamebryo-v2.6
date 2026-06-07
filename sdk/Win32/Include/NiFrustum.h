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

#ifndef NIFRUSTUM_H
#define NIFRUSTUM_H

#include "NiMainLibType.h"
#include <NiMemObject.h>

// view frustum and view port
class NIMAIN_ENTRY NiFrustum : public NiMemObject
{
public:
    NiFrustum(bool bOrtho = false);
    NiFrustum(float fLeft, float fRight, float fTop,
        float fBottom, float fNear, float fFar,
        bool bOrtho = false);

    float m_fLeft;
    float m_fRight;
    float m_fTop;
    float m_fBottom;
    float m_fNear;
    float m_fFar;
    bool  m_bOrtho;
#ifndef __SPU__
    char* GetViewerString(const char* pcPrefix) const;
#endif
};

#endif
