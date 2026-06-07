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
inline NiScene::NiScene(const NiFixedString& kName,
    unsigned int uiEntityArraySize, unsigned int uiSelectionSetArraySize,
    unsigned int uiLayerFilenameArrayCount) :
    m_kName(kName), m_kEntities(uiEntityArraySize),
    m_kSelectionSets(uiSelectionSetArraySize),
    m_kLayerFilenames(uiLayerFilenameArrayCount)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiScene::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiScene::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline unsigned int NiScene::GetEntityCount() const
{
    return m_kEntities.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiScene::GetEntityAt(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kEntities.GetSize());
    return m_kEntities.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiScene::GetEntityByName(const NiFixedString& kName) 
    const
{
    unsigned int uiSize = m_kEntities.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiEntityInterface* pkEntity = m_kEntities.GetAt(ui);
        if (pkEntity->GetName() == kName)
        {
            return pkEntity;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiScene::GetEntityByID(const NiUniqueID& kID) const
{
    const unsigned int uiEntityCount = m_kEntities.GetSize();
    for (unsigned int ui = 0; ui < uiEntityCount; ++ui)
    {
        NiEntityInterface* pkEntity = m_kEntities.GetAt(ui);
        if (pkEntity->GetID() == kID)
        {
            return pkEntity;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiBool NiScene::AddEntity(NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);

    NIASSERT(GetEntityByID(pkEntity->GetID()) == NULL);

    m_kEntities.Add(pkEntity);
    m_kQuickEntityLookUpMap.SetAt(pkEntity, true);
    pkEntity->SetSourceFilename(m_kSourceFilename);
    return true;
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveEntity(NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);
    int iIndex = m_kEntities.Find(pkEntity);
    if (iIndex > -1)
    {
        pkEntity->SetSourceFilename(NULL);
        m_kEntities.OrderedRemoveAt(iIndex);
        m_kQuickEntityLookUpMap.RemoveAt(pkEntity);
    }
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveEntityAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kEntities.GetSize());
    m_kEntities.GetAt(uiIndex)->SetSourceFilename(NULL);
    m_kQuickEntityLookUpMap.RemoveAt(m_kEntities.GetAt(uiIndex));
    m_kEntities.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveAllEntities()
{
    const unsigned int uiEntityCount = m_kEntities.GetSize();
    for (unsigned int ui = 0; ui < uiEntityCount; ++ui)
    {
        m_kEntities.GetAt(ui)->SetSourceFilename(NULL);
    }
    m_kEntities.RemoveAll();
    m_kQuickEntityLookUpMap.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiBool NiScene::IsEntityInScene(NiEntityInterface* pkEntity) const
{
    bool InScene;
    return m_kQuickEntityLookUpMap.GetAt(pkEntity, InScene);
}
//---------------------------------------------------------------------------
inline unsigned int NiScene::GetSelectionSetCount() const
{
    return m_kSelectionSets.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntitySelectionSet* NiScene::GetSelectionSetAt(unsigned int uiIndex)
    const
{
    NIASSERT(uiIndex < m_kSelectionSets.GetSize());
    return m_kSelectionSets.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiEntitySelectionSet* NiScene::GetSelectionSetByName(
    const NiFixedString& kName) const
{
    unsigned int uiSize = m_kSelectionSets.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiEntitySelectionSet* pkSelectionSet = m_kSelectionSets.GetAt(ui);
        if (pkSelectionSet->GetName() == kName)
        {
            return pkSelectionSet;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiBool NiScene::AddSelectionSet(NiEntitySelectionSet* pkSelectionSet)
{
    NIASSERT(pkSelectionSet);

    if (GetSelectionSetByName(pkSelectionSet->GetName()) != NULL)
    {
        return false;
    }

    m_kSelectionSets.Add(pkSelectionSet);
    return true;
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveSelectionSet(NiEntitySelectionSet* pkSelectionSet)
{
    NIASSERT(pkSelectionSet);
    int iIndex = m_kSelectionSets.Find(pkSelectionSet);
    if (iIndex > -1)
    {
        m_kSelectionSets.OrderedRemoveAt(iIndex);
    }
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveSelectionSetAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kSelectionSets.GetSize());
    m_kSelectionSets.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveAllSelectionSets()
{
    m_kSelectionSets.RemoveAll();
}
//---------------------------------------------------------------------------
inline unsigned int NiScene::GetLayerFilenameCount() const
{
    return m_kLayerFilenames.GetSize();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiScene::GetLayerFilenameAt(unsigned int uiIndex)
    const
{
    NIASSERT(uiIndex < m_kLayerFilenames.GetSize());
    return m_kLayerFilenames.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiBool NiScene::HasLayerFilename(
    const NiFixedString& kLayerFilename) const
{
    int iIndex = m_kLayerFilenames.Find(kLayerFilename);
    return (iIndex > -1);
}
//---------------------------------------------------------------------------
inline NiBool NiScene::AddLayerFilename(const NiFixedString& kLayerFilename)
{
    if (HasLayerFilename(kLayerFilename))
    {
        return false;
    }

    m_kLayerFilenames.Add(kLayerFilename);
    return true;
}
//---------------------------------------------------------------------------
inline NiBool NiScene::RenameLayerFilename(
    const NiFixedString& kOldLayerFilename,
    const NiFixedString& kNewLayerFilename)
{
    int iIndex = m_kLayerFilenames.Find(kOldLayerFilename);
    if (iIndex == -1)
    {
        return false;
    }
    m_kLayerFilenames.GetAt(iIndex) = kNewLayerFilename;
    return true;
}

//---------------------------------------------------------------------------
inline void NiScene::RemoveLayerFilename(const NiFixedString& kLayerFilename)
{
    int iIndex = m_kLayerFilenames.Find(kLayerFilename);
    if (iIndex > -1)
    {
        m_kLayerFilenames.OrderedRemoveAt(iIndex);
    }
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveLayerFilenameAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kLayerFilenames.GetSize());
    m_kLayerFilenames.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiScene::RemoveAllLayerFilenames()
{
    m_kLayerFilenames.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiScene::GetBound(NiBound& kBound)
{
    NiTPrimitiveArray<const NiBound*> kSelectionBounds;

    unsigned int uiSize = m_kEntities.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiEntityInterface* pkEntity = m_kEntities.GetAt(ui);
        if (pkEntity != NULL)
        {
            NiAVObject* pkScene;
            NiObject* pkObject;
            if (pkEntity->GetPropertyData(ms_kSceneRootName, pkObject, 0))
            {
                pkScene = NiDynamicCast(NiAVObject, pkObject);
                if (pkScene)
                {
                    pkScene->UpdateNodeBound();
                    kSelectionBounds.Add(&(pkScene->GetWorldBound()));
                }
            }
        }
    }
    if (kSelectionBounds.GetEffectiveSize() == 0)
    {
        kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    }
    else
    {
        kBound.ComputeMinimalBound(kSelectionBounds);
    }
}
//---------------------------------------------------------------------------
inline void NiScene::Update(float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    unsigned int uiSize = m_kEntities.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        m_kEntities.GetAt(ui)->Update(NULL, fTime, pkErrors, pkAssetManager);
    }
}
//---------------------------------------------------------------------------
inline void NiScene::BuildVisibleSet(NiEntityRenderingContext*
    pkRenderingContext, NiEntityErrorInterface* pkErrors)
{
    unsigned int uiSize = m_kEntities.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        m_kEntities.GetAt(ui)->BuildVisibleSet(pkRenderingContext, pkErrors);
    }
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiScene::GetSourceFilename() const
{
    return m_kSourceFilename;
}
//---------------------------------------------------------------------------
inline void NiScene::SetSourceFilename(
    const NiFixedString& kSourceFilename,
    bool bUpdateEntities)
{
    m_kSourceFilename = kSourceFilename;

    if (bUpdateEntities)
    {
        const unsigned int uiEntityCount = m_kEntities.GetSize();
        for (unsigned int ui = 0; ui < uiEntityCount; ++ui)
        {
            m_kEntities.GetAt(ui)->SetSourceFilename(kSourceFilename);
        }
    }
}
//---------------------------------------------------------------------------
