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

#include "MGhostRenderingMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MGhostRenderingMode::MGhostRenderingMode() :
    m_pkAlphaAccumulator(NULL), m_pkErrors(NULL), m_pkAlphaProperty(NULL)
{
    m_pkAlphaAccumulator = NiNew NiAlphaAccumulator();
    MInitRefObject(m_pkAlphaAccumulator);

    m_pkErrors = NiNew NiDefaultErrorHandler();
    MInitInterfaceReference(m_pkErrors);

    m_pkAlphaProperty = NiNew NiAlphaProperty();
    m_pkAlphaProperty->SetAlphaBlending(true);
    m_pkAlphaProperty->SetSrcBlendMode(NiAlphaProperty::ALPHA_INVSRCCOLOR);
    m_pkAlphaProperty->SetDestBlendMode(NiAlphaProperty::ALPHA_ONE);
    MInitRefObject(m_pkAlphaProperty);
}
//---------------------------------------------------------------------------
void MGhostRenderingMode::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkAlphaProperty);
    MDisposeInterfaceReference(m_pkErrors);
    MDisposeRefObject(m_pkAlphaAccumulator);
}
//---------------------------------------------------------------------------
String* MGhostRenderingMode::get_Name()
{
    MVerifyValidInstance;

    return "Ghost";
}
//---------------------------------------------------------------------------
bool MGhostRenderingMode::get_DisplayToUser()
{
    MVerifyValidInstance;

    return false;
}
//---------------------------------------------------------------------------
void MGhostRenderingMode::Update(float)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MGhostRenderingMode::Begin(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Clear out error handler.
    m_pkErrors->ClearErrors();

    // Set up the renderer's camera data.
    pkRenderingContext->m_pkRenderer->SetCameraData(
        pkRenderingContext->m_pkCamera);

    // Clear out visible array.
    pkRenderingContext->m_pkCullingProcess->GetVisibleSet()->RemoveAll();
}
//---------------------------------------------------------------------------
void MGhostRenderingMode::Render(MEntity* pmEntity,
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* amEntities[] = {pmEntity};
    Render(amEntities, pmRenderingContext);
}
//---------------------------------------------------------------------------
void MGhostRenderingMode::Render(MEntity* amEntities[],
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");
    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    NiTObjectArray<NiPropertyPtr> kOldProperties(0);
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        MAssert(pmEntity != NULL, "Null entity in array!");

        pmEntity->GetNiEntityInterface()->BuildVisibleSet(pkRenderingContext,
            m_pkErrors);
    }
}
//---------------------------------------------------------------------------
void MGhostRenderingMode::End(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();
    NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
        ->GetVisibleSet();
    NIASSERT(pkVisibleSet);

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
        NIASSERT(pkPropertyState);
        kOldProperties.SetAt(ui, pkPropertyState->GetAlpha());
        pkPropertyState->SetProperty(m_pkAlphaProperty);
    }

    // Draw objects in the visible set.
    NiDrawVisibleArray(pkRenderingContext->m_pkCamera, *pkVisibleSet);

    // Detach properties.
    for (unsigned int ui = 0; ui < pkVisibleSet->GetCount(); ui++)
    {
        NiPropertyState* pkPropertyState =
            pkVisibleSet->GetAt(ui).GetPropertyState();
        NIASSERT(pkPropertyState);
        pkPropertyState->SetProperty(kOldProperties.GetAt(ui));
    }

    // Restore accumulator.
    pkRenderingContext->m_pkRenderer->SetSorter(spOldAccumulator);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------
