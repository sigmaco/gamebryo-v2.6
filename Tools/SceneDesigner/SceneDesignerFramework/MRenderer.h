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
#include "MScene.h"
#include "MRenderingContext.h"
#include "MViewport.h"
#include "IRenderingModeService.h"
#include "IInteractionModeService.h"
#include "IMessageService.h"
#include "ISelectionService.h"
#include "ISettingsService.h"
#include "IOptionsService.h"

using namespace System::Drawing;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI
    ::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MRenderer : public MDisposable
    {
    public:
        __property Color get_BackgroundColor();
        __property void set_BackgroundColor(Color mBackgroundColor);

        __property int get_Width();
        __property int get_Height();
        __property MRenderingContext* get_RenderingContext();

        __property bool get_D3D10();
        void UseD3D10();

        bool Create(IntPtr hTopLevelWndPtr, IntPtr hRendererWndPtr);
        bool Recreate(IntPtr hRendererWndPtr);

        void Render();

    private:
        void CreateShaderSystem();
        void ReloadShaders();
        void CreateRenderedTextures();
        void CreateScreenQuad();
        void SetScreenQuadVertexColors();

        void RegisterForHighlightColorSetting();
        void OnHighlightColorChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);
        void RegisterForTransparencySetting();
        void OnTransparencyChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);
        void RegisterForBackgroundColorSetting();
        void OnBackgroundColorChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        NiRenderer* m_pkRenderer;
        NiRenderTargetGroup* m_pkMainRenderTarget;
        HWND m_hRendererWnd;
        Color m_mBackgroundColor;
        MRenderingContext* m_pmRenderingContext;
        NiVisibleArray* m_pkVisibleArray;
        NiCullingProcess* m_pkCullingProcess;
        HMODULE m_hNiCgShaderLib;

        NiRenderedTexture* m_pkTexture1;
        NiRenderedTexture* m_pkTexture2;
        NiRenderTargetGroup* m_pkTarget1;
        NiRenderTargetGroup* m_pkTarget2;
        NiMeshScreenElements* m_pkScreenQuad;
        NiTexturingProperty* m_pkScreenQuadTexProp;
        NiAlphaProperty* m_pkScreenQuadAlphaProp;
        NiVertexColorProperty* m_pkScreenQuadVertexColorProp;
        NiStencilProperty* m_pkScreenQuadStencilProp;
        NiSingleShaderMaterial* m_pkScreenQuadShaderMaterial;

        NiColorA* m_pkHighlightColor;
        bool m_bD3D10;
        bool m_bNeedsRecreate;

        static String* ms_strHighlightColorSettingName = "Entity Highlight "
            "Color";
        static String* ms_strTransparencySettingName = "Entity Highlight "
            "Transparency";
        static String* ms_strBackgroundColorSettingName = "Background Color";
        static String* ms_strOptionCategoryName = "Viewport Settings";
        static String* ms_strHighlightColorOptionName =
            String::Concat(ms_strOptionCategoryName, ".",
            ms_strHighlightColorSettingName);
        static String* ms_strTransparencyOptionName =
            String::Concat(ms_strOptionCategoryName, ".",
            ms_strTransparencySettingName);
        static String* ms_strBackgroundColorOptionName =
            String::Concat(ms_strOptionCategoryName, ".",
            ms_strBackgroundColorSettingName);

        static IRenderingModeService* ms_pmRenderingModeService;
        __property static IRenderingModeService* get_RenderingModeService();

        static IInteractionModeService* ms_pmInteractionModeService;
        __property static IInteractionModeService*
            get_InteractionModeService();

        static IMessageService* ms_pmMessageService;
        __property static IMessageService* get_MessageService();

        static ISelectionService* ms_pmSelectionService;
        __property static ISelectionService* get_SelectionService();

        static ISettingsService* ms_pmSettingsService;
        __property static ISettingsService* get_SettingsService();

        static IOptionsService* ms_pmOptionsService;
        __property static IOptionsService* get_OptionsService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MRenderer* get_Instance();
    private:
        static MRenderer* ms_pmThis = NULL;
        MRenderer();
    };

#pragma unmanaged
    class RendererInfo
    {
    public:
        static bool m_bDeviceLost;
        static bool m_bDeviceReset;

        static bool OnDeviceLost(void* pvData);
        static bool OnDeviceReset(bool bBeforeReset, void* pvData);
    };
#pragma managed
}}}}
