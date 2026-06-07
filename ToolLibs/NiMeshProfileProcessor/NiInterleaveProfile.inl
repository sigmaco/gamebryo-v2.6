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
// NiMeshStreamPackingDef::Element
//---------------------------------------------------------------------------
inline NiInterleaveProfile::Element::Element()
    : m_kSemantic("-"), m_uiSemanticIndex(0), m_uiElementIdx(0),
    m_bStrictInterleave(true), m_bCoalesceOnSplit(false)

{
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::Element::Element(NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex, bool bStrictInterleave, bool bCoalesceOnSplit)
    : m_kSemantic(kSemantic), m_uiSemanticIndex(uiSemanticIndex),
    m_uiElementIdx(0), m_bStrictInterleave(bStrictInterleave),
    m_bCoalesceOnSplit(bCoalesceOnSplit)
{
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::Element::Element(const char *pcSemantic,
    NiUInt32 uiSemanticIndex, bool bStrictInterleave, bool bCoalesceOnSplit)
    : m_kSemantic(pcSemantic), m_uiSemanticIndex(uiSemanticIndex),
    m_uiElementIdx(0), m_bStrictInterleave(bStrictInterleave),
    m_bCoalesceOnSplit(bCoalesceOnSplit)
{
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::Element::Element(const Element& rhs)
{
    *this = rhs;
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::Element& 
    NiInterleaveProfile::Element::operator=(const Element& rhs)
{
    m_kSemantic = rhs.m_kSemantic;
    m_uiSemanticIndex = rhs.m_uiSemanticIndex;
    m_kStreamRef = rhs.m_kStreamRef;
    m_kElement = rhs.m_kElement;
    m_uiElementIdx = rhs.m_uiElementIdx;
    m_bStrictInterleave = rhs.m_bStrictInterleave;
    m_bCoalesceOnSplit = rhs.m_bCoalesceOnSplit;
    return (*this);
}
//---------------------------------------------------------------------------
// NiMeshStreamPackingDef::StreamLayout
//---------------------------------------------------------------------------
inline NiInterleaveProfile::StreamLayout::StreamLayout() 
    : NiTObjectSet<Element>(0)
{
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::StreamLayout::StreamLayout(const
    StreamLayout& rhs)
    : NiTObjectSet<Element>(0)
{
    *this = rhs;
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::StreamLayout& 
    NiInterleaveProfile::StreamLayout::operator=(const StreamLayout& rhs)
{
    RemoveAll();
    NiUInt32 uiSize = rhs.GetSize();
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        Add(rhs.GetAt(i));
    }
    return (*this);
}
//---------------------------------------------------------------------------
// NiMeshStreamPackingDef::StreamLayoutSet
//---------------------------------------------------------------------------
inline NiInterleaveProfile::StreamLayoutSet::StreamLayoutSet()
    : NiTObjectSet<StreamLayout>(0)
{
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::StreamLayoutSet::StreamLayoutSet(const
    StreamLayoutSet& rhs)
    : NiTObjectSet<StreamLayout>(0)
{
    *this = rhs;
}
//---------------------------------------------------------------------------
inline NiInterleaveProfile::StreamLayoutSet& 
    NiInterleaveProfile::StreamLayoutSet::operator=(const
    StreamLayoutSet& rhs)
{
    RemoveAll();
    NiUInt32 uiSize = rhs.GetSize();
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        Add(rhs.GetAt(i));
    }
    return (*this);
}

//---------------------------------------------------------------------------
// NiMeshStreamPackingDef::NiMeshStreamPackingDef
//---------------------------------------------------------------------------
inline NiInterleaveProfile::NiInterleaveProfile()
{
    /* Do nothing */
}
//---------------------------------------------------------------------------

