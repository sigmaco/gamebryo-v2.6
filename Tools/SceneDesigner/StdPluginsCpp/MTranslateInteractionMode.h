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
    public __gc class MTranslateInteractionMode : 
        public MSelectionInteractionMode
    {
    public:
        MTranslateInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        __value enum TranslateAxis
        {
            AXIS_X = 0,
            AXIS_Y,
            AXIS_Z,
            PLANE_XY,
            PLANE_XZ,
            PLANE_YZ
        };

        [UICommandHandlerAttribute("TranslateInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("TranslateInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    protected:
        // setting for allowing us to invert default distance to a ratio
        static const float STANDARD_DISTANCE = 10.0f;

        // fixed strings for finding objects in the gizmo
        NiFixedString* m_pkXAxisName;
        NiFixedString* m_pkYAxisName;
        NiFixedString* m_pkZAxisName;
        NiFixedString* m_pkXYPlaneName;
        NiFixedString* m_pkXZPlaneName;
        NiFixedString* m_pkYZPlaneName;
        NiFixedString* m_pkXLineName;
        NiFixedString* m_pkYLineName;
        NiFixedString* m_pkZLineName;
        NiFixedString* m_pkXYLineName;
        NiFixedString* m_pkXZLineName;
        NiFixedString* m_pkYXLineName;
        NiFixedString* m_pkYZLineName;
        NiFixedString* m_pkZXLineName;
        NiFixedString* m_pkZYLineName;

        // values that need to be stored between frames
        TranslateAxis m_eAxis;
        TranslateAxis m_eCurrentAxis;
        bool m_bOnGizmo;
        bool m_bAlreadyTranslating;
        float m_fStartScale;
        NiPoint3* m_pkStartPoint;
        NiPoint3* m_pkStartPick;
        int m_iMouseX;
        int m_iMouseY;

        // per scene settings
        float m_fSnapSpacing;
        bool m_bSnapEnabled;
        NiTObjectSet<NiPoint3>* m_pkInitialTranslation;
        NiTPrimitiveSet<bool>* m_pkInitialTranslationUnique;
        NiTObjectSet<NiMatrix3>* m_pkInitialRotation;
        NiTPrimitiveSet<bool>* m_pkInitialRotationUnique;
        float m_fPrecision;
        bool m_bPrecisionEnabled;
        bool m_bSnapToPickEnabled;
        bool m_bRotateToPickEnabled;
        unsigned short m_usAlignFacingAxis;
        unsigned short m_usAlignUpAxis;

        // members for cloning
        ArrayList* m_pmPreviousSelection;
        bool m_bCloning;
        typedef MEntity* EntityArray[];
        typedef List<EntityArray> CloneArray;
        CloneArray* m_pmCloneArray;
        NiPoint3* m_pkCloneCenter;

        NiColor* m_pkHighLightColor;
        NiPick* m_pkPick;

        bool CanTransform();
        void TranslateHelper(const NiPoint3* pkOrigin, const NiPoint3* pkDir);
        TranslateAxis GetBestAxis(const TranslateAxis eAxis);
        void HighLightAxis(const TranslateAxis eAxis);
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
