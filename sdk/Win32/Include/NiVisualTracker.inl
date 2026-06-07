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
// NiVisualTracker inline functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline void NiVisualTracker::ShowGraph(bool bShow, unsigned int uiWhichGraph)
{
    NIASSERT(uiWhichGraph < m_kCallbackData.GetSize());
    m_kCallbackData.GetAt(uiWhichGraph)->SetShow(bShow);
}
//---------------------------------------------------------------------------
inline bool NiVisualTracker::GetGraphShow(unsigned int uiWhichGraph)
{
    NIASSERT(uiWhichGraph < m_kCallbackData.GetSize());
    return m_kCallbackData.GetAt(uiWhichGraph)->GetShow();
}
//---------------------------------------------------------------------------
