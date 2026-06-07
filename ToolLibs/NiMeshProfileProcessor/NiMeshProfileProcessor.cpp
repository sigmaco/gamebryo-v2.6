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

#include <NiSystem.h>
#include <NiMeshUtilities.h>
#include "NiMeshProfileProcessor.h"
#include "NiOpProfile.h"
#include "NiOpProfileFragment.h"
#include "NiBitPackInfo.h"
#include "NiStringExtraData.h"
#include "NiComponentStreamInput.h"
#include "NiMeshProfileOperations.h"
#include "NiMPPMessages.h"
#include "NiMergeRequirements.h"
#include "NiProfileFinalize.h"

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------

#include <NiVersion.h>

static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING(NiMeshProfileProcessor);

NiMeshProfileProcessor* NiMeshProfileProcessor::ms_pkThis = NULL;

typedef NiDataStreamElement NIDS;
typedef NiToolPipelineCloneHelper::CloneSet CloneSet;
typedef NiToolPipelineCloneHelper::CloneSetPtr CloneSetPtr;

//---------------------------------------------------------------------------
NiMeshProfileProcessor* NiMeshProfileProcessor::CreateMeshProfileProcessor()
{
    if (ms_pkThis)
        return ms_pkThis;

    // Create this profile processor
    ms_pkThis = NiNew NiMeshProfileProcessor;

    return ms_pkThis;
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::DestroyMeshProfileProcessor()
{
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}
//---------------------------------------------------------------------------
NiMeshProfileProcessor* NiMeshProfileProcessor::GetMeshProfileProcessor()
{
    return ms_pkThis;
}
//---------------------------------------------------------------------------
NiMeshProfileProcessor::NiMeshProfileProcessor()
{
    m_pfnProgressInitCallback = NULL;
    m_pfnProgressCallback = NULL;
}
//---------------------------------------------------------------------------
NiMeshProfileProcessor::~NiMeshProfileProcessor()
{
    Reset();
    m_kProfiles.RemoveAll();
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::CreateMeshToProfileMap(
    NiToolPipelineCloneHelper& kCloneHelper,
    NiTPointerMap<NiMesh*, NiOpProfile*>& kMeshToProfile,
    NiSystemDesc::RendererID eRenderer)
{
    // Create a mapping of mesh objects to profiles
    NiTPrimitiveSet<NiMesh*>& kAllMesh = kCloneHelper.GetCompleteMeshSet();
    NiUInt32 uiMeshCnt = kAllMesh.GetSize();
    for(NiUInt32 uiMesh=0; uiMesh<uiMeshCnt; uiMesh++)
    {
        NiMesh* pkMesh = kAllMesh.GetAt(uiMesh);
        NIASSERT(pkMesh);
        NiOpProfile* pkProfile = ms_pkThis->GetProfile(pkMesh, eRenderer);
        if (!pkProfile)
        {
            const char* pcProfileName = GetProfileName(pkMesh);
            const char* pcMeshName = pkMesh->GetName();
            if (pcProfileName && pcMeshName)
            {
                GetErrorHandler().ReportWarning(
                    NiMPPMessages::WARNING_PROFILE_NOT_FOUND_PROF_MESH,
                    pcProfileName, pcMeshName);
            }
            else if (pcMeshName)
            {
                GetErrorHandler().ReportWarning(
                    NiMPPMessages::WARNING_PROFILE_NOT_FOUND_MESH_ONLY,
                    pcMeshName);
            }
            else if (pcProfileName)
            {
                GetErrorHandler().ReportWarning(
                    NiMPPMessages::WARNING_PROFILE_NOT_FOUND_PROF_ONLY,
                    pcProfileName);
            }
            else
            {
                GetErrorHandler().ReportWarning(
                    NiMPPMessages::WARNING_PROFILE_NOT_FOUND);
            }
            pkProfile = ms_pkThis->GetDefaultProfile(eRenderer);
            if (!pkProfile)
                continue;
        }
        kMeshToProfile.SetAt(pkMesh, pkProfile);
    }
}
//---------------------------------------------------------------------------
NiOpProfile* NiMeshProfileProcessor::FindProfile(const char* pcName,
    NiSystemDesc::RendererID eRenderer)
{
    NIASSERT(ms_pkThis);
    
    // Run through and return all the profile names
    for (NiUInt32 ui = 0; ui < ms_pkThis->m_kProfiles.GetSize(); ui++)
    {
        NiOpProfile* pkOpProfile = ms_pkThis->m_kProfiles[ui];
        NIASSERT(pkOpProfile);
        if (pkOpProfile->GetRenderer() == eRenderer &&
            strcmp(pkOpProfile->GetName(), pcName)==0)
        {
            return pkOpProfile;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
const char* NiMeshProfileProcessor::GetProfileName(NiMesh* pkMesh)
{
    NiStringExtraData* pkData = NiDynamicCast(NiStringExtraData, 
        pkMesh->GetExtraData("MeshProfileName"));
    if (pkData)
    {
        return pkData->GetValue();
    }
    else
    {
        return "Default";
    }
}
//---------------------------------------------------------------------------
NiOpProfile* NiMeshProfileProcessor::GetProfile(NiMesh* pkMesh,
    NiSystemDesc::RendererID eRenderer)
{
    NIASSERT(ms_pkThis);

    if (ms_pkThis->m_kProfiles.GetSize() == 0)
        return NULL;

    const char* pcProfileName = GetProfileName(pkMesh);
    
    if (pcProfileName == NULL)
        return NULL;

    NiOpProfile* pkProfile = FindProfile(pcProfileName, eRenderer);

    return pkProfile;
}
//---------------------------------------------------------------------------
NiOpProfile* NiMeshProfileProcessor::GetDefaultProfile( 
        NiSystemDesc::RendererID eRenderer)
{
    return FindProfile("Default", eRenderer);
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::Finalize(NiTLargeObjectArray<NiObjectPtr>&
    kTopObjects, NiSystemDesc::RendererID eRenderer)
{
    NIASSERT(ms_pkThis);
    Reset();

    NiToolPipelineCloneHelper kCloneHelper;
    kCloneHelper.InitializeCloneSetMaps(kTopObjects);

    NiTPointerMap<NiMesh*, NiOpProfile*> kMeshToProfile;
    CreateMeshToProfileMap(kCloneHelper, kMeshToProfile, eRenderer);
    NiProfileFinalize kFinalize(kCloneHelper, kMeshToProfile, eRenderer,
        ms_pkThis->GetErrorHandler(),
        ms_pkThis->m_pfnProgressInitCallback,
        ms_pkThis->m_pfnProgressCallback);
    kFinalize.PerformStage();
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::Finalize(NiMesh* pkMesh,
    NiSystemDesc::RendererID eRenderer)
{
    NIASSERT(ms_pkThis);
    NIASSERT(pkMesh);
    Reset();
    
    NiToolPipelineCloneHelper kCloneHelper;
    kCloneHelper.InitializeCloneSetMaps(pkMesh);

    NiTPointerMap<NiMesh*, NiOpProfile*> kMeshToProfile;
    CreateMeshToProfileMap(kCloneHelper, kMeshToProfile, eRenderer);
    NiProfileFinalize kFinalize(kCloneHelper, kMeshToProfile, eRenderer,
        ms_pkThis->GetErrorHandler(),
        ms_pkThis->m_pfnProgressInitCallback,
        ms_pkThis->m_pfnProgressCallback);
    kFinalize.PerformStage();
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::GetAvailableProfiles(
    NiTPrimitiveSet<const char*>& kProfiles, 
    NiSystemDesc::RendererID eRenderer)
{
    NIASSERT(ms_pkThis);
    
    // Run through and return all the profile names
    for (NiUInt32 ui = 0; ui < ms_pkThis->m_kProfiles.GetSize(); ui++)
    {
        NiOpProfile* pkProfile = ms_pkThis->m_kProfiles.GetAt(ui);
        if (pkProfile->GetRenderer() == eRenderer)
        {
            kProfiles.Add(pkProfile->GetName());
        }
    }
}
//---------------------------------------------------------------------------
const NiOpProfile* NiMeshProfileProcessor::GetProfile(const char* pcName,
    NiSystemDesc::RendererID eRenderer)
{
    return ms_pkThis->FindProfile(pcName, eRenderer);
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::Reset()
{
    NIASSERT(ms_pkThis);

    GetErrorHandler().ClearMessages();
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::AddProfile(NiOpProfile* pkProfile)
{
    NIASSERT(ms_pkThis);
    NIASSERT(pkProfile);
    ms_pkThis->m_kProfiles.Add(pkProfile);
}
//---------------------------------------------------------------------------
NiMeshProfileErrorHandler& NiMeshProfileProcessor::GetErrorHandler()
{
    NIASSERT(ms_pkThis);
    return ms_pkThis->m_kReportedErrors;
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::SetProgressCallback(ProgressCallback
    pfnCallback)
{
    NIASSERT(ms_pkThis);
    ms_pkThis->m_pfnProgressCallback = pfnCallback;
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::GetProgressCallback(ProgressCallback&
    pfnCallback)
{
    NIASSERT(ms_pkThis);
    pfnCallback = ms_pkThis->m_pfnProgressCallback;
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::SetProgressInitCallback(ProgressInitCallback
    pfnCallback)
{
    NIASSERT(ms_pkThis);
    ms_pkThis->m_pfnProgressInitCallback = pfnCallback;
}
//---------------------------------------------------------------------------
void NiMeshProfileProcessor::GetProgressInitCallback(ProgressInitCallback&
    pfnCallback)
{
    NIASSERT(ms_pkThis);
    pfnCallback = ms_pkThis->m_pfnProgressInitCallback;
}
//---------------------------------------------------------------------------
