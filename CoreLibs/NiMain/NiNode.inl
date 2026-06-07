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
//  NiNode inline functions

//---------------------------------------------------------------------------
inline void NiNode::RemoveAllChildren()
{
    m_kChildren.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiNode::CompactChildArray(const bool bRecursive)
{   
    m_kChildren.Compact();
    m_kChildren.UpdateSize();

    if (bRecursive)
    {
        for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
        {
            NiNode* pkChild = NiDynamicCast(NiNode, GetAt(i));
            if (pkChild)
            {
                pkChild->CompactChildArray(bRecursive);
            }
        }
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiNode::GetArrayCount() const
{
    return m_kChildren.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiNode::GetChildCount() const
{
    return m_kChildren.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline NiAVObject* NiNode::GetAt(unsigned int i) const
{
    if (m_kChildren.GetSize() <= i)
    {
        return 0;
    }
    else
    {
        return m_kChildren.GetAt(i);
    }
}
//---------------------------------------------------------------------------
inline const NiDynamicEffectList& NiNode::GetEffectList() const
{
    return m_kEffectList;
}
//---------------------------------------------------------------------------
