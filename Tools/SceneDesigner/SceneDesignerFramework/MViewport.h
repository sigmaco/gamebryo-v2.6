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
#include "MEntity.h"
#include "MScene.h"
#include "NiScreenConsole.h"
#include "IRenderingModeService.h"
#include "IInteractionModeService.h"
#include "ISettingsService.h"
#include "IOptionsService.h"
#include "ISelectionService.h"
#include "ICommandService.h"

using namespace System::Drawing;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI
    ::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MViewport : public MDisposable
    {
    public:
        MViewport();
        MViewport(float fLeft, float fRight, float fTop, float fBottom);
        MViewport(float fLeft, float fRight, float fTop, float fBottom,
            MEntity* pmCamera);

        static void _SDMInit();
        static void _SDMShutdown();

        __property MEntity* get_CameraEntity();
        NiCamera* GetNiCamera();
        void SetCamera(MEntity* pmCamera, bool bUndoable);

        void Update(float fTime);

        void UpdateClippingPlanes();
        void RenderUnselectedEntities();
        void RenderSelectedEntities();
        void RenderGizmo();
        void RenderScreenElements(bool bActiveViewport);
        void UpdateScreenConsole();

        __property float get_Left();
        __property float get_Right();
        __property float get_Top();
        __property float get_Bottom();

        bool SetViewportValues(float fLeft, float fRight, float fTop,
            float fBottom);

        __property int get_Width();
        __property int get_Height();

        __property MScene* get_ToolScene();
        __property IRenderingMode* get_RenderingMode();
        __property void set_RenderingMode(IRenderingMode* pmRenderingMode);

        __property System::Drawing::Rectangle get_CameraNameRect();

    private public:
        void CreateScreenConsole();
        void ReloadScreenConsolePixelData();
        void InternalSetCamera(MEntity* pmCamera);

    private:
        void InitToolScene();
        void CreateBorderGeometry();
        void UpdateBorderGeometry();
        void UpdateCameraViewport(NiCamera* pkCamera);
        IRenderingMode* GetFirstRenderingMode();
        void FindSelectedEntities(MEntity* amEntities[]);

        void RegisterForHighlightColorSetting();
        void OnHighlightColorChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        void OnEntityNameChanged(MEntity* pmEntity, String* strOldName,
            bool bInBatch);

        static NiFixedString* ms_pkTranslationName;
        static NiFixedString* ms_pkRotationName;
        static NiFixedString* ms_pkNearClipName;
        static NiFixedString* ms_pkFarClipName;

        MEntity* m_pmCamera;
        float m_fLeft;
        float m_fRight;
        float m_fTop;
        float m_fBottom;

        MScene* m_pmToolScene;
        IRenderingMode* m_pmRenderingMode;
        IRenderingMode* m_pmStandardRenderingMode;
        NiMesh* m_pkBorder;
        NiMaterialProperty* m_pkBorderMaterial;
        NiColor* m_pkHighlightColor;
        NiScreenConsole* m_pkScreenConsole;

        ArrayList* m_pmEntitiesToRender;

        static int ms_iScreenTextureOffsetX = 9;

        static String* ms_strHighlightColorSettingName = "Viewport Highlight "
            "Color";
        static String* ms_strOptionCategoryName = "Viewport Settings";
        static String* ms_strHighlightColorOptionName =
            String::Concat(ms_strOptionCategoryName, ".",
            ms_strHighlightColorSettingName);

        static IRenderingModeService* ms_pmRenderingModeService;
        __property static IRenderingModeService* get_RenderingModeService();

        static IInteractionModeService* ms_pmInteractionModeService;
        __property static IInteractionModeService*
            get_InteractionModeService();

        static ISettingsService* ms_pmSettingsService;
        __property static ISettingsService* get_SettingsService();

        static IOptionsService* ms_pmOptionsService;
        __property static IOptionsService* get_OptionsService();

        static ISelectionService* ms_pmSelectionService;
        __property static ISelectionService* get_SelectionService();

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
