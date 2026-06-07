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

#include "stdafx.h"
#include "resource.h"
#include "NiControllerExtractorPlugin.h"
#include "NiControllerExtractorDialog.h"
#include "ChooseActorRootDialog.h"
#include <NiControllerExtractorSharedData.h>
#include "ScriptParameterDefinitions.h"
#include <errno.h>
#include <NiAnimationCompression.h>
#include <NiControllerExtractor.h>

NiImplementRTTI(NiControllerExtractorPlugin, NiExportPlugin);

//---------------------------------------------------------------------------
NiControllerExtractorPlugin::NiControllerExtractorPlugin() : NiExportPlugin(
    "Controller Extractor", "2.6", "Extract animations for characters.",
    "Extract animations for characters."), m_pkProgressBar(NULL)
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NiControllerExtractorPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiExportPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName(PARAM_PLUGINNAME);
    pkPluginInfo->SetType("EXPORT");

    pkPluginInfo->SetBool(PARAM_BOOL_SINGLEKF, true);
    pkPluginInfo->SetBool(PARAM_BOOL_EXPORTEDFILENAME, true);
    pkPluginInfo->SetBool(PARAM_BOOL_CHARACTERNAME, true);
    pkPluginInfo->SetBool(PARAM_BOOL_UPPERCASE, false);
    pkPluginInfo->SetValue(PARAM_STRING_SEPARATOR, "_");
    pkPluginInfo->SetBool(PARAM_BOOL_CREATEKFM, true);
    pkPluginInfo->SetBool(PARAM_BOOL_CHARACTERNAMEINKFMFILE, true);
    pkPluginInfo->SetInt(PARAM_INT_SYNCTRANSTYPE, NiKFMTool::TYPE_MORPH);
    pkPluginInfo->SetInt(PARAM_INT_NONSYNCTRANSTYPE,
        NiKFMTool::TYPE_CROSSFADE);
    pkPluginInfo->SetFloat(PARAM_FLOAT_SYNCTRANSDURATION, 0.1f);
    pkPluginInfo->SetFloat(PARAM_FLOAT_NONSYNCTRANSDURATION, 0.1f);
    pkPluginInfo->SetBool(PARAM_BOOL_ALWAYSCOMPRESS, false);
    pkPluginInfo->SetValue(PARAM_STRING_EXCLUDEDCONTROLLERTYPES,
        NiControllerExtractorDialog::ms_pcPSResetControllerType);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiControllerExtractorPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiControllerExtractorPlugin::DoManagementDialog(
    NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    // Convert old version script, if necessary.
    bool bConvertedOldScript = false;
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInitialInfo->GetVersion());
    if (CompareVersions(uiScriptVersion, m_strVersion))
    {
        if (CompareVersions(uiScriptVersion, "1.1"))
        {
            pkInitialInfo->SetBool(PARAM_BOOL_CHARACTERNAMEINKFMFILE, true);
        }
        if (CompareVersions(uiScriptVersion, "1.2.1"))
        {
            pkInitialInfo->SetInt(PARAM_INT_SYNCTRANSTYPE,
                NiKFMTool::TYPE_MORPH);
            pkInitialInfo->SetInt(PARAM_INT_NONSYNCTRANSTYPE,
                NiKFMTool::TYPE_BLEND);

            float fTransDuration = pkInitialInfo->GetFloat(
                PARAM_FLOAT_TRANSDURATION);
            pkInitialInfo->SetFloat(PARAM_FLOAT_SYNCTRANSDURATION,
                fTransDuration);
            pkInitialInfo->SetFloat(PARAM_FLOAT_NONSYNCTRANSDURATION,
                fTransDuration);
            pkInitialInfo->RemoveParameter(PARAM_FLOAT_TRANSDURATION);
        }
        if (CompareVersions(uiScriptVersion, "1.2.2"))
        {
            pkInitialInfo->SetBool(PARAM_BOOL_ALWAYSCOMPRESS, false);
        }
        if (CompareVersions(uiScriptVersion, "2.3"))
        {
            pkInitialInfo->SetValue(PARAM_STRING_EXCLUDEDCONTROLLERTYPES, "");
        }

        pkInitialInfo->SetVersion(m_strVersion);
        bConvertedOldScript = true;
    }

    NiControllerExtractorDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch (kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        break;
    default:
        NIASSERT(false);
    }

    return bConvertedOldScript;
}
//---------------------------------------------------------------------------
bool NiControllerExtractorPlugin::CanExport(NiExportPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == PARAM_PLUGINNAME &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiControllerExtractorPlugin::GetGlobalOptionsForCharRoot(
    const NiControllerExtractor::NiCCAGlobalInfoArray& kArray,
    NiAVObject* pkCharRoot, bool& bCompress, bool& bCompactCPs,
    float& fKeyPercentage)
{
    for (unsigned int ui = 0; ui < kArray.GetSize(); ui++)
    {
        NiControllerExtractor::NiCCAGlobalInfo* pkInfo =
            kArray.GetAt(ui);
        if (pkInfo && pkInfo->m_spCharacterRoot == pkCharRoot)
        {
            bCompress = pkInfo->m_bUseBSplineCompression;
            fKeyPercentage = pkInfo->m_fBSplineCompressionRatio;
            bCompactCPs = pkInfo->m_bUseCompactControlPoints;

            NiOutputDebugString("Global overrides found for "
                "character root\n");
            return;
        }
    }
}
//---------------------------------------------------------------------------
bool NiControllerExtractorPlugin::GetOverrideOptionsForSequence(
    const NiControllerExtractor::NiCCAInfoArray& kArray,
    const NiSequenceData*, 
    const NiControllerExtractor::SequenceInfo* pkSeqInfo, 
    bool& bCompress, bool& bCompactCPs, float& fKeyPercentage)
{
    for (unsigned int ui = 0; ui < kArray.GetSize(); ui++)
    {
        NiControllerExtractor::NiCCAInfo* pkInfo =
            kArray.GetAt(ui);
        if (pkInfo &&  pkInfo->m_pcSequenceName &&
            pkInfo->m_spCharacterRoot == pkSeqInfo->m_spCharacterRoot &&
            strcmp(pkInfo->m_pcSequenceName, pkSeqInfo->m_pcName) == 0 &&
            pkInfo->m_spObject != NULL)
        {
            bCompress = pkInfo->m_bUseBSplineCompression;
            fKeyPercentage = pkInfo->m_fBSplineCompressionRatio;
            bCompactCPs = pkInfo->m_bUseCompactControlPoints;
            NiOutputDebugString("Overrides found for "
                "sequence: \"");
            NiOutputDebugString(pkInfo->m_pcSequenceName);
            NiOutputDebugString("\"\n");
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
void NiControllerExtractorPlugin::TransferOverrideOptionsToNonAccum(
    NiControllerExtractor::NiCCAInfoArray& kArray, 
    NiAVObject* pkAccumRoot)
{
    if (!pkAccumRoot || !NiIsKindOf(NiNode, pkAccumRoot))
        return;

    NiNode* pkAccumNode = (NiNode*) pkAccumRoot;
    if (pkAccumNode->GetChildCount() < 1)
        return;

    NiAVObject* pkNonAccumNode = NULL;

    // Non-Accum node may not be the 0th item in the child array.
    for (unsigned int uj = 0; uj < pkAccumNode->GetArrayCount(); uj++)
    {
        pkNonAccumNode = pkAccumNode->GetAt(uj);
        if (pkNonAccumNode != NULL)
            break;
    }

    // Iterate over all perobject and perobjectpersequence entries
    // and create duplicates for the nonaccum
    for (unsigned int ui = 0; ui < kArray.GetSize(); ui++)
    {
        NiControllerExtractor::NiCCAInfo* pkInfo =
            kArray.GetAt(ui);

        if (pkInfo && pkInfo->m_spObject == pkAccumRoot)
        {
            NiControllerExtractor::NiCCAInfo* pkNewInfo = NiNew 
                NiControllerExtractor::NiCCAInfo();

            pkNewInfo->SetSequenceName(pkInfo->m_pcSequenceName);
            pkNewInfo->m_spCharacterRoot = pkInfo->m_spCharacterRoot;
            pkNewInfo->m_spObject = pkNonAccumNode;
            pkNewInfo->m_bUseBSplineCompression = 
                pkInfo->m_bUseBSplineCompression;
            pkNewInfo->m_fBSplineCompressionRatio =
                pkInfo->m_fBSplineCompressionRatio;
            pkNewInfo->m_bUseCompactControlPoints = 
                pkInfo->m_bUseCompactControlPoints;
          
            kArray.Add(pkNewInfo);
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractorPlugin::SetObjectOverrideOptionsForSequence(
    const NiControllerExtractor::NiCCAInfoArray& kArray,
    const NiSequenceData*, 
    const NiControllerExtractor::SequenceInfo* pkSeqInfo)
{
    for (unsigned int ui = 0; ui < kArray.GetSize(); ui++)
    {
        NiControllerExtractor::NiCCAInfo* pkInfo =
            kArray.GetAt(ui);

        // Determine the override options per object.
        // As necessary, allow the per object per sequence options
        // to overwrite the per object options. Note that this code
        // assumes that the per-object options occur first.
        if (pkInfo &&  pkInfo->m_spObject != NULL && 
            pkInfo->m_spCharacterRoot == pkSeqInfo->m_spCharacterRoot &&
            (pkInfo->m_pcSequenceName == NULL || 
             strcmp(pkInfo->m_pcSequenceName, pkSeqInfo->m_pcName) == 0))
        {
            NiBooleanExtraDataPtr spCompressED = NiDynamicCast(
                NiBooleanExtraData,
                pkInfo->m_spObject->GetExtraData(NI_COMPRESS_KEY));
            if (!spCompressED)
            {
                spCompressED = NiNew NiBooleanExtraData(
                    pkInfo->m_bUseBSplineCompression);
                pkInfo->m_spObject->AddExtraData(NI_COMPRESS_KEY, 
                    spCompressED);
            }
            else
            {
                spCompressED->SetValue(pkInfo->m_bUseBSplineCompression);
            }

            NiFloatExtraDataPtr spRatioED =  NiDynamicCast(
                NiFloatExtraData,
                pkInfo->m_spObject->GetExtraData(NI_COMPRESSION_RATIO_KEY));
            if (!spRatioED)
            {
                spRatioED = NiNew NiFloatExtraData(
                    pkInfo->m_fBSplineCompressionRatio);
                pkInfo->m_spObject->AddExtraData(NI_COMPRESSION_RATIO_KEY,
                    spRatioED);
            }
            else
            {
                spRatioED->SetValue(pkInfo->m_fBSplineCompressionRatio);
            }
            
            NiBooleanExtraDataPtr spCompactED = NiDynamicCast(
                NiBooleanExtraData, pkInfo->m_spObject->GetExtraData(
                NI_COMPACT_CONTROL_POINTS_KEY));
            if (!spCompactED)
            {
                spCompactED = NiNew NiBooleanExtraData(
                    pkInfo->m_bUseCompactControlPoints);
                pkInfo->m_spObject->AddExtraData(
                    NI_COMPACT_CONTROL_POINTS_KEY, spCompactED);
            }
            else
            {
                spCompactED->SetValue(pkInfo->m_bUseCompactControlPoints);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiControllerExtractorPlugin::RemoveObjectOverrideOptionsForSequence(
    const NiControllerExtractor::NiCCAInfoArray& kArray,
    const NiSequenceData*, 
    const NiControllerExtractor::SequenceInfo* pkSeqInfo)
{
    for (unsigned int ui = 0; ui < kArray.GetSize(); ui++)
    {
        NiControllerExtractor::NiCCAInfo* pkInfo =
            kArray.GetAt(ui);

        // Remove the override options per object.
        // As necessary, remove the per object per sequence options.
        if (pkInfo &&  pkInfo->m_spObject != NULL && 
            pkInfo->m_spCharacterRoot == pkSeqInfo->m_spCharacterRoot &&
            (pkInfo->m_pcSequenceName == NULL || 
             strcmp(pkInfo->m_pcSequenceName, pkSeqInfo->m_pcName) == 0))
        {
            pkInfo->m_spObject->RemoveExtraData(NI_COMPRESS_KEY);
            pkInfo->m_spObject->RemoveExtraData(NI_COMPRESSION_RATIO_KEY);
            pkInfo->m_spObject->RemoveExtraData(
                NI_COMPACT_CONTROL_POINTS_KEY);
        }
    }
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiControllerExtractorPlugin::Export(
    const NiExportPluginInfo* pkInfo)
{
    // Get script version.
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInfo->GetVersion());

    // Get the ControllerExtractor shared data.
    NiSharedDataList* pkList = NiSharedDataList::GetInstance();
    NIASSERT(pkList);
    NiControllerExtractorSharedData* pkSharedData =
        (NiControllerExtractorSharedData*) pkList->Get(NiGetSharedDataType(
        NiControllerExtractorSharedData));
    if (!pkSharedData)
    {
        NiSceneGraphSharedData* pkSG = (NiSceneGraphSharedData*) pkList->Get(
            NiGetSharedDataType(NiSceneGraphSharedData));
        if (!pkSG)
        {
            return NiNew NiExecutionResult(
                NiExecutionResult::EXECUTE_FAILURE);
        }

        pkSharedData = NiNew NiControllerExtractorSharedData;
        NiControllerExtractor::ProcessTextKeys(pkSG->GetFullSceneGraph(),
            pkSharedData->GetInfoArray(),
            pkSharedData->GetCCAGlobalInfoArray(),
            pkSharedData->GetCCAOverrideInfoArray());
        pkList->Insert(pkSharedData);
    }

    // Set the endianness of the exported kf/kfms.
    // Priority 1 - Exporter Options (art exporter), via
    //     NiSystemDesc::GetToolModeRendererIsLittleEndian
    // Priority 2 - Scene Graph Options (importer)
    // Fallback - use default (exporter platform) endianness
    NiExporterOptionsSharedData* pkEOSharedData = NULL;
    pkEOSharedData = (NiExporterOptionsSharedData*)pkList->Get(
        NiGetSharedDataType(NiExporterOptionsSharedData));
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    bool bTargetLittle;
    if (pkEOSharedData)
    {
        bTargetLittle =
            NiSystemDesc::GetSystemDesc().GetToolModeRendererIsLittleEndian();
    }
    else if (pkSGSharedData->GetPreserveEndianness())
    {
        bTargetLittle = pkSGSharedData->GetSourceLittleEndian();
    }
    else
    {
        bTargetLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    }

    m_pkProgressBar = NiNew NiProgressDialog("Controller Extractor");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetRangeSpan(5);
    m_pkProgressBar->SetPosition(0);
    m_pkProgressBar->SetLineOne("Building Sequences...");
    m_pkProgressBar->StepIt();

    bool bAlwaysCompress = false;

    if (!CompareVersions(uiScriptVersion, "1.2.2"))
    {
        bAlwaysCompress = pkInfo->GetBool(PARAM_BOOL_ALWAYSCOMPRESS);
    }

    NiString strExcludedControllerTypes;
    const char* pcExcludedControllerTypes = NULL;
    if (!CompareVersions(uiScriptVersion, "2.3"))
    {
        strExcludedControllerTypes = pkInfo->GetValue(
            PARAM_STRING_EXCLUDEDCONTROLLERTYPES);
        if (!strExcludedControllerTypes.IsEmpty())
        {
            pcExcludedControllerTypes = strExcludedControllerTypes;
        }
    }
    else
    {
        pcExcludedControllerTypes =
            NiControllerExtractorDialog::ms_pcPSResetControllerType;
    }

    NiControllerExtractor::SequenceDataArray kSequenceDataArray;
    NiControllerExtractor::BuildSequenceDatas(pkSharedData->GetInfoArray(),
        kSequenceDataArray, true, true, pcExcludedControllerTypes);

    unsigned int uiNumSequenceDatas = kSequenceDataArray.GetSize();
    if (uiNumSequenceDatas == 0)
    {
        bool bWriteResultsToLog = false; 
        NiXMLLogger* pkLogger = NULL;

        if (pkEOSharedData)
        {
            bWriteResultsToLog = pkEOSharedData->GetWriteResultsToLog();
            pkLogger = pkEOSharedData->GetXMLLogger();
        }

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("ControllerExtractorError", "There are "
                "no animation sequences defined in the scene. In order\n"
                "to successfully extract KF or KFM files, you must define at "
                "least\none animation sequence. No KF or KFM files will be "
                "created until\nyou do so.");
        }
        else
        {
            MessageBox(NULL, "There are no animation sequences defined in "
                "the scene. In order\nto successfully extract KF or KFM "
                "files, you must define at least\none animation sequence. "
                "No KF or KFM files will be created until\nyou do so.", 
                "Controller Extractor Error: No Sequences Defined", 
                MB_OK | MB_ICONERROR);
        }
        m_pkProgressBar->Destroy();
        NiDelete m_pkProgressBar;
        m_pkProgressBar = NULL;
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    m_pkProgressBar->SetLineOne("Compressing Sequences...");
    m_pkProgressBar->StepIt();

    NiControllerExtractor::NiCCAGlobalInfoArray& kGlobalArray = 
        pkSharedData->GetCCAGlobalInfoArray();
    NiControllerExtractor::NiCCAInfoArray& kOverrideArray = 
        pkSharedData->GetCCAOverrideInfoArray();

    // We need to transfer any override data from the accumulation root
    // to the nonaccum child node.    
    TransferOverrideOptionsToNonAccum(kOverrideArray, 
         pkSharedData->GetInfoArray().GetAt(0)->m_spAccumRoot);

    NiTPrimitiveSet<unsigned int> kFailedSequences;
    for (unsigned int ui = 0; ui < pkSharedData->GetInfoArray().GetSize();
        ui++)
    {
        NiControllerExtractor::SequenceInfo* pkSeqInfo = pkSharedData
            ->GetInfoArray().GetAt(ui);

        if (pkSeqInfo)
        {
            NiSequenceData* pkSeqData = kSequenceDataArray.GetAt(ui);
            if (!pkSeqData)
            {
                kFailedSequences.Add(ui);
                continue;
            }            
            NIASSERT(strcmp(pkSeqData->GetName(), pkSeqInfo->m_pcName) == 0);

            bool bCompress = false;
            bool bCompactCPs = false;
            float fKeyPercentage = 1.0f;

            bool bCompressOverride = false;
            
            // Determine the global options for this actor
            GetGlobalOptionsForCharRoot(kGlobalArray,
                pkSeqInfo->m_spCharacterRoot, bCompress, 
                bCompactCPs, fKeyPercentage);

            // Determine the options for this sequence
            bCompressOverride = GetOverrideOptionsForSequence(kOverrideArray,
                pkSeqData, pkSeqInfo, bCompress, 
                bCompactCPs, fKeyPercentage);

            if (bCompress)
            {
                if (bCompressOverride)
                {
                    // Tag each object with the necessary 
                    // overrides for this sequence
                    SetObjectOverrideOptionsForSequence(
                        kOverrideArray, pkSeqData, pkSeqInfo);
                }

                NiAnimationCompression::CompressEvaluators(
                    pkSeqData, pkSeqInfo->GetSequenceRoot(),
                    fKeyPercentage, bAlwaysCompress,
                    bCompactCPs);

                if (bCompressOverride)
                {
                    // Remove override from objects 
                    RemoveObjectOverrideOptionsForSequence(
                        kOverrideArray, pkSeqData, pkSeqInfo);
                }

            }
        }
    }

    if (kFailedSequences.GetSize() > 0)
    {
        const unsigned int uiBuffSize = 1024;
        char acErrorMsg[uiBuffSize];
        NiStrcpy(acErrorMsg, uiBuffSize, 
            "The following sequences were not created because they do not \n"
            "contain any animation keys:\n\n");
        for (unsigned int ui = 0; ui < kFailedSequences.GetSize(); ui++)
        {
            NiControllerExtractor::SequenceInfo* pkSeqInfo =
                pkSharedData->GetInfoArray().GetAt(kFailedSequences.GetAt(
                ui));
            NiStrcat(acErrorMsg, uiBuffSize, pkSeqInfo->m_pcName);
            NiStrcat(acErrorMsg, uiBuffSize, "\n");
        }

        bool bWriteResultsToLog = false; 
        NiXMLLogger* pkLogger = NULL;

        if (pkEOSharedData)
        {
            bWriteResultsToLog = pkEOSharedData->GetWriteResultsToLog();
            pkLogger = pkEOSharedData->GetXMLLogger();
        }

        if (bWriteResultsToLog && pkLogger)
        {
            pkLogger->LogElement("ControllerExtractorError", acErrorMsg);
        }
        else
        {
            MessageBox(NULL, acErrorMsg, "Controller Extractor Error",
                MB_OK | MB_ICONERROR);
        }
    }

    m_pkProgressBar->SetLineOne("Saving KF File(s)...");
    m_pkProgressBar->StepIt();

    NiString* pstrFilenames = NiNew NiString[uiNumSequenceDatas];
    unsigned int* puiFileIndices = NiAlloc(unsigned int, uiNumSequenceDatas);

    NiString strKFFilePath = pkInfo->GetDirectory();
    if (pkInfo->GetBool(PARAM_BOOL_SINGLEKF))
    {
        NiString strKFFileName = pkInfo->GetFile();
        if (pkInfo->GetBool(PARAM_BOOL_UPPERCASE))
        {
            strKFFileName += ".KF";
        }
        else
        {
            strKFFileName += ".kf";
        }

        NiStream kKFStream;
        kKFStream.SetSaveAsLittleEndian(bTargetLittle);
        unsigned int uiStreamIndex = 0;
        for (unsigned int ui = 0; ui < uiNumSequenceDatas; ui++)
        {
            NiSequenceData* pkSeqData = kSequenceDataArray.GetAt(ui);
            if (pkSeqData)
            {
                kKFStream.InsertObject(pkSeqData);
                pstrFilenames[ui] = strKFFileName;
                puiFileIndices[ui] = uiStreamIndex++;
            }
        }
        if (kKFStream.GetObjectCount() > 0)
        {
            // Check for existing read-only file.
            NiString strTempFullKFPath = strKFFilePath + strKFFileName;
            if (!NiFile::Access(strTempFullKFPath, NiFile::WRITE_ONLY))
            {
                NiString strMessage;
                strMessage.Format("%s is read-only. The file will not be "
                    "overwritten.", strTempFullKFPath);
                bool bWriteResultsToLog = false; 
                NiXMLLogger* pkLogger = NULL;

                if (pkEOSharedData)
                {
                    bWriteResultsToLog = 
                        pkEOSharedData->GetWriteResultsToLog();
                    pkLogger = pkEOSharedData->GetXMLLogger();
                }

                if ((bWriteResultsToLog) && (pkLogger))
                {
                    pkLogger->LogElement("ControllerExtractorError", 
                        strMessage);
                }
                else
                {
                    MessageBox(NULL, strMessage, "Read-Only File",
                        MB_ICONERROR | MB_OK);
                }

                NiDelete [] pstrFilenames;
                NiFree(puiFileIndices);
                m_pkProgressBar->Destroy();
                NiDelete m_pkProgressBar;
                m_pkProgressBar = NULL;
                return NiNew NiExecutionResult(NiExecutionResult::
                    EXECUTE_FAILURE);
            }

            // Save KF file.
            if (!kKFStream.Save(strTempFullKFPath))
            {
                NiDelete [] pstrFilenames;
                NiFree(puiFileIndices);
                m_pkProgressBar->Destroy();
                NiDelete m_pkProgressBar;
                m_pkProgressBar = NULL;
                return NiNew NiExecutionResult(NiExecutionResult::
                    EXECUTE_FAILURE);
            }
        }
    }
    else
    {
        const NiString strSeparator = pkInfo->GetValue(
            PARAM_STRING_SEPARATOR);

        NiString strBaseKFFileName;
        if (pkInfo->GetBool(PARAM_BOOL_EXPORTEDFILENAME))
        {
            strBaseKFFileName = pkInfo->GetFile() + strSeparator;
        }

        for (unsigned int ui = 0; ui < uiNumSequenceDatas; ui++)
        {
            NiSequenceData* pkSeqData = kSequenceDataArray.GetAt(ui);
            NiControllerExtractor::SequenceInfo* pkSeqInfo = pkSharedData
                ->GetInfoArray().GetAt(ui);
            if (pkSeqData)
            {
                NiString strCurKFFileName = strBaseKFFileName;

                if (pkInfo->GetBool(PARAM_BOOL_CHARACTERNAME))
                {
                    strCurKFFileName += pkSeqInfo->m_spCharacterRoot
                        ->GetName() + strSeparator;
                }

                if (pkSeqInfo->m_spLayerRoot)
                {
                    strCurKFFileName += pkSeqInfo->m_spLayerRoot->GetName() +
                        strSeparator;
                }

                strCurKFFileName = strCurKFFileName + pkSeqData->GetName();

                if (pkInfo->GetBool(PARAM_BOOL_UPPERCASE))
                {
                    strCurKFFileName += ".KF";
                }
                else
                {
                    strCurKFFileName += ".kf";
                }

                // Ensure that filename does not contain invalid characters.
                char* pcCurKFFileName = strCurKFFileName.MakeExternalCopy();
                NiPath::ReplaceInvalidFilenameCharacters(pcCurKFFileName,
                    '_');
                strCurKFFileName = pcCurKFFileName;
                NiFree(pcCurKFFileName);

                NiStream kKFStream;
                kKFStream.SetSaveAsLittleEndian(bTargetLittle);
                kKFStream.InsertObject(pkSeqData);

                // Check for existing read-only file.
                NiString strTempFullKFPath = strKFFilePath + strCurKFFileName;
                if (!NiFile::Access(strTempFullKFPath, NiFile::WRITE_ONLY))
                {
                    NiString strMessage;
                    strMessage.Format("%s is read-only. The file will not be "
                        "overwritten.", strTempFullKFPath);
                    bool bWriteResultsToLog = false; 
                    NiXMLLogger* pkLogger = NULL;

                    if (pkEOSharedData)
                    {
                        bWriteResultsToLog = 
                            pkEOSharedData->GetWriteResultsToLog();
                        pkLogger = pkEOSharedData->GetXMLLogger();
                    }

                    if ((bWriteResultsToLog) && (pkLogger))
                    {
                        pkLogger->LogElement("ControllerExtractorError", 
                            strMessage);
                    }
                    else
                    {
                        MessageBox(NULL, strMessage, "Read-Only File",
                            MB_ICONERROR | MB_OK);
                    }

                    NiDelete [] pstrFilenames;
                    NiFree(puiFileIndices);
                    m_pkProgressBar->Destroy();
                    NiDelete m_pkProgressBar;
                    m_pkProgressBar = NULL;
                    return NiNew NiExecutionResult(NiExecutionResult::
                        EXECUTE_FAILURE);
                }

                // Save KF file.
                if (!kKFStream.Save(strKFFilePath + strCurKFFileName))
                {
                    NiDelete [] pstrFilenames;
                    NiFree(puiFileIndices);
                    m_pkProgressBar->Destroy();
                    NiDelete m_pkProgressBar;
                    m_pkProgressBar = NULL;
                    return NiNew NiExecutionResult(NiExecutionResult::
                        EXECUTE_FAILURE);
                }
                
                pstrFilenames[ui] = strCurKFFileName;
                puiFileIndices[ui] = 0;
            }
        }
    }

    // Create a KFM file.
    if (pkInfo->GetBool(PARAM_BOOL_CREATEKFM))
    {
        m_pkProgressBar->SetLineOne("Creating KFM File...");
        m_pkProgressBar->StepIt();

        // KFM files only support a single character. Determine the character
        // for which the KFM file should be created.
        NiTPrimitiveSet<NiAVObject*> kActorRoots;
        unsigned int ui = 0;
        for (; ui < pkSharedData->GetInfoArray().GetSize(); ui++)
        {
            NiControllerExtractor::SequenceInfo* pkSeqInfo = pkSharedData
                ->GetInfoArray().GetAt(ui);
            if (pkSeqInfo)
            {
                bool bFound = false;
                for (unsigned int uj = 0; uj < kActorRoots.GetSize(); uj++)
                {
                    if (pkSeqInfo->m_spCharacterRoot == kActorRoots.GetAt(uj))
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    kActorRoots.Add(pkSeqInfo->m_spCharacterRoot);
                }
            }
        }
        NIASSERT(kActorRoots.GetSize() != 0);
        NiAVObject* pkCurActorRoot = kActorRoots.GetAt(0);
        bool bMultipleActorRoots = false;
        if (kActorRoots.GetSize() > 1)
        {
            bMultipleActorRoots = true;

            // Display Choose Actor Root dialog box.
            ChooseActorRootDialog kDialog(m_hModule, NULL);
            kDialog.m_pkActorRoots = &kActorRoots;
#ifdef NIDEBUG    
            int iResult = 
#endif
                kDialog.DoModal();
            NIASSERT(iResult == IDOK);
            pkCurActorRoot = kDialog.m_pkSelectedActorRoot;            
        }

        // Build KFM file name string.
        NiString strKFMFileName = pkInfo->GetFile();
        bool bUseCharacterNameInKFMFile;
        if (CompareVersions(uiScriptVersion, "1.1"))
        {
            bUseCharacterNameInKFMFile = true;
        }
        else
        {
            bUseCharacterNameInKFMFile = pkInfo->GetBool(
                PARAM_BOOL_CHARACTERNAMEINKFMFILE);
        }
        if (bMultipleActorRoots && bUseCharacterNameInKFMFile)
        {
            const NiString strSeparator = pkInfo->GetValue(
                PARAM_STRING_SEPARATOR);
            strKFMFileName += strSeparator + pkCurActorRoot->GetName();
        }
        NiString strBaseKFMFilename = strKFMFileName;
        if (pkInfo->GetBool(PARAM_BOOL_UPPERCASE))
        {
            strKFMFileName += ".KFM";
        }
        else
        {
            strKFMFileName += ".kfm";
        }

        // Ensure that filename does not contain invalid characters.
        char* pcKFMFileName = strKFMFileName.MakeExternalCopy();
        NiPath::ReplaceInvalidFilenameCharacters(pcKFMFileName, '_');
        strKFMFileName = pcKFMFileName;
        NiFree(pcKFMFileName);

        // Build full KFM file path.
        strKFMFileName = pkInfo->GetDirectory() + strKFMFileName;

        // Check for presence of KFM file (to detect overwrite).
        NiFileFinder kFileFinder(pkInfo->GetDirectory(), false, "kfm");
        while (kFileFinder.HasMoreFiles())
        {
            NiFoundFile* pkFoundFile = kFileFinder.GetNextFile();
            if (pkFoundFile->m_strPath.EqualsNoCase(strKFMFileName))
            {
                bool bWriteResultsToLog = false; 
                NiXMLLogger* pkLogger = NULL;

                if (pkEOSharedData)
                {
                    bWriteResultsToLog =
                        pkEOSharedData->GetWriteResultsToLog();
                    pkLogger = pkEOSharedData->GetXMLLogger();
                }

                int iResult;
                if ((bWriteResultsToLog) && (pkLogger))
                {
                    iResult = IDNO;
                }
                else
                {
                    iResult = MessageBox(NULL, "A KFM file with the same "
                        "name already exists in the\nlocation you specified. "
                        "Do you want to overwrite it?", "Controller "
                        "Extractor: Overwrite KFM File?", 
                        MB_YESNO | MB_ICONQUESTION);
                }
                if (iResult == IDNO)
                {
                    NiDelete [] pstrFilenames;
                    NiFree(puiFileIndices);
                    m_pkProgressBar->Destroy();
                    NiDelete m_pkProgressBar;
                    m_pkProgressBar = NULL;
                    return NiNew NiExecutionResult(
                        NiExecutionResult::EXECUTE_SUCCESS);
                }
                else
                {
                    // Check for existing read-only files.
                    if (!NiFile::Access(strKFMFileName, NiFile::WRITE_ONLY) ||
                        !NiFile::Access(strBaseKFMFilename + ".set", 
                            NiFile::WRITE_ONLY) ||
                        !NiFile::Access(strBaseKFMFilename + ".map", 
                            NiFile::WRITE_ONLY) ||
                        !NiFile::Access(strBaseKFMFilename + ".settings", 
                            NiFile::WRITE_ONLY)||
                        !NiFile::Access(strBaseKFMFilename + "_Anim.h", 
                            NiFile::WRITE_ONLY))
                    {
                        NiString strMessage;
                        strMessage.Format("%s or one of its associated files "
                            "is read-only.\nThe files will not be "
                            "overwritten.", strKFMFileName);
                        MessageBox(NULL, strMessage, "Read-Only Files",
                            MB_ICONERROR | MB_OK);

                        NiDelete [] pstrFilenames;
                        NiFree(puiFileIndices);
                        m_pkProgressBar->Destroy();
                        NiDelete m_pkProgressBar;
                        m_pkProgressBar = NULL;
                        return NiNew NiExecutionResult(
                            NiExecutionResult::EXECUTE_FAILURE);
                    }

                    // Delete KFM, SET, MAP, SETTINGS, and H files.
                    remove(strKFMFileName);
                    remove(strBaseKFMFilename + ".set");
                    remove(strBaseKFMFilename + ".map");
                    remove(strBaseKFMFilename + ".settings");
                    remove(strBaseKFMFilename + "_Anim.h");
                }
            }
        }

        NiKFMTool::KFM_RC eRC, eSuccess = NiKFMTool::KFM_SUCCESS;

        // Create the KFM tool.
        NiKFMToolPtr spKFMTool = NiNew NiKFMTool((const char*)
            pkInfo->GetDirectory());

        // Add the NIF file path.
        NiString strNIFFileName = pkInfo->GetFile();
        if (pkInfo->GetBool(PARAM_BOOL_UPPERCASE))
        {
            strNIFFileName += ".NIF";
        }
        else
        {
            strNIFFileName += ".nif";
        }
        spKFMTool->SetModelPath((const char*)strNIFFileName);

        // Add the character root name.
        spKFMTool->SetModelRoot(pkCurActorRoot->GetName());

        // Add an animation for each sequence.
        NiTPrimitiveSet<NiSequenceData*> kKFMSequenceDatas;
        unsigned int uiEventCode = 0;
        for (ui = 0; ui < uiNumSequenceDatas; ui++)
        {
            NiSequenceData* pkSeqData = kSequenceDataArray.GetAt(ui);
            NiControllerExtractor::SequenceInfo* pkSeqInfo = pkSharedData
                ->GetInfoArray().GetAt(ui);
            if (pkSeqData && pkSeqInfo &&
                pkSeqInfo->m_spCharacterRoot == pkCurActorRoot)
            {
                eRC = spKFMTool->AddSequence(uiEventCode++,
                    (const char*)pstrFilenames[ui], pkSeqData->GetName());
                NIASSERT(eRC == eSuccess);
                kKFMSequenceDatas.Add(pkSeqData);
            }
        }

        // Set default transition type.
        NiKFMTool::TransitionType eSyncTransType, eNonSyncTransType;
        if (CompareVersions(uiScriptVersion, "1.2.1"))
        {
            eSyncTransType = NiKFMTool::TYPE_MORPH;
            eNonSyncTransType = NiKFMTool::TYPE_BLEND;
        }
        else
        {
            eSyncTransType = (NiKFMTool::TransitionType) pkInfo->GetInt(
                PARAM_INT_SYNCTRANSTYPE);
            eNonSyncTransType = (NiKFMTool::TransitionType) pkInfo->GetInt(
                PARAM_INT_NONSYNCTRANSTYPE);
        }
        eRC = spKFMTool->SetDefaultSyncTransType(eSyncTransType);
        NIASSERT(eRC == eSuccess);
        eRC = spKFMTool->SetDefaultNonSyncTransType(eNonSyncTransType);
        NIASSERT(eRC == eSuccess);

        // Set default transition duration.
        float fSyncTransDuration, fNonSyncTransDuration;
        if (CompareVersions(uiScriptVersion, "1.2.1"))
        {
            fSyncTransDuration = fNonSyncTransDuration =
                pkInfo->GetFloat(PARAM_FLOAT_TRANSDURATION);
        }
        else
        {
            fSyncTransDuration = pkInfo->GetFloat(
                PARAM_FLOAT_SYNCTRANSDURATION);
            fNonSyncTransDuration = pkInfo->GetFloat(
                PARAM_FLOAT_NONSYNCTRANSDURATION);
        }
        spKFMTool->SetDefaultSyncTransDuration(fSyncTransDuration);
        spKFMTool->SetDefaultNonSyncTransDuration(fNonSyncTransDuration);

        // Add default transitions for all animations.
        for (unsigned int uiSrc = 0; uiSrc < uiEventCode; uiSrc++)
        {
            NiSequenceData* pkSrcSeqData = kKFMSequenceDatas.GetAt(uiSrc);

            // Add default transition to every other event code.
            for (unsigned int uiDes = 0; uiDes < uiEventCode; uiDes++)
            {
                if (uiDes == uiSrc)
                {
                    continue;
                }

                NiSequenceData* pkDesSeqData = kKFMSequenceDatas.GetAt(
                    uiDes);

                NiKFMTool::TransitionType eTransType;
                if (pkSrcSeqData->CanSyncTo(pkDesSeqData))
                {
                    eTransType = NiKFMTool::TYPE_DEFAULT_SYNC;
                }
                else
                {
                    eTransType = NiKFMTool::TYPE_DEFAULT_NONSYNC;
                }

                // Create transitions.
                eRC = spKFMTool->AddTransition(uiSrc, uiDes, eTransType,
                    NiKFMTool::MAX_DURATION);
                NIASSERT(eRC == eSuccess);
                eRC = spKFMTool->AddTransition(uiDes, uiSrc, eTransType,
                    NiKFMTool::MAX_DURATION);
                NIASSERT(eRC == eSuccess);
            }
        }

        // Save KFM file.
        eRC = spKFMTool->SaveFile(strKFMFileName, true, bTargetLittle);
        if (eRC != eSuccess)
        {
            NiDelete [] pstrFilenames;
            NiFree(puiFileIndices);
            m_pkProgressBar->Destroy();
            NiDelete m_pkProgressBar;
            m_pkProgressBar = NULL;
            return NiNew NiExecutionResult(
                NiExecutionResult::EXECUTE_FAILURE);
        }
    }

    m_pkProgressBar->StepIt();

    NiDelete [] pstrFilenames;
    NiFree(puiFileIndices);
    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
const char* NiControllerExtractorPlugin::GetFileDescriptorString()
{
    return "Gamebryo KF File (*.kf)";
}
//---------------------------------------------------------------------------
const char* NiControllerExtractorPlugin::GetFileTypeString()
{
    return "*.kf";
}
//---------------------------------------------------------------------------
