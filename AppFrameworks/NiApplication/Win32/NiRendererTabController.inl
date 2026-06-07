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
// Constructor / destructor
//---------------------------------------------------------------------------
inline NiRendererTabController::NiRendererTabController() :
    m_pDlgHandle(NULL),
    m_uiCurrentViewIdx(0),
    m_uiChildDlgOffset(0)
{
}

//---------------------------------------------------------------------------
inline NiRendererTabController::~NiRendererTabController()
{
    DeleteOptionsViews();
}

//---------------------------------------------------------------------------
// Returns a name of the tab
//---------------------------------------------------------------------------
inline char* NiRendererTabController::GetCaption()
{
    return "Renderer";
}

