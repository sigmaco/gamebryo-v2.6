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

#include "MViewInteractionMode.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MSelectionInteractionMode : public MViewInteractionMode
    {
    public:
        MSelectionInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

    protected:
        static const float STANDARD_DISTANCE = 10.0f;

        NiPick::Record* m_pkPickRecord;
        float m_fDefaultDistance;
        NiNode* m_pkGizmo;
        void SetGizmoScale(NiCamera* pkCamera);
        bool LoadGizmo();

    private:
        [UICommandHandlerAttribute("SelectInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("SelectInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

        [UICommandHandlerAttribute("PickPrefabEntities")]
        void SetPickPrefabEntities(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("PickPrefabEntities")]
        void ValidatePickPrefabEntities(Object* pmSender, UIState* pmState);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IInteractionMode members.
    public:
        __property String* get_Name();
        bool Initialize();
        void RenderGizmo(MRenderingContext* pmRenderingContext);
        void MouseDown(MouseButtonType eType, int iX, int iY);
    };
}}}}
