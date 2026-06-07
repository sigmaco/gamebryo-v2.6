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

#include <NiSystem.h>

//---------------------------------------------------------------------------
inline NiRefObject::NiRefObject()
{
    m_uiRefCount = 0;
    NiAtomicIncrement(ms_uiObjects);
}
//---------------------------------------------------------------------------
inline NiRefObject::~NiRefObject()
{
    NiAtomicDecrement(ms_uiObjects);
}
//---------------------------------------------------------------------------
inline void NiRefObject::IncRefCount()
{
    NiAtomicIncrement(m_uiRefCount);
}
//---------------------------------------------------------------------------
inline void NiRefObject::DecRefCount()
{
    if (NiAtomicDecrement(m_uiRefCount) == 0)
        DeleteThis();
}
//---------------------------------------------------------------------------
inline unsigned int NiRefObject::GetRefCount() const
{
    return m_uiRefCount;
}
//---------------------------------------------------------------------------
inline unsigned int NiRefObject::GetTotalObjectCount()
{
    return ms_uiObjects;
}
