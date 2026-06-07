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
inline NiVisibleArray::NiVisibleArray()
{
    m_ppkArray = NULL;
    m_uiCurrentSize = 0;
    m_uiAllocatedSize = 0;

    m_uiGrowBy = DEF_GROW_BY;
    SetAllocatedSize(DEF_MAX_SIZE);
}
//---------------------------------------------------------------------------
inline NiVisibleArray::NiVisibleArray(unsigned int uiMaxSize, 
    unsigned int uiGrowBy)
{
    m_ppkArray = NULL;
    m_uiCurrentSize = 0;
    m_uiAllocatedSize = 0;

    m_uiGrowBy = uiGrowBy;
    if (m_uiGrowBy == 0)
        m_uiGrowBy = 1;
    SetAllocatedSize(uiMaxSize);
}
//---------------------------------------------------------------------------
inline NiVisibleArray::~NiVisibleArray()
{
    // no need to handle any smart pointers - just delete
    NiFree(m_ppkArray);
}
//---------------------------------------------------------------------------
inline void NiVisibleArray::Add(NiRenderObject& kObj)
{
    NIASSERT(m_uiCurrentSize <= m_uiAllocatedSize);

    if (m_uiCurrentSize == m_uiAllocatedSize)
        SetAllocatedSize(m_uiAllocatedSize + m_uiGrowBy);

    m_ppkArray[m_uiCurrentSize] = &kObj;
    m_uiCurrentSize++;
}
//---------------------------------------------------------------------------
inline void NiVisibleArray::RemoveAll()
{
    // Because we don't need to worry about smart pointers, this can be an
    // extremely fast function.
    m_uiCurrentSize = 0;
}
//---------------------------------------------------------------------------
inline void NiVisibleArray::SetAt(unsigned int uiIndex, NiRenderObject& kObj)
{
    NIASSERT(uiIndex < m_uiCurrentSize);

    m_ppkArray[uiIndex] = &kObj;
}
//---------------------------------------------------------------------------
inline NiRenderObject& NiVisibleArray::RemoveAtAndFill(unsigned int uiIndex)
{
    NIASSERT(m_uiCurrentSize != 0);
    NIASSERT(uiIndex < m_uiCurrentSize);

    // Save a ref to the item to be returned
    NiRenderObject& kDeleted = *m_ppkArray[uiIndex];

    // Decrement the current count, which means that it will point to the
    // last element in the array (the one to be swapped to the deleted spot)
    m_uiCurrentSize--;

    // swap the last element into the space we want to delete - this is safe
    // even if the two items are the same element (i.e. we're deleting the
    // last element in the array)
    m_ppkArray[uiIndex] = m_ppkArray[m_uiCurrentSize];

    return kDeleted;
}
//---------------------------------------------------------------------------
inline NiRenderObject& NiVisibleArray::GetAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiCurrentSize);

    return *(m_ppkArray[uiIndex]);
}
//---------------------------------------------------------------------------
inline NiRenderObject& NiVisibleArray::GetAt(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_uiCurrentSize);

    return *(m_ppkArray[uiIndex]);
}
//---------------------------------------------------------------------------
inline unsigned int NiVisibleArray::GetCount() const
{
    return m_uiCurrentSize;
}
//---------------------------------------------------------------------------
inline unsigned int NiVisibleArray::GetAllocatedSize() const
{
    return m_uiAllocatedSize;
}
//---------------------------------------------------------------------------
inline unsigned int NiVisibleArray::GetGrowBy() const
{
    return m_uiGrowBy;
}
//---------------------------------------------------------------------------
inline void NiVisibleArray::SetGrowBy(unsigned int uiGrowBy)
{
    if (uiGrowBy == 0)
        m_uiGrowBy = 1;
    else
        m_uiGrowBy = uiGrowBy;
}
//---------------------------------------------------------------------------
