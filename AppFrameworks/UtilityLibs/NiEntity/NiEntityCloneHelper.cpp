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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiEntityCloneHelper.h"
#include "NiPrefabComponent.h"

//---------------------------------------------------------------------------
NiEntityCloneHelper::NiEntityCloneHelper() :
    m_bFixupCalled(false)
{
}
//---------------------------------------------------------------------------
void NiEntityCloneHelper::Clone(
    NiEntityInterface* pkEntity,
    const NiFixedString& kNewName,
    bool bInheritProperties)
{
    NIASSERT(!m_bFixupCalled);
    RecursiveClone(pkEntity, kNewName, bInheritProperties);
}
//---------------------------------------------------------------------------
NiEntityInterface* NiEntityCloneHelper::RecursiveClone(
    NiEntityInterface* pkEntity,
    const NiFixedString& kNewName,
    bool bInheritProperties)
{
    NiEntityInterface* pkClone = pkEntity->Clone(kNewName,
        bInheritProperties);

    m_kEntities.Add(pkClone);
    m_kLookupMap.SetAt(pkEntity, pkClone);

    // See if this is a prefab root.
    unsigned int uiCount = 0;
    if (pkEntity->GetElementCount(NiPrefabComponent::PROP_PREFAB_ENTITIES(), 
        uiCount))
    {
        for (unsigned int ui = 0; ui < uiCount; ++ui)
        {
            NiEntityInterface* pkPrefabEntity = NULL;
            NIVERIFY(pkEntity->GetPropertyData(
                NiPrefabComponent::PROP_PREFAB_ENTITIES(), pkPrefabEntity,
                ui));
            if (pkPrefabEntity)
            {
                NiEntityInterface* pkChild = RecursiveClone(pkPrefabEntity,
                    pkPrefabEntity->GetName(), bInheritProperties);
                pkChild->SetPrefabRoot(pkClone);
            }
        }
    }

    return pkClone;
}
//---------------------------------------------------------------------------
void NiEntityCloneHelper::Fixup(bool bClearExternalReferences)
{
    NIASSERT(!m_bFixupCalled);
    m_bFixupCalled = true;

    // Update all entity pointers to point to the cloned entities.
    const unsigned int uiEntityCount = m_kEntities.GetSize();
    for (unsigned int uiEntity = 0; uiEntity < uiEntityCount; ++uiEntity)
    {
        NiEntityInterface* pkEntity = m_kEntities.GetAt(uiEntity);

        NiTObjectSet<NiFixedString> kPropertyNames;
        pkEntity->GetPropertyNames(kPropertyNames);

        // Look at all properties for entity pointers.
        for (unsigned int uiProp = 0; uiProp < kPropertyNames.GetSize();
            ++uiProp)
        {
            const NiFixedString& kPropertyName = kPropertyNames.GetAt(uiProp);

            NiFixedString kPrimitiveType;
            NIVERIFY(pkEntity->GetPrimitiveType(kPropertyName,
                kPrimitiveType));

            if (kPrimitiveType == NiEntityInterface::PT_ENTITYPOINTER && 
                kPropertyName != NiPrefabComponent::PROP_PREFAB_ENTITIES())
            {
                FixupProperty(pkEntity, kPropertyName, 
                    bClearExternalReferences);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityCloneHelper::FixupProperty(
    NiEntityInterface* pkEntity,
    const NiFixedString& kPropName,
    bool bClearExternalReferences)
{
    // Now, change any non-null entity pointers to point to the cloned
    // version of what it pointed to before.
    unsigned int uiCount = 0;
    NIVERIFY(pkEntity->GetElementCount(kPropName, uiCount));
    for (unsigned int ui = 0; ui < uiCount; ++ui)
    {
        NiEntityInterface* pkOldEntity = NULL;
        NIVERIFY(pkEntity->GetPropertyData(kPropName, pkOldEntity, ui));

        // Look up the value to find the new pointer.
        NiEntityInterface* pkNewEntity = Lookup(pkOldEntity);

        // If the lookup returns null, we don't want to set the value
        // unless bClearExternalReferences is true.
        if (pkNewEntity || bClearExternalReferences)
        {
            NIVERIFY(pkEntity->SetPropertyData(kPropName, pkNewEntity, ui));
        }
    }
}
//---------------------------------------------------------------------------
NiEntityInterfaceIPtr NiEntityCloneHelper::Lookup(
    NiEntityInterface* pkOriginalEntity) const
{
    NIASSERT(m_bFixupCalled);

    NiEntityInterface* pkNewEntity = NULL;
    if (m_kLookupMap.GetAt(pkOriginalEntity, pkNewEntity))
    {
        return pkNewEntity;
    }

    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiEntityCloneHelper::GetEntityCount() const
{
    NIASSERT(m_bFixupCalled);

    return m_kEntities.GetSize();
}
//---------------------------------------------------------------------------
NiEntityInterfaceIPtr NiEntityCloneHelper::GetEntityAt(unsigned int uiIndex)
    const
{
    NIASSERT(m_bFixupCalled);

    NIASSERT(uiIndex < m_kEntities.GetSize());
    return m_kEntities.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiEntityCloneHelper::Reset()
{
    m_kEntities.RemoveAll();
    m_kLookupMap.RemoveAll();
    m_bFixupCalled = false;
}
//---------------------------------------------------------------------------
