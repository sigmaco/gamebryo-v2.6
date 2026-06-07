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
inline NiSceneGraphComponent::NiSceneGraphComponent() : m_uFlags(0)
{
}
//---------------------------------------------------------------------------
inline NiSceneGraphComponent::NiSceneGraphComponent(
    const NiFixedString& kNifFilePath) :
    m_uFlags(0), m_kNifFilePath(kNifFilePath)
{
}
//---------------------------------------------------------------------------
inline NiSceneGraphComponent::NiSceneGraphComponent(
    NiSceneGraphComponent* pkMasterComponent) : m_uFlags(0),
    m_spMasterComponent(pkMasterComponent)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSceneGraphComponent::GetNifFilePath() const
{
    if (!GetNifFilePathChanged() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetNifFilePath();
    }
    else
    {
        return m_kNifFilePath;
    }
}
//---------------------------------------------------------------------------
inline void NiSceneGraphComponent::SetNifFilePath(
    const NiFixedString& kNifFilePath)
{
    if (GetNifFilePath() != kNifFilePath)
    {
        m_kNifFilePath = kNifFilePath;
        m_spSceneRoot = NULL;
        SetLoadErrorHit(false);
        SetNifFilePathChanged(true);
    }
}
//---------------------------------------------------------------------------
inline NiAVObject* NiSceneGraphComponent::GetSceneRoot() const
{
    return m_spSceneRoot;
}
//---------------------------------------------------------------------------
inline NiBool NiSceneGraphComponent::ShouldReloadScene() const
{
    if (GetNifFilePath() != m_kNifFilePath)
    {
        return true;
    }
    else if (!m_spSceneRoot)
    {
        return !GetLoadErrorHit();
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiBool NiSceneGraphComponent::GetLoadErrorHit() const
{
    return GetBit(LOAD_ERROR_HIT_MASK);
}
//---------------------------------------------------------------------------
inline void NiSceneGraphComponent::SetLoadErrorHit(bool bLoadErrorHit)
{
    SetBit(bLoadErrorHit, LOAD_ERROR_HIT_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiSceneGraphComponent::GetNifFilePathChanged() const
{
    return GetBit(NIF_FILE_PATH_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiSceneGraphComponent::SetNifFilePathChanged(
    bool bNifFilePathChanged)
{
    SetBit(bNifFilePathChanged, NIF_FILE_PATH_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiSceneGraphComponent::GetShouldUpdateSceneRoot() const
{
    return GetBit(SHOULD_UPDATE_SCENE_ROOT_MASK);
}
//---------------------------------------------------------------------------
inline void NiSceneGraphComponent::SetShouldUpdateSceneRoot(
    bool bShouldUpdateSceneRoot)
{
    SetBit(bShouldUpdateSceneRoot, SHOULD_UPDATE_SCENE_ROOT_MASK);
}
//---------------------------------------------------------------------------
