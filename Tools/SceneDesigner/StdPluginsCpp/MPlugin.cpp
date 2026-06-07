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

#include "MPlugin.h"
#include "MShadowHelper.h"

// Services.
#include "MCommandService.h"
#include "MComponentService.h"
#include "MInteractionModeService.h"
#include "MMessageService.h"
#include "MPropertyTypeService.h"
#include "MRenderingModeService.h"
#include "MSelectionSetService.h"
#include "MSelectionService.h"

// Primitive type converters and editors.
#include "MPoint2Converter.h"
#include "MPoint3Converter.h"
#include "MEulerQuaternionConverter.h"
#include "MEulerMatrix3Converter.h"

// Semantic type converters and editors.
#include "MDepthBiasEditor.h"
#include "MFilenameEditor.h"
#include "MNifFilenameEditor.h"
#include "MKfmFilenameEditor.h"
#include "MDepthBiasConverter.h"
#include "MLightTypeConverter.h"
#include "MSequenceIDConverter.h"
#include "MShadowMapSizeHintConverter.h"
#include "MShadowTechniqueConverter.h"

// Interaction modes.
#include "MTranslateInteractionMode.h"
#include "MRotationInteractionMode.h"
#include "MScaleInteractionMode.h"
#include "MSelectionInteractionMode.h"
#include "MCreateInteractionMode.h"
#include "MPanInteractionMode.h"
#include "MZoomInteractionMode.h"
#include "MLookInteractionMode.h"
#include "MOrbitInteractionMode.h"

// Rendering modes.
#include "MGhostRenderingMode.h"
#include "MStandardRenderingMode.h"
#include "MWireframeRenderingMode.h"
#include "MFlatWireframeRenderingMode.h"
#include "MNoShadowsRenderingMode.h"
#include "MNoTexturesRenderingMode.h"
#include "MNoBackfaceCullingRenderingMode.h"

// Components
#include "NiGridComponent.h"

//The following is related to a bug in VC7.1/.Net CLR 1.1
// see http://support.microsoft.com/?id=814472
#if _MSC_VER == 1310
#pragma unmanaged
#include <windows.h>
#include <_vcclrit.h>
#pragma managed
#endif


using namespace System::Reflection;
using namespace System::Drawing;
using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

//---------------------------------------------------------------------------
MPlugin::MPlugin()
{
}
//---------------------------------------------------------------------------
void MPlugin::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        NiGridComponent::_SDMShutdown();
        if (m_pmGridManager != NULL)
            m_pmGridManager->Dispose();
        if (m_pmToolbarListener != NULL)
            m_pmToolbarListener->Dispose();
        MSettingsHelper::Shutdown();
    }
}
//---------------------------------------------------------------------------
String* MPlugin::get_Name()
{
    return this->GetType()->ToString();
}
//---------------------------------------------------------------------------
Version* MPlugin::get_Version()
{
    return Assembly::GetExecutingAssembly()->GetName()->Version;
}
//---------------------------------------------------------------------------
Version* MPlugin::get_ExpectedVersion()
{
    return new System::Version(1, 0);
}
//---------------------------------------------------------------------------
void MPlugin::Load(int, int)
{
    NiGridComponent::_SDMInit();
    MSettingsHelper::Init();
}
//---------------------------------------------------------------------------
IService* MPlugin::GetProvidedServices()[]
{
    // If you are receiving a compile time error at this location related
    // to the GetMessage() method please ensure that the Microsoft Platform
    // SDK is installed and that the Platform SDK's include paths are at 
    // the top of the include list.
    
    ArrayList* pmServices = new ArrayList();

    pmServices->Add(new MCommandService());
    pmServices->Add(new MComponentService());
    pmServices->Add(new MInteractionModeService());
    pmServices->Add(new MMessageService());
    pmServices->Add(new MPropertyTypeService());
    pmServices->Add(new MRenderingModeService());
    m_pmSelectionService = new MSelectionService();
    pmServices->Add(m_pmSelectionService);
    pmServices->Add(new MSelectionSetService());

    return dynamic_cast<IService*[]>(pmServices->ToArray(__typeof(IService)));
}
//---------------------------------------------------------------------------
void MPlugin::Start()
{
    AddPropertyTypes();
    RegisterComponents();
    RegisterSettings();
    AddInteractionModes();
    AddRenderingModes();
    AddGrid();
    AddToolbarListener();
}
//---------------------------------------------------------------------------
void MPlugin::AddPropertyTypes()
{
    // Get property type service.
    IPropertyTypeService* pmPropertyTypeService = MGetService(
        IPropertyTypeService);
    MAssert(pmPropertyTypeService != NULL, "Service not found!");

    // Register primitive types.
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_FLOAT,
        NiEntityPropertyInterface::PT_FLOAT, __typeof(float)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_BOOL,
        NiEntityPropertyInterface::PT_BOOL, __typeof(bool)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_INT,
        NiEntityPropertyInterface::PT_INT, __typeof(int)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_UINT,
        NiEntityPropertyInterface::PT_UINT, __typeof(unsigned int)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_SHORT,
        NiEntityPropertyInterface::PT_SHORT, __typeof(short)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_USHORT,
        NiEntityPropertyInterface::PT_USHORT, __typeof(unsigned short)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_STRING,
        NiEntityPropertyInterface::PT_STRING, __typeof(String)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_POINT2,
        NiEntityPropertyInterface::PT_POINT2, __typeof(MPoint2), NULL,
        __typeof(MPoint2Converter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_POINT3,
        NiEntityPropertyInterface::PT_POINT3, __typeof(MPoint3), NULL,
        __typeof(MPoint3Converter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_QUATERNION,
        NiEntityPropertyInterface::PT_QUATERNION, __typeof(MQuaternion), NULL,
        __typeof(MEulerQuaternionConverter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_MATRIX3,
        NiEntityPropertyInterface::PT_MATRIX3, __typeof(MMatrix3), NULL,
        __typeof(MEulerMatrix3Converter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_COLOR,
        NiEntityPropertyInterface::PT_COLOR, __typeof(Color)));

    // Register standard semantic types.
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Filename", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MFilenameEditor), NULL));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "NIF Filename", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MNifFilenameEditor), NULL));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "KFM Filename", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MKfmFilenameEditor), NULL));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Light Type", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), NULL, __typeof(MLightTypeConverter)), false);
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Sequence ID", NiEntityPropertyInterface::PT_UINT,
        __typeof(unsigned int), NULL, __typeof(MSequenceIDConverter)), false);
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiShadowGeneratorComponent::SEMANTIC_SIZE_HINT,
        NiEntityPropertyInterface::PT_USHORT, __typeof(unsigned short),
        NULL, __typeof(MShadowMapSizeHintConverter)), false);
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiShadowGeneratorComponent::SEMANTIC_SHADOW_TECHNIQUE,
        NiEntityPropertyInterface::PT_STRING, __typeof(String),
        NULL, __typeof(MShadowTechniqueConverter)), false);
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiShadowGeneratorComponent::SEMANTIC_DEPTH_BIAS,
        NiEntityPropertyInterface::PT_FLOAT, __typeof(float),
        __typeof(MDepthBiasEditor), __typeof(MDepthBiasConverter)));
}
//---------------------------------------------------------------------------
void MPlugin::RegisterComponents()
{
    // Get component service.
    IComponentService* pmComponentService = MGetService(IComponentService);
    MAssert(pmComponentService != NULL, "Component service not found!");

    // Get component factory.
    MComponentFactory* pmFactory = MFramework::Instance->ComponentFactory;
    MAssert(pmFactory != NULL, "Component factory not found!");

    // Register standard component types.
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiTransformationComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiActorComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiCameraComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiLightComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiSceneGraphComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiShadowGeneratorComponent()));

    // Register components that should not be visible to users.
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiPrefabComponent()), false);
}
//---------------------------------------------------------------------------
void MPlugin::RegisterSettings()
{
    m_pmSelectionService->RegisterSettings();
    MSettingsHelper::RegisterStandardSettings();
    MShadowHelper::RegisterSettings();
}
//---------------------------------------------------------------------------
void MPlugin::AddInteractionModes()
{
    // Add interaction modes.
    IInteractionModeService* pmInteractionModeService = MGetService(
        IInteractionModeService);
    MAssert(pmInteractionModeService != NULL, "Service not found!");
    MTranslateInteractionMode* pmTranslate = new MTranslateInteractionMode();
    MRotationInteractionMode* pmRotate = new MRotationInteractionMode();
    MScaleInteractionMode* pmScale = new MScaleInteractionMode();
    MSelectionInteractionMode* pmSelect = new MSelectionInteractionMode();
    MCreateInteractionMode* pmCreate = new MCreateInteractionMode();
    MPanInteractionMode* pmPan = new MPanInteractionMode();
    MZoomInteractionMode* pmZoom = new MZoomInteractionMode();
    MLookInteractionMode* pmLook = new MLookInteractionMode();
    MOrbitInteractionMode* pmOrbit = new MOrbitInteractionMode();
    pmInteractionModeService->AddInteractionMode(pmSelect);
    pmInteractionModeService->AddInteractionMode(pmTranslate);
    pmInteractionModeService->AddInteractionMode(pmRotate);
    pmInteractionModeService->AddInteractionMode(pmScale);
    pmInteractionModeService->AddInteractionMode(pmCreate);
    pmInteractionModeService->AddInteractionMode(pmPan);
    pmInteractionModeService->AddInteractionMode(pmZoom);
    pmInteractionModeService->AddInteractionMode(pmLook);
    pmInteractionModeService->AddInteractionMode(pmOrbit);

    // Register each interaction mode's settings
    pmTranslate->RegisterSettings();
    pmRotate->RegisterSettings();
    pmScale->RegisterSettings();
    pmSelect->RegisterSettings();
    pmCreate->RegisterSettings();
    pmPan->RegisterSettings();
    pmZoom->RegisterSettings();
    pmLook->RegisterSettings();
    pmOrbit->RegisterSettings();

    // Bind interaction callbacks to commands
    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    MAssert(pmUICommandService != NULL, "Service not found!");
    pmUICommandService->BindCommands(pmTranslate);
    pmUICommandService->BindCommands(pmRotate);
    pmUICommandService->BindCommands(pmScale);
    pmUICommandService->BindCommands(pmSelect);
    pmUICommandService->BindCommands(pmCreate);
    pmUICommandService->BindCommands(pmPan);
    pmUICommandService->BindCommands(pmZoom);
    pmUICommandService->BindCommands(pmLook);
    pmUICommandService->BindCommands(pmOrbit);
}
//---------------------------------------------------------------------------
void MPlugin::AddRenderingModes()
{
    // Add rendering modes.
    IRenderingModeService* pmRenderingModeService = MGetService(
        IRenderingModeService);
    MAssert(pmRenderingModeService != NULL, "Service not found!");

    MFlatWireframeRenderingMode* pkFlatWireMode = 
        new MFlatWireframeRenderingMode();
    pmRenderingModeService->AddRenderingMode(new MStandardRenderingMode());
    pmRenderingModeService->AddRenderingMode(new MWireframeRenderingMode());
    pmRenderingModeService->AddRenderingMode(pkFlatWireMode);
    pmRenderingModeService->AddRenderingMode(new MGhostRenderingMode());
    pmRenderingModeService->AddRenderingMode(
        new MNoBackfaceCullingRenderingMode());
    pmRenderingModeService->AddRenderingMode(new MNoShadowsRenderingMode());

    // Here we are adding two rendering modes using the same redering mode
    // class.  The first instance is the "No Textures" rendering mode;
    // the second is "No Textures + Edged Faces" rendering mode.
    pmRenderingModeService->AddRenderingMode(new MNoTexturesRenderingMode(
        false));
    pmRenderingModeService->AddRenderingMode(new MNoTexturesRenderingMode(
        true));

    pkFlatWireMode->RegisterSettings();
}
//---------------------------------------------------------------------------
void MPlugin::AddGrid()
{
    m_pmGridManager = new MGridManager();
    m_pmGridManager->CreateGridEntity();
    m_pmGridManager->RegisterSettings();
}
//---------------------------------------------------------------------------
void MPlugin::AddToolbarListener()
{
    m_pmToolbarListener = new MToolbarListener();
    m_pmToolbarListener->RegisterSettings();
    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    MAssert(pmUICommandService != NULL, "Service not found!");
    pmUICommandService->BindCommands(m_pmToolbarListener);
}
//---------------------------------------------------------------------------
void MPlugin::InitStatics()
{
#if _MSC_VER == 1310
    __crt_dll_initialize();
#endif
}
//---------------------------------------------------------------------------
void MPlugin::ShutdownStatics()
{
#if _MSC_VER == 1310
    __crt_dll_terminate();
#endif
}
