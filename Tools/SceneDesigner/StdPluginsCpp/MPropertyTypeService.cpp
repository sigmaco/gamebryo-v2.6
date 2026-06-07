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
#include "StdPluginsCppPCH.h"

#include "MPropertyTypeService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MPropertyTypeService::MPropertyTypeService()
{
    m_pmNameToType = new Hashtable();
    m_pmNotVisibleToUser = new ArrayList();
}
//---------------------------------------------------------------------------
void MPropertyTypeService::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MPropertyTypeService::get_Name()
{
    MVerifyValidInstance;

    return "Property Type Service";
}
//---------------------------------------------------------------------------
bool MPropertyTypeService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MPropertyTypeService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// IPropertyTypeService members.
//---------------------------------------------------------------------------
void MPropertyTypeService::RegisterType(PropertyType* pmType)
{
    MVerifyValidInstance;

    RegisterType(pmType, true);
}
//---------------------------------------------------------------------------
void MPropertyTypeService::RegisterType(PropertyType* pmType,
    bool bVisibleToUser)
{
    MVerifyValidInstance;

    if (pmType != NULL)
    {
        m_pmNameToType->Item[pmType->Name] = pmType;
        if (!bVisibleToUser)
        {
            m_pmNotVisibleToUser->Add(pmType);
        }
    }
}
//---------------------------------------------------------------------------
void MPropertyTypeService::UnregisterType(PropertyType* pmType)
{
    MVerifyValidInstance;

    if (pmType != NULL)
    {
        m_pmNameToType->Remove(pmType->Name);
        m_pmNotVisibleToUser->Remove(pmType);
    }
}
//---------------------------------------------------------------------------
void MPropertyTypeService::Clear()
{
    MVerifyValidInstance;

    m_pmNameToType->Clear();
    m_pmNotVisibleToUser->Clear();
}
//---------------------------------------------------------------------------
PropertyType* MPropertyTypeService::LookupType(String* strName)
{
    MVerifyValidInstance;

    return dynamic_cast<PropertyType*>(m_pmNameToType->Item[strName]);
}
//---------------------------------------------------------------------------
bool MPropertyTypeService::IsTypeVisibleToUser(PropertyType* pmType)
{
    MVerifyValidInstance;

    if (pmType == NULL)
    {
        return false;
    }

    return !m_pmNotVisibleToUser->Contains(pmType);
}
//---------------------------------------------------------------------------
PropertyType* MPropertyTypeService::GetAllPropertyTypes()[]
{
    MVerifyValidInstance;

    PropertyType* amAllProperties[] = 
        new PropertyType*[m_pmNameToType->Count];

    m_pmNameToType->Values->CopyTo(amAllProperties, 0);
    return amAllProperties;
}
//---------------------------------------------------------------------------
