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

//---------------------------------------------------------------------------
inline NiMatrix3x4::NiMatrix3x4(){}
//---------------------------------------------------------------------------
inline NiMatrix3x4::NiMatrix3x4(NiTransform& kTransform)
{
    *this = kTransform;    
}
//---------------------------------------------------------------------------
inline NiMatrix3x4& NiMatrix3x4::operator =(const NiMatrix3x4& kMatrix)
{
    m_kEntry[0] = kMatrix.m_kEntry[0];
    m_kEntry[1] = kMatrix.m_kEntry[1];
    m_kEntry[2] = kMatrix.m_kEntry[2];
    return *this;
}
//---------------------------------------------------------------------------
inline NiMatrix3x4 NiMatrix3x4::operator* (float fScalar) const
{
    NiMatrix3x4 result;
    result.m_kEntry[0] = m_kEntry[0] * fScalar;
    result.m_kEntry[1] = m_kEntry[1] * fScalar;
    result.m_kEntry[2] = m_kEntry[2] * fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiMatrix3x4::operator* (const NiPoint3& pt) const
{
    NiPoint3 kPoint(
        m_kEntry[0][0]*pt.x + m_kEntry[0][1]*pt.y + m_kEntry[0][2]*pt.z,
        m_kEntry[1][0]*pt.x + m_kEntry[1][1]*pt.y + m_kEntry[1][2]*pt.z,
        m_kEntry[2][0]*pt.x + m_kEntry[2][1]*pt.y + m_kEntry[2][2]*pt.z);

    kPoint.x += m_kEntry[0][3];
    kPoint.y += m_kEntry[1][3];
    kPoint.z += m_kEntry[2][3];
    return kPoint;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiMatrix3x4::ScaleAndRotate(const NiPoint3& pt) const
{
    return NiPoint3(
        m_kEntry[0][0]*pt.x + m_kEntry[0][1]*pt.y + m_kEntry[0][2]*pt.z,
        m_kEntry[1][0]*pt.x + m_kEntry[1][1]*pt.y + m_kEntry[1][2]*pt.z,
        m_kEntry[2][0]*pt.x + m_kEntry[2][1]*pt.y + m_kEntry[2][2]*pt.z);
}
//---------------------------------------------------------------------------
inline NiPoint2 NiMatrix3x4::CalculateST(const register NiPoint3& pt) const
{
    return NiPoint2(
        m_kEntry[0][0]*pt.x + m_kEntry[0][1]*pt.y + m_kEntry[0][2]*pt.z + m_kEntry[0][3],
        m_kEntry[1][0]*pt.x + m_kEntry[1][1]*pt.y + m_kEntry[1][2]*pt.z + m_kEntry[1][3]);
}
//---------------------------------------------------------------------------
inline NiPoint4 NiMatrix3x4::operator* (const NiPoint4& pt) const
{
    return NiPoint4(
        m_kEntry[0][0] * pt[0] + 
        m_kEntry[0][1] * pt[1] + 
        m_kEntry[0][2] * pt[2] + 
        m_kEntry[0][3] * pt[3],

        m_kEntry[1][0] * pt[0] + 
        m_kEntry[1][1] * pt[1] + 
        m_kEntry[1][2] * pt[2] + 
        m_kEntry[1][3] * pt[3],

        m_kEntry[2][0] * pt[0] + 
        m_kEntry[2][1] * pt[1] + 
        m_kEntry[2][2] * pt[2] + 
        m_kEntry[2][3] * pt[3],

        pt[3]);

}
//---------------------------------------------------------------------------
inline NiMatrix3x4& NiMatrix3x4::operator =(const NiTransform& kTransform)
{
    float fScale = kTransform.m_fScale;
    m_kEntry[0].Set(
        kTransform.m_Rotate.m_pEntry[0][0] * fScale,
        kTransform.m_Rotate.m_pEntry[0][1] * fScale,
        kTransform.m_Rotate.m_pEntry[0][2] * fScale,
        kTransform.m_Translate.x);

    m_kEntry[1].Set(
        kTransform.m_Rotate.m_pEntry[1][0] * fScale,
        kTransform.m_Rotate.m_pEntry[1][1] * fScale,
        kTransform.m_Rotate.m_pEntry[1][2] * fScale,
        kTransform.m_Translate.y);               

    m_kEntry[2].Set(
        kTransform.m_Rotate.m_pEntry[2][0] * fScale,
        kTransform.m_Rotate.m_pEntry[2][1] * fScale,
        kTransform.m_Rotate.m_pEntry[2][2] * fScale,
        kTransform.m_Translate.z);
    return *this;
}

//---------------------------------------------------------------------------
inline NiMatrix3x4& NiMatrix3x4::operator+=(const NiMatrix3x4& kMatrix)
{
    m_kEntry[0] = m_kEntry[0] + kMatrix.m_kEntry[0];
    m_kEntry[1] = m_kEntry[1] + kMatrix.m_kEntry[1];
    m_kEntry[2] = m_kEntry[2] + kMatrix.m_kEntry[2];
    return *this;
}
//---------------------------------------------------------------------------
inline const NiMatrix3x4 NiMatrix3x4::Identity()
{
    NiMatrix3x4 kMatrix;
    kMatrix.MakeIdentity();
    return kMatrix;
}
//---------------------------------------------------------------------------
inline void NiMatrix3x4::MakeIdentity()
{
    m_kEntry[0].SetX(1.0f);
    m_kEntry[0].SetY(0.0f);
    m_kEntry[0].SetZ(0.0f);
    m_kEntry[0].SetW(0.0f);
    m_kEntry[1].SetX(0.0f);
    m_kEntry[1].SetY(1.0f);
    m_kEntry[1].SetZ(0.0f);
    m_kEntry[1].SetW(0.0f);
    m_kEntry[2].SetX(0.0f);
    m_kEntry[2].SetY(0.0f);
    m_kEntry[2].SetZ(1.0f);
    m_kEntry[2].SetW(0.0f);
}
//---------------------------------------------------------------------------