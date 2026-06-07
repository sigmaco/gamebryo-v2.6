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
float NiSqrDistance::Compute(const NiPoint3& kPnt, const NiTrigon& kTri, 
    float& fTriParam0, float& fTriParam1)
{
    NiPoint3 kDiff = kTri.m_kOrigin - kPnt;
    float fA = kTri.m_kEdge0.Dot(kTri.m_kEdge0);
    float fB = kTri.m_kEdge0.Dot(kTri.m_kEdge1);
    float fC = kTri.m_kEdge1.Dot(kTri.m_kEdge1);
    float fD = kDiff.Dot(kTri.m_kEdge0);
    float fE = kDiff.Dot(kTri.m_kEdge1);
    float fF = kDiff.Dot(kDiff);
    float fDet = NiAbs(fA * fC - fB * fB);  // = |Cross(e0, e1)|^2 >= 0
    // fA-2*fB+fC = Dot(e0, e0)-2*Dot(e0, e1)+Dot(e1, e1) = |e0-e1|^2 > 0

    fTriParam0 = fB * fE - fC * fD;
    fTriParam1 = fB * fD - fA * fE;

    if (fTriParam0 + fTriParam1 <= fDet)
    {
        if (fTriParam0 < 0.0f)
        {
            if (fTriParam1 < 0.0f)  // region 4
            {
                if (fD < 0.0f)
                {
                    fTriParam1 = 0.0f;
                    if (-fD >= fA)
                    {
                        fTriParam0 = 1.0f;
                        return NiAbs(fA + 2.0f * fD + fF);
                    }
                    else
                    {
                        fTriParam0 = -fD / fA;
                        return NiAbs(fD * fTriParam0 + fF);
                    }
                }
                else
                {
                    fTriParam0 = 0.0f;
                    if (fE >= 0.0f)
                    {
                        fTriParam1 = 0.0f;
                        return NiAbs(fF);
                    }
                    else if (-fE >= fC)
                    {
                        fTriParam1 = 1.0f;
                        return NiAbs(fC + 2.0f * fE + fF);
                    }
                    else
                    {
                        fTriParam1 = -fE / fC;
                        return NiAbs(fE * fTriParam1 + fF);
                    }
                }
            }
            else  // region 3
            {
                fTriParam0 = 0.0f;
                if (fE >= 0.0f)
                {
                    fTriParam1 = 0.0f;
                    return NiAbs(fF);
                }
                else if (-fE >= fC)
                {
                    fTriParam1 = 1.0f;
                    return NiAbs(fC + 2.0f * fE + fF);
                }
                else
                {
                    fTriParam1 = -fE / fC;
                    return NiAbs(fE * fTriParam1 + fF);
                }
            }
        }
        else if (fTriParam1 < 0.0f)  // region 5
        {
            fTriParam1 = 0.0f;
            if (fD >= 0.0f)
            {
                fTriParam0 = 0.0f;
                return NiAbs(fF);
            }
            else if (-fD >= fA)
            {
                fTriParam0 = 1.0f;
                return NiAbs(fA + 2.0f * fD + fF);
            }
            else
            {
                fTriParam0 = -fD / fA;
                return NiAbs(fD * fTriParam0 + fF);
            }
        }
        else  // region 0
        {
            // minimum at interior point
            float fInvDet = 1.0f / fDet;
            fTriParam0 *= fInvDet;
            fTriParam1 *= fInvDet;
            return NiAbs(fTriParam0 * (fA * fTriParam0 + fB * fTriParam1 
                + 2 * fD) + fTriParam1 * (fB * fTriParam0 + fC * fTriParam1
                + 2 * fE) + fF);
        }
    }
    else
    {
        float fTmp0;
        float fTmp1;
        float fNumer;
        float fDenom;

        if (fTriParam0 < 0.0f)  // region 2
        {
            fTmp0 = fB + fD;
            fTmp1 = fC + fE;
            if (fTmp1 > fTmp0)
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA - 2.0f * fB + fC;
                if (fNumer >= fDenom)
                {
                    fTriParam0 = 1.0f;
                    fTriParam1 = 0.0f;
                    return NiAbs(fA + 2.0f * fD + fF);
                }
                else
                {
                    fTriParam0 = fNumer / fDenom;
                    fTriParam1 = 1.0f - fTriParam0;
                    return NiAbs(fC + 2.0f * fE + fF - fNumer * fTriParam0);
                }
            }
            else
            {
                fTriParam0 = 0.0f;
                if (fTmp1 <= 0.0f)
                {
                    fTriParam1 = 1.0f;
                    return NiAbs(fC + 2.0f * fE + fF);
                }
                else if (fE >= 0.0f)
                {
                    fTriParam1 = 0.0f;
                    return NiAbs(fF);
                }
                else
                {
                    fTriParam1 = -fE / fC;
                    return NiAbs(fE * fTriParam1 + fF);
                }
            }
        }
        else if (fTriParam1 < 0.0f)  // region 6
        {
            fTmp0 = fB + fE;
            fTmp1 = fA + fD;
            if (fTmp1 > fTmp0)
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA - 2.0f * fB + fC;
                if (fNumer >= fDenom)
                {
                    fTriParam1 = 1.0f;
                    fTriParam0 = 0.0f;
                    return NiAbs(fC + 2.0f * fE + fF);
                }
                else
                {
                    fTriParam1 = fNumer / fDenom;
                    fTriParam0 = 1.0f - fTriParam1;
                    return NiAbs(fA + 2.0f * fD + fF - fNumer * fTriParam1);
                }
            }
            else
            {
                fTriParam1 = 0.0f;
                if (fTmp1 <= 0.0f)
                {
                    fTriParam0 = 1.0f;
                    return NiAbs(fA + 2.0f * fD + fF);
                }
                else if (fD >= 0.0f)
                {
                    fTriParam0 = 0.0f;
                    return NiAbs(fF);
                }
                else
                {
                    fTriParam0 = -fD / fA;
                    return NiAbs(fD * fTriParam0 + fF);
                }
            }
        }
        else  // region 1
        {
            fNumer = (fC + fE) - (fB + fD);
            if (fNumer <= 0)
            {
                fTriParam0 = 0.0f;
                fTriParam1 = 1.0f;
                return NiAbs(fC + 2.0f * fE + fF);
            }
            else
            {
                fDenom = fA - 2.0f * fB + fC;
                if (fNumer >= fDenom)
                {
                    fTriParam0 = 1.0f;
                    fTriParam1 = 0.0f;
                    return NiAbs(fA + 2.0f * fD + fF);
                }
                else
                {
                    fTriParam0 = fNumer / fDenom;
                    fTriParam1 = 1.0f - fTriParam0;
                    return NiAbs(fC + 2.0f * fE + fF - fNumer * fTriParam0);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
