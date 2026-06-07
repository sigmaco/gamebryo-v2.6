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
//  NiAlphaAccumulator inline functions
//---------------------------------------------------------------------------
#include "NiAlphaProperty.h"

//---------------------------------------------------------------------------
inline void NiAlphaAccumulator::SetObserveNoSortHint(bool bObserve)
{
    m_bObserveNoSortHint = bObserve;
}
//---------------------------------------------------------------------------
inline bool NiAlphaAccumulator::GetObserveNoSortHint() const
{
    return m_bObserveNoSortHint;
}
//---------------------------------------------------------------------------
inline void NiAlphaAccumulator::SetSortByClosestPoint(bool bClosest)
{
    m_bSortByClosestPoint = bClosest;
}
//---------------------------------------------------------------------------
inline bool NiAlphaAccumulator::GetSortByClosestPoint() const
{
    return m_bSortByClosestPoint;
}
//---------------------------------------------------------------------------
