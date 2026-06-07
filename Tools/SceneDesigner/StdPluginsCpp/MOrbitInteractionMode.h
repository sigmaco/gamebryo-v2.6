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

#include "MExitableInteractionMode.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MOrbitInteractionMode : public MExitableInteractionMode
    {
    public:
        MOrbitInteractionMode();

        [UICommandHandlerAttribute("OrbitInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("OrbitInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    protected:
        bool m_bDragging;

        // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

        // IInteractionMode members.
    public:
        __property String* get_Name();
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);
    };
}}}}
