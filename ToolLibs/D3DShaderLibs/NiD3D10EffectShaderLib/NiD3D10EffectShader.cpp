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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectShader.h"
#include "NiD3D10EffectShaderSDM.h"
#include "NiD3D10EffectFactory.h"
#include "NiD3D10EffectFile.h"
#include "NiD3D10EffectParameter.h"
#include "NiD3D10EffectPass.h"
#include "NiD3D10EffectTechniqueSet.h"

#include <NiD3D10Renderer.h>
#include <NiD3D10ShaderConstantManager.h>
#include <NiNode.h>
#include <NiRenderObject.h>
#include <NiTimeSyncController.h>

static NiD3D10EffectShaderSDM NiD3D10EffectShaderSDMObject;
bool NiD3D10EffectShader::
    bEnabledParameterTypes[NiD3D10EffectParameterDesc::PT_NUMBER_TYPES] = 
    {    
        false,  //PT_NONE = 0,
        false,  //PT_MATRIX,
        false,  //PT_VECTOR,
        false,  //PT_FLOAT,
        false,  //PT_BOOL,
        false,  //PT_UINT,
        true,   //PT_TEXTURE,
        false,  //PT_PREDEFINED,
        false,  //PT_OBJECT,
        false   //PT_ARRAY
    };

NiImplementRTTI(NiD3D10EffectShader, NiD3D10Shader);

//---------------------------------------------------------------------------
NiD3D10EffectShader::NiD3D10EffectShader() :
    NiD3D10Shader(),
    m_pkD3D10Effect(NULL),
    m_pkD3D10EffectTechnique(NULL),
    m_uiActualImplementation(0)
{ /* */ }
//---------------------------------------------------------------------------
NiD3D10EffectShader::~NiD3D10EffectShader()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShader::Initialize()
{
    if (!m_bInitialized)
    {
        if (!NiD3D10Shader::Initialize())
            return false;

        if (!m_kName.Exists() || m_kName.GetLength() == 0)
            return false;

        RecreateRendererData();
        if (m_pkD3D10Effect == NULL || m_kPasses.GetSize() == 0)
            return false;
    }

    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10EffectShader::PreProcessPipeline(
    const NiRenderCallContext& kRCC)
{
    NiUInt32 uiReturn = NiD3D10Shader::PreProcessPipeline(kRCC);

    // Make sure renderer data is up to date
    if (m_pkD3D10Effect == NULL || m_kPasses.GetSize() == 0)
    {
        RecreateRendererData();
        if (m_pkD3D10Effect == NULL)
        {
            NiRenderer::Error(
                "%s - %s - Effect failed to load\n",
                __FUNCTION__,
                (const char*)m_kName);
        }
    }

    return uiReturn;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10EffectShader::SetupShaderPrograms(
    const NiRenderCallContext& kRCC)
{
    // Clear constant buffers - D3D10 will handle setting the buffers.
    NiD3D10ShaderConstantManager* pkShaderConstantManager = 
        NiD3D10Renderer::GetRenderer()->GetShaderConstantManager();
    NIASSERT(pkShaderConstantManager);
    pkShaderConstantManager->ResetConstantBuffers();

    // Set parameters and textures
    // Check the Technique for each paramenter type that
    // we have enabled. Currently we only care about 
    // Textures.


    unsigned int i = ( unsigned int ) NiD3D10EffectParameterDesc::PT_NONE;
    unsigned int last = 
              ( unsigned int ) NiD3D10EffectParameterDesc::PT_NUMBER_TYPES; 
    for(;i < last;i++)    
    {
        NiD3D10EffectParameterDesc::ParameterType kType =  
            (NiD3D10EffectParameterDesc::ParameterType) i;
        if (!NiD3D10EffectShader::bEnabledParameterTypes[kType])
            continue;
        if (m_pkD3D10EffectTechnique->HasParametersOfType(kType))
        {
            NiD3D10EffectParameter* pkParam = 
                m_pkD3D10EffectTechnique->GetFirstParameter(kType);
            // go thru the parameters for this type.
            while (pkParam)
            {
                NiShaderError eResult = pkParam->SetParameter(kRCC);

                if (eResult != NISHADERERR_OK)
                {
                    switch (eResult)
                    {
                    case NISHADERERR_DYNEFFECTNOTFOUND:
                        if (NiShaderErrorSettings::
                            GetAllowDynEffectNotFound())
                        {
                            NiRenderer::Warning(
                                "%s - %s - "
                                "Parameter %s on geometry with name \"%s\" "
                                "references a nonexistent NiDynamicEffect "
                                "object. Default values used.\n", 
                                __FUNCTION__,
                                (const char*)m_kName, 
                                (const char*)pkParam->GetName(), 
                                (const char*)kRCC.m_pkMesh->GetName());
                        }
                        break;
                    case NISHADERERR_ENTRYNOTFOUND:
                        NiRenderer::Warning(
                            "%s - %s - "
                            "Parameter %s was not found on geometry with "
                            "name \"%s\". This warning can be ignores during "
                            "intialization in the first frame\.\n", 
                            __FUNCTION__,
                            (const char*)m_kName,
                            (const char*)pkParam->GetName(), 
                            (const char*)kRCC.m_pkMesh->GetName()); 
                        break;
                    default:
                        NiRenderer::Error(
                            "%s - %s - "
                            "Parameter %s failed to be set on geometry with "
                            "name \"%s\"\n", 
                            __FUNCTION__,
                            (const char*)m_kName,
                            (const char*)pkParam->GetName(), 
                            (const char*)kRCC.m_pkMesh->GetName());
                        break;
                    }
                }
                pkParam = m_pkD3D10EffectTechnique->GetNextParameter(kType);          
            }
        }
    }
    // This is valid, since (1) the NiD3D10EffectPass objects will apply the 
    // shaders correctly, and (2) the GetVertexShader, GetGeometryShader, and
    // GetPixelShader functions will all return NULL, so no "global" shader
    // constants will be set. 

    return NiD3D10Shader::SetupShaderPrograms(kRCC);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10EffectShader::PostProcessPipeline(
    const NiRenderCallContext& kRCC)
{
    // Shader will have set various state without informing Gamebryo
    // Invalidate the currently cached device state
    NiD3D10Renderer* pkRenderer = 
        NiVerifyStaticCast(NiD3D10Renderer, NiRenderer::GetRenderer());
    pkRenderer->InvalidateDeviceState();

    return NiD3D10Shader::PostProcessPipeline(kRCC);
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::SetEffectTechnique(
    NiD3D10EffectTechnique* pkD3D10EffectTechnique)
{
    m_pkD3D10EffectTechnique = pkD3D10EffectTechnique;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShader::SetupGeometry(NiRenderObject* pkMesh, 
    NiMaterialInstance* pkMaterialInstance)
{
    if (!pkMesh)
        return false;

    // Attach time parameter
    bool bTimeRequired = m_pkD3D10EffectTechnique->GetTimeParameterPresent();

    if (bTimeRequired)
    {
        NiTimeSyncController* pkTimeCtrlr = NiNew NiTimeSyncController();
        NIASSERT(pkTimeCtrlr);
        pkTimeCtrlr->SetTarget(pkMesh);

        // Ensure object will be updated by UpdateSelected.
        NiAVObject* pkObject = pkMesh;
        while (pkObject != NULL)
        {
            pkObject->SetSelectiveUpdate(true);
            pkObject = pkObject->GetParent();
        }
    }

    // This is valid, since SetupSemanticAdapterTable will fail gracefully,
    // as a semantic adapter will already exist.
    return NiD3D10Shader::SetupGeometry(pkMesh, pkMaterialInstance);
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::SetActualImplementation(
    unsigned int uiActualImplementation)
{
    m_uiActualImplementation = uiActualImplementation;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10EffectShader::GetActualImplementation() const
{
    return m_uiActualImplementation;
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::DestroyRendererData()
{
    NIASSERT(m_pkD3D10EffectTechnique && 
        m_pkD3D10EffectTechnique->GetFile());
    m_pkD3D10EffectTechnique->GetFile()->DestroyRendererData();

    if (m_pkD3D10Effect)
    {
        m_pkD3D10Effect->Release();
        m_pkD3D10Effect = NULL;
    }

    m_bInitialized = false;
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::RecreateRendererData()
{
    // There had better be a renderer present before we attempt to recreate
    // the renderer data!
#ifdef NIDEBUG
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
#endif
    NIASSERT(pkRenderer);

    NiD3D10EffectFile* pkFile = m_pkD3D10EffectTechnique->GetFile();
    pkFile->LinkRendererData();
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::InitializePassArray(NiUInt32 uiPassCount)
{
    // Don't try to initialize an already-initialized pass
    NIASSERT(m_kPasses.GetSize() == 0);

    m_kPasses.SetSize(uiPassCount);
    for (NiUInt32 i = 0; i < uiPassCount; i++)
    {
        m_kPasses.SetAt(i, NiD3D10EffectPass::Create());
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::SetAdapterSemanticTable(
    const NiSemanticAdapterTable& kTable)
{
    NIASSERT(m_kAdapterTable.GetNumFilledTableEntries() == 0);
    m_kAdapterTable = kTable;
}
//---------------------------------------------------------------------------
NiD3D10EffectPass* NiD3D10EffectShader::GetPass(NiUInt32 uiPass) const
{
    if (uiPass > m_kPasses.GetSize())
        return NULL;
    return NiSmartPointerCast(NiD3D10EffectPass, m_kPasses.GetAt(uiPass));
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::SetD3D10Effect(ID3D10Effect* pkEffect)
{
    if (pkEffect != m_pkD3D10Effect)
    {
        if (m_pkD3D10Effect)
            m_pkD3D10Effect->Release();
        m_pkD3D10Effect = pkEffect;
        if (m_pkD3D10Effect)
            m_pkD3D10Effect->AddRef();
    }
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShader::GetVertexInputSemantics(
    NiShaderDeclaration::ShaderRegisterEntry*, 
    unsigned int)
{
    // No NiShaderDeclaration-derived object is used, so it's not necessary
    // to provide any vertex input semantics. 
    return false;
}
//---------------------------------------------------------------------------
const D3D10_STATE_BLOCK_MASK* NiD3D10EffectShader::GetStateBlockMask(
    const NiRenderCallContext&) const
{
    NIASSERT(m_pkCurrentPass);
    NiD3D10EffectPass* pkCurrentPass = (NiD3D10EffectPass*)m_pkCurrentPass;

    return pkCurrentPass->GetStateBlockMask();
}
//---------------------------------------------------------------------------
void NiD3D10EffectShader::EnableParameterType(
    NiD3D10EffectParameterDesc::ParameterType kType, bool bval)
{
    NiD3D10EffectShader::bEnabledParameterTypes[kType] = bval; 
}
//---------------------------------------------------------------------------
