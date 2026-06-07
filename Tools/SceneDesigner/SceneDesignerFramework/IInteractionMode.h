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

#include "MRenderingContext.h"
using namespace System::Runtime::InteropServices;

using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI
{
    public __gc __interface IInteractionMode
    {
    public:
        __value enum MouseButtonType
        {
            LeftButton,
            MiddleButton,
            RightButton
        };

        __property String* get_Name();
        __property Cursor* get_MouseCursor();

        bool Initialize();

        void Update(float fTime);
        void RenderGizmo(MRenderingContext* pmRenderingContext);

        String* GetHoverData([Out]int* piHoverX, [Out]int* piHoverY);

        void MouseEnter();
        void MouseLeave();
        void MouseHover();
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);
        void MouseWheel(int iDelta);
        void DoubleClick();
    };
}}}}
