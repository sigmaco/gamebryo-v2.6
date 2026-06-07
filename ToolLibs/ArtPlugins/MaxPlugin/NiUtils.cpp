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

#include "MaxImmerse.h"
#include "NiUtils.h"

static bool IsRotation(const NiMatrix3& kRot);

//---------------------------------------------------------------------------
void
MaxToNI(Matrix3 &max, NiMatrix3 &ni)
{
    Point3 p;

    p = max.GetRow(0);
    ni.SetCol(0,p.x,p.y,p.z);
    p = max.GetRow(1);
    ni.SetCol(1,p.x,p.y,p.z);
    p = max.GetRow(2);
    ni.SetCol(2,p.x,p.y,p.z);
}

//---------------------------------------------------------------------------
void
NIToMAX(NiMatrix3 &ni, Matrix3 &max)
{
    Point3 p;

    ni.GetCol(0,p.x,p.y,p.z);
    max.SetRow(0, p);

    ni.GetCol(1,p.x,p.y,p.z);
    max.SetRow(1, p);

    ni.GetCol(2,p.x,p.y,p.z);
    max.SetRow(2, p);
}

//---------------------------------------------------------------------------
void
Normalize(NiPoint3 *pAxis)
{
    float f;

    f = pAxis->x * pAxis->x +
        pAxis->y * pAxis->y +
        pAxis->z * pAxis->z;
    f = (float) sqrt(f);
    if (f > 0.0001f || f < -0.0001)
        f = 1.0f / f;
    else
        f = 0.0f;

    pAxis->x *= f;
    pAxis->y *= f;
    pAxis->z *= f;
}

//---------------------------------------------------------------------------
void SetFlags(ReferenceMaker *pRefMaker, int iStat)
{
    int i;

    for (i = 0; i < pRefMaker->NumRefs(); i++) 
    {
        ReferenceMaker *pChildRef = pRefMaker->GetReference(i);
        if (pChildRef) 
            SetFlags(pChildRef, iStat);
    }

    switch (iStat) 
    {
    case ND_CLEAR:
        pRefMaker->ClearAFlag(A_WORK1);
        break;
    case ND_SET:
        pRefMaker->SetAFlag(A_WORK1);
        break;
    }
}
//---------------------------------------------------------------------------
bool MaxToNI(Matrix3 &max, NiTransform &xform)
{
    // This function is used to convert bind-pose matrices (returned by the
    // function IPhysiqueExport::GetInitNodeTM) of skinned objects and bones
    // to Gamebryo transformations. Sometimes, we get bogus matrices from
    // MAX. Return value indicates whether the transformation seems to be 
    // valid.

    AffineParts parts;
    decomp_affine(max, &parts);

    if (NiAbs(parts.k.x) < 1e-10f ||
        NiAbs(parts.k.y) < 1e-10f ||
        NiAbs(parts.k.z) < 1e-10f ||
        NiAbs(parts.k.x) > 1e+10f ||
        NiAbs(parts.k.y) > 1e+10f ||
        NiAbs(parts.k.z) > 1e+10f )
    {
        return false;
    }

    NiQuaternion q;
    q.SetW(parts.q.w);
    q.SetX(-parts.q.x);
    q.SetY(-parts.q.y);
    q.SetZ(-parts.q.z);

    q.ToRotation(xform.m_Rotate);
    xform.m_Translate.x = parts.t.x;
    xform.m_Translate.y = parts.t.y;
    xform.m_Translate.z = parts.t.z;

    xform.m_fScale = 1.0f;

    return IsRotation(xform.m_Rotate);
}
//---------------------------------------------------------------------------
bool IsRotation(const NiMatrix3& kRot)
{
    // IsRotation: all columns have magnitude 1.0 and are perpendicular to
    // each other. Also make sure all entries are valid floats.

    const float fEpsilon = 0.01f;

    for (unsigned int i = 0; i < 3; i++)
    {
        NiPoint3 kCol0;

        kRot.GetCol(i, kCol0);
        float fDot = kCol0 * kCol0;
        float fDelta = NiAbs(fDot - 1.0f);
        if (fDelta > fEpsilon)
            return false;

        for (unsigned int j = 0; j < 3; j++)
        {
            if (_isnan(kRot.GetEntry(i, j)))
                return false;

            if (j > i)
            {
                NiPoint3 kCol1;
                
                kRot.GetCol(j, kCol1);
                fDot = NiAbs(kCol0 * kCol1);
                if (fDot > fEpsilon)
                    return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
