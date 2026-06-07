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

#include "MStandardRenderingMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MStandardRenderingMode::MStandardRenderingMode() : m_pkAlphaAccumulator(NULL),
    m_pkErrors(NULL), m_pkShadowRenderStep(NULL)
{
    m_pkAlphaAccumulator = NiNew NiAlphaAccumulator();
    MInitRefObject(m_pkAlphaAccumulator);

    m_pkErrors = NiNew NiDefaultErrorHandler();
    MInitInterfaceReference(m_pkErrors);

    m_pkShadowRenderStep = NiNew NiDefaultClickRenderStep;
    MInitRefObject(m_pkShadowRenderStep);

    m_pmEntitiesToRender = new ArrayList();
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkShadowRenderStep);
    MDisposeInterfaceReference(m_pkErrors);
    MDisposeRefObject(m_pkAlphaAccumulator);
}
//---------------------------------------------------------------------------
String* MStandardRenderingMode::get_Name()
{
    MVerifyValidInstance;

    return "Standard";
}
//---------------------------------------------------------------------------
bool MStandardRenderingMode::get_DisplayToUser()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Update(float)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Begin(MRenderingContext*)
{
    MVerifyValidInstance;

    // Clear out error handler.
    m_pkErrors->ClearErrors();

    // Clear out array of entities to render.
    m_pmEntitiesToRender->Clear();
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Render(MEntity* pmEntity,
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* amEntities[] = {pmEntity};
    Render(amEntities, pmRenderingContext);
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Render(MEntity* amEntities[],
    MRenderingContext*)
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");

    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        MAssert(pmEntity != NULL, "Null entity in array!");

        m_pmEntitiesToRender->Add(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::End(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");
    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Activate the shadow manager and inform it to retain its current
    // shadow maps.
    NiShadowManager::SetActive(true, true);

    // Backup current render target group.
    const NiRenderTargetGroup* pkCurRenderTarget = pkRenderingContext
        ->m_pkRenderer->GetCurrentRenderTargetGroup();
    MAssert(pkCurRenderTarget != NULL, "No active render target group!");
    pkRenderingContext->m_pkRenderer->EndUsingRenderTargetGroup();

    // Generate list of shadow render clicks.
    NiShadowManager::SetSceneCamera(pkRenderingContext->m_pkCamera);
    const NiTPointerList<NiRenderClick*>& kShadowClicks =
        NiShadowManager::GenerateRenderClicks();
    m_pkShadowRenderStep->GetRenderClickList().RemoveAll();
    NiTListIterator kIter = kShadowClicks.GetHeadPos();
    while (kIter)
    {
        m_pkShadowRenderStep->AppendRenderClick(kShadowClicks.GetNext(kIter));
    }

    // Render shadow maps.
    m_pkShadowRenderStep->Render();

    // Restore previous render target group.
    if (pkRenderingContext->m_pkRenderer->IsRenderTargetGroupActive())
    {
        pkRenderingContext->m_pkRenderer->EndUsingRenderTargetGroup();
    }
    pkRenderingContext->m_pkRenderer->BeginUsingRenderTargetGroup(
        (NiRenderTargetGroup*) pkCurRenderTarget, NiRenderer::CLEAR_NONE);

    // Get visible set.
    NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
        ->GetVisibleSet();
    NIASSERT(pkVisibleSet);

    // Clear out visible set.
    pkVisibleSet->RemoveAll();

    // Build visible set. This is built here instead of in the Render function
    // because shadow maps are rendered at the beginning of this function.
    // After being culled, NiMesh objects must be rendered before they are
    // culled again, so the initial culling pass cannot happen prior to
    // rendering the shadow maps.
    MEntity* amEntities[] = static_cast<MEntity*[]>(
        m_pmEntitiesToRender->ToArray(__typeof(MEntity)));
    for (int i = 0; i < amEntities->Length; ++i)
    {
        amEntities[i]->GetNiEntityInterface()->BuildVisibleSet(
            pkRenderingContext, m_pkErrors);
    }

    // Set up the renderer's camera data.
    pkRenderingContext->m_pkRenderer->SetCameraData(
        pkRenderingContext->m_pkCamera);

    // Set accumulator.
    NiAccumulatorPtr spOldAccumulator = pkRenderingContext->m_pkRenderer
        ->GetSorter();
    pkRenderingContext->m_pkRenderer->SetSorter(m_pkAlphaAccumulator);

    for (unsigned int ui = 0; ui < pkVisibleSet->GetCount(); ui++)
        pkVisibleSet->GetAt(ui).SetMaterialNeedsUpdate(true);
    
    // Draw objects in the visible set.
    NiDrawVisibleArray(pkRenderingContext->m_pkCamera, *pkVisibleSet);

    // Restore accumulator.
    pkRenderingContext->m_pkRenderer->SetSorter(spOldAccumulator);

    // Deactivate the shadow manager and inform it to retain its current
    // shadow maps.
    NiShadowManager::SetActive(false, true);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);

    // Clear out array of entities to render.
    m_pmEntitiesToRender->Clear();
}
//---------------------------------------------------------------------------
