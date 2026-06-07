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

#include "NiCollisionPCH.h"
#include "NiSqrDistance.h"

//---------------------------------------------------------------------------
static void Face (const NiBox& kBox, int i0, int i1, int i2, NiPoint3& rkPnt,
    const NiPoint3& rkDir, const NiPoint3& rkPmE, float& fLParam,
    float& rfSqrDistance)
{
    NiPoint3 kPpE;
    float fLSqr, fInv, fTmp, fParam, fT, fDelta;

    kPpE[i1] = rkPnt[i1] + kBox.m_afExtent[i1];
    kPpE[i2] = rkPnt[i2] + kBox.m_afExtent[i2];
    if ( rkDir[i0]*kPpE[i1] >= rkDir[i1]*rkPmE[i0] )
    {
        if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
        {
            // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
            rkPnt[i0] = kBox.m_afExtent[i0];
            fInv = ((float)1.0)/rkDir[i0];
            rkPnt[i1] -= rkDir[i1]*rkPmE[i0]*fInv;
            rkPnt[i2] -= rkDir[i2]*rkPmE[i0]*fInv;
            fLParam = -rkPmE[i0]*fInv;
        }
        else
        {
            // v[i1] >= -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i2]*kPpE[i2]);
            if ( fTmp <= ((float)2.0)*fLSqr*kBox.m_afExtent[i1] )
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i1]*rkDir[i1];
                fTmp = kPpE[i1] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
                    rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
                    kPpE[i2]*kPpE[i2] + fDelta*fParam;

                fLParam = fParam;
                rkPnt[i0] = kBox.m_afExtent[i0];
                rkPnt[i1] = fT - kBox.m_afExtent[i1];
                rkPnt[i2] = -kBox.m_afExtent[i2];
            }
            else
            {
                fLSqr += rkDir[i1]*rkDir[i1];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
                    rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] +
                    kPpE[i2]*kPpE[i2] + fDelta*fParam;

                fLParam = fParam;
                rkPnt[i0] = kBox.m_afExtent[i0];
                rkPnt[i1] = kBox.m_afExtent[i1];
                rkPnt[i2] = -kBox.m_afExtent[i2];
            }
        }
    }
    else
    {
        if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
        {
            // v[i1] < -e[i1], v[i2] >= -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i1]*kPpE[i1]);
            if ( fTmp <= ((float)2.0)*fLSqr*kBox.m_afExtent[i2] )
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i2]*rkDir[i2];
                fTmp = kPpE[i2] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                    rkDir[i2]*fTmp;
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                    fTmp*fTmp + fDelta*fParam;

                fLParam = fParam;
                rkPnt[i0] = kBox.m_afExtent[i0];
                rkPnt[i1] = -kBox.m_afExtent[i1];
                rkPnt[i2] = fT - kBox.m_afExtent[i2];
            }
            else
            {
                fLSqr += rkDir[i2]*rkDir[i2];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                    rkDir[i2]*rkPmE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                    rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                fLParam = fParam;
                rkPnt[i0] = kBox.m_afExtent[i0];
                rkPnt[i1] = -kBox.m_afExtent[i1];
                rkPnt[i2] = kBox.m_afExtent[i2];
            }
        }
        else
        {
            // v[i1] < -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0]+rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i2]*kPpE[i2]);
            if ( fTmp >= (float)0.0 )
            {
                // v[i1]-edge is closest
                if ( fTmp <= ((float)2.0)*fLSqr*kBox.m_afExtent[i1] )
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fTmp = kPpE[i1] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
                        rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
                        kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    fLParam = fParam;
                    rkPnt[i0] = kBox.m_afExtent[i0];
                    rkPnt[i1] = fT - kBox.m_afExtent[i1];
                    rkPnt[i2] = -kBox.m_afExtent[i2];
                }
                else
                {
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
                        rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1]
                        + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    fLParam = fParam;
                    rkPnt[i0] = kBox.m_afExtent[i0];
                    rkPnt[i1] = kBox.m_afExtent[i1];
                    rkPnt[i2] = -kBox.m_afExtent[i2];
                }
                return;
            }

            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i1]*kPpE[i1]);
            if ( fTmp >= (float)0.0 )
            {
                // v[i2]-edge is closest
                if ( fTmp <= ((float)2.0)*fLSqr*kBox.m_afExtent[i2] )
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fTmp = kPpE[i2] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                        rkDir[i2]*fTmp;
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                        fTmp*fTmp + fDelta*fParam;

                    fLParam = fParam;
                    rkPnt[i0] = kBox.m_afExtent[i0];
                    rkPnt[i1] = -kBox.m_afExtent[i1];
                    rkPnt[i2] = fT - kBox.m_afExtent[i2];
                }
                else
                {
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                        rkDir[i2]*rkPmE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                        rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                    fLParam = fParam;
                    rkPnt[i0] = kBox.m_afExtent[i0];
                    rkPnt[i1] = -kBox.m_afExtent[i1];
                    rkPnt[i2] = kBox.m_afExtent[i2];
                }
                return;
            }

            // (v[i1],v[i2])-corner is closest
            fLSqr += rkDir[i2]*rkDir[i2];
            fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                rkDir[i2]*kPpE[i2];
            fParam = -fDelta/fLSqr;
            rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                kPpE[i2]*kPpE[i2] + fDelta*fParam;

            fLParam = fParam;
            rkPnt[i0] = kBox.m_afExtent[i0];
            rkPnt[i1] = -kBox.m_afExtent[i1];
            rkPnt[i2] = -kBox.m_afExtent[i2];
        }
    }
}
//----------------------------------------------------------------------------
static void CaseNoZeros (const NiBox& kBox, NiPoint3& rkPnt,
    const NiPoint3& rkDir, float& fLParam, float& rfSqrDistance)
{
    NiPoint3 kPmE(
        rkPnt.x - kBox.m_afExtent[0],
        rkPnt.y - kBox.m_afExtent[1],
        rkPnt.z - kBox.m_afExtent[2]);

    float fProdDxPy = rkDir.x*kPmE.y;
    float fProdDyPx = rkDir.y*kPmE.x;
    float fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

    if ( fProdDyPx >= fProdDxPy )
    {
        fProdDzPx = rkDir.z*kPmE.x;
        fProdDxPz = rkDir.x*kPmE.z;
        if ( fProdDzPx >= fProdDxPz )
        {
            // line intersects x = e0
            Face(kBox,0,1,2,rkPnt,rkDir,kPmE,fLParam,rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(kBox,2,0,1,rkPnt,rkDir,kPmE,fLParam,rfSqrDistance);
        }
    }
    else
    {
        fProdDzPy = rkDir.z*kPmE.y;
        fProdDyPz = rkDir.y*kPmE.z;
        if ( fProdDzPy >= fProdDyPz )
        {
            // line intersects y = e1
            Face(kBox,1,2,0,rkPnt,rkDir,kPmE,fLParam,rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(kBox,2,0,1,rkPnt,rkDir,kPmE,fLParam,rfSqrDistance);
        }
    }
}
//----------------------------------------------------------------------------
static void Case0 (const NiBox& kBox, int i0, int i1, int i2, NiPoint3& rkPnt,
    const NiPoint3& rkDir, float& fLParam, float& rfSqrDistance)
{
    float fPmE0 = rkPnt[i0] - kBox.m_afExtent[i0];
    float fPmE1 = rkPnt[i1] - kBox.m_afExtent[i1];
    float fProd0 = rkDir[i1]*fPmE0;
    float fProd1 = rkDir[i0]*fPmE1;
    float fDelta, fInvLSqr, fInv;

    if ( fProd0 >= fProd1 )
    {
        // line intersects P[i0] = e[i0]
        rkPnt[i0] = kBox.m_afExtent[i0];

        float fPpE1 = rkPnt[i1] + kBox.m_afExtent[i1];
        fDelta = fProd0 - rkDir[i0]*fPpE1;
        if ( fDelta >= (float)0.0 )
        {
            fInvLSqr = ((float)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            rkPnt[i1] = -kBox.m_afExtent[i1];
            fLParam = -(rkDir[i0]*fPmE0+rkDir[i1]*fPpE1)*fInvLSqr;
        }
        else
        {
            fInv = ((float)1.0)/rkDir[i0];
            rkPnt[i1] -= fProd0*fInv;
            fLParam = -fPmE0*fInv;
        }
    }
    else
    {
        // line intersects P[i1] = e[i1]
        rkPnt[i1] = kBox.m_afExtent[i1];

        float fPpE0 = rkPnt[i0] + kBox.m_afExtent[i0];
        fDelta = fProd1 - rkDir[i1]*fPpE0;
        if ( fDelta >= (float)0.0 )
        {
            fInvLSqr = ((float)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            rkPnt[i0] = -kBox.m_afExtent[i0];
            fLParam = -(rkDir[i0]*fPpE0+rkDir[i1]*fPmE1)*fInvLSqr;
        }
        else
        {
            fInv = ((float)1.0)/rkDir[i1];
            rkPnt[i0] -= fProd1*fInv;
            fLParam = -fPmE1*fInv;
        }
    }

    if ( rkPnt[i2] < -kBox.m_afExtent[i2] )
    {
        fDelta = rkPnt[i2] + kBox.m_afExtent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = -kBox.m_afExtent[i2];
    }
    else if ( rkPnt[i2] > kBox.m_afExtent[i2] )
    {
        fDelta = rkPnt[i2] - kBox.m_afExtent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = kBox.m_afExtent[i2];
    }
}
//----------------------------------------------------------------------------
static void Case00 (const NiBox& kBox, int i0, int i1, int i2,
    NiPoint3& rkPnt, const NiPoint3& rkDir, float& fLParam,
    float& rfSqrDistance)
{
    float fDelta;

    fLParam = (kBox.m_afExtent[i0] - rkPnt[i0])/rkDir[i0];

    rkPnt[i0] = kBox.m_afExtent[i0];

    if ( rkPnt[i1] < -kBox.m_afExtent[i1] )
    {
        fDelta = rkPnt[i1] + kBox.m_afExtent[i1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = -kBox.m_afExtent[i1];
    }
    else if ( rkPnt[i1] > kBox.m_afExtent[i1] )
    {
        fDelta = rkPnt[i1] - kBox.m_afExtent[i1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = kBox.m_afExtent[i1];
    }

    if ( rkPnt[i2] < -kBox.m_afExtent[i2] )
    {
        fDelta = rkPnt[i2] + kBox.m_afExtent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = -kBox.m_afExtent[i2];
    }
    else if ( rkPnt[i2] > kBox.m_afExtent[i2] )
    {
        fDelta = rkPnt[i2] - kBox.m_afExtent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = kBox.m_afExtent[i2];
    }
}
//----------------------------------------------------------------------------
static void Case000 (const NiBox& kBox, NiPoint3& rkPnt, float& rfSqrDistance)
{
    float fDelta;

    if ( rkPnt.x < -kBox.m_afExtent[0] )
    {
        fDelta = rkPnt.x + kBox.m_afExtent[0];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.x = -kBox.m_afExtent[0];
    }
    else if ( rkPnt.x > kBox.m_afExtent[0] )
    {
        fDelta = rkPnt.x - kBox.m_afExtent[0];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.x = kBox.m_afExtent[0];
    }

    if ( rkPnt.y < -kBox.m_afExtent[1] )
    {
        fDelta = rkPnt.y + kBox.m_afExtent[1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.y = -kBox.m_afExtent[1];
    }
    else if ( rkPnt.y > kBox.m_afExtent[1] )
    {
        fDelta = rkPnt.y - kBox.m_afExtent[1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.y = kBox.m_afExtent[1];
    }

    if ( rkPnt.z < -kBox.m_afExtent[2] )
    {
        fDelta = rkPnt.z + kBox.m_afExtent[2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.z = -kBox.m_afExtent[2];
    }
    else if ( rkPnt.z > kBox.m_afExtent[2] )
    {
        fDelta = rkPnt.z - kBox.m_afExtent[2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.z = kBox.m_afExtent[2];
    }
}
//----------------------------------------------------------------------------
float NiSqrDistance::Compute(const NiSegment& kSeg, const NiBox& kBox,
    float& fSegParam, float& fBoxParam0, float& fBoxParam1,
    float& fBoxParam2)
{
    // compute coordinates of line in box coordinate system
    NiPoint3 kDiff = kSeg.m_kOrigin - kBox.m_kCenter;
    NiPoint3 kPnt(
        kDiff.Dot(kBox.m_akAxis[0]),
        kDiff.Dot(kBox.m_akAxis[1]),
        kDiff.Dot(kBox.m_akAxis[2]));
    NiPoint3 kDir(
        kSeg.m_kDirection.Dot(kBox.m_akAxis[0]),
        kSeg.m_kDirection.Dot(kBox.m_akAxis[1]),
        kSeg.m_kDirection.Dot(kBox.m_akAxis[2]));

    // Apply reflections so that direction vector has nonnegative components.
    bool bReflect[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        if ( kDir[i] < (float)0.0 )
        {
            kPnt[i] = -kPnt[i];
            kDir[i] = -kDir[i];
            bReflect[i] = true;
        }
        else
        {
            bReflect[i] = false;
        }
    }

    float fSqrDistance = (float)0.0;
    fSegParam = (float)0.0;  // parameter for closest point on line

    if ( kDir.x > (float)0.0 )
    {
        if ( kDir.y > (float)0.0 )
        {
            if ( kDir.z > (float)0.0 )  // (+,+,+)
                CaseNoZeros(kBox,kPnt,kDir,fSegParam,fSqrDistance);
            else  // (+,+,0)
                Case0(kBox,0,1,2,kPnt,kDir,fSegParam,fSqrDistance);
        }
        else
        {
            if ( kDir.z > (float)0.0 )  // (+,0,+)
                Case0(kBox,0,2,1,kPnt,kDir,fSegParam,fSqrDistance);
            else  // (+,0,0)
                Case00(kBox,0,1,2,kPnt,kDir,fSegParam,fSqrDistance);
        }
    }
    else
    {
        if ( kDir.y > (float)0.0 )
        {
            if ( kDir.z > (float)0.0 )  // (0,+,+)
                Case0(kBox,1,2,0,kPnt,kDir,fSegParam,fSqrDistance);
            else  // (0,+,0)
                Case00(kBox,1,0,2,kPnt,kDir,fSegParam,fSqrDistance);
        }
        else
        {
            if ( kDir.z > (float)0.0 )  // (0,0,+)
                Case00(kBox,2,0,1,kPnt,kDir,fSegParam,fSqrDistance);
            else  // (0,0,0)
                Case000(kBox,kPnt,fSqrDistance);
        }
    }

    if (0.0f <= fSegParam)
    {
        if (fSegParam <= 1.0f)
        {
            for (i = 0; i < 3; i++)
            {
                // undo the reflections applied previously
                if ( bReflect[i] )
                    kPnt[i] = -kPnt[i];
            }

            fBoxParam0 = kPnt[0];
            fBoxParam1 = kPnt[1];
            fBoxParam2 = kPnt[2];
        }
        else
        {
            fSegParam = 1.0f;
            fSqrDistance = NiSqrDistance::Compute(kSeg.m_kOrigin +
                kSeg.m_kDirection, kBox, fBoxParam0, fBoxParam1, fBoxParam2);
        }
    }
    else
    {
        fSegParam = 0.0f;
        fSqrDistance = NiSqrDistance::Compute(kSeg.m_kOrigin, kBox,
            fBoxParam0, fBoxParam1, fBoxParam2);
    }

    return fSqrDistance;
}
//---------------------------------------------------------------------------
