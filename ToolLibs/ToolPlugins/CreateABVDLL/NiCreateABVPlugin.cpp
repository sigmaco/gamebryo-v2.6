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

#include <StdAfx.h>
#include "NiCreateABVPlugin.h"
#include "NiCreateABVDialog.h"
#include "CreateABVResultsDialog.h"

NiImplementRTTI(NiCreateABVPlugin, NiPlugin);

//---------------------------------------------------------------------------
const char* FindSubString(const char* pcStr, const char* pcKey,
    bool bFindTermination = 0 )
{
    if (pcStr == NULL)
        return NULL;

    // looks for the first ocurrance of the given string key in the main 
    // string, returning a pointer to the start of that substring if found
    size_t stLength = strlen(pcKey);

    while(pcStr && pcStr[0])
    {
        if(!strncmp(pcStr, pcKey, stLength))
        {
            if (bFindTermination)
            {
                pcStr += stLength;

                while (*pcStr == ' ')
                    pcStr++;
            }

            return pcStr;
        }
        pcStr++;
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool ParseWireframeABVStr(char* pcStr,
    char* pcType,
    size_t pcTypeMaxLen,
    float* pafColor,
    float& fMult,
    char* pcName,
    size_t uiNameLength)
{
    char acTag[256] ="";
    char acUpperTag[256] = "";
    pcName[0] = '\0';

    const char* pcEndStr = FindSubString(pcStr,"\n");
    size_t stLen = strlen(pcStr);
    if (pcEndStr)
       stLen -= strlen(pcEndStr);

    if (stLen == 0 || stLen >= 256)   
        return false;

    NiStrncpy(acTag, 256, pcStr, stLen);
    NiStrncpy(acUpperTag, 256, pcStr, stLen);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _strupr_s(acUpperTag, 256);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    strupr( acUpperTag );
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    const char* pcMatch;
    pcMatch = FindSubString(acUpperTag,"NDL_WIREFRAME");
    pcMatch = FindSubString(pcMatch,"=",true);
    
    if (!pcMatch)
        return false;  
    
#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf_s(pcMatch, "%s %f %f %f %f", pcType, pcTypeMaxLen,
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf(pcMatch, "%s %f %f %f %f", pcType,
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        &pafColor[0], &pafColor[1], &pafColor[2], &fMult);
    pcMatch = FindSubString(acTag,"=",true);
    pcMatch = pcMatch + strlen(pcType);

    const char* pcNamePtr = FindSubString(pcMatch,"\"",true);
    
    if (pcNamePtr == NULL)
        return false; // opening not found.

    const char* pcEndPtr = FindSubString(pcNamePtr,"\""); // ending
    
    if (pcEndPtr == NULL)
        return false; // closing not found

    stLen = 0;
    
    if (pcNamePtr)
        stLen = strlen(pcNamePtr);
    if (pcEndPtr)
        stLen -= strlen(pcEndPtr);
    NiStrncpy(pcName, uiNameLength, pcNamePtr, stLen);

    return true;
}
//---------------------------------------------------------------------------
void CreateSelectionBox(NiNode* pkNode, NiColor kColor, float fMult,
    char *pcSelectName)
{
    NiNode* pkProxy = NiOptimizeABV::GetNodeByFirstToken(pkNode, "NDLCD");

    NIASSERT(pkProxy);

    if (pkProxy == NULL)
        return;

    NiOptimizeABV::CreateWireABVProxy(pkProxy, kColor, fMult, pcSelectName);

    NiNode* pkNodeA = pkProxy->GetParent();
    NiNode* pkNode3 = (NiNode*)pkNodeA->GetObjectByName(pcSelectName);
    pkNode3->SetAppCulled(true);
    pkNode3->SetSelectiveUpdate(false);
}
//---------------------------------------------------------------------------
void ExecuteWireframeABVInstruction(NiNodePtr spNode, char* pcType,
    float*  pafColor, float fMult, char* pcName, bool bABV, bool bABVProxy, 
    bool bSelection)
{
    if (bABV && strcmp(pcType,"ABV")==0)
    {
        NiOptimizeABV::CreateWireABV(spNode,
            NiColor(pafColor[0], pafColor[1], pafColor[2] ),
            fMult, pcName, false);
    }
    else if (bABVProxy && strcmp(pcType,"ABVPROXY")==0)
    {
       NiOptimizeABV::CreateWireABVProxy(spNode,
           NiColor(pafColor[0], pafColor[1], pafColor[2]),
           fMult, pcName);
    }
    else if (bSelection && strcmp(pcType,"ABVSELECTION")==0)
    {
        unsigned int uiChildCnt = spNode->GetArrayCount();

        unsigned int uiChild;
        for(uiChild=0;uiChild < uiChildCnt; uiChild++)
        {
            NiNode* pkSel = (NiNode*)spNode->GetAt(uiChild);
            if (pkSel)
            {
                CreateSelectionBox(pkSel,
                    NiColor(pafColor[0],pafColor[1],pafColor[2]), fMult,
                    (char*)pcName);
            }
        }
    }
    else
    {
        NIASSERT(0);
    }
}
//---------------------------------------------------------------------------
void CreateUserDefinedWireFrames(NiNodePtr spNode, bool bABV, bool bABVProxy,
    bool bSelection)
{
    NiUInt32 uiCnt = 0;
            
    // user define?
    NiObjectNET* pkObj = NiSmartPointerCast(NiObjectNET,spNode);

    for (uiCnt=0; uiCnt < pkObj->GetExtraDataSize(); uiCnt++)
    {
        NiExtraData* pkExtra = pkObj->GetExtraDataAt((unsigned short)uiCnt);

        if(NiIsKindOf(NiStringExtraData,pkExtra))
        {
            const char* pcExtra = ((NiStringExtraData*)pkExtra)->GetValue();
            char* pcNewExtra = NULL;
            if (!pcExtra)
                continue;

            size_t stNewExtraLength = strlen(pcExtra) + 1;
            pcNewExtra = NiAlloc(char, stNewExtraLength);
            NiStrcpy(pcNewExtra, stNewExtraLength, pcExtra);

            char* pcCurr = pcNewExtra;
            
            while(pcCurr)
            {
                char* pcNext = (char*)FindSubString(pcCurr,"\n");
                char acType[80] = "";
                char acName[256] = "";
                float afColor[3];
                float fMult = 0.0f;

                if (ParseWireframeABVStr(pcCurr, 
                    acType,
                    sizeof(acType),
                    afColor,
                    fMult,
                    acName,
                    sizeof(acName)))
                {
                    ExecuteWireframeABVInstruction(spNode, acType, afColor,
                        fMult, acName, bABV, bABVProxy, bSelection);

                    if (pcNext)
                    {
                        // delete the portion of the string data.
                        size_t stLenNext = strlen(pcNext);
                        NiStrcpy(pcCurr, stLenNext + 1, pcNext + 1);
                        pcNext = pcCurr + stLenNext;
                        pcNext = pcCurr;
                    }
                    else
                    {
                        pcCurr[0] = '\0';
                    }
                                   
                    pcCurr = pcNext;
                }
                else
                {
                    pcCurr = pcNext;
                    if (pcCurr)
                        pcCurr++;
                }
            }

            if (strcmp(pcExtra, pcNewExtra) != 0)
            {
                // some string data can be removed.
                pkObj->RemoveExtraDataAt((unsigned short)uiCnt);

                if (pcNewExtra && (strcmp(pcNewExtra,"") != 0) &&
                    strcmp(pcNewExtra,"\r\n") != 0)
                    pkObj->AddExtraData(NiNew NiStringExtraData(pcNewExtra));

                // We effect the count, so adjust.
                if (uiCnt)
                    uiCnt--;
            }
            NiFree(pcNewExtra);
        }
    }

    NiUInt32 uiNumChildren = spNode->GetArrayCount();
    for (uiCnt = 0; uiCnt < uiNumChildren; uiCnt++)
    {
        NiAVObject* pkObj = spNode->GetAt(uiCnt);

        if (pkObj && NiIsKindOf(NiNode, pkObj))
            CreateUserDefinedWireFrames((NiNode*)pkObj, bABV, bABVProxy,
            bSelection);
    }
}
//---------------------------------------------------------------------------
NiCreateABVPlugin::NiCreateABVPlugin() :
    NiPlugin("Create ABV", "1.0", 
    "Turn mesh into an alternate bounding volume.",
    "Turn mesh into an alternate bounding volume.")
{
}
//---------------------------------------------------------------------------
bool NiCreateABVPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiCreateABVPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiCreateABVPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);


    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        spScene->Update(0.0f);
        NiMesh::CompleteSceneModifiers(spScene);

        NIASSERT(spScene);
        if (spScene)
        {
            NiTPointerList<char*> pkCharErrors;

            if (pkInfo->GetValue("CreateABV") == "TRUE")
                NiOptimizeABV::CreateABVFromProxy(spScene, &pkCharErrors);

            bool bABV = (pkInfo->GetValue("CreateWireframeABV") == "TRUE");
            bool bABVProxy = (pkInfo->GetValue("CreateWireframeABVProxy")
                == "TRUE");
            bool bSelected = (pkInfo->GetValue("CreateWireframeSelection")
                == "TRUE");

            if (bABV || bABVProxy || bSelected)
                CreateUserDefinedWireFrames(spScene,bABV,bABVProxy,bSelected);

            if (pkInfo->GetValue("RemoveProxyGeometry") == "TRUE")
            {
                NiOptimizeABV::RemoveCollisionDataProxyMesh(spScene,
                    &pkCharErrors);
            }

            // Convert Char Errors to NiString Errors.
            while (!pkCharErrors.IsEmpty())
            {
                NiString* pkString = NiNew NiString(pkCharErrors.GetHead());
                m_pkErrors.AddTail(pkString);
                NiFree(pkCharErrors.RemoveHead());
            }

            // Show any errors which have occurred.
            ShowAllErrors();

        }
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiCreateABVPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiCreateABVPlugin");
    pkPluginInfo->SetType("PROCESS");
        
    pkPluginInfo->SetBool("CreateABV", true);
    pkPluginInfo->SetBool("CreateWireframeABVProxy", false);
    pkPluginInfo->SetBool("CreateWireframeABV", false);
    pkPluginInfo->SetBool("CreateWireframeSelection", false);
    pkPluginInfo->SetBool("RemoveProxyGeometry", true);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiCreateABVPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiCreateABVPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    NiCreateABVDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::ClearAllErrors()
{
    // remove all errors deleting the strings
    while (!m_pkErrors.IsEmpty())
    {
        NiDelete m_pkErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::ClearAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        NiDelete m_pkSectionErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::AppendAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        m_pkErrors.AddTail( m_pkSectionErrors.RemoveHead() );;
    }
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::ShowAllErrors()
{
    if (m_pkErrors.IsEmpty())
        return;

    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if (bWriteResultsToLog && pkLogger)
    {
        // Convert to a single string
        NiString kErrorText;
        NiTListIterator kIter = m_pkErrors.GetHeadPos();

        while (kIter != NULL)
        {
            // Append into the Text String
            kErrorText += *m_pkErrors.GetNext(kIter);
            kErrorText += "\r\n";
        }

        pkLogger->LogElement("CreateAVBPlugin", kErrorText);
    }
    else
    {
        CreateABVResultsDialog kDialog(m_hModule, NULL, &m_pkErrors);

        kDialog.DoModal();
    }

    ClearAllErrors();
    ClearAllSectionErrors();
}
//---------------------------------------------------------------------------
