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
// NiMultiTargetPoseHandler::InterpCtlrInfo
//---------------------------------------------------------------------------
inline NiMultiTargetPoseHandler::InterpCtlrInfo::InterpCtlrInfo() :
    m_sLOD(LODSKIPINTERP), m_usInterpolatorIndex(0), m_pkInterpCtlr(NULL)
{
}
//---------------------------------------------------------------------------
inline void NiMultiTargetPoseHandler::InterpCtlrInfo::ClearValues()
{
    m_sLOD = LODSKIPINTERP;
    m_usInterpolatorIndex = 0;
    m_pkInterpCtlr = NULL;
}
//---------------------------------------------------------------------------
inline bool NiMultiTargetPoseHandler::InterpCtlrInfo::IsTransformEvaluator() const
{
    return (m_usInterpolatorIndex == NiInterpController::INVALID_INDEX);
}

//---------------------------------------------------------------------------
// NiMultiTargetPoseHandler
//---------------------------------------------------------------------------
inline const NiFixedString& NiMultiTargetPoseHandler::GetAccumRootName() const
{
    return m_kAccumRootName;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiMultiTargetPoseHandler::GetAccumRoot() const
{
    return m_pkAccumRoot;
}
//---------------------------------------------------------------------------
inline const NiQuatTransform& 
    NiMultiTargetPoseHandler::GetAccumulatedTransform() const
{
    return m_kAccumTransform;
}
//---------------------------------------------------------------------------
inline void NiMultiTargetPoseHandler::SetAccumulatedTransform(
    const NiQuatTransform& kTransform)
{
    if (kTransform.IsScaleValid())
    {
        m_kAccumTransform.SetScale(kTransform.GetScale());
    }
    if (kTransform.IsRotateValid())
    {
        m_kAccumTransform.SetRotate(kTransform.GetRotate());
    }
    if (kTransform.IsTranslateValid())
    {
        m_kAccumTransform.SetTranslate(kTransform.GetTranslate());
    }

    // Reset if previously handling a single sequence.
    m_spSoleSequence = NULL;

    // Update the scene graph to reflect any changes.
    if (m_pkAccumRoot)
    {
        NIASSERT(m_kAccumTransform.IsScaleValid());
        m_pkAccumRoot->SetScale(m_kAccumTransform.GetScale());
        NIASSERT(m_kAccumTransform.IsRotateValid());
        m_pkAccumRoot->SetRotate(m_kAccumTransform.GetRotate());
        NIASSERT(m_kAccumTransform.IsTranslateValid());
        m_pkAccumRoot->SetTranslate(m_kAccumTransform.GetTranslate());
    }
}
//---------------------------------------------------------------------------
inline void NiMultiTargetPoseHandler::ClearAccumulatedTransform()
{
    m_kAccumTransform.SetScale(1.0f);
    m_kAccumTransform.SetRotate(NiQuaternion::IDENTITY);
    m_kAccumTransform.SetTranslate(NiPoint3::ZERO);

    // Reset if previously handling a single sequence.
    m_spSoleSequence = NULL;

    // Update the scene graph to reflect this change.
    if (m_pkAccumRoot)
    {
        m_pkAccumRoot->SetScale(1.0f);
        m_pkAccumRoot->SetRotate(NiQuaternion::IDENTITY);
        m_pkAccumRoot->SetTranslate(NiPoint3::ZERO);
    }
}
//---------------------------------------------------------------------------
inline NiControllerManager* NiMultiTargetPoseHandler::GetOwner() const
{
    return m_pkOwner;
}
//---------------------------------------------------------------------------
inline float NiMultiTargetPoseHandler::GetLastUpdateTime() const
{
    return m_fLastUpdateTime;
}
//---------------------------------------------------------------------------
inline void NiMultiTargetPoseHandler::AddNewTargetsFromPoseBinding()
{
    AddInterpControllerInfos();
}
//---------------------------------------------------------------------------
