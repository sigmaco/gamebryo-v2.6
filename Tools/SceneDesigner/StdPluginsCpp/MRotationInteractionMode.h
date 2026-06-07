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
    public __gc class MRotationInteractionMode : 
        public MSelectionInteractionMode
    {
    public:
        MRotationInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        __value enum RotateAxis
        {
            AXIS_X = 0,
            AXIS_Y,
            AXIS_Z,
        };

        [UICommandHandlerAttribute("RotateInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("RotateInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    protected:
        static const float STANDARD_DISTANCE = 15.0f;

        NiFixedString* m_pkXAxisName;
        NiFixedString* m_pkYAxisName;
        NiFixedString* m_pkZAxisName;
        NiFixedString* m_pkXLineName;
        NiFixedString* m_pkYLineName;
        NiFixedString* m_pkZLineName;

        RotateAxis m_eAxis;
        RotateAxis m_eCurrentAxis;
        bool m_bAlreadyRotating;
        bool m_bLinearMode;
        float m_fStartingRadians;
        float m_fCurrentRadians;
        int m_iMouseX;
        int m_iMouseY;
        NiPoint3* m_pkRotationLine;

        float m_fRadianSnap;
        bool m_bSnapEnabled;
        NiTObjectSet<NiMatrix3>* m_pkInitialRotation;
        NiTPrimitiveSet<bool>* m_pkInitialRotationUnique;
        NiTObjectSet<NiPoint3>* m_pkInitialTranslation;
        NiTPrimitiveSet<bool>* m_pkInitialTranslationUnique;

        float m_fTranslationPrecision;
        bool m_bPrecisionEnabled;

        ArrayList* m_pmPreviousSelection;
        bool m_bCloning;
        typedef MEntity* EntityArray[];
        typedef List<EntityArray> CloneArray;
        CloneArray* m_pmCloneArray;
        NiPoint3* m_pkCloneCenter;

        NiColor* m_pkHighLightColor;
        NiPick* m_pkPick;

        bool CanTransform();
        void RotateHelper(const NiPoint3* pkOrigin, const NiPoint3* pkDir);
        float GetRadians(const NiPoint3* pkOrigin, const NiPoint3* pkDir);
        bool RotateLinear(const RotateAxis eAxis);
        void HighLightAxis(const RotateAxis eAxis);
        void SetupRotation(int iX, int iY);
        void SetGizmoScale(NiCamera* pkCamera);

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
        bool LoadGizmo();
    };
}}}}
