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
inline NiActorComponent::NiActorComponent() :
    m_uFlags(0), m_uiActiveSequenceID(NiActorManager::INVALID_SEQUENCE_ID),
    m_bAccumulateTransforms(false)
{
}
//---------------------------------------------------------------------------
inline NiActorComponent::NiActorComponent(const NiFixedString& kKfmFilePath,
    unsigned int uiActiveSequenceID, bool bAccumulateTransforms) :
    m_uFlags(0), m_kKfmFilePath(kKfmFilePath),
    m_uiActiveSequenceID(uiActiveSequenceID),
    m_bAccumulateTransforms(bAccumulateTransforms)
{
}
//---------------------------------------------------------------------------
inline NiActorComponent::NiActorComponent(
    NiActorComponent* pkMasterComponent) : m_uFlags(0),
    m_uiActiveSequenceID(NiActorManager::INVALID_SEQUENCE_ID),
    m_bAccumulateTransforms(false), m_spMasterComponent(pkMasterComponent)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiActorComponent::GetKfmFilePath() const
{
    if (!GetKfmFilePathChanged() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetKfmFilePath();
    }
    else
    {
        return m_kKfmFilePath;
    }
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetKfmFilePath(
    const NiFixedString& kKfmFilePath)
{
    if (GetKfmFilePath() != kKfmFilePath)
    {
        m_kKfmFilePath = kKfmFilePath;
        ClearActor();
        m_uiActiveSequenceID = NiActorManager::INVALID_SEQUENCE_ID;
        SetLoadErrorHit(false);
        SetKfmFilePathChanged(true);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiActorComponent::GetActiveSequenceID() const
{
    if (!GetActiveSequenceIDChanged() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetActiveSequenceID();
    }
    else
    {
        if (m_spActor)
        {
            return m_spActor->GetTargetAnimation();
        }
        else
        {
            return m_uiActiveSequenceID;
        }
    }
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetActiveSequenceID(
    unsigned int uiActiveSequenceID)
{
    if (GetActiveSequenceID() != uiActiveSequenceID)
    {
        m_uiActiveSequenceID = uiActiveSequenceID;
        if (m_spActor)
        {
            m_spActor->SetTargetAnimation(m_uiActiveSequenceID);
        }
        SetActiveSequenceIDChanged(true);
    }
}
//---------------------------------------------------------------------------
inline void NiActorComponent::ResetAnimation()
{
    if (m_spActor)
    {
        unsigned int uiActiveSequenceID = GetActiveSequenceID();
        m_spActor->Reset();
        m_spActor->SetTargetAnimation(uiActiveSequenceID);
        m_spActor->GetControllerManager()->SetAccumulatedTransform(
            NiQuatTransform(NiPoint3::ZERO, NiQuaternion::IDENTITY, 1.0f));
    }
}
//---------------------------------------------------------------------------
inline NiActorManager* NiActorComponent::GetActorManager() const
{
    return m_spActor;
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::GetAccumulateTransforms() const
{
    if (!GetAccumulateTransformsChanged() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetAccumulateTransforms();
    }
    else
    {
        return m_bAccumulateTransforms;
    }
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetAccumulateTransforms(
    bool bAccumulateTransforms)
{
    if (NIBOOL_IS_TRUE(GetAccumulateTransforms()) != bAccumulateTransforms)
    {
        m_bAccumulateTransforms = bAccumulateTransforms;
        ClearActor();
        SetLoadErrorHit(false);
        SetAccumulateTransformsChanged(true);
    }
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::ShouldReloadActor() const
{
    if (GetKfmFilePath() != m_kKfmFilePath ||
        NIBOOL_IS_TRUE(GetAccumulateTransforms()) != m_bAccumulateTransforms)
    {
        return true;
    }
    else if (!m_spActor)
    {
        return !GetLoadErrorHit();
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::ShouldChangeSequenceID() const
{
    if (GetActiveSequenceID() != m_uiActiveSequenceID)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline void NiActorComponent::ClearActor()
{
    if (m_spActor)
    {
        m_uiActiveSequenceID = m_spActor->GetTargetAnimation();
    }
    m_spActor = NULL;
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::GetLoadErrorHit() const
{
    return GetBit(LOAD_ERROR_HIT_MASK);
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetLoadErrorHit(bool bLoadErrorHit)
{
    SetBit(bLoadErrorHit, LOAD_ERROR_HIT_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::GetKfmFilePathChanged() const
{
    return GetBit(KFM_FILE_PATH_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetKfmFilePathChanged(bool bKfmFilePathChanged)
{
    SetBit(bKfmFilePathChanged, KFM_FILE_PATH_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::GetActiveSequenceIDChanged() const
{
    return GetBit(ACTIVE_SEQUENCE_ID_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetActiveSequenceIDChanged(
    bool bActiveSequenceIDChanged)
{
    SetBit(bActiveSequenceIDChanged, ACTIVE_SEQUENCE_ID_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiActorComponent::GetAccumulateTransformsChanged() const
{
    return GetBit(ACCUMULATE_TRANSFORMS_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiActorComponent::SetAccumulateTransformsChanged(
    bool bAccumulateTransformsChanged)
{
    SetBit(bAccumulateTransformsChanged, ACCUMULATE_TRANSFORMS_CHANGED_MASK);
}
//---------------------------------------------------------------------------
