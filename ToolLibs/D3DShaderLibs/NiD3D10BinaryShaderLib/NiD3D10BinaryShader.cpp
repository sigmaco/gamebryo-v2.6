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
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NiD3D10BinaryShader.h"
#include "NSBD3D10Utility.h"

#include <NiD3D10Renderer.h>
#include <NiShadowMap.h>
#include <NiShadowGenerator.h>
#include <NiD3D10RenderStateManager.h>
#include <NiD3D10ShaderProgramFactory.h>
#include <NiTimeSyncController.h>


NiImplementRTTI(NiD3D10BinaryShader, NiD3D10Shader);

//---------------------------------------------------------------------------
NiD3D10BinaryShader::NiD3D10BinaryShader() :
    NiD3D10Shader(),
    m_uiBonesPerPartition(0), 
    m_uiBoneMatrixRegisters(0), 
    m_eBinormalTangentMethod(NiShaderRequirementDesc::NBT_METHOD_NONE),
    m_uiBinormalTangentUVSource(NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT),
    m_eBoneCalcMethod(BONECALC_SKIN),
    m_uiPassCount(0)
{
    for (unsigned int ui = 0; ui < m_kPasses.GetAllocatedSize(); ui++)
        m_kPasses.SetAt(ui, 0);
}
//---------------------------------------------------------------------------
NiD3D10BinaryShader::~NiD3D10BinaryShader()
{
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::Initialize()
{
    if (m_bInitialized)
        return true;

    // Cycle through the passes and load the shader programs.  That step
    // should be all that's needed at this point.
    NiD3D10Pass* pkPass;
    unsigned int uiCount = m_kPasses.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
        {
            if (!LoadVertexShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
            if (!LoadGeometryShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
            if (!LoadPixelShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
        }
    }

    // All bone matrices will be transposed.
    SetBoneParameters(true, GetBoneMatrixRegisters(),
        GetBoneCalcMethod() == BONECALC_NO_SKIN);
    
    if (!NiD3D10Shader::Initialize())
        return false;

    m_bInitialized = true;
    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10BinaryShader::UpdatePipeline(
    const NiRenderCallContext& kRCC)
{
    for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPass);
        if (!pkPass)
        {
            continue;
        }

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();

        const unsigned int uiSamplerCount = pkPass->GetActiveSamplerCount();
        NIASSERT(uiSamplerCount == pkPass->GetActiveTextureCount());

        for (unsigned int i = 0; i < uiSamplerCount; i++)
        {
            PrepareTexture(kRCC, i, pkPass, pkTexProp);
        }
    }

    return NiD3D10Shader::UpdatePipeline(kRCC);
}
//---------------------------------------------------------------------------
void NiD3D10BinaryShader::PrepareTexture(
    const NiRenderCallContext& kRCC,
    unsigned int uiTexture,
    NiD3D10Pass* pkPass,
#ifdef NIDEBUG
    const NiTexturingProperty* pkTexProp)
#else
    const NiTexturingProperty*)
#endif
{
    NIASSERT(pkPass != 0 && uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);

    // The provided NiTexturingProperty had better match what's in the 
    // property state object.
    NIASSERT(kRCC.m_pkState != NULL &&
        kRCC.m_pkState->GetTexturing() == pkTexProp);

    // For the time being, assume that the texture ID and the sampler ID
    // correspond directly.

    NiTexture* pkTexture = NULL;
    NiTexturingProperty::ClampMode eClampMode =
        NiTexturingProperty::CLAMP_S_CLAMP_T;
    NiTexturingProperty::FilterMode eFilterMode =
        NiTexturingProperty::FILTER_NEAREST;
    unsigned short usMaxAnisotropy = 1;

    NiUInt32 uiGBMapFlags = pkPass->GetGamebryoMapFlags(uiTexture);
    NiUInt16 uiObjectTextureFlags = pkPass->GetObjectTextureFlags(uiTexture);

    ObtainTexture(uiGBMapFlags, 
        uiObjectTextureFlags, 
        kRCC, 
        pkTexture, 
        eClampMode, 
        eFilterMode,
        usMaxAnisotropy);

    // Even if the texture is NULL, we need to set that here to prevent a 
    // stale texture from getting used by accident.
    pkPass->SetPixelTexture(uiTexture, pkTexture);

    NiD3D10RenderStateGroup* pkRSGroup = pkPass->GetRenderStateGroup();
    if (pkRSGroup == NULL)
    {
        pkRSGroup = NiNew NiD3D10RenderStateGroup;
        pkPass->SetRenderStateGroup(pkRSGroup);
    }

    // Check to see if RenderStateGroup already defines these parameters
    // before filling in the version from the texturing property.
    D3D10_FILTER eTempFilter;
    if (pkRSGroup->GetSamplerFilter(NiGPUProgram::PROGRAM_PIXEL, uiTexture, 
        eTempFilter) == false)
    {
        pkRSGroup->SetSamplerFilter(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
            NiD3D10RenderStateManager::ConvertGbFilterModeToD3D10Filter(
            eFilterMode));

        pkRSGroup->SetSamplerMaxAnisotropy(NiGPUProgram::PROGRAM_PIXEL, uiTexture, usMaxAnisotropy);

        bool bMipmapEnable = 
            NiD3D10RenderStateManager::ConvertGbFilterModeToMipmapEnable(
            eFilterMode);

        float fMaxLOD = (bMipmapEnable ? D3D10_FLOAT32_MAX : 0.0f);
        pkRSGroup->SetSamplerMaxLOD(NiGPUProgram::PROGRAM_PIXEL, uiTexture, 
            fMaxLOD);
    }

    D3D10_TEXTURE_ADDRESS_MODE eTempAddress;
    if (pkRSGroup->GetSamplerAddressU(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
        eTempAddress) == false)
    {
        pkRSGroup->SetSamplerAddressU(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
            NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressU(
            eClampMode));
    }

    if (pkRSGroup->GetSamplerAddressV(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
        eTempAddress) == false)
    {
        pkRSGroup->SetSamplerAddressV(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
            NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressV(
            eClampMode));
    }
}
//---------------------------------------------------------------------------


bool NiD3D10BinaryShader::SetupGeometry(NiRenderObject* pkMesh, 
    NiMaterialInstance* pkMaterialInstance)
{
    if (!pkMesh)
        return false;

    //-----------------------------------------------------------------------

    // Add a 'dummy' alpha property if one is needed.

    NiAlphaProperty* pkAlphaProperty = NULL;

    // Check the render state group for any alpha blend among the render
    // targets.
    if (m_spRenderStateGroup)
    {
        bool bAlphaBlendEnabled = false;
        for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
            i++)
        {
            bool bBlendEnable = false;
            if (m_spRenderStateGroup->GetBSBlendEnable(i, bBlendEnable))
            {
                if (bBlendEnable)
                {
                    bAlphaBlendEnabled = true;
                    break;
                }
            }
        }

        // If it's enabled, set it in the property.
        if (bAlphaBlendEnabled)
        {
            if (!pkAlphaProperty)
            {
                pkAlphaProperty = NiNew NiAlphaProperty();
                NIASSERT(pkAlphaProperty);
            }
            pkAlphaProperty->SetAlphaBlending(true);
        }
    }

    // Check the first pass afterwards, as it can override the global
    NiD3D10Pass* pkPass = m_kPasses.GetAt(0);
    if (pkPass)
    {
        bool bAlphaBlendEnabled = false;
        for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
            i++)
        {
            bool bBlendEnable = false;
            NiD3D10RenderStateGroup* pkRSGroup
                = pkPass->GetRenderStateGroup();
            if (pkRSGroup && pkRSGroup->GetBSBlendEnable(i, bBlendEnable))
            {
                if (bBlendEnable)
                {
                    bAlphaBlendEnabled = true;
                    break;
                }
            }
        }

        // If it's enabled, set it in the property.
        if (bAlphaBlendEnabled)
        {
            if (!pkAlphaProperty)
            {
                pkAlphaProperty = NiNew NiAlphaProperty();
                NIASSERT(pkAlphaProperty);
            }
            pkAlphaProperty->SetAlphaBlending(true);
        }
    }

    if (pkAlphaProperty)
    {
        NiAlphaProperty* pkAttached =
            (NiAlphaProperty*)pkMesh->GetProperty(NiProperty::ALPHA);
        if (pkAttached)
        {
            if (!pkAttached->GetAlphaBlending() &&
                !pkAttached->GetAlphaTesting())
            {
                pkMesh->RemoveProperty(NiProperty::ALPHA);
                pkMesh->AttachProperty(pkAlphaProperty);
            }
            else
            {
                NiDelete pkAlphaProperty;
            }
        }
        else
        {
            pkMesh->AttachProperty(pkAlphaProperty);
        }
    }

    //-----------------------------------------------------------------------

    // See if there is a 'time' attribute required.  Cycle over each pass and
    // check the contant maps for occurrences of one of the time-based
    // pre-defines.  If any of the vertex, geometry, or pixel shader constant
    // maps uses time, then time is required.

    bool bTimeRequired = false;

    // First, check the 'global' attributes (constant maps per shader).
    for (unsigned int i = 0; i < GetVertexConstantMapCount(); i++)
    {
        if (IsTimeBased(GetVertexConstantMap(i)))
        {
            bTimeRequired = true;
            break;
        }
    }
    if (!bTimeRequired)
    {
        for (unsigned int i = 0; i < GetGeometryConstantMapCount(); i++)
        {
            if (IsTimeBased(GetGeometryConstantMap(i)))
            {
                bTimeRequired = true;
                break;
            }
        }
    }
    if (!bTimeRequired)
    {
        for (unsigned int i = 0; i < GetPixelConstantMapCount(); i++)
        {
            if (IsTimeBased(GetPixelConstantMap(i)))
            {
                bTimeRequired = true;
                break;
            }
        }
    }

    if (!bTimeRequired) // Next, check each pass.
    {
        for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
        {
            pkPass = m_kPasses.GetAt(uiPass);
            if (pkPass)
            {
                // Check each pass for vertex, geometry, and pixel constant
                // maps.
                for (unsigned int i = 0;
                    i < pkPass->GetVertexConstantMapCount(); i++)
                {
                    if (IsTimeBased(pkPass->GetVertexConstantMap(i)))
                    {
                        bTimeRequired = true;
                        break;
                    }
                }
                for (unsigned int i = 0;
                    i < pkPass->GetGeometryConstantMapCount(); i++)
                {
                    if (IsTimeBased(pkPass->GetGeometryConstantMap(i)))
                    {
                        bTimeRequired = true;
                        break;
                    }
                }
                for (unsigned int i = 0;
                    i < pkPass->GetPixelConstantMapCount(); i++)
                {
                    if (IsTimeBased(pkPass->GetPixelConstantMap(i)))
                    {
                        bTimeRequired = true;
                        break;
                    }
                }
            }
            if (bTimeRequired)
                break;
        }
    }

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
            pkObject = (NiAVObject*)(pkObject->GetParent());
        }
    }

    return NiD3D10Shader::SetupGeometry(pkMesh, pkMaterialInstance);
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::IsTimeBased(NiD3D10ShaderConstantMap* pkMap)
{
    if (pkMap)
    {
        if (pkMap->GetEntry("time") || pkMap->GetEntry("sin_time") ||
            pkMap->GetEntry("cos_time") || pkMap->GetEntry("tan_time") ||
            pkMap->GetEntry("time_sin_cos_tan"))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::LoadVertexShaderProgram(NiD3D10Pass& kPass)
{
    const char* pcProgramFileName = kPass.GetVertexShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if ((pcProgramFileName == NULL) || (*pcProgramFileName == '\0'))
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolveVertexShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetVertexShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    unsigned int uiFlags = 0;
    NiFixedString kShaderTarget = kPass.GetVertexShaderProgramShaderTarget();
    if (kShaderTarget == "vs_1_1" ||
        kShaderTarget == "vs_2_0" ||
        kShaderTarget == "vs_2_a" ||
        kShaderTarget == "vs_3_0")
    {
        uiFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
        kShaderTarget = "vs_4_0";
    }

    NiD3D10VertexShaderPtr spVertexShader;
    bool bSuccess = NiD3D10ShaderProgramFactory::CreateVertexShaderFromFile(
        acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget,
        uiFlags, acShaderName, spVertexShader);

    if (!bSuccess)
        return false;

    kPass.SetVertexShader(spVertexShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::LoadGeometryShaderProgram(NiD3D10Pass& kPass)
{
    const char* pcProgramFileName = kPass.GetGeometryShaderProgramFileName();
    unsigned int uiFlags = 0;
    unsigned int uiStreamOutTargetCount = 
        kPass.GetStreamOutSettings().GetStreamOutTargetCount();
    unsigned int uiOutputStreamDescCount = 
        m_kOutputStreamDescriptors.GetSize();
        
    NiD3D10GeometryShaderPtr spGeometryShader = NULL;
    bool bSuccess = false;

    if ((pcProgramFileName == NULL || *pcProgramFileName == '\0') && 
        uiStreamOutTargetCount == 0)
    {
        // If not GS was specified and there is no stream out, 
        // it's not a failure.
        // Just return true!
        return true;
    }

    // Prepare stream output declaration, if we need one.
    D3D10_SO_DECLARATION_ENTRY pkSODecl[
        D3D10_SO_SINGLE_BUFFER_COMPONENT_LIMIT];
    unsigned int uiNumEntries = 0;
    unsigned int uiOutputStreamStride = 0;
    if (uiStreamOutTargetCount > 0)
    {
        // Go through the streams available and find the one(s) that
        // this pass wants, by name.  Then we'll use info about these
        // stream(s) to fill out pkSODecl[].
        
        for (unsigned int i = 0; i < uiStreamOutTargetCount; i++)
        {
            // first, find the outputstream on the pass, by name.
            const NiFixedString& kNameToFind = kPass.GetStreamOutSettings().
                GetStreamOutTarget(i);
            bool bFound = false;
            for (unsigned int j = 0; j < uiOutputStreamDescCount; j++)
            {
                NiOutputStreamDescriptor& kDesc = 
                    m_kOutputStreamDescriptors[j];
                const NiFixedString& kName = kDesc.GetName();
                if (kNameToFind == kName)
                {
                    // MATCH FOUND
                    bFound = true;
                    const NiOutputStreamDescriptor::VertexFormat& 
                        kVertexFormat = kDesc.GetVertexFormat();
                    unsigned int uiTableEntries = kVertexFormat.GetSize();
                    if (uiStreamOutTargetCount == 1)
                    {
                        // streaming out to one "fat" buffer 
                        for (unsigned int k = 0; k < uiTableEntries; k++)
                        {
                            const NiOutputStreamDescriptor::VertexFormatEntry& 
                                kEntry = kVertexFormat[k];
                            pkSODecl[uiNumEntries].OutputSlot = 0;
                            pkSODecl[uiNumEntries].StartComponent = 0;
                            pkSODecl[uiNumEntries].ComponentCount = 
                                (BYTE)kEntry.m_uiComponentCount;
                            pkSODecl[uiNumEntries].SemanticName = 
                                kEntry.m_kSemanticName;
                            pkSODecl[uiNumEntries].SemanticIndex = 
                                kEntry.m_uiSemanticIndex;
                            
                            uiNumEntries++;
                            uiOutputStreamStride += 
                                kEntry.m_uiComponentCount * 4;
                        }
                        if (uiOutputStreamStride > 256)
                        {
                            NiD3D10ShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                false, "* ERROR:  "
                                "NiD3D10BinaryShader::"
                                "LoadGeometryShaderProgram\n"
                                " When streaming out to one large streams,"
                                " D3D10 can only handle a maximum of "
                                "%d scalar\n"
                                " components (256 bytes) per vertex.\n"
                                " Shader %s, pass %s\n", 
                                D3D10_SO_SINGLE_BUFFER_COMPONENT_LIMIT,
                                GetName(), kPass.GetName());
                            return false;
                        }
                    }
                    else
                    {
                        // streaming out to 1-4 "thin" buffers (each one must
                        // be float4 or less)
                        const NiOutputStreamDescriptor::VertexFormatEntry& 
                            kEntry = kVertexFormat[0];
                        pkSODecl[uiNumEntries].OutputSlot = (BYTE)i;
                        pkSODecl[uiNumEntries].StartComponent = 0;
                        pkSODecl[uiNumEntries].ComponentCount = 
                            (BYTE)kEntry.m_uiComponentCount;
                        pkSODecl[uiNumEntries].SemanticName = 
                            kEntry.m_kSemanticName;
                        pkSODecl[uiNumEntries].SemanticIndex = 
                            kEntry.m_uiSemanticIndex;
                        
                        uiNumEntries++;
                        uiOutputStreamStride += 
                            kEntry.m_uiComponentCount * 4;

                        if (uiTableEntries != 1) 
                        {
                            NiD3D10ShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, false, "* ERROR:  "
                                "NiD3D10BinaryShader::"
                                "LoadGeometryShaderProgram\n"
                                " When streaming out to multiple streams,"
                                " D3D10 mandates that each of the streams\n"
                                " have 4 elements (for example, 4 floats)"
                                " or less.\n"
                                " Shader %s, pass %s\n", 
                                GetName(), kPass.GetName());
                            return false;
                        }
                    }
                    break;
                }
            }
            if (!bFound)
            {
                NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    false, "* ERROR:  "
                    "NiD3D10BinaryShader::LoadGeometryShaderProgram\nFailed"
                    " to find an OutputStreamDescriptor for a stream of the"
                    " name %s.\nShader %s, pass %s\n", 
                    kNameToFind, GetName(), kPass.GetName());
                return false;
            }
        }
    }

    if (pcProgramFileName == NULL || *pcProgramFileName == '\0')
    {
        // They want stream out, without a GS - so we need to call
        // ID3D10Device::CreateGeometryShaderWithStreamOutput() using the
        // *vertex shader's* shader Bytecode.

        NIASSERT(uiStreamOutTargetCount > 0);

        NiFixedString kShaderTarget = "gs_4_0";
        char acShaderName[] = "_gsless_stream_output_";

        const char* pcVertexProgramFileName = 
            kPass.GetVertexShaderProgramFileName();
        char acTrueVertexFileName[_MAX_PATH];
        if (!ResolveVertexShaderFileName(pcVertexProgramFileName, 
            acTrueVertexFileName, _MAX_PATH))
        {
            return false;
        }

        const char* pcVertexShaderEntryPoint = 
            kPass.GetVertexShaderProgramEntryPoint();

        bSuccess = NiD3D10ShaderProgramFactory::
            CreateGeometryShaderWithStreamOutputFromCompiledBlob(
            kPass.GetVertexShader()->GetShaderByteCode(), acTrueVertexFileName,
            NULL, NULL, pcVertexShaderEntryPoint, kShaderTarget, uiFlags, 
            acShaderName, pkSODecl, uiNumEntries, uiOutputStreamStride, 
            spGeometryShader);
    }
    else
    {
        // Compile a real (existing) geometry shader - either with or without
        // stream output.

        char acTrueFileName[_MAX_PATH];
        if (!ResolveGeometryShaderFileName(pcProgramFileName, acTrueFileName,
            _MAX_PATH))
        {
            return false;
        }

        // Construct a shader name from the shader file and entry point
        const char* pcShaderEntryPoint = 
            kPass.GetGeometryShaderProgramEntryPoint();
        char acShaderName[_MAX_PATH];
        if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
        {
            NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
                < _MAX_PATH);
            NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
                pcShaderEntryPoint);
        }
        else
        {
            NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
        }

        NiFixedString kShaderTarget = 
            kPass.GetGeometryShaderProgramShaderTarget();
        // No need to check for backwards compatibility for geometry shaders.

        if (uiStreamOutTargetCount == 0)
        {
            // no stream output
            bSuccess = 
                NiD3D10ShaderProgramFactory::CreateGeometryShaderFromFile(
                acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget,
                uiFlags, acShaderName, spGeometryShader);
        }
        else
        {
            // stream output case
            bSuccess = NiD3D10ShaderProgramFactory::
                CreateGeometryShaderWithStreamOutputFromFile(
                acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget,
                uiFlags, acShaderName, pkSODecl, uiNumEntries, 
                uiOutputStreamStride, spGeometryShader);
        }
    }

    if (!bSuccess)
        return false;

    kPass.SetGeometryShader(spGeometryShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::LoadPixelShaderProgram(NiD3D10Pass& kPass)
{
    const char* pcProgramFileName = kPass.GetPixelShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (pcProgramFileName == NULL || *pcProgramFileName == '\0')
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolvePixelShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetPixelShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT (strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    unsigned int uiFlags = 0;
    NiFixedString kShaderTarget = kPass.GetPixelShaderProgramShaderTarget();
    if (kShaderTarget == "ps_1_1" ||
        kShaderTarget == "ps_1_2" ||
        kShaderTarget == "ps_1_3" ||
        kShaderTarget == "ps_1_4" ||
        kShaderTarget == "ps_2_0" ||
        kShaderTarget == "ps_2_a" ||
        kShaderTarget == "ps_2_b" ||
        kShaderTarget == "ps_3_0")
    {
        uiFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
        kShaderTarget = "ps_4_0";
    }

    NiD3D10PixelShaderPtr spPixelShader;
    bool bSuccess = NiD3D10ShaderProgramFactory::CreatePixelShaderFromFile(
        acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget, 
        uiFlags, acShaderName, spPixelShader);

    if (!bSuccess)
        return false;

    kPass.SetPixelShader(spPixelShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::ResolveVertexShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NSBD3D10Utility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.vsh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::ResolveGeometryShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NSBD3D10Utility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName,
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.gsh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::ResolvePixelShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NSBD3D10Utility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName,
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.psh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
