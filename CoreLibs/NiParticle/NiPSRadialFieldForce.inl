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
inline float NiPSRadialFieldForce::GetRadialType() const
{
    return m_kInputData.m_fRadialType;
}
//---------------------------------------------------------------------------
inline void NiPSRadialFieldForce::SetRadialType(float fRadialType)
{
    // Ensure that the range is acceptable.
    NIASSERT(fRadialType >= 0.0f && fRadialType <= 1.0f);

    fRadialType = NiMax(0.0f, fRadialType);
    fRadialType = NiMin(1.0f, fRadialType);

    m_kInputData.m_fRadialType = fRadialType;
}
//---------------------------------------------------------------------------
