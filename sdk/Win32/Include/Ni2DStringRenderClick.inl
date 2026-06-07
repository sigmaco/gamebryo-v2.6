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
inline Ni2DStringRenderClick::Ni2DStringRenderClick() : m_uiNumObjectsDrawn(0),
    m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
inline void Ni2DStringRenderClick::Append2DString(Ni2DString* pk2DString)
{
    NIASSERT(pk2DString);
    m_k2DStrings.AddTail(pk2DString);
}
//---------------------------------------------------------------------------
inline void Ni2DStringRenderClick::Prepend2DString(Ni2DString* pk2DString)
{
    NIASSERT(pk2DString);
    m_k2DStrings.AddHead(pk2DString);
}
//---------------------------------------------------------------------------
inline void Ni2DStringRenderClick::Remove2DString(Ni2DString* pk2DString)
{
    NIASSERT(pk2DString);
    m_k2DStrings.Remove(pk2DString);
}
//---------------------------------------------------------------------------
inline void Ni2DStringRenderClick::RemoveAll2DStrings()
{
    m_k2DStrings.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<Ni2DStringPtr>& Ni2DStringRenderClick::Get2DStrings()
{
    return m_k2DStrings;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<Ni2DStringPtr>&
    Ni2DStringRenderClick::Get2DStrings() const
{
    return m_k2DStrings;
}
//---------------------------------------------------------------------------
