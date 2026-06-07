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
#include "NiSceneCommandList.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiSceneCommandList::NiSceneCommandList()
{
    m_pkList = NiNew NiTList<NiSceneCommandInfo*>;
}
//---------------------------------------------------------------------------
NiSceneCommandList::~NiSceneCommandList()
{
    RemoveAll();
    NiDelete m_pkList;
}
//---------------------------------------------------------------------------
unsigned int NiSceneCommandList::GetSize()
{
    return m_pkList->GetSize();
}
//---------------------------------------------------------------------------
int NiSceneCommandList::Add(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL || m_pkList == NULL)
        return -1;

    NiTListIterator pkCurrent = m_pkList->GetHeadPos();
    NiTListIterator pkPrevious = NULL;

    int iIndex = 0;
    if (pkCurrent == NULL)
    {
        NiSceneCommandInfo* pkCommand = pkInfo;
        m_pkList->AddHead( pkCommand);
        return iIndex;
    }

    NiSceneCommandInfo* pkCurrentInfo = m_pkList->Get(pkCurrent);
    while (pkCurrentInfo != NULL && pkCurrent != NULL &&
        pkCurrentInfo->m_iPriority > pkInfo->m_iPriority)
    {
        pkPrevious = pkCurrent;
        pkCurrentInfo= m_pkList->GetNext(pkCurrent);
        iIndex++;
    }

    if (pkCurrentInfo != NULL)
    {
        NiSceneCommandInfo* pkCommand = pkInfo;
        if (pkCurrent == NULL)
        {
             m_pkList->InsertAfter(pkPrevious, pkCommand);
             iIndex++;
        }
        else
        {
            m_pkList->InsertBefore(pkCurrent, pkCommand);
        }
    }
    else
    {
        NiSceneCommandInfo* pkCommand = pkInfo;
        iIndex = m_pkList->GetSize();
        m_pkList->AddTail(pkCommand);
    }
    return iIndex;
}
//---------------------------------------------------------------------------
NiTList<NiSceneCommandInfo*>* NiSceneCommandList::GetList()
{
    return m_pkList;
}
//---------------------------------------------------------------------------
void NiSceneCommandList::PrintDebug()
{
    NiTListIterator pkCurrent = m_pkList->GetHeadPos();
    NiSceneCommandInfo* pkCurrentInfo = m_pkList->Get(pkCurrent);
    unsigned int uiCount = 0;
    while (pkCurrentInfo != NULL && pkCurrent != NULL)
    {
        char acLine[256];
        NiSprintf(acLine, 256, "[%d] Class: %d Priority: %d\n", uiCount, 
            pkCurrentInfo->m_uiClassID, pkCurrentInfo->m_iPriority );
        NiOutputDebugString(acLine);
        pkCurrentInfo = m_pkList->GetNext(pkCurrent);
        uiCount++;
    }
}
//---------------------------------------------------------------------------
bool NiSceneCommandList::Remove(unsigned int uiIndex)
{
    if (uiIndex > GetSize())
        return false;

    NiTListIterator pkIterator = m_pkList->GetHeadPos();
    NiSceneCommandInfo* pkInfo = NULL;
    unsigned int ui = 0;
    while (pkIterator != NULL && ui != uiIndex)
    {
        pkIterator = m_pkList->GetNextPos(pkIterator);
        ui++;
    }

    if (pkIterator == NULL)
        return false;

    pkInfo = m_pkList->RemovePos(pkIterator);
    NiDelete pkInfo;
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneCommandList::RemoveAll()
{
    NiTListIterator pkIterator = m_pkList->GetHeadPos();
    while (pkIterator != NULL)
    {
        NiSceneCommandInfo* ptr = m_pkList->Get(pkIterator);
        NiDelete ptr;
        pkIterator = m_pkList->GetNextPos(pkIterator);
    }

    m_pkList->RemoveAll();
    return true;
}
//---------------------------------------------------------------------------
NiSceneCommandInfo* NiSceneCommandList::GetAt(unsigned int uiIndex)
{
    if (uiIndex > GetSize())
        return NULL;

    NiTListIterator pkIterator = m_pkList->GetHeadPos();
    NiSceneCommandInfo* pkInfo = NULL;
    unsigned int ui = 0;

    if (uiIndex == 0)
        pkInfo = m_pkList->Get(pkIterator);

    while (pkIterator != NULL && ui-1 != uiIndex)
    {
        pkInfo = m_pkList->Get(pkIterator);
        pkIterator = m_pkList->GetNextPos(pkIterator);
        ui++;
    }


    if (pkIterator == NULL && ui-1 != uiIndex)
        return NULL;

    return pkInfo;
}
//---------------------------------------------------------------------------
void NiSceneCommandList::SetAt(unsigned int uiIndex, 
    NiSceneCommandInfo* pkInfo)
{
    if (uiIndex > GetSize() || pkInfo == NULL)
        return;

    NiTListIterator pkIterator = m_pkList->GetHeadPos();
    NiSceneCommandInfo* pkPtr = NULL;
    unsigned int ui = 0;
    while (pkIterator != NULL && ui-1 != uiIndex)
    {
        pkPtr = m_pkList->Get(pkIterator);
        pkIterator = m_pkList->GetNextPos(pkIterator);
        ui++;
    }

    if (pkIterator == NULL)
        return;
  
    m_pkList->InsertBefore(pkIterator, pkInfo);
    pkPtr = m_pkList->RemovePos(pkIterator);
    if (pkPtr != pkInfo)
        NiDelete pkPtr;
}
//---------------------------------------------------------------------------
