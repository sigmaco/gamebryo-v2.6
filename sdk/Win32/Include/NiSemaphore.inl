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

#include "NiSystem.h"

inline NiSemaphore::NiSemaphore() : 
    m_iCount(0),
    m_iMaxCount(1)
{
    m_hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    NIASSERT(m_hSemaphore != NULL);
}
//---------------------------------------------------------------------------
inline NiSemaphore::NiSemaphore(int iCount) :
    m_iCount(iCount),
    m_iMaxCount(iCount + 1)
{
    m_hSemaphore = CreateSemaphore(NULL, m_iCount, m_iMaxCount, NULL);
    NIASSERT(m_hSemaphore != NULL);
}
//---------------------------------------------------------------------------
inline NiSemaphore::NiSemaphore(int iCount, int iMaxCount) :
    m_iCount(iCount),
    m_iMaxCount(iMaxCount)
{
    m_hSemaphore = CreateSemaphore(NULL, m_iCount, m_iMaxCount, NULL);
    NIASSERT(m_hSemaphore != NULL);
}
//---------------------------------------------------------------------------
inline NiSemaphore::~NiSemaphore()
{
    CloseHandle(m_hSemaphore);
}
//---------------------------------------------------------------------------
inline int NiSemaphore::GetCount()
{
    return m_iCount;
}
//---------------------------------------------------------------------------
inline int NiSemaphore::GetMaxCount()
{
    return m_iMaxCount;
}
//---------------------------------------------------------------------------
inline int NiSemaphore::Signal()
{
    ReleaseSemaphore(m_hSemaphore, 1, NULL);
    return NiAtomicIncrement((int)m_iCount);
}
//---------------------------------------------------------------------------
inline int NiSemaphore::Wait()
{
    WaitForSingleObject(m_hSemaphore, INFINITE);
    return NiAtomicDecrement((int)m_iCount);
}
//---------------------------------------------------------------------------
