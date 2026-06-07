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
#include "ISettingsService.h"
#include "ICommandService.h"
#include "ServiceProvider.h"
#include "MViewport.h"
#include "MPoint3.h"
#include "MMatrix3.h"
#include "UICommandHandlerAttribute.h"
#include "MProxyManager.h"

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MCameraManager : public MDisposable,
        public IProxyHandler
    {
    private public:
        void Startup();

    public:
        __value enum StandardCamera
        {
            Perspective,
            User,
            PositiveX,
            NegativeX,
            PositiveY,
            NegativeY,
            PositiveZ,
            NegativeZ
        };

        static bool EntityIsCamera(MEntity* pmEntity);

        NiCamera* GetStandardCamera(MViewport* pmViewport,
            StandardCamera eCamera);
        MEntity* GetStandardCameraEntity(MViewport* pmViewport,
            StandardCamera eCamera);

        __property unsigned int get_SceneCameraCount();
        NiCamera* GetSceneCamera(unsigned int uiIndex);
        MEntity* GetSceneCameraEntity(unsigned int uiIndex);

        void Update(float fTime);
        void TransitionViewportToCamera(MViewport* pmViewport, 
            MEntity* pmTargetCamera);
        void TransitionCamera(MViewport* pmViewport, NiPoint3* pkDestPoint, 
            NiMatrix3* pkDestRot, NiFrustum* pkDestFrustum);
        __property bool get_Transitioning();

        void SetAspectRatioOnStandardCameras(MViewport* pmViewport,
            float fAspectRatio);

        bool IsAxisAlignedStandardCamera(MViewport* pmViewport,
            MEntity* pmCamera);
        bool IsStandardCamera(MViewport* pmViewport, MEntity* pmCamera);

    private:
        void CollectSceneCameras(MEntity* amEntities[]);
        void ClearSceneCameras();

        void CreateAndAddDefaultCameras();
        void TransitionCameraHelper(float fTime);
        void CreateCameraHelper(StandardCamera eCameraType, bool bOrtho,
            NiPoint3* kTranslate);
        void ResetAllDefaultCameraTransforms();
        void AddCameraToAllViewports(MEntity* pmCamera);
        void MoveDefaultCamerasOutOfBounds();

        void OnLayerAdded(MLayer* pmLayer, MLayer* pmParent);
        void OnSceneClosing(MScene* pmScene);
        void OnNewSceneLoaded(MScene* pmScene);
        void OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityComponentAdded(MEntity* pmEntity, 
            MComponent* pmComponent);
        void OnEntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);

        void RegisterUpAxisSetting();
        void OnUpAxisSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        void RegisterViewUndoableSetting();
        void OnViewUndoableSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        [UICommandHandlerAttribute("ResetViewportCameras")]
        void OnResetViewportCameras(Object* pmObject, EventArgs* pmEventArgs);

        ArrayList* m_pmSceneCameras;
        NiPoint3* m_pkUpAxis;

        MEntity* m_pmMasterProxy;
        MEntity* m_pmTransitionCamera;
        MEntity* m_pmTargetCamera;
        MEntity* m_pmSourceCamera;
        MViewport* m_pmTargetViewport;
        NiPoint3* m_pkCameraSourcePoint;
        NiPoint3* m_pkCameraDestPoint;
        NiMatrix3* m_pkCameraSourceRot;
        NiMatrix3* m_pkCameraDestRot;
        NiFrustum* m_pkSourceFrustum;
        NiFrustum* m_pkDestFrustum;
        bool m_bCamTransitioning;
        bool m_bTransitionBetweenCams;
        float m_fCamTransitionStartTime;
        float m_fCamTransitionDuration;

        __gc class CameraTransform
        {
        public:
            CameraTransform(MPoint3* pmTranslation, MMatrix3* pmRotation);

            MPoint3* m_pmTranslation;
            MMatrix3* m_pmRotation;
        };
        CameraTransform* m_amDefaultCameraTransforms[];

        bool m_bViewUndoable;
        static String* ms_strUpAxisSettingName = "Up Axis";
        static String* ms_strViewUndoableSettingName = "View Commands "
            "Undoable";

        static String* ms_strAspectRatioName = "Aspect Ratio";
        static String* ms_strTransitionCameraName = "Transition";

        static NiFixedString* ms_pkFOVString;
        static NiFixedString* ms_pkAspectString;
        static NiFixedString* ms_pkWidthString;
        static NiFixedString* ms_pkOrthoString;
        static NiFixedString* ms_pkNearClipString;
        static NiFixedString* ms_pkFarClipString;
        static NiFixedString* ms_pkTranslateString;
        static NiFixedString* ms_pkRotateString;
        static NiFixedString* ms_pkSceneRootString;

        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IProxyHandler members.
    public:
        virtual MEntity* GetMasterProxyEntity(MEntity* pmEntity);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MCameraManager* get_Instance();
    private:
        static MCameraManager* ms_pmThis = NULL;
        MCameraManager();
    };
}}}}
