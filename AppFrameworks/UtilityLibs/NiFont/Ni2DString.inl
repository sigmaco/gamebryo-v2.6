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
// Ni2DString inline functions
//---------------------------------------------------------------------------
inline void Ni2DString::GetPosition(short& sX, short& sY)
{
    sX = m_sX;
    sY = m_sY;
}
//---------------------------------------------------------------------------
inline void Ni2DString::SetPosition(short sX, short sY)
{
    if (m_bStatic)
        return;

    if ((m_sX != sX) || (m_sY != sY))
    {
        m_sX = sX;
        m_sY = sY;
        m_bModified = true;
    }
}
//---------------------------------------------------------------------------
inline unsigned short Ni2DString::GetPointSize()
{
    // Default to the point size of the font
    if (m_usPointSize == 0)
        return (unsigned short)m_spFont->GetHeight();

    return m_usPointSize;
}
//---------------------------------------------------------------------------
inline void Ni2DString::SetPointSize(unsigned short usPointSize)
{
    if (m_bStatic)
        return;

    if (usPointSize == m_usPointSize)
        return;

    // Set the new point size and set the modified flag so the underlining
    // NiMeshScreenElements can be rebuilt before the next draw.
    m_usPointSize = usPointSize;
    m_bModified = true;
}
//---------------------------------------------------------------------------
inline const NiRenderTargetGroup* Ni2DString::GetRenderTargetGroup()
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline void Ni2DString::SetRenderTargetGroup(
    NiRenderTargetGroup* pkTarget)
{
    if (m_bStatic)
        return;

    if (pkTarget == m_pkTarget)
        return;

    // Set the new point size and set the modified flag so the underlining
    // NiMeshScreenElements can be rebuilt before the next draw.
    m_pkTarget = pkTarget;
    m_bModified = true;
}
//---------------------------------------------------------------------------
inline NiMeshScreenElements* Ni2DString::GetScreenElements(
    unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_kMeshScreenElements.GetSize());
    NIASSERT(m_kMeshScreenElements.GetAt(ucIndex) != NULL);

    return m_kMeshScreenElements.GetAt(ucIndex);
}
//---------------------------------------------------------------------------
