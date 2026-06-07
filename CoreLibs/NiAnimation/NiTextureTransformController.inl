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
inline NiTexturingProperty::Map* NiTextureTransformController::GetMap()
{
    return m_pkMap;
}
//---------------------------------------------------------------------------
inline NiTextureTransformController::TransformMember 
    NiTextureTransformController::GetAffectedMember()
{
    return m_eMember;
}
//---------------------------------------------------------------------------
inline void NiTextureTransformController::SetAffectedMember(
    TransformMember eMember)
{
    m_eMember = eMember;
    ResetCtlrID();
}
//---------------------------------------------------------------------------
inline bool NiTextureTransformController::SetInternalMap()
{
    // Check if we are already set
    if (m_pkMap)
        return true;

    // Must have a target to set the internal map
    if (m_pkTarget == NULL)
        return false;

    if (m_bShaderMap)
    {
        m_pkMap = ((NiTexturingProperty*)m_pkTarget)->GetShaderMap(
            m_uiMapIndex);
    }
    else
    {
        m_pkMap = ((NiTexturingProperty*)m_pkTarget)->GetMaps().GetAt(
            m_uiMapIndex);
    }

    if (m_pkMap == NULL)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiTextureTransformController::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    return NiIsKindOf(NiTexturingProperty, pkTarget);
}
//---------------------------------------------------------------------------
