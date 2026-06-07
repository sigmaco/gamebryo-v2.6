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
inline NiInterpolator* NiPhysXInterpolatorDest::GetTarget() const
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline NiNode* NiPhysXInterpolatorDest::GetSceneGraphParent() const
{
    return m_pkSceneParent;
}
//---------------------------------------------------------------------------
inline void NiPhysXInterpolatorDest::SetTarget(
    NiInterpolator* pkTarget, NiNode* pkParent)
{
    m_pkTarget = pkTarget;
    m_pkSceneParent = pkParent;
}
//---------------------------------------------------------------------------
inline bool NiPhysXInterpolatorDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

