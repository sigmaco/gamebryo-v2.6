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

#include "stdafx.h"
#include "MUICameraFreeLookState.h"
#include "MSharedData.h"
#include "MFramework.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
MUICameraFreeLookState::MUICameraFreeLookState()
{
   m_bProcessMouseMove = false;
}
//---------------------------------------------------------------------------
MUIState::UIType MUICameraFreeLookState::GetUIType()
{
    return MUIState::FREELOOK;
}
//---------------------------------------------------------------------------
String* MUICameraFreeLookState::GetName()
{
    return "Free Look Camera";
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseEnter()
{
    MUIState::MouseEnter();
    
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseLeave()
{
    MUIState::MouseLeave();
    
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseHover()
{
    MUIState::MouseHover();
    
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::DoubleClick()
{
    MUIState::DoubleClick();
    
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseMove(int x, int y)
{
    MUIState::MouseMove(x,y);
    if (m_bProcessMouseMove)
    {
        POINT point;
        point.x = x;
        point.y = y;

        GetPositionInScreenSpace(point);

        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
        {
            MouseZoom(point);
        }
        else
        {
            bool bHorz = false;
            bool bVert = false;
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                bVert = true;
            }
            else if (GetAsyncKeyState(VK_MENU) & 0x8000)
            {
                bHorz = true;
            }
            else
            {
                bVert = true;
                bHorz = true;
            }

            MousePan(point, bVert, bHorz);
        }

        m_ptClickPoint.x = point.x;   
        m_ptClickPoint.y = point.y;   
    }
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseUp(MouseButtonType eType, int x, int y)
{
    MUIState::MouseUp(eType, x, y);
    if ((eType == LMB || eType == MMB))
    {
        m_bProcessMouseMove = false;
        ReleaseCapture();

        if (m_pkMOrbitPoint)
            m_pkMOrbitPoint->AppCulled = true;

        EndPause();
        ShowCursor(TRUE);

    }
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseDown(MouseButtonType eType, int x, int y)
{
    MUIState::MouseDown(eType, x, y);

    if ((eType == LMB || eType == MMB))
    {
        POINT point;
        point.x = x;
        point.y = y;

        GetPositionInScreenSpace(point);
        m_ptClickPoint.x = point.x;
        m_ptClickPoint.y = point.y;
        m_bProcessMouseMove = true;
        MRenderer* pkRenderer = MFramework::Instance->Renderer;
        SetCapture(pkRenderer->GetRenderWindowHandle());

        BeginPause();
        if (m_pkMOrbitPoint)
            m_pkMOrbitPoint->AppCulled = false;

        ShowCursor(FALSE);

    }
    
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::MouseWheel(int iDelta)
{
    WheelZoom(iDelta);
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::RefreshData()
{
    MUIState::RefreshData();
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::DeleteContents()
{
    MUIState::DeleteContents();   
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::Activate(bool)
{
    if (m_pkMOrbitPoint)
        m_pkMOrbitPoint->AppCulled = true;
}
//---------------------------------------------------------------------------
void MUICameraFreeLookState::Deactivate()
{
   if (m_pkMOrbitPoint)
        m_pkMOrbitPoint->AppCulled = true;
}
//---------------------------------------------------------------------------
