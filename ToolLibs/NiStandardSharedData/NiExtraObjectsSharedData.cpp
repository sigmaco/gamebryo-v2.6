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

#include "NiExtraObjectsSharedData.h"
#include "NiSharedDataList.h"

NiImplementRTTI(NiExtraObjectsSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiExtraObjectsSharedData::NiExtraObjectsSharedData() 
{

}
//---------------------------------------------------------------------------
NiExtraObjectsSharedData::~NiExtraObjectsSharedData()
{
    m_kObjects.RemoveAll();
    m_kObjectInfoMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiExtraObjectsSharedData::AddObject(NiObject* pkObj,
    const char* pcName, bool bStreamable)
{
    if (pkObj)
    {
        m_kObjects.AddFirstEmpty(pkObj);
        ObjectInfo* pkInfo = NiNew ObjectInfo(pcName, bStreamable);
        m_kObjectInfoMap.SetAt(pkObj, pkInfo);
    }
}
//---------------------------------------------------------------------------
NiObjectPtr NiExtraObjectsSharedData::RemoveObject(NiObject* pkObj)
{
    if (!pkObj)
    {
        return 0;
    }

    NiObjectPtr spOriginalObj = 0;

    for (unsigned int ui = 0; ui < m_kObjects.GetSize(); ui++)
    {
        if (m_kObjects.GetAt(ui) == pkObj)
        {
            spOriginalObj = m_kObjects.RemoveAtAndFill(ui);
            m_kObjectInfoMap.RemoveAt(pkObj);
            break;
        }
    }

    return spOriginalObj;
}
//---------------------------------------------------------------------------
void NiExtraObjectsSharedData::RemoveAllObjects()
{
    m_kObjects.RemoveAll();
    m_kObjectInfoMap.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiExtraObjectsSharedData::GetObjectCount()
{
    return m_kObjects.GetSize();
}
//---------------------------------------------------------------------------
NiObject* NiExtraObjectsSharedData::GetObjectAt(unsigned int uiIndex)
{
    if (uiIndex >= m_kObjects.GetSize())
    {
        return NULL;
    }

    return m_kObjects.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiString NiExtraObjectsSharedData::GetObjectName(NiObject* pkObj)
{
    NiString strRootName;

    if (!pkObj)
    {
        return strRootName;
    }

    ObjectInfoPtr spInfo;
    if (m_kObjectInfoMap.GetAt(pkObj, spInfo))
    {
        strRootName = spInfo->m_strName;
    }

    return strRootName;
}
//---------------------------------------------------------------------------
bool NiExtraObjectsSharedData::IsObjectStreamable(NiObject* pkObj)
{
    if (!pkObj)
    {
        return false;
    }

    ObjectInfoPtr spInfo;
    if (m_kObjectInfoMap.GetAt(pkObj, spInfo))
    {
        return spInfo->m_bStreamable;
    }

    return false;
}
//---------------------------------------------------------------------------
