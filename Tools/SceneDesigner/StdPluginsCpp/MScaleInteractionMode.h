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

#include "MSelectionInteractionMode.h"

using namespace System::Collections;
using namespace System::Collections::Generic;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MScaleInteractionMode : 
        public MSelectionInteractionMode
    {
    public:
        MScaleInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        [UICommandHandlerAttribute("ScaleInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("ScaleInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    protected:
        static const float STANDARD_DISTANCE = 15.0f;

        bool m_bAlreadyScaling;
        float m_fStartDistance;
        float m_fCurrentScale;

        float m_fScaleSnapIncrement;
        bool m_bSnapEnabled;
        float m_fInitialGizmoScale;
        NiTPrimitiveSet<float>* m_pkInitialScales;
        NiTPrimitiveSet<bool>* m_pkInitialScalesUnique;
        NiTObjectSet<NiPoint3>* m_pkInitialTranslations;
        NiTPrimitiveSet<bool>* m_pkInitialTranslationsUnique;
        float m_fPrecision;
        bool m_bPrecisionEnabled;
        float m_fTranslationPrecision;
        bool m_bTranslationPrecisionEnabled;

        ArrayList* m_pmPreviousSelection;
        bool m_bCloning;
        typedef MEntity* EntityArray[];
        typedef List<EntityArray> CloneArray;
        CloneArray* m_pmCloneArray;
        NiPoint3* m_pkCloneCenter;

        NiPick* m_pkPick;

        bool CanTransform();
        void ScaleHelper(const float fX, const float fY);
        float GetDistance(const float fX, const float fY);
        void SetGizmoScale(NiCamera* pkCamera);
        bool LoadGizmo();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IInteractionMode members.
    public:
        __property String* get_Name();
        bool Initialize();
        void Update(float fTime);
        void RenderGizmo(MRenderingContext* pmRenderingContext);
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);
    };
}}}}
