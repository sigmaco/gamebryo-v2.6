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
float NiSqrDistance::Compute(const NiSegment& kSeg0, 
    const NiSegment& kSeg1, float& fSeg0Param, float& fSeg1Param)
{
    const float fTolerance = 1e-06f;
    float fTmp;

    NiPoint3 kDiff = kSeg0.m_kOrigin - kSeg1.m_kOrigin;
    float fA = kSeg0.m_kDirection.Dot(kSeg0.m_kDirection);
    float fB = -(kSeg0.m_kDirection.Dot(kSeg1.m_kDirection));
    float fC = kSeg1.m_kDirection.Dot(kSeg1.m_kDirection);
    float fD = kSeg0.m_kDirection.Dot(kDiff);
    float fE = -(kSeg1.m_kDirection.Dot(kDiff));
    float fF = kDiff.Dot(kDiff);

    // fA*fC-fB*fB = |Cross(Dir0, Dir1)|^2 >= 0
    float fDet = NiAbs(fA * fC - fB * fB); 

    if (kSeg0.m_kDirection.Cross(kSeg1.m_kDirection).SqrLength() > fTolerance)
    {
        // line segments are not parallel

        // Use of these temp variables avoids LHS penalties on Xbox 360.
        // Do not remove.
        float fTempParam0 = fB * fE - fC * fD;
        float fTempParam1 = fB * fD - fA * fE;
        fSeg0Param = fTempParam0;
        fSeg1Param = fTempParam1;
        
        if (fTempParam0 >= 0)
        {
            if (fTempParam0 <= fDet)
            {
                if (fSeg1Param >= 0)
                {
                    if (fSeg1Param <= fDet)  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        float fInvDet = 1.0f / fDet;
                        fSeg0Param *= fInvDet;
                        fSeg1Param *= fInvDet;
                        return NiAbs(fSeg0Param * (fA * fSeg0Param + fB 
                            * fSeg1Param + 2 * fD) + fSeg1Param * (fB 
                            * fSeg0Param + fC * fSeg1Param + 2 * fE) + fF);
                    }
                    else  // region 3 (side)
                    {
                        fSeg1Param = 1;
                        fTmp = fB + fD;
                        if (fTmp >= 0)
                        {
                            fSeg0Param = 0;
                            return NiAbs(fC + 2 * fE + fF);
                        }
                        else if (-fTmp >= fA)
                        {
                            fSeg0Param = 1;
                            return NiAbs(fA + fC + fF + 2 * (fE + fTmp));
                        }
                        else
                        {
                            fSeg0Param = -fTmp / fA;
                            return NiAbs(fTmp * fSeg0Param + fC + 2 * fE 
                                + fF);
                        }
                    }
                }
                else  // region 7 (side)
                {
                    fSeg1Param = 0;
                    if (fD >= 0)
                    {
                        fSeg0Param = 0;
                        return NiAbs(fF);
                    }
                    else if (-fD >= fA)
                    {
                        fSeg0Param = 1;
                        return NiAbs(fA + 2 * fD + fF);
                    }
                    else
                    {
                        fSeg0Param = -fD / fA;
                        return NiAbs(fD * fSeg0Param + fF);
                    }
                }
            }
            else
            {
                if (fSeg1Param >= 0)
                {
                    if (fSeg1Param <= fDet)  // region 1 (side)
                    {
                        fSeg0Param = 1;
                        fTmp = fB + fE;
                        if (fTmp >= 0)
                        {
                            fSeg1Param = 0;
                            return NiAbs(fA + 2 * fD + fF);
                        }
                        else if (-fTmp >= fC)
                        {
                            fSeg1Param = 1;
                            return NiAbs(fA + fC + fF + 2 * (fD + fTmp));
                        }
                        else
                        {
                            fSeg1Param = -fTmp / fC;
                            return NiAbs(fTmp * fSeg1Param + fA + 2 * fD 
                                + fF);
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fTmp = fB + fD;
                        if (-fTmp <= fA)
                        {
                            fSeg1Param = 1;
                            if (fTmp >= 0)
                            {
                                fSeg0Param = 0;
                                return NiAbs(fC + 2 * fE + fF);
                            }
                            else
                            {
                                 fSeg0Param = -fTmp / fA;
                                 return NiAbs(fTmp * fSeg0Param + fC + 2 
                                     * fE + fF);
                            }
                        }
                        else
                        {
                            fSeg0Param = 1;
                            fTmp = fB + fE;
                            if (-fTmp >= fC)
                            {
                                fSeg1Param = 1;
                                return NiAbs(fA + fC + fF + 2 * (fD + fTmp));
                            }
                            else if (fTmp > 0)
                            {
                                fSeg1Param = 0;
                                return NiAbs(fA + 2 * fD + fF);
                            }
                            else
                            {
                                fSeg1Param = -fTmp / fC;
                                return NiAbs(fTmp * fSeg1Param + fA + 2 * fD 
                                    + fF);
                            }
                        }
                    }
                }
                else  // region 8 (corner)
                {
                    if (-fD < fA)
                    {
                        fSeg1Param = 0;
                        if (fD >= 0)
                        {
                            fSeg0Param = 0;
                            return NiAbs(fF);
                        }
                        else
                        {
                            fSeg0Param = -fD / fA;
                            return NiAbs(fD * fSeg0Param + fF);
                        }
                    }
                    else
                    {
                        fSeg0Param = 1;
                        fTmp = fB + fE;
                        if (fTmp >= 0)
                        {
                            fSeg1Param = 0;
                            return NiAbs(fA + 2 * fD + fF);
                        }
                        else if (-fTmp >= fC)
                        {
                            fSeg1Param = 1;
                            return NiAbs(fA + fC + fF + 2 * (fD + fTmp));
                        }
                        else
                        {
                            fSeg1Param = -fTmp / fC;
                            return NiAbs(fTmp * fSeg1Param + fA + 2 * fD 
                                + fF);
                        }
                    }
                }
            }
        }
        else 
        {
            if (fTempParam1 >= 0)
            {
                if (fTempParam1 <= fDet)  // region 5 (side)
                {
                    fSeg0Param = 0;
                    if (fE >= 0)
                    {
                        fSeg1Param = 0;
                        return NiAbs(fF);
                    }
                    else if (-fE >= fC)
                    {
                        fSeg1Param = 1;
                        return NiAbs(fC + 2 * fE + fF);
                    }
                    else
                    {
                        fSeg1Param = -fE / fC;
                        return NiAbs(fE * fSeg1Param + fF);
                    }
                }
                else  // region 4 (corner)
                {
                    fTmp = fB + fD;
                    if (fTmp < 0)
                    {
                        fSeg1Param = 1;
                        if (-fTmp >= fA)
                        {
                            fSeg0Param = 1;
                            return NiAbs(fA + fC + fF + 2 * (fE + fTmp));
                        }
                        else
                        {
                            fSeg0Param = -fTmp / fA;
                            return NiAbs(fTmp * fSeg0Param + fC + 2 * fE 
                                + fF);
                        }
                    }
                    else
                    {
                        fSeg0Param = 0;
                        fTmp = fC + fE;
                        if (fTmp <= 0)
                        {
                            fSeg1Param = 1;
                            return NiAbs(fC + 2 * fE + fF);
                        }
                        else if (fE >= 0)
                        {
                            fSeg1Param = 0;
                            return NiAbs(fF);
                        }
                        else
                        {
                            fSeg1Param = -fE / fC;
                            return NiAbs(fE * fSeg1Param + fF);
                        }
                    }
                }
            }
            else   // region 6 (corner)
            {
                if (fD < 0)
                {
                    fSeg1Param = 0;
                    if (-fD >= fA)
                    {
                        fSeg0Param = 1;
                        return NiAbs(fA + 2 * fD + fF);
                    }
                    else
                    {
                        fSeg0Param = -fD / fA;
                        return NiAbs(fD * fSeg0Param + fF);
                    }
                }
                else
                {
                    fSeg0Param = 0;
                    if (fE >= 0)
                    {
                        fSeg1Param = 0;
                        return NiAbs(fF);
                    }
                    else if (-fE >= fC)
                    {
                        fSeg1Param = 1;
                        return NiAbs(fC + 2 * fE + fF);
                    }
                    else
                    {
                        fSeg1Param = -fE / fC;
                        return NiAbs(fE * fSeg1Param + fF);
                    }
                }
            }
        }
    }
    else
    {
        // line segments are parallel
        if (fB > 0)
        {
            // direction vectors form an obtuse angle
            if (fD >= 0)
            {
                fSeg0Param = 0;
                fSeg1Param = 0;
                return NiAbs(fF);
            }
            else if (-fD <= fA)
            {
                fSeg0Param = -fD / fA;
                fSeg1Param = 0;
                return NiAbs(fD * fSeg0Param + fF);
            }
            else
            {
                fSeg0Param = 1;
                fTmp = fA + fD;
                if (-fTmp >= fB)
                {
                    fSeg1Param = 1;
                    return NiAbs(fA + fC + fF + 2 * (fB + fD + fE));
                }
                else
                {
                    fSeg1Param = -fTmp / fB;
                    return NiAbs(fA + 2 * fD + fF + fSeg1Param * (fC 
                        * fSeg1Param + 2 * (fB + fE)));
                }
            }
        }
        else
        {
            // direction vectors form an acute angle
            if (-fD >= fA)
            {
                fSeg0Param = 1;
                fSeg1Param = 0;
                return NiAbs(fA + 2 * fD + fF);
            }
            else if (fD <= 0)
            {
                fSeg0Param = -fD / fA;
                fSeg1Param = 0;
                return NiAbs(fD * fSeg0Param + fF);
            }
            else
            {
                fSeg0Param = 0;
                if (fD >= -fB)
                {
                    fSeg1Param = 1;
                    return NiAbs(fC + 2 * fE + fF);
                }
                else
                {
                    fSeg1Param = -fD / fB;
                    return NiAbs(fF + fSeg1Param * (2 * fE 
                        + fC * fSeg1Param));
                }
            }
        }
    }
}
//---------------------------------------------------------------------------

