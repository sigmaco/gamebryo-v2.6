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

#include "MPoint3.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPoint3::MPoint3() : m_fX(0.0f), m_fY(0.0f), m_fZ(0.0f)
{
}
//---------------------------------------------------------------------------
MPoint3::MPoint3(float fX, float fY, float fZ) : m_fX(fX), m_fY(fY), m_fZ(fZ)
{
}
//---------------------------------------------------------------------------
MPoint3::MPoint3(const NiPoint3& kPoint3) : m_fX(kPoint3.x), m_fY(kPoint3.y),
    m_fZ(kPoint3.z)
{
}
//---------------------------------------------------------------------------
void MPoint3::SetData(const NiPoint3& kPoint3)
{
    m_fX = kPoint3.x;
    m_fY = kPoint3.y;
    m_fZ = kPoint3.z;
}
//---------------------------------------------------------------------------
void MPoint3::ToNiPoint3(NiPoint3& kPoint3)
{
    kPoint3 = NiPoint3(m_fX, m_fY, m_fZ);
}
//---------------------------------------------------------------------------
float MPoint3::get_X()
{
    return m_fX;
}
//---------------------------------------------------------------------------
void MPoint3::set_X(float fX)
{
    m_fX = fX;
}
//---------------------------------------------------------------------------
float MPoint3::get_Y()
{
    return m_fY;
}
//---------------------------------------------------------------------------
void MPoint3::set_Y(float fY)
{
    m_fY = fY;
}
//---------------------------------------------------------------------------
float MPoint3::get_Z()
{
    return m_fZ;
}
//---------------------------------------------------------------------------
void MPoint3::set_Z(float fZ)
{
    m_fZ = fZ;
}
//---------------------------------------------------------------------------
MPoint3* MPoint3::Add(MPoint3* pmA, MPoint3* pmB)
{
    NiPoint3 kPtA, kPtB;
    pmA->ToNiPoint3(kPtA);
    pmB->ToNiPoint3(kPtB);
    return new MPoint3(kPtA + kPtB);
}
//---------------------------------------------------------------------------
MPoint3* MPoint3::Subtract(MPoint3* pmA, MPoint3* pmB)
{
    NiPoint3 kPtA, kPtB;
    pmA->ToNiPoint3(kPtA);
    pmB->ToNiPoint3(kPtB);
    return new MPoint3(kPtA - kPtB);
}
//---------------------------------------------------------------------------
MPoint3* MPoint3::Multiply(float fScalar, MPoint3* pmB)
{
    NiPoint3 kPtB;
    pmB->ToNiPoint3(kPtB);
    return new MPoint3(fScalar * kPtB);
}
//---------------------------------------------------------------------------
float MPoint3::Dot(MPoint3* pmA, MPoint3* pmB)
{
    NiPoint3 kPtA, kPtB;
    pmA->ToNiPoint3(kPtA);
    pmB->ToNiPoint3(kPtB);
    return kPtA.Dot(kPtB);
}
//---------------------------------------------------------------------------
MPoint3* MPoint3::Cross(MPoint3* pmA, MPoint3* pmB)
{
    NiPoint3 kPtA, kPtB;
    pmA->ToNiPoint3(kPtA);
    pmB->ToNiPoint3(kPtB);
    return new MPoint3(kPtA.Cross(kPtB));
}
//---------------------------------------------------------------------------
MPoint3* MPoint3::Unitize(MPoint3* pmA)
{
    NiPoint3 kPtA;
    pmA->ToNiPoint3(kPtA);
    kPtA.Unitize();
    return new MPoint3(kPtA);
}
//---------------------------------------------------------------------------
float MPoint3::Length(MPoint3* pmA)
{
    NiPoint3 kPtA;
    pmA->ToNiPoint3(kPtA);
    return kPtA.Length();
}
//---------------------------------------------------------------------------
bool MPoint3::Equals(Object* pmObj)
{
    MPoint3* pmPoint3 = dynamic_cast<MPoint3*>(pmObj);
    if (pmPoint3 == NULL)
    {
        return false;
    }
    NiPoint3 kPoint3;
    pmPoint3->ToNiPoint3(kPoint3);
    NiPoint3 kThisPoint3;
    this->ToNiPoint3(kThisPoint3);
    return (kThisPoint3 == kPoint3);
}
//---------------------------------------------------------------------------
