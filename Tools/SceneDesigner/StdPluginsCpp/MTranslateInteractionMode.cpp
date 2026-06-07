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

#include "MTranslateInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

#define ENSURE_GIZMO_LOADED() if (!m_pkGizmo) \
    { \
        if (LoadGizmo() == false) \
        { \
            NIASSERT(!"MTranslateInteractionMode could not load " \
                "..\\..\\Data\\translate.nif"); \
        } \
    }

//---------------------------------------------------------------------------
MTranslateInteractionMode::MTranslateInteractionMode() : m_pkXAxisName(NULL),
    m_pkYAxisName(NULL), m_pkZAxisName(NULL), m_pkXYPlaneName(NULL), 
    m_pkXZPlaneName(NULL), m_pkYZPlaneName(NULL), m_pkXLineName(NULL), 
    m_pkYLineName(NULL), m_pkZLineName(NULL), m_pkXYLineName(NULL), 
    m_pkXZLineName(NULL), m_pkYXLineName(NULL), m_pkYZLineName(NULL), 
    m_pkZXLineName(NULL), m_pkZYLineName(NULL), 
    m_pkHighLightColor(NULL), m_pkStartPoint(NULL), 
    m_pkInitialTranslation(NULL), m_pkInitialTranslationUnique(NULL),
    m_pkInitialRotation(NULL), m_pkInitialRotationUnique(NULL),
    m_pmPreviousSelection(NULL), m_pmCloneArray(NULL), m_pkCloneCenter(NULL)
{
    m_pkXAxisName = NiNew NiFixedString("XAxis");
    m_pkYAxisName = NiNew NiFixedString("YAxis");
    m_pkZAxisName = NiNew NiFixedString("ZAxis");
    m_pkXYPlaneName = NiNew NiFixedString("XYPlane");
    m_pkXZPlaneName = NiNew NiFixedString("XZPlane");
    m_pkYZPlaneName = NiNew NiFixedString("YZPlane");
    m_pkXLineName = NiNew NiFixedString("XLine");
    m_pkYLineName = NiNew NiFixedString("YLine");
    m_pkZLineName = NiNew NiFixedString("ZLine");
    m_pkXYLineName = NiNew NiFixedString("XYLine");
    m_pkXZLineName = NiNew NiFixedString("XZLine");
    m_pkYXLineName = NiNew NiFixedString("YXLine");
    m_pkYZLineName = NiNew NiFixedString("YZLine");
    m_pkZXLineName = NiNew NiFixedString("ZXLine");
    m_pkZYLineName = NiNew NiFixedString("ZYLine");
    m_pkInitialTranslation = NiNew NiTObjectSet<NiPoint3>(32);
    m_pkInitialTranslationUnique = NiNew NiTPrimitiveSet<bool>(32);
    m_pkInitialRotation = NiNew NiTObjectSet<NiMatrix3>(32);
    m_pkInitialRotationUnique = NiNew NiTPrimitiveSet<bool>(32);
    m_pmPreviousSelection = new ArrayList(1);
    m_pmCloneArray = new CloneArray();
    m_pkCloneCenter = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);

    NiDelete m_pkXAxisName;
    NiDelete m_pkYAxisName;
    NiDelete m_pkZAxisName;
    NiDelete m_pkXYPlaneName;
    NiDelete m_pkXZPlaneName;
    NiDelete m_pkYZPlaneName;
    NiDelete m_pkXLineName;
    NiDelete m_pkYLineName;
    NiDelete m_pkZLineName;
    NiDelete m_pkXYLineName;
    NiDelete m_pkXZLineName;
    NiDelete m_pkYXLineName;
    NiDelete m_pkYZLineName;
    NiDelete m_pkZXLineName;
    NiDelete m_pkZYLineName;
    m_pkInitialTranslation->RemoveAll();
    NiDelete m_pkInitialTranslation;
    m_pkInitialTranslationUnique->RemoveAll();
    NiDelete m_pkInitialTranslationUnique;
    m_pkInitialRotation->RemoveAll();
    NiDelete m_pkInitialRotation;
    m_pkInitialRotationUnique->RemoveAll();
    NiDelete m_pkInitialRotationUnique;

    NiDelete m_pkHighLightColor;
    NiDelete m_pkPick;
    NiDelete m_pkStartPoint;
    NiDelete m_pkStartPick;

    NiDelete m_pkCloneCenter;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MTranslateInteractionMode::OnSettingChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::HIGHLIGHT_COLOR,
        *m_pkHighLightColor, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_SNAP,
        m_fSnapSpacing, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_SNAP_ENABLED, m_bSnapEnabled, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_PRECISION,
        m_fPrecision, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED, m_bPrecisionEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED, m_bSnapToPickEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED, m_bRotateToPickEnabled, 
        pmHandler);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::OnSettingChanged(Object*,
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
        MSettingsHelper::TRANSLATION_SNAP))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fSnapSpacing = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_SNAP_ENABLED))
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
            m_fPrecision = *pfVal;
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
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapToPickEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bRotateToPickEnabled = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
String* MTranslateInteractionMode::get_Name()
{
    return "GamebryoTranslation";
}
//---------------------------------------------------------------------------
bool MTranslateInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_eAxis = TranslateAxis::PLANE_XY;
    m_eCurrentAxis = TranslateAxis::PLANE_XY;
    m_bOnGizmo = false;
    m_bAlreadyTranslating = false;
    m_fDefaultDistance = STANDARD_DISTANCE;
    m_fSnapSpacing = 1.0f;
    m_bSnapEnabled = true;
    m_fPrecision = 0.1f;
    m_bPrecisionEnabled = true;
    m_bSnapToPickEnabled = false;
    m_bRotateToPickEnabled = false;
    m_usAlignFacingAxis = 2;
    m_usAlignUpAxis = 1;
    m_fStartScale = 1.0f;

    m_pkHighLightColor = NiNew NiColor(1.0f, 1.0f, 0.0f);
    m_pkStartPoint = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
    m_pkStartPick = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
    m_pkPick = NiNew NiPick();

    // Loading of NIF file is deferred

    return true;
}
//---------------------------------------------------------------------------
bool MTranslateInteractionMode::LoadGizmo()
{
    String* pmPath = String::Concat(MFramework::Instance->AppStartupPath,
        "..\\..\\Data\\translate.nif");
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
            "Failed to load translation gizmo file 'translation.nif'");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::SetInteractionMode(Object*, 
    EventArgs*)
{
    MVerifyValidInstance;

    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
}
//---------------------------------------------------------------------------
bool MTranslateInteractionMode::CanTransform()
{
    MEntity* amEntities[] = SelectionService->GetSelectedEntities();
    for (int i = 0; i < amEntities->Count; ++i)
    {
        if (!amEntities[i]->Writable)
        {
            return false;
        }
    }
    for (int i = 0; i < amEntities->Count; i++)
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
void MTranslateInteractionMode::TranslateHelper(
    const NiPoint3* pkOrigin, const NiPoint3* pkDir)
{
    MVerifyValidInstance;

    NiPoint3 kTranslation;
    NiMatrix3 kRotation;
    NiPoint3 kAxis;
    bool bPicked;

    if ((m_eAxis == AXIS_X) || (m_eAxis == PLANE_YZ))
        kAxis = NiPoint3::UNIT_X;
    else if ((m_eAxis == AXIS_Y) || (m_eAxis == PLANE_XZ))
        kAxis = NiPoint3::UNIT_Y;
    else if ((m_eAxis == AXIS_Z) || (m_eAxis == PLANE_XY))
        kAxis = NiPoint3::UNIT_Z;

    if ((m_eAxis == AXIS_X) || (m_eAxis == AXIS_Y) || (m_eAxis == AXIS_Z))
    {
        kTranslation = NiViewMath::TranslateOnAxis(*m_pkStartPick, kAxis, 
            *pkOrigin, *pkDir);
    }
    else if ((m_eAxis == PLANE_XY) || (m_eAxis == PLANE_XZ) ||
        (m_eAxis == PLANE_YZ))
    {
        kTranslation = NiViewMath::TranslateOnPlane(*m_pkStartPick, kAxis, 
            *pkOrigin, *pkDir);
    }

    kRotation = NiMatrix3::IDENTITY;
    bPicked = false;

    if (m_bSnapEnabled)
    {
        // if snap is enabled, alter kNewTranslation such that we round 
        // to the nearest m_fSnapSpacing
        kTranslation /= m_fSnapSpacing;
        kTranslation.x = NiFloor(kTranslation.x + 0.5f);
        kTranslation.y = NiFloor(kTranslation.y + 0.5f);
        kTranslation.z = NiFloor(kTranslation.z + 0.5f);
        kTranslation *= m_fSnapSpacing;
    }

    if (m_bSnapToPickEnabled)
    {
        // perform a pick, and use the delta between that and the starting pt
        if (MFramework::Instance->PickUtility->PerformPick(
            MFramework::Instance->Scene, *pkOrigin, *pkDir, true))
        {
            const NiPick* pkPick;
            pkPick = MFramework::Instance->PickUtility->GetNiPick();
            const NiPick::Results& kPickResults = pkPick->GetResults();
            NiPick::Record* pkFinalRecord = NULL;
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            for (unsigned int ui = 0; ui < kPickResults.GetSize(); ui++)
            {
                NiPick::Record* pkPickRecord = kPickResults.GetAt(ui);
                if (pkPickRecord != NULL)
                {
                    NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                    MEntity* pmPickedEntity = MFramework::Instance->
                        PickUtility->GetEntityFromPickedObject(pkPickedObject);
                    bool bIsSelected = false;
                    for (int j = 0; j < amEntities->Count; j++)
                    {
                        if (pmPickedEntity == amEntities[j])
                        {
                            // the picked entity is selected; ignore it
                            bIsSelected = true;
                            break;
                        }
                    }
                    if (!bIsSelected)
                    {
                        pkFinalRecord = pkPickRecord;
                        bPicked = true;
                        break;
                    }
                }
            }
            if (pkFinalRecord)
            {
                kTranslation = pkFinalRecord->GetIntersection() - 
                    *m_pkStartPoint;
                if (m_bRotateToPickEnabled)
                {
                    NiPoint3 kNormal;
                    NiPoint3 kUpAxis;
                    kNormal = pkFinalRecord->GetNormal();
                    kUpAxis = *m_pkUpAxis;
                    // first check if model's up and facing axis are parallel
                    if ((m_usAlignUpAxis - m_usAlignFacingAxis) % 3 == 0)
                    {
                        m_usAlignUpAxis += 1;
                    }
                    if ((kNormal.Dot(kUpAxis) > NiViewMath::PARALLEL_THRESHOLD)
                        || (kNormal.Dot(kUpAxis) < 
                        -NiViewMath::PARALLEL_THRESHOLD))
                    {
                        if ((kNormal.Dot(NiPoint3::UNIT_Z) < 
                            NiViewMath::PARALLEL_THRESHOLD) &&
                            (kNormal.Dot(NiPoint3::UNIT_Z) > 
                            -NiViewMath::PARALLEL_THRESHOLD))
                        {
                            kUpAxis = NiPoint3::UNIT_Z;
                        }
                        else if ((kNormal.Dot(NiPoint3::UNIT_Y) < 
                            NiViewMath::PARALLEL_THRESHOLD) &&
                            (kNormal.Dot(NiPoint3::UNIT_Y) >
                            -NiViewMath::PARALLEL_THRESHOLD))
                        {
                            kUpAxis = NiPoint3::UNIT_Y;
                        }
                        else
                        {
                            kUpAxis = NiPoint3::UNIT_X;
                        }
                    }
                    while (m_usAlignFacingAxis > 5)
                        m_usAlignFacingAxis -= 6;
                    while (m_usAlignUpAxis > 5)
                        m_usAlignUpAxis -= 6;
                    if (m_usAlignFacingAxis > 2)
                    {
                        m_usAlignFacingAxis -= 3;
                        kNormal = -kNormal;
                    }
                    if (m_usAlignUpAxis > 2)
                    {
                        m_usAlignUpAxis -= 3;
                        kUpAxis = -kUpAxis;
                    }

                    // calculate rotation matrix
                    NiPoint3 kFrameX;
                    NiPoint3 kFrameY;
                    NiPoint3 kFrameZ;
                    if (m_usAlignFacingAxis == 0)
                    {
                        if (m_usAlignUpAxis == 1)
                        {
                            // we want X axis to face N and Y to face up
                            kFrameX = kNormal;
                            kFrameZ = kFrameX.Cross(kUpAxis);
                            kFrameY = kFrameZ.Cross(kFrameX);
                        }
                        else
                        {
                            // we want X axis to face N and Z to face up
                            kFrameX = kNormal;
                            kFrameY = kUpAxis.Cross(kFrameX);
                            kFrameZ = kFrameX.Cross(kFrameY);
                        }
                    }
                    else if (m_usAlignFacingAxis == 1)
                    {
                        if (m_usAlignUpAxis == 0)
                        {
                            // we want Y axis to face N and X to face up
                            kFrameY = kNormal;
                            kFrameZ = kUpAxis.Cross(kFrameY);
                            kFrameX = kFrameY.Cross(kFrameZ);
                        }
                        else
                        {
                            // we want Y axis to face N and Z to face up
                            kFrameY = kNormal;
                            kFrameX = kFrameY.Cross(kUpAxis);
                            kFrameZ = kFrameX.Cross(kFrameY);
                        }
                    }
                    else
                    {
                        if (m_usAlignUpAxis == 0)
                        {
                            // we want Z axis to face N and X to face up
                            kFrameZ = kNormal;
                            kFrameY = kFrameZ.Cross(kUpAxis);
                            kFrameX = kFrameY.Cross(kFrameZ);
                        }
                        else
                        {
                            // we want Z axis to face N and Y to face up
                            kFrameZ = kNormal;
                            kFrameX = kUpAxis.Cross(kFrameZ);
                            kFrameY = kFrameZ.Cross(kFrameX);
                        }
                    }
                    kRotation.SetCol(0, kFrameX);
                    kRotation.SetCol(1, kFrameY);
                    kRotation.SetCol(2, kFrameZ);
                    kRotation.Reorthogonalize();
                }
            }
        }
    }

    if (m_bCloning)
    {
        // if cloning, transform clones
        NiPoint3 kNewPosition;
        bool bMultipleSelected;
        bMultipleSelected = (m_pmCloneArray->Count > 1);
        unsigned int uiIndex = 0;
        for (int i = 0; i < m_pmCloneArray->Count; i++)
        {
            EntityArray pmEntities = m_pmCloneArray->Item[i];
            if (pmEntities->Length > 0)
            {
                MEntity* pmEntity = pmEntities[0];
                if (pmEntity == NULL ||
                    !pmEntity->HasProperty(*m_pkTranslationName) ||
                    !pmEntity->HasProperty(*m_pkRotationName))
                {
                    continue;
                }

                NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();
                kNewPosition = m_pkInitialTranslation->GetAt(uiIndex) + 
                    kTranslation;
                if ((m_bSnapToPickEnabled) && (m_bRotateToPickEnabled))
                {
                    if (bPicked)
                    {
                        // do additional transformation
                        if (bMultipleSelected)
                        {
                            NiPoint3 kDelta;
                            kDelta = m_pkInitialTranslation->GetAt(uiIndex) - 
                                *m_pkStartPoint;
                            kDelta = kRotation * kDelta;
                            kNewPosition = *m_pkStartPoint + kTranslation +
                                kDelta;

                            NiMatrix3 kNewRotation;
                            kNewRotation = kRotation * 
                                m_pkInitialRotation->GetAt(uiIndex);
                            pkEntity->SetPropertyData(*m_pkRotationName, 
                                kNewRotation);
                        }
                        else
                        {
                            pkEntity->SetPropertyData(
                                *m_pkRotationName, kRotation);
                        }
                    }
                    else
                    {
                        pkEntity->SetPropertyData(*m_pkRotationName, 
                            m_pkInitialRotation->GetAt(uiIndex));
                    }
                }
                if (m_bPrecisionEnabled)
                {
                    kNewPosition /= m_fPrecision;
                    kNewPosition.x = NiFloor(kNewPosition.x + 0.5f);
                    kNewPosition.y = NiFloor(kNewPosition.y + 0.5f);
                    kNewPosition.z = NiFloor(kNewPosition.z + 0.5f);
                    kNewPosition *= m_fPrecision;
                }
                pkEntity->SetPropertyData(*m_pkTranslationName, kNewPosition);
                uiIndex++;
            }
        }
        *m_pkCloneCenter = *m_pkStartPoint + kTranslation;
    }
    else
    {
        // if not cloning, transform selection
        MEntity* amEntities[] = SelectionService->GetSelectedEntities();
        bool bMultipleSelected;
        bMultipleSelected = (amEntities->Count > 1);
        unsigned int uiIndex = 0;
        for (int i = 0; i < amEntities->Count; i++)
        {
            if (amEntities[i] != NULL &&
                amEntities[i]->HasProperty(*m_pkTranslationName) &&
                amEntities[i]->HasProperty(*m_pkRotationName))
            {
                NiEntityInterface* pkEntity = amEntities[i]->
                    GetNiEntityInterface();
                NiPoint3 kNewPosition;
                kNewPosition = m_pkInitialTranslation->GetAt(uiIndex) + 
                    kTranslation;
                if ((m_bSnapToPickEnabled) && (m_bRotateToPickEnabled))
                {
                    if (bPicked)
                    {
                        // do additional transformation
                        if (bMultipleSelected)
                        {
                            NiPoint3 kDelta;
                            kDelta = m_pkInitialTranslation->GetAt(uiIndex) - 
                                *m_pkStartPoint;
                            kDelta = kRotation * kDelta;
                            kNewPosition = *m_pkStartPoint + kTranslation 
                                + kDelta;

                            NiMatrix3 kNewRotation;
                            kNewRotation = kRotation * 
                                m_pkInitialRotation->GetAt(uiIndex);
                            pkEntity->SetPropertyData(*m_pkRotationName, 
                                kNewRotation);
                        }
                        else
                        {
                            pkEntity->SetPropertyData(*m_pkRotationName, 
                                kRotation);
                        }
                    }
                    else
                    {
                        pkEntity->SetPropertyData(*m_pkRotationName,
                            m_pkInitialRotation->GetAt(uiIndex));
                    }
                }
                if (m_bPrecisionEnabled)
                {
                    kNewPosition /= m_fPrecision;
                    kNewPosition.x = NiFloor(kNewPosition.x + 0.5f);
                    kNewPosition.y = NiFloor(kNewPosition.y + 0.5f);
                    kNewPosition.z = NiFloor(kNewPosition.z + 0.5f);
                    kNewPosition *= m_fPrecision;
                }
                pkEntity->SetPropertyData(*m_pkTranslationName, kNewPosition);
                uiIndex++;
            }
        }
        NiPoint3 kNewPosition = *m_pkStartPoint + kTranslation;
        SelectionService->SelectionCenter->X = kNewPosition.x;
        SelectionService->SelectionCenter->Y = kNewPosition.y;
        SelectionService->SelectionCenter->Z = kNewPosition.z;
    }
}
//---------------------------------------------------------------------------
MTranslateInteractionMode::TranslateAxis 
    MTranslateInteractionMode::GetBestAxis(const TranslateAxis eAxis)
{
    TranslateAxis eReturnValue;
    NiPoint3 kAxisDirection;
    bool bAxis = false;

    eReturnValue = eAxis;
    if (eAxis == AXIS_X)
    {
        kAxisDirection = NiPoint3::UNIT_X;
        bAxis = true;
    }
    else if (eAxis == AXIS_Y)
    {
        kAxisDirection = NiPoint3::UNIT_Y;
        bAxis = true;
    }
    else if (eAxis == AXIS_Z)
    {
        kAxisDirection = NiPoint3::UNIT_Z;
        bAxis = true;
    }
    else if (eAxis == PLANE_XY)
    {
        kAxisDirection = NiPoint3::UNIT_Z;
        bAxis = false;
    }
    else if (eAxis == PLANE_XZ)
    {
        kAxisDirection = NiPoint3::UNIT_Y;
        bAxis = false;
    }
    else if (eAxis == PLANE_YZ)
    {
        kAxisDirection = NiPoint3::UNIT_X;
        bAxis = false;
    }

    NiCamera* pkCam;
    NiPoint3 kLook;
    float kCosine;

    pkCam = MFramework::Instance->ViewportManager->ActiveViewport->
        GetNiCamera();
    pkCam->GetRotate().GetCol(0, (float*)&kLook);
    kCosine = kLook.Dot(kAxisDirection);

    // for axis, check if the axis is parallel to the view
    // for plane, check if the normal is perpendicular to view
    if ((bAxis) && ((kCosine >= NiViewMath::PARALLEL_THRESHOLD) || 
        (kCosine <= -NiViewMath::PARALLEL_THRESHOLD)))
    {
        // if axis is parallel to view, make us translate along
        // the perpendicular plane
        if (eReturnValue == AXIS_X)
        {
            eReturnValue = PLANE_YZ;
        }
        else if (eReturnValue == AXIS_Y)
        {
            eReturnValue = PLANE_XZ;
        }
        else if (eReturnValue == AXIS_Z)
        {
            eReturnValue = PLANE_XY;
        }
    }
    else if ((kCosine <= NiViewMath::INV_PARALLEL_THRESHOLD) && 
        (kCosine >= -NiViewMath::INV_PARALLEL_THRESHOLD))
    {
        // if plane's normal is perpendicular to view, we are looking down
        // the plane - use the plane's normal as a translation axis instead
        if (eReturnValue == PLANE_YZ)
        {
            eReturnValue = AXIS_X;
        }
        else if (eReturnValue == PLANE_XZ)
        {
            eReturnValue = AXIS_Y;
        }
        else if (eReturnValue == PLANE_XY)
        {
            eReturnValue = AXIS_Z;
        }
    }

    return eReturnValue;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::HighLightAxis(const TranslateAxis eAxis)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    // first, reset everything to it's default
    NiAVObject* pkLineX;
    NiAVObject* pkLineY;
    NiAVObject* pkLineZ;
    NiAVObject* pkLineXY;
    NiAVObject* pkLineXZ;
    NiAVObject* pkLineYX;
    NiAVObject* pkLineYZ;
    NiAVObject* pkLineZX;
    NiAVObject* pkLineZY;

    pkLineX = m_pkGizmo->GetObjectByName(*m_pkXLineName);
    pkLineY = m_pkGizmo->GetObjectByName(*m_pkYLineName);
    pkLineZ = m_pkGizmo->GetObjectByName(*m_pkZLineName);
    pkLineXY = m_pkGizmo->GetObjectByName(*m_pkXYLineName);
    pkLineXZ = m_pkGizmo->GetObjectByName(*m_pkXZLineName);
    pkLineYX = m_pkGizmo->GetObjectByName(*m_pkYXLineName);
    pkLineYZ = m_pkGizmo->GetObjectByName(*m_pkYZLineName);
    pkLineZX = m_pkGizmo->GetObjectByName(*m_pkZXLineName);
    pkLineZY = m_pkGizmo->GetObjectByName(*m_pkZYLineName);
    NIASSERT(pkLineX);
    NIASSERT(pkLineY);
    NIASSERT(pkLineZ);
    NIASSERT(pkLineXY);
    NIASSERT(pkLineXZ);
    NIASSERT(pkLineYX);
    NIASSERT(pkLineYZ);
    NIASSERT(pkLineZX);
    NIASSERT(pkLineZY);

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

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineXY->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineXZ->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineYX->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineYZ->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineZX->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineZY->GetProperty(NiProperty::MATERIAL));
    NIASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    if (eAxis == AXIS_X)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == AXIS_Y)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == AXIS_Z)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == PLANE_XY)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineXY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineYX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == PLANE_XZ)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineXZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == PLANE_YZ)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineYZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }

    m_pkGizmo->UpdateProperties();
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::SetGizmoScale(NiCamera* pkCamera)
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
    if (m_bAlreadyTranslating)
    {
        m_pkGizmo->SetScale(m_fStartScale);
    }
    else
    {
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
    }
    m_pkGizmo->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkGizmo);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::Update(float fTime)
{
    MVerifyValidInstance;

    __super::Update(fTime);

    if (m_bCloning)
    {
        CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
        while (en.MoveNext())
        {
            EntityArray pmEntities = en.Current;
            for (int i = 0; i != pmEntities->Length; ++i)
            {
                MEntity* pmEntity = pmEntities[i];
                pmEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
                    MFramework::Instance->ExternalAssetManager);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::RenderGizmo(
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
        NiCamera* pkCam = pkContext->m_pkCamera;

        if (m_bCloning)
        {
            // if we are cloning, draw a ghost of the new objects
            GhostRenderingMode->Begin(pmRenderingContext);
            CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
            while (en.MoveNext())
            {
                EntityArray amEntities = en.Current;
                for (int i = 0; i != amEntities->Length; ++i)
                {
                    MEntity* pmEntity = amEntities[i];
                    GhostRenderingMode->Render(pmEntity, pmRenderingContext);
                }
            }
            GhostRenderingMode->End(pmRenderingContext);
        }

        //clear the z-buffer
        pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

        SetGizmoScale(pkCam);
        NiDrawScene(pkCam, m_pkGizmo, *pkContext->m_pkCullingProcess);
    }
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::MouseDown(MouseButtonType eType, int iX,
    int iY)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    if (eType == MouseButtonType::LeftButton)
    {
        // we use these to see if the mouse has moved far enough to count
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
            // pass the call off to selection mode, it handles if we 
            // are or aren't clicking on an object
            __super::MouseDown(eType, iX, iY);

            m_eAxis = GetBestAxis(m_eAxis);
        }
        else
        {
            // set the working axis to the one we are hovering over
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
                else if (kName == *m_pkXYPlaneName)
                    m_eCurrentAxis = PLANE_XY;
                else if (kName == *m_pkXZPlaneName)
                    m_eCurrentAxis = PLANE_XZ;
                else if (kName == *m_pkYZPlaneName)
                    m_eCurrentAxis = PLANE_YZ;
                else
                    m_eCurrentAxis = PLANE_XY;
                m_eCurrentAxis = GetBestAxis(m_eCurrentAxis);

                // set axis colors appropriately
                HighLightAxis(m_eCurrentAxis);

                m_bOnGizmo = true;
            }
            m_eAxis = m_eCurrentAxis;
            m_bLeftDown = true;
        }

        if (CanTransform())
        {
            // record the starting position of the gizmo
            SelectionService->SelectionCenter->ToNiPoint3(*m_pkStartPoint);
            // store where the pick landed in world space
            if (m_bOnGizmo)
            {
                // if our mouse is on the gizmo, we must use different 
                // pick results
                const NiPick::Results& kPickResults = 
                    m_pkPick->GetResults();
                NiPick::Record* pkPickRecord = NULL;
                pkPickRecord = kPickResults.GetAt(0);
                if (pkPickRecord)
                {
                    *m_pkStartPick = pkPickRecord->GetIntersection();
                }
                else
                {
                    // we have no choice but to pick some arbitrary point
                    *m_pkStartPick = *m_pkStartPoint;
                }
            }
            else
            {
                // if our mouse is on the entity, use the most recent pick 
                // utility results
                *m_pkStartPick = m_pkPickRecord->GetIntersection();
            }
        }
    }
    else
    {
        if ((m_bAlreadyTranslating) && (eType == MouseButtonType::RightButton))
        {
            // if the user right-click canceled, translate objects back
            // if the operation was a clone, restore the original selection
            if (m_bCloning)
            {
                SelectionService->AddEntitiesToSelection(static_cast<
                    MEntity*[]>(m_pmPreviousSelection->ToArray(__typeof(
                    MEntity))));
                // detach the clones from scene lights
                CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
                while (en.MoveNext())
                {
                    for (int i = 0; i != en.Current->Length; ++i)
                    {
                        MEntity* pmEntity = en.Current[i];
                        MFramework::Instance->EntityFactory->Remove(
                            pmEntity->GetNiEntityInterface());
                    }
                }
                m_pmPreviousSelection->Clear();
                m_pmCloneArray->Clear();
                MFramework::Instance->Scene->UpdateEffects();
                m_bCloning = false;
            }
            else
            {
                MEntity* amEntities[] = 
                    SelectionService->GetSelectedEntities();
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
                            *m_pkTranslationName))
                        {
                            pkEntity->SetPropertyData(*m_pkTranslationName, 
                                m_pkInitialTranslation->GetAt(uiIndex));

                            if (!m_pkInitialTranslationUnique->GetAt(uiIndex))
                            {
                                pkEntity->ResetProperty(*m_pkTranslationName);
                            }
                        }
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
                        uiIndex++;
                    }
                }
                SelectionService->SelectionCenter->X = m_pkStartPoint->x;
                SelectionService->SelectionCenter->Y = m_pkStartPoint->y;
                SelectionService->SelectionCenter->Z = m_pkStartPoint->z;
            }

            m_pkInitialTranslation->RemoveAll();
            m_pkInitialTranslationUnique->RemoveAll();
            m_pkInitialRotation->RemoveAll();
            m_pkInitialRotationUnique->RemoveAll();
            m_bAlreadyTranslating = false;
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
void MTranslateInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if ((eType == MouseButtonType::LeftButton) && (m_bAlreadyTranslating))
    {
        // stop transforming object
        m_bAlreadyTranslating = false;

        // commit transform to properties
        NiPoint3 kCenter;
        NiPoint3 kDeltaPosition;
        NiPoint3 kCurrentPosition;
        NiMatrix3 kCurrentRotation;

        if (m_bCloning)
        {
            kCenter = *m_pkCloneCenter;
        }
        else
        {
            SelectionService->SelectionCenter->ToNiPoint3(kCenter);
        }
        kDeltaPosition = *m_pkStartPoint - kCenter;

        // if we were cloning, add them to the scene
        if (m_bCloning)
        {
            CommandService->BeginUndoFrame("Placement: Clone and translate "
                "selection");
            MScene* pmScene = MFramework::Instance->Scene;

            CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
            while (en.MoveNext())
            {
                // we must assign each clone a unique name before adding
                en.Current[0]->Name = 
                    pmScene->GetUniqueEntityName(en.Current[0]->Name);

                // detach the default lights and let the scene handle adding 
                // appropriate ones
                MFramework::Instance->LightManager->
                    RemoveEntitiesFromDefaultLights(en.Current);
                for (int i = 0; i < en.Current->Length; i++)
                {
                    MEntity* pmClone = en.Current[i];

                    // If the entity is a light, we must prepare its affected
                    // entities before adding.
                    if (MLightManager::EntityIsLight(pmClone))
                    {
                        PrepareClonedLight(pmClone);
                    }

                    // Add entity.
                    pmScene->AddEntity(pmClone, true);
                }
                SelectionService->AddEntityToSelection(en.Current[0]);
            }
            CommandService->EndUndoFrame(true);
            m_pmPreviousSelection->Clear();
            m_pmCloneArray->Clear();
            m_bCloning = false;
        }
        else if ((kDeltaPosition.x > 0.01f) || (kDeltaPosition.x < -0.01f) ||
            (kDeltaPosition.y > 0.01f) || (kDeltaPosition.y < -0.01f) ||
            (kDeltaPosition.z > 0.01f) || (kDeltaPosition.z < -0.01f))
        {
            CommandService->BeginUndoFrame("Placement: Translate selected "
                "entities");
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            unsigned int uiIndex = 0;
            for (int i = 0; i < amEntities->Count; i++)
            {
                if (amEntities[i] != NULL &&
                    amEntities[i]->HasProperty(*m_pkTranslationName) &&
                    amEntities[i]->HasProperty(*m_pkRotationName))
                {
                    // in order for the undo operation to store the right info,
                    // we need to change from the original strait to the final
                    // in a single MEntity->SetPropertyData call.  Therefore,
                    // we set the original translation back on the interface
                    // right before setting the final.
                    NiEntityInterface* pkEntity = amEntities[i]->
                        GetNiEntityInterface();
                    pkEntity->GetPropertyData(*m_pkTranslationName, 
                        kCurrentPosition);
                    pkEntity->GetPropertyData(*m_pkRotationName,
                        kCurrentRotation);

                    pkEntity->SetPropertyData(*m_pkTranslationName,
                        m_pkInitialTranslation->GetAt(uiIndex));
                    pkEntity->SetPropertyData(*m_pkRotationName,
                        m_pkInitialRotation->GetAt(uiIndex));

                    if (!m_pkInitialTranslationUnique->GetAt(uiIndex))
                    {
                        pkEntity->ResetProperty(*m_pkTranslationName);
                    }
                    if (!m_pkInitialRotationUnique->GetAt(uiIndex))
                    {
                        pkEntity->ResetProperty(*m_pkRotationName);
                    }

                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkTranslationName))
                    {
                        amEntities[i]->SetPropertyData(*m_pkTranslationName,
                            new MPoint3(kCurrentPosition), true);
                    }
                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkRotationName))
                    {
                        amEntities[i]->SetPropertyData(*m_pkRotationName, new
                            MMatrix3(kCurrentRotation), true);
                    }
                    uiIndex++;
                }
            }
            CommandService->EndUndoFrame(true);
        }
        else
        {
            // if we did not move far enough, reset transformation
            MEntity* amEntities[] = 
                SelectionService->GetSelectedEntities();
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
                        *m_pkTranslationName))
                    {
                        pkEntity->SetPropertyData(*m_pkTranslationName, 
                            m_pkInitialTranslation->GetAt(uiIndex));

                        if (!m_pkInitialTranslationUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkTranslationName);
                        }
                    }
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
                    uiIndex++;
                }
            }
            SelectionService->SelectionCenter->X = m_pkStartPoint->x;
            SelectionService->SelectionCenter->Y = m_pkStartPoint->y;
            SelectionService->SelectionCenter->Z = m_pkStartPoint->z;
        }

        m_pkInitialTranslation->RemoveAll();
        m_pkInitialTranslationUnique->RemoveAll();
        m_pkInitialRotation->RemoveAll();
        m_pkInitialRotationUnique->RemoveAll();
    }

    __super::MouseUp(eType, iX, iY);
        // make sure that our gizmo is updated
    MouseMove(iX, iY);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    NiPoint3 kOrigin, kDir;

    if (m_bAlreadyTranslating)
    {
        //transform
        NiViewMath::MouseToRay((float)iX, (float)iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);
        TranslateHelper(&kOrigin, &kDir);
    }
    else if (CanTransform())
    {
        // allow camera movement
        __super::MouseMove(iX, iY);

        NiViewMath::MouseToRay((float) iX, (float) iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);
        // highlight the axis we are over
        // set gizmo scale
        SetGizmoScale(MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera());
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
                else if (kName == *m_pkXYPlaneName)
                    m_eCurrentAxis = PLANE_XY;
                else if (kName == *m_pkXZPlaneName)
                    m_eCurrentAxis = PLANE_XZ;
                else if (kName == *m_pkYZPlaneName)
                    m_eCurrentAxis = PLANE_YZ;
                else
                    m_eCurrentAxis = PLANE_XY;
                m_eCurrentAxis = GetBestAxis(m_eCurrentAxis);

                // set axis colors appropriately
                HighLightAxis(m_eCurrentAxis);

                m_bOnGizmo = true;
            }
        }
        else
        {
            //  reset selected axis to previous value
            m_bOnGizmo = false;
            m_eCurrentAxis = m_eAxis;
            HighLightAxis(m_eAxis);
        }

        if (m_bLeftDown)
        {
            // if the left is down and we are not already translating, the
            // mouse hasn't moved yet
            if ((m_iMouseX != iX) || (m_iMouseY != iY))
            {
                // if the user is just click + dragging, move the selection
                MEntity* amEntities[] = SelectionService->
                    GetSelectedEntities();
                for (int i = 0; i < amEntities->Count; i++)
                {
                    if (amEntities[i] != NULL &&
                        amEntities[i]->HasProperty(*m_pkTranslationName) &&
                        amEntities[i]->HasProperty(*m_pkRotationName))
                    {
                        NiEntityInterface* pkEntity = amEntities[i]->
                            GetNiEntityInterface();

                        NiPoint3 kCurrentPosition;
                        pkEntity->GetPropertyData(*m_pkTranslationName, 
                            kCurrentPosition);
                        m_pkInitialTranslation->Add(kCurrentPosition);

                        bool bUnique;
                        NIVERIFY(pkEntity->IsPropertyUnique(
                            *m_pkTranslationName, bUnique));
                        m_pkInitialTranslationUnique->Add(bUnique);

                        NiMatrix3 kCurrentRotation;
                        pkEntity->GetPropertyData(*m_pkRotationName,
                            kCurrentRotation);
                        m_pkInitialRotation->Add(kCurrentRotation);

                        NIVERIFY(pkEntity->IsPropertyUnique(
                            *m_pkRotationName, bUnique));
                        m_pkInitialRotationUnique->Add(bUnique);
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
                        for (int i = 0; i != amClones->Length; ++i)
                        {
                            amClones[i]->Update(FW->TimeManager->CurrentTime,
                                FW->ExternalAssetManager);
                        }

                        // Add the default lights to the clones so we can see
                        // them.
                        FW->LightManager->AddEntitiesToDefaultLights(amClones);
                    }
                    SelectionService->ClearSelectedEntities();
                    m_bCloning = true;
                }
                else
                {
                    m_pmPreviousSelection->Clear();
                    m_pmCloneArray->Clear();
                    m_bCloning = false;
                }

                m_bAlreadyTranslating = true;
                if (m_pkGizmo)
                    m_fStartScale = m_pkGizmo->GetScale();
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
