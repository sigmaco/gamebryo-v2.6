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
#include "SceneDesignerFrameworkPCH.h"

#include "MComponent.h"
#include "MComponentFactory.h"
#include "MAddRemovePropertyCommand.h"
#include "Mutility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MComponent::MComponent(NiEntityComponentInterface* pkComponent) :
    m_pkComponent(pkComponent)
{
    MInitInterfaceReference(m_pkComponent);
}
//---------------------------------------------------------------------------
MComponent* MComponent::CreateGeneralComponent(String* strName)
{
    const char* pcName = MStringToCharPointer(strName);
    NiUniqueID kUniqueID;
    MUtility::GuidToID(Guid::NewGuid(), kUniqueID);
    NiGeneralComponent* pkGeneralComponent =
        NiNew NiGeneralComponent(pcName, kUniqueID);
    MFreeCharPointer(pcName);
    return MComponentFactory::Instance->Get(pkGeneralComponent);
}
//---------------------------------------------------------------------------
void MComponent::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);

    if (bDisposing)
    {
        MDisposeInterfaceReference(m_pkComponent);
    }
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* MComponent::GetNiEntityComponentInterface()
{
    MVerifyValidInstance;

    return m_pkComponent;
}
//---------------------------------------------------------------------------
String* MComponent::get_Name()
{
    MVerifyValidInstance;

    return m_pkComponent->GetName();
}
//---------------------------------------------------------------------------
Guid MComponent::get_TemplateID()
{
    MVerifyValidInstance;

    return MUtility::IDToGuid(m_pkComponent->GetTemplateID());
}
//---------------------------------------------------------------------------
void MComponent::set_TemplateID(Guid mID)
{
    MVerifyValidInstance;

    this->Dirty = true;

    NiUniqueID kUniqueID;
    MUtility::GuidToID(mID, kUniqueID);
    m_pkComponent->SetTemplateID(kUniqueID);
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* MComponent::get_PropertyInterface()
{
    return m_pkComponent;
}
//---------------------------------------------------------------------------
MComponent* MComponent::Clone(bool bInheritProperties)
{
    MVerifyValidInstance;

    return MComponentFactory::Instance->Get(m_pkComponent->Clone(
        bInheritProperties));
}
//---------------------------------------------------------------------------
MComponent* MComponent::get_MasterComponent()
{
    MVerifyValidInstance;

    return MComponentFactory::Instance->Get(
        m_pkComponent->GetMasterComponent());
}
//---------------------------------------------------------------------------
void MComponent::set_MasterComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    this->Dirty = true;

    NiEntityComponentInterface* pkComponent = NULL;
    if (pmComponent != NULL)
    {
        pkComponent = pmComponent->GetNiEntityComponentInterface();
    }

    m_pkComponent->SetMasterComponent(pkComponent);
}
//---------------------------------------------------------------------------
MComponent* MComponent::get_RootMasterComponent()
{
    MVerifyValidInstance;

    MComponent* pmMaster = MasterComponent;
    while (pmMaster != NULL && pmMaster->MasterComponent != NULL)
    {
        pmMaster = pmMaster->MasterComponent;
    }

    return pmMaster;
}
//---------------------------------------------------------------------------
bool MComponent::IsAddOrRemovePropertySupported()
{
    MVerifyValidInstance;

    return NIBOOL_IS_TRUE(m_pkComponent->IsAddPropertySupported());
}

//---------------------------------------------------------------------------
void MComponent::AddProperty(String* strPropertyName, String* strDisplayName,
    String* strPrimitiveType, String* strSemanticType,
    String* strDescription, bool bUndoable)
{
    MVerifyValidInstance;

    AddProperty(strPropertyName, strDisplayName, strPrimitiveType,
        strSemanticType, false, strDescription, bUndoable);
}
//---------------------------------------------------------------------------
void MComponent::AddProperty(String* strPropertyName, String* strDisplayName,
    String* strPrimitiveType, String* strSemanticType, bool bCollection,
    String* strDescription, bool bUndoable)
{
    MVerifyValidInstance;

    AddRemoveProperty(strPropertyName, strDisplayName, strPrimitiveType, 
        strSemanticType, bCollection, strDescription, bUndoable, true);
}
//---------------------------------------------------------------------------
void MComponent::RemoveProperty(String* strPropertyName, bool bUndoable)
{
    MVerifyValidInstance;

    String* strDisplayName = GetPropertyDisplayName(strPropertyName);
    PropertyType* pmPropertyType = GetPropertyType(strPropertyName);
    MAssert(pmPropertyType != NULL, "Property type not found!");
    String* strPrimitiveType = pmPropertyType->PrimitiveType;
    String* strSemanticType = pmPropertyType->Name;
    String* strDescription = GetDescription(strPropertyName);
    
    AddRemoveProperty(strPropertyName, strDisplayName, strPrimitiveType, 
        strSemanticType, false, strDescription, bUndoable, false);
}
//---------------------------------------------------------------------------
String* MComponent::GetDependentPropertyNames()[]
{
    MVerifyValidInstance;

    ArrayList* pmNameList = new ArrayList();

    NiTObjectSet<NiFixedString> pkDependentNames;
    m_pkComponent->GetDependentPropertyNames(pkDependentNames);

    for (unsigned int uiIndex = 0; uiIndex < pkDependentNames.GetSize(); 
        uiIndex++)
    {
        NiFixedString pkName = pkDependentNames.GetAt(uiIndex);
        String* strName = new String((const char *)pkName);
        pmNameList->Add(strName);
    }

    return dynamic_cast<String*[]>( pmNameList->ToArray(__typeof(String)));
}
//---------------------------------------------------------------------------
String* MComponent::GetDescription(String* strPropertyName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strPropertyName);

    NiFixedString pkDescription;

    bool bSuccess = NIBOOL_IS_TRUE(
        m_pkComponent->GetDescription(pcName, pkDescription));

    MFreeCharPointer(pcName);
    MAssert(bSuccess, "MComponent:Error could not get description");

    return new String(pkDescription);
}
//---------------------------------------------------------------------------
bool MComponent::SetDescription(String* strPropertyName,
    String* strDescription)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcName = MStringToCharPointer(strPropertyName);
    const char* pcDescription = MStringToCharPointer(strDescription);

    bool bSuccess = NIBOOL_IS_TRUE(m_pkComponent->SetDescription(pcName,
        pcDescription));

    MFreeCharPointer(pcDescription);
    MFreeCharPointer(pcName);
    MAssert(bSuccess, "MComponent:Error could not set description");
    return bSuccess;
}
//---------------------------------------------------------------------------
void MComponent::AddRemoveProperty(String* strPropertyName,
    String* strDisplayName, String* strPrimitiveType, String* strSemanticType,
    bool bCollection, String* strDescription, bool bUndoable, bool bAdd)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    const char* pcDisplayName = MStringToCharPointer(strDisplayName);
    const char* pcPrimitiveType = MStringToCharPointer(strPrimitiveType);
    const char* pcSemanticType = MStringToCharPointer(strSemanticType);
    const char* pcDescription = MStringToCharPointer(strDescription);

    CommandService->ExecuteCommand(new MAddRemovePropertyCommand(
        NiNew NiAddRemovePropertyCommand(m_pkComponent, pcPropertyName,
        pcDisplayName, pcPrimitiveType, pcSemanticType, bCollection,
        pcDescription, bAdd), this), bUndoable);

    MFreeCharPointer(pcDescription);
    MFreeCharPointer(pcSemanticType);
    MFreeCharPointer(pcPrimitiveType);
    MFreeCharPointer(pcDisplayName);
    MFreeCharPointer(pcPropertyName);
}
//---------------------------------------------------------------------------
