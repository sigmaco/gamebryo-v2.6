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
#include "StdPluginsCppPCH.h"

#include "MRotationInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

#define ENSURE_GIZMO_LOADED() if (!m_pkGizmo) \
    { \
        if (LoadGizmo() == false) \
        { \
            NIASSERT(!"MRotationInteractionMode could not load " \
                "..\\..\\Data\\rotate.nif"); \
        } \
    }

//---------------------------------------------------------------------------
MRotationInteractionMode::MRotationInteractionMode() : m_pkXAxisName(NULL),
    m_pkYAxisName(NULL), m_pkZAxisName(NULL), m_pkXLineName(NULL), 
    m_pkYLineName(NULL), m_pkZLineName(NULL), 
    m_pkHighLightColor(NULL), m_pkInitialRotation(NULL), 
    m_pkInitialRotationUnique(NULL), m_pkInitialTranslation(NULL),
    m_pkInitialTranslationUnique(NULL), m_pmPreviousSelection(NULL),
    m_pmCloneArray(NULL), m_pkCloneCenter(NULL)
{
    m_pkXAxisName = NiNew NiFixedString("XAxis");
    m_pkYAxisName = NiNew NiFixedString("YAxis");
    m_pkZAxisName = NiNew NiFixedString("ZAxis");
    m_pkXLineName = NiNew NiFixedString("XLine");
    m_pkYLineName = NiNew NiFixedString("YLine");
    m_pkZLineName = NiNew NiFixedString("ZLine");
    m_pkInitialRotation = NiNew NiTObjectSet<NiMatrix3>(32);
    m_pkInitialRotationUnique = NiNew NiTPrimitiveSet<bool>(32);
    m_pkInitialTranslation = NiNew NiTObjectSet<NiPoint3>(32);
    m_pkInitialTranslationUnique = NiNew NiTPrimitiveSet<bool>(32);
    m_pmPreviousSelection = new ArrayList(1);
    m_pmCloneArray = new CloneArray();
    m_pkCloneCenter = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
    m_pkRotationLine = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MRotationInteractionMode::OnSettingChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::HIGHLIGHT_COLOR,
        *m_pkHighLightColor, pmHandler);
    m_fRadianSnap *= 360.0f / NI_TWO_PI;
    MSettingsHelper::GetStandardSetting(MSettingsHelper::ROTATION_SNAP,
        m_fRadianSnap, pmHandler);
    m_fRadianSnap *= NI_TWO_PI / 360.0f;
    MSettingsHelper::GetStandardSetting(MSettingsHelper::ROTATION_SNAP_ENABLED,
        m_bSnapEnabled, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_PRECISION,
        m_fTranslationPrecision, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED, m_bPrecisionEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::ALWAYS_USE_LINEAR_ROTATION, m_bLinearMode, pmHandler);
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::OnSettingChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::HIGHLIGHT_COLOR))
    {
        Object* pmObj;
        __box Color* pmValue;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        pmValue = dynamic_cast<__box Color*>(pmObj);
        if (pmValue != NULL)
        {
            m_pkHighLightColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkHighLightColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkHighLightColor->b = MUtility::RGBToFloat((*pmValue).B);
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ROTATION_SNAP))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            // we must convert this one from degrees to radians
            m_fRadianSnap = (*pfVal) * NI_TWO_PI / 360.0f;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ROTATION_SNAP_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fTranslationPrecision = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bPrecisionEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ALWAYS_USE_LINEAR_ROTATION))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bLinearMode = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);

    NiDelete m_pkXAxisName;
    NiDelete m_pkYAxisName;
    NiDelete m_pkZAxisName;
    NiDelete m_pkXLineName;
    NiDelete m_pkYLineName;
    NiDelete m_pkZLineName;
    m_pkInitialRotation->RemoveAll();
    NiDelete m_pkInitialRotation;
    m_pkInitialRotationUnique->RemoveAll();
    NiDelete m_pkInitialRotationUnique;
    m_pkInitialTranslation->RemoveAll();
    NiDelete m_pkInitialTranslation;
    m_pkInitialTranslationUnique->RemoveAll();
    NiDelete m_pkInitialTranslationUnique;

    NiDelete m_pkHighLightColor;
    NiDelete m_pkPick;

    NiDelete m_pkCloneCenter;
    NiDelete m_pkRotationLine;
}
//---------------------------------------------------------------------------
String* MRotationInteractionMode::get_Name()
{
    return "GamebryoRotation";
}
//---------------------------------------------------------------------------
bool MRotationInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_eAxis = RotateAxis::AXIS_Z;
    m_eCurrentAxis = RotateAxis::AXIS_Z;
    m_bAlreadyRotating = false;
    m_fDefaultDistance = STANDARD_DISTANCE;
    m_fRadianSnap = 10.0f * NI_TWO_PI / 360.0f;
    m_bSnapEnabled = true;
    m_fTranslationPrecision = 0.1f;
    m_bPrecisionEnabled = true;
    m_bLinearMode = false;

    m_pkHighLightColor = NiNew NiColor(1.0f, 1.0f, 0.0f);
    m_pkPick = NiNew NiPick();

    // Loading of NIF file is deferred

    return true;
}
//---------------------------------------------------------------------------
bool MRotationInteractionMode::LoadGizmo()
{
    String* pmPath = String::Concat(MFramework::Instance->AppStartupPath,
        "..\\..\\Data\\rotate.nif");
    NiStream kStream;
    const char* pcPath = MStringToCharPointer(pmPath);
    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream, pcPath);
    MFreeCharPointer(pcPath);

    if (iSuccess != 0)
    {
        m_pkGizmo = (NiNode*)kStream.GetObjectAt(0);
        MInitRefObject(m_pkGizmo);
        m_pkGizmo->UpdateProperties();
        m_pkGizmo->UpdateEffects();
        m_pkGizmo->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_pkGizmo);
    }
    else
    {
        MessageService->AddMessage(MessageChannelType::Errors, 
            "Failed to load rotatation gizmo file 'rotation.nif'");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::SetInteractionMode(Object*, 
    EventArgs*)
{
    MVerifyValidInstance;

    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
}
//---------------------------------------------------------------------------
bool MRotationInteractionMode::CanTransform()
{
    MEntity* amEntities[] = SelectionService->GetSelectedEntities();
    for (int i = 0; i < amEntities->Count; ++i)
    {
        if (!amEntities[i]->Writable)
        {
            return false;
        }
    }
    for (int i = 0; i < amEntities->Count; ++i)
    {
        if (amEntities[i] != NULL &&
            amEntities[i]->HasProperty(*m_pkTranslationName) &&
            amEntities[i]->HasProperty(*m_pkRotationName))
        {
            if (!amEntities[i]->IsPropertyReadOnly(*m_pkTranslationName) &&
                !amEntities[i]->IsPropertyReadOnly(*m_pkRotationName))
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::RotateHelper(
    const NiPoint3* pkOrigin, const NiPoint3* pkDir)
{
    MVerifyValidInstance;

    float fCurrentRadians;
    NiMatrix3 kRotation;

    fCurrentRadians = GetRadians(pkOrigin, pkDir);
    
    if (m_bSnapEnabled)
    {
        // if snap is on, we need to construct a rotation such that it is 
        // rounded to the nearest m_fRadianSnap relative to the starting
        // rotation
        m_fCurrentRadians = m_fStartingRadians - fCurrentRadians;
        // round fTotalRotation to the nearest m_fRadianSnap
        m_fCurrentRadians /= m_fRadianSnap;
        m_fCurrentRadians = NiFloor(m_fCurrentRadians + 0.5f);
        m_fCurrentRadians *= m_fRadianSnap;
    }
    else
    {
        // if snap is off, just build a rotation matrix using starting rotation
        m_fCurrentRadians = m_fStartingRadians - fCurrentRadians;
    }
    if (m_eAxis == AXIS_X)
        kRotation.MakeXRotation(m_fCurrentRadians);
    else if (m_eAxis == AXIS_Y)
        kRotation.MakeYRotation(m_fCurrentRadians);
    else if (m_eAxis == AXIS_Z)
        kRotation.MakeZRotation(m_fCurrentRadians);

    if (m_bCloning)
    {
        // if cloning, transform the clones
        unsigned int uiIndex = 0;
        CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
        while (en.MoveNext())
        {
            EntityArray amEntities = en.Current;
            NIASSERT(amEntities->Length > 0);
            MEntity* pmEntity = amEntities[0];
            if (pmEntity == NULL ||
                !pmEntity->HasProperty(*m_pkTranslationName) ||
                !pmEntity->HasProperty(*m_pkRotationName))
            {
                continue;
            }

            NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();
            NiMatrix3 kCurrentRotation;
            kCurrentRotation = kRotation * m_pkInitialRotation->GetAt(
                uiIndex);
            pkEntity->SetPropertyData(*m_pkRotationName, kCurrentRotation);

            // now translate the object so it orbits about the center
            NiPoint3 kCurrentPosition;
            NiPoint3 kDelta;
            kDelta = m_pkInitialTranslation->GetAt(uiIndex) -
                *m_pkCloneCenter;
            kDelta = kRotation * kDelta;
            kCurrentPosition = *m_pkCloneCenter + kDelta;
            if (m_bPrecisionEnabled)
            {
                // if precision is turned on, we must round translation to
                // the nearest m_fTranslationPrecision;
                kCurrentPosition /= m_fTranslationPrecision;
                kCurrentPosition.x = NiFloor(kCurrentPosition.x + 0.5f);
                kCurrentPosition.y = NiFloor(kCurrentPosition.y + 0.5f);
                kCurrentPosition.z = NiFloor(kCurrentPosition.z + 0.5f);
                kCurrentPosition *= m_fTranslationPrecision;
            }
            pkEntity->SetPropertyData(*m_pkTranslationName, 
                kCurrentPosition);
            uiIndex++;
        }
    }
    else
    {
        MEntity* amEntities[] = SelectionService->GetSelectedEntities();
        unsigned int uiIndex = 0;
        for (int i = 0; i < amEntities->Count; i++)
        {
            if (amEntities[i] != NULL &&
                amEntities[i]->HasProperty(*m_pkTranslationName) &&
                amEntities[i]->HasProperty(*m_pkRotationName))
            {
                NiEntityInterface* pkEntity = amEntities[i]->
                    GetNiEntityInterface();
                NiMatrix3 kCurrentRotation;
                kCurrentRotation = kRotation * m_pkInitialRotation->GetAt(
                    uiIndex);
                pkEntity->SetPropertyData(*m_pkRotationName, kCurrentRotation);
                
                // now translate the object so it orbits about the center
                NiPoint3 kCurrentPosition;
                NiPoint3 kCenter;
                NiPoint3 kDelta;
                SelectionService->SelectionCenter->ToNiPoint3(kCenter);
                kDelta = m_pkInitialTranslation->GetAt(uiIndex) - kCenter;
                kDelta = kRotation * kDelta;
                kCurrentPosition = kCenter + kDelta;
                if (m_bPrecisionEnabled)
                {
                    // if precision is turned on, we must round translation to 
                    // the nearest m_fTranslationPrecision;
                    kCurrentPosition /= m_fTranslationPrecision;
                    kCurrentPosition.x = NiFloor(kCurrentPosition.x + 0.5f);
                    kCurrentPosition.y = NiFloor(kCurrentPosition.y + 0.5f);
                    kCurrentPosition.z = NiFloor(kCurrentPosition.z + 0.5f);
                    kCurrentPosition *= m_fTranslationPrecision;
                }
                pkEntity->SetPropertyData(*m_pkTranslationName, 
                    kCurrentPosition);
                uiIndex++;
            }
        }
    }
}
//---------------------------------------------------------------------------
float MRotationInteractionMode::GetRadians(const NiPoint3* pkOrigin, 
    const NiPoint3* pkDir)
{
    ENSURE_GIZMO_LOADED();

    if (RotateLinear(m_eAxis))
    {
        NiCamera* pkCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera();
        NiPoint3 kDelta;
        kDelta = NiViewMath::TranslateOnAxis(m_pkGizmo->GetTranslate(), 
            *m_pkRotationLine, *pkOrigin, *pkDir);
        // reduce delta proportional to gizmo scale
        float fDistance;
        if (pkCamera->GetViewFrustum().m_bOrtho)
        {
            NiFrustum pkFrustum = pkCamera->GetViewFrustum();
            fDistance = ((pkFrustum.m_fRight * 2.0f) / 
                m_fDefaultDistance);
        }
        else
        {
            fDistance = (((pkCamera->GetTranslate() - 
                m_pkGizmo->GetTranslate()).Length() / m_fDefaultDistance) *
                pkCamera->GetViewFrustum().m_fRight * 2.0f);
        }
        float fRadians = kDelta.Dot(*m_pkRotationLine) / fDistance;

        return (fRadians);
    }
    else
    {
        NiPoint3 kCenter;
        NiPoint3 kAxis;
        NiPoint3 kTangent;
        NiPoint3 kBiTangent;

        if (m_bCloning)
        {
            kCenter = *m_pkCloneCenter;
        }
        else
        {
            SelectionService->SelectionCenter->ToNiPoint3(kCenter);
        }

        if (m_eAxis == AXIS_X)
        {
            kAxis = NiPoint3::UNIT_X;
            kTangent = NiPoint3::UNIT_Y;
            kBiTangent = NiPoint3::UNIT_Z;
        }
        else if (m_eAxis == AXIS_Y)
        {
            kAxis = NiPoint3::UNIT_Y;
            kTangent = NiPoint3::UNIT_Z;
            kBiTangent = NiPoint3::UNIT_X;
        }
        else if (m_eAxis == AXIS_Z)
        {
            kAxis = NiPoint3::UNIT_Z;
            kTangent = NiPoint3::UNIT_X;
            kBiTangent = NiPoint3::UNIT_Y;
        }

        return (NiViewMath::RotateAboutAxis(kCenter, kAxis, kTangent, 
            kBiTangent, *pkOrigin, *pkDir));
    }
}
//---------------------------------------------------------------------------
bool MRotationInteractionMode::RotateLinear(const RotateAxis eAxis)
{
    // This method should tell the RotateHelper whether to use linear
    // or radial rotation based off of the setting and the view vs. axis
    if (m_bLinearMode)
    {
        return true;
    }
    else
    {
        //if the linear setting is not turned on, we must determine if the
        //axis is perpendicular to the view vector
        NiPoint3 kAxisDirection;
    
        if (eAxis == RotateAxis::AXIS_X)
        {
            kAxisDirection = NiPoint3::UNIT_X;
        }
        else if (eAxis == RotateAxis::AXIS_Y)
        {
            kAxisDirection = NiPoint3::UNIT_Y;
        }
        else if (eAxis == RotateAxis::AXIS_Z)
        {
            kAxisDirection = NiPoint3::UNIT_Z;
        }

        NiCamera* pkCam;
        NiPoint3 kLook;
        float fCosine;

        pkCam = MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera();
        pkCam->GetRotate().GetCol(0, (float*)&kLook);
        fCosine = kLook.Dot(kAxisDirection);

        // check if the rotation axis is perpendicular to the view
        if ((fCosine <= NiViewMath::INV_PARALLEL_THRESHOLD * 5.0f) && 
            (fCosine >= -NiViewMath::INV_PARALLEL_THRESHOLD * 5.0f))
        {
            return true;
        }
        return false;
    }
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::HighLightAxis(const RotateAxis eAxis)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    // first, reset everything to it's default
    NiAVObject* pkLineX;
    NiAVObject* pkLineY;
    NiAVObject* pkLineZ;

    pkLineX = m_pkGizmo->GetObjectByName(*m_pkXLineName);
    pkLineY = m_pkGizmo->GetObjectByName(*m_pkYLineName);
    pkLineZ = m_pkGizmo->GetObjectByName(*m_pkZLineName);
    NIASSERT(pkLineX);
    NIASSERT(pkLineY);
    NIASSERT(pkLineZ);

    NiMaterialProperty* pkMaterial;
    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineX->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineY->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineZ->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    if (eAxis == RotateAxis::AXIS_X)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == RotateAxis::AXIS_Y)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == RotateAxis::AXIS_Z)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }

    m_pkGizmo->UpdateProperties();
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::SetupRotation(int iX, int iY)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;


    NiPoint3 kOrigin, kDir;
    NiCamera* pkCamera;
    pkCamera = 
        MFramework::Instance->ViewportManager->ActiveViewport->GetNiCamera();
    NiViewMath::MouseToRay((float) iX, (float) iY, 
        MFramework::Instance->ViewportManager->ActiveViewport->Width,
        MFramework::Instance->ViewportManager->ActiveViewport->Height,
        pkCamera, kOrigin, kDir);

    if (RotateLinear(m_eAxis))
    {
        NiPoint3 kAxis;
        if (m_eAxis == AXIS_X)
        {
            kAxis = NiPoint3::UNIT_X;
        }
        else if (m_eAxis == AXIS_Y)
        {
            kAxis = NiPoint3::UNIT_Y;
        }
        else if (m_eAxis == AXIS_Z)
        {
            kAxis = NiPoint3::UNIT_Z;
        }

        // project our mouse click on to gizmo axis plane and find the 
        // delta vector between the gizmo center and projection result
        NiPoint3 kLook;
        float fCosine;
        NiPoint3 kDelta;

        pkCamera->GetRotate().GetCol(0, (float*)&kLook);
        fCosine = kLook.Dot(kAxis);

        // check if the rotation axis is perpendicular to the view
        if ((fCosine <= NiViewMath::INV_PARALLEL_THRESHOLD * 5.0f) && 
            (fCosine >= -NiViewMath::INV_PARALLEL_THRESHOLD * 5.0f))
        {
            kDelta = -kLook;
        }
        else
        {
            kDelta = NiViewMath::TranslateOnPlane(m_pkGizmo->GetTranslate(), 
                kAxis, kOrigin, kDir);
        }

        NiPoint3 kTangent;
        // the cross product of delta and axis gives us the tangent vector
        kTangent = kAxis.Cross(kDelta);
        // now project the tangent vector parallel to the view plane
        *m_pkRotationLine = kTangent - kDir * kDir.Dot(kTangent);
        m_pkRotationLine->Unitize();
        // the initial linear offset accounts for the place that the user
        // initially clicks is not at the origin
        m_fStartingRadians = GetRadians(&kOrigin, &kDir);
    }
    else
    {
        // record the most recent angle
        m_fStartingRadians = GetRadians(&kOrigin, &kDir);
    }
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::SetGizmoScale(NiCamera* pkCamera)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;


    // figure out the gizmo's location
    NiPoint3 kCenter;
    if (m_bCloning)
    {
        kCenter = *m_pkCloneCenter;
    }
    else
    {
        SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    }
    m_pkGizmo->SetTranslate(kCenter);
    // figure out how large it needs to be
    if (pkCamera->GetViewFrustum().m_bOrtho)
    {
        NiFrustum pkFrustum = pkCamera->GetViewFrustum();
        m_pkGizmo->SetScale((pkFrustum.m_fRight * 2.0f) / 
            m_fDefaultDistance);
    }
    else
    {
        float fCamDistance = (kCenter - 
            pkCamera->GetWorldTranslate()).Length();
        if ((fCamDistance / m_fDefaultDistance) > 0.0f)
        {
            m_pkGizmo->SetScale((fCamDistance / m_fDefaultDistance) * 
                pkCamera->GetViewFrustum().m_fRight * 2.0f);
        }
        else
        {
            m_pkGizmo->SetScale(1.0f);
        }
    }
    m_pkGizmo->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkGizmo);
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::Update(float fTime)
{
    MVerifyValidInstance;

    __super::Update(fTime);

    if (m_bCloning)
    {
        CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
        while (en.MoveNext())
        {
            EntityArray amEntities = en.Current;
            for (int i = 0; i < amEntities->Length; ++i)
            {
                MEntity* pmEntity = amEntities[i];
                pmEntity->Update(
                    MFramework::Instance->TimeManager->CurrentTime,
                    MFramework::Instance->ExternalAssetManager);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;


    if (((CanTransform()) || (m_bCloning)))
    {
        NiEntityRenderingContext* pkContext = 
            pmRenderingContext->GetRenderingContext();

        if (m_bCloning)
        {
            // if we are cloning, draw a ghost of the new objects
            GhostRenderingMode->Begin(pmRenderingContext);
            CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
            while (en.MoveNext())
            {
                EntityArray amEntities = en.Current;
                for (int i = 0; i < amEntities->Length; ++i)
                {
                    MEntity* pmEntity = amEntities[i];
                    GhostRenderingMode->Render(pmEntity, pmRenderingContext);
                }
            }
            GhostRenderingMode->End(pmRenderingContext);
        }

        //clear the z-buffer
        pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

        SetGizmoScale(pkContext->m_pkCamera);
        NiDrawScene(pkContext->m_pkCamera, m_pkGizmo, 
            *pkContext->m_pkCullingProcess);
    }
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::MouseDown(MouseButtonType eType, int iX,
    int iY)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;


    if (eType == MouseButtonType::LeftButton)
    {
        // use these to tell if we have moved the mouse far enough
        m_iMouseX = iX;
        m_iMouseY = iY;

        NiPoint3 kOrigin, kDir;
        NiViewMath::MouseToRay((float) iX, (float) iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);

        SetGizmoScale(MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera());
        m_pkPick->SetTarget(m_pkGizmo);
        if ((!CanTransform()) || 
            (!m_pkPick->PickObjects(kOrigin, kDir, false)))
        {
            // pass the call off to selection mode, it handles if we are or
            // aren't clicking on an object
            __super::MouseDown(eType, iX, iY);
        }
        else
        {
            const NiPick::Results& kPickResults = m_pkPick->GetResults();
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                // set current axis
                NiFixedString kName = pkPickedObject->GetName();
                if (kName == *m_pkXAxisName)
                    m_eCurrentAxis = AXIS_X;
                else if (kName == *m_pkYAxisName)
                    m_eCurrentAxis = AXIS_Y;
                else if (kName == *m_pkZAxisName)
                    m_eCurrentAxis = AXIS_Z;

                // set axis colors appropriately
                HighLightAxis(m_eCurrentAxis);
            }
            // set the working axis to the one we are hovering over
            m_eAxis = m_eCurrentAxis;
            m_bLeftDown = true;
        }

        if (CanTransform())
        {
            // set gizmo scale appropriately
            NiCamera* pkCam = MFramework::Instance->ViewportManager->
                ActiveViewport->GetNiCamera();

            SetGizmoScale(pkCam);
            SetupRotation(iX, iY);
        }
    }
    else
    {
        if ((m_bAlreadyRotating) && (eType == MouseButtonType::RightButton))
        {
            if (m_bCloning)
            {
                // if user cancels a clone, we must revert to old selection
                SelectionService->AddEntitiesToSelection(static_cast<
                    MEntity*[]>(m_pmPreviousSelection->ToArray(__typeof(
                    MEntity))));
                // detach the clones from scene lights
                CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
                while (en.MoveNext())
                {
                    EntityArray amEntities = en.Current;
                    MFramework::Instance->LightManager->
                        RemoveEntitiesFromDefaultLights(amEntities);
                    for (int i = 0; i < amEntities->Length; i++)
                    {
                        MFramework::Instance->EntityFactory->Remove(
                            amEntities[i]->GetNiEntityInterface());
                    }
                }
                m_pmPreviousSelection->Clear();
                m_pmCloneArray->Clear();
                MFramework::Instance->Scene->UpdateEffects();
                m_bCloning = false;
            }
            else
            {
                MEntity* amEntities[] = SelectionService
                    ->GetSelectedEntities();
                unsigned int uiIndex = 0;
                for (int i = 0; i < amEntities->Count; i++)
                {
                    if (amEntities[i] != NULL &&
                        amEntities[i]->HasProperty(*m_pkTranslationName) &&
                        amEntities[i]->HasProperty(*m_pkRotationName))
                    {
                        NiEntityInterface* pkEntity = amEntities[i]->
                            GetNiEntityInterface();
                        if (!amEntities[i]->IsPropertyReadOnly(
                            *m_pkRotationName))
                        {
                            pkEntity->SetPropertyData(*m_pkRotationName, 
                                m_pkInitialRotation->GetAt(uiIndex));

                            if (!m_pkInitialRotationUnique->GetAt(uiIndex))
                            {
                                pkEntity->ResetProperty(*m_pkRotationName);
                            }
                        }

                        if (!amEntities[i]->IsPropertyReadOnly(
                            *m_pkTranslationName))
                        {
                            pkEntity->SetPropertyData(*m_pkTranslationName, 
                                m_pkInitialTranslation->GetAt(uiIndex));

                            if (!m_pkInitialTranslationUnique->GetAt(uiIndex))
                            {
                                pkEntity->ResetProperty(*m_pkTranslationName);
                            }
                        }
                        uiIndex++;
                    }
                }
            }

            m_pkInitialRotation->RemoveAll();
            m_pkInitialRotationUnique->RemoveAll();
            m_pkInitialTranslation->RemoveAll();
            m_pkInitialTranslationUnique->RemoveAll();
            m_bAlreadyRotating = false;
            m_bLeftDown = false;
        }
        else
        {
            // if a button other than the left was clicked, just let the super
            // class deal with it - probably viewport movement
            __super::MouseDown(eType, iX, iY);
        }
    }
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if ((eType == MouseButtonType::LeftButton) && (m_bAlreadyRotating))
    {
        // stop transforming object
        m_bAlreadyRotating = false;

        // commit transform to properties
        NiMatrix3 kCurrentRotation;
        NiPoint3 kCurrentPosition;

        if (m_bCloning)
        {
            // if we are cloning, we need to add the new entities to the scene
            CommandService->BeginUndoFrame("Placement: Clone and rotate "
                "selection");
            MScene* pmScene = MFramework::Instance->Scene;
            CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
            while (en.MoveNext())
            {
                EntityArray amEntities = en.Current;
                // we must assign each clone a unique name before adding
                amEntities[0]->Name = 
                    pmScene->GetUniqueEntityName(amEntities[0]->Name);

                // detach the default lights and let the scene handle adding 
                // appropriate ones
                MFramework::Instance->LightManager->
                    RemoveEntitiesFromDefaultLights(amEntities);

                for (int i = 0; i < amEntities->Length; i++)
                {
                    MEntity* pmClone = amEntities[i];

                    // If the entity is a light, we must prepare its affected
                    // entities before adding.
                    if (MLightManager::EntityIsLight(pmClone))
                    {
                        PrepareClonedLight(pmClone);
                    }

                    // Add entity.
                    pmScene->AddEntity(pmClone, true);
                }
                SelectionService->AddEntityToSelection(amEntities[0]);
            }
            CommandService->EndUndoFrame(true);
            m_pmPreviousSelection->Clear();
            m_pmCloneArray->Clear();
            m_bCloning = false;
        }
        else if (NiAbs(m_fCurrentRadians) >= 0.001f)
        {
            CommandService->BeginUndoFrame("Placement: Rotate selected "
                "entities");
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            unsigned int uiIndex = 0;
            for (int i = 0; i < amEntities->Count; i++)
            {
                if (amEntities[i] != NULL &&
                    amEntities[i]->HasProperty(*m_pkTranslationName) &&
                    amEntities[i]->HasProperty(*m_pkRotationName))
                {
                    // in order to undo properly, the previously existing
                    // state must be the one that we want to undo to
                    // therefore we reverse the transformation and set the
                    // original back, then set the new rotation again
                    NiEntityInterface* pkEntity = amEntities[i]->
                        GetNiEntityInterface();
                    if (!amEntities[i]->IsPropertyReadOnly(*m_pkRotationName))
                    {
                        pkEntity->GetPropertyData(*m_pkRotationName, 
                            kCurrentRotation);
                        pkEntity->SetPropertyData(*m_pkRotationName, 
                            m_pkInitialRotation->GetAt(uiIndex));

                        if (!m_pkInitialRotationUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkRotationName);
                        }

                        amEntities[i]->SetPropertyData(*m_pkRotationName, new 
                            MMatrix3(kCurrentRotation), true);
                    }

                    // set the translation as well
                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkTranslationName))
                    {
                        pkEntity->GetPropertyData(*m_pkTranslationName, 
                            kCurrentPosition);
                        pkEntity->SetPropertyData(*m_pkTranslationName, 
                            m_pkInitialTranslation->GetAt(uiIndex));

                        if (!m_pkInitialTranslationUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkTranslationName);
                        }

                        amEntities[i]->SetPropertyData(*m_pkTranslationName, 
                            new MPoint3(kCurrentPosition), true);
                    }

                    uiIndex++;
                }
            }
            CommandService->EndUndoFrame(true);

        }
        else
        {
            // if we did not move enough, put back where we started
            MEntity* amEntities[] = SelectionService
                ->GetSelectedEntities();
            unsigned int uiIndex = 0;
            for (int i = 0; i < amEntities->Count; i++)
            {
                if (amEntities[i] != NULL &&
                    amEntities[i]->HasProperty(*m_pkTranslationName) &&
                    amEntities[i]->HasProperty(*m_pkRotationName))
                {
                    NiEntityInterface* pkEntity = amEntities[i]->
                        GetNiEntityInterface();
                    if (!amEntities[i]->IsPropertyReadOnly(*m_pkRotationName))
                    {
                        pkEntity->SetPropertyData(*m_pkRotationName, 
                            m_pkInitialRotation->GetAt(uiIndex));

                        if (!m_pkInitialRotationUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkRotationName);
                        }
                    }

                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkTranslationName))
                    {
                        pkEntity->SetPropertyData(*m_pkTranslationName, 
                            m_pkInitialTranslation->GetAt(uiIndex));

                        if (!m_pkInitialTranslationUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkTranslationName);
                        }
                    }

                    uiIndex++;
                }
            }
        }
        m_pkInitialRotation->RemoveAll();
        m_pkInitialRotationUnique->RemoveAll();
        m_pkInitialTranslation->RemoveAll();
        m_pkInitialTranslationUnique->RemoveAll();
    }

    // make sure that super class handles its mouseup
    __super::MouseUp(eType, iX, iY);
    // make sure that our gizmo is updated
    MouseMove(iX, iY);
}
//---------------------------------------------------------------------------
void MRotationInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;


    NiPoint3 kOrigin, kDir;
    NiViewMath::MouseToRay((float) iX, (float) iY, 
        MFramework::Instance->ViewportManager->ActiveViewport->Width,
        MFramework::Instance->ViewportManager->ActiveViewport->Height,
        MFramework::Instance->ViewportManager->ActiveViewport->GetNiCamera(), 
        kOrigin, kDir);

    if (m_bAlreadyRotating)
    {
        //transform
        RotateHelper(&kOrigin, &kDir);
    }
    else if (CanTransform())
    {
        // allow camera movement
        __super::MouseMove(iX, iY);

        // highlight the axis we are over
        // before we pick, set the scale
        SetGizmoScale(MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera());
        m_pkPick->SetTarget(m_pkGizmo);
        if (m_pkPick->PickObjects(kOrigin, kDir, false))
        {
            const NiPick::Results& kPickResults = m_pkPick->GetResults();
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                // set current axis
                NiFixedString kName = pkPickedObject->GetName();
                if (kName == *m_pkXAxisName)
                    m_eCurrentAxis = AXIS_X;
                else if (kName == *m_pkYAxisName)
                    m_eCurrentAxis = AXIS_Y;
                else if (kName == *m_pkZAxisName)
                    m_eCurrentAxis = AXIS_Z;

                // set axis colors appropriately
                HighLightAxis(m_eCurrentAxis);
            }
        }
        else
        {
            //  reset selected axis to previous value
            m_eCurrentAxis = m_eAxis;
            HighLightAxis(m_eAxis);
        }

        if (m_bLeftDown)
        {
            if ((m_iMouseX != iX) || (m_iMouseY != iY))
            {
                // store the initial position and rotation of the selection
                MEntity* amEntities[] = 
                    SelectionService->GetSelectedEntities();
                for (int i = 0; i < amEntities->Count; i++)
                {
                    if (amEntities[i] != NULL &&
                        amEntities[i]->HasProperty(*m_pkTranslationName) &&
                        amEntities[i]->HasProperty(*m_pkRotationName))
                    {
                        NiEntityInterface* pkEntity = amEntities[i]->
                            GetNiEntityInterface();
                        NiMatrix3 kCurrentRotation;
                        pkEntity->GetPropertyData(*m_pkRotationName, 
                            kCurrentRotation);
                        m_pkInitialRotation->Add(kCurrentRotation);

                        bool bUnique;
                        NIVERIFY(pkEntity->IsPropertyUnique(*m_pkRotationName,
                            bUnique));
                        m_pkInitialRotationUnique->Add(bUnique);

                        NiPoint3 kCurrentPosition;
                        pkEntity->GetPropertyData(*m_pkTranslationName, 
                            kCurrentPosition);
                        m_pkInitialTranslation->Add(kCurrentPosition);

                        NIVERIFY(pkEntity->IsPropertyUnique(
                            *m_pkTranslationName, bUnique));
                        m_pkInitialTranslationUnique->Add(bUnique);
                    }
                }

                if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
                {
                    MFramework* FW = MFramework::Instance;

                    // if the user is holding shift, we need to clone the 
                    // selection

                    // First, narrow down the selection to only those entities
                    // that can be added to the scene.
                    List<MEntity*>* pmSelectedEntities = new List<MEntity*>(
                        SelectionService->NumSelectedEntities);
                    MEntity* amSelectedEntities[] = SelectionService->
                        GetSelectedEntities();
                    for (int i = 0; i < amSelectedEntities->Count; i++)
                    {
                        bool bDoNotAdd = false;
                        FW->EventManager->RaiseRequestAddCloneToScene(
                            FW->Scene, amSelectedEntities[i], &bDoNotAdd);
                        if (!bDoNotAdd)
                        {
                            pmSelectedEntities->Add(amSelectedEntities[i]);
                        }
                    }

                    // Iterate over surviving entities, cloning each.
                    List<MEntity*>::Enumerator mEnum =
                        pmSelectedEntities->GetEnumerator();
                    while (mEnum.MoveNext())
                    {
                        m_pmPreviousSelection->Add(mEnum.Current);
                        MEntity* amClones[] = mEnum.Current->Clone(
                            mEnum.Current->Name, false);
                        m_pmCloneArray->Add(amClones);
                        for (int i = 0; i < amClones->Length; ++i)
                        {
                            amClones[i]->Update(
                                FW->TimeManager->CurrentTime,
                                FW->ExternalAssetManager);
                        }

                        // Add the default lights to the clones so we can see
                        // them.
                        FW->LightManager->AddEntitiesToDefaultLights(amClones);
                    }
                    SelectionService->SelectionCenter->ToNiPoint3(
                        *m_pkCloneCenter);
                    SelectionService->ClearSelectedEntities();
                    m_bCloning = true;
                }
                else
                {
                    m_pmPreviousSelection->Clear();
                    m_pmCloneArray->Clear();
                    m_bCloning = false;
                }

                m_bAlreadyRotating = true;
                RotateHelper(&kOrigin, &kDir);
            }
        }
    }
    else
    {
        // allow camera movement
        __super::MouseMove(iX, iY);
    }
}
//---------------------------------------------------------------------------
