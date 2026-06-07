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
#include "SceneDesignerFrameworkPCH.h"

#include "MMatrix3.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MMatrix3::MMatrix3() : m_fXAngle(0.0f), m_fYAngle(0.0f), m_fZAngle(0.0f),
    m_bRecomputeEulerAngles(true)
{
    m_afData = new float __gc [9];
    for (int i = 0; i < 9; i++)
    {
        m_afData[i] = 0.0f;
    }
}
//---------------------------------------------------------------------------
MMatrix3::MMatrix3(const NiMatrix3& kMatrix3) : m_fXAngle(0.0f),
    m_fYAngle(0.0f), m_fZAngle(0.0f), m_bRecomputeEulerAngles(true)
{
    m_afData = new float __gc [9];
    SetData(kMatrix3);
}
//---------------------------------------------------------------------------
void MMatrix3::SetData(const NiMatrix3& kMatrix3)
{
    m_afData[0] = kMatrix3.GetEntry(0, 0);
    m_afData[1] = kMatrix3.GetEntry(1, 0);
    m_afData[2] = kMatrix3.GetEntry(2, 0);
    m_afData[3] = kMatrix3.GetEntry(0, 1);
    m_afData[4] = kMatrix3.GetEntry(1, 1);
    m_afData[5] = kMatrix3.GetEntry(2, 1);
    m_afData[6] = kMatrix3.GetEntry(0, 2);
    m_afData[7] = kMatrix3.GetEntry(1, 2);
    m_afData[8] = kMatrix3.GetEntry(2, 2);
    m_bRecomputeEulerAngles = true;
}
//---------------------------------------------------------------------------
void MMatrix3::ToNiMatrix3(NiMatrix3& kMatrix3)
{
    kMatrix3 = NiMatrix3(
        NiPoint3(m_afData[0], m_afData[1], m_afData[2]),
        NiPoint3(m_afData[3], m_afData[4], m_afData[5]),
        NiPoint3(m_afData[6], m_afData[7], m_afData[8]));
}
//---------------------------------------------------------------------------
float MMatrix3::get_XAngle()
{
    if (m_bRecomputeEulerAngles)
    {
        ComputeEulerAnglesXYZ();
    }
    return m_fXAngle;
}
//---------------------------------------------------------------------------
float MMatrix3::get_YAngle()
{
    if (m_bRecomputeEulerAngles)
    {
        ComputeEulerAnglesXYZ();
    }
    return m_fYAngle;
}
//---------------------------------------------------------------------------
float MMatrix3::get_ZAngle()
{
    if (m_bRecomputeEulerAngles)
    {
        ComputeEulerAnglesXYZ();
    }
    return m_fZAngle;
}
//---------------------------------------------------------------------------
void MMatrix3::FromEulerAnglesXYZ(float fXAngle, float fYAngle, float fZAngle)
{
    NiMatrix3 kMatrix3;
    ToNiMatrix3(kMatrix3);
    kMatrix3.FromEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
    SetData(kMatrix3);
    m_fXAngle = fXAngle;
    m_fYAngle = fYAngle;
    m_fZAngle = fZAngle;
    m_bRecomputeEulerAngles = false;
}
//---------------------------------------------------------------------------
bool MMatrix3::Equals(Object* pmObj)
{
    MMatrix3* pmMatrix = dynamic_cast<MMatrix3*>(pmObj);
    if (pmMatrix == NULL)
    {
        return false;
    }
    NiMatrix3 kMatrix3;
    pmMatrix->ToNiMatrix3(kMatrix3);
    NiMatrix3 kThisMatrix3;
    this->ToNiMatrix3(kThisMatrix3);
    return (kThisMatrix3 == kMatrix3);
}
//---------------------------------------------------------------------------
void MMatrix3::ComputeEulerAnglesXYZ()
{
    NiMatrix3 kMatrix3;
    ToNiMatrix3(kMatrix3);
    float fXAngle, fYAngle, fZAngle;
    kMatrix3.ToEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
    m_fXAngle = fXAngle;
    m_fYAngle = fYAngle;
    m_fZAngle = fZAngle;
    m_bRecomputeEulerAngles = false;
}
//---------------------------------------------------------------------------
