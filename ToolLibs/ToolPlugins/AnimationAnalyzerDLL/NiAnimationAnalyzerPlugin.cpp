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
#include "NiAnimationAnalyzerPlugin.h"
#include <NiAnimation.h>
#include <NiXMLLogger.h>

NiImplementRTTI(NiAnimationAnalyzerPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiAnimationAnalyzerPlugin::NiAnimationAnalyzerPlugin() :
    NiPlugin("Animation Analyzer", "1.0",
    "Displays Animation partitioning statistics",
    "Searches the scene graph for Animationned objects, printing "
    "Animation partition information.")
{
    m_uiTotalNumKeys = 0;
    m_uiTotalNumControllers = 0;
    m_uiTotalSizeKeys = 0;
    m_uiTotalRotKeys= 0;
    m_uiTotalPosKeys = 0;
    m_uiTotalScaleKeys= 0;

}
//---------------------------------------------------------------------------
bool NiAnimationAnalyzerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiAnimationAnalyzerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
const char* 
NiAnimationAnalyzerPlugin::GetKeyTypeEnumString(NiAnimationKey::KeyType eType)
{
    switch (eType)
    {
        case NiAnimationKey::NOINTERP:
            return "NOINTERP";
        case NiAnimationKey::LINKEY:
            return "LINKEY";
        case NiAnimationKey::BEZKEY:
            return "BEZKEY";
        case NiAnimationKey::TCBKEY:
            return "TCBKEY";
        case NiAnimationKey::EULERKEY:
            return "EULERKEY";
    }
    return NULL;
}

//---------------------------------------------------------------------------
unsigned int NiAnimationAnalyzerPlugin::GetKeySize(NiAnimationKey* pkKey, 
    NiAnimationKey::KeyContent eContentType, NiAnimationKey::KeyType eType)
{
    switch (eContentType)
    {
        case NiAnimationKey::FLOATKEY:
            {
                switch (eType)
                {
                    case NiAnimationKey::LINKEY:
                        return sizeof(NiLinFloatKey);
                        break;
                    case NiAnimationKey::BEZKEY:
                        return sizeof(NiBezFloatKey);
                        break;
                    case NiAnimationKey::TCBKEY:
                        return sizeof(NiTCBFloatKey);
                        break;
                }
            }
            break;
        case NiAnimationKey::POSKEY:
            {
                switch (eType)
                {
                    case NiAnimationKey::LINKEY:
                        return sizeof(NiLinPosKey);
                        break;
                    case NiAnimationKey::BEZKEY:
                        return sizeof(NiBezPosKey);
                        break;
                    case NiAnimationKey::TCBKEY:
                        return sizeof(NiTCBPosKey);
                        break;
                }
            }
            break;
        case NiAnimationKey::ROTKEY:
            {
                switch (eType)
                {
                    case NiAnimationKey::LINKEY:
                        return sizeof(NiLinRotKey);
                        break;
                    case NiAnimationKey::BEZKEY:
                        return sizeof(NiBezRotKey);
                        break;
                    case NiAnimationKey::TCBKEY:
                        return sizeof(NiTCBRotKey);
                        break;
                    case NiAnimationKey::EULERKEY:
                        {
                            NiEulerRotKey* pkEulerKey = (NiEulerRotKey*)
                                pkKey;
                            unsigned int uiSize = sizeof(NiEulerRotKey);
                            for (unsigned char ucTrack = 0; ucTrack < 3;
                                ucTrack++)
                            {
                                unsigned int uiNumFloatKeys =
                                    pkEulerKey->GetNumKeys(ucTrack);
                                unsigned char ucFSize =
                                    pkEulerKey->GetKeySize(ucTrack);
                                uiSize += uiNumFloatKeys * ucFSize;
                            }
                            return uiSize;
                        }
                        break;
                }
            }
            break;
    }
    return 0;

}
//---------------------------------------------------------------------------
NiString NiAnimationAnalyzerPlugin::ToString(float fValue)
{
    return NiString::FromFloat(fValue);
}
//---------------------------------------------------------------------------
NiString NiAnimationAnalyzerPlugin::ToString(NiPoint3 kValue)
{
    NiString strValue;
    strValue += "[ ";
    strValue +=  NiString::FromFloat(kValue.x);
    strValue +=  ", ";
    strValue +=  NiString::FromFloat(kValue.y);
    strValue +=  ", ";
    strValue +=  NiString::FromFloat(kValue.z);
    strValue += " ]";
    return strValue;
}
//---------------------------------------------------------------------------
NiString NiAnimationAnalyzerPlugin::ToString(NiQuaternion kValue)
{
    NiString strValue;
    strValue += "[ ";
    strValue +=  NiString::FromFloat(kValue.GetX());
    strValue +=  ", ";
    strValue +=  NiString::FromFloat(kValue.GetY());
    strValue +=  ", ";
    strValue +=  NiString::FromFloat(kValue.GetZ());
    strValue +=  ", ";
    strValue +=  NiString::FromFloat(kValue.GetW());
    strValue += " ]";
    return strValue;
}

//---------------------------------------------------------------------------
NiString NiAnimationAnalyzerPlugin::GetKeyAsString(NiAnimationKey* pkKey, 
    NiAnimationKey::KeyContent eContentType, NiAnimationKey::KeyType eType)
{
    NiString strValue = "t= " ;
    strValue += ToString(pkKey->GetTime());  
    strValue += "\t";
    strValue += "v = ";

    switch (eContentType)
    {
        case NiAnimationKey::FLOATKEY:
            {
                NiFloatKey* pkFloatKey = (NiFloatKey*) pkKey;
                strValue += ToString(pkFloatKey->GetValue());
                strValue += "\t";

                switch (eType)
                {
                    case NiAnimationKey::BEZKEY:
                        {
                            NiBezFloatKey* pkBezKey =
                                (NiBezFloatKey*) pkFloatKey;
                            strValue += "it= ";
                            strValue += ToString(pkBezKey->GetInTan());
                            strValue += "\t ot= ";
                            strValue += ToString(pkBezKey->GetOutTan());
                        }
                        break;
                    case NiAnimationKey::TCBKEY:
                        {
                            NiTCBFloatKey* pkTCBKey = 
                                (NiTCBFloatKey*) pkFloatKey;
                            strValue += "tension = ";
                            strValue += ToString(pkTCBKey->GetTension());
                            strValue += "\tcont = ";
                            strValue += ToString(pkTCBKey->GetContinuity());
                            strValue += "\tbias = ";
                            strValue += ToString(pkTCBKey->GetBias());
                        }
                        break;
                }
            }
            break;
        case NiAnimationKey::POSKEY:
            {
                NiPosKey* pkPosKey = (NiPosKey*) pkKey;
                NiPoint3 kPos = pkPosKey->GetPos();
                strValue += ToString(kPos);
                strValue += "\t";

                switch (eType)
                {
                    case NiAnimationKey::BEZKEY:
                        {
                            NiBezPosKey* pkBezKey = (NiBezPosKey*) pkPosKey;
                            strValue += "it= ";
                            strValue += ToString(pkBezKey->GetInTan());
                            strValue += "\t ot= ";
                            strValue += ToString(pkBezKey->GetOutTan());
                        }
                        break;
                    case NiAnimationKey::TCBKEY:
                        {
                            NiTCBPosKey* pkTCBKey = (NiTCBPosKey*) pkPosKey;
                            strValue += "tension = ";
                            strValue += ToString(pkTCBKey->GetTension());
                            strValue += "\tcont = ";
                            strValue += ToString(pkTCBKey->GetContinuity());
                            strValue += "\tbias = ";
                            strValue += ToString(pkTCBKey->GetBias());
                        }
                        break;
                }
            }
            break;
        case NiAnimationKey::ROTKEY:
            {
                NiRotKey* pkRotKey = (NiRotKey*) pkKey;

                NiQuaternion kRot = pkRotKey->GetQuaternion();
                strValue += ToString(kRot);
                strValue += "\t";

                switch (eType)
                {
                    case NiAnimationKey::TCBKEY:
                        {
                            NiTCBRotKey* pkTCBKey = (NiTCBRotKey*) pkRotKey;
                            strValue += "tension = ";
                            strValue += ToString(pkTCBKey->GetTension());
                            strValue += "\tcont = ";
                            strValue += ToString(pkTCBKey->GetContinuity());
                            strValue += "\tbias = ";
                            strValue += ToString(pkTCBKey->GetBias());
                        }
                        break;
                    case NiAnimationKey::EULERKEY:
                        {
                            NiEulerRotKey* pkEulerKey = 
                                (NiEulerRotKey*) pkRotKey;
                            for (unsigned char ucTrack = 0; ucTrack < 3; 
                                ucTrack++)
                            {
                                NiFloatKey* pkFloatKey =
                                    pkEulerKey->GetKeys(ucTrack);
                                NiFloatKey::KeyType eFType =
                                    pkEulerKey->GetType(ucTrack);
                                unsigned char ucFSize =
                                    pkEulerKey->GetKeySize(ucTrack);
                                strValue += "\n\tTrack #";
                                strValue += NiString::FromInt(ucTrack);
                                strValue += " NumKeys: ";
                                strValue += NiString::FromInt(
                                    pkEulerKey->GetNumKeys(ucTrack));
                                strValue += "\n";

                                for (unsigned int uiKey = 0;
                                    uiKey < pkEulerKey->GetNumKeys(ucTrack);
                                    uiKey++)
                                {
                                    strValue += "\t\t" + GetKeyAsString(
                                        pkFloatKey->GetKeyAt(uiKey, ucFSize),
                                        NiAnimationKey::FLOATKEY, eFType);
                                    strValue += "\n";
                                }
                            }
                        }
                        break;
                }
            }
            break;
    }
    return strValue;
}

//---------------------------------------------------------------------------
void  NiAnimationAnalyzerPlugin::FindAnimations(NiAVObject* pkObj, 
    NiString& strAnimationRes)
{
    if (pkObj != NULL)
    {
        NiTimeController* pkControl = pkObj->GetControllers();

        if (pkControl)
        {
            strAnimationRes += "\n\n";
            strAnimationRes += "\nAnimated Object: ";
            strAnimationRes += pkObj->GetName();
            strAnimationRes += "\n";
        }

        while (pkControl)
        {
            strAnimationRes += "Controller: ";
            strAnimationRes += (pkControl->GetRTTI()->GetName());
            strAnimationRes += "\n";

            if (NiIsKindOf(NiTransformController, pkControl))
            {
                m_uiTotalNumControllers++;
                NiTransformController* pkKFControl = (NiTransformController*)
                    pkControl;

                NiInterpolator* pkInterp = pkKFControl->GetInterpolator();
                if (!pkInterp)
                    continue;
                strAnimationRes += "Interpolator: ";
                strAnimationRes += (pkInterp->GetRTTI()->GetName());
                strAnimationRes += "\n";
                
                if (!NiIsKindOf(NiTransformInterpolator, pkInterp))
                    continue;

                NiTransformInterpolator* pkTransformInterp = 
                    (NiTransformInterpolator*) pkInterp;

                NiTransformData* pkData = pkTransformInterp
                    ->GetTransformData();
                if (pkData)
                {
                    unsigned int uiNumRotKeys;
                    NiRotKey::KeyType eRotType;
                    unsigned char ucRotSize;
                    NiRotKey* pkRotKey = pkData->GetRotAnim(uiNumRotKeys,
                        eRotType, ucRotSize);

                    m_uiTotalRotKeys += uiNumRotKeys;
                    m_uiTotalNumKeys += uiNumRotKeys;
                    unsigned int uiRotKeySize = uiNumRotKeys *  
                        GetKeySize(pkRotKey, NiAnimationKey::ROTKEY,
                        eRotType);
                    m_uiTotalSizeKeys += uiRotKeySize;

                    strAnimationRes += "Rot Keys: " ;
                    strAnimationRes += "(Count = ";
                    strAnimationRes += NiString::FromInt(uiNumRotKeys);
                    strAnimationRes += "  Type = ";
                    strAnimationRes += GetKeyTypeEnumString(eRotType);
                    strAnimationRes += "  Size = ";
                    strAnimationRes += NiString::FromInt(uiRotKeySize);
                    strAnimationRes += " bytes)\n";
                   
                    for (unsigned int uiKey = 0; uiKey < uiNumRotKeys;
                        uiKey++)
                    {
                        strAnimationRes += "\t(";
                        strAnimationRes += GetKeyAsString(pkRotKey->GetKeyAt(
                            uiKey, ucRotSize), NiAnimationKey::ROTKEY, 
                            eRotType);
                        strAnimationRes += ")\n";
                    }

                    strAnimationRes += "\n\n";

                    unsigned int uiNumPosKeys;
                    NiPosKey::KeyType ePosType;
                    unsigned char ucPosSize;
                    NiPosKey* pkPosKey = pkData->GetPosAnim(uiNumPosKeys, 
                        ePosType, ucPosSize);

                    m_uiTotalPosKeys += uiNumPosKeys;
                    m_uiTotalNumKeys += uiNumPosKeys;
                    unsigned int uiPosKeySize = uiNumPosKeys * 
                        GetKeySize(pkPosKey, NiAnimationKey::POSKEY,
                        ePosType);
                    m_uiTotalSizeKeys += uiPosKeySize;

                    strAnimationRes += "Pos Keys: " ;
                    strAnimationRes += "(Count = ";
                    strAnimationRes += NiString::FromInt(uiNumPosKeys);
                    strAnimationRes += "  Type = ";
                    strAnimationRes += GetKeyTypeEnumString(ePosType);
                    strAnimationRes += "  Size = ";
                    strAnimationRes += NiString::FromInt(uiPosKeySize);
                    strAnimationRes += " bytes)\n";
                    
                    for (unsigned int uiKey2 = 0; uiKey2 < uiNumPosKeys; 
                        uiKey2++)
                    {
                        strAnimationRes += "\t(";
                        strAnimationRes +=
                            GetKeyAsString(pkPosKey->GetKeyAt(uiKey2,
                            ucPosSize), NiAnimationKey::POSKEY, ePosType);
                        strAnimationRes += ")\n";
                    }

                    strAnimationRes += "\n\n";

                    unsigned int uiNumScaleKeys;
                    NiFloatKey::KeyType eScaleType;
                    unsigned char ucScaleSize;
                    NiFloatKey* pkScaleKey =
                        pkData->GetScaleAnim(uiNumScaleKeys, 
                        eScaleType, ucScaleSize);

                    m_uiTotalScaleKeys += uiNumScaleKeys;
                    m_uiTotalNumKeys += uiNumScaleKeys;
                    
                    unsigned int uiScaleKeySize = uiNumScaleKeys * 
                        GetKeySize(pkScaleKey, NiAnimationKey::FLOATKEY,
                            eScaleType);
                    m_uiTotalSizeKeys += uiScaleKeySize;

                    strAnimationRes += "Scale Keys: " ;
                    strAnimationRes += "(Count = ";
                    strAnimationRes += NiString::FromInt(uiNumScaleKeys);
                    strAnimationRes += "  Type = ";
                    strAnimationRes += GetKeyTypeEnumString(eScaleType);
                    strAnimationRes += "  Size = ";
                    strAnimationRes += NiString::FromInt(uiScaleKeySize);
                    strAnimationRes += " bytes)\n";
                    
                    for (unsigned int uiKey3 = 0; uiKey3 < uiNumScaleKeys; 
                        uiKey3++)
                    {
                        strAnimationRes += "\t(";
                        strAnimationRes +=
                            GetKeyAsString(pkScaleKey->GetKeyAt(uiKey3,
                            ucScaleSize), NiAnimationKey::FLOATKEY, 
                            eScaleType);
                        strAnimationRes += ")\n";
                    }

                    strAnimationRes += "\n\n";
                }
            }
            pkControl = pkControl->GetNext();
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkObject = pkNode->GetAt(ui);
            if (pkObject)
                FindAnimations(pkObject, strAnimationRes);
        }
    }
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiAnimationAnalyzerPlugin::Execute(
    const NiPluginInfo*)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    m_uiTotalNumKeys = 0;
    m_uiTotalNumControllers = 0;
    m_uiTotalSizeKeys = 0;
    m_uiTotalRotKeys= 0;
    m_uiTotalPosKeys = 0;
    m_uiTotalScaleKeys= 0;

    NiString strAnimationRes(50000);
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);

        FindAnimations(spScene, strAnimationRes);
    }


    m_strMessage = "Totals:\n";
    m_strMessage += "=====================================================\n";
    m_strMessage += "# kf Controllers: \t\t";
    m_strMessage += NiString::FromInt(m_uiTotalNumControllers);
    m_strMessage += "\n";

    m_strMessage += "# keys: \t\t\t";
    m_strMessage += NiString::FromInt(m_uiTotalNumKeys);
    m_strMessage += "\n";

    m_strMessage += "size keys: \t\t";
    m_strMessage += NiString::FromInt(m_uiTotalSizeKeys);
    m_strMessage += " bytes\n";

    m_strMessage += "# pos keys: \t\t";
    m_strMessage += NiString::FromInt(m_uiTotalPosKeys);
    m_strMessage += "\n";

    m_strMessage += "# rot keys: \t\t";
    m_strMessage += NiString::FromInt(m_uiTotalRotKeys);
    m_strMessage += "\n";

    m_strMessage += "# scale keys: \t\t";
    m_strMessage += NiString::FromInt(m_uiTotalScaleKeys);
    m_strMessage += "\n";

    m_strMessage += "\n\nDetails\n";
    m_strMessage += "=====================================================\n";
    m_strMessage += strAnimationRes;

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

    if ((bWriteResultsToLog) && (pkLogger))
    {
        pkLogger->LogElement("AnimationAnalysisResults", m_strMessage);
    }
    else
    {
        NiInfoDialog kDlg("Animation Analysis Results:");
        kDlg.SetText(m_strMessage);
        kDlg.DoModal();
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiAnimationAnalyzerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiAnimationAnalyzerPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiAnimationAnalyzerPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiAnimationAnalyzerPlugin::DoManagementDialog(
    NiPluginInfo*, NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
