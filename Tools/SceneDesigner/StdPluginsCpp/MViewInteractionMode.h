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
using namespace System::Runtime::InteropServices;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MViewInteractionMode : public MDisposable,
        public IInteractionMode
    {
    public:
        MViewInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        virtual void DoubleClick(MouseButtonType eType, int iX, int iY);

    protected:
        void OnNewSceneLoaded(MScene* pmScene);

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

        __property static IInteractionModeService*
            get_InteractionModeService();
        static IInteractionModeService* ms_pmInteractionModeService;

        __property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        __property static IEntityPathService* get_EntityPathService();
        static IEntityPathService* ms_psEntityPathService;

        __property static IRenderingModeService* get_RenderingModeService();
        static IRenderingModeService* ms_pmRenderingModeService;

        __property static IRenderingMode* get_GhostRenderingMode();
        static IRenderingMode* ms_pmGhostRenderingMode;

        NiFixedString* m_pkOrthoWidthName;
        NiFixedString* m_pkTranslationName;
        NiFixedString* m_pkRotationName;
        NiFixedString* m_pkScaleName;
        NiFixedString* m_pkStaticName;
        NiFixedString* m_pkNearClipName;
        NiFixedString* m_pkFarClipName;

        float m_fZoomStartTime;
        float m_fStartOrthoWidth;
        bool m_bZooming;
        bool m_bRotating;

        float m_fHoverStartTime;
        bool m_bOKToHover;
        bool m_bHovering;
        String* m_strHoverText;
        int m_iHoverX;
        int m_iHoverY;

        float m_fDefaultOrbitDistance;
        float m_fMouseWheelScalar;
        float m_fPanScalar;
        float m_fMouseLookScalar;
        float m_fPickDistance;
        bool m_bPickHit;
        bool m_bOrthographic;
        bool m_bWASDModeActive;
        NiPoint3* m_pkUpAxis;
        NiPoint3* m_pkStartPosition;
        NiPoint3* m_pkCurrentPosition;
        NiMatrix3* m_pkStartRotation;
        NiPoint3* m_pkOrbitCenter;
        bool m_bViewUndoable;
        
        bool m_bRightDown;
        bool m_bMiddleDown;
        bool m_bLeftDown;

        float m_fLeftClickTime;
        float m_fMiddleClickTime;
        float m_fRightClickTime;
        float m_fDoubleClickTime;
        int m_iLastX;
        int m_iLastY;
        int m_iStartX;
        int m_iStartY;

        MEntity* m_pmOriginalCamera;

        void SetupOrbit(int iX, int iY);
        void PanToSelection();
        void LookAtSelection();
        void AdjustOrthoDistance();
        void CommitZoom();
        void UserCameraSwitchHelper();
        void GetPickDistance(NiPoint3* pkOrigin, NiPoint3* pkDir);
        void PrepareClonedLight(MEntity* pmLight);


        void CalculateHoverData();

        static String* ms_strAffectedEntitiesName = "Affected Entities";


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
        void MouseEnter();
        void MouseLeave();
        void MouseHover();
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);
        void MouseWheel(int iDelta);
        void DoubleClick();
        String* GetHoverData([Out]int* piX, [Out]int* piY);
    };
}}}}
