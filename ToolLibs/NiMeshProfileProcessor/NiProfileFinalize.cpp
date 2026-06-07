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
//---------------------------------------------------------------------------

#include "NiProfileFinalize.h"
#include "NiOpProfile.h"
#include "NiMergeRequirements.h"
#include "NiMPPMessages.h"

typedef NiDataStreamElement NIDS;
typedef NiToolPipelineCloneHelper::CloneSet CloneSet;
typedef NiToolPipelineCloneHelper::CloneSetPtr CloneSetPtr;

//--------------------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------------------
void ReportAnnotation(NiMeshErrorInterface* pkError, 
    NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet, const char*
    pcAnnotation)
{
    NiString strAnnotation;
    strAnnotation = pcAnnotation;
    strAnnotation += "{";
    NiUInt32 uiClones = spCloneSet->GetSize();
    for(NiUInt32 ui=0; ui<uiClones; ui++)
    {
        NiMesh* pkMesh = spCloneSet->GetAt(ui);
        strAnnotation += pkMesh->GetName();
        if (ui < uiClones-1)
            strAnnotation += ", ";
        else
            strAnnotation += "}\n";
    }
    pkError->ReportAnnotation(strAnnotation);
}
//---------------------------------------------------------------------------
/*
void DebugLogMerge(NiMeshRequirements& kReq, NiMeshProfileErrorHandler& kLog)
{
    if (kReq.GetSetCount() < 1)
        return; // could not set.

    NiUInt32 uiSetCnt = kReq.GetSetCount();

    for(NiUInt32 uiSet=0; uiSet<uiSetCnt; uiSet++)
    {
        kLog.ReportAnnotation("Set: %d\n", uiSet);

        for (NiUInt32 ui = 0; ui < kReq.GetRequirementCount(uiSet); ui++)
        {
            NiMeshRequirements::SemanticRequirement* pkReq = 
                kReq.GetRequirement(uiSet, ui);

            if (!pkReq)
                continue;

            const NiUInt32 cuiMaxSemanticLength = 32;
            NiString kStr;
            kStr.Format("%s,%d", pkReq->m_kSemantic, pkReq->m_uiIndex);
            NIASSERT(kStr.Length() <= cuiMaxSemanticLength);
            NiUInt32 uiDiff = cuiMaxSemanticLength - kStr.Length();
            for(NiUInt32 uiSpace=0; uiSpace < uiDiff; uiSpace++)
                kStr += " ";

            NiString kTmp;
            if (pkReq->m_eStreamBehavior == NiMeshRequirements::
                STRICT_INTERLEAVE)
            {
                kTmp.Format("STRICT   =%d ", pkReq->m_uiStreamID);
            }
            else if (pkReq->m_eStreamBehavior == NiMeshRequirements::CAN_SPLIT)
            {
                kTmp.Format("CAN_SPLIT=%d ", pkReq->m_uiStreamID); 
            }
            else
            {
                kTmp = "FLOATER    "; 
            }

            kStr += kTmp;

            switch(pkReq->m_kUsage)
            {
                case NiDataStream::USAGE_USER: kStr +=         "USER   :";
                    break;
                case NiDataStream::USAGE_VERTEX: kStr +=       "VERTEX :";
                    break;
                case NiDataStream::USAGE_VERTEX_INDEX: kStr += "INDEX  :";
                    break;
                default: kStr +=                               "UNKNOWN:";
                    break;
            }

            kTmp.Format("0x%x { ", pkReq->m_uiAccessFlags);
            kStr += kTmp;

            NiUInt32 uiPossibleFCnt = pkReq->m_kFormats.GetSize();
            if (uiPossibleFCnt == 14) // the 14 known ones
            {
                // Special case - don't care - any format possible
                kStr += "ANY_FORMAT ";
            }
            else
            {
                for(NiUInt32 uiFIndex=0; uiFIndex < uiPossibleFCnt; uiFIndex++)
                {
                    switch(pkReq->m_kFormats.GetAt(uiFIndex))
                    {
                        case NIDS::F_INT8_1: kStr += "INT8_1 "; break;
                        case NIDS::F_INT8_2: kStr += "INT8_2 "; break;
                        case NIDS::F_INT8_3: kStr += "INT8_3 "; break;
                        case NIDS::F_INT8_4: kStr += "INT8_4 "; break;
                        case NIDS::F_UINT8_1: kStr += "UINT8_1 "; break;
                        case NIDS::F_UINT8_2: kStr += "UINT8_2 "; break;
                        case NIDS::F_UINT8_3: kStr += "UINT8_3 "; break;
                        case NIDS::F_UINT8_4: kStr += "UINT8_4 "; break;
                        case NIDS::F_NORMINT8_1: kStr += "NORMINT8_1 "; break;
                        case NIDS::F_NORMINT8_2: kStr += "NORMINT8_2 "; break;
                        case NIDS::F_NORMINT8_3: kStr += "NORMINT8_3 "; break;
                        case NIDS::F_NORMINT8_4: kStr += "NORMINT8_4 "; break;
                        case NIDS::F_NORMUINT8_1: kStr += "NORMUINT8_1 ";break;
                        case NIDS::F_NORMUINT8_2: kStr += "NORMUINT8_2 ";break;
                        case NIDS::F_NORMUINT8_3: kStr += "NORMUINT8_3 ";break;
                        case NIDS::F_NORMUINT8_4: kStr += "NORMUINT8_4 ";break;
                        case NIDS::F_INT16_1: kStr += "INT16_1 "; break;
                        case NIDS::F_INT16_2: kStr += "INT16_2 "; break;
                        case NIDS::F_INT16_3: kStr += "INT16_3 "; break;
                        case NIDS::F_INT16_4: kStr += "INT16_4 "; break;
                        case NIDS::F_UINT16_1: kStr += "UINT16_1 "; break;
                        case NIDS::F_UINT16_2: kStr += "UINT16_2 "; break;
                        case NIDS::F_UINT16_3: kStr += "UINT16_3 "; break;
                        case NIDS::F_UINT16_4: kStr += "UINT16_4 "; break;
                        case NIDS::F_NORMINT16_1: kStr += "NORMINT16_1 ";break;
                        case NIDS::F_NORMINT16_2: kStr += "NORMINT16_2 ";break;
                        case NIDS::F_NORMINT16_3: kStr += "NORMINT16_3 ";break;
                        case NIDS::F_NORMINT16_4: kStr += "NORMINT16_4 ";break;
                        case NIDS::F_NORMUINT16_1: kStr += "NORMUINT16_1 ";
                            break;
                        case NIDS::F_NORMUINT16_2: kStr += "NORMUINT16_2 ";
                            break;
                        case NIDS::F_NORMUINT16_3: kStr += "NORMUINT16_3 ";
                            break;
                        case NIDS::F_NORMUINT16_4: kStr += "NORMUINT16_4 ";
                            break;
                        case NIDS::F_INT32_1: kStr += "INT32_1 "; break;
                        case NIDS::F_INT32_2: kStr += "INT32_2 "; break;
                        case NIDS::F_INT32_3: kStr += "INT32_3 "; break;
                        case NIDS::F_INT32_4: kStr += "INT32_4 "; break;
                        case NIDS::F_UINT32_1: kStr += "UINT32_1 "; break;
                        case NIDS::F_UINT32_2: kStr += "UINT32_2 "; break;
                        case NIDS::F_UINT32_3: kStr += "UINT32_3 "; break;
                        case NIDS::F_UINT32_4: kStr += "UINT32_4 "; break;
                        case NIDS::F_NORMINT32_1: kStr += "NORMINT32_1 ";break;
                        case NIDS::F_NORMINT32_2: kStr += "NORMINT32_2 ";break;
                        case NIDS::F_NORMINT32_3: kStr += "NORMINT32_3 ";break;
                        case NIDS::F_NORMINT32_4: kStr += "NORMINT32_4 ";break;
                        case NIDS::F_NORMUINT32_1: kStr += "NORMUINT32_1 ";
                            break;
                        case NIDS::F_NORMUINT32_2: kStr += "NORMUINT32_2 ";
                            break;
                        case NIDS::F_NORMUINT32_3: kStr += "NORMUINT32_3 ";
                            break;
                        case NIDS::F_NORMUINT32_4: kStr += "NORMUINT32_4 ";
                            break;
                        case NIDS::F_FLOAT16_1: kStr += "FLOAT16_1 "; break;
                        case NIDS::F_FLOAT16_2: kStr += "FLOAT16_2 "; break;
                        case NIDS::F_FLOAT16_3: kStr += "FLOAT16_3 "; break;
                        case NIDS::F_FLOAT16_4: kStr += "FLOAT16_4 "; break;
                        case NIDS::F_FLOAT32_1: kStr += "FLOAT32_1 "; break;
                        case NIDS::F_FLOAT32_2: kStr += "FLOAT32_2 "; break;
                        case NIDS::F_FLOAT32_3: kStr += "FLOAT32_3 "; break;
                        case NIDS::F_FLOAT32_4: kStr += "FLOAT32_4 "; break;
                        default: kStr += "UNKNOWN "; break;
                    }
                }
            }

            kStr += "}\n";

            kLog.ReportAnnotation(kStr);
        }
    }
}
//---------------------------------------------------------------------------
void DebugLogMerge(NiMeshRequirements& kProfileReq, NiMeshRequirements&
    kModifierReq, NiMeshRequirements& kMerged)
{
    NiMeshProfileErrorHandler kLog;
    kLog.ReportAnnotation("%s Requirements\n", kProfileReq.m_kReqName);
    DebugLogMerge(kProfileReq, kLog);

    kLog.ReportAnnotation("\n%s Requirements\n", kModifierReq.m_kReqName);
    DebugLogMerge(kModifierReq, kLog);

    kLog.ReportAnnotation("\n%s (merged) Requirements\n", kMerged.m_kReqName);
    DebugLogMerge(kMerged, kLog);

    kLog.ReportAnnotation("\n\nNotes on Access Flags\n");
    kLog.ReportAnnotation("x03:          | CPU_READ | CPU_WRITE_STATIC  |\n");
    kLog.ReportAnnotation("x12: GPU_READ |          | CPU_WRITE_STATIC  |\n");
    kLog.ReportAnnotation("x13: GPU_READ | CPU_READ | CPU_WRITE_STATIC  |\n");
    kLog.ReportAnnotation("x18: GPU_READ |          |                   | " \
        "CPU_WRITE_VOLATILE\n");

    kLog.SaveMessages("c:\\testfile.txt");
}
*/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
NiProfileFinalize::NiProfileFinalize(NiToolPipelineCloneHelper& kCloneHelper,
    NiTPointerMap<NiMesh*, NiOpProfile*>& kMeshToProfile,
    NiSystemDesc::RendererID eRenderer, NiMeshProfileErrorHandler& kLog,
    ProgressInitCallback pfnProgressInitCallback,
    ProgressCallback pfnProgressCallback) : NiProfileStage(kCloneHelper,
    kMeshToProfile, eRenderer, kLog, pfnProgressInitCallback,
    pfnProgressCallback)
{
}
//---------------------------------------------------------------------------
void NiProfileFinalize::PerformStage()
{
    NiTPrimitiveSet<CloneSet*> kAllCloneSets;
    m_pkCloneHelper->RetrieveAllCloneSets(kAllCloneSets);
    NiUInt32 uiNumCloneSets = kAllCloneSets.GetSize();

    NiTPrimitiveSet<NiMesh*>& kAllMesh = m_pkCloneHelper->GetCompleteMeshSet();
    NiUInt32 uiMeshCnt = kAllMesh.GetSize();

    // Initialize to the iterations that will be perform.
    if (m_pfnProgressInitCallback)
        m_pfnProgressInitCallback(2*uiNumCloneSets*2 + uiMeshCnt*3);

    // While we could de-interleave only streams that have profile operations
    // applied within NiProfileStage::CreateFragments, we instead choose to
    // De-interleave all streams and allow the profile to splice back the
    // streams if it sees fit.
    DeInterleave(kAllCloneSets);

    // Create Finalize Cloneset Profile-Fragments. Iterates over all mesh.
    CreateFinalizeFragments(kAllMesh);

    // Adjust fragments due to modifiers, etc. Iterates over all mesh
    AdjustFinalizeFragments(kAllMesh);

    // Resolve all Fragments. Iterates over all clone sets.
    ResolveFragments(kAllCloneSets);

    // InterleaveFragments. Iterates over all clone sets.
    InterleaveFinalizeFragment(kAllCloneSets);

    // Validate modifiers, recalc bounds, etc. Iterates over all mesh
    PostFinalizeStep(kAllMesh);

}
//---------------------------------------------------------------------------
void NiProfileFinalize::CreateFinalizeFragments(NiTPrimitiveSet<NiMesh*>&
    kAllMesh)
{
    NiUInt32 uiMeshCnt = kAllMesh.GetSize();
    for(NiUInt32 uiMesh=0; uiMesh<uiMeshCnt; uiMesh++)
    {
        NiMesh* pkMesh = kAllMesh.GetAt(uiMesh);
        NIASSERT(pkMesh);
        NiOpProfile* pkMasterProfile = NULL;
        m_pkMeshToProfile->GetAt(pkMesh, pkMasterProfile);

        if (pkMasterProfile == NULL)
        {
            // Still need to process to add the NULL fragment to the clone map
            NiTObjectArray<NiOpStartPtr> kStartNodes;
            NiTObjectArray<NiOpTerminatorPtr> kTerminatorNodes;
            CreateFragments(pkMesh, NULL, kStartNodes, kTerminatorNodes, true);
            continue;
        }
        
        CreateFragments(pkMesh, pkMasterProfile,
            pkMasterProfile->GetFinalizeStartNodes(),
            pkMasterProfile->GetFinalizeTerminatorNodes(),
            pkMasterProfile->GetKeepUnspecifiedFinalizeStreams());

        if (m_pfnProgressCallback)
            m_pfnProgressCallback(*m_pkLog);
    }
}
//---------------------------------------------------------------------------
void NiProfileFinalize::AdjustFinalizeFragments(NiTPrimitiveSet<NiMesh*>&
    kAllMesh)
{
    NiUInt32 uiMeshCnt = kAllMesh.GetSize();

    // Adjust Profile Fragments due to modifiers and clonesets
    for(NiUInt32 uiMesh=0; uiMesh<uiMeshCnt; uiMesh++)
    {
        NiMesh* pkMesh = kAllMesh.GetAt(uiMesh);
        NIASSERT(pkMesh);

        // Among other things, will merge interleaving requirements. Note that
        // Interleaving between clonesets is not allowed.
        MergeModifierRequirementsIntoProfileFragment(pkMesh);

        if (m_pfnProgressCallback)
            m_pfnProgressCallback(*m_pkLog);
    }
}
//---------------------------------------------------------------------------
bool NiProfileFinalize::MergeModifierRequirementsIntoProfileFragment(
    NiMesh* pkMesh)
{
    // No further adjustments needed if no modifiers
    if (pkMesh->GetModifierCount() == 0)
        return true;

    // Retrieve Requirements from each fragment.
    NiTObjectSet<CloneSetPtr> kCloneSets;
    m_pkCloneHelper->GetCloneSetsThatIncludeMesh(kCloneSets, pkMesh);

    // All fragment requirements get pushed into kProfileRequirements.
    NiMeshRequirements kRequirements("Merged", m_eRenderer);

    NiUInt32 uiCloneSets = kCloneSets.GetSize();
    for(NiUInt32 uiSet = 0; uiSet < uiCloneSets; uiSet++)
    {
        CloneSet* pkCS = kCloneSets.GetAt(uiSet);
        NiOpProfileFragmentPtr spProfileFragment = NULL;

        NIVERIFY(m_kCloneSetToProfileFragment.GetAt(pkCS, spProfileFragment));
        
        if (spProfileFragment)
            spProfileFragment->GetRequirements(kRequirements);
    }



    // Now merge profiles by these kProfileRequirements
    bool bAtLeastOneSuccessfullMerge = false;

    for (NiUInt32 ui = 0; ui < pkMesh->GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = pkMesh->GetModifierAt(ui);
        if (!pkModifier)
            continue;

        NiMeshRequirements kModifierReq(pkModifier->GetRTTI()->GetName(), 
            m_eRenderer);
        NiMeshRequirements kMergedReq("Merged", m_eRenderer);
        pkModifier->RetrieveRequirements(kModifierReq);

        NiMeshProfileErrorHandler kTmpHandler;
        if (NiMergeRequirements::MergeRequirementsIntoProfile(kRequirements,
            kModifierReq, kMergedReq, &kTmpHandler))
        {
            kMergedReq.m_kReqName.Format("%s_%s", kRequirements.m_kReqName,
                kModifierReq.m_kReqName);

            // Uncomment to log out profile requirements, modifiers
            // requirements, and the merge of the two.
            //DebugLogMerge(kRequirements, kModifierReq, kMergedReq);

            kRequirements = kMergedReq;
            bAtLeastOneSuccessfullMerge = true;
        }
        else
        {
            m_pkLog->ReportWarning(NiMPPMessages::
                WARNING_FAILED_TO_MERGE_1ARG,
                pkModifier->GetRTTI()->GetName());

            // Report the errors and warnings of the function as well.
            for(NiUInt32 uiErr=0; uiErr<kTmpHandler.GetMessageCount();
                uiErr++)
            {
                // Add some additional formatting and indention
                if (kTmpHandler.GetMessageType(uiErr) == 
                    NiMeshProfileErrorHandler::MT_ANNOTATION)
                {
                    // Report the annotation as an error
                    m_pkLog->ReportError("    %s",
                        kTmpHandler.GetMessage(uiErr));

                }
                else
                {
                    // Add some indention
                    m_pkLog->ReportError("      %s",
                        kTmpHandler.GetMessage(uiErr));
                }
            }
        }
    }

    // If there is at least one successful merge, the function considers
    // this a success.
    if (bAtLeastOneSuccessfullMerge == false)
    {
        return false;
    }

    // Take the requirements and adjust the
    for(NiUInt32 uiSet = 0; uiSet < uiCloneSets; uiSet++)
    {
        CloneSet* pkCS = kCloneSets.GetAt(uiSet);
        NiOpProfileFragmentPtr spProfileFragment = NULL;

        NIVERIFY(m_kCloneSetToProfileFragment.GetAt(pkCS, spProfileFragment));

        spProfileFragment->AdjustFinalizeOps(kRequirements);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiProfileFinalize::InterleaveFinalizeFragment(
    NiTPrimitiveSet<NiToolPipelineCloneHelper::CloneSet*>& kAllCloneSets)
{
    NiUInt32 uiNumCloneSets = kAllCloneSets.GetSize();

    for (NiUInt32 ui=0; ui<uiNumCloneSets; ui++)
    {
        CloneSetPtr spCloneSet = kAllCloneSets.GetAt(ui);

        NIASSERT(spCloneSet->GetSize() > 0);

        NiOpProfileFragmentPtr spProfile = NULL;
        if (m_kCloneSetToProfileFragment.GetAt(spCloneSet, spProfile) == false)
        {
            // Notice about skipping interleaving was given by resolveerror
            continue;
        }

        if (spProfile->GetHadResolveErrors())
        {
            // Notice about skipping interleaving was given by resolve error
            continue;
        }

        ReportAnnotation(m_pkLog, spCloneSet,
            "\nInterleaving datastreams in Mesh(s): ");

        spProfile->Interleave(spCloneSet, m_pkLog);

        if (m_pfnProgressCallback)
            m_pfnProgressCallback(*m_pkLog);
    }
}
//---------------------------------------------------------------------------
void NiProfileFinalize::PostFinalizeStep(NiTPrimitiveSet<NiMesh*>& kAllMesh)
{
    NiUInt32 uiMeshCnt = kAllMesh.GetSize();

    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        NiMesh* pkMesh = kAllMesh.GetAt(ui);

        // Attach calls assumed to not fail.
        NiMesh::ResetSceneModifiers(pkMesh);

        ValidateMeshModifiers(pkMesh);

        NiOpProfile* pkMasterProfile = NULL;
        m_pkMeshToProfile->GetAt(pkMesh, pkMasterProfile);

        // Remove extra data concerning profile name.
        pkMesh->RemoveExtraData("MeshProfileName");

        if (pkMasterProfile)
            pkMasterProfile->PostResolveFinalizeOps(pkMesh);

        if (m_pfnProgressCallback)
            m_pfnProgressCallback(*m_pkLog);
    }
}
//---------------------------------------------------------------------------
void NiProfileFinalize::ValidateMeshModifiers(NiMesh* pkMesh)
{
    if (!pkMesh)
        return;

    if (pkMesh->GetModifierCount() == 0)
        return;

    m_pkLog->ReportAnnotation(
        "\nValidating Mesh Modifiers for Mesh: %s\n", pkMesh->GetName());

    for (NiUInt32 ui = 0; ui < pkMesh->GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = pkMesh->GetModifierAt(ui);
        if (!pkModifier->AreRequirementsMet(pkMesh, m_eRenderer))
        {
            // Remove the modifier from the mesh.
            char acRTTI[256];
            if (pkModifier->GetRTTI()->CopyName(acRTTI, 256) == false)
                NiStrcpy(acRTTI, 256, "UNKNOWN");
            pkMesh->RemoveModifier(pkModifier);
            m_pkLog->ReportWarning(
                NiMPPMessages::WARNING_MESH_MODIFIER_REMOVED_1ARG, acRTTI);
        }
    }
}
//---------------------------------------------------------------------------



