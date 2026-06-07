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
inline void NiD3DUtility::GetD3DFromNi(D3DMATRIX& kD3D,
    const NiTransform& kNi)
{
    GetD3DFromNi(kD3D, kNi.m_Rotate, kNi.m_Translate, kNi.m_fScale);
}
//---------------------------------------------------------------------------
inline void NiD3DUtility::GetD3DFromNi(D3DMATRIX& kD3D,
    const NiMatrix3& kNiRot, const NiPoint3& kNiTrans, float fNiScale)
{
    kD3D._11 = kNiRot.GetEntry(0, 0) * fNiScale;
    kD3D._12 = kNiRot.GetEntry(1, 0) * fNiScale;
    kD3D._13 = kNiRot.GetEntry(2, 0) * fNiScale;
    kD3D._14 = 0.0f;
    kD3D._21 = kNiRot.GetEntry(0, 1) * fNiScale;
    kD3D._22 = kNiRot.GetEntry(1, 1) * fNiScale;
    kD3D._23 = kNiRot.GetEntry(2, 1) * fNiScale;
    kD3D._24 = 0.0f;
    kD3D._31 = kNiRot.GetEntry(0, 2) * fNiScale;
    kD3D._32 = kNiRot.GetEntry(1, 2) * fNiScale;
    kD3D._33 = kNiRot.GetEntry(2, 2) * fNiScale;
    kD3D._34 = 0.0f;
    kD3D._41 = kNiTrans.x;
    kD3D._42 = kNiTrans.y;
    kD3D._43 = kNiTrans.z;
    kD3D._44 = 1.0f;
}
//---------------------------------------------------------------------------
inline void NiD3DUtility::GetD3DTransposeFromNi(D3DMATRIX& kD3D,
    const NiTransform& kNi)
{
    GetD3DTransposeFromNi(kD3D, kNi.m_Rotate, kNi.m_Translate, kNi.m_fScale);
}
//---------------------------------------------------------------------------
inline void NiD3DUtility::GetD3DTransposeFromNi(D3DMATRIX& kD3D,
    const NiMatrix3& kNiRot, const NiPoint3& kNiTrans,
    float fNiScale)
{
    kD3D._11 = kNiRot.GetEntry(0, 0) * fNiScale;
    kD3D._12 = kNiRot.GetEntry(0, 1) * fNiScale;
    kD3D._13 = kNiRot.GetEntry(0, 2) * fNiScale;
    kD3D._14 = kNiTrans.x;
    kD3D._21 = kNiRot.GetEntry(1, 0) * fNiScale;
    kD3D._22 = kNiRot.GetEntry(1, 1) * fNiScale;
    kD3D._23 = kNiRot.GetEntry(1, 2) * fNiScale;
    kD3D._24 = kNiTrans.y;
    kD3D._31 = kNiRot.GetEntry(2, 0) * fNiScale;
    kD3D._32 = kNiRot.GetEntry(2, 1) * fNiScale;
    kD3D._33 = kNiRot.GetEntry(2, 2) * fNiScale;
    kD3D._34 = kNiTrans.z;
    kD3D._41 = 0.0f;
    kD3D._42 = 0.0f;
    kD3D._43 = 0.0f;
    kD3D._44 = 1.0f;
}
//---------------------------------------------------------------------------
/*
static D3DXMATRIX* D3DXMatrixMultiply_NonAccelerated(
    D3DXMATRIX * NI_RESTRICT pkOut, 
    const D3DXMATRIX * NI_RESTRICT pkM1, 
    const D3DXMATRIX * NI_RESTRICT pkM2)
{
    // Similar to the D3DXMatrixMultiply command:
    //
    // The result represents the transformation 
    // M1 followed by the transformation M2
    //
    // This function is used when optimizations in the 
    // D3DXMatrixMultiply function result in incorrect
    // results. 

    NIASSERT(pkOut != pkM1);
    NIASSERT(pkOut != pkM2);
    NIASSERT(pkM1 != pkM2);

    const D3DMATRIX &a = *pkM2;
    const D3DMATRIX &b = *pkM1;
    D3DMATRIX &c = *pkOut;
    
    // The following code multiplies matrices as such: c = a * b
    // where elements 11, 12, 13, 14 are interpreted as a column
    c._11 = a._11*b._11 + a._21*b._12 + a._31*b._13 + a._41*b._14;
    c._12 = a._12*b._11 + a._22*b._12 + a._32*b._13 + a._42*b._14;
    c._13 = a._13*b._11 + a._23*b._12 + a._33*b._13 + a._43*b._14;
    c._14 = a._14*b._11 + a._24*b._12 + a._34*b._13 + a._44*b._14;
    c._21 = a._11*b._21 + a._21*b._22 + a._31*b._23 + a._41*b._24;
    c._22 = a._12*b._21 + a._22*b._22 + a._32*b._23 + a._42*b._24;
    c._23 = a._13*b._21 + a._23*b._22 + a._33*b._23 + a._43*b._24;
    c._24 = a._14*b._21 + a._24*b._22 + a._34*b._23 + a._44*b._24;
    c._31 = a._11*b._31 + a._21*b._32 + a._31*b._33 + a._41*b._34;
    c._32 = a._12*b._31 + a._22*b._32 + a._32*b._33 + a._42*b._34;
    c._33 = a._13*b._31 + a._23*b._32 + a._33*b._33 + a._43*b._34;
    c._34 = a._14*b._31 + a._24*b._32 + a._34*b._33 + a._44*b._34;
    c._41 = a._11*b._41 + a._21*b._42 + a._31*b._43 + a._41*b._44;
    c._42 = a._12*b._41 + a._22*b._42 + a._32*b._43 + a._42*b._44;
    c._43 = a._13*b._41 + a._23*b._42 + a._33*b._43 + a._43*b._44;
    c._44 = a._14*b._41 + a._24*b._42 + a._34*b._43 + a._44*b._44;
    
    return pkOut;
}
*/
//---------------------------------------------------------------------------
