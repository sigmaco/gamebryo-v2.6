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

#include "MNoShadowsRenderingMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MNoShadowsRenderingMode::MNoShadowsRenderingMode() :
    m_pkAlphaAccumulator(NULL), m_pkErrors(NULL)
{
    m_pkAlphaAccumulator = NiNew NiAlphaAccumulator();
    MInitRefObject(m_pkAlphaAccumulator);

    m_pkErrors = NiNew NiDefaultErrorHandler();
    MInitInterfaceReference(m_pkErrors);
}
//---------------------------------------------------------------------------
void MNoShadowsRenderingMode::Do_Dispose(bool)
{
    MDisposeInterfaceReference(m_pkErrors);
    MDisposeRefObject(m_pkAlphaAccumulator);
}
//---------------------------------------------------------------------------
String* MNoShadowsRenderingMode::get_Name()
{
    MVerifyValidInstance;

    return "No Shadows";
}
//---------------------------------------------------------------------------
bool MNoShadowsRenderingMode::get_DisplayToUser()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
void MNoShadowsRenderingMode::Update(float)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MNoShadowsRenderingMode::Begin(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Clear out error handler.
    m_pkErrors->ClearErrors();

    // Clear out visible array.
    pkRenderingContext->m_pkCullingProcess->GetVisibleSet()->RemoveAll();
}
//---------------------------------------------------------------------------
void MNoShadowsRenderingMode::Render(MEntity* pmEntity,
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* amEntities[] = {pmEntity};
    Render(amEntities, pmRenderingContext);
}
//---------------------------------------------------------------------------
void MNoShadowsRenderingMode::Render(MEntity* amEntities[],
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
void MNoShadowsRenderingMode::End(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();
    NiRenderer* pkRenderer = pkRenderingContext->m_pkRenderer;
    NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
        ->GetVisibleSet();
    NIASSERT(pkVisibleSet);

    // Set up the renderer's camera data.
    pkRenderer->SetCameraData(pkRenderingContext->m_pkCamera);

    // Set accumulator.
    NiAccumulatorPtr spOldAccumulator = pkRenderer->GetSorter();
    pkRenderer->SetSorter(m_pkAlphaAccumulator);

    // Draw objects in the visible array.
    NiDrawVisibleArray(pkRenderingContext->m_pkCamera, *pkVisibleSet);

    // Restore accumulator.
    pkRenderingContext->m_pkRenderer->SetSorter(spOldAccumulator);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------
