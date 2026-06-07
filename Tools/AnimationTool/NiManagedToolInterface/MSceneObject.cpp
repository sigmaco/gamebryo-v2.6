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

#include "stdafx.h"
#include "MSceneObject.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;
#define NI_TOOL_ONLY_EXTRA_DATA_NAME "NI_TOOL_ONLY_EXTRA_DATA"

//---------------------------------------------------------------------------
MSceneObject::MSceneObject(NiAVObject* pkObject) : m_pkObject(pkObject)
{
    NIASSERT(m_pkObject);
}
//---------------------------------------------------------------------------
MSceneObject* MSceneObject::GetChildren()[]
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiNode* pkNode = NiDynamicCast(NiNode, m_pkObject);
    if (!pkNode)
    {
        pkData->Unlock();
        return NULL;
    }

    pkNode->GetChildCount();
    unsigned int uiVisibleChildCount = 0;
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkObj = pkNode->GetAt(ui);
        if (pkObj && !IsToolOnlyObject(pkObj))
            uiVisibleChildCount++;
    }

    if (uiVisibleChildCount == 0)
    {
        pkData->Unlock();
        return NULL;
    }

    MSceneObject* aObjects[] = NiExternalNew MSceneObject*[
        uiVisibleChildCount];

    unsigned int uiCurrIndex = 0;
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && !IsToolOnlyObject(pkChild))
        {
            aObjects[uiCurrIndex++] = NiExternalNew MSceneObject(pkChild);
        }
    }

    pkData->Unlock();

    return aObjects;
}
//---------------------------------------------------------------------------
NiAVObject* MSceneObject::GetObject()
{
    return m_pkObject;
}
//---------------------------------------------------------------------------
bool MSceneObject::IsToolOnlyObject(NiAVObject* pkObject)
{
    if (!pkObject)
        return false;
    NiExtraData* pkED = pkObject->GetExtraData(NI_TOOL_ONLY_EXTRA_DATA_NAME);
    return pkED != NULL;
}
//---------------------------------------------------------------------------
void MSceneObject::SetToolOnlyObject(NiAVObject* pkObject, bool bIsToolOnly)
{
    if (!pkObject)
        return;

    NiExtraData* pkED = pkObject->GetExtraData(NI_TOOL_ONLY_EXTRA_DATA_NAME);
    if (pkED && !bIsToolOnly)
    {
        pkObject->RemoveExtraData(NI_TOOL_ONLY_EXTRA_DATA_NAME);
    }
    else if (bIsToolOnly)
    {
        pkED = NiNew NiBooleanExtraData(true);
        pkED->SetName(NI_TOOL_ONLY_EXTRA_DATA_NAME);
        pkObject->AddExtraData(pkED);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
String* MSceneObject::get_Name()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strName = m_pkObject->GetName();
    pkData->Unlock();

    return strName;
}
//---------------------------------------------------------------------------
String* MSceneObject::get_RTTIName()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strRTTIName = m_pkObject->GetRTTI()->GetName();
    pkData->Unlock();

    return strRTTIName;
}
//---------------------------------------------------------------------------
