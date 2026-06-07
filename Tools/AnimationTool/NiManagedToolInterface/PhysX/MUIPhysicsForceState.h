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

#pragma once

#include "MUIState.h"

namespace NiManagedToolInterface
{
    public __gc class MUIPhysicsForceState : public MUIState
    {
    public:
        MUIPhysicsForceState();
        __value enum UIForceConstraint  : unsigned char
        {
            PERPENDICULAR_SCREEN = 0,
            PARALLEL_SCREEN,
            CONSTRAINT_COUNT
        };

        virtual MUIState::UIType GetUIType();
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

    protected:
        bool m_bProcessMouseMove;
        UIForceConstraint m_eForceConstraint;
    };

}
