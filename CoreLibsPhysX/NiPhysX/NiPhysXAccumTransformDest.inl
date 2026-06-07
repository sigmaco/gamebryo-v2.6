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
inline NiAVObject* NiPhysXAccumTransformDest::GetAccumTarget() const
{
    return m_pkAccumTarget;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiPhysXAccumTransformDest::GetNonAccumTarget() const
{
    return m_pkNonAccumTarget;
}
//---------------------------------------------------------------------------
inline NiNode* NiPhysXAccumTransformDest::GetSceneGraphParent() const
{
    return m_pkAccumTarget->GetParent();
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumTransformDest::SetAccumTarget(NiAVObject* pkTarget)
{
    m_pkAccumTarget = pkTarget;
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumTransformDest::SetNonAccumTarget(NiAVObject* pkTarget)
{
    m_pkNonAccumTarget = pkTarget;
}
//---------------------------------------------------------------------------
inline bool NiPhysXAccumTransformDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
