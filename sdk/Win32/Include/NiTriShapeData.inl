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
//  NiTriShapeData inline functions

//---------------------------------------------------------------------------
inline unsigned short* NiTriShapeData::GetTriList()
{
    return m_pusTriList;
}
//---------------------------------------------------------------------------
inline const unsigned short* NiTriShapeData::GetTriList() const
{
    return m_pusTriList;
}
//---------------------------------------------------------------------------
inline unsigned int NiTriShapeData::GetTriListLength() const
{
    return m_uiTriListLength;
}
//---------------------------------------------------------------------------
inline NiTriShapeData::SharedNormalArray::SharedNormalArray()
{
    m_usNumSharedNormals = 0;
    m_pusSharedNormalIndexArray = 0;
}
//---------------------------------------------------------------------------
inline unsigned short NiTriShapeData::SharedNormalArray::GetCount()
{
    return m_usNumSharedNormals;
}
//---------------------------------------------------------------------------
inline unsigned short* NiTriShapeData::SharedNormalArray::GetArray()
{
    return m_pusSharedNormalIndexArray;
}
//---------------------------------------------------------------------------
inline void NiTriShapeData::SharedNormalArray::SetArray(
    unsigned short usCount, unsigned short* aArray)
{
    if(!usCount || !aArray)
    {
        m_usNumSharedNormals = 0;
        m_pusSharedNormalIndexArray = NULL;
    }
    else
    {
        m_usNumSharedNormals = usCount;
        m_pusSharedNormalIndexArray = aArray;
    }
}
//---------------------------------------------------------------------------
inline NiTriShapeData::SNAMemBlock::SNAMemBlock()
{
    m_pusBlock = 0;
    m_pusFreeBlock = 0;
    m_uiBlockSize = 0;
    m_uiFreeBlockSize = 0;
    m_pkNext = 0;
}
//---------------------------------------------------------------------------
inline NiTriShapeData::SNAMemBlock::SNAMemBlock(unsigned int uiSize)
{
    m_pusBlock = NiAlloc(unsigned short,uiSize);
    m_pusFreeBlock = m_pusBlock;
    m_uiBlockSize = uiSize;
    m_uiFreeBlockSize = uiSize;
    m_pkNext = 0;
}
//---------------------------------------------------------------------------
inline NiTriShapeData::SNAMemBlock::~SNAMemBlock()
{
    NiFree(m_pusBlock);

    // Iterate to next block
    NiDelete m_pkNext;
}
//---------------------------------------------------------------------------
inline void NiTriShapeData::GetTriangleIndices_Inline(unsigned short i, 
    unsigned short& i0, unsigned short& i1, unsigned short& i2) const
{
    unsigned int j0 = 3 * i;
    unsigned int j1 = j0 + 1;
    unsigned int j2 = j1 + 1;
    i0 = m_pusTriList[j0];
    i1 = m_pusTriList[j1];
    i2 = m_pusTriList[j2];
}
