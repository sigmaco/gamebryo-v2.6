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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiBound.h"
#include "NiMath.h"
#include "NiMatrix3.h"
#include "NiNode.h"
#include "NiPlane.h"
#include "NiPoint3.h"
#include <NiRTLib.h>

//---------------------------------------------------------------------------
void NiBound::Merge(const NiBound *pBound)
{
    // difference of this' and pBound's sphere center
    NiPoint3 diff = m_kCenter-pBound->m_kCenter;
    
    float fLengthSqr = diff*diff;
    float fDeltaRad = pBound->m_fRadius - m_fRadius;
    float fDeltaRadSqr = fDeltaRad*fDeltaRad;
    float fLength, fAlpha;
    
    if (fDeltaRad >= 0.0f)
    {
        if (fDeltaRadSqr >= fLengthSqr)
        {
            // pBound's sphere encloses this's sphere
            m_kCenter = pBound->m_kCenter;
            m_fRadius = pBound->m_fRadius;
        }
        else
        {
            // this' sphere does not enclose pBound's sphere
            fLength = NiSqrt(fLengthSqr);
            if (fLength > ms_fTolerance)
            {
                fAlpha = (fLength - fDeltaRad)/(2.0f*fLength);
                m_kCenter = pBound->m_kCenter + fAlpha*diff;
            }
            m_fRadius = 0.5f*(pBound->m_fRadius+fLength+m_fRadius);
        }
    }
    else if (fDeltaRadSqr < fLengthSqr)
    {
        // this' sphere does not enclose pBound's sphere
        fLength = NiSqrt(fLengthSqr);
        if (fLength > ms_fTolerance)
        {
            fAlpha = (fLength - fDeltaRad)/(2.0f*fLength);
            m_kCenter = pBound->m_kCenter + fAlpha*diff;
        }
        m_fRadius = 0.5f*(pBound->m_fRadius+fLength+m_fRadius);
    }
    // else this's sphere encloses pBound's sphere
}
//---------------------------------------------------------------------------
