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
inline NiTransform NiTransform::operator*(const NiTransform &xform) const
{
    NiTransform res;

    res.m_fScale = m_fScale * xform.m_fScale;
    res.m_Rotate = m_Rotate * xform.m_Rotate;
    res.m_Translate = m_Translate + m_fScale * (m_Rotate * xform.m_Translate);

    return res;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiTransform::operator*(const NiPoint3 &kPoint) const
{
    return (((m_Rotate * kPoint) * m_fScale) + m_Translate);
}
