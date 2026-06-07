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

//---------------------------------------------------------------------------
inline void* NiObjectGroup::Allocate(unsigned int uiBytes)
{
    void* pvMem = m_pvFree;
    m_pvFree = (void*)((char*)m_pvFree + uiBytes);
    m_uiRefCount++;
    return pvMem;
}
//---------------------------------------------------------------------------
inline void NiObjectGroup::DecRefCount()
{
    if (--m_uiRefCount == 0)
        NiDelete this;
}
//---------------------------------------------------------------------------
inline void NiObjectGroup::SetSize(unsigned int uiSize)
{
    m_uiSize = uiSize;
}
//---------------------------------------------------------------------------
inline unsigned int NiObjectGroup::GetSize() const
{
    return m_uiSize;
}
//---------------------------------------------------------------------------
