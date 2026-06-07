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

#include "MPoint2.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPoint2::MPoint2() : m_fX(0.0f), m_fY(0.0f)
{
}
//---------------------------------------------------------------------------
MPoint2::MPoint2(float fX, float fY) : m_fX(fX), m_fY(fY)
{
}
//---------------------------------------------------------------------------
MPoint2::MPoint2(const NiPoint2& kPoint2) : m_fX(kPoint2.x), m_fY(kPoint2.y)
{
}
//---------------------------------------------------------------------------
void MPoint2::SetData(const NiPoint2& kPoint2)
{
    m_fX = kPoint2.x;
    m_fY = kPoint2.y;
}
//---------------------------------------------------------------------------
void MPoint2::ToNiPoint2(NiPoint2& kPoint2)
{
    kPoint2 = NiPoint2(m_fX, m_fY);
}
//---------------------------------------------------------------------------
float MPoint2::get_X()
{
    return m_fX;
}
//---------------------------------------------------------------------------
void MPoint2::set_X(float fX)
{
    m_fX = fX;
}
//---------------------------------------------------------------------------
float MPoint2::get_Y()
{
    return m_fY;
}
//---------------------------------------------------------------------------
void MPoint2::set_Y(float fY)
{
    m_fY = fY;
}
//---------------------------------------------------------------------------
MPoint2* MPoint2::Add(MPoint2* pmA, MPoint2* pmB)
{
    return new MPoint2(pmA->X + pmB->X, pmA->Y + pmB->Y);
}
//---------------------------------------------------------------------------
MPoint2* MPoint2::Subtract(MPoint2* pmA, MPoint2* pmB)
{
    return new MPoint2(pmA->X - pmB->X, pmA->Y - pmB->Y);
}
//---------------------------------------------------------------------------
bool MPoint2::Equals(Object* pmObj)
{
    MPoint2* pmPoint2 = dynamic_cast<MPoint2*>(pmObj);
    if (pmPoint2 == NULL)
    {
        return false;
    }
    NiPoint2 kPoint2;
    pmPoint2->ToNiPoint2(kPoint2);
    NiPoint2 kThisPoint2;
    this->ToNiPoint2(kThisPoint2);
    return (kThisPoint2 == kPoint2);
}
//---------------------------------------------------------------------------
