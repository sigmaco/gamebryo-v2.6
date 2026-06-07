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

#include "MQuaternion.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MQuaternion::MQuaternion() : m_fW(0.0f), m_fX(0.0f), m_fY(0.0f), m_fZ(0.0f),
    m_fXAngle(0.0f), m_fYAngle(0.0f), m_fZAngle(0.0f),
    m_bRecomputeEulerAngles(true)
{
}
//---------------------------------------------------------------------------
MQuaternion::MQuaternion(const NiQuaternion& kQuaternion) : m_fW(0.0f),
    m_fX(0.0f), m_fY(0.0f), m_fZ(0.0f), m_fXAngle(0.0f), m_fYAngle(0.0f),
    m_fZAngle(0.0f), m_bRecomputeEulerAngles(true)
{
    SetData(kQuaternion);
}
//---------------------------------------------------------------------------
void MQuaternion::SetData(const NiQuaternion& kQuaternion)
{
    m_fW = kQuaternion.GetW();
    m_fX = kQuaternion.GetX();
    m_fY = kQuaternion.GetY();
    m_fZ = kQuaternion.GetZ();
    m_bRecomputeEulerAngles = true;
}
//---------------------------------------------------------------------------
void MQuaternion::ToNiQuaternion(NiQuaternion& kQuaternion)
{
    kQuaternion = NiQuaternion(m_fW, m_fX, m_fY, m_fZ);
}
//---------------------------------------------------------------------------
float MQuaternion::get_W()
{
    return m_fW;
}
//---------------------------------------------------------------------------
void MQuaternion::set_W(float fW)
{
    m_fW = fW;
}
//---------------------------------------------------------------------------
float MQuaternion::get_X()
{
    return m_fX;
}
//---------------------------------------------------------------------------
void MQuaternion::set_X(float fX)
{
    m_fX = fX;
}
//---------------------------------------------------------------------------
float MQuaternion::get_Y()
{
    return m_fY;
}
//---------------------------------------------------------------------------
void MQuaternion::set_Y(float fY)
{
    m_fY = fY;
}
//---------------------------------------------------------------------------
float MQuaternion::get_Z()
{
    return m_fZ;
}
//---------------------------------------------------------------------------
void MQuaternion::set_Z(float fZ)
{
    m_fZ = fZ;
}
//---------------------------------------------------------------------------
float MQuaternion::get_XAngle()
{
    if (m_bRecomputeEulerAngles)
    {
        ComputeEulerAnglesXYZ();
    }
    return m_fXAngle;
}
//---------------------------------------------------------------------------
float MQuaternion::get_YAngle()
{
    if (m_bRecomputeEulerAngles)
    {
        ComputeEulerAnglesXYZ();
    }
    return m_fYAngle;
}
//---------------------------------------------------------------------------
float MQuaternion::get_ZAngle()
{
    if (m_bRecomputeEulerAngles)
    {
        ComputeEulerAnglesXYZ();
    }
    return m_fZAngle;
}
//---------------------------------------------------------------------------
void MQuaternion::FromEulerAnglesXYZ(float fXAngle, float fYAngle,
    float fZAngle)
{
    NiQuaternion kQuaternion;
    ToNiQuaternion(kQuaternion);
    NiMatrix3 kMatrix3;
    kQuaternion.ToRotation(kMatrix3);
    kMatrix3.FromEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
    kQuaternion.FromRotation(kMatrix3);
    SetData(kQuaternion);
    m_fXAngle = fXAngle;
    m_fYAngle = fYAngle;
    m_fZAngle = fZAngle;
    m_bRecomputeEulerAngles = false;
}
//---------------------------------------------------------------------------
bool MQuaternion::Equals(Object* pmObj)
{
    MQuaternion* pmQuaternion = dynamic_cast<MQuaternion*>(pmObj);
    if (pmQuaternion == NULL)
    {
        return false;
    }
    NiQuaternion kQuaternion;
    pmQuaternion->ToNiQuaternion(kQuaternion);
    NiQuaternion kThisQuaternion;
    this->ToNiQuaternion(kThisQuaternion);
    return (kThisQuaternion == kQuaternion);
}
//---------------------------------------------------------------------------
void MQuaternion::ComputeEulerAnglesXYZ()
{
    NiQuaternion kQuaternion;
    ToNiQuaternion(kQuaternion);
    NiMatrix3 kMatrix3;
    kQuaternion.ToRotation(kMatrix3);
    float fXAngle, fYAngle, fZAngle;
    kMatrix3.ToEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
    m_fXAngle = fXAngle;
    m_fYAngle = fYAngle;
    m_fZAngle = fZAngle;
    m_bRecomputeEulerAngles = false;
}
//---------------------------------------------------------------------------
