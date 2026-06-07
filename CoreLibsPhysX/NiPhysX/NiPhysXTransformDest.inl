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
inline NiAVObject* NiPhysXTransformDest::GetTarget() const
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline NiNode* NiPhysXTransformDest::GetSceneGraphParent() const
{
    return m_pkTarget->GetParent();
}
//---------------------------------------------------------------------------
inline void NiPhysXTransformDest::SetTarget(NiAVObject* pkTarget)
{
    m_pkTarget = pkTarget;
}
//---------------------------------------------------------------------------
inline bool NiPhysXTransformDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
