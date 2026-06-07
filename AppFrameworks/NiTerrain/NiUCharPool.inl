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
inline NiUCharPool::NiUCharPool(NiUInt32 uiSize, NiUInt32 uiGrowby) :
    m_uiMaxSize(0),
    m_uiCurrentSize(0),    
    m_uiGrowBy(1),
    m_pucPool(0)
{
    SetGrowBy(uiGrowby);
    SetSize(uiSize);
    Reset();
}
//---------------------------------------------------------------------------
inline NiUCharPool::~NiUCharPool()
{
    NiFree(m_pucPool);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiUCharPool::GetSize() const
{
    return m_uiCurrentSize;
}
//---------------------------------------------------------------------------
inline void NiUCharPool::SetSize(NiUInt32 uiSize)
{
    m_pucPool = (NiUInt8*)NiRealloc(m_pucPool, uiSize);

    m_uiMaxSize = uiSize;
    if (m_uiCurrentSize > m_uiMaxSize)
    {
        m_uiCurrentSize = m_uiMaxSize;
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiUCharPool::GetGrowBy() const
{
    return m_uiCurrentSize;
}
//---------------------------------------------------------------------------
inline void NiUCharPool::SetGrowBy(NiUInt32 uiGrowBy)
{
    if (uiGrowBy == 0)
    {
        uiGrowBy = 1;
    }
    m_uiGrowBy = uiGrowBy;
}
//---------------------------------------------------------------------------
inline void NiUCharPool::Reset()
{
    memset(m_pucPool, 0, m_uiCurrentSize);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiUCharPool::GetNew()
{
    if (m_uiCurrentSize == m_uiMaxSize)
    {
        SetSize(m_uiMaxSize + m_uiGrowBy);
    }

    NiUInt32 uiOldSize = m_uiCurrentSize++;
    return uiOldSize;
}
//---------------------------------------------------------------------------
inline void NiUCharPool::ReleaseValue()
{
    m_uiCurrentSize--;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiUCharPool::GetValue(NiUInt32 uiIndex) const
{
    NIASSERT(uiIndex < m_uiCurrentSize);
    return m_pucPool[uiIndex];
}
//---------------------------------------------------------------------------
inline void NiUCharPool::CombineValue(NiUInt32 uiIndex, 
    NiUInt8 ucValue)
{
    NIASSERT(uiIndex < m_uiCurrentSize);
    m_pucPool[uiIndex] |= ucValue;
}
//---------------------------------------------------------------------------
inline void NiUCharPool::SetValue(NiUInt32 uiIndex, NiUInt8 ucValue)
{
    NIASSERT(uiIndex < m_uiCurrentSize);
    m_pucPool[uiIndex] = ucValue;
}
