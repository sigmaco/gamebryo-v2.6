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
#include "NiCollisionPCH.h"

#include <NiMath.h>
#include "NiSqrDistance.h"

//--------------------------------------------------------------------------- 
float NiSqrDistance::Compute(const NiPoint3& kPnt, 
    const NiParallelogram& kPgm, float& fPgmParam0, float& fPgmParam1)
{
    NiPoint3 kDiff = kPgm.m_kOrigin - kPnt;
    float fA = kPgm.m_kEdge0.Dot(kPgm.m_kEdge0);
    float fB = kPgm.m_kEdge0.Dot(kPgm.m_kEdge1);
    float fC = kPgm.m_kEdge1.Dot(kPgm.m_kEdge1);
    float fD = kPgm.m_kEdge0.Dot(kDiff);
    float fE = kPgm.m_kEdge1.Dot(kDiff);
    float fF = kDiff.Dot(kDiff);
    float fDet = NiAbs(fA * fC - fB * fB);

    const float fTolerance = 1e-05f;
    if (fDet >= fTolerance)
    {
        float fTmp;
        
        fPgmParam0 = fB * fE - fC * fD;
        fPgmParam1 = fB * fD - fA * fE;
        
        if (fPgmParam0 < 0)
        {
            if (fPgmParam1 < 0)  // region 6
            {
                if (fD < 0)
                {
                    fPgmParam1 = 0;
                    if (-fD >= fA)
                    {
                        fPgmParam0 = 1;
                        return NiAbs(fA + 2 * fD + fF);
                    }
                    else
                    {
                        fPgmParam0 = -fD / fA;
                        return NiAbs(fD * fPgmParam0 + fF);
                    }
                }
                else
                {
                    fPgmParam0 = 0;
                    if (fE >= 0)
                    {
                        fPgmParam1 = 0;
                        return NiAbs(fF);
                    }
                    else if (-fE >= fC)
                    {
                        fPgmParam1 = 1;
                        return NiAbs(fC + 2 * fE + fF);
                    }
                    else
                    {
                        fPgmParam1 = -fE / fC;
                        return NiAbs(fE * fPgmParam1 + fF);
                    }
                }
            }
            else if (fPgmParam1 <= fDet)  // region 5
            {
                fPgmParam0 = 0;
                if (fE >= 0)
                {
                    fPgmParam1 = 0;
                    return NiAbs(fF);
                }
                else if (-fE >= fC)
                {
                    fPgmParam1 = 1;
                    return NiAbs(fC + 2 * fE + fF);
                }
                else
                {
                    fPgmParam1 = -fE / fC;
                    return NiAbs(fE * fPgmParam1 + fF);
                }
            }
            else  // region 4
            {
                fTmp = fB + fD;
                if (fTmp < 0)
                {
                    fPgmParam1 = 1;
                    if (-fTmp >= fA)
                    {
                        fPgmParam0 = 1;
                        return NiAbs(fA + fC + fF + 2 * (fB + fD + fE));
                    }
                    else
                    {
                        fPgmParam0 = -fTmp / fA;
                        return NiAbs(fTmp * fPgmParam0 + fC + 2 * fE + fF);
                    }
                }
                else
                {
                    fPgmParam0 = 0;
                    if (fE >= 0)
                    {
                        fPgmParam1 = 0;
                        return NiAbs(fF);
                    }
                    else if (-fE >= fC)
                    {
                        fPgmParam1 = 1;
                        return NiAbs(fC + 2 * fE + fF);
                    }
                    else
                    {
                        fPgmParam1 = -fE / fC;
                        return NiAbs(fE * fPgmParam1 + fF);
                    }
                }
            }
        }
        else if (fPgmParam0 <= fDet)
        {
            if (fPgmParam1 < 0)  // region 7
            {
                fPgmParam1 = 0;
                if (fD >= 0)
                {
                    fPgmParam0 = 0;
                    return NiAbs(fF);
                }
                else if (-fD >= fA)
                {
                    fPgmParam0 = 1;
                    return NiAbs(fA + 2 * fD + fF);
                }
                else
                {
                    fPgmParam0 = -fD / fA;
                    return NiAbs(fD * fPgmParam0 + fF);
                }
            }
            else if (fPgmParam1 <= fDet)  // region 0
            {
                // minimum at interior point
                float fInvDet = 1.0f / fDet;
                fPgmParam0 *= fInvDet;
                fPgmParam1 *= fInvDet;
                return NiAbs(fPgmParam0 * (fA * fPgmParam0 + fB * fPgmParam1 
                    + 2 * fD) +  fPgmParam1 * (fB * fPgmParam0 
                    + fC * fPgmParam1 + 2 * fE) + fF);
            }
            else  // region 3
            {
                fPgmParam1 = 1;
                fTmp = fB + fD;
                if (fTmp >= 0)
                {
                    fPgmParam0 = 0;
                    return NiAbs(fC + 2 * fE + fF);
                }
                else if (-fTmp >= fA)
                {
                    fPgmParam0 = 1;
                    return NiAbs(fA + fC + fF + 2 * (fB + fD + fE));
                }
                else
                {
                    fPgmParam0 = -fTmp / fA;
                    return NiAbs(fTmp * fPgmParam0 + fC + 2 * fE + fF);
                }
            }
        }
        else
        {
            if (fPgmParam1 < 0)  // region 8
            {
                if ( - fD < fA)
                {
                    fPgmParam1 = 0;
                    if (fD >= 0)
                    {
                        fPgmParam0 = 0;
                        return NiAbs(fF);
                    }
                    else
                    {
                        fPgmParam0 = -fD / fA;
                        return NiAbs(fD * fPgmParam0 + fF);
                    }
                }
                else
                {
                    fPgmParam0 = 1;
                    fTmp = fB + fE;
                    if (fTmp >= 0)
                    {
                        fPgmParam1 = 0;
                        return NiAbs(fA + 2 * fD + fF);
                    }
                    else if (-fTmp >= fC)
                    {
                        fPgmParam1 = 1;
                        return NiAbs(fA + fC + fF + 2 * (fB + fD + fE));
                    }
                    else
                    {
                        fPgmParam1 = -fTmp / fC;
                        return NiAbs(fTmp * fPgmParam1 + fA + 2 * fD + fF);
                    }
                }
            }
            else if (fPgmParam1 <= fDet)  // region 1
            {
                fPgmParam0 = 1;
                fTmp = fB + fE;
                if (fTmp >= 0)
                {
                    fPgmParam1 = 0;
                    return NiAbs(fA + 2 * fD + fF);
                }
                else if (-fTmp >= fC)
                {
                    fPgmParam1 = 1;
                    return NiAbs(fA + fC + fF + 2 * (fB + fD + fE));
                }
                else
                {
                    fPgmParam1 = -fTmp / fC;
                    return NiAbs(fTmp * fPgmParam1 + fA + 2 * fD + fF);
                }
            }
            else  // region 2
            {
                fTmp = fB + fD;
                if (-fTmp < fA)
                {
                    fPgmParam1 = 1;
                    if (fTmp >= 0)
                    {
                        fPgmParam0 = 0;
                        return NiAbs(fC + 2 * fE + fF);
                    }
                    else
                    {
                        fPgmParam0 = -fTmp / fA;
                        return NiAbs(fTmp * fPgmParam0 + fC + 2 * fE + fF);
                    }
                }
                else
                {
                    fPgmParam0 = 1;
                    fTmp = fB + fE;
                    if (fTmp >= 0)
                    {
                        fPgmParam1 = 0;
                        return NiAbs(fA + 2 * fD + fF);
                    }
                    else if (-fTmp >= fC)
                    {
                        fPgmParam1 = 1;
                        return NiAbs(fA + fC + fF + 2 * (fB + fD + fE));
                    }
                    else
                    {
                        fPgmParam1 = -fTmp / fC;
                        return NiAbs(fTmp * fPgmParam1 + fA + 2 * fD + fF);
                    }
                }
            }
        }
    }
    else
    {
        // parallelogram is degenerate (a line segment)
        NiSegment kSeg;
        kSeg.m_kOrigin = kPgm.m_kOrigin;

        if (fA >= fC)
        {
            kSeg.m_kDirection = kPgm.m_kEdge0;
            fPgmParam1 = 0.0f;
            return NiSqrDistance::Compute(kPnt, kSeg, fPgmParam0);
        }
        else
        {
            kSeg.m_kDirection = kPgm.m_kEdge1;
            fPgmParam0 = 0.0f;
            return NiSqrDistance::Compute(kPnt, kSeg, fPgmParam1);
        }
    }
}
//--------------------------------------------------------------------------- 

