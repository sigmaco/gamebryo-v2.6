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

#include "MaxImmerse.h"
#include "NiMAXConverter.h"
#include "NiMAXCameraConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXLightConverter.h"
#include "NiMAXTextureConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXGeomConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiMAXMorphConverter.h"
#include "NiMAXAnimationConverter.h"
#include "NiMAXDynamicEffectConverter.h"
#include "NiMAXPhysicsConverter.h"

#ifndef NO_PARTICLES
#include "NiMAXPSysConverter.h"
#endif

#include "NiMAXShapeConverter.h"
#include "NiPNGReader.h"
#include "NiHDRReader.h"
#include "NiOpenEXRReader.h"

#include <NiSkinningUtilities.h>


extern HINSTANCE hInstance;

bool NiMAXConverter::ms_bExportSelected = false;
bool NiMAXConverter::m_bUseAppTime = true;
bool NiMAXConverter::ms_bViewerRun = false;
NiInfoDialog* NiMAXConverter::ms_pkInfoDialog = NULL;
NiProgressDialog* NiMAXConverter::ms_pkViewerLoaderDlg = NULL;
void* NiMAXConverter::m_pUserData = NULL;
unsigned int NiMAXConverter::ms_uiNumErrors = 0;
NiXMLLogger NiMAXConverter::ms_kLogger;


//---------------------------------------------------------------------------
void NiMAXConverter::ConvertBackGround(Interface* pIntf)
{
    Interval tmpValid=FOREVER;
    Point3 back;

    back = pIntf->GetBackGround(m_animStart, tmpValid);
    m_backGnd.r = back.x;
    m_backGnd.g = back.y;
    m_backGnd.b = back.z;
}

Class_ID fogClassID(FOG_CLASS_ID,0);
//---------------------------------------------------------------------------
void NiMAXConverter::ConvertAtmospherics(Interface* pIntf, NiNode* pkScene)
{
    if (!pIntf || !pkScene)
        return;


    for (int i = 0; i < pIntf->NumAtmospheric(); i++)
    {
        Atmospheric* pkAtm = pIntf->GetAtmospheric(i);

        if (pkAtm != NULL && pkAtm->ClassID() == fogClassID)
        {
            StdFog* pkStdFog = (StdFog*) pkAtm;
            NiFogProperty* pkFogProperty = NiNew NiFogProperty();

            Color kFogColor = pkStdFog->GetColor(0);
            pkFogProperty->SetFogColor(NiColor(kFogColor.r, kFogColor.g, 
                kFogColor.b));
            float fDepth = 0.0f;
            fDepth = 1.0f - pkStdFog->GetNear(0);
            pkFogProperty->SetDepth(fDepth);
            pkFogProperty->SetFog(true);
            NiFogProperty::FogFunction eFunc = NiFogProperty::FOG_RANGE_SQ;
            pkFogProperty->SetFogFunction(eFunc);


            pkScene->AttachProperty(pkFogProperty);
            return;
        }
    }


}
//---------------------------------------------------------------------------
bool NiMAXConverter::GetExportSelected()
{
    return ms_bExportSelected;
}
//---------------------------------------------------------------------------
int NiMAXConverter::Convert(Interface* pIntf, bool bExportSelected)
{
    CHECK_MEMORY();
    ms_bExportSelected = bExportSelected;

    m_animStart = pIntf->GetAnimRange().Start();
    m_animEnd = pIntf->GetAnimRange().End();

    if (m_animStart > 0)
        m_animStart = 0;

    INode* pMaxRoot = pIntf->GetRootNode();
    TimeValue tempStart = TIME_PosInfinity;
    TimeValue tempEnd = TIME_NegInfinity;
    NiMAXAnimationConverter::CalculateGlobalTimeRange(tempStart, tempEnd);
    int iStatus;

    if (tempEnd < TIME_PosInfinity && 
        tempEnd > TIME_NegInfinity && tempEnd > m_animEnd)
        m_animEnd = tempEnd;
    
    // use same name for object clones
    NiObjectNET::SetDefaultCopyType(NiObjectNET::COPY_EXACT);

    // background color for renderer
    ConvertBackGround(pIntf);

    //ms_pkViewerLoaderDlg = new MiViewerLoaderDialog(hInstance,
    //  "Generating Scene Graph");
    //ms_pkViewerLoaderDlg->UpdateLoadingString("Preprocessing Data..");
    ms_pkViewerLoaderDlg = NiNew NiProgressDialog("Generating Scene Graph");
    ms_pkViewerLoaderDlg->Create();
    ms_pkViewerLoaderDlg->SetLineOne("Preprocessing Data..");

    unsigned int uiNodeCount = CalculateTotalNodeCount(pMaxRoot) + 2;
    
    ms_pkViewerLoaderDlg->SetRangeSpan(uiNodeCount);
 
    NiMAXHierConverter::Preprocess(pMaxRoot);
    NiMAXMeshConverter::Preprocess(pMaxRoot);
    NiMAXMorphConverter::Preprocess(pMaxRoot);

    ScaleInfo rootScale;

    rootScale.bIsIdent = true;
    rootScale.scale.s.x = 1.0f;
    rootScale.scale.s.y = 1.0f;
    rootScale.scale.s.z = 1.0f;
    rootScale.scale.q.x = 0.0f;
    rootScale.scale.q.y = 0.0f;
    rootScale.scale.q.z = 0.0f;
    rootScale.scale.q.w = 1.0f;

    {
        NiMAXHierConverter hier(m_animStart, m_animEnd);
        ms_pkViewerLoaderDlg->SetLineOne("Converting: ");
        CHECK_MEMORY();

        iStatus = hier.BuildNode(pIntf, pMaxRoot, &m_spScene, &rootScale,
                false, false);

        if (!iStatus)
        {
            iStatus = NiMAXPhysicsConverter::Finish(m_animStart);
        }

        ms_pkViewerLoaderDlg->StepIt();
        CHECK_MEMORY();
        if (iStatus)
        {
            ms_pkViewerLoaderDlg->Destroy();
            NiDelete ms_pkViewerLoaderDlg;
            ms_pkViewerLoaderDlg = NULL;
            return(iStatus);
        }
        
    }

    {
        NiMAXAnimationConverter anim(m_animStart, m_animEnd);

        anim.Finish();
    }

    {
        NiMAXLightConverter light(m_animStart, m_animEnd);

        light.Finish(pIntf, m_spScene);
        light.ConvertAmbientLight(pIntf, m_spScene);
    }

    NiOptimize::CleanUpAnimationKeys(m_spScene);

    ConvertAtmospherics(pIntf, m_spScene);

    NiMAXGeomConverter::Finish(m_spScene, m_animStart);
#ifndef NO_PARTICLES
    NiMAXPSysConverter::Finish(m_spScene);
#endif

    NiMAXDynamicEffectConverter::Finish(m_spScene);
    NiMAXMorphConverter::Postprocess(pMaxRoot);
    NiMAXMeshConverter::Postprocess(pMaxRoot);
    NiMAXHierConverter::Postprocess(pMaxRoot);
    CHECK_MEMORY();

    NiVertexColorProperty* pLitMode;
    pLitMode = NiNew NiVertexColorProperty;
    pLitMode->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);
    pLitMode->SetLightingMode(NiVertexColorProperty::LIGHTING_E_A_D);
    m_spScene->AttachProperty(pLitMode);

    // use z-buffering
    NiZBufferProperty* pZBufferMode;
    pZBufferMode = NiNew NiZBufferProperty;
    pZBufferMode->SetZBufferTest(TRUE);
    pZBufferMode->SetZBufferWrite(TRUE);
    m_spScene->AttachProperty(pZBufferMode);

    // Reorder the bones to make sure the correct update order.
    NiSkinningUtilities::ReorderBonesForSkinnedMeshes(m_spScene);
    CHECK_MEMORY();

    // propagate transforms downwards, calculate bounds upwards
    m_spScene->Update(0.0f);
    ms_pkViewerLoaderDlg->Destroy();
    NiDelete ms_pkViewerLoaderDlg;
    ms_pkViewerLoaderDlg = NULL;

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
NiMAXConverter::NiMAXConverter()
{
    m_animStart = 0;
    m_animEnd = 0;

    m_fInvTicksPerSecond = 
        1.0f / ((float)(GetFrameRate() * GetTicksPerFrame()));


    NiDevImageConverter* pkConverter;
    pkConverter = 
        (NiDevImageConverter*)NiImageConverter::GetImageConverter();
    pkConverter->AddReader(NiNew NiPNGReader);
    pkConverter->AddReader(NiNew NiHDRReader);
    pkConverter->AddReader(NiNew NiOpenEXRReader);
}
//---------------------------------------------------------------------------
void NiMAXConverter::Init()
{
    NiMAXCameraConverter::Init();
    NiMAXHierConverter::Init();
    NiMAXLightConverter::Init();
    NiMAXTextureConverter::Init();
    NiMAXMaterialConverter::Init();
    NiMAXGeomConverter::Init();
    NiMAXMeshConverter::Init();
    NiMAXMorphConverter::Init();
    NiMAXAnimationConverter::Init();
    NiMAXDynamicEffectConverter::Init();
    ms_pkInfoDialog = NiNew NiInfoDialog("Gamebryo/Max Export Errors:");
    ClearLog();
}
//---------------------------------------------------------------------------
void NiMAXConverter::Shutdown()
{
    NiMAXCameraConverter::Shutdown();
    NiMAXHierConverter::Shutdown();
    NiMAXLightConverter::Shutdown();
    NiMAXTextureConverter::Shutdown();
    NiMAXMaterialConverter::Shutdown();
    NiMAXMeshConverter::Shutdown();
    NiMAXGeomConverter::Shutdown();
    NiMAXMorphConverter::Shutdown();
    NiMAXAnimationConverter::Shutdown();
    NiMAXDynamicEffectConverter::Shutdown();
    NiDelete ms_pkInfoDialog;
    ms_pkInfoDialog = NULL;
}
//---------------------------------------------------------------------------
NiMAXConverter::~NiMAXConverter()
{
}
//---------------------------------------------------------------------------
unsigned int NiMAXConverter::CalculateTotalNodeCount(INode* pkNode)
{
    if(pkNode == NULL)
        return 0;

    unsigned int uiCount = (unsigned int) pkNode->NumberOfChildren();
    unsigned int childCount = uiCount;
    for(unsigned int ui = 0; ui < uiCount; ui++)
    {
        childCount += CalculateTotalNodeCount(pkNode->GetChildNode(ui));
    }

    return childCount;
}
//---------------------------------------------------------------------------
NiProgressDialog* NiMAXConverter::GetDialog()
{
    return ms_pkViewerLoaderDlg;
}
//---------------------------------------------------------------------------
void NiMAXConverter::LogError(const char* pcError)
{
    NiOutputDebugString("\nERROR: ");
    NiOutputDebugString(pcError);

    ms_uiNumErrors++;

    if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == true)
    {
        NiXMLLogger* pkLogger = NiMAXConverter::GetXMLLogger();
        if (pkLogger)
            pkLogger->LogElement("ERROR", pcError);
    }
    else
    {
        if (ms_pkInfoDialog)
        {
            ms_pkInfoDialog->AppendText("\nERROR: ");
            ms_pkInfoDialog->AppendText(pcError);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiMAXConverter::LogWarning(const char* pcError)
{
    NiOutputDebugString("\nWARNING: ");
    NiOutputDebugString(pcError);
            
    ms_uiNumErrors++;

    if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == true)
    {
        NiXMLLogger* pkLogger = NiMAXConverter::GetXMLLogger();
        if (pkLogger)
            pkLogger->LogElement("WARNING", pcError);
    }
    else
    {
        if (ms_pkInfoDialog)
        {
            ms_pkInfoDialog->AppendText("\nWARNING: ");
            ms_pkInfoDialog->AppendText(pcError);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiMAXConverter::ClearLog()
{
    ms_uiNumErrors = 0;
    if (ms_pkInfoDialog)
    {
        ms_pkInfoDialog->SetText("");
        return;
    }
}
//---------------------------------------------------------------------------
bool NiMAXConverter::AreAnyErrors()
{
    if (ms_pkInfoDialog)
    {
        return ms_uiNumErrors != 0;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiMAXConverter::ShowLog()
{
    if (ms_pkInfoDialog)
    {
        ms_pkInfoDialog->DoModal();
        return;
    }
}
//---------------------------------------------------------------------------
unsigned int NiMAXConverter::GetErrorCount()
{
    return ms_uiNumErrors;
}

