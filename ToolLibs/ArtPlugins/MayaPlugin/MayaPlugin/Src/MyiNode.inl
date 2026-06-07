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
inline NiNode *MyiNode::GetNode()
{
    if (m_spJointOrientation != 0)
        return m_spJointOrientation;

    return m_spNode;
}
//---------------------------------------------------------------------------
inline bool MyiNode::AnyOrientationsOrPivots(void)
{
    return (AnyOrientations() || AnyPivots() || AnyScalePivots());
}
//---------------------------------------------------------------------------
inline bool MyiNode::AnyPivots(void)
{
    if ( EquivalentToZero(m_afPivot[0], m_afPivot[1], m_afPivot[2]) &&
         EquivalentToZero(m_afPivotTranslation[0], m_afPivotTranslation[1], 
            m_afPivotTranslation[2]))
        return false;

    return true;
}
//---------------------------------------------------------------------------
inline bool MyiNode::AnyScalePivots(void)
{
    if ( EquivalentToZero(m_afScalePivot[0], m_afScalePivot[1], 
            m_afScalePivot[2]) &&
         EquivalentToZero(m_afScalePivotTranslation[0], 
            m_afScalePivotTranslation[1], m_afScalePivotTranslation[2]))
        return false;

    return true;
}
//---------------------------------------------------------------------------
inline bool MyiNode::AnyOrientations(void)
{
    return !EquivalentToZero(m_afOrientation);
}
//---------------------------------------------------------------------------
inline bool MyiNode::AnyRotateAxis(void)
{
    return !EquivalentToZero(m_afRotateAxis);
}
//---------------------------------------------------------------------------
