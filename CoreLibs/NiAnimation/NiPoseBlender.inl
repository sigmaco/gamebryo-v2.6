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
// NiPoseBlender::ActivePose
//---------------------------------------------------------------------------
inline NiPoseBlender::ActivePose::ActivePose() : 
    m_pkPoseBuffer(NULL), m_puiFlagWeight(NULL), 
    m_bHasContributingItems(false), m_iPriority(0), 
    m_fWeight(0.0f), m_pkSequence(NULL)
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiPoseBlender
//---------------------------------------------------------------------------
inline NiControllerManager* NiPoseBlender::GetOwner() const
{
    return m_pkOwner;
}
//---------------------------------------------------------------------------
