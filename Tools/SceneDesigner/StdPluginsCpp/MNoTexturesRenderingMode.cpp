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

#include <NiD3DShaderFactory.h>
#include "MNoTexturesRenderingMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MNoTexturesRenderingMode::MNoTexturesRenderingMode(bool bWireframe) :
    m_pkAlphaAccumulator(NULL),
    m_pkErrors(NULL),
    m_pkTexturingProperty(NULL),
    m_bWireframe(bWireframe)
{
    m_pkAlphaAccumulator = NiNew NiAlphaAccumulator();
    MInitRefObject(m_pkAlphaAccumulator);

    m_pkErrors = NiNew NiDefaultErrorHandler();
    MInitInterfaceReference(m_pkErrors);

    m_pkTexturingProperty = NiNew NiTexturingProperty();
    MInitRefObject(m_pkTexturingProperty);

    m_pkColor = NiNew NiColor(1.0f, 1.0f, 1.0f);
    m_pkPropState = NiNew NiPropertyState();
    m_pkPropState->IncRefCount();

    NiWireframeProperty* pkWireProp = NiNew NiWireframeProperty();
    pkWireProp->SetWireframe(true);
    NiZBufferProperty* pkZBufferProp = NiNew NiZBufferProperty();
    pkZBufferProp->SetZBufferTest(true);
    pkZBufferProp->SetZBufferWrite(true);
    pkZBufferProp->SetTestFunction(NiZBufferProperty::TEST_ALWAYS);
    NiMaterialProperty* pkMaterialProp = NiNew NiMaterialProperty();
    m_pkPropState->SetProperty(pkWireProp);
    m_pkPropState->SetProperty(pkZBufferProp);
    m_pkPropState->SetProperty(pkMaterialProp);
}
//---------------------------------------------------------------------------
void MNoTexturesRenderingMode::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkPropState);
    NiDelete m_pkColor;

    MDisposeRefObject(m_pkTexturingProperty);
    MDisposeInterfaceReference(m_pkErrors);
    MDisposeRefObject(m_pkAlphaAccumulator);
}
//---------------------------------------------------------------------------
String* MNoTexturesRenderingMode::get_Name()
{
    MVerifyValidInstance;

/*
    ISettingsService* pmSettingService = dynamic_cast<ISettingsService *>(
        ServiceProvider::Instance->GetService(__typeof(ISettingsService)));
    Object* pmObject = pmSettingService->GetSettingsObject(
        S"Display Wireframe with No Textures", SettingsCategory::PerUser);
    if (pmObject)
    {
        if (System::Convert::ToBoolean(pmObject))
        {
            return "No Textures + Wireframe";
        }
    }
*/

    if (m_bWireframe)
        return "No Textures + Edged Faces";

    return "No Textures";
}
//---------------------------------------------------------------------------
bool MNoTexturesRenderingMode::get_DisplayToUser()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
void MNoTexturesRenderingMode::Update(float)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MNoTexturesRenderingMode::Begin(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

/*
    m_bWireframe = false;
    ISettingsService* pmSettingService = dynamic_cast<ISettingsService *>(
        ServiceProvider::Instance->GetService(__typeof(ISettingsService)));
    Object* pmObject = pmSettingService->GetSettingsObject(
        S"Display Wireframe with No Textures", SettingsCategory::PerUser);
    if (pmObject)
    {
        if (System::Convert::ToBoolean(pmObject))
        {
            m_bWireframe = true;

            Object* pmObj;
            __box System::Drawing::Color* pmValue;
            NiColor kValue;
            pmObj = pmSettingService->GetSettingsObject(
                "No Textures Wireframe Color", SettingsCategory::PerUser);
            pmValue = dynamic_cast<__box System::Drawing::Color*>(pmObj);
            if (pmValue != NULL)
            {
                m_pkColor->r = MUtility::RGBToFloat((*pmValue).R);
                m_pkColor->g = MUtility::RGBToFloat((*pmValue).G);
                m_pkColor->b = MUtility::RGBToFloat((*pmValue).B);
            }
        }
    }
*/
    if (m_bWireframe)
    {
        ISettingsService* pmSettingService = dynamic_cast<ISettingsService *>(
            ServiceProvider::Instance->GetService(__typeof(ISettingsService)));

        Object* pmObj;
        __box System::Drawing::Color* pmValue;
        NiColor kValue;
        pmObj = pmSettingService->GetSettingsObject(
            "No Textures Wireframe Color", SettingsCategory::PerUser);
        pmValue = dynamic_cast<__box System::Drawing::Color*>(pmObj);
        if (pmValue != NULL)
        {
            m_pkColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkColor->b = MUtility::RGBToFloat((*pmValue).B);
        }
    }

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Clear out error handler.
    m_pkErrors->ClearErrors();

    // Set up the renderer's camera data.
    pkRenderingContext->m_pkRenderer->SetCameraData(
        pkRenderingContext->m_pkCamera);

    // Clear out visible array.
    pkRenderingContext->m_pkCullingProcess->GetVisibleSet()->RemoveAll();

    if (m_bWireframe)
    {
        m_pkPropState->GetMaterial()->SetEmittance(*(m_pkColor));
    }
}
//---------------------------------------------------------------------------
void MNoTexturesRenderingMode::Render(MEntity* pmEntity,
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* amEntities[] = {pmEntity};
    Render(amEntities, pmRenderingContext);
}
//---------------------------------------------------------------------------
void MNoTexturesRenderingMode::Render(MEntity* amEntities[],
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");
    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        MAssert(pmEntity != NULL, "Null entity in array!");

        pmEntity->GetNiEntityInterface()->BuildVisibleSet(pkRenderingContext,
            m_pkErrors);
    }
}
//---------------------------------------------------------------------------
void MNoTexturesRenderingMode::End(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();
    NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
        ->GetVisibleSet();
    assert(pkVisibleSet);

    // Set accumulator.
    NiAccumulatorPtr spOldAccumulator = pkRenderingContext->m_pkRenderer
        ->GetSorter();
    pkRenderingContext->m_pkRenderer->SetSorter(m_pkAlphaAccumulator);

    // Attach properties.
    NiTObjectArray<NiPropertyPtr> kOldProperties(pkVisibleSet->GetCount());
    for (unsigned int ui = 0; ui < pkVisibleSet->GetCount(); ui++)
    {
        NiPropertyState* pkPropertyState =
            pkVisibleSet->GetAt(ui).GetPropertyState();
        assert(pkPropertyState);
        kOldProperties.SetAt(ui, pkPropertyState->GetTexturing());
        pkPropertyState->SetProperty(m_pkTexturingProperty);
        pkVisibleSet->GetAt(ui).SetMaterialNeedsUpdate(true);
    }

    // Draw objects in the visible set.
    NiDrawVisibleArray(pkRenderingContext->m_pkCamera, *pkVisibleSet);

    // Detach properties.
    for (unsigned int ui = 0; ui < pkVisibleSet->GetCount(); ui++)
    {
        NiPropertyState* pkPropertyState =
            pkVisibleSet->GetAt(ui).GetPropertyState();
        assert(pkPropertyState);
        pkPropertyState->SetProperty(kOldProperties.GetAt(ui));
        pkVisibleSet->GetAt(ui).SetMaterialNeedsUpdate(true);
    }

    // If the rendering mode is "No Textures + Edged Faces", draw geometry
    // again as wireframe.
    if (m_bWireframe)
    {
        const unsigned int uiQuantity = pkVisibleSet->GetCount();
        unsigned int ui;
        NiRenderer* pkRenderer = NiRenderer::GetRenderer();

        for (ui = 0; ui < uiQuantity; ui++)
        {
            NiPropertyStatePtr spPrevProp =
                pkVisibleSet->GetAt(ui).GetPropertyState();

            pkVisibleSet->GetAt(ui).SetPropertyState(m_pkPropState);
            pkVisibleSet->GetAt(ui).SetMaterialNeedsUpdate(true);

            pkVisibleSet->GetAt(ui).RenderImmediate(pkRenderer);

            if (spPrevProp)
            {
                pkVisibleSet->GetAt(ui).SetPropertyState(spPrevProp);
                pkVisibleSet->GetAt(ui).SetMaterialNeedsUpdate(true);
            }
        }
    }

    // Restore accumulator.
    pkRenderingContext->m_pkRenderer->SetSorter(spOldAccumulator);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------
