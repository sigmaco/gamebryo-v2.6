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

using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MCreateInteractionMode : 
        public MExitableInteractionMode
    {
    public:
        MCreateInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        [UICommandHandlerAttribute("CreateInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("CreateInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    protected:
        bool m_bAlreadyDragging;
        typedef List<MEntity*> Entities;
        MEntity* m_pmNewEntity[];
        MEntity* m_pmNewProxy[];

        float m_fSnapSpacing;
        bool m_bSnapEnabled;
        float m_fPrecision;
        bool m_bPrecisionEnabled;
        bool m_bSnapToPickEnabled;
        bool m_bRotateToPickEnabled;
        unsigned short m_usAlignFacingAxis;
        unsigned short m_usAlignUpAxis;

        Cursor* m_pmCursor;
        Cursor* m_pmTmpCursor;

        NiPoint3* m_pkPlaneNormal;

        void CreateHelper(const float fX, const float fY);
        void GetBestPlane(const NiPoint3* pkLook);

        // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

        // IInteractionMode members.
    public:
        __property String* get_Name();
        __property Cursor* get_MouseCursor();
        bool Initialize();
        void Update(float fTime);
        void RenderGizmo(MRenderingContext* pmRenderingContext);
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);
    };
}}}}
