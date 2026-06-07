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

#include "NiEntityComponentInterface.h"
#include "NiEntityInterface.h"

NiFixedString NiEntityComponentInterface::IT_COMPONENTINTERFACE;

//---------------------------------------------------------------------------
void NiEntityComponentInterface::_SDMInit()
{
    IT_COMPONENTINTERFACE = "Component Interface";
}
//---------------------------------------------------------------------------
void NiEntityComponentInterface::_SDMShutdown()
{
    IT_COMPONENTINTERFACE = NULL;
}
//---------------------------------------------------------------------------
NiFixedString NiEntityComponentInterface::GetInterfaceType()
{
    return IT_COMPONENTINTERFACE;
}
//---------------------------------------------------------------------------
NiBool NiEntityComponentInterface::CanAttachToEntity(
    NiEntityInterface* pkEntity)
{
    return CanAttachComponentToEntity(this, pkEntity);
}
//---------------------------------------------------------------------------
NiBool NiEntityComponentInterface::CanDetachFromEntity(
    NiEntityInterface* pkEntity)
{
    return CanDetachComponentFromEntity(this, pkEntity);
}
//---------------------------------------------------------------------------
NiBool NiEntityComponentInterface::CanAttachComponentToEntity(
    NiEntityComponentInterface* pkComponent,
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkComponent && pkEntity);

    // Get array of component property names.
    NiTObjectSet<NiFixedString> kComponentPropertyNames;
    pkComponent->GetPropertyNames(kComponentPropertyNames);

    // Get array of entity property names.
    NiTObjectSet<NiFixedString> kEntityPropertyNames;
    pkEntity->GetPropertyNames(kEntityPropertyNames);

    // Ensure that none of the component's properties are already on the
    // entity.
    unsigned int uiComponentPropertyNamesSize = 
        kComponentPropertyNames.GetSize();
    for (unsigned int ui = 0; ui < uiComponentPropertyNamesSize; ui++)
    {
        const NiFixedString& kComponentPropertyName =
            kComponentPropertyNames.GetAt(ui);
        int iIndex = kEntityPropertyNames.Find(kComponentPropertyName);
        if (iIndex != -1)
        {
            return false;
        }
    }

    // Get array of dependent property names for the component.
    NiTObjectSet<NiFixedString> kDependentPropertyNames;
    pkComponent->GetDependentPropertyNames(kDependentPropertyNames);

    // Ensure that all dependent properties exist on the entity.
    unsigned int uiDependentPropertyNamesSize = 
        kDependentPropertyNames.GetSize();
    for (unsigned int ui = 0; ui < uiDependentPropertyNamesSize; ui++)
    {
        const NiFixedString& kDependentPropertyName =
            kDependentPropertyNames.GetAt(ui);
        int iIndex = kEntityPropertyNames.Find(kDependentPropertyName);
        if (iIndex == -1)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityComponentInterface::CanDetachComponentFromEntity(
    NiEntityComponentInterface* pkComponent,
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkComponent && pkEntity);

    // Ensure the the component exists on the entity.
    bool bFoundComponent = false;
    {
        unsigned int uiSize = pkEntity->GetComponentCount();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            if (pkComponent == pkEntity->GetComponentAt(ui))
            {
                bFoundComponent = true;
                break;
            }
        }
    }
    if (!bFoundComponent)
    {
        return false;
    }

    // Get array of component property names.
    NiTObjectSet<NiFixedString> kComponentPropertyNames;
    pkComponent->GetPropertyNames(kComponentPropertyNames);

    // Search dependent properties to ensure that none of them exist in the
    // component to be removed.
    unsigned int uiSize = pkEntity->GetComponentCount();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiEntityComponentInterface* pkEntityComponent =
            pkEntity->GetComponentAt(ui);

        // Get array of dependent property names for the component.
        NiTObjectSet<NiFixedString> kDependentPropertyNames;
        pkEntityComponent->GetDependentPropertyNames(kDependentPropertyNames);

        for (unsigned int uj = 0; uj < kDependentPropertyNames.GetSize();
            uj++)
        {
            int iIndex = kComponentPropertyNames.Find(
                kDependentPropertyNames.GetAt(uj));
            if (iIndex != -1)
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
