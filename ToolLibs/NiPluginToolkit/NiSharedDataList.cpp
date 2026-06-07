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

#include "NiSharedDataList.h"

NiSharedDataList* NiSharedDataList::ms_pkThis = NULL;

//---------------------------------------------------------------------------
void NiSharedDataList::CreateInstance()
{
    NIASSERT(ms_pkThis == NULL);
    ms_pkThis = NiNew NiSharedDataList;
}
//---------------------------------------------------------------------------
void NiSharedDataList::DestroyInstance()
{
    NIASSERT(ms_pkThis != NULL);
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}
//---------------------------------------------------------------------------
NiSharedDataList* NiSharedDataList::GetInstance()
{
    return ms_pkThis;
}
//---------------------------------------------------------------------------
void NiSharedDataList::LockSharedData()
{
    NIASSERT(ms_pkThis != NULL);
    ms_pkThis->Lock();
}
//---------------------------------------------------------------------------
void NiSharedDataList::UnlockSharedData()
{
    NIASSERT(ms_pkThis != NULL);
    ms_pkThis->Unlock();
}
//---------------------------------------------------------------------------
void NiSharedDataList::Lock()
{
    m_kCriticalSection.Lock();
}
//---------------------------------------------------------------------------
void NiSharedDataList::Unlock()
{
    m_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiSharedDataList::ErrorCode NiSharedDataList::Insert(NiSharedData* pkNewData)
{
    NIASSERT(m_kCriticalSection.GetOwningThreadID() == GetCurrentThreadId());

    NiSharedDataPtr spTemp;
    if (m_kSharedDataObjects.GetAt(pkNewData->GetType(), spTemp))
    {
        m_eLastError = DUPLICATE_OBJECT;
        return DUPLICATE_OBJECT;
    }

    m_kSharedDataObjects.SetAt(pkNewData->GetType(), pkNewData);
    m_eLastError = SUCCESS;
    return SUCCESS;
}
//---------------------------------------------------------------------------
NiSharedDataList::ErrorCode NiSharedDataList::Delete(NiSharedDataType pkType)
{
    NIASSERT(m_kCriticalSection.GetOwningThreadID() == GetCurrentThreadId());

    NiSharedDataPtr spSharedData;
    m_kSharedDataObjects.GetAt(pkType, spSharedData);

    if (!m_kSharedDataObjects.RemoveAt(pkType))
    {
       m_eLastError = OBJECT_NOT_FOUND;
       return OBJECT_NOT_FOUND;
    }
    m_eLastError = SUCCESS;
    return SUCCESS;
}
//---------------------------------------------------------------------------
NiSharedDataList::ErrorCode 
NiSharedDataList::DeleteAll()
{
    NIASSERT(m_kCriticalSection.GetOwningThreadID() == GetCurrentThreadId());

    NiSharedDataType pkType;
    NiSharedDataPtr spSharedData;

    NiTMapIterator kIterator = m_kSharedDataObjects.GetFirstPos();
    while (!m_kSharedDataObjects.IsEmpty())
    {
        m_kSharedDataObjects.GetNext(kIterator, pkType, spSharedData);
        Delete(pkType);
    }
    m_eLastError = SUCCESS;
    return SUCCESS;
}
//---------------------------------------------------------------------------
NiSharedData* NiSharedDataList::Get(NiSharedDataType pkType)
{
    NIASSERT(m_kCriticalSection.GetOwningThreadID() == GetCurrentThreadId());

    NiSharedDataPtr spTemp = NULL;
    if (!m_kSharedDataObjects.GetAt(pkType, spTemp))
    {
        m_eLastError = OBJECT_NOT_FOUND;
        return NULL;
    }

    m_eLastError = SUCCESS;

    return spTemp;
}
//---------------------------------------------------------------------------
/// Get the total number of shared data pieces
unsigned int NiSharedDataList::GetSharedDataCount()
{
    return m_kSharedDataObjects.GetCount();
}

//---------------------------------------------------------------------------
/// Get the piece of shared data at a particular location
NiSharedData* NiSharedDataList::GetSharedDataAt(unsigned int uiWhichData)
{
    NIASSERT(m_kCriticalSection.GetOwningThreadID() == GetCurrentThreadId());
    NiSharedDataPtr spSharedData = NULL;
    unsigned int uiCount = GetSharedDataCount();
    if (uiWhichData >= uiCount)
    {
        return NULL;
    }
    else
    {
        NiTMapIterator kIterator = m_kSharedDataObjects.GetFirstPos();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            if (kIterator != NULL)
            {
                NiSharedDataType eType;
                m_kSharedDataObjects.GetNext(kIterator, eType, spSharedData);
                if (ui == uiWhichData)
                    return spSharedData;
            }
            
        }    
    }

    return NULL;
}
//---------------------------------------------------------------------------
/// The last error code generated by this object.
NiSharedDataList::ErrorCode NiSharedDataList::GetLastError()
{
    return m_eLastError;
}

//---------------------------------------------------------------------------
