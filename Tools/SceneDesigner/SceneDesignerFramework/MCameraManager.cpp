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

// Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MCameraManager.h"
#include "MFramework.h"
#include "MEntityFactory.h"
#include "MUtility.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MCameraManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MCameraManager();
    }
}
//---------------------------------------------------------------------------
void MCameraManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MCameraManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MCameraManager* MCameraManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MCameraManager::MCameraManager() : m_pkUpAxis(NULL),
    m_fCamTransitionDuration(0.3f), m_bViewUndoable(true), 
    m_pmTargetCamera(NULL), m_pmSourceCamera(NULL)
{
    ms_pkFOVString = NiNew NiFixedString("Field of View");
    ms_pkAspectString = NiNew NiFixedString("Aspect Ratio");
    ms_pkWidthString = NiNew NiFixedString("Orthographic Frustum Width");
    ms_pkOrthoString = NiNew NiFixedString("Orthographic");
    ms_pkNearClipString = NiNew NiFixedString("Near Clipping Plane");
    ms_pkFarClipString = NiNew NiFixedString("Far Clipping Plane");
    ms_pkTranslateString = NiNew NiFixedString("Translation");
    ms_pkRotateString = NiNew NiFixedString("Rotation");
    ms_pkSceneRootString = NiNew NiFixedString("Scene Root Pointer");

    // we don't hook LayerRemoved because that is handled when the entities
    // are removed from the main scene.
    __hook(&MEventManager::LayerAdded, MEventManager::Instance,
        &MCameraManager::OnLayerAdded);
    __hook(&MEventManager::SceneClosing, MEventManager::Instance,
        &MCameraManager::OnSceneClosing);
    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MCameraManager::OnNewSceneLoaded);
    __hook(&MEventManager::EntityAddedToScene, MEventManager::Instance, 
        &MCameraManager::OnEntityAddedToScene);
    __hook(&MEventManager::EntityRemovedFromScene, MEventManager::Instance, 
        &MCameraManager::OnEntityRemovedFromScene);
    __hook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
        &MCameraManager::OnEntityComponentAdded);
    __hook(&MEventManager::EntityComponentRemoved, MEventManager::Instance,
        &MCameraManager::OnEntityComponentRemoved);

    m_pmSceneCameras = new ArrayList();
    m_pkUpAxis = NiNew NiPoint3(0.0f, 0.0f, 1.0f);
    m_pkCameraSourcePoint = NiNew NiPoint3;
    m_pkCameraDestPoint = NiNew NiPoint3;
    m_pkCameraSourceRot  = NiNew NiMatrix3;
    m_pkCameraDestRot = NiNew NiMatrix3;
    m_pkSourceFrustum = NiNew NiFrustum;
    m_pkDestFrustum = NiNew NiFrustum;
}
//---------------------------------------------------------------------------
void MCameraManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        __unhook(&MEventManager::LayerAdded, MEventManager::Instance,
            &MCameraManager::OnLayerAdded);
        __unhook(&MEventManager::SceneClosing, MEventManager::Instance,
            &MCameraManager::OnSceneClosing);
        __unhook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
            &MCameraManager::OnNewSceneLoaded);
        __unhook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
            &MCameraManager::OnEntityAddedToScene);
        __unhook(&MEventManager::EntityRemovedFromScene,
            MEventManager::Instance,
            &MCameraManager::OnEntityRemovedFromScene);
        __unhook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
            &MCameraManager::OnEntityComponentAdded);
        __unhook(
            &MEventManager::EntityComponentRemoved, MEventManager::Instance,
            &MCameraManager::OnEntityComponentRemoved);
    }

    NiDelete m_pkUpAxis;
    NiDelete m_pkCameraSourcePoint;
    NiDelete m_pkCameraDestPoint;
    NiDelete m_pkCameraSourceRot;
    NiDelete m_pkCameraDestRot;
    NiDelete m_pkSourceFrustum;
    NiDelete m_pkDestFrustum;

    NiDelete ms_pkFOVString;
    NiDelete ms_pkAspectString;
    NiDelete ms_pkWidthString;
    NiDelete ms_pkOrthoString;
    NiDelete ms_pkNearClipString;
    NiDelete ms_pkFarClipString;
    NiDelete ms_pkTranslateString;
    NiDelete ms_pkRotateString;
    NiDelete ms_pkSceneRootString;
}
//---------------------------------------------------------------------------
void MCameraManager::Startup()
{
    RegisterUpAxisSetting();
    RegisterViewUndoableSetting();
    CreateAndAddDefaultCameras();

    m_pmMasterProxy = MProxyManager::CreateGenericMasterProxyEntity(
        "..\\..\\Data\\camera.nif", "Camera Proxy", true);

    MProxyManager::Instance->AddProxyHandler(this);
}
//---------------------------------------------------------------------------
void MCameraManager::RegisterUpAxisSetting()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(ms_strUpAxisSettingName,
        new MPoint3(*m_pkUpAxis), SettingsCategory::PerScene);
    SettingsService->SetChangedSettingHandler(ms_strUpAxisSettingName,
        SettingsCategory::PerScene, new SettingChangedHandler(this,
        &MCameraManager::OnUpAxisSettingChanged));
    OnUpAxisSettingChanged(NULL, NULL);
}
//---------------------------------------------------------------------------
void MCameraManager::OnUpAxisSettingChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    MPoint3* pmUpAxis = dynamic_cast<MPoint3*>(
        SettingsService->GetSettingsObject(ms_strUpAxisSettingName,
        SettingsCategory::PerScene));
    if (pmUpAxis != NULL)
    {
        pmUpAxis->ToNiPoint3(*m_pkUpAxis);
    }
}
//---------------------------------------------------------------------------
void MCameraManager::RegisterViewUndoableSetting()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(ms_strViewUndoableSettingName,
        __box(m_bViewUndoable), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strViewUndoableSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MCameraManager::OnViewUndoableSettingChanged));
    OnViewUndoableSettingChanged(NULL, NULL);
}
//---------------------------------------------------------------------------
void MCameraManager::OnViewUndoableSettingChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box bool* pbViewUndoable = dynamic_cast<__box bool*>(
        SettingsService->GetSettingsObject(ms_strViewUndoableSettingName,
        SettingsCategory::PerUser));
    if (pbViewUndoable != NULL)
    {
        m_bViewUndoable = *pbViewUndoable;
    }
}
//---------------------------------------------------------------------------
MEntity* MCameraManager::GetMasterProxyEntity(MEntity* pmEntity)
{
    if (MCameraManager::EntityIsCamera(pmEntity))
    {
        return m_pmMasterProxy;
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MCameraManager::EntityIsCamera(MEntity* pmEntity)
{
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(0);
    if (pkSceneRoot != NULL && NiIsKindOf(NiCamera, pkSceneRoot))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnLayerAdded(MLayer* pmLayer, MLayer*)
{
    MEntity* amEntities[] = pmLayer->GetEntities();
    CollectSceneCameras(amEntities);
}
//---------------------------------------------------------------------------
void MCameraManager::OnSceneClosing(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        for (unsigned int ui = 0;
            ui < MViewportManager::Instance->ViewportCount; ui++)
        {
            MViewport* pmViewport = MViewportManager::Instance->GetViewport(
                ui);
            if (m_pmSceneCameras->Contains(pmViewport->CameraEntity))
            {
                pmViewport->SetCamera(GetStandardCameraEntity(pmViewport,
                    StandardCamera::Perspective), false);
            }
        }
        ClearSceneCameras();
        ResetAllDefaultCameraTransforms();
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        MEntity* amEntities[] = pmScene->GetEntities();
        CollectSceneCameras(amEntities);
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene && EntityIsCamera(pmEntity))
    {
        if (!m_pmSceneCameras->Contains(pmEntity))
        {
            // add this entity to the camera list
            m_pmSceneCameras->Add(pmEntity);

            MEventManager::Instance->RaiseCameraAddedToManager(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnEntityRemovedFromScene(MScene* pmScene, 
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        if (m_pmSceneCameras->Contains(pmEntity))
        {
            // if the entity is in our camera array, remove it
            m_pmSceneCameras->Remove(pmEntity);

            MEventManager::Instance->RaiseCameraRemovedFromManager(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnEntityComponentAdded(MEntity* pmEntity, 
    MComponent*)
{
    MVerifyValidInstance;
    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity) &&
        EntityIsCamera(pmEntity))
    {
        if (!m_pmSceneCameras->Contains(pmEntity))
        {
            // add this entity to the camera list
            m_pmSceneCameras->Add(pmEntity);

            MEventManager::Instance->RaiseCameraAddedToManager(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnEntityComponentRemoved(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    if (m_pmSceneCameras->Contains(pmEntity))
    {

        if (pmComponent->Name->Equals("Camera")&&
            MFramework::Instance->Scene->IsEntityInScene(pmEntity) )
        {
            // if the entity is in our camera array, remove it
            m_pmSceneCameras->Remove(pmEntity);

            MEventManager::Instance->RaiseCameraRemovedFromManager(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MCameraManager::OnResetViewportCameras(Object*,
    EventArgs*)
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame("Reset viewport cameras to default "
        "positions");
    ResetAllDefaultCameraTransforms();
    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MCameraManager::CreateAndAddDefaultCameras()
{
    MVerifyValidInstance;

    NiPoint3 kTranslation;

    kTranslation = -100.0f * NiPoint3::UNIT_X;
    CreateCameraHelper(StandardCamera::PositiveX, true, &kTranslation);
    kTranslation = 100.0f * NiPoint3::UNIT_X;
    CreateCameraHelper(StandardCamera::NegativeX, true, &kTranslation);
    kTranslation = -100.0f * NiPoint3::UNIT_Y;
    CreateCameraHelper(StandardCamera::PositiveY, true, &kTranslation);
    kTranslation = 100.0f * NiPoint3::UNIT_Y;
    CreateCameraHelper(StandardCamera::NegativeY, true, &kTranslation);
    kTranslation = -100.0f * NiPoint3::UNIT_Z;
    CreateCameraHelper(StandardCamera::PositiveZ, true, &kTranslation);
    kTranslation = 100.0f * NiPoint3::UNIT_Z;
    CreateCameraHelper(StandardCamera::NegativeZ, true, &kTranslation);
    kTranslation = NiPoint3(-64.0f, -64.0f, 64.0f);
    CreateCameraHelper(StandardCamera::Perspective, false, &kTranslation);
    CreateCameraHelper(StandardCamera::User, true, &kTranslation);
    
    // now we must create the transition camera
    const char* pcTransitionCameraName = MStringToCharPointer(
        ms_strTransitionCameraName);
    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterface* pkEntity = NiNew NiGeneralEntity(
        pcTransitionCameraName, kTemplateID, 2);
    MFreeCharPointer(pcTransitionCameraName);
    NiMatrix3 kRotation;
    kRotation = NiViewMath::LookAt(NiPoint3::ZERO, kTranslation, 
        *m_pkUpAxis);
    NiTransformationComponent* pkTransComp =
        NiNew NiTransformationComponent(kTranslation, kRotation, 1.0f);
    NIVERIFY(pkEntity->AddComponent(pkTransComp));

    NiCameraComponent* pkCameraComp = NiNew NiCameraComponent();
    NIVERIFY(pkEntity->AddComponent(pkCameraComp));

    // set the properties on the camera component
    pkCameraComp->SetPropertyData(*ms_pkAspectString, 1.0f, 0);
    pkCameraComp->SetPropertyData(*ms_pkOrthoString, false, 0);
    pkCameraComp->SetPropertyData(*ms_pkFOVString, 90.0f, 0);

    m_pmTransitionCamera = MEntityFactory::Instance->Get(pkEntity);
}
//---------------------------------------------------------------------------
void MCameraManager::CollectSceneCameras(MEntity* amEntities[])
{
    MVerifyValidInstance;

    // search given scene for any entities that contain an NiCameraComponent
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        NiAVObject* pkRoot = pmEntity->GetSceneRootPointer(0);
        if ((pkRoot != NULL) && (NiIsKindOf(NiCamera, pkRoot)))
        {
            // add this entity to the camera list
            m_pmSceneCameras->Add(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MCameraManager::ClearSceneCameras()
{
    MVerifyValidInstance;

    m_pmSceneCameras->Clear();
}
//---------------------------------------------------------------------------
NiCamera* MCameraManager::GetStandardCamera(MViewport* pmViewport,
    StandardCamera eCamera)
{
    MVerifyValidInstance;

    MEntity* pmCamera = GetStandardCameraEntity(pmViewport, eCamera);
    NiAVObject* pkCamera = pmCamera->GetSceneRootPointer(0);
    return NiDynamicCast(NiCamera, pkCamera);
}
//---------------------------------------------------------------------------
MEntity* MCameraManager::GetStandardCameraEntity(MViewport* pmViewport,
    StandardCamera eCamera)
{
    MVerifyValidInstance;

    MAssert(pmViewport != NULL, "Null viewport provided to function!");

    MEntity* pmCamera = pmViewport->ToolScene->GetEntityByName(
        __box(eCamera)->ToString());
    MAssert(pmCamera != NULL, "Standard camera not found in viewport!");

    return pmCamera;
}
//---------------------------------------------------------------------------
unsigned int MCameraManager::get_SceneCameraCount()
{
    MVerifyValidInstance;

    return (unsigned int) m_pmSceneCameras->Count;
}
//---------------------------------------------------------------------------
NiCamera* MCameraManager::GetSceneCamera(unsigned int uiIndex)
{
    MVerifyValidInstance;

    MEntity* pmCamera = GetSceneCameraEntity(uiIndex);
    NiAVObject* pkCamera = pmCamera->GetSceneRootPointer(0);
    return NiDynamicCast(NiCamera, pkCamera);
}
//---------------------------------------------------------------------------
MEntity* MCameraManager::GetSceneCameraEntity(unsigned int uiIndex)
{
    MVerifyValidInstance;

    MAssert(uiIndex < (unsigned int) m_pmSceneCameras->Count, "Invalid index "
        "provided to function!");

    return dynamic_cast<MEntity*>(m_pmSceneCameras->Item[(int) uiIndex]);
}
//---------------------------------------------------------------------------
void MCameraManager::Update(float fTime)
{
    MVerifyValidInstance;

    if (m_bCamTransitioning)
    {
        TransitionCameraHelper(fTime);
    }
}
//---------------------------------------------------------------------------
void MCameraManager::TransitionViewportToCamera(MViewport* pmViewport, 
    MEntity* pmTargetCamera)
{
    MVerifyValidInstance;

    if (m_bCamTransitioning)
    {
        // If a transition is currently happening, just immediately set the
        // target camera on the viewport and return.
        if (m_pmTargetViewport != pmViewport)
        {
            CommandService->BeginUndoFrame(String::Format("Transition viewport"
                " to \"{0}\" camera", m_pmTargetCamera->Name));
            pmViewport->SetCamera(pmTargetCamera, m_bViewUndoable);
            CommandService->EndUndoFrame(m_bViewUndoable);
        }
        return;
    }

    NiAVObject* pkObject;
    NiCamera* pkSourceCamera;
    NiCamera* pkDestCamera;
    pkObject = pmTargetCamera->GetSceneRootPointer(0);
    pkDestCamera = NiDynamicCast(NiCamera, pkObject);
    pkSourceCamera = pmViewport->GetNiCamera();
    if ((pkSourceCamera != NULL) && (pkDestCamera != NULL))
    {
        *m_pkCameraSourcePoint = pkSourceCamera->GetTranslate();
        *m_pkCameraDestPoint = pkDestCamera->GetTranslate();

        *m_pkCameraSourceRot = pkSourceCamera->GetRotate();
        *m_pkCameraDestRot = pkDestCamera->GetRotate();

        *m_pkSourceFrustum = pkSourceCamera->GetViewFrustum();
        *m_pkDestFrustum = pkDestCamera->GetViewFrustum();

        m_fCamTransitionStartTime = MTimeManager::Instance->ContinuousTime;

        m_bTransitionBetweenCams = true;
        m_pmSourceCamera = pmViewport->CameraEntity;
        m_pmTargetCamera = pmTargetCamera;
        m_pmTargetViewport = pmViewport;

        TransitionCameraHelper(m_fCamTransitionStartTime);
        m_pmTargetViewport->SetCamera(m_pmTransitionCamera, false);

        m_bCamTransitioning = true;
    }
}
//---------------------------------------------------------------------------
void MCameraManager::TransitionCamera(MViewport* pmViewport, 
    NiPoint3* pkDestPoint, NiMatrix3* pkDestRot, NiFrustum* pkDestFrustum)
{
    MVerifyValidInstance;

    if (m_bCamTransitioning)
    {
        // If a transition is currently happening, just immediately set the
        // parameters on the viewport camera and return.
        if (m_pmTargetViewport != pmViewport &&
            pmViewport->CameraEntity != NULL)
        {
            MEntity* pmCamera;
            pmCamera = pmViewport->CameraEntity;
            CommandService->BeginUndoFrame(String::Format("Transition \"{0}\" "
                "to new location", pmCamera->Name));
            pmCamera->SetPropertyData(*ms_pkTranslateString, new 
                MPoint3(*pkDestPoint), 0, true);
            pmCamera->SetPropertyData(*ms_pkRotateString, new 
                MMatrix3(*pkDestRot), 0, true);
            float fFOV = (NiASin(pkDestFrustum->m_fRight) * 2.0f) * 360.0f /
                NI_TWO_PI;
            float fAspect = (pkDestFrustum->m_fRight / pkDestFrustum->m_fTop);
            float fWidth = (pkDestFrustum->m_fRight - pkDestFrustum->m_fLeft);
            pmCamera->SetPropertyData(*ms_pkFOVString, __box(fFOV), 0, true);
            pmCamera->SetPropertyData(*ms_pkAspectString, __box(fAspect), 0, 
                true);
            pmCamera->SetPropertyData(*ms_pkWidthString, __box(fWidth), 0, 
                true);
            pmCamera->SetPropertyData(*ms_pkOrthoString,
                __box(pkDestFrustum->m_bOrtho), 0, true);
            pmCamera->SetPropertyData(*ms_pkNearClipString, 
                __box(pkDestFrustum->m_fNear), 0, true);
            pmCamera->SetPropertyData(*ms_pkFarClipString,
                __box(pkDestFrustum->m_fFar), 0, true);

            bool bUndo = !(IsStandardCamera(pmViewport, pmCamera));

            CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
        }
        return;
    }

    NiCamera* pkSourceCamera;
    pkSourceCamera = pmViewport->GetNiCamera();
    NiEntityInterface* pkSource;
    pkSource = pmViewport->CameraEntity->GetNiEntityInterface();
    if (pkSourceCamera != NULL)
    {
        *m_pkCameraSourcePoint = pkSourceCamera->GetTranslate();
        *m_pkCameraDestPoint = *pkDestPoint;

        *m_pkCameraSourceRot = pkSourceCamera->GetRotate();
        *m_pkCameraDestRot = *pkDestRot;

        *m_pkSourceFrustum = pkSourceCamera->GetViewFrustum();
        *m_pkDestFrustum = *pkDestFrustum;

        m_fCamTransitionStartTime = MTimeManager::Instance->ContinuousTime;

        m_bTransitionBetweenCams = false;
        m_pmSourceCamera = NULL;
        m_pmTargetCamera = pmViewport->CameraEntity;
        m_pmTargetViewport = pmViewport;

        pkSource->SetPropertyData(*ms_pkTranslateString, *pkDestPoint);
        pkSource->SetPropertyData(*ms_pkRotateString, *pkDestRot);
        float fFOV;
        float fAspect;
        float fWidth;
        fFOV = (NiASin(pkDestFrustum->m_fRight) * 2.0f) * 360.0f / NI_TWO_PI;
        fAspect = (pkDestFrustum->m_fRight / pkDestFrustum->m_fTop);
        fWidth = (pkDestFrustum->m_fRight - pkDestFrustum->m_fLeft);
        pkSource->SetPropertyData(*ms_pkFOVString, fFOV);
        pkSource->SetPropertyData(*ms_pkAspectString, fAspect);
        pkSource->SetPropertyData(*ms_pkWidthString, fWidth);
        pkSource->SetPropertyData(*ms_pkOrthoString, pkDestFrustum->m_bOrtho);
        pkSource->SetPropertyData(*ms_pkNearClipString, 
            pkDestFrustum->m_fNear);
        pkSource->SetPropertyData(*ms_pkFarClipString, pkDestFrustum->m_fFar);

        TransitionCameraHelper(m_fCamTransitionStartTime);
        m_pmTargetViewport->SetCamera(m_pmTransitionCamera, false);

        m_bCamTransitioning = true;
    }
}
//---------------------------------------------------------------------------
bool MCameraManager::get_Transitioning()
{
    MVerifyValidInstance;

    return m_bCamTransitioning;
}
//---------------------------------------------------------------------------
void MCameraManager::TransitionCameraHelper(float fTime)
{
    MVerifyValidInstance;

    float fAlpha = ((fTime - m_fCamTransitionStartTime) / 
        m_fCamTransitionDuration);

    if (fAlpha >= 1.0f)
        fAlpha = 1.0f;

    NiPoint3 kDeltaPoint;
    NiPoint3 kResultPoint;
    NiMatrix3 kResultRotation;

    NiEntityPropertyInterface* pkCamera;
    pkCamera = m_pmTransitionCamera->GetNiEntityInterface();

    bool bSrcOrtho;
    bool bDestOrtho;
    NiFrustum kDeltaFrustum;
    bSrcOrtho = m_pkSourceFrustum->m_bOrtho;
    bDestOrtho = m_pkDestFrustum->m_bOrtho;

    // get the scene bounds including tool scene
    NiBound* pkBound;
    pkBound = MFramework::Instance->BoundManager->GetToolSceneBound(
        m_pmTargetViewport);

    if (bSrcOrtho == bDestOrtho)
    {
        kDeltaPoint = *m_pkCameraDestPoint - *m_pkCameraSourcePoint;
        kResultPoint = *m_pkCameraSourcePoint + kDeltaPoint * fAlpha;

        // find point that linearly interpolates along closest line 
        // between two view vectors in space
        // make this the new look at point
        NiPoint3 kP1, kP2;  // line origins
        NiPoint3 kD1, kD2;  // line directions
        NiPoint3 kDelta;    // line between origins

        m_pkCameraDestRot->GetCol(0, (float*)&kD1);
        m_pkCameraSourceRot->GetCol(0, (float*)&kD2);
        kP1 = *m_pkCameraDestPoint;
        kP2 = *m_pkCameraSourcePoint;
        kDelta = kP1 - kP2;

        // if cameras aren't pointing parallel
        if ((fabs(kD1.Dot(kD2)) <= NiViewMath::PARALLEL_THRESHOLD) && 
            (kDelta.Length() >= NiViewMath::INV_PARALLEL_THRESHOLD))
        {

            float fDenominator = 1.0f - kD2.Dot(kD1) * kD2.Dot(kD1);
            float fNumerator1 = kDelta.Dot(kD2) * kD2.Dot(kD1) - 
                kDelta.Dot(kD1) * kD2.Dot(kD2);
            kDelta = kDelta * -1.0f;
            float fNumerator2 = kDelta.Dot(kD1) * kD1.Dot(kD2) - 
                kDelta.Dot(kD2) * kD1.Dot(kD1);

            NiPoint3 kLookAtPoint = (kP1 + kD1 * fNumerator1 / 
                fDenominator) * fAlpha + (kP2 + kD2 * fNumerator2 / 
                fDenominator) * (1.0f - fAlpha);
            NiPoint3 kLook = kLookAtPoint - kResultPoint;
            kLook.Unitize();
            NiPoint3 kLookTangent;
            NiPoint3 kLookBiTangent;
            kLookTangent = kLook.Cross(*m_pkUpAxis);
            kLookTangent.Unitize();
            kLookBiTangent = kLookTangent.Cross(kLook);
            kResultRotation.SetCol(0, (float*)&kLook);
            kResultRotation.SetCol(1, (float*)&kLookBiTangent);
            kResultRotation.SetCol(2, (float*)&kLookTangent);
        }
        else
        {
            // if cameras are pointing parallel directions, the focal point
            // is unclear; use quaternion slerp instead
            NiQuaternion kSourceQuat;
            NiQuaternion kDestQuat;
            kSourceQuat.FromRotation(*m_pkCameraSourceRot);
            kDestQuat.FromRotation(*m_pkCameraDestRot);
            NiQuaternion kDeltaQuat = NiQuaternion::Slerp(fAlpha, 
                kSourceQuat, kDestQuat);
            kDeltaQuat.ToRotation(kResultRotation);
        }

        kDeltaFrustum.m_fLeft = m_pkSourceFrustum->m_fLeft + (
            (m_pkDestFrustum->m_fLeft - m_pkSourceFrustum->m_fLeft) * fAlpha);
        kDeltaFrustum.m_fRight = m_pkSourceFrustum->m_fRight + (
            (m_pkDestFrustum->m_fRight - m_pkSourceFrustum->m_fRight) * 
            fAlpha);
        kDeltaFrustum.m_fTop = m_pkSourceFrustum->m_fTop + (
            (m_pkDestFrustum->m_fTop - m_pkSourceFrustum->m_fTop) * fAlpha);
        kDeltaFrustum.m_fBottom = m_pkSourceFrustum->m_fBottom + (
            (m_pkDestFrustum->m_fBottom - m_pkSourceFrustum->m_fBottom) * 
            fAlpha);
        if (pkBound != NULL)
        {
            // if we have a scene bounds, use it for clip planes
            NiCamera kTempCam;
            kTempCam.SetTranslate(kResultPoint);
            kTempCam.SetRotate(kResultRotation);
            kTempCam.Update(0.0f);
            kTempCam.FitNearAndFarToBound(*pkBound);
            kDeltaFrustum.m_fNear = kTempCam.GetViewFrustum().m_fNear;
            kDeltaFrustum.m_fFar = kTempCam.GetViewFrustum().m_fFar;
        }
        else
        {
            kDeltaFrustum.m_fNear = m_pkSourceFrustum->m_fNear + (
                (m_pkDestFrustum->m_fNear - m_pkSourceFrustum->m_fNear) * 
                fAlpha);
            kDeltaFrustum.m_fFar = m_pkSourceFrustum->m_fFar + (
                (m_pkDestFrustum->m_fFar - m_pkSourceFrustum->m_fFar) * 
                fAlpha);
        }
    }
    else
    {
        // we are switching between orthographic and projection
        NiPoint3 kOffset;
        NiPoint3 kSourcePoint;
        NiPoint3 kDestPoint;
        NiMatrix3 kDestRot;

        float fOrthoScalar = 4.0f;
        float fFinalDistance = pow(10.0f, fOrthoScalar);
        if (bSrcOrtho)
            fOrthoScalar = 1.0f / fOrthoScalar;
        float fExpAlpha = pow(fAlpha, fOrthoScalar);
        float fCurrentDistance = 0.0f;
        float fSourceDistance = 0.0f;
        float fDestDistance = 0.0f;
        NiPoint3 kCurrentLocation;
        pkCamera->GetPropertyData(*ms_pkTranslateString, kCurrentLocation);

        if (bDestOrtho)
        {
            m_pkCameraDestRot->GetCol(0, (float*)&kOffset);
            kDestPoint = *m_pkCameraDestPoint - kOffset * fFinalDistance;
            kSourcePoint = *m_pkCameraSourcePoint;
            fSourceDistance = m_pkCameraSourcePoint->Length();
            fDestDistance = 1.0f;
        }
        else if (bSrcOrtho)
        {
            m_pkCameraSourceRot->GetCol(0, (float*)&kOffset);
            kSourcePoint = *m_pkCameraSourcePoint - kOffset * fFinalDistance;
            kDestPoint = *m_pkCameraDestPoint;
            fSourceDistance = 1.0f;
            fDestDistance = m_pkCameraDestPoint->Length();
        }
        kResultPoint = kSourcePoint + 
            (kDestPoint - kSourcePoint) * fExpAlpha;
        fCurrentDistance = kResultPoint.Length();

        // find point that linearly interpolates along closest line between
        // two view vectors in space
        // make this the new look at point
        NiPoint3 kD1, kD2;  // line directions

        m_pkCameraDestRot->GetCol(0, (float*)&kD1);
        m_pkCameraSourceRot->GetCol(0, (float*)&kD2);
        if (fabs(kD1.Dot(kD2)) <= NiViewMath::PARALLEL_THRESHOLD)
        {
            NiPoint3 kP1, kP2;  // line origins
            NiPoint3 kDelta;    // line between origins
            kP1 = kDestPoint;
            kP2 = kSourcePoint;
            kDelta = kP1 - kP2;

            float fDenominator = 1.0f - kD2.Dot(kD1) * kD2.Dot(kD1);
            float fNumerator1 = kDelta.Dot(kD2) * kD2.Dot(kD1) - 
                kDelta.Dot(kD1) * kD2.Dot(kD2);
            kDelta = kDelta * -1.0f;
            float fNumerator2 = kDelta.Dot(kD1) * kD1.Dot(kD2) - 
                kDelta.Dot(kD2) * kD1.Dot(kD1);

            NiPoint3 kLookAtPoint = (kP1 + kD1 * fNumerator1 / 
                fDenominator) * fAlpha + (kP2 + kD2 * fNumerator2 / 
                fDenominator) * (1.0f - fAlpha);
            NiPoint3 kLook = kLookAtPoint - kResultPoint;
            kLook.Unitize();
            NiPoint3 kLookTangent;
            NiPoint3 kLookBiTangent;
            kLookTangent = kLook.Cross(*m_pkUpAxis);
            kLookTangent.Unitize();
            kLookBiTangent = kLookTangent.Cross(kLook);
            kResultRotation.SetCol(0, (float*)&kLook);
            kResultRotation.SetCol(1, (float*)&kLookBiTangent);
            kResultRotation.SetCol(2, (float*)&kLookTangent);
        }
        else
        {
            NiQuaternion kSourceQuat;
            NiQuaternion kDestQuat;
            kSourceQuat.FromRotation(*m_pkCameraSourceRot);
            kDestQuat.FromRotation(*m_pkCameraDestRot);
            NiQuaternion kDeltaQuat = NiQuaternion::Slerp(fAlpha, 
                kSourceQuat, kDestQuat);
            kDeltaQuat.ToRotation(kResultRotation);
        }

        kDeltaFrustum.m_fLeft = (fSourceDistance * 
            m_pkSourceFrustum->m_fLeft * (1.0f - fAlpha) + 
            fDestDistance * m_pkDestFrustum->m_fLeft * fAlpha) / 
            fCurrentDistance;
        kDeltaFrustum.m_fRight = (fSourceDistance * 
            m_pkSourceFrustum->m_fRight * (1.0f - fAlpha) + 
            fDestDistance * m_pkDestFrustum->m_fRight * fAlpha) / 
            fCurrentDistance;
        kDeltaFrustum.m_fTop = (fSourceDistance * 
            m_pkSourceFrustum->m_fTop * (1.0f - fAlpha) + 
            fDestDistance * m_pkDestFrustum->m_fTop * fAlpha) / 
            fCurrentDistance;
        kDeltaFrustum.m_fBottom = (fSourceDistance * 
            m_pkSourceFrustum->m_fBottom * (1.0f - fAlpha) + 
            fDestDistance * m_pkDestFrustum->m_fBottom * fAlpha) / 
            fCurrentDistance;

        if (pkBound != NULL)
        {
            // if we have a scene bounds, use it for clip planes
            NiCamera kTempCam;
            kTempCam.SetTranslate(kResultPoint);
            kTempCam.SetRotate(kResultRotation);
            kTempCam.Update(0.0f);
            kTempCam.FitNearAndFarToBound(*pkBound);
            kDeltaFrustum.m_fNear = kTempCam.GetViewFrustum().m_fNear;
            kDeltaFrustum.m_fFar = kTempCam.GetViewFrustum().m_fFar;
        }
        else
        {
            NiPoint3 kTheoreticalPos = *m_pkCameraSourcePoint + 
                (*m_pkCameraDestPoint - *m_pkCameraSourcePoint) * fAlpha;
            kDeltaFrustum.m_fNear = (kResultPoint - kTheoreticalPos).Length();
            kDeltaFrustum.m_fFar = kDeltaFrustum.m_fNear + 
                m_pkDestFrustum->m_fFar;
        }
    }
    kDeltaFrustum.m_bOrtho = (bSrcOrtho && bDestOrtho);

    pkCamera->SetPropertyData(*ms_pkTranslateString, kResultPoint);
    pkCamera->SetPropertyData(*ms_pkRotateString, kResultRotation);
    float fFOV;
    float fAspect;
    float fWidth;
    fFOV = (NiASin(kDeltaFrustum.m_fRight) * 2.0f) * 360.0f / NI_TWO_PI;
    fAspect = (kDeltaFrustum.m_fRight / kDeltaFrustum.m_fTop);
    fWidth = (kDeltaFrustum.m_fRight - kDeltaFrustum.m_fLeft);
    pkCamera->SetPropertyData(*ms_pkFOVString, fFOV);
    pkCamera->SetPropertyData(*ms_pkAspectString, fAspect);
    pkCamera->SetPropertyData(*ms_pkWidthString, fWidth);
    pkCamera->SetPropertyData(*ms_pkOrthoString, kDeltaFrustum.m_bOrtho);
    pkCamera->SetPropertyData(*ms_pkNearClipString, kDeltaFrustum.m_fNear);
    pkCamera->SetPropertyData(*ms_pkFarClipString, kDeltaFrustum.m_fFar);

    m_pmTransitionCamera->Update(fTime, MFramework::Instance->
        ExternalAssetManager);

    if (fAlpha >= 1.0f)
    {
        if (!m_bTransitionBetweenCams)
        {
            bool bUndo = !(IsStandardCamera(m_pmTargetViewport, 
                m_pmTargetCamera));

            CommandService->BeginUndoFrame(String::Format("Transition \"{0}\" "
                "camera to new location", m_pmTargetCamera->Name));
            float fFOV;
            float fAspect;
            float fWidth;
            m_pmTargetViewport->SetCamera(m_pmTargetCamera, false);
            if (bUndo || m_bViewUndoable)
            {
                // set the initial values on the camera
                NiEntityPropertyInterface* pkCamera;
                pkCamera = m_pmTargetCamera->GetNiEntityInterface();
                pkCamera->SetPropertyData(*ms_pkTranslateString, 
                    *m_pkCameraSourcePoint);
                pkCamera->SetPropertyData(*ms_pkRotateString, 
                    *m_pkCameraSourceRot);
                fFOV = (NiASin(m_pkSourceFrustum->m_fRight) * 2.0f) * 360.0f / 
                    NI_TWO_PI;
                fAspect = (m_pkSourceFrustum->m_fRight / 
                    m_pkSourceFrustum->m_fTop);
                fWidth = (m_pkSourceFrustum->m_fRight - 
                    m_pkSourceFrustum->m_fLeft);
                pkCamera->SetPropertyData(*ms_pkFOVString, fFOV);
                pkCamera->SetPropertyData(*ms_pkAspectString, fAspect);
                pkCamera->SetPropertyData(*ms_pkWidthString, fWidth);
                pkCamera->SetPropertyData(*ms_pkOrthoString, 
                    m_pkSourceFrustum->m_bOrtho);
                pkCamera->SetPropertyData(*ms_pkNearClipString, 
                    m_pkSourceFrustum->m_fNear);
                pkCamera->SetPropertyData(*ms_pkFarClipString, 
                    m_pkSourceFrustum->m_fFar);
            }

            // set the final values on the camera
            m_pmTargetCamera->SetPropertyData(*ms_pkTranslateString, new
                MPoint3(*m_pkCameraDestPoint), 0, true);
            m_pmTargetCamera->SetPropertyData(*ms_pkRotateString, new
                MMatrix3(*m_pkCameraDestRot), 0, true);
            fFOV = (NiASin(m_pkDestFrustum->m_fRight) * 2.0f) * 360.0f / 
                NI_TWO_PI;
            fAspect = (m_pkDestFrustum->m_fRight / 
                m_pkDestFrustum->m_fTop);
            fWidth = (m_pkDestFrustum->m_fRight - m_pkDestFrustum->m_fLeft);
            m_pmTargetCamera->SetPropertyData(*ms_pkFOVString, __box(fFOV), 
                0, true);
            m_pmTargetCamera->SetPropertyData(*ms_pkAspectString, 
                __box(fAspect), 0, true);
            m_pmTargetCamera->SetPropertyData(*ms_pkWidthString, 
                __box(fWidth), 0, true);
            m_pmTargetCamera->SetPropertyData(*ms_pkOrthoString, 
                __box(m_pkDestFrustum->m_bOrtho), 0, true);
            m_pmTargetCamera->SetPropertyData(*ms_pkNearClipString, 
                __box(m_pkDestFrustum->m_fNear), 0, true);
            m_pmTargetCamera->SetPropertyData(*ms_pkFarClipString, 
                __box(m_pkDestFrustum->m_fFar), 0, true);

            CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
        }
        else
        {
            CommandService->BeginUndoFrame(String::Format("Transition viewport"
                " to \"{0}\" camera", m_pmTargetCamera->Name));
            if (m_bViewUndoable)
            {
                m_pmTargetViewport->SetCamera(m_pmSourceCamera, false);
            }

            m_pmTargetViewport->SetCamera(m_pmTargetCamera, m_bViewUndoable);
            CommandService->EndUndoFrame(m_bViewUndoable);
        }
        // we have completed the transition
        m_bCamTransitioning = false;
    }
}
//---------------------------------------------------------------------------
void MCameraManager::CreateCameraHelper(StandardCamera eCameraType,
    bool bOrtho, NiPoint3* pkTranslate)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(__box(eCameraType)->ToString());
    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterface* pkEntity = NiNew NiGeneralEntity(pcName, kTemplateID,
        2);
    MFreeCharPointer(pcName);
    NiMatrix3 kRotation;
    if (pkTranslate->Cross(*m_pkUpAxis).Length() != 0.0f)
    {
        kRotation = NiViewMath::LookAt(NiPoint3::ZERO, *pkTranslate, 
            *m_pkUpAxis);
    }
    else
    {
        NiPoint3 kNewUpAxis;
        if ((pkTranslate->Dot(NiPoint3::UNIT_Z) < 
            NiViewMath::PARALLEL_THRESHOLD) &&
            (pkTranslate->Dot(NiPoint3::UNIT_Z) > 
            -NiViewMath::PARALLEL_THRESHOLD))
        {
            kNewUpAxis = NiPoint3::UNIT_Z;
        }
        else if ((pkTranslate->Dot(NiPoint3::UNIT_Y) < 
            NiViewMath::PARALLEL_THRESHOLD) &&
            (pkTranslate->Dot(NiPoint3::UNIT_Y) >
            -NiViewMath::PARALLEL_THRESHOLD))
        {
            kNewUpAxis = NiPoint3::UNIT_Y;
        }
        else
        {
            kNewUpAxis = NiPoint3::UNIT_X;
        }
        kRotation = NiViewMath::LookAt(NiPoint3::ZERO, *pkTranslate, 
            kNewUpAxis);
    }
    NiTransformationComponent* pkTransComp =
        NiNew NiTransformationComponent(*pkTranslate, kRotation, 1.0f);
    NIVERIFY(pkEntity->AddComponent(pkTransComp));

    NiCameraComponent* pkCameraComp = NiNew NiCameraComponent();
    NIVERIFY(pkEntity->AddComponent(pkCameraComp));

    // set the properties on the camera component
    pkCameraComp->SetPropertyData(*ms_pkAspectString, 1.0f, 0);
    pkCameraComp->SetPropertyData(*ms_pkOrthoString, bOrtho, 0);
    if (bOrtho)
    {
        pkCameraComp->SetPropertyData(*ms_pkWidthString, 100.0f, 0);
    }
    else
    {
        pkCameraComp->SetPropertyData(*ms_pkFOVString, 90.0f, 0);
    }

    MEntity* pmCamera = MEntityFactory::Instance->Get(pkEntity);
    AddCameraToAllViewports(pmCamera);

    // Create the default camera transform array, if it doesn't exist.
    if (m_amDefaultCameraTransforms == NULL)
    {
        m_amDefaultCameraTransforms = new CameraTransform*[Enum::GetValues(
            __typeof(StandardCamera))->Length];
    }
    if (m_amDefaultCameraTransforms[(int) eCameraType] == NULL)
    {
        NiPoint3 kTranslation;
        NIVERIFY(pkTransComp->GetPropertyData(
            NiTransformationComponent::PROP_TRANSLATION(), kTranslation, 0));

        NiMatrix3 kRotation;
        NIVERIFY(pkTransComp->GetPropertyData(
            NiTransformationComponent::PROP_ROTATION(), kRotation, 0));

        m_amDefaultCameraTransforms[(int) eCameraType] = new CameraTransform(
            new MPoint3(kTranslation),
            new MMatrix3(kRotation));
    }
}
//---------------------------------------------------------------------------
void MCameraManager::ResetAllDefaultCameraTransforms()
{
    MVerifyValidInstance;

    // Only reset if the array exists.
    if (m_amDefaultCameraTransforms == NULL)
    {
        return;
    }

    // Build array of valid enum values.
    Array* pmStandardCameraValues = Enum::GetValues(__typeof(StandardCamera));
    StandardCamera aeStandardCameraValues[] = new StandardCamera[
        pmStandardCameraValues->Length];
    for (int i = 0; i < pmStandardCameraValues->Length; i++)
    {
        aeStandardCameraValues[i] = *dynamic_cast<__box StandardCamera*>(
            pmStandardCameraValues->GetValue(i));
    }

    // For each viewport camera, reset each of its transforms to the default.
    MViewportManager* pmViewportManager = MViewportManager::Instance;
    MAssert(pmViewportManager != NULL, "Viewport manager does not exist!");
    for (unsigned int ui = 0; ui < pmViewportManager->ViewportCount; ui++)
    {
        MViewport* pmViewport = pmViewportManager->GetViewport(ui);

        for (int i = 0; i < aeStandardCameraValues->Length; i++)
        {
            MEntity* pmCamera = GetStandardCameraEntity(pmViewport,
                aeStandardCameraValues[i]);
            CameraTransform* pmCameraTransform = m_amDefaultCameraTransforms[
                (int) aeStandardCameraValues[i]];
            pmCamera->SetPropertyData(*ms_pkTranslateString,
                pmCameraTransform->m_pmTranslation, true);
            pmCamera->SetPropertyData(*ms_pkRotateString,
                pmCameraTransform->m_pmRotation, true);
        }
    }
    MoveDefaultCamerasOutOfBounds();
}
//---------------------------------------------------------------------------
void MCameraManager::AddCameraToAllViewports(MEntity* pmCamera)
{
    MVerifyValidInstance;

    MViewportManager* pmViewportManager = MViewportManager::Instance;
    MAssert(pmViewportManager != NULL, "Viewport manager does not exist!");
    for (unsigned int ui = 0; ui < pmViewportManager->ViewportCount; ui++)
    {
        MEntity* pmClone;
        if (ui == 0)
        {
            pmClone = pmCamera;
        }
        else
        {
            MEntity* amClones[] = pmCamera->Clone(pmCamera->Name, false);
            NIASSERT(amClones->Count == 1);
            pmClone = amClones[0];
        }

        pmViewportManager->GetViewport(ui)->ToolScene->AddEntity(pmClone,
            false);
    }
}
//---------------------------------------------------------------------------
void MCameraManager::MoveDefaultCamerasOutOfBounds()
{
    // here we adjust the translation of standard orthographic cameras
    // to be outside of the scene bounds

    Array* pmStandardCameraValues = Enum::GetValues(__typeof(StandardCamera));
    StandardCamera aeStandardCameraValues[] = new StandardCamera[
        pmStandardCameraValues->Length];
    for (int i = 0; i < pmStandardCameraValues->Length; i++)
    {
        aeStandardCameraValues[i] = *dynamic_cast<__box StandardCamera*>(
            pmStandardCameraValues->GetValue(i));
    }

    MViewportManager* pmViewportManager = MViewportManager::Instance;
    MAssert(pmViewportManager != NULL, "Viewport manager does not exist!");
    for (unsigned int ui = 0; ui < pmViewportManager->ViewportCount; ui++)
    {
        MViewport* pmViewport = pmViewportManager->GetViewport(ui);
        NiBound kBound;
        kBound = *(MFramework::Instance->BoundManager->GetToolSceneBound(
            pmViewport));

        for (int i = 0; i < aeStandardCameraValues->Length; i++)
        {
            MEntity* pmCamera = GetStandardCameraEntity(pmViewport,
                aeStandardCameraValues[i]);
            NiEntityPropertyInterface* pkCameraEntity = 
                pmCamera->GetNiEntityInterface();
            bool bOrtho;
            pkCameraEntity->GetPropertyData(*ms_pkOrthoString, bOrtho);
            if (bOrtho)
            {
                NiObject* pkObject;
                pkCameraEntity->GetPropertyData(*ms_pkSceneRootString, 
                    pkObject, 0);
                NiCamera* pkCamera = NiDynamicCast(NiCamera, pkObject);

                NiPoint3 kOrigin;
                pkCameraEntity->GetPropertyData(*ms_pkTranslateString, 
                    kOrigin);
                NiMatrix3 kRotation;
                pkCameraEntity->GetPropertyData(*ms_pkRotateString, kRotation);
                NiPoint3 kCenter;
                kCenter = kBound.GetCenter();
                float fRadius;
                fRadius = kBound.GetRadius();
                NiPoint3 kDir;
                kRotation.GetCol(0, kDir);

                // project pt onto plane tangent to the sphere and our kDir
                NiPoint3 kTangentPoint;
                kTangentPoint = kCenter - fRadius * kDir;
                float fDistance = (kTangentPoint - kOrigin).Dot(kDir);
                NiPoint3 kDestination;
                kDestination = kOrigin + fDistance * kDir;
                pkCamera->SetWorldTranslate(kDestination);
                pkCamera->FitNearAndFarToBound(kBound);
                // now move the camera to that location and adjust
                pkCameraEntity->SetPropertyData(*ms_pkTranslateString, 
                    kDestination);
                float fNear, fFar;
                fNear = pkCamera->GetViewFrustum().m_fNear;
                fFar = pkCamera->GetViewFrustum().m_fFar;
                pkCameraEntity->SetPropertyData(*ms_pkNearClipString, fNear);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MCameraManager::SetAspectRatioOnStandardCameras(MViewport* pmViewport,
    float fAspectRatio)
{
    MVerifyValidInstance;

    MAssert(pmViewport != NULL, "Null viewport provided to function!");

    __box float* pfAspectRatio = __box(fAspectRatio);

    String* astrStandardCameraNames[] = Enum::GetNames(
        __typeof(StandardCamera));
    for (int i = 0; i < astrStandardCameraNames->Length; i++)
    {
        String* strStandardCameraName = astrStandardCameraNames[i];
        MEntity* pmCamera = pmViewport->ToolScene->GetEntityByName(
            strStandardCameraName);
        MAssert(pmCamera != NULL, "Camera not found in viewport!");

        MAssert(pmCamera->HasProperty(ms_strAspectRatioName), "Camera does "
            "not have aspect ratio property!");
        pmCamera->SetPropertyData(ms_strAspectRatioName, pfAspectRatio,
            false);
    }
    
    MAssert(m_pmTransitionCamera != NULL, "Transition camera does not "
        "exist!");
    MAssert(m_pmTransitionCamera->HasProperty(ms_strAspectRatioName),
        "Camera does not have aspect ratio property!");
    m_pmTransitionCamera->SetPropertyData(ms_strAspectRatioName,
        pfAspectRatio, false);
}
//---------------------------------------------------------------------------
bool MCameraManager::IsAxisAlignedStandardCamera(MViewport* pmViewport,
    MEntity* pmCamera)
{
    MVerifyValidInstance;

    if (pmCamera == GetStandardCameraEntity(pmViewport,
            StandardCamera::PositiveX) ||
        pmCamera == GetStandardCameraEntity(pmViewport,
            StandardCamera::NegativeX) ||
        pmCamera == GetStandardCameraEntity(pmViewport,
            StandardCamera::PositiveY) ||
        pmCamera == GetStandardCameraEntity(pmViewport,
            StandardCamera::NegativeY) ||
        pmCamera == GetStandardCameraEntity(pmViewport,
            StandardCamera::PositiveZ) ||
        pmCamera == GetStandardCameraEntity(pmViewport,
            StandardCamera::NegativeZ))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool MCameraManager::IsStandardCamera(MViewport* pmViewport, MEntity* pmCamera)
{
    if (IsAxisAlignedStandardCamera(pmViewport, pmCamera) ||
        (pmCamera == GetStandardCameraEntity(pmViewport, 
        StandardCamera::User)) || (pmCamera == GetStandardCameraEntity(
        pmViewport, StandardCamera::Perspective)))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
ISettingsService* MCameraManager::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found.");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
ICommandService* MCameraManager::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found.");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
MCameraManager::CameraTransform::CameraTransform(MPoint3* pmTranslation,
    MMatrix3* pmRotation) : m_pmTranslation(pmTranslation),
    m_pmRotation(pmRotation)
{
}
//---------------------------------------------------------------------------
