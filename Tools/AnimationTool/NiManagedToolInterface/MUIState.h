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

#ifndef MUISTATE_H
#define MUISTATE_H

#include "MCamera.h"
#include "MOrbitPoint.h"

namespace NiManagedToolInterface
{
    public __abstract __gc class MUIState
    {
    public:
        __value enum MouseButtonType : unsigned int
        {
            LMB = 0,
            MMB,
            RMB,
            MAX_MOUSE_BUTTON_TYPES
        };

        __value enum UIType : unsigned int
        {
            ORBIT = 0,
            FREELOOK,
            BACKGROUNDHEIGHT,
#if defined(EE_PHYSX_BUILD)
            PHYSICSFORCE,
#endif
            MAX_UI_TYPES
        };

        MUIState();
        virtual UIType GetUIType() = 0;
        virtual String* GetName();
        virtual void MouseEnter();
        virtual void MouseLeave();
        virtual void MouseHover();
        virtual void DoubleClick();
        virtual void MouseMove(int x, int y);
        virtual void MouseUp(MouseButtonType eType, int x, int y);
        virtual void MouseDown(MouseButtonType eType, int x, int y);
        virtual void MouseWheel(int iDelta);

        virtual void Update();
        virtual void DeleteContents();
        virtual void RefreshData();

        virtual void Activate(bool bFlushPrevious);
        virtual void Deactivate();

        void SetWindowHandle(HWND hWnd);
        
        void SetCamera(MCamera* pkMCamera);
        void SetOrbitPoint(MOrbitPoint* pkMOrbitPoint);

        void SetFollowCam(bool bFollow);
        void ForceMouseShow();
        void ShowCursor(bool bShow);
        void SetCapture(HWND hwnd);
        void ReleaseCapture();
        
    protected:

        void MouseZoom(POINT& kPt);
        void MousePan(POINT& kPt, bool bVert, bool bHorz);
        void MouseOrbit(POINT& kPt, bool bVert, bool bHorz);
        void WheelZoom(int iWheel);

        void BeginPause();
        void EndPause();

        POINT GetCurrentMousePosition();
        void GetPositionInScreenSpace(POINT& kPt);
        POINT m_kMousePos;
        static float ms_fWheelDelta = 0.0f; 
        static bool* ms_bMouseDown = NiExternalNew bool[
            MAX_MOUSE_BUTTON_TYPES];
        bool m_bEntered;
        bool m_bHovering;
        HWND m_hWnd;
        MCamera* m_pkMCamera;
        MOrbitPoint* m_pkMOrbitPoint;
        bool m_bFollowCam;

        POINT m_ptClickPoint;
        bool m_bCachedPlayState;
    };

}

#endif  // #ifndef MUISTATE_H
