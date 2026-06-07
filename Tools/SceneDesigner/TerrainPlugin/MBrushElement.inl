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

inline void MBrushElement::ScaleTowards(NiDeformablePoint *pkPoint, 
    float fValue, float fBoundary, float fScale)
{ 
    // Scales the falloff to the scaled value
    float fVertValue = GetValue(pkPoint);
    float fDesired = (fValue - fBoundary)*fScale + fBoundary;
    SetValue(pkPoint, (fDesired - fVertValue)*fScale + fVertValue);
}
//--------------------------------------------------------------------------

inline void MBrushElement::ScaleTowardsIgnorePoint(NiDeformablePoint *pkPoint,
    float fValue, float fBoundary, float fScale)
{
    // ignores any existing value in the falloff regions.
    SetValue(pkPoint, fBoundary + (fValue - fBoundary)*fScale);
}
//---------------------------------------------------------------------------
inline void MBrushElement::GetMinAndMax(float& fMin, float& fMax)
{
    fMin = m_fMinimumValue;
    fMax = m_fMaximumValue;
}
//---------------------------------------------------------------------------
inline float MBrushElement::GetSmoothedValue(NiTerrainInteractor* pkInteractor,
    NiDeformablePointSet* pkPointSet, const NiDeformablePoint* pkPoint)
{
    float fValue;
    pkInteractor->GetSmoothedValue(pkPointSet, pkPoint, fValue);
    return fValue;
}
//---------------------------------------------------------------------------
inline void MBrushElement::SetAssociatedOperation(MBrushOperation* pmOperation)
{
    m_pmAssociatedOperation = pmOperation;
}