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
#include "MUICameraOrbitState.h"
#include "MSharedData.h"
#include "MRenderer.h"
#include "MFramework.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
MUICameraOrbitState::MUICameraOrbitState()
{
    m_bProcessMouseMove = false;
    m_bInitCameraPosition = false;
    m_bSkipMovement = false;
    m_bOrbitMode = true;
}
//---------------------------------------------------------------------------
MUIState::UIType MUICameraOrbitState::GetUIType()
{
    return MUIState::ORBIT;
}
//---------------------------------------------------------------------------
String* MUICameraOrbitState::GetName()
{
    return "Orbit Camera";
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseEnter()
{
    MUIState::MouseEnter();
    
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseLeave()
{
    MUIState::MouseLeave();
    
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseHover()
{
    MUIState::MouseHover();
    
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::DoubleClick()
{
    MUIState::DoubleClick();
    
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseMove(int x, int y)
{
    MUIState::MouseMove(x,y);
    if (m_bProcessMouseMove)
    {
        POINT point;
        point.x = x;
        point.y = y;

        GetPositionInScreenSpace(point);

        if (m_bOrbitMode)
        {
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
                    bHorz = true;
                }
                else if (GetAsyncKeyState(VK_MENU) & 0x8000)
                {
                    bVert = true;
                }
                else
                {
                    bVert = true;
                    bHorz = true;
                }

                MouseOrbit(point, bVert, bHorz);
            }
        }
        else
        {
            MousePan(point, true, true);
        }

        m_ptClickPoint.x = point.x;   
        m_ptClickPoint.y = point.y;   
    }

    if (m_bSkipMovement)
        m_bSkipMovement = false;
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseUp(MouseButtonType eType, int x, int y)
{
    MUIState::MouseUp(eType, x, y);
    if ((eType == LMB || eType == MMB))
    {
        m_bProcessMouseMove = false;
        ReleaseCapture(); 

        if (eType == MMB)
        {
            EndPause();
            if (m_pkMOrbitPoint)
                m_pkMOrbitPoint->AppCulled = true;

            ShowCursor(TRUE);
        }
    }
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseDown(MouseButtonType eType, int x, int y)
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
        //::SetCapture(pkRenderer->GetRenderWindowHandle());

        m_bOrbitMode = (eType == LMB);

        if (eType == MMB)
        {
            BeginPause();
            if (m_pkMOrbitPoint)
                m_pkMOrbitPoint->AppCulled = false;

            ShowCursor(FALSE);
        }
    }
    
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::MouseWheel(int iDelta)
{
    WheelZoom(iDelta);
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::RefreshData()
{
    MUIState::RefreshData();
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::DeleteContents()
{
    MUIState::DeleteContents();
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::Activate(bool)
{
    if (m_pkMOrbitPoint)
    {
        m_pkMOrbitPoint->AppCulled = true;
    }
}
//---------------------------------------------------------------------------
void MUICameraOrbitState::Deactivate()
{
    if (m_pkMOrbitPoint)
    {
        m_pkMOrbitPoint->AppCulled = true;
    }
}

//---------------------------------------------------------------------------
