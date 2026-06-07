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

#include "MDisposable.h"
#include "MViewport.h"
#include "UICommandHandlerAttribute.h"
#include "ISettingsService.h"
#include "IRenderingModeService.h"

using namespace System::Collections;
using namespace System::Runtime::InteropServices;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI
    ::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MViewportManager : public MDisposable
    {
    private public:
        void Startup();
        void OnResize();

    public:
        void ClearExclusiveViewport();
        void SetExclusiveViewport(unsigned int uiIndex);
        __property MViewport* get_ExclusiveViewport();

        void SetActiveViewport(unsigned int uiIndex);
        __property MViewport* get_ActiveViewport();

        __property unsigned int get_ViewportCount();
        MViewport* GetViewport(unsigned int uiIndex);

        void ResetViewportCameras();

        void Update(float fTime);

        void PreRender();
        void RenderUnselectedEntities();
        void RenderSelectedEntities();
        void RenderGizmo();
        void RenderScreenElements();

        void ActivateViewport(int iScreenX, int iScreenY);
        bool IsInsideActiveViewport(int iScreenX, int iScreenY);
        bool ScreenCoordinatesToViewportCoordinates(int iScreenX,
            int iScreenY, [Out] Int32& iViewportX, [Out] Int32& iViewportY);

    private:
        void AssignDefaultCamerasAndRenderingModes();
        void AdjustLetterboxedViewportValues(MViewport* pmViewport,
            float& fLeft, float& fRight, float& fTop, float& fBottom);
        void ResetViewportValues(MViewport* pmViewport, bool bAdjustAspect);

        void OnNewSceneLoaded(MScene* pmScene);
        void OnCameraRemovedFromManager(MEntity* pmCamera);
        void OnViewportCameraChanged(MViewport* pmViewport,
            MEntity* pmCamera);
        void OnEntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex, 
            bool bInBatch);
        void RegisterViewUndoableSetting();
        void OnViewUndoableSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        [UICommandHandlerAttribute("ToggleViewports")]
        void ToggleViewports(Object* pmObject, EventArgs* pmArgs);

        [UICommandHandlerAttribute("ResetViewportCameras")]
        void OnResetViewportCameras(Object* pmObject, EventArgs* pmEventArgs);

        MViewport* m_amViewports[];
        int m_iExclusiveIndex;
        int m_iActiveIndex;

        float m_fXViewportCenter;
        float m_fYViewportCenter;
        static String* m_strAspectPropertyName = "Aspect Ratio";
        static const float ms_fBorderOffset = 0.002f;

        bool m_bViewUndoable;
        static String* ms_strViewUndoableSettingName = "View Commands "
            "Undoable";

        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

        __property static IRenderingModeService* get_RenderingModeService();
        static IRenderingModeService* ms_pmRenderingModeService;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MViewportManager* get_Instance();
    private:
        static MViewportManager* ms_pmThis = NULL;
        MViewportManager();
    };
}}}}
