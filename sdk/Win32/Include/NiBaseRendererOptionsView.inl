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
inline NiBaseRendererOptionsView::NiBaseRendererOptionsView() :
    m_pDlgHandle(0),
    m_bViewAdvanced(false)
{
}

//---------------------------------------------------------------------------
inline NiBaseRendererOptionsView::~NiBaseRendererOptionsView()
{
}

//---------------------------------------------------------------------------
inline void NiBaseRendererOptionsView::Activate()
{
    // If we receive focus, show our dialog and reinit all controls
    ShowWindow(m_pDlgHandle, SW_SHOW);
    GetRendDesc()->Activate();
    InitDialogControls();
}

//---------------------------------------------------------------------------
inline void NiBaseRendererOptionsView::Deactivate()
{
    // If we have lost the focus, hide dialog
    ShowWindow(m_pDlgHandle, SW_HIDE);
}

//---------------------------------------------------------------------------
inline bool NiBaseRendererOptionsView::IsActive()
{
    return GetRendDesc()->IsActive();
}

