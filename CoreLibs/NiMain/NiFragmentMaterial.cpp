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
#include "NiMainPCH.h"

#include "NiFragmentMaterial.h"
#include "NiFragmentShaderInstanceDescriptor.h"
#include "NiMaterialNodeLibrary.h"
#include "NiTexturingProperty.h"
#include "NiVertexColorProperty.h"
#include "NiFogProperty.h"
#include "NiSpecularProperty.h"
#include "NiMaterialProperty.h"
#include "NiAlphaProperty.h"
#include "NiTextureEffect.h"
#include "NiBitfield.h"
#include "NiShaderFactory.h"
#include "NiRenderer.h"
#include "NiGPUProgram.h"

NiImplementRTTI(NiFragmentMaterial, NiMaterial);

bool NiFragmentMaterial::ms_bLoadProgramCacheOnCreation = true;
bool NiFragmentMaterial::ms_bAutoWriteProgramCache = true;
bool NiFragmentMaterial::ms_bWriteDebugProgram = true;
bool NiFragmentMaterial::ms_bLockProgramCache = false;
bool NiFragmentMaterial::ms_bAutoCreateCache = true;

//---------------------------------------------------------------------------
bool NiFragmentMaterial::GetDefaultLoadProgramCacheOnCreation()
{
    return ms_bLoadProgramCacheOnCreation;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GetDefaultAutoSaveProgramCache()
{
    return ms_bAutoWriteProgramCache;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GetDefaultWriteDebugProgramData()
{
    return ms_bWriteDebugProgram;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GetDefaultLockProgramCache()
{
    return ms_bLockProgramCache;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GetDefaultAutoCreateProgramCache()
{
    return ms_bAutoCreateCache;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetDefaultLoadProgramCacheOnCreation(bool bSet)
{
    ms_bLoadProgramCacheOnCreation = bSet;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetDefaultAutoSaveProgramCache(bool bSet)
{
    ms_bAutoWriteProgramCache = bSet;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetDefaultWriteDebugProgramData(bool bSet)
{
    ms_bWriteDebugProgram = bSet;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetDefaultLockProgramCache(bool bSet)
{
    ms_bLockProgramCache = bSet;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetDefaultAutoCreateProgramCache(bool bSet)
{
    ms_bAutoCreateCache = bSet;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::RenderPassDescriptor::RenderPassDescriptor() :
    m_bUsesNiRenderState(false),
    m_bAlphaOverride(false),
    m_bAlphaBlend(false),
    m_bResetObjectOffsets(false),
    m_bUsePreviousSrcBlendMode(false),
    m_bUsePreviousDestBlendMode(false),
    m_eSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA),
    m_eDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA)
{
    /* */
}
//---------------------------------------------------------------------------
NiFragmentMaterial::RenderPassDescriptor::RenderPassDescriptor(
    RenderPassDescriptor& kOther) : 
    m_kVertexDesc(kOther.m_kVertexDesc),
    m_kGeometryDesc(kOther.m_kGeometryDesc),
    m_kPixelDesc(kOther.m_kPixelDesc),
    m_bUsesNiRenderState(kOther.m_bUsesNiRenderState),
    m_bAlphaOverride(kOther.m_bAlphaOverride),
    m_bAlphaBlend(kOther.m_bAlphaBlend),
    m_bResetObjectOffsets(kOther.m_bResetObjectOffsets),
    m_kObjectOffsets(kOther.m_kObjectOffsets.GetSize()),
    m_bUsePreviousSrcBlendMode(kOther.m_bUsePreviousSrcBlendMode),
    m_bUsePreviousDestBlendMode(kOther.m_bUsePreviousDestBlendMode),
    m_eSrcBlendMode(kOther.m_eSrcBlendMode),     
    m_eDestBlendMode(kOther.m_eDestBlendMode)
{
    for (unsigned int ui = 0; ui < kOther.m_kObjectOffsets.GetSize();ui++)
    {
        m_kObjectOffsets.Add(kOther.m_kObjectOffsets.GetAt(ui));    
    }
}
//---------------------------------------------------------------------------
NiFragmentMaterial::RenderPassDescriptor& 
    NiFragmentMaterial::RenderPassDescriptor::operator=(
    RenderPassDescriptor& kOther)
{
    m_kVertexDesc           =   kOther.m_kVertexDesc;
    m_kGeometryDesc         =   kOther.m_kGeometryDesc;      
    m_kPixelDesc            =   kOther.m_kPixelDesc;      
    m_bUsesNiRenderState    =   kOther.m_bUsesNiRenderState;
    m_bAlphaOverride        =   kOther.m_bAlphaOverride;  
    m_bAlphaBlend           =   kOther.m_bAlphaBlend;       
    m_bResetObjectOffsets   =   kOther.m_bResetObjectOffsets;       
    m_eSrcBlendMode         =   kOther.m_eSrcBlendMode;     
    m_eDestBlendMode        =   kOther.m_eDestBlendMode; 
    m_bUsePreviousSrcBlendMode = kOther.m_bUsePreviousSrcBlendMode;
    m_bUsePreviousDestBlendMode = kOther.m_bUsePreviousDestBlendMode;

    m_kObjectOffsets.RemoveAll();
    for (unsigned int ui = 0; ui < kOther.m_kObjectOffsets.GetSize();ui++)
    {
        m_kObjectOffsets.Add(kOther.m_kObjectOffsets.GetAt(ui));    
    }

    return *(this);
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::RenderPassDescriptor::Initialize()
{
    m_bUsesNiRenderState = false;
    m_bAlphaOverride = false;
    m_bAlphaBlend = false;
    m_bResetObjectOffsets = false;
    m_eSrcBlendMode = NiAlphaProperty::ALPHA_SRCALPHA;
    m_eDestBlendMode = NiAlphaProperty::ALPHA_INVSRCALPHA;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ObjectResourceOffset::ObjectResourceOffset() :
    m_eProgramType(NiGPUProgram::PROGRAM_MAX),
    m_eObjectType(NiShaderAttributeDesc::OT_UNDEFINED),
    m_uiOffset(0)
{ /* */ }
//---------------------------------------------------------------------------
NiFragmentMaterial::NiFragmentMaterial(NiMaterialNodeLibrary* pkLibrary,
    const NiFixedString& kName, unsigned short usVertexVersion, 
    unsigned short usGeometryVersion, unsigned short usPixelVersion, 
    bool bAutoCreateCaches) : 
    NiMaterial(kName), 
    m_uiMaxPassCount(10),
    m_usVertexVersion(usVertexVersion), 
    m_usGeometryVersion(usGeometryVersion),
    m_usPixelVersion(usPixelVersion)
{
    m_kLibraries.Add(pkLibrary);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (bAutoCreateCaches && pkRenderer != NULL)
    {
        pkRenderer->SetDefaultProgramCache(this);
    }
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType 
NiFragmentMaterial::GetAttributeTypeFromString(const char* pcValue)
{
    if (NiStricmp(pcValue, "float") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
    else if (NiStricmp(pcValue, "float2") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
    else if (NiStricmp(pcValue, "float3") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
    else if (NiStricmp(pcValue, "float4") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    else if (NiStricmp(pcValue, "float3x3") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
    else if (NiStricmp(pcValue, "float4x4") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
    else if (NiStricmp(pcValue, "sampler") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE;
    else if (NiStricmp(pcValue, "int") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
    else if (NiStricmp(pcValue, "uint") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
    else if (NiStricmp(pcValue, "bool") == 0)
        return NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
    else
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiFixedString NiFragmentMaterial::GetStringFromAttributeType(
    NiShaderAttributeDesc::AttributeType eType)
{
    if (NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT == eType)
        return "float";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_POINT2 == eType)
        return "float2";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_POINT3 == eType)
        return "float3";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_COLOR == eType)
        return "float4";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 == eType)
        return "float4";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3 == eType)
        // BONEMATRIX_TYPE will become either:
        //   float4x3 (4 columns, 3 rows in HLSL) or
        //   float3x4 (3 rows, 4 columns in PS3 Cg)
        return "BONEMATRIX_TYPE";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4 == eType)
        return "float4x4";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE == eType)
        return "sampler";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT == eType)
        return "int";
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_BOOL == eType)
        return "bool";

    return NULL;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetProgramCache(NiGPUProgramCache* pkCache, 
    NiGPUProgram::ProgramType eType)
{
    if (m_aspProgramCaches[eType] != pkCache)
    {
        for (unsigned int ui = 0; ui < MATERIAL_HASH_BUCKET_COUNT; ui++)
            m_akShaderHashTable[ui].RemoveAll();
    }

    m_aspProgramCaches[eType] = pkCache;
}
//---------------------------------------------------------------------------
NiGPUProgramCache* NiFragmentMaterial::GetProgramCache(
    NiGPUProgram::ProgramType eType) const
{
    return m_aspProgramCaches[eType];
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::LoadAllCaches()
{
    for (unsigned int ui = 0; ui < NiGPUProgram::PROGRAM_MAX; ui++)
    {
        NiGPUProgramCache* pkCache = GetProgramCache(
            (NiGPUProgram::ProgramType)ui);

        if (!pkCache)
            continue;

        pkCache->Load();
    }
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetWorkingDirectory(const char* pcWorkingDir)
{
    for (unsigned int ui = 0; ui < NiGPUProgram::PROGRAM_MAX; ui++)
    {
        NiGPUProgramCache* pkCache = GetProgramCache(
            (NiGPUProgram::ProgramType)ui);

        if (!pkCache)
            continue;

        NIVERIFY(pkCache->SetWorkingDirectory(pcWorkingDir));
    }
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetProgramVersion(
    NiGPUProgram::ProgramType eType) const
{
    unsigned int uiVersion = 0;
    
    switch (eType)
    {
        case NiGPUProgram::PROGRAM_VERTEX:
            uiVersion |= m_usVertexVersion << 16;
            break;
        case NiGPUProgram::PROGRAM_GEOMETRY:
            uiVersion |= m_usGeometryVersion << 16;
            break;
        case NiGPUProgram::PROGRAM_PIXEL:
            uiVersion |= m_usPixelVersion << 16;
            break;
        default:
            NIASSERT(!"Invalid program type");
            break;
    }

    for (unsigned int ui = 0; ui < m_kLibraries.GetSize(); ui++)
    {
        NiMaterialNodeLibrary* pkLib = m_kLibraries.GetAt(ui);
        if (pkLib)
        {
            uiVersion += pkLib->GetVersion();
        }
    }
    return uiVersion;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::IsShaderCurrent(NiShader* pkShader, 
    const NiRenderObject* pkGeometry,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    unsigned int)
{
    if (pkShader == NULL)
    {
        return false;
    }
    else
    {
        NiMaterialDescriptor kDesc;
        NIVERIFY(GenerateDescriptor(pkGeometry, pkState, pkEffects, kDesc));

        const NiFragmentShaderInstanceDescriptor * pkShaderFragDesc = 
            NiDynamicCast(NiFragmentShaderInstanceDescriptor,
            pkShader->GetShaderInstanceDesc());

        if (pkShaderFragDesc && pkShaderFragDesc->m_kMatDesc.IsEqual(&kDesc))
            return true;

        return false;
    }
}
//---------------------------------------------------------------------------
NiShader* NiFragmentMaterial::GetCurrentShader(
    const NiRenderObject* pkGeometry, 
    const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects,
    unsigned int)
{   
    NiMaterialDescriptor kDesc;
    bool bSuccess = GenerateDescriptor(pkGeometry, pkState, pkEffects, kDesc);

    if (!bSuccess)
        return NULL;

#if defined(NIDEBUG)
    m_kDebugIdentifier = pkGeometry->GetName();
#endif

    NiShader* pkShader = GenerateShader(kDesc);
    return pkShader;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::PrecacheGPUPrograms(const NiRenderObject* pkGeometry, 
    const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects, 
    unsigned int)
{   
    NiMaterialDescriptor kDesc;
    bool bSuccess = GenerateDescriptor(pkGeometry, pkState, pkEffects, kDesc);

    if (!bSuccess)
        return NULL;

#if defined(NIDEBUG)
    m_kDebugIdentifier = pkGeometry->GetName();
#endif

    return PrecacheGPUPrograms(kDesc);
}
//---------------------------------------------------------------------------
NiGPUProgram* NiFragmentMaterial::EvaluateProgram(Context& kContext,
        const char* pcFilename, NiGPUProgram::ProgramType eProgType, 
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    NiGPUProgramCache* pkCache = m_aspProgramCaches[eProgType];
    NIASSERT(pkCache);
    NiString strFile = kContext.m_spConfigurator->Evaluate(pcFilename, 
        eProgType, kUniforms);
    NiGPUProgram* pkProgram = pkCache->GenerateProgram(pcFilename,  
        (const char*) strFile, kUniforms);

    return pkProgram;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::UnloadShaders()
{
    for (unsigned int ui = 0; ui < MATERIAL_HASH_BUCKET_COUNT; ui++)
        m_akShaderHashTable[ui].RemoveAll();

    for (unsigned int ui = 0; ui < NiGPUProgram::PROGRAM_MAX; ui++)
    {
        if (m_aspProgramCaches[ui])
           m_aspProgramCaches[ui]->Clear();
    }
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetHashCode(
    const NiMaterialDescriptor* pkDescriptor)
{
    unsigned int uiHash = 0;
    for (unsigned int ui = 0; ui < NiMaterialDescriptor::BIT_ARRAY_SIZE;
        ui++)
    {
        uiHash = (uiHash << 5) + uiHash + pkDescriptor->m_auiBitArray[ui];
    }
    
    return uiHash % MATERIAL_HASH_BUCKET_COUNT;
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetHashCode(
    const NiGPUProgramDescriptor* pkDescriptor)
{
    unsigned int uiHash = 0;
    for (unsigned int ui = 0; ui < NiGPUProgramDescriptor::BIT_ARRAY_SIZE;
        ui++)
    {
        uiHash = (uiHash << 5) + uiHash + pkDescriptor->m_auiBitArray[ui];
    }
    
    return uiHash % MATERIAL_HASH_BUCKET_COUNT;
}
//---------------------------------------------------------------------------
NiShader* NiFragmentMaterial::RetrieveFromHash(
    const NiMaterialDescriptor* pkDescriptor)
{
    unsigned int uiBucket = GetHashCode(pkDescriptor);

    for (unsigned int ui = 0; ui < m_akShaderHashTable[uiBucket].GetSize();
        ui++)
    {
        NiShader* pkShader = m_akShaderHashTable[uiBucket].GetAt(ui);
        NIASSERT(pkShader);

        const NiFragmentShaderInstanceDescriptor* pkShaderFragDesc = 
            NiDynamicCast(NiFragmentShaderInstanceDescriptor,
            pkShader->GetShaderInstanceDesc());
        NIASSERT(pkShaderFragDesc != NULL);

        if (pkShaderFragDesc->m_kMatDesc.IsEqual(pkDescriptor))
        {
            return pkShader;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::InsertIntoHash(NiShader* pkShader)
{
    const NiFragmentShaderInstanceDescriptor* pkShaderFragDesc = 
        NiDynamicCast(NiFragmentShaderInstanceDescriptor,
        pkShader->GetShaderInstanceDesc());
    NIASSERT(pkShaderFragDesc != NULL);

    NIASSERT(NULL == RetrieveFromHash(&pkShaderFragDesc->m_kMatDesc));
    unsigned int uiBucket = GetHashCode(&pkShaderFragDesc->m_kMatDesc);
    m_akShaderHashTable[uiBucket].Add(pkShader);
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::RemoveFromHash(NiShader* pkShader)
{
    const NiFragmentShaderInstanceDescriptor * pkShaderFragDesc = 
        NiDynamicCast(NiFragmentShaderInstanceDescriptor,
        pkShader->GetShaderInstanceDesc());
    NIASSERT(pkShaderFragDesc != NULL);

    unsigned int uiBucket = GetHashCode(&pkShaderFragDesc->m_kMatDesc);
    int iElement = m_akShaderHashTable[uiBucket].Find(pkShader);
    NIASSERT(iElement != -1);
    m_akShaderHashTable[uiBucket].RemoveAt(iElement);
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::CheckFailedShader(
    const NiMaterialDescriptor* pkDescriptor)
{
    unsigned int uiBucket = GetHashCode(pkDescriptor);

    for (unsigned int ui = 0; 
        ui < m_akFailedShaderDescriptorHashTable[uiBucket].GetSize(); ui++)
    {
        const NiMaterialDescriptor& kFailedDescriptor = 
            m_akFailedShaderDescriptorHashTable[uiBucket].GetAt(ui);

        if (kFailedDescriptor.IsEqual(pkDescriptor))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetFailedShader(
    const NiMaterialDescriptor* pkDescriptor)
{
    NIASSERT(false == CheckFailedShader(pkDescriptor));

    unsigned int uiBucket = GetHashCode(pkDescriptor);
    m_akFailedShaderDescriptorHashTable[uiBucket].Add(*pkDescriptor);
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetFailedGPUProgram(
    NiGPUProgram::ProgramType eProgramType,
    const NiGPUProgramDescriptor* pkDescriptor)
{
    NIASSERT(eProgramType < NiGPUProgram::PROGRAM_MAX);
    char acFilename[NI_MAX_PATH];
    NIVERIFY(pkDescriptor->GenerateKey(acFilename, 48));

    if (!m_aspProgramCaches[eProgramType])
        return;

    m_aspProgramCaches[eProgramType]->AddFailedProgram(acFilename);
}
//---------------------------------------------------------------------------
NiShader* NiFragmentMaterial::GenerateShader(
    NiMaterialDescriptor& kMaterialDescriptor)
{
    NiShader* pkRetrievedShader = RetrieveFromHash(&kMaterialDescriptor);
    if (pkRetrievedShader)
        return pkRetrievedShader;

    bool bFailedShader = CheckFailedShader(&kMaterialDescriptor);
    if (bFailedShader)
        return NULL;

    unsigned int uiPassCount = 0;

    NIASSERT(m_uiMaxPassCount > 0);
    RenderPassDescriptor* pkPassDesc = 
        NiNew RenderPassDescriptor[m_uiMaxPassCount];

    NiShader* pkCurrentBestShader = NULL;
    unsigned int uiCurrentBestShaderPassCount = UINT_MAX;

    unsigned int uiNumFallbacks = m_kShaderFallbackFuncs.GetSize();
    unsigned int uiCurrentFallback = 0;

    SHADERFALLBACKFUNC pfnFallbackFunction = NULL;
    unsigned int uiFallbackFunctionData = 0;

    bool bFindShortestFallback = false;
    bool bKeepSearching = true;
    bool bNextFallback = true;
    bool bFallbackNeeded = false;

    while (bKeepSearching)
    {
        if (bNextFallback)
        {
            pfnFallbackFunction = NULL;
            while (uiCurrentFallback < uiNumFallbacks &&
                pfnFallbackFunction == NULL)
            {
                pfnFallbackFunction = m_kShaderFallbackFuncs.GetAt(
                    uiCurrentFallback++);
            }
            bNextFallback = false;
            uiFallbackFunctionData = 0;

            for (unsigned int i = 0; i < m_uiMaxPassCount; i++)
                pkPassDesc[i].Initialize();

            // Re-generate original descriptor to re-attempt fallbacks
            if (RC_SUCCESS != GenerateShaderDescArray(&kMaterialDescriptor, 
                pkPassDesc, m_uiMaxPassCount, uiPassCount))
            {
                SetFailedShader(&kMaterialDescriptor);
                NiDelete[] pkPassDesc;
                return NULL;
            }
        }

        // Check to see if we need a fallback technique but there are no more
        // registered
        if (bFallbackNeeded && pfnFallbackFunction == NULL)
        {
            break;
        }

        if (uiPassCount == 0)
        {
            SetFailedShader(&kMaterialDescriptor);
            NiDelete[] pkPassDesc;
            return NULL;
        }

        NiShader* pkShader = CreateShader(&kMaterialDescriptor);
        if (!pkShader)
        {
            SetFailedShader(&kMaterialDescriptor);
            NiDelete[] pkPassDesc;
            return NULL;
        }

        if (!SetupPackingRequirements(pkShader, &kMaterialDescriptor, 
            pkPassDesc, uiPassCount))
        {
            NiDelete pkShader;
            SetFailedShader(&kMaterialDescriptor);
            NiDelete[] pkPassDesc;
            return NULL;
        }

        pkShader->SetUsesNiRenderState(pkPassDesc[0].m_bUsesNiRenderState);

        unsigned int uiPass = 0;
        for (unsigned int ui = 0; ui < uiPassCount; ui++)
        {
            if (pkShader->AppendRenderPass(uiPass))
            {
                if (pkPassDesc[ui].m_bAlphaOverride)
                {
                    if (!pkShader->SetAlphaOverride(uiPass, 
                        pkPassDesc[ui].m_bAlphaBlend, 
                        pkPassDesc[ui].m_bUsePreviousSrcBlendMode,
                        pkPassDesc[ui].m_eSrcBlendMode, 
                        pkPassDesc[ui].m_bUsePreviousDestBlendMode,
                        pkPassDesc[ui].m_eDestBlendMode))
                    {
                        NiDelete pkShader;
                        SetFailedShader(&kMaterialDescriptor);
                        NiDelete[] pkPassDesc;
                        return NULL;
                    }
                }

                ReturnCode eRC = GenerateShaderProgramsForPass(
                    &pkPassDesc[ui].m_kVertexDesc, 
                    &pkPassDesc[ui].m_kGeometryDesc, 
                    &pkPassDesc[ui].m_kPixelDesc, 
                    pkPassDesc[ui].m_kObjectOffsets, pkShader, uiPass);

                if (RC_SUCCESS != eRC)
                {
                    bFallbackNeeded = true;
                    NiDelete pkShader;
                    pkShader = NULL;

                    if (pfnFallbackFunction)
                    {
                        bool bSuccess = (*pfnFallbackFunction)(
                            &kMaterialDescriptor, eRC, uiPass, pkPassDesc,
                            m_uiMaxPassCount, uiPassCount, 
                            uiFallbackFunctionData);

                        if (!bSuccess)
                        {
                            bNextFallback = true;
                        }
                    }
                    break;  // exit the loop, we may need to regenerate
                            // previous passes
                }
            }
        }
        
        if (pkShader)
        {
            // At this point, we have successfully compiled all the passes
            if (bFallbackNeeded && bFindShortestFallback)
            {
                if (uiPassCount < uiCurrentBestShaderPassCount)
                {
                    NiDelete pkCurrentBestShader;
                    pkCurrentBestShader = pkShader;
                    pkShader = NULL;
                }
                bNextFallback = true;
            }
            else
            {
                pkCurrentBestShader = pkShader;
                bKeepSearching = false;
            }
        }
    }
    NiDelete[] pkPassDesc;

    if (pkCurrentBestShader == NULL)
    {
        SetFailedShader(&kMaterialDescriptor);
        return NULL;
    }

    if (!pkCurrentBestShader->IsInitialized())
    {
        NIVERIFY(pkCurrentBestShader->Initialize());
    }

    InsertIntoHash(pkCurrentBestShader);
    return pkCurrentBestShader;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::PrecacheGPUPrograms(
    NiMaterialDescriptor& kMaterialDescriptor)
{
    // Shader will be stored in hash; no need to explicitly delete
    NiShader* pkShader = GenerateShader(kMaterialDescriptor);
    return (pkShader != NULL);
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ReturnCode 
    NiFragmentMaterial::GenerateShaderProgramsForPass(
    NiGPUProgramDescriptor* pkVertexDesc, 
    NiGPUProgramDescriptor* pkGeometryDesc, 
    NiGPUProgramDescriptor* pkPixelDesc, 
    const NiTObjectSet<ObjectResourceOffset>& kObjectOffsets, 
    NiShader* pkShader, unsigned int uiRenderPass)
{
    if(!pkShader->IsGenericallyConfigurable())
        return RC_INVALID_SHADER;

    const unsigned int uiNumProgramTypes = NiGPUProgram::PROGRAM_MAX;
    NiGPUProgramPtr aspPrograms[uiNumProgramTypes];
    NiTObjectPtrSet<NiMaterialResourcePtr> kUniforms[uiNumProgramTypes];
    
    aspPrograms[NiGPUProgram::PROGRAM_VERTEX] = GenerateShaderProgram(
        NiGPUProgram::PROGRAM_VERTEX, pkVertexDesc, 
        kUniforms[NiGPUProgram::PROGRAM_VERTEX]);

    aspPrograms[NiGPUProgram::PROGRAM_GEOMETRY] = GenerateShaderProgram(
        NiGPUProgram::PROGRAM_GEOMETRY, pkGeometryDesc, 
        kUniforms[NiGPUProgram::PROGRAM_GEOMETRY]);

    aspPrograms[NiGPUProgram::PROGRAM_PIXEL] = GenerateShaderProgram(
        NiGPUProgram::PROGRAM_PIXEL, pkPixelDesc, 
        kUniforms[NiGPUProgram::PROGRAM_PIXEL]);

    unsigned int uiRC = VerifyShaderPrograms(
        aspPrograms[NiGPUProgram::PROGRAM_VERTEX],
        aspPrograms[NiGPUProgram::PROGRAM_GEOMETRY], 
        aspPrograms[NiGPUProgram::PROGRAM_PIXEL]);

    if (uiRC != 0)
        return (ReturnCode) uiRC;

    for (unsigned int uiProgram = 0; uiProgram < uiNumProgramTypes; 
        uiProgram++)
    {
        // NULL GPU programs can be checked by VerifyShaderPrograms.
        // Any NULL GPU programs at this point should be considered acceptable.
        if (aspPrograms[uiProgram] == NULL)
            continue;

        NiGPUProgram::ProgramType eProgramType = 
            aspPrograms[uiProgram]->GetProgramType();

        if (!pkShader->SetGPUProgram(uiRenderPass, aspPrograms[uiProgram],
            eProgramType))
        {
            return RC_INVALID_SHADER;
        }

        NiShaderConstantMap* pkConstantMap = pkShader->CreateShaderConstantMap(
            uiRenderPass, eProgramType);

        if (!pkConstantMap)
            return RC_INVALID_CONSTANTMAP;

        for (unsigned int ui = 0; ui < kUniforms[uiProgram].GetSize(); ui++)
        {
            NiMaterialResource* pkRes = kUniforms[uiProgram].GetAt(ui);

            if (!pkRes)
                continue;

            if (!AddResourceToShaderConstantMap(pkShader, pkConstantMap, 
                pkRes, pkVertexDesc, pkGeometryDesc, pkPixelDesc, 
                kObjectOffsets, uiRenderPass, eProgramType))
            {
                return RC_INVALID_CONSTANTMAP;
            }
        }
    }

    return RC_SUCCESS;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::AddResourceToShaderConstantMap(NiShader* pkShader,
    NiShaderConstantMap* pkConstantMap, NiMaterialResource* pkRes,
    NiGPUProgramDescriptor*, 
    NiGPUProgramDescriptor*, 
    NiGPUProgramDescriptor*, 
    const NiTObjectSet<ObjectResourceOffset>& kObjectOffsets, 
    unsigned int uiPass, NiGPUProgram::ProgramType eCurrentType)
{
    unsigned int uiCount = pkRes->GetCount();
    unsigned int uiExtra = 0;
    if (uiCount > 1)
    {
        uiExtra = (0 & ~0xffff0000) | (uiCount << 16); 
    }
   
    uiExtra += pkRes->GetExtraData();


    switch (pkRes->GetSource())
    {
    case NiMaterialResource::SOURCE_OBJECT:
        {
            unsigned int uiOffset = 0;
            for (unsigned int ui = 0; ui < kObjectOffsets.GetSize(); ui++)
            {
                const ObjectResourceOffset& kOffset = kObjectOffsets.GetAt(ui);
                if (kOffset.m_eProgramType == eCurrentType &&
                    kOffset.m_eObjectType == pkRes->GetObjectType())
                {
                    uiOffset = kOffset.m_uiOffset;
                    break;
                }
            }

            if (NISHADERERR_OK != pkConstantMap->AddObjectEntry(
                pkRes->GetSemantic(), 0,  pkRes->GetVariable(),
                pkRes->GetExtraData() + uiOffset, pkRes->GetObjectType()))
            {
                return false;
            }
        }
        return true;
        break;
    case NiMaterialResource::SOURCE_PREDEFINED:
        if (pkRes->GetType().EqualsNoCase("sampler2D") ||
            pkRes->GetType().EqualsNoCase("samplerCUBE"))
        {
            unsigned int uiSamplerId = UINT_MAX;
            unsigned int uiOffset = 0;

            if (pkRes->GetObjectType() != NiShaderAttributeDesc::OT_UNDEFINED)
            {
                for (unsigned int ui = 0; ui < kObjectOffsets.GetSize(); ui++)
                {
                    const ObjectResourceOffset& kOffset = 
                        kObjectOffsets.GetAt(ui);
                    if (kOffset.m_eProgramType == eCurrentType &&
                        kOffset.m_eObjectType == pkRes->GetObjectType())
                    {
                        uiOffset = kOffset.m_uiOffset;
                        break;
                    }
                }
            }

            if (!pkShader->AppendTextureSampler(uiPass, 
                uiSamplerId, pkRes->GetSemantic(), 
                pkRes->GetVariable(),
                pkRes->GetExtraData() + uiOffset))
            {
                return false;
            }

            return true;
        }
        else if (NISHADERERR_OK != pkConstantMap->AddPredefinedEntry(
            pkRes->GetSemantic(), uiExtra, 0, pkRes->GetVariable()))
        {
            return false;
        }
        return true;
        break;
    case NiMaterialResource::SOURCE_GLOBAL:
        {
            if (pkRes->GetType().EqualsNoCase("sampler"))
            {
                return false;
            }

            NiShaderAttributeDesc::AttributeType eElementAttribType = 
                GetAttributeTypeFromString(pkRes->GetType());
            NiShaderAttributeDesc::AttributeType eAttribType =
                (uiCount == 1) ? eElementAttribType : 
                NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;
            unsigned int uiFlags = eAttribType;
            NiFixedString kKey = pkRes->GetVariable();
            unsigned int uiDataStride = 
                static_cast<unsigned int>(
                NiShaderAttributeDesc::GetSizeOfAttributeType(
                eElementAttribType));
            unsigned int uiDataSize = uiDataStride * uiCount;

            if (!NiShaderFactory::RegisterGlobalShaderConstant(
                kKey, eAttribType, uiDataSize, 
                NULL))
            {
                NIASSERT(!"Failed to add global constant!");
            }

            // Retrieve the global entry
            NiShaderFactory* pkShaderFactory = NiShaderFactory::GetInstance();

            NiGlobalConstantEntry* pkGlobal = 
                pkShaderFactory->GetGlobalShaderConstantEntry(kKey);
            NIASSERT(pkGlobal);

            if (NISHADERERR_OK != pkConstantMap->AddGlobalEntry(
                kKey, uiFlags, uiExtra, 0, 0, kKey, uiDataSize, uiDataStride, 
                pkGlobal->GetDataSource()))
            {
                NiShaderFactory::ReleaseGlobalShaderConstant(
                    pkRes->GetVariable());
                return false;
            }

            // Release the entry immediately; the NiD3DShaderConstantMap 
            // should maintain a reference to the entry
            NiShaderFactory::ReleaseGlobalShaderConstant(pkRes->GetVariable());
        }
        return true;
        break;
    case NiMaterialResource::SOURCE_ATTRIBUTE:
        {
            if (pkRes->GetType().EqualsNoCase("sampler"))
            {
                return false;
            }       
            NiShaderAttributeDesc::AttributeType eElementAttribType = 
                GetAttributeTypeFromString(pkRes->GetType());
            NiShaderAttributeDesc::AttributeType eAttribType =
                (uiCount == 1) ? eElementAttribType : 
                NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;
            unsigned int uiFlags = eAttribType;
            NiFixedString kKey = pkRes->GetVariable();
            unsigned int uiDataStride = 
                static_cast<unsigned int>(
                NiShaderAttributeDesc::GetSizeOfAttributeType(
                eElementAttribType));
            unsigned int uiDataSize = uiDataStride * uiCount;

            if (NISHADERERR_OK != pkConstantMap->AddAttributeEntry(
                kKey, uiFlags, uiExtra, 0, 0, kKey, uiDataSize, uiDataStride, 
                NULL, false))
            {
                return false;
            }
        }
        return true;
        break;
    default:
        return false;
        break;
    }
}
//---------------------------------------------------------------------------
NiGPUProgram* NiFragmentMaterial::GenerateShaderProgram(
    NiGPUProgram::ProgramType eProgType,
    NiGPUProgramDescriptor* pkDesc, 
    NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    char acFilename[NI_MAX_PATH];
    NIVERIFY(pkDesc->GenerateKey(acFilename, 48));
    
    NiGPUProgramCache* pkCache = 
        m_aspProgramCaches[eProgType];

    if (!pkCache)
        return NULL;
    
    bool bFailedToCompilePreviously = false;
    NiGPUProgram* pkCachedShader = pkCache->FindCachedProgram(
        acFilename, kUniforms, bFailedToCompilePreviously);
    
    if (pkCachedShader)
        return pkCachedShader;

    if (bFailedToCompilePreviously)
        return NULL;

    Context kContext;
    kContext.m_spConfigurator = NiNew NiMaterialConfigurator(
        pkCache->GetPlatform(), pkCache->GetPlatformSpecificCodeID(),
        pkCache->GetValidCompileTargetsForPlatform());

    bool bShadeTreeGenerated = false;
    switch (eProgType)
    {
        case NiGPUProgram::PROGRAM_VERTEX:
            {
                NiOutputDebugString("Generating vertex shader for object \"");
                NiOutputDebugString((const char*)m_kDebugIdentifier);
                NiOutputDebugString("\":\n");

                bShadeTreeGenerated = GenerateVertexShadeTree(kContext, 
                    pkDesc);
            }
            break;
        case NiGPUProgram::PROGRAM_PIXEL:
            {
                NiOutputDebugString("Generating pixel shader for object \"");
                NiOutputDebugString((const char*)m_kDebugIdentifier);
                NiOutputDebugString("\":\n");

                bShadeTreeGenerated = GeneratePixelShadeTree(kContext, pkDesc);
            }
            break;
        case NiGPUProgram::PROGRAM_GEOMETRY:
            {
                NiOutputDebugString("Generating geometry shader for object"
                    " \"");
                NiOutputDebugString((const char*)m_kDebugIdentifier);
                NiOutputDebugString("\":\n");

                bShadeTreeGenerated = GenerateGeometryShadeTree(kContext, 
                    pkDesc);
            }
            break;
        default:
            NIASSERT(!"Unknown program type!");
            break;
    }

    if (bShadeTreeGenerated && kContext.m_spConfigurator->GetNodeCount() != 0)
    {
        NiGPUProgram* pkProgram = EvaluateProgram(kContext, acFilename,
            eProgType, kUniforms);

        if (pkProgram == NULL)
        {
            SetFailedGPUProgram(eProgType, pkDesc);
            return NULL;
        }

        return pkProgram;
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GenerateVertexShadeTree(Context&, 
    NiGPUProgramDescriptor*)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GenerateGeometryShadeTree(Context&, 
    NiGPUProgramDescriptor*)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::GeneratePixelShadeTree(Context&, 
    NiGPUProgramDescriptor*)
{
    return false;
}
//---------------------------------------------------------------------------
NiFixedString NiFragmentMaterial::GenerateUVSetName(unsigned int uiIndex)
{
    char acVarName[32];
    NiSprintf(acVarName, 32, "UVSet%d", uiIndex);
    return acVarName;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiFragmentMaterial::InsertTextureSampler(
    Context& kContext,
    const NiFixedString& kMapName,
    TextureMapSamplerType eSamplerType,
    unsigned int uiOccurance,
    NiShaderAttributeDesc::ObjectType eObjectType)
{
    char acName[256];
    if (uiOccurance != 0)
        NiSprintf(acName, 256, "%s%d", (const char*)kMapName, uiOccurance);
    else
        NiSprintf(acName, 256, "%s", (const char*)kMapName);

    NiFixedString kSamplerType;
    switch (eSamplerType)
    {
        case TEXTURE_SAMPLER_CUBE:
            kSamplerType = "samplerCUBE";
            break;
        default:
            NIASSERT(eSamplerType == TEXTURE_SAMPLER_2D);
            kSamplerType = "sampler2D";
            break;
    }

    return kContext.m_spUniforms->AddOutputResource(
        kSamplerType, kMapName, "",
        acName, 1, NiMaterialResource::SOURCE_PREDEFINED, 
        eObjectType, uiOccurance);
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::AddDefaultMaterialNodes(Context& kContext,
    NiGPUProgramDescriptor*, NiGPUProgram::ProgramType eType)
{
    NiFixedString kInputNodeName;
    NiFixedString kOutputNodeName;
    NiFixedString kProducerType;

    switch (eType)
    {
    case NiGPUProgram::PROGRAM_VERTEX:
        kInputNodeName = "VertexIn";
        kOutputNodeName = "VertexOut";
        kProducerType = "Vertex";
        break;
    case NiGPUProgram::PROGRAM_PIXEL:
        kInputNodeName = "PixelIn";
        kOutputNodeName = "PixelOut";
        kProducerType = "Pixel";
        break;
    case NiGPUProgram::PROGRAM_GEOMETRY:
        kInputNodeName = "GeometryIn";
        kOutputNodeName = "GeometryOut";
        kProducerType = "Geometry";
        break;
    default:
        NIASSERT(!"Unknown GPU program type!");
        break;
    }

    // Create input node
    kContext.m_spInputs = NiNew NiMaterialResourceProducerNode(kInputNodeName,
        kProducerType);  
    kContext.m_spConfigurator->AddNode(kContext.m_spInputs);

    // Add constant map elements
    kContext.m_spUniforms = NiNew NiMaterialResourceProducerNode("Uniforms",
        "Uniform");
    kContext.m_spConfigurator->AddNode(kContext.m_spUniforms);

    // Add constant elements
    kContext.m_spStatics = NiNew NiMaterialResourceProducerNode("Statics",
        "Static");
    kContext.m_spConfigurator->AddNode(kContext.m_spStatics);

    // Create output node
    kContext.m_spOutputs = NiNew NiMaterialResourceConsumerNode(
        kOutputNodeName, kProducerType);
    kContext.m_spConfigurator->AddNode(kContext.m_spOutputs);
    
    return true;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiFragmentMaterial::AddOutputPredefined(
    NiMaterialNode* pkNode, NiShaderConstantMap::DefinedMappings eMapping,
    unsigned int uiNumRegisters, unsigned int uiCount, 
    unsigned int uiExtraData)
{
    NiFixedString kSemantic;
    NiShaderConstantMap::LookUpPredefinedMappingName(eMapping, kSemantic);
    NiShaderAttributeDesc::AttributeType eAttribType =
        NiShaderConstantMap::LookUpPredefinedMappingType(eMapping, 
        uiNumRegisters);
    NiFixedString kType = GetStringFromAttributeType(eAttribType);

    char acString[256];
    NiSprintf(acString, 256, "g_%s", (const char*) kSemantic);
    return pkNode->AddOutputResource(kType, kSemantic, "", 
        acString, uiCount, NiMaterialResource::SOURCE_PREDEFINED,
        NiShaderAttributeDesc::OT_UNDEFINED, uiExtraData);
}
//---------------------------------------------------------------------------
NiMaterialResource* NiFragmentMaterial::AddOutputObject(NiMaterialNode* pkNode,
    NiShaderConstantMap::ObjectMappings eMapping, 
    NiShaderAttributeDesc::ObjectType eObjectType,
    unsigned int uiObjectCount, const char* pcVariableModifier,
    unsigned int uiCount)
{
    
    NiFixedString kSemantic;
    if (!NiShaderConstantMap::LookUpObjectMappingName(eMapping, kSemantic))
        return NULL;

    unsigned int uiRegCount;
    unsigned int uiFloatCount;
    NiShaderAttributeDesc::AttributeType eAttribType = 
        NiShaderConstantMap::LookUpObjectMappingType(eMapping,
        uiRegCount, uiFloatCount);

    NiFixedString kType = GetStringFromAttributeType(eAttribType);

    if (!pcVariableModifier)
        pcVariableModifier = "";

    char acString[256];
    if (uiObjectCount == 1)
    {
        NiSprintf(acString, 256, "g_%s%s", pcVariableModifier,
            (const char*) kSemantic);
    }
    else 
    {
        NiSprintf(acString, 256, "g_%s%s%d",pcVariableModifier,
            (const char*) kSemantic, uiObjectCount);
    }

    return pkNode->AddOutputResource(kType, kSemantic, "", 
        acString, uiCount, NiMaterialResource::SOURCE_OBJECT,
        eObjectType, uiObjectCount);
}
//---------------------------------------------------------------------------
NiMaterialResource* NiFragmentMaterial::AddOutputAttribute(
    NiMaterialNode* pkNode, const NiFixedString& kVariableName,
    NiShaderAttributeDesc::AttributeType eType, unsigned int uiCount)
{
    NiFixedString kType = GetStringFromAttributeType(eType);
    return pkNode->AddOutputResource(kType, kVariableName, "", 
        kVariableName, uiCount, NiMaterialResource::SOURCE_ATTRIBUTE);
}
//---------------------------------------------------------------------------
NiMaterialResource* NiFragmentMaterial::AddOutputGlobal(NiMaterialNode* pkNode,
    const NiFixedString& kVariableName, 
    NiShaderAttributeDesc::AttributeType eType, unsigned int uiCount)
{
    NiFixedString kType = GetStringFromAttributeType(eType);
    return pkNode->AddOutputResource(kType, kVariableName, "", 
        kVariableName, uiCount, NiMaterialResource::SOURCE_GLOBAL);
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetHighestObjectOffset(
    NiShaderAttributeDesc::ObjectType eObjectType, 
    RenderPassDescriptor* pkRenderPasses, unsigned int uiCount)
{
    unsigned int uiLastOffset = 0;

    for (unsigned int ui = uiCount - 1; ((int)ui) >= 0; ui--)
    {
        for (unsigned int uj = 0; uj < 
            pkRenderPasses[ui].m_kObjectOffsets.GetSize(); uj++)
        {
            const ObjectResourceOffset& kOffset = 
                pkRenderPasses[ui].m_kObjectOffsets.GetAt(uj);
            if (kOffset.m_eObjectType == eObjectType)
            {
                if (kOffset.m_uiOffset > uiLastOffset)
                    uiLastOffset = kOffset.m_uiOffset;
            }
        }

        // Go no further back if this pass resets object offsets
        if (pkRenderPasses[ui].m_bResetObjectOffsets)
            break;
    }

    return uiLastOffset;
}
//---------------------------------------------------------------------------
NiMaterialNode* NiFragmentMaterial::GetAttachableNodeFromLibrary(
    const NiFixedString& kNodeName)
{
    for (unsigned int ui = 0; ui < m_kLibraries.GetSize(); ui++)
    {
        NiMaterialNodeLibrary* pkLib = m_kLibraries.GetAt(ui);
        if (pkLib)
        {
            NiMaterialNode* pkNode = pkLib->GetAttachableNodeByName(kNodeName);
            if (pkNode)
                return pkNode;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetMaterialNodeLibraryCount() const
{
    return m_kLibraries.GetSize();
}
//---------------------------------------------------------------------------
NiMaterialNodeLibrary* NiFragmentMaterial::GetMaterialNodeLibrary(
    unsigned int uiIndex) const
{
    return m_kLibraries.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::AddShaderFallbackFunc(
    SHADERFALLBACKFUNC pfnFallback)
{
    unsigned int uiReturn = m_kShaderFallbackFuncs.Add(pfnFallback);

    return uiReturn;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::RemoveShaderFallbackFunc(
    SHADERFALLBACKFUNC pfnFallback)
{
    unsigned int uiIndex = FindShaderFallbackFunc(pfnFallback);
    if (uiIndex == UINT_MAX)
        return false;

    m_kShaderFallbackFuncs.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentMaterial::RemoveShaderFallbackFunc(unsigned int uiIndex)
{
    if (m_kShaderFallbackFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kShaderFallbackFuncs.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::RemoveAllShaderFallbackFuncs()
{
    m_kShaderFallbackFuncs.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetShaderFallbackFuncCount() const
{
    return m_kShaderFallbackFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetShaderFallbackFuncArrayCount() const
{
    return m_kShaderFallbackFuncs.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::FindShaderFallbackFunc(
    SHADERFALLBACKFUNC pfnFallback) const
{
    for (unsigned int i = 0; i < m_kShaderFallbackFuncs.GetSize(); i++)
    {
        if (m_kShaderFallbackFuncs.GetAt(i) == pfnFallback)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::SHADERFALLBACKFUNC 
    NiFragmentMaterial::GetShaderFallbackFunc(unsigned int uiIndex) const
{
    return m_kShaderFallbackFuncs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::AddDefaultFallbacks()
{
    // Derived classes will need to provide their own fallbacks.
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::SetMaxPassCount(unsigned int uiMaxPassCount)
{
    m_uiMaxPassCount = uiMaxPassCount;
}
//---------------------------------------------------------------------------
unsigned int NiFragmentMaterial::GetMaxPassCount() const
{
    return m_uiMaxPassCount;
}
//---------------------------------------------------------------------------
void NiFragmentMaterial::LoadAllCachesForAllMaterials()
{
    NiMaterialIterator kIter = NiMaterial::GetFirstMaterialIter();
    while (kIter)
    {
        NiMaterial* pkMaterial = NiMaterial::GetNextMaterial(kIter);

        NiFragmentMaterial* pkFragMaterial =
            NiDynamicCast(NiFragmentMaterial, pkMaterial);

        if (pkFragMaterial)
            pkFragMaterial->LoadAllCaches();
    }
}
//---------------------------------------------------------------------------
