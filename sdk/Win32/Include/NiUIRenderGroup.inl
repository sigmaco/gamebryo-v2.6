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
inline void NiUIRenderGroup::EnterPressed(unsigned char)
{
}
//---------------------------------------------------------------------------
inline void NiUIRenderGroup::LeftMouseButton(unsigned char)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiUIRenderGroup::NumQuadsRequired()
{
    return 0;
}
//---------------------------------------------------------------------------
inline unsigned char NiUIRenderGroup::GetDepth()
{
    return m_ucDepth;
}
//---------------------------------------------------------------------------
inline void NiUIRenderGroup::AddChild(NiUIBaseElement* pkChild)
{
    if (pkChild != NULL)
    {
        NiUIBaseElementPtr spChild = pkChild;
        pkChild->SetVisible(m_bVisible);
        m_kChildren.AddUnique(spChild);
        spChild->SetParent(this);
    }
}
//---------------------------------------------------------------------------
inline void NiUIRenderGroup::RemoveChild(NiUIBaseElement* pkChild)
{
    if (pkChild == NULL)
        return;
    int iIdx = m_kChildren.Find(pkChild);
    if (iIdx == -1)
        return;

    pkChild->SetParent(this);
    m_kChildren.RemoveAt((unsigned int)iIdx);
}
//---------------------------------------------------------------------------
inline void NiUIRenderGroup::SetVisible(bool bVisible)
{
    NiUIBaseElement::SetVisible(bVisible);
}
//---------------------------------------------------------------------------
