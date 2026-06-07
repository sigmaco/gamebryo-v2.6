// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed excekPt in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

//---------------------------------------------------------------------------
inline bool NiIndex::operator== (const NiIndex& kPt) const
{
    return (x == kPt.x && y == kPt.y);
}
//---------------------------------------------------------------------------
inline NiIndex NiIndex::operator+ (const NiIndex& kPt) 
    const
{
    return NiIndex(x+kPt.x,y+kPt.y);
}
//---------------------------------------------------------------------------
inline NiIndex NiIndex::operator- (const NiIndex& kPt) 
    const
{
    return NiIndex(x-kPt.x,y-kPt.y);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiIndex::operator* (const NiIndex& kPt) const
{
    return x*kPt.x+y*kPt.y;
}
//---------------------------------------------------------------------------
inline NiIndex NiIndex::operator* (NiUInt32 uiScalar) const
{
    return NiIndex(uiScalar*x,uiScalar*y);
}
//---------------------------------------------------------------------------
inline NiIndex NiIndex::operator/ (NiUInt32 uiScalar) const
{
    return NiIndex(x/uiScalar,y/uiScalar);
}
//---------------------------------------------------------------------------
inline NiIndex operator* (NiUInt32 uiScalar, const NiIndex& kPt)
{
    return NiIndex(uiScalar*kPt.x,uiScalar*kPt.y);
}
//---------------------------------------------------------------------------
inline NiIndex& NiIndex::operator+= (const NiIndex& kPt)
{
    x += kPt.x;
    y += kPt.y;
    return *this;
}
//---------------------------------------------------------------------------
inline NiIndex& NiIndex::operator-= (const NiIndex& kPt)
{
    x -= kPt.x;
    y -= kPt.y;
    return *this;
}
//---------------------------------------------------------------------------
inline NiIndex& NiIndex::operator*= (NiUInt32 uiScalar)
{
    x *= uiScalar;
    y *= uiScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiIndex& NiIndex::operator/= (NiUInt32 uiScalar)
{
    x /= uiScalar;
    y /= uiScalar;
    return *this;
}
