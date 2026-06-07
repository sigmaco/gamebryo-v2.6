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
inline NiD3D10RendererOptionsView::NiD3D10RendererOptionsView()
{
    m_pkD3D10RendererDesc = NiNew NiD3D10RendererDesc();
}

//---------------------------------------------------------------------------
inline NiD3D10RendererOptionsView::~NiD3D10RendererOptionsView()
{
    NiDelete m_pkD3D10RendererDesc;
}

//---------------------------------------------------------------------------
// Returns pointer to NiD3D10RendererDesc instance
//---------------------------------------------------------------------------
inline NiBaseRendererDesc* NiD3D10RendererOptionsView::GetRendDesc()
{
    NIASSERT(m_pkD3D10RendererDesc);
    return m_pkD3D10RendererDesc;
}

//---------------------------------------------------------------------------
// Returns name of the renderer
//---------------------------------------------------------------------------
inline char* NiD3D10RendererOptionsView::GetName()
{
    return "Direct3D 10";
}

//---------------------------------------------------------------------------
// Height adjusting functions for basic / advanced view
//---------------------------------------------------------------------------
inline unsigned int NiD3D10RendererOptionsView::SetBasicHeight()
{
    ShowWindow(m_pD3D10DlgHandle, SW_HIDE);
    return NiBaseRendererOptionsView::SetBasicHeight();
}

//---------------------------------------------------------------------------
inline unsigned int NiD3D10RendererOptionsView::SetAdvancedHeight()
{
    ShowWindow(m_pD3D10DlgHandle, SW_SHOW);
    return NiBaseRendererOptionsView::SetAdvancedHeight();
}

