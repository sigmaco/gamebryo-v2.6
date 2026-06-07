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
inline NiCursorRenderClick::NiCursorRenderClick() : m_uiNumObjectsDrawn(0),
    m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
inline void NiCursorRenderClick::AppendCursor(NiCursor* pkCursor)
{
    NIASSERT(pkCursor);
    m_kCursors.AddTail(pkCursor);
}
//---------------------------------------------------------------------------
inline void NiCursorRenderClick::PrependCursor(NiCursor* pkCursor)
{
    NIASSERT(pkCursor);
    m_kCursors.AddHead(pkCursor);
}
//---------------------------------------------------------------------------
inline void NiCursorRenderClick::RemoveCursor(NiCursor* pkCursor)
{
    NIASSERT(pkCursor);
    m_kCursors.Remove(pkCursor);
}
//---------------------------------------------------------------------------
inline void NiCursorRenderClick::RemoveAllCursors()
{
    m_kCursors.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiCursorPtr>& NiCursorRenderClick::GetCursors()
{
    return m_kCursors;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiCursorPtr>& NiCursorRenderClick::GetCursors()
    const
{
    return m_kCursors;
}
//---------------------------------------------------------------------------
