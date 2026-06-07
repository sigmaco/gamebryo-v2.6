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

#include "NiCommonSemantics.h"
#include "NiStandardMaterial.h"
#include "NiStandardVertexProgramDescriptor.h"
#include "NiStandardPixelProgramDescriptor.h"
#include "NiMaterialNodeLibrary.h"
#include "NiTexturingProperty.h"
#include "NiVertexColorProperty.h"
#include "NiFogProperty.h"
#include "NiShadowGenerator.h"
#include "NiSpecularProperty.h"
#include "NiMaterialProperty.h"
#include "NiAlphaProperty.h"
#include "NiTextureEffect.h"
#include "NiStandardMaterialDescriptor.h"
#include "NiBitfield.h"
#include "NiShaderDeclaration.h"
#include "NiShadowMap.h"
#include "NiShadowTechnique.h"
#include "NiRenderObjectMaterialOption.h"

NiImplementRTTI(NiStandardMaterial, NiFragmentMaterial);

//---------------------------------------------------------------------------
NiStandardMaterial::NiStandardMaterial(NiMaterialNodeLibrary* pkLibrary,
    bool bAutoCreateCaches) :
    NiFragmentMaterial(pkLibrary, "NiStandardMaterial", 
        VERTEX_VERSION, GEOMETRY_VERSION, PIXEL_VERSION, 
        bAutoCreateCaches), 
    m_kDescriptorName("NiStandardMaterialDescriptor"),
    m_bForcePerPixelLighting(false), 
    m_bSaturateShading(false),
    m_bSaturateTextures(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiStandardMaterial::NiStandardMaterial(const NiFixedString& kName, 
    NiMaterialNodeLibrary* pkLibrary, bool bAutoCreateCaches) : 
    NiFragmentMaterial(pkLibrary, kName, VERTEX_VERSION, 
        GEOMETRY_VERSION, PIXEL_VERSION, bAutoCreateCaches), 
    m_kDescriptorName("NiStandardMaterialDescriptor"),
    m_bForcePerPixelLighting(false), 
    m_bSaturateShading(false),
    m_bSaturateTextures(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiStandardMaterial::NiStandardMaterial(const NiFixedString& kName, 
    NiMaterialNodeLibrary* pkLibrary, unsigned int uiVertexVersion,
    unsigned int uiGeometryVersion, unsigned int uiPixelVersion, 
    bool bAutoCreateCaches) : 
    NiFragmentMaterial(pkLibrary, kName, (unsigned short)uiVertexVersion, 
        (unsigned short)uiGeometryVersion, (unsigned short)uiPixelVersion, 
        bAutoCreateCaches), 
    m_kDescriptorName("NiStandardMaterialDescriptor"),
    m_bForcePerPixelLighting(false), 
    m_bSaturateShading(false),
    m_bSaturateTextures(false)
{
    /* */
}
//---------------------------------------------------------------------------
int NiStandardMaterial::MapIndexFromTextureEnum(TextureMap eMap,
    NiStandardPixelProgramDescriptor* pkPixDesc)
{
    // Note that this is in reverse order from the enumeration. This allows
    // us to use the counts to determine which map index is actually being
    // used for this texture map in the descriptor.
    
    int iMapIdx = -1;
    switch (eMap)
    {
        case MAP_MAX: 
        default:
            NIASSERT(!"Should not get here!");
            break;
        case MAP_DECAL02: 
            if (pkPixDesc->GetDECALMAPCOUNT() > 2)
            {
                iMapIdx++;
            }
        case MAP_DECAL01: 
            if (pkPixDesc->GetDECALMAPCOUNT() > 1)
            {
                iMapIdx++;
            }
        case MAP_DECAL00: 
            if (pkPixDesc->GetDECALMAPCOUNT() > 0)
            {
                iMapIdx++;
            }
        case MAP_CUSTOM04: 
            iMapIdx += pkPixDesc->GetCUSTOMMAP04COUNT();
        case MAP_CUSTOM03:
            iMapIdx += pkPixDesc->GetCUSTOMMAP03COUNT();
        case MAP_CUSTOM02: 
            iMapIdx += pkPixDesc->GetCUSTOMMAP02COUNT();
        case MAP_CUSTOM01: 
            iMapIdx += pkPixDesc->GetCUSTOMMAP01COUNT();
        case MAP_CUSTOM00: 
            iMapIdx += pkPixDesc->GetCUSTOMMAP00COUNT();
        case MAP_GLOW: 
            iMapIdx += pkPixDesc->GetGLOWMAPCOUNT();
        case MAP_GLOSS: 
            iMapIdx += pkPixDesc->GetGLOSSMAPCOUNT();
        case MAP_BUMP: 
            iMapIdx += pkPixDesc->GetBUMPMAPCOUNT();
        case MAP_DETAIL:
            iMapIdx += pkPixDesc->GetDETAILMAPCOUNT();
        case MAP_DARK: 
            iMapIdx += pkPixDesc->GetDARKMAPCOUNT();
        case MAP_NORMAL: 
            iMapIdx += pkPixDesc->GetNORMALMAPCOUNT();
        case MAP_BASE: 
            iMapIdx += pkPixDesc->GetBASEMAPCOUNT();
        case MAP_PARALLAX: 
            iMapIdx += pkPixDesc->GetPARALLAXMAPCOUNT();
    }

    return iMapIdx;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GetTextureNameFromTextureEnum(TextureMap eMap,
    NiFixedString& kString, unsigned int& uiOccurance)
{
    switch (eMap)
    {
    case MAP_MAX: 
    default:
        NIASSERT(!"Could not find map!\n");
        return false;
    case MAP_DECAL02: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::DECAL_BASE);
        uiOccurance = 2;
        return true;
    case MAP_DECAL01: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::DECAL_BASE);
        uiOccurance = 1;
        return true;
    case MAP_DECAL00: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::DECAL_BASE);
        uiOccurance = 0;
        return true;
    case MAP_CUSTOM04: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::SHADER_BASE);
        uiOccurance = 4;
        return true;
    case MAP_CUSTOM03:
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::SHADER_BASE);
        uiOccurance = 3;
        return true;
    case MAP_CUSTOM02: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::SHADER_BASE);
        uiOccurance = 2;
        return true;
    case MAP_CUSTOM01: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::SHADER_BASE);
        uiOccurance = 1;
        return true;
    case MAP_CUSTOM00: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::SHADER_BASE);
        uiOccurance = 0;
        return true;
    case MAP_GLOW: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::GLOW_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_GLOSS: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::GLOSS_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_BUMP: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::BUMP_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_DETAIL:
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::DETAIL_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_DARK: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::DARK_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_NORMAL: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::NORMAL_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_BASE: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::BASE_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_PARALLAX: 
        kString = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::PARALLAX_INDEX);
        uiOccurance = 0;
        return true;
    case MAP_DIRSHADOW00:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 0;
        return true;
    case MAP_DIRSHADOW01:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 1;
        return true;
    case MAP_DIRSHADOW02:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 2;
        return true;
    case MAP_DIRSHADOW03:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 3;
        return true;
    case MAP_DIRSHADOW04:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 4;
        return true;
    case MAP_DIRSHADOW05:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 5;
        return true;
    case MAP_DIRSHADOW06:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 6;
        return true;
    case MAP_DIRSHADOW07:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_DIR);
        uiOccurance = 7;
        return true;
    case MAP_POINTSHADOW00:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 0;
        return true;
    case MAP_POINTSHADOW01:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 1;
        return true;
    case MAP_POINTSHADOW02:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 2;
        return true;
    case MAP_POINTSHADOW03:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 3;
        return true;
    case MAP_POINTSHADOW04:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 4;
        return true;
    case MAP_POINTSHADOW05:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 5;
        return true;
    case MAP_POINTSHADOW06:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 6;
        return true;
    case MAP_POINTSHADOW07:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_POINT);
        uiOccurance = 7;
        return true;
    case MAP_SPOTSHADOW00:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 0;
        return true;
    case MAP_SPOTSHADOW01:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 1;
        return true;
    case MAP_SPOTSHADOW02:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 2;
        return true;
    case MAP_SPOTSHADOW03:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 3;
        return true;
    case MAP_SPOTSHADOW04:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 4;
        return true;
    case MAP_SPOTSHADOW05:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 5;
        return true;
    case MAP_SPOTSHADOW06:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 6;
        return true;
    case MAP_SPOTSHADOW07:  
        kString = NiShadowMap::GetMapNameFromType(LIGHT_SPOT);
        uiOccurance = 7;
        return true;

    }
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GetTextureTransformMappingFromEnum(TextureMap eMap, 
    NiShaderConstantMap::DefinedMappings& eMapping, 
    unsigned int& uiOccurance)
{
    switch (eMap)
    {
    case MAP_MAX: 
    default:
        NIASSERT(!"Could not find map!\n");
        return false;
    case MAP_DECAL02: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDECAL;
        uiOccurance = 2;
        return true;
    case MAP_DECAL01: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDECAL;
        uiOccurance = 1;
        return true;
    case MAP_DECAL00: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDECAL;
        uiOccurance = 0;
        return true;
    case MAP_CUSTOM04: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER;
        uiOccurance = 4;
        return true;
    case MAP_CUSTOM03:
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER;
        uiOccurance = 3;
        return true;
    case MAP_CUSTOM02: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER;
        uiOccurance = 2;
        return true;
    case MAP_CUSTOM01: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER;
        uiOccurance = 1;
        return true;
    case MAP_CUSTOM00: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER;
        uiOccurance = 0;
        return true;
    case MAP_GLOW: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMGLOW;
        uiOccurance = 0;
        return true;
    case MAP_GLOSS: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMGLOSS;
        uiOccurance = 0;
        return true;
    case MAP_BUMP: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBUMP;
        uiOccurance = 0;
        return true;
    case MAP_DETAIL:
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDETAIL;
        uiOccurance = 0;
        return true;
    case MAP_DARK: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDARK;
        uiOccurance = 0;
        return true;
    case MAP_NORMAL: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMNORMAL;
        uiOccurance = 0;
        return true;
    case MAP_BASE: 
        eMapping = NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBASE;
        uiOccurance = 0;
        return true;
    case MAP_PARALLAX: 
        eMapping =  NiShaderConstantMap::SCM_DEF_TEXTRANSFORMPARALLAX;
        uiOccurance = 0;
        return true;
    }
}
//---------------------------------------------------------------------------
void NiStandardMaterial::SetForcePerPixelLighting(bool bForce)
{
    m_bForcePerPixelLighting = bForce;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GetForcePerPixelLighting() const
{
    return m_bForcePerPixelLighting;
}
//---------------------------------------------------------------------------
void NiStandardMaterial::SetSaturateShading(bool bSaturate)
{
    m_bSaturateShading = bSaturate;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GetSaturateShading() const
{
    return m_bSaturateShading;
}
//---------------------------------------------------------------------------
void NiStandardMaterial::SetSaturateTextures(bool bSaturate)
{
    m_bSaturateTextures = bSaturate;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GetSaturateTextures() const
{
    return m_bSaturateTextures;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GenerateDescriptor(const NiRenderObject* pkMesh, 
    const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState,
    NiMaterialDescriptor& kMaterialDesc)
{
    if (!pkPropState)
    {
        NIASSERT(!"Could not find property state! Try calling"
            " UpdateProperties.\n");
        return false;
    }

    NiStandardMaterialDescriptor* pkDesc = (NiStandardMaterialDescriptor*) 
        &kMaterialDesc;
    pkDesc->m_kIdentifier = m_kDescriptorName;
    
    // Handle transform
    bool bHardwareSkinned = false;
    pkDesc->SetTransformDescriptor(pkMesh, bHardwareSkinned);

    // Handle normals
    bool bNBTs = false;
    bool bNormals = false;
    pkDesc->SetNBTDescriptor(pkMesh, bHardwareSkinned,  bNormals, bNBTs);

    // Handle vertex colors
    bool bVertexColors = false;
    pkDesc->SetVertexColorDescriptor(pkMesh, bVertexColors);
    
    pkDesc->SetINPUTUVCOUNT(0);

    bool bSpecularEnabled = false;

    if (pkPropState)
    {
        pkDesc->SetVertexColorPropertyDescriptor(pkMesh, pkPropState, 
            bVertexColors);
        pkDesc->SetSpecularPropertyDescriptor(pkMesh, pkPropState, 
            bSpecularEnabled);
        pkDesc->SetMaterialPropertyDescriptor(pkMesh, pkPropState);
        pkDesc->SetFogPropertyDescriptor(pkMesh, pkPropState);
        pkDesc->SetAlphaPropertyDescriptor(pkMesh, pkPropState);

        // create active map array
        const NiTexturingProperty::Map* apkActiveMaps[
            STANDARD_PIPE_MAX_TEXTURE_MAPS];

        // Reset active maps to zero
        memset(apkActiveMaps, 0, sizeof(NiTexturingProperty::Map*) * 
            STANDARD_PIPE_MAX_TEXTURE_MAPS);

        pkDesc->SetTexuringPropertyDescriptor(pkMesh, pkPropState, 
            pkEffectState, apkActiveMaps, STANDARD_PIPE_MAX_TEXTURE_MAPS, 
            STANDARD_PIPE_MAX_DECAL_MAPS, STANDARD_PIPE_MAX_SHADER_MAPS,
            bSpecularEnabled);
    }
        

    if (pkEffectState)
    {
        pkDesc->SetLightsDescriptor(pkMesh, pkEffectState);
        pkDesc->SetEnvMapDescriptor(pkMesh, pkEffectState);
        pkDesc->SetProjLightMapDescriptor(pkMesh, pkEffectState);
        pkDesc->SetProjShadowMapDescriptor(pkMesh, pkEffectState);        
    }

    bool bDynamicLighting = pkDesc->GetPOINTLIGHTCOUNT() != 0 ||
        pkDesc->GetSPOTLIGHTCOUNT() != 0 || 
        pkDesc->GetDIRLIGHTCOUNT() != 0;
    
    unsigned int uiShadowMapsForLight = pkDesc->GetSHADOWMAPFORLIGHT();

    // Only force per pixel lighting if a dynamic light exist.
    bool bUsePerPixelLighting = bDynamicLighting && m_bForcePerPixelLighting;

    if (uiShadowMapsForLight || bUsePerPixelLighting || 
        (bDynamicLighting && (pkDesc->GetPARALLAXMAPCOUNT() != 0 ||
        pkDesc->GetNORMALMAPCOUNT() != 0)))
    {
        pkDesc->SetPERVERTEXFORLIGHTS(0);
    }
    else
    {
        // Even if no lights actually exist, VS will handle all lighting
        pkDesc->SetPERVERTEXFORLIGHTS(1);
    }

    // If there are no normals, disable effects that require normals
    if (pkDesc->GetNORMAL() == NORMAL_NONE)
    {
        pkDesc->SetPOINTLIGHTCOUNT(0);
        pkDesc->SetDIRLIGHTCOUNT(0);
        pkDesc->SetSPOTLIGHTCOUNT(0);

        NiStandardMaterial::TexEffectType eTexEffect = 
            (NiStandardMaterial::TexEffectType) pkDesc->GetENVMAPTYPE();

        if (eTexEffect == NiStandardMaterial::TEXEFFECT_SPHERICAL || 
            eTexEffect == NiStandardMaterial::TEXEFFECT_SPECULAR_CUBE || 
            eTexEffect == NiStandardMaterial::TEXEFFECT_DIFFUSE_CUBE)
        {
            pkDesc->SetENVMAPTYPE(NiStandardMaterial::TEXEFFECT_NONE);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::AddResourceToShaderConstantMap(NiShader* pkShader,
    NiShaderConstantMap* pkConstantMap, NiMaterialResource* pkRes,
    NiGPUProgramDescriptor* pkVertexDesc, 
    NiGPUProgramDescriptor* pkGeometryDesc, 
    NiGPUProgramDescriptor* pkPixelDesc, 
    const NiTObjectSet<ObjectResourceOffset>& kObjectOffsets, 
    unsigned int uiPass, NiGPUProgram::ProgramType eCurrentType)
{
    if (pkRes->GetSource() == NiMaterialResource::SOURCE_PREDEFINED &&
        pkRes->GetSemantic().EqualsNoCase("TexTransform"))
    {
        // Texture transforms are a special case, there is no
        // easy way of knowing in the vertex shader what the 
        // texture map mapping will be. Therefore, we resolve
        // the value here before adding to the constant map.

        if (pkPixelDesc->m_kIdentifier != 
            "NiStandardPixelProgramDescriptor")
        {
            NIASSERT(!"Unknown pixel descriptor identifier!\n");
            return false;
        }

        NiStandardPixelProgramDescriptor* pkDefPixelDesc = 
            (NiStandardPixelProgramDescriptor*) pkPixelDesc;

        if (!pkDefPixelDesc)
        {
            NIASSERT(!"NULL pixel descriptor!");
            return false;
        }

        TextureMap eMap = MAP_MAX;
        for (unsigned int ui = 0; ui < MAP_MAX; ui++)
        {
            int iUVIndexForEnum = FindUVSetIndexForTextureEnum(
                (TextureMap)ui, pkDefPixelDesc);
            unsigned int uiObjIndex = pkRes->GetExtraData();
            if ((unsigned int) iUVIndexForEnum == uiObjIndex)
            {
                eMap = (TextureMap)ui;
                break;
            }
        }

        NIASSERT(eMap != MAP_MAX);
        NiShaderConstantMap::DefinedMappings eMapping;
        unsigned int uiExtraData = pkRes->GetExtraData();
        if (!GetTextureTransformMappingFromEnum(eMap, 
            eMapping, uiExtraData))
        {
            return false;
        }

        NiFixedString kSemantic;
        if (!NiShaderConstantMap::LookUpPredefinedMappingName(
            eMapping, kSemantic))
        {
            NIASSERT(!"Unknown predefined mapping");
            return false;
        }

        if (NISHADERERR_OK != pkConstantMap->AddPredefinedEntry(
            kSemantic, uiExtraData, 0, pkRes->GetVariable()))
        {
            NIASSERT(!"Unable to add predefined entry");
            return false;
        }
        return true;
    }
    else
    {
        return NiFragmentMaterial::AddResourceToShaderConstantMap(pkShader,
            pkConstantMap, pkRes, pkVertexDesc, pkGeometryDesc, pkPixelDesc, 
            kObjectOffsets, uiPass, eCurrentType);
    }
}
//---------------------------------------------------------------------------
NiShader* NiStandardMaterial::CreateShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
        return false;

    return pkRenderer->GetFragmentShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SetupPackingRequirements(NiShader* pkShader,
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor*, unsigned int)
{
    NiStandardMaterialDescriptor* pkRealMaterialDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    unsigned int uiStreamCount = 1;

    // If transform instancing is used create another stream to store the
    // per instance parameters.
    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        uiStreamCount++;
    }

    NiShaderDeclarationPtr spShaderDecl = 
        NiShaderDeclaration::Create(15, uiStreamCount);

    if (!spShaderDecl)
    {
        NIASSERT(!"Invalid shader declaration");
        return false;
    }
    
    unsigned int uiInputUVCount = pkRealMaterialDesc->GetINPUTUVCOUNT();
    unsigned int uiEntryCount = 0;
    unsigned int uiUVOffset = 0;
#if !defined(_XENON)
    // If transform instancing is used, the the instance transforms will
    // consume the first three texture coordinates, so offset all other
    // texture coordinates by three.
    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        uiUVOffset = 3;
        if (uiUVOffset + uiInputUVCount > 8)
        {
            NIASSERT(!"Invalid shader declaration. Too many TexCoords.");
            return false;
        }
    }
#endif //#if !defined(_XENON)

    spShaderDecl->SetEntry(uiEntryCount++, 
        NiShaderDeclaration::SHADERPARAM_NI_POSITION0,
        NiShaderDeclaration::SPTYPE_FLOAT3);

    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_SKINNED)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT,
            NiShaderDeclaration::SPTYPE_FLOAT3); // 3, 4th is implicit.
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES,
            NiShaderDeclaration::SPTYPE_UBYTE4);
    }
    else if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        spShaderDecl->SetEntry(0, 
            NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM0,
            NiShaderDeclaration::SPTYPE_FLOAT4, uiStreamCount-1);

        spShaderDecl->SetEntry(1,
            NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM1,
            NiShaderDeclaration::SPTYPE_FLOAT4, uiStreamCount-1);

        spShaderDecl->SetEntry(2, 
            NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM2,
            NiShaderDeclaration::SPTYPE_FLOAT4, uiStreamCount-1);
    }


    if (pkRealMaterialDesc->GetNORMAL() != NORMAL_NONE)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_NORMAL,
            NiShaderDeclaration::SPTYPE_FLOAT3);
    }

    if (pkRealMaterialDesc->GetVERTEXCOLORS() == VC_EXISTS)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_COLOR,
            NiShaderDeclaration::SPTYPE_UBYTECOLOR);
    }

    // This is primarily to set up the number of Texcoord entries in the
    // eventual NiSemanticAdapterTable - the generic semantic index of the 
    // entries set here may be incorrect.
    const unsigned int uiTexCoordEntries = uiEntryCount;
    unsigned int ui = 0;
    for (; ui < 8 && ui < uiInputUVCount; ui++)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            (NiShaderDeclaration::ShaderParameter)
            (NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0 + ui + uiUVOffset),
            NiShaderDeclaration::SPTYPE_FLOAT2);
    }

    // Binormals and tangents must be last, so this shader declaration
    // can be used with the fixed-function pipeline in DX9.
    if (pkRealMaterialDesc->GetNORMAL() == NORMAL_NBT &&
        (pkRealMaterialDesc->GetNORMALMAPCOUNT() || 
         pkRealMaterialDesc->GetPARALLAXMAPCOUNT()) )
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_BINORMAL,
            NiShaderDeclaration::SPTYPE_FLOAT3);

        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_TANGENT,
            NiShaderDeclaration::SPTYPE_FLOAT3);
    }

    pkShader->SetSemanticAdapterTableFromShaderDeclaration(spShaderDecl);

    // Fix up the texture coordinate entries
    NiSemanticAdapterTable& kTable = pkShader->GetSemanticAdapterTable();

    // Create list of all UV sets that the shader will use
    unsigned int uiTextureCount = 
        pkRealMaterialDesc->GetStandardTextureCount();
    NIASSERT(uiTextureCount <= STANDARD_PIPE_MAX_TEXTURE_MAPS);
    bool abUVSetsUsed[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(abUVSetsUsed, 0, sizeof(abUVSetsUsed));

    unsigned int uiCounter = 0;
    for (ui = 0; ui < uiTextureCount; ui++)
    {
        unsigned int uiUVSet;
        TexGenOutput eTexOutput;
        pkRealMaterialDesc->GetTextureUsage(ui, uiUVSet, eTexOutput);
        NIASSERT(uiUVSet < STANDARD_PIPE_MAX_TEXTURE_MAPS);
        if (abUVSetsUsed[uiUVSet] == false)
        {
            uiCounter++;
            abUVSetsUsed[uiUVSet] = true;
        }
    }
    NIASSERT(uiCounter == uiInputUVCount);

    // Set those UV sets on the semantic adapter table
    uiCounter = 0;
    for (ui = 0; ui < uiInputUVCount; ui++)
    {
        while (abUVSetsUsed[uiCounter] == false)
        {
            NIASSERT(uiCounter < STANDARD_PIPE_MAX_TEXTURE_MAPS);
            uiCounter++;
        }

        NiFixedString kGenericSemantic = 
            kTable.GetGenericSemanticName(uiTexCoordEntries + ui);

        kTable.SetGenericSemantic(
            uiTexCoordEntries + ui, 
            kGenericSemantic, 
            (NiUInt8)uiCounter);
        uiCounter++;
    }

    return true;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ReturnCode NiStandardMaterial::GenerateShaderDescArray(
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, 
    unsigned int& uiCountAdded)
{
    NI_UNUSED_ARG(uiMaxCount);
    NIASSERT(uiMaxCount != 0);
    uiCountAdded = 0;

    if (pkMaterialDescriptor->m_kIdentifier != "NiStandardMaterialDescriptor")
        return RC_INVALID_MATERIAL;

    // Make sure that we're using the Gamebryo render state on the first pass.
    pkRenderPasses[0].m_bUsesNiRenderState = true;

    // Reset all object offsets for the first pass.
    pkRenderPasses[0].m_bResetObjectOffsets = true;

    NiStandardMaterialDescriptor* pkMatlDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    // Uncomment these lines to get a human-readable version of the 
    // material description
    // NiString kDescString = pkMatlDesc->ToString();

    NiStandardVertexProgramDescriptor* pkVertexDesc = 
        (NiStandardVertexProgramDescriptor*) &pkRenderPasses[0].m_kVertexDesc;
    pkVertexDesc->m_kIdentifier = "NiStandardVertexProgramDescriptor";

    NiStandardPixelProgramDescriptor* pkPixelDesc = 
        (NiStandardPixelProgramDescriptor*) &pkRenderPasses[0].m_kPixelDesc;
    pkPixelDesc->m_kIdentifier = "NiStandardPixelProgramDescriptor";

#if defined(NIDEBUG)
    NormalType eNormalType = (NormalType)pkMatlDesc->GetNORMAL();

    bool bHasNormal = eNormalType != NORMAL_NONE;
    bool bHasNBT = eNormalType == NORMAL_NBT;
#endif

    // Pixel Desc
    unsigned int uiApplyMode = pkMatlDesc->GetAPPLYMODE();
    pkPixelDesc->SetAPPLYMODE(uiApplyMode);

    unsigned int uiNormalMapType = pkMatlDesc->GetNORMALMAPTYPE();
    pkPixelDesc->SetNORMALMAPTYPE(uiNormalMapType);

    unsigned int uiParallaxCount = pkMatlDesc->GetPARALLAXMAPCOUNT();
    pkPixelDesc->SetPARALLAXMAPCOUNT(uiParallaxCount);

    unsigned int uiBaseCount = pkMatlDesc->GetBASEMAPCOUNT();
    pkPixelDesc->SetBASEMAPCOUNT(uiBaseCount);

    unsigned int uiNormalMapCount = pkMatlDesc->GetNORMALMAPCOUNT();
    pkPixelDesc->SetNORMALMAPCOUNT(uiNormalMapCount);

    unsigned int uiDarkMapCount = pkMatlDesc->GetDARKMAPCOUNT();
    pkPixelDesc->SetDARKMAPCOUNT(uiDarkMapCount);

    unsigned int uiDetailMapCount = pkMatlDesc->GetDETAILMAPCOUNT();
    pkPixelDesc->SetDETAILMAPCOUNT(uiDetailMapCount);

    unsigned int uiBumpMapCount = pkMatlDesc->GetBUMPMAPCOUNT();
    pkPixelDesc->SetBUMPMAPCOUNT(uiBumpMapCount);

    unsigned int uiGlossMapCount = pkMatlDesc->GetGLOSSMAPCOUNT();
    pkPixelDesc->SetGLOSSMAPCOUNT(uiGlossMapCount);

    unsigned int uiGlowMapCount = pkMatlDesc->GetGLOWMAPCOUNT();
    pkPixelDesc->SetGLOWMAPCOUNT(uiGlowMapCount);

    unsigned int uiCustomMap00Count = pkMatlDesc->GetCUSTOMMAP00COUNT();
    pkPixelDesc->SetCUSTOMMAP00COUNT(uiCustomMap00Count);

    unsigned int uiCustomMap01Count = pkMatlDesc->GetCUSTOMMAP01COUNT();
    pkPixelDesc->SetCUSTOMMAP01COUNT(uiCustomMap01Count);

    unsigned int uiCustomMap02Count = pkMatlDesc->GetCUSTOMMAP02COUNT();
    pkPixelDesc->SetCUSTOMMAP02COUNT(uiCustomMap02Count);

    unsigned int uiCustomMap03Count = pkMatlDesc->GetCUSTOMMAP03COUNT();
    pkPixelDesc->SetCUSTOMMAP03COUNT(uiCustomMap03Count);

    unsigned int uiCustomMap04Count = pkMatlDesc->GetCUSTOMMAP04COUNT();
    pkPixelDesc->SetCUSTOMMAP04COUNT(uiCustomMap04Count);

    unsigned int uiDecalMapCount = pkMatlDesc->GetDECALMAPCOUNT();
    pkPixelDesc->SetDECALMAPCOUNT(uiDecalMapCount);

    unsigned int uiProjLightMapCount = pkMatlDesc->GetPROJLIGHTMAPCOUNT();
    pkPixelDesc->SetPROJLIGHTMAPCOUNT(uiProjLightMapCount);

    unsigned int uiProjShadowMapCount = pkMatlDesc->GetPROJSHADOWMAPCOUNT();
    pkPixelDesc->SetPROJSHADOWMAPCOUNT(uiProjShadowMapCount);

    unsigned int uiProjShadowMapTypes = pkMatlDesc->GetPROJSHADOWMAPTYPES();
    pkPixelDesc->SetPROJSHADOWMAPTYPES(uiProjShadowMapTypes);

    unsigned int uiProjLightMapTypes = pkMatlDesc->GetPROJLIGHTMAPTYPES();
    pkPixelDesc->SetPROJLIGHTMAPTYPES(uiProjLightMapTypes);

    unsigned int uiProjShadowMapClipped = 
        pkMatlDesc->GetPROJSHADOWMAPCLIPPED();
    pkPixelDesc->SetPROJSHADOWMAPCLIPPED(uiProjShadowMapClipped);

    unsigned int uiProjLightMapClipped = pkMatlDesc->GetPROJLIGHTMAPCLIPPED();
    pkPixelDesc->SetPROJLIGHTMAPCLIPPED(uiProjLightMapClipped);

    unsigned int uiAmbDiffEmissive = pkMatlDesc->GetAMBDIFFEMISSIVE();
    pkPixelDesc->SetAMBDIFFEMISSIVE(uiAmbDiffEmissive);

    unsigned int uiLightingMode = pkMatlDesc->GetLIGHTINGMODE();
    pkPixelDesc->SetLIGHTINGMODE(uiLightingMode);

    unsigned int uiPerVertexForLights = pkMatlDesc->GetPERVERTEXFORLIGHTS();
    pkPixelDesc->SetPERVERTEXLIGHTING(uiPerVertexForLights);

    unsigned int uiFogType = pkMatlDesc->GetFOGTYPE();
    pkPixelDesc->SetFOGENABLED(uiFogType != FOG_NONE);

    unsigned int uiAlphaTest = pkMatlDesc->GetALPHATEST();
    pkPixelDesc->SetALPHATEST(uiAlphaTest);

    if (NiShadowManager::GetShadowManager() && 
        NiShadowManager::GetActive())
    {
        unsigned int uiShadowMapsForLight =
            pkMatlDesc->GetSHADOWMAPFORLIGHT();
        pkPixelDesc->SetSHADOWMAPFORLIGHT(uiShadowMapsForLight);

        unsigned int uiShadowTechniqueSlot = pkMatlDesc->GetSHADOWTECHNIQUE();
        NiShadowTechnique* pkShadowTechnique = 
            NiShadowManager::GetActiveShadowTechnique(
            (unsigned short)uiShadowTechniqueSlot);

        pkPixelDesc->SetSHADOWTECHNIQUE(pkShadowTechnique->GetTechniqueID());
    }
    else
    {
        pkPixelDesc->SetSHADOWTECHNIQUE(0);
    }

    unsigned int uiDirLightCount = pkMatlDesc->GetDIRLIGHTCOUNT();
    unsigned int uiSpotLightCount = pkMatlDesc->GetSPOTLIGHTCOUNT();
    unsigned int uiPointLightCount = pkMatlDesc->GetPOINTLIGHTCOUNT();
    unsigned int uiShadowMapForLight = pkMatlDesc->GetSHADOWMAPFORLIGHT();
    unsigned int uiSpecular = pkMatlDesc->GetSPECULAR();

    unsigned int uiNumLights = uiDirLightCount + uiSpotLightCount + 
        uiPointLightCount;

    // If the apply mode is REPLACE, then no lighting takes place
    if (uiApplyMode == APPLY_REPLACE)
    {
        uiNumLights = 0;
        uiDirLightCount = 0;
        uiSpotLightCount = 0;
        uiPointLightCount = 0;
        uiSpecular = 0;
        uiPerVertexForLights = 1;
    }

    // Wait to set specular until after the apply mode has been taken into
    // consideration.
    pkPixelDesc->SetSPECULAR(uiSpecular);

    // If per-pixel lighting
    if (uiPerVertexForLights == 0)
    {
        pkPixelDesc->SetPOINTLIGHTCOUNT(uiPointLightCount);
        pkPixelDesc->SetSPOTLIGHTCOUNT(uiSpotLightCount);
        pkPixelDesc->SetDIRLIGHTCOUNT(uiDirLightCount);
        pkPixelDesc->SetSHADOWMAPFORLIGHT(uiShadowMapForLight);
        pkPixelDesc->SetAPPLYAMBIENT(true);
        pkPixelDesc->SetAPPLYEMISSIVE(true);

        if (uiSpecular != 0)
        {
            pkVertexDesc->SetOUTPUTWORLDVIEW(uiSpecular);
            pkPixelDesc->SetWORLDVIEW(1);
        }

        if (uiNumLights != 0)
        {
            pkVertexDesc->SetOUTPUTWORLDPOS(1);

            NIASSERT(bHasNormal);
            pkVertexDesc->SetOUTPUTWORLDNBT(1);

            pkPixelDesc->SetWORLDNORMAL(1);
            pkPixelDesc->SetWORLDPOSITION(1);
    
            pkVertexDesc->SetVERTEXLIGHTSONLY(0);
        }
        else
        {
            pkVertexDesc->SetVERTEXLIGHTSONLY(1);
            pkPixelDesc->SetPERVERTEXLIGHTING(1);
        }

    }
    else
    {   
        pkVertexDesc->SetPOINTLIGHTCOUNT(uiPointLightCount);

        pkVertexDesc->SetSPOTLIGHTCOUNT(uiSpotLightCount);

        pkVertexDesc->SetDIRLIGHTCOUNT(uiDirLightCount);

        pkVertexDesc->SetSPECULAR(uiSpecular);

        pkPixelDesc->SetAPPLYAMBIENT(false);

        pkPixelDesc->SetAPPLYEMISSIVE(false);

        pkVertexDesc->SetVERTEXLIGHTSONLY(1);
    }

    NIASSERT(pkVertexDesc->GetVERTEXLIGHTSONLY() ==
        pkPixelDesc->GetPERVERTEXLIGHTING());

    // Vertex Desc
    unsigned int uiTransform = pkMatlDesc->GetTRANSFORM();
    pkVertexDesc->SetTRANSFORM(uiTransform);

    unsigned int uiNormal = pkMatlDesc->GetNORMAL();

    if (uiNormal == NORMAL_NBT && 
        !pkMatlDesc->GetNORMALMAPCOUNT() &&
        !pkMatlDesc->GetPARALLAXMAPCOUNT())
    {
        // Only use full NBT frame if normal mapping is being used.
        uiNormal = NORMAL_ONLY;
    }
    pkVertexDesc->SetNORMAL(uiNormal);

    pkVertexDesc->SetFOGTYPE(uiFogType);
    
    unsigned int uiVertexColors = pkMatlDesc->GetVERTEXCOLORS();
    pkVertexDesc->SetVERTEXCOLORS(uiVertexColors);

    pkVertexDesc->SetPROJLIGHTMAPCOUNT(uiProjLightMapCount);
    pkVertexDesc->SetPROJSHADOWMAPCOUNT(uiProjShadowMapCount);

    unsigned int uiEnvMapType = pkMatlDesc->GetENVMAPTYPE();

    // If the normal map exists, we want the per-pixel normal to 
    // affect the environment map.
    if (uiNormalMapCount != 0)
    {
        NIASSERT(bHasNBT);       
        pkVertexDesc->SetOUTPUTWORLDNBT(1);
        pkPixelDesc->SetWORLDNBT(1);

        pkVertexDesc->SetENVMAPTYPE(TEXEFFECT_NONE);

        pkPixelDesc->SetENVMAPTYPE(uiEnvMapType);

        if (uiEnvMapType == TEXEFFECT_SPECULAR_CUBE || 
            uiEnvMapType == TEXEFFECT_SPHERICAL)
        {
            pkVertexDesc->SetOUTPUTWORLDVIEW(1);
            pkPixelDesc->SetWORLDVIEW(1);
        }
    }
    else
    {
        pkVertexDesc->SetENVMAPTYPE(uiEnvMapType);
        pkPixelDesc->SetENVMAPTYPE(uiEnvMapType);
    }

    if (uiParallaxCount != 0)
    {
        NIASSERT(bHasNBT);
        pkVertexDesc->SetOUTPUTTANGENTVIEW(1);
        pkVertexDesc->SetOUTPUTWORLDNBT(1);
        pkPixelDesc->SetWORLDNBT(1);
    }

    if (pkPixelDesc->GetPROJLIGHTMAPCLIPPED() != 0 ||
        pkPixelDesc->GetPROJSHADOWMAPCLIPPED() != 0)
    {
        pkVertexDesc->SetOUTPUTWORLDPOS(1);
        pkPixelDesc->SetWORLDPOSITION(1);
    }

    unsigned int auiUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiUVSets, UINT_MAX, sizeof(auiUVSets));

    TexGenOutput aeTexGenOutputs[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(aeTexGenOutputs, 0, sizeof(aeTexGenOutputs));

    unsigned int uiTextureCount = pkMatlDesc->GetStandardTextureCount();
    NIASSERT(uiTextureCount <= STANDARD_PIPE_MAX_TEXTURE_MAPS);

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkMatlDesc->GetTextureUsage(ui, auiUVSets[ui], aeTexGenOutputs[ui]);
    }

    AssignTextureCoordinates(auiUVSets, aeTexGenOutputs, uiTextureCount, 
        pkVertexDesc, pkPixelDesc);

    pkVertexDesc->SetAMBDIFFEMISSIVE(uiAmbDiffEmissive);
    pkVertexDesc->SetLIGHTINGMODE(uiLightingMode);
    pkVertexDesc->SetAPPLYMODE(uiApplyMode);

    uiCountAdded++;
    return RC_SUCCESS;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::AssignTextureCoordinates(
    const unsigned int* puiUVSets,
    const TexGenOutput* peTexGenOutputs,
    const unsigned int uiTextureCount,
    NiStandardVertexProgramDescriptor* pkVertexDesc, 
    NiStandardPixelProgramDescriptor* pkPixelDesc)
{
    NIASSERT(uiTextureCount <= STANDARD_PIPE_MAX_TEXTURE_MAPS);

    unsigned int auiTextureUVAssignments[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiTextureUVAssignments, UINT_MAX,
        sizeof(auiTextureUVAssignments));

    unsigned int auiUVIndicesUsed[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiUVIndicesUsed, 0, sizeof(auiUVIndicesUsed));

    // Remap uv sets from input to output locations.
    // Pass-thru UV Sets get first dibs.
    int iMax = -1;
    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        unsigned int uiUVSet = puiUVSets[ui];

        if (peTexGenOutputs[ui] == TEX_OUT_PASSTHRU)
        {
            auiTextureUVAssignments[ui] = uiUVSet;

            if ((int)uiUVSet > iMax)
                iMax = (int)uiUVSet;

            auiUVIndicesUsed[uiUVSet] = 1;
        }
    }

    // Fill in any empty slots so that all passthrus are contiguous
    int iRealOutputMax = -1;
    for (unsigned int ui = 0; iMax != -1 && ui <= (unsigned int) iMax ; ui++)
    {
        if (auiUVIndicesUsed[ui] == 0)
            continue;
        
        iRealOutputMax++;

        for (unsigned int uiTextureIdx = 0; uiTextureIdx < 
            uiTextureCount; uiTextureIdx++)
        {
            if (auiTextureUVAssignments[uiTextureIdx] == ui)
            {
                auiTextureUVAssignments[uiTextureIdx] = 
                    (unsigned int)iRealOutputMax;
            }
        }
    }

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        unsigned int uiUVSet = puiUVSets[ui];
        TexGenOutput eTexOutput = peTexGenOutputs[ui];

        if (eTexOutput == TEX_OUT_PASSTHRU)
        {
            pkPixelDesc->SetUVSetForMap(ui, auiTextureUVAssignments[ui]);
            pkVertexDesc->SetOutputUVUsage(auiTextureUVAssignments[ui], 
                uiUVSet, eTexOutput);
        }
        else
        {
            iRealOutputMax++;
            pkPixelDesc->SetUVSetForMap(ui, iRealOutputMax);
            pkVertexDesc->SetOutputUVUsage(iRealOutputMax, 
                uiUVSet, eTexOutput);
        }
    }

    if (iRealOutputMax != -1)
        pkVertexDesc->SetOUTPUTUVCOUNT(1 + (unsigned int)iRealOutputMax);
    else
        pkVertexDesc->SetOUTPUTUVCOUNT(0);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::RemoveTextureMap(TextureMap eMap, 
    NiStandardVertexProgramDescriptor* pkVertDesc, 
    NiStandardPixelProgramDescriptor* pkPixDesc)
{
    int iTextureIdx = MapIndexFromTextureEnum(eMap, pkPixDesc);
    if (iTextureIdx == -1)
        return false;

    unsigned int uiTextureIdx = (unsigned int) iTextureIdx;

    unsigned int auiInputUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    unsigned int auiTexIdxUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    TexGenOutput aeTexGenOutputs[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    unsigned int uiTextureCount = 0;
    unsigned int uiOutputUVCount = 0;

    if (false == GenerateVertexOutputUVArrays(auiInputUVSets, aeTexGenOutputs,
        auiTexIdxUVSets, uiTextureCount, uiOutputUVCount,
        pkVertDesc, pkPixDesc, STANDARD_PIPE_MAX_TEXTURE_MAPS))
    {
        return false;
    }

    if (uiTextureIdx > uiTextureCount)
        return false;

    unsigned int uiRemovedTexUV = auiTexIdxUVSets[uiTextureIdx];
    bool bOtherTexUsesUV = false;
    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        if (ui == uiTextureIdx)
            continue;

        if (auiTexIdxUVSets[ui] == uiRemovedTexUV)
        {
            bOtherTexUsesUV = true;
            break;
        }
    }

    // Remove the UV set if no one else is using it
    if (false == bOtherTexUsesUV)
    {
        for (unsigned int ui = uiTextureIdx; ui < uiTextureCount - 1; ui++)
        {
            unsigned int uiNextIdxUVSet = 0;
            if (ui >= uiTextureIdx)
                uiNextIdxUVSet = pkPixDesc->GetUVSetForMap(ui + 1);
            else
                uiNextIdxUVSet = pkPixDesc->GetUVSetForMap(ui);

            if (uiNextIdxUVSet > uiRemovedTexUV)
                uiNextIdxUVSet--;

            pkPixDesc->SetUVSetForMap(ui, uiNextIdxUVSet);
        }
        pkPixDesc->SetUVSetForMap(uiTextureCount - 1, 0);

        for (unsigned int ui = uiRemovedTexUV; ui < uiOutputUVCount - 1; ui++)
        {
            unsigned int uiInputUV;
            TexGenOutput eOutputUV;
            pkVertDesc->GetOutputUVUsage(ui + 1, uiInputUV, eOutputUV);
            pkVertDesc->SetOutputUVUsage(ui, uiInputUV, eOutputUV);
        }
        pkVertDesc->SetOutputUVUsage(uiOutputUVCount - 1, 0, 
            (TexGenOutput)0);

        uiOutputUVCount--;
        NiBitfieldSetIndexedField(
            NiStandardVertexProgramDescriptor::OUTPUTUVCOUNT,
            uiOutputUVCount, unsigned int, pkVertDesc->m_auiBitArray);
    }

    SetMapEnabled(eMap, false, pkPixDesc);

    {
        unsigned int uiNewOutputCount;
        GenerateVertexOutputUVArrays(auiInputUVSets, aeTexGenOutputs,
            auiTexIdxUVSets, uiTextureCount, uiNewOutputCount,
            pkVertDesc, pkPixDesc, STANDARD_PIPE_MAX_TEXTURE_MAPS);
        NIASSERT(uiOutputUVCount == uiNewOutputCount);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GenerateVertexOutputUVArrays(
    unsigned int* puiInputUVSets, TexGenOutput* peTexGenOutputs,
    unsigned int* puiOutputUVSetsByTexIndex,
    unsigned int& uiTextureCount, 
    unsigned int& uiOutputUVCount,
    NiStandardVertexProgramDescriptor* pkVertexDesc, 
    NiStandardPixelProgramDescriptor* pkPixelDesc, 
    unsigned int uiMaxTextureCount)
{
    uiTextureCount = pkPixelDesc->GetStandardTextureCount();
    uiOutputUVCount = pkPixelDesc->GetInputUVCount();

    memset(puiInputUVSets, UINT_MAX, sizeof(unsigned int)*uiMaxTextureCount);
    memset(peTexGenOutputs, 0,  sizeof(TexGenOutput) * uiMaxTextureCount);
    memset(puiOutputUVSetsByTexIndex, UINT_MAX, 
        sizeof(unsigned int) * uiMaxTextureCount);

    for (unsigned int uiUV = 0; uiUV < uiOutputUVCount; uiUV++)
    {
        pkVertexDesc->GetOutputUVUsage(uiUV, puiInputUVSets[uiUV], 
            peTexGenOutputs[uiUV]);
    }

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        puiOutputUVSetsByTexIndex[ui] = pkPixelDesc->GetUVSetForMap(ui);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::IsMapEnabled(TextureMap eMap, 
    NiStandardPixelProgramDescriptor* pkPixDesc)
{
    switch (eMap)
    {
        case MAP_MAX: 
        default:
            NIASSERT(!"Should not get here!");
            return false;
        case MAP_DECAL02: 
            return (pkPixDesc->GetDECALMAPCOUNT() > 2);
        case MAP_DECAL01: 
            return (pkPixDesc->GetDECALMAPCOUNT() > 1);
        case MAP_DECAL00: 
            return (pkPixDesc->GetDECALMAPCOUNT() > 0);
        case MAP_CUSTOM04: 
            return pkPixDesc->GetCUSTOMMAP04COUNT() == 1;
        case MAP_CUSTOM03:
            return pkPixDesc->GetCUSTOMMAP03COUNT() == 1;
        case MAP_CUSTOM02: 
            return pkPixDesc->GetCUSTOMMAP02COUNT() == 1;
        case MAP_CUSTOM01: 
            return pkPixDesc->GetCUSTOMMAP01COUNT() == 1;
        case MAP_CUSTOM00: 
            return pkPixDesc->GetCUSTOMMAP00COUNT() == 1;
        case MAP_GLOW: 
            return pkPixDesc->GetGLOWMAPCOUNT() == 1;
        case MAP_GLOSS: 
            return pkPixDesc->GetGLOSSMAPCOUNT() == 1;
        case MAP_BUMP: 
            return pkPixDesc->GetBUMPMAPCOUNT() == 1;
        case MAP_DETAIL:
            return pkPixDesc->GetDETAILMAPCOUNT() == 1;
        case MAP_DARK: 
            return pkPixDesc->GetDARKMAPCOUNT() == 1;
        case MAP_NORMAL: 
            return pkPixDesc->GetNORMALMAPCOUNT() == 1;
        case MAP_BASE: 
            return pkPixDesc->GetBASEMAPCOUNT() == 1;
        case MAP_PARALLAX: 
            return pkPixDesc->GetPARALLAXMAPCOUNT() == 1;
    }
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SetMapEnabled(TextureMap eMap, bool bEnabled,
    NiStandardPixelProgramDescriptor* pkPixDesc)
{
    if (IsMapEnabled(eMap, pkPixDesc) == bEnabled)
        return true;

    unsigned int uiNewState = 1;
    if (!bEnabled)
        uiNewState = 0;

    switch (eMap)
    {
        case MAP_MAX: 
        default:
            NIASSERT(!"Should not get here!");
            return false;
            break;
        case MAP_DECAL02: 
        case MAP_DECAL01: 
        case MAP_DECAL00: 
            {
                unsigned int uiDecalCount = pkPixDesc->GetDECALMAPCOUNT();
                if (bEnabled)
                    uiDecalCount++;
                else
                    uiDecalCount--;
                
                pkPixDesc->SetDECALMAPCOUNT(uiDecalCount);
            }
            break;
        case MAP_CUSTOM04: 
            pkPixDesc->SetCUSTOMMAP04COUNT(uiNewState); 
            break;
        case MAP_CUSTOM03:
            pkPixDesc->SetCUSTOMMAP03COUNT(uiNewState); 
            break;
        case MAP_CUSTOM02: 
            pkPixDesc->SetCUSTOMMAP02COUNT(uiNewState);
            break;
        case MAP_CUSTOM01: 
            pkPixDesc->SetCUSTOMMAP01COUNT(uiNewState); 
            break;
        case MAP_CUSTOM00: 
            pkPixDesc->SetCUSTOMMAP00COUNT(uiNewState); 
            break;
        case MAP_GLOW: 
            pkPixDesc->SetGLOWMAPCOUNT(uiNewState); 
            break;
        case MAP_GLOSS: 
            pkPixDesc->SetGLOSSMAPCOUNT(uiNewState); 
            break;
        case MAP_BUMP: 
            pkPixDesc->SetBUMPMAPCOUNT(uiNewState); 
            break;
        case MAP_DETAIL:
            pkPixDesc->SetDETAILMAPCOUNT(uiNewState); 
            break;
        case MAP_DARK: 
            pkPixDesc->SetDARKMAPCOUNT(uiNewState); 
            break;
        case MAP_NORMAL: 
            pkPixDesc->SetNORMALMAPCOUNT(uiNewState); 
            break;
        case MAP_BASE: 
            pkPixDesc->SetBASEMAPCOUNT(uiNewState);  
            break;
        case MAP_PARALLAX: 
            pkPixDesc->SetPARALLAXMAPCOUNT(uiNewState);  
            break;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SplitPerPixelLights(
    NiMaterialDescriptor*, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int&)
{
    // This function can only deal with failed pixel shader compiles...
    if ((eFailedRC & RC_COMPILE_FAILURE_PIXEL) == 0)
        return false;

    // This function adds an additional pass - make sure there's room
    if (uiCount == uiMaxCount - 1)
        return false;

    unsigned int uiOriginalCount = uiCount;
    unsigned int uiNewPass = uiFailedPass + 1;

    NiStandardPixelProgramDescriptor* pkInvalidDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kPixelDesc;
    unsigned int uiPerVertexLighting = pkInvalidDesc->GetPERVERTEXLIGHTING();

    // If all lighting is per-vertex, this function won't work
    if (uiPerVertexLighting == 1)
        return false;

    unsigned int uiDirLightCount = pkInvalidDesc->GetDIRLIGHTCOUNT();
    unsigned int uiSpotLightCount = pkInvalidDesc->GetSPOTLIGHTCOUNT();
    unsigned int uiPointLightCount = pkInvalidDesc->GetPOINTLIGHTCOUNT();
    bool bApplyAmbient = pkInvalidDesc->GetAPPLYAMBIENT() != 0;
    bool bApplyEmissive =  pkInvalidDesc->GetAPPLYEMISSIVE() != 0;

    // Can't do anything if there is only one light in this pass
    unsigned int uiTotalLightCount = uiDirLightCount + uiSpotLightCount + 
        uiPointLightCount;
    if ((uiTotalLightCount == 0 || 
        uiTotalLightCount == 1 && !bApplyAmbient))
    {
        return false;
    }

    NiOutputDebugString("Trying to distribute per-pixel lights\n");

    // Make room for new pass immediately after failed pass, and 
    // duplicate failed pass to new pass
    for (unsigned int i = uiOriginalCount; i > uiFailedPass; i--)
    {
        pkRenderPasses[i] = pkRenderPasses[i - 1];
    }

    // Remove any existing offsets from the new pass, since they will be
    // recalculated.
    pkRenderPasses[uiNewPass].m_kObjectOffsets.RemoveAll();
    // Disable lights reset on new pass
    pkRenderPasses[uiNewPass].m_bResetObjectOffsets = false;

    NiStandardPixelProgramDescriptor* pkNewDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiNewPass].m_kPixelDesc;

    NIASSERT(pkInvalidDesc->m_kIdentifier ==
        "NiStandardPixelProgramDescriptor");

    unsigned int uiNewLightCount = uiTotalLightCount - uiTotalLightCount / 2;
    unsigned int uiNumNewLightsLeft = uiNewLightCount;

    unsigned int uiNewSpotLightCount = uiSpotLightCount - 
        uiSpotLightCount / 2;
    if (uiNewSpotLightCount > uiNumNewLightsLeft)
        uiNewSpotLightCount = uiNumNewLightsLeft;
    uiNumNewLightsLeft -= uiNewSpotLightCount;

    unsigned int uiNewDirLightCount = uiDirLightCount - uiDirLightCount / 2;
    if (uiNewDirLightCount > uiNumNewLightsLeft)
        uiNewDirLightCount = uiNumNewLightsLeft;
    uiNumNewLightsLeft -= uiNewDirLightCount;

    unsigned int uiNewPointLightCount = uiPointLightCount - 
        uiPointLightCount / 2;
    if (uiNewPointLightCount > uiNumNewLightsLeft)
        uiNewPointLightCount = uiNumNewLightsLeft;
    uiNumNewLightsLeft -= uiNewPointLightCount;

    NIASSERT(uiNumNewLightsLeft == 0);

    unsigned int uiPreviousDirLightCount = 
        uiDirLightCount - uiNewDirLightCount;
    unsigned int uiPreviousPointLightCount = 
        uiPointLightCount - uiNewPointLightCount;
    unsigned int uiPreviousSpotLightCount = 
        uiSpotLightCount - uiNewSpotLightCount;

    NIASSERT(uiNewDirLightCount + uiNewPointLightCount + 
        uiNewSpotLightCount == uiNewLightCount);
    NIASSERT(uiNewLightCount + uiPreviousDirLightCount + 
        uiPreviousPointLightCount + uiPreviousSpotLightCount == 
        uiTotalLightCount);

    pkNewDesc->SetDIRLIGHTCOUNT(uiNewDirLightCount);
    pkNewDesc->SetPOINTLIGHTCOUNT(uiNewPointLightCount);
    pkNewDesc->SetSPOTLIGHTCOUNT(uiNewSpotLightCount);
    pkNewDesc->SetAPPLYAMBIENT(false);
    pkNewDesc->SetAPPLYEMISSIVE(false);

    // Update shadow map bitfields
    pkNewDesc->SetSHADOWMAPFORLIGHT(0);

    unsigned int uiPreviousShadowBitfield = 
        pkInvalidDesc->GetSHADOWMAPFORLIGHT();
    unsigned int uiNewShadowBitfield = 0;
    unsigned int uiNewLight = 0;
    unsigned int uiPreviousLight = 0;

    uiPreviousLight += uiPreviousPointLightCount;
    for (unsigned int ui = 0; ui < uiNewPointLightCount; ui++)
    {
        unsigned int uiMask = NiTGetBitMask<unsigned int>(uiPreviousLight, 1);
        bool bShadow = 
            NiTGetBit<unsigned int>(uiPreviousShadowBitfield, uiMask);
        if (bShadow)
        {
            NiTSetBit<unsigned int>(uiPreviousShadowBitfield, 0, uiMask);
            uiMask = NiTGetBitMask<unsigned int>(uiNewLight, 1);
            uiNewShadowBitfield |= uiMask;
        }
        uiNewLight++;
        uiPreviousLight++;
    }

    uiPreviousLight += uiPreviousDirLightCount;
    for (unsigned int ui = 0; ui < uiNewDirLightCount; ui++)
    {
        unsigned int uiMask = NiTGetBitMask<unsigned int>(uiPreviousLight, 1);
        bool bShadow = 
            NiTGetBit<unsigned int>(uiPreviousShadowBitfield, uiMask);
        if (bShadow)
        {
            NiTSetBit<unsigned int>(uiPreviousShadowBitfield, 0, uiMask);
            uiMask = NiTGetBitMask<unsigned int>(uiNewLight, 1);
            uiNewShadowBitfield |= uiMask;
        }
        uiNewLight++;
        uiPreviousLight++;
    }

    uiPreviousLight += uiPreviousSpotLightCount;
    for (unsigned int ui = 0; ui < uiNewSpotLightCount; ui++)
    {
        unsigned int uiMask = NiTGetBitMask<unsigned int>(uiPreviousLight, 1);
        bool bShadow = 
            NiTGetBit<unsigned int>(uiPreviousShadowBitfield, uiMask);
        if (bShadow)
        {
            NiTSetBit<unsigned int>(uiPreviousShadowBitfield, 0, uiMask);
            uiMask = NiTGetBitMask<unsigned int>(uiNewLight, 1);
            uiNewShadowBitfield |= uiMask;
        }
        uiNewLight++;
        uiPreviousLight++;
    }

    NIASSERT(uiNewLightCount == uiNewLight);
    NIASSERT(uiPreviousLight == uiTotalLightCount);
    pkNewDesc->SetSHADOWMAPFORLIGHT(uiNewShadowBitfield);
    pkInvalidDesc->SetSHADOWMAPFORLIGHT(uiPreviousShadowBitfield);


    pkRenderPasses[uiNewPass].m_bAlphaOverride = true;
    pkRenderPasses[uiNewPass].m_bAlphaBlend = true;
    pkRenderPasses[uiNewPass].m_bUsePreviousSrcBlendMode = true;
    pkRenderPasses[uiNewPass].m_bUsePreviousDestBlendMode = false;
    pkRenderPasses[uiNewPass].m_eSrcBlendMode = 
        NiAlphaProperty::ALPHA_ONE;
    pkRenderPasses[uiNewPass].m_eDestBlendMode = 
        NiAlphaProperty::ALPHA_ONE;

    if (uiNewDirLightCount != 0)
    {
        ObjectResourceOffset kOffset;
        kOffset.m_eObjectType = 
            NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT;
        kOffset.m_eProgramType = NiGPUProgram::PROGRAM_PIXEL;
        kOffset.m_uiOffset = GetHighestObjectOffset(kOffset.m_eObjectType, 
            pkRenderPasses, uiOriginalCount) + uiPreviousDirLightCount;
        pkRenderPasses[uiNewPass].m_kObjectOffsets.Add(kOffset);
    }

    if (uiNewSpotLightCount != 0)
    {
        ObjectResourceOffset kOffset;
        kOffset.m_eObjectType = NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT;
        kOffset.m_eProgramType = NiGPUProgram::PROGRAM_PIXEL;
        kOffset.m_uiOffset = GetHighestObjectOffset(kOffset.m_eObjectType, 
            pkRenderPasses, uiOriginalCount) + uiPreviousSpotLightCount;
        pkRenderPasses[uiNewPass].m_kObjectOffsets.Add(kOffset);
    }

    if (uiNewPointLightCount != 0)
    {
        ObjectResourceOffset kOffset;
        kOffset.m_eObjectType = NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT;
        kOffset.m_eProgramType = NiGPUProgram::PROGRAM_PIXEL;        
        kOffset.m_uiOffset = GetHighestObjectOffset(kOffset.m_eObjectType, 
            pkRenderPasses, uiOriginalCount) + uiPreviousPointLightCount;
        pkRenderPasses[uiNewPass].m_kObjectOffsets.Add(kOffset);
    }

    pkInvalidDesc->SetDIRLIGHTCOUNT(uiPreviousDirLightCount);
    pkInvalidDesc->SetPOINTLIGHTCOUNT(uiPreviousPointLightCount);
    pkInvalidDesc->SetSPOTLIGHTCOUNT(uiPreviousSpotLightCount);
    pkInvalidDesc->SetAPPLYAMBIENT(bApplyAmbient);
    pkInvalidDesc->SetAPPLYEMISSIVE(bApplyEmissive);

    uiCount++;

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::DropShadowMaps(
    NiMaterialDescriptor*, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int, unsigned int&, 
    unsigned int&)
{
    // This function can only deal with failed pixel shader compiles...
    if ((eFailedRC & RC_COMPILE_FAILURE_PIXEL) == 0)
        return false;

    NiStandardPixelProgramDescriptor* pkInvalidDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kPixelDesc;

    // This function only works for shaders with shadow maps
    if (!pkInvalidDesc->GetSHADOWMAPFORLIGHT())
        return false;

    NiOutputDebugString("Trying to remove all shadow maps.\n");

    pkInvalidDesc->SetSHADOWMAPFORLIGHT(0);

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::DropShadowMapsThenSplitPerPixelLights(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int& uiFunctionData)
{
    // Attempt to remove the shadow maps, 
    // and then split up the per-pixel lights

    // The first time this function is encountered, uiFunctionData should be 0
    if (uiFunctionData == 0)
    {
        uiFunctionData = 1;
        // If DropShadowMaps returns false (meaning it can't do anything)
        // then there's no point continuing this fallback either.
        return DropShadowMaps(pkMaterialDescriptor, eFailedRC,
            uiFailedPass, pkRenderPasses, uiMaxCount, 
            uiCount, uiFunctionData);
    }

    // In subsequent iterations, attempt to split up the per-pixel lights
    return SplitPerPixelLights(pkMaterialDescriptor, eFailedRC,
        uiFailedPass, pkRenderPasses, uiMaxCount, uiCount, uiFunctionData);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SplitPerVertexLights(
    NiMaterialDescriptor*, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int&)
{
    // This function can only deal with failed pixel shader compiles...
    if ((eFailedRC & RC_COMPILE_FAILURE_VERTEX) == 0)
        return false;

    // This function adds an additional pass - make sure there's room
    if (uiCount == uiMaxCount - 1)
        return false;

    unsigned int uiOriginalCount = uiCount;
    unsigned int uiNewPass = uiFailedPass + 1;
    NiStandardVertexProgramDescriptor* pkInvalidDesc = 
        (NiStandardVertexProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kVertexDesc;

    unsigned int uiPerVertexLighting = ((NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kPixelDesc)->GetPERVERTEXLIGHTING();

    unsigned int uiDirLightCount = pkInvalidDesc->GetDIRLIGHTCOUNT();
    unsigned int uiSpotLightCount = pkInvalidDesc->GetSPOTLIGHTCOUNT();
    unsigned int uiPointLightCount = pkInvalidDesc->GetPOINTLIGHTCOUNT();
    unsigned int uiTotalLightCount = uiDirLightCount + uiSpotLightCount + 
        uiPointLightCount;

    // If there are no per-vertex lights, this function won't work
    if (uiPerVertexLighting == 0 || uiTotalLightCount == 0)
        return false;

    NiOutputDebugString("Trying to distribute per-vertex lights\n");

    // Make room for new pass immediately after failed pass, and 
    // duplicate failed pass to new pass
    for (unsigned int i = uiOriginalCount; i > uiFailedPass; i--)
    {
        pkRenderPasses[i] = pkRenderPasses[i - 1];
    }

    // Remove any existing offsets from the new pass, since they will be
    // recalculated.
    pkRenderPasses[uiNewPass].m_kObjectOffsets.RemoveAll();
    // Disable lights reset on new pass
    pkRenderPasses[uiNewPass].m_bResetObjectOffsets = false;

    NiStandardVertexProgramDescriptor* pkNewDesc = 
        (NiStandardVertexProgramDescriptor*)
        &pkRenderPasses[uiNewPass].m_kVertexDesc;

    NIASSERT(pkInvalidDesc->m_kIdentifier == 
        "NiStandardVertexProgramDescriptor");

    unsigned int uiNewLightCount = uiTotalLightCount - uiTotalLightCount / 2;
    unsigned int uiNumNewLightsLeft = uiNewLightCount;

    unsigned int uiNewSpotLightCount = 
        uiSpotLightCount - uiSpotLightCount / 2;
    if (uiNewSpotLightCount > uiNumNewLightsLeft)
        uiNewSpotLightCount = uiNumNewLightsLeft;
    uiNumNewLightsLeft -= uiNewSpotLightCount;

    unsigned int uiNewPointLightCount = uiPointLightCount - 
        uiPointLightCount / 2;
    if (uiNewPointLightCount > uiNumNewLightsLeft)
        uiNewPointLightCount = uiNumNewLightsLeft;
    uiNumNewLightsLeft -= uiNewPointLightCount;

    unsigned int uiNewDirLightCount = uiDirLightCount - uiDirLightCount / 2;
    if (uiNewDirLightCount > uiNumNewLightsLeft)
        uiNewDirLightCount = uiNumNewLightsLeft;
    uiNumNewLightsLeft -= uiNewDirLightCount;

    NIASSERT(uiNumNewLightsLeft == 0);

    unsigned int uiPreviousDirLightCount = 
        uiDirLightCount - uiNewDirLightCount;
    unsigned int uiPreviousPointLightCount = 
        uiPointLightCount - uiNewPointLightCount;
    unsigned int uiPreviousSpotLightCount = 
        uiSpotLightCount - uiNewSpotLightCount;

    NIASSERT(uiNewDirLightCount + uiNewPointLightCount + 
        uiNewSpotLightCount == uiNewLightCount);
    NIASSERT(uiNewLightCount + uiPreviousDirLightCount + 
        uiPreviousPointLightCount + uiPreviousSpotLightCount == 
        uiTotalLightCount);

    pkNewDesc->SetDIRLIGHTCOUNT(uiNewDirLightCount);
    pkNewDesc->SetPOINTLIGHTCOUNT(uiNewPointLightCount);
    pkNewDesc->SetSPOTLIGHTCOUNT(uiNewSpotLightCount);

    pkRenderPasses[uiNewPass].m_bAlphaOverride = true;
    pkRenderPasses[uiNewPass].m_bAlphaBlend = true;    
    pkRenderPasses[uiNewPass].m_bUsePreviousSrcBlendMode = true;
    pkRenderPasses[uiNewPass].m_bUsePreviousDestBlendMode = false;
    pkRenderPasses[uiNewPass].m_eSrcBlendMode = 
        NiAlphaProperty::ALPHA_ONE;
    pkRenderPasses[uiNewPass].m_eDestBlendMode = 
        NiAlphaProperty::ALPHA_ONE;

    if (uiNewDirLightCount != 0)
    {
        ObjectResourceOffset kOffset;
        kOffset.m_eObjectType = 
            NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT;
        kOffset.m_eProgramType = NiGPUProgram::PROGRAM_VERTEX;
        kOffset.m_uiOffset = GetHighestObjectOffset(kOffset.m_eObjectType, 
            pkRenderPasses, uiOriginalCount) + uiPreviousDirLightCount;
        pkRenderPasses[uiNewPass].m_kObjectOffsets.Add(kOffset);
    }

    if (uiNewSpotLightCount != 0)
    {
        ObjectResourceOffset kOffset;
        kOffset.m_eObjectType = NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT;
        kOffset.m_eProgramType = NiGPUProgram::PROGRAM_VERTEX;
        kOffset.m_uiOffset = GetHighestObjectOffset(kOffset.m_eObjectType, 
            pkRenderPasses, uiOriginalCount) + uiPreviousSpotLightCount;
        pkRenderPasses[uiNewPass].m_kObjectOffsets.Add(kOffset);
    }

    if (uiNewPointLightCount != 0)
    {
        ObjectResourceOffset kOffset;
        kOffset.m_eObjectType = NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT;
        kOffset.m_eProgramType = NiGPUProgram::PROGRAM_VERTEX;        
        kOffset.m_uiOffset = GetHighestObjectOffset(kOffset.m_eObjectType, 
            pkRenderPasses, uiOriginalCount) + uiPreviousPointLightCount;
        pkRenderPasses[uiNewPass].m_kObjectOffsets.Add(kOffset);
    }

    pkInvalidDesc->SetDIRLIGHTCOUNT(uiPreviousDirLightCount);
    pkInvalidDesc->SetPOINTLIGHTCOUNT(uiPreviousPointLightCount);
    pkInvalidDesc->SetSPOTLIGHTCOUNT(uiPreviousSpotLightCount);

    uiCount++;

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SplitTextureMaps(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int&)
{
    // This function can only deal with failed pixel or vertex shader compiles
    if ((eFailedRC & RC_COMPILE_FAILURE_PIXEL) == 0 &&
        (eFailedRC & RC_COMPILE_FAILURE_VERTEX) == 0)
    {
        return false;
    }

    // This function adds an additional pass - make sure there's room
    if (uiCount == uiMaxCount - 1)
        return false;

    unsigned int uiOriginalCount = uiCount;
    unsigned int uiNewPass = uiFailedPass + 1;

    // This function pushes the following textures to a later pass: 
    // Environment, bump, and glow
    // If these maps are missing, then this function can't do anything.
    // Any parallax and normal maps will also be needed, as well as the
    // gloss map and the alpha component of the base map
    NiStandardPixelProgramDescriptor* pkInvalidPixelDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kPixelDesc;
    NiStandardVertexProgramDescriptor* pkInvalidVertexDesc = 
        (NiStandardVertexProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kVertexDesc;

    bool bEnvMap = pkInvalidPixelDesc->GetENVMAPTYPE() != 
        TEXEFFECT_NONE;
    bool bSpecular = pkInvalidPixelDesc->GetSPECULAR() != 0;
    bool bPerPixelLighting = pkInvalidPixelDesc->GetPERVERTEXLIGHTING() == 0;
    unsigned int uiPerPixelLights = pkInvalidPixelDesc->GetDIRLIGHTCOUNT() +
        pkInvalidPixelDesc->GetSPOTLIGHTCOUNT() +
        pkInvalidPixelDesc->GetPOINTLIGHTCOUNT();

    unsigned int uiGlowMapCount = pkInvalidPixelDesc->GetGLOWMAPCOUNT();

    if (!bEnvMap && uiGlowMapCount == 0)
        return false;

    NiOutputDebugString("Trying to distribute textures\n");

    // Make room for new pass immediately after failed pass, and 
    // duplicate failed pass to new pass
    for (unsigned int i = uiOriginalCount; i > uiFailedPass; i--)
    {
        pkRenderPasses[i] = pkRenderPasses[i - 1];
    }

    // No need to change object offsets - the new pass will have the same
    // lighting behavior as the failed pass
    NiStandardPixelProgramDescriptor* pkNewPixelDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiNewPass].m_kPixelDesc;
    NiStandardVertexProgramDescriptor* pkNewVertexDesc = 
        (NiStandardVertexProgramDescriptor*)
        &pkRenderPasses[uiNewPass].m_kVertexDesc;

    NiStandardMaterialDescriptor* pkMatlDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    // Prepare first pass
    unsigned int auiUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiUVSets, UINT_MAX, sizeof(auiUVSets));

    TexGenOutput aeTexGenOutputs[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(aeTexGenOutputs, 0, sizeof(aeTexGenOutputs));

    unsigned int uiTextureCount = 0;

    // Store texture coordinate information from maps in first pass
    if (pkInvalidPixelDesc->GetPARALLAXMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_PARALLAX, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetBASEMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_BASE, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetNORMALMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_NORMAL, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetDARKMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_DARK, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetDETAILMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_DETAIL, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (bSpecular && pkInvalidPixelDesc->GetGLOSSMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_GLOSS, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP00COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM00, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP01COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM01, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP02COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM02, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP03COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM03, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP04COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM04, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    unsigned int uiDecalMapCount = pkInvalidPixelDesc->GetDECALMAPCOUNT();
    for (unsigned int i = 0; i < uiDecalMapCount; i++)
    {
        int iTexture = MapIndexFromTextureEnum((TextureMap)(MAP_DECAL00 + i), 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    // Turn off environment, bump, and glow maps in first pass,
    // as well as gloss if not using specular
    pkInvalidVertexDesc->SetENVMAPTYPE(TEXEFFECT_NONE);

    if (!bSpecular || !bPerPixelLighting)
    {
        pkInvalidVertexDesc->SetOUTPUTWORLDVIEW(0);

        pkInvalidPixelDesc->SetWORLDVIEW(0);
    }

    pkInvalidPixelDesc->SetENVMAPTYPE(TEXEFFECT_NONE);

    pkInvalidPixelDesc->SetBUMPMAPCOUNT(0);
    pkInvalidPixelDesc->SetGLOWMAPCOUNT(0);

    if (!bSpecular)
    {
        pkInvalidPixelDesc->SetGLOSSMAPCOUNT(0);
    }

    // Re-apply texture coordinates for first pass
    AssignTextureCoordinates(auiUVSets, aeTexGenOutputs, uiTextureCount, 
        pkInvalidVertexDesc, pkInvalidPixelDesc);

    // Prepare second pass
    memset(auiUVSets, UINT_MAX, sizeof(auiUVSets));
    memset(aeTexGenOutputs, 0, sizeof(aeTexGenOutputs));

    uiTextureCount = 0;

    // Store texture coordinate information from maps in second pass
    if (pkNewPixelDesc->GetPARALLAXMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_PARALLAX, 
            pkNewPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkNewPixelDesc->GetBASEMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_BASE, 
            pkNewPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
            
            pkNewPixelDesc->SetBASEMAPALPHAONLY(1);
        }
    }

    if (pkNewPixelDesc->GetNORMALMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_NORMAL, 
            pkNewPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkNewPixelDesc->GetBUMPMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_BUMP, 
            pkNewPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkNewPixelDesc->GetGLOSSMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_GLOSS, 
            pkNewPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkNewPixelDesc->GetGLOWMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_GLOW, 
            pkNewPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }
    

    // Turn off base, dark, detail, and decal maps in second pass, as well as
    // projected lights and shadows
    // (Parallax, normal, and gloss maps will still be applied in both)
    pkNewVertexDesc->SetPROJLIGHTMAPCOUNT(0);
    pkNewVertexDesc->SetPROJSHADOWMAPCOUNT(0);
    if (!bPerPixelLighting && uiPerPixelLights == 0)
    {
        pkInvalidVertexDesc->SetOUTPUTWORLDPOS(0);
        pkInvalidPixelDesc->SetWORLDPOSITION(0);
    }

    
    pkNewPixelDesc->SetDARKMAPCOUNT(0);
    pkNewPixelDesc->SetDETAILMAPCOUNT(0);
    pkNewPixelDesc->SetDECALMAPCOUNT(0);
    pkNewPixelDesc->SetPROJLIGHTMAPCOUNT(0);
    pkNewPixelDesc->SetPROJSHADOWMAPCOUNT(0);

    // Remove the lights
    pkNewPixelDesc->SetPOINTLIGHTCOUNT(0);
    pkNewPixelDesc->SetSPOTLIGHTCOUNT(0);
    pkNewPixelDesc->SetDIRLIGHTCOUNT(0);
    pkNewPixelDesc->SetAPPLYAMBIENT(0);
    pkNewPixelDesc->SetAPPLYEMISSIVE(0);

    // Since only lighting and projected texture effects need the WorldPos,
    // make sure that it isn't used by this vertex shader.
    pkNewVertexDesc->SetOUTPUTWORLDPOS(0);
    pkNewPixelDesc->SetWORLDPOSITION(0);
    
    // Re-apply texture coordinates for first pass
    AssignTextureCoordinates(auiUVSets, aeTexGenOutputs, uiTextureCount, 
        pkNewVertexDesc, pkNewPixelDesc);

    // Set second pass to additive mode
    pkRenderPasses[uiNewPass].m_bAlphaOverride = true;
    pkRenderPasses[uiNewPass].m_bAlphaBlend = true;    
    pkRenderPasses[uiNewPass].m_bUsePreviousSrcBlendMode = true;
    pkRenderPasses[uiNewPass].m_bUsePreviousDestBlendMode = false;
    pkRenderPasses[uiNewPass].m_eSrcBlendMode = 
        NiAlphaProperty::ALPHA_ONE;
    pkRenderPasses[uiNewPass].m_eDestBlendMode = 
        NiAlphaProperty::ALPHA_ONE;

    uiCount++;


    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::DropShadowMapsThenSplitTextureMaps(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int& uiFunctionData)
{
    // Attempt to remove the shadow maps, 
    // and then split up the textures

    // The first time this function is encountered, uiFunctionData should be 0
    if (uiFunctionData == 0)
    {
        uiFunctionData = 1;
        // If DropShadowMaps returns false (meaning it can't do anything)
        // then there's no point continuing this fallback either.
        return DropShadowMaps(pkMaterialDescriptor, eFailedRC,
            uiFailedPass, pkRenderPasses, uiMaxCount,
            uiCount, uiFunctionData);
    }

    // In subsequent iterations, attempt to split up the texture maps.
    return SplitTextureMaps(pkMaterialDescriptor, eFailedRC,
        uiFailedPass, pkRenderPasses, uiMaxCount, uiCount, uiFunctionData);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::DropParallaxMap(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int, unsigned int&, 
    unsigned int&)
{
    // This function can only deal with failed pixel or vertex shader compiles
    if ((eFailedRC & RC_COMPILE_FAILURE_PIXEL) == 0 &&
        (eFailedRC & RC_COMPILE_FAILURE_VERTEX) == 0)
    {
        return false;
    }

    NiStandardPixelProgramDescriptor* pkInvalidPixelDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kPixelDesc;
    NiStandardVertexProgramDescriptor* pkInvalidVertexDesc = 
        (NiStandardVertexProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kVertexDesc;

    // This function simply drops the parallax map.
    unsigned int uiParallaxMapCount = 
        pkInvalidPixelDesc->GetPARALLAXMAPCOUNT();

    if (uiParallaxMapCount == 0)
        return false;

    NiOutputDebugString("Trying to remove parallax map\n");

    bool bPerPixelLighting = 
        pkInvalidPixelDesc->GetPERVERTEXLIGHTING() == 0;
    unsigned int uiPerPixelLights =
        pkInvalidPixelDesc->GetDIRLIGHTCOUNT() +
        pkInvalidPixelDesc->GetSPOTLIGHTCOUNT() +
        pkInvalidPixelDesc->GetPOINTLIGHTCOUNT();

    NiStandardMaterialDescriptor* pkMatlDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    // Prepare first pass
    unsigned int auiUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiUVSets, UINT_MAX, sizeof(auiUVSets));

    TexGenOutput aeTexGenOutputs[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(aeTexGenOutputs, 0, sizeof(aeTexGenOutputs));

    unsigned int uiTextureCount = 0;

    if (pkInvalidPixelDesc->GetBASEMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_BASE, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    unsigned int uiNormalMapCount = pkInvalidPixelDesc->GetNORMALMAPCOUNT();
    if (uiNormalMapCount != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_NORMAL, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetDARKMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_DARK, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetDETAILMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_DETAIL, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetBUMPMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_BUMP, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetGLOSSMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_GLOSS, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetGLOWMAPCOUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_GLOW, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP00COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM00, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP01COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM01, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP02COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM02, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP03COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM03, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    if (pkInvalidPixelDesc->GetCUSTOMMAP04COUNT() != 0)
    {
        int iTexture = MapIndexFromTextureEnum(MAP_CUSTOM04, 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    unsigned int uiDecalMapCount = pkInvalidPixelDesc->GetDECALMAPCOUNT();
    for (unsigned int i = 0; i < uiDecalMapCount; i++)
    {
        int iTexture = MapIndexFromTextureEnum((TextureMap)(MAP_DECAL00 + i), 
            pkInvalidPixelDesc);
        if (iTexture >= 0)
        {
            pkMatlDesc->GetTextureUsage(iTexture, auiUVSets[uiTextureCount], 
                aeTexGenOutputs[uiTextureCount]);
            uiTextureCount++;
        }
    }

    // Turn off parallax map and associated calculations
    pkInvalidPixelDesc->SetPARALLAXMAPCOUNT(0);
    pkInvalidVertexDesc->SetOUTPUTTANGENTVIEW(0);

    if ((!bPerPixelLighting || uiPerPixelLights == 0) &&
        uiNormalMapCount == 0)
    {
        pkInvalidVertexDesc->SetOUTPUTWORLDNBT(0);
        pkInvalidPixelDesc->SetWORLDNORMAL(0);
        pkInvalidPixelDesc->SetWORLDNBT(0);
    }

    // Re-apply texture coordinates
    AssignTextureCoordinates(auiUVSets, aeTexGenOutputs, uiTextureCount, 
        pkInvalidVertexDesc, pkInvalidPixelDesc);

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::DropParallaxMapThenSplitLights(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int& uiFunctionData)
{
    // Attempt to remove the parallax map, 
    // and then split up the per-pixel lights

    // The first time this function is encountered, uiFunctionData should be 0
    if (uiFunctionData == 0)
    {
        uiFunctionData = 1;
        // If DropParallaxMap returns false (meaning it can't do anything)
        // then there's no point continuing this fallback either.
        return DropParallaxMap(pkMaterialDescriptor, eFailedRC,
            uiFailedPass, pkRenderPasses, uiMaxCount,
            uiCount, uiFunctionData);
    }

    // In subsequent iterations, attempt to split up the per-pixel lights.
    return SplitPerPixelLights(pkMaterialDescriptor, eFailedRC,
        uiFailedPass, pkRenderPasses, uiMaxCount, uiCount, uiFunctionData);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::DropShadowMapsThenDropParallaxMapThenSplitLights(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int& uiFunctionData)
{
    // Attempt to remove the shadow maps, 
    // and then remove the parallax map, 
    // and then split up the per-pixel lights

    // The first time this function is encountered, uiFunctionData should be 0
    if (uiFunctionData == 0)
    {
        uiFunctionData = 1;
        // If DropShadowMaps returns false (meaning it can't do anything)
        // then there's no point continuing this fallback either.
        return DropShadowMaps(pkMaterialDescriptor, eFailedRC,
            uiFailedPass, pkRenderPasses, uiMaxCount,
            uiCount, uiFunctionData);
    }

    // The second time this function is encountered, uiFunctionData should be
    // 1
    if (uiFunctionData == 1)
    {
        uiFunctionData = 2;
        // If DropParallaxMap returns false (meaning it can't do anything)
        // then there's no point continuing this fallback either.
        return DropParallaxMap(pkMaterialDescriptor, eFailedRC,
            uiFailedPass, pkRenderPasses, uiMaxCount, 
            uiCount, uiFunctionData);
    }

    // In subsequent iterations, attempt to split up the per-pixel lights
    return SplitPerPixelLights(pkMaterialDescriptor, eFailedRC,
        uiFailedPass, pkRenderPasses, uiMaxCount, uiCount, uiFunctionData);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GenerateVertexShadeTree(Context& kContext, 
    NiGPUProgramDescriptor* pkDesc)
{   
    NIASSERT(pkDesc->m_kIdentifier == "NiStandardVertexProgramDescriptor");
    NiStandardVertexProgramDescriptor* pkVertexDesc = 
        (NiStandardVertexProgramDescriptor*)pkDesc;

    kContext.m_spConfigurator->SetDescription(pkVertexDesc->ToString());

    // Add vertex in, vertex out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_VERTEX))
    {
        return false;
    }

    // Add projected position out
    NiMaterialResource* pkVertOutProjPos = 
        kContext.m_spOutputs->AddInputResource("float4", "Position", "World", 
        "PosProjected");
    
    // Handle transform pipeline
    NiMaterialResource* pkWorldPos = NULL;
    NiMaterialResource* pkWorldNormal = NULL;
    NiMaterialResource* pkWorldView = NULL;
    NiMaterialResource* pkWorldReflect = NULL;
    NiMaterialResource* pkViewPos = NULL;
    NiMaterialResource* pkProjPos = NULL;

    bool bForceWorldView = false;
    bool bForceViewPos = false;

    unsigned int uiPointLights = pkVertexDesc->GetPOINTLIGHTCOUNT();
    unsigned int uiDirLights = pkVertexDesc->GetDIRLIGHTCOUNT();
    unsigned int uiSpotLights = pkVertexDesc->GetSPOTLIGHTCOUNT();
    bool bSpecularOn = pkVertexDesc->GetSPECULAR() == 1;

    unsigned int uiNumPerVertexLights = uiPointLights + uiDirLights + 
        uiSpotLights; 

    TexEffectType eEnvMapType = (TexEffectType) pkVertexDesc->GetENVMAPTYPE();
    
    if (eEnvMapType == TEXEFFECT_SPHERICAL || 
        eEnvMapType == TEXEFFECT_SPECULAR_CUBE ||
        (uiNumPerVertexLights != 0 && bSpecularOn))
    {
        bForceWorldView = true;
    }

    Fog eFogType = (Fog) pkVertexDesc->GetFOGTYPE();

    if (eFogType != FOG_NONE)
    {
        bForceViewPos = true;
    }

    if (!SetupTransformPipeline(kContext, pkVertOutProjPos, pkVertexDesc,
        bForceWorldView, bForceViewPos, pkWorldPos, pkViewPos, pkProjPos,
        pkWorldNormal, pkWorldView))
    {
        return false;
    }

    if (eEnvMapType == TEXEFFECT_SPHERICAL || 
        eEnvMapType == TEXEFFECT_SPECULAR_CUBE)
    {
        // Normal is not normal here; must normalize
        if (!HandleReflectionVectorFragment(kContext, pkWorldNormal, true, 
            pkWorldView, pkWorldReflect))
        {
            return false;
        }
    }

    if (!HandleVertexLightingAndMaterials(kContext, pkVertexDesc, pkWorldPos,
        pkWorldNormal, pkWorldView))
    {
        return false;
    }

#ifdef _XENON
    // If mesh instancing is used we need to manually perform a vfetch on all
    // vertex input data members and populate the input structure.
    if (pkVertexDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        kContext.m_spConfigurator->SetProgramDataInputType(
            NiMaterialConfigurator::INPUT_XENON_INDEX);
    }
#endif

    if (!HandleFinalVertexOutputs(kContext, pkVertexDesc, pkWorldPos, 
        pkWorldNormal, pkWorldReflect, pkViewPos, pkProjPos))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GenerateGeometryShadeTree(Context&, 
    NiGPUProgramDescriptor*)
{
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::GeneratePixelShadeTree(Context& kContext, 
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiStandardPixelProgramDescriptor");
    NiStandardPixelProgramDescriptor* pkPixelDesc = 
        (NiStandardPixelProgramDescriptor*)pkDesc;

    kContext.m_spConfigurator->SetDescription(pkPixelDesc->ToString());
    
    // Add pixel in, pixel out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_PIXEL))
    {
        return false;
    }

    // Add pixel input, projected position
    kContext.m_spInputs->AddOutputResource("float4", "Position",
        "World",  "PosProjected");

    NiMaterialResource* pkPixelWorldPos = NULL;
    NiMaterialResource* pkPixelWorldNorm = NULL;
    NiMaterialResource* pkPixelWorldBinormal = NULL;
    NiMaterialResource* pkPixelWorldTangent = NULL;
    NiMaterialResource* pkPixelWorldViewVector = NULL;
    NiMaterialResource* pkPixelTangentViewVector = NULL;
    
    if (!HandlePixelInputs(kContext, pkPixelDesc, pkPixelWorldPos,
        pkPixelWorldNorm, pkPixelWorldBinormal, pkPixelWorldTangent,
        pkPixelWorldViewVector, pkPixelTangentViewVector))
    {
        return false;
    }

    unsigned int uiPointLightCount = pkPixelDesc->GetPOINTLIGHTCOUNT();
    unsigned int uiDirLightCount = pkPixelDesc->GetDIRLIGHTCOUNT();
    unsigned int uiSpotLightCount = pkPixelDesc->GetSPOTLIGHTCOUNT();
    unsigned int uiPixelLightCount = 
        uiPointLightCount + uiDirLightCount + uiSpotLightCount;

    bool bSpecular = pkPixelDesc->GetSPECULAR() ? true : false;

    NiMaterialResource* pkMatDiffuse = NULL;
    NiMaterialResource* pkMatSpecular = NULL;
    NiMaterialResource* pkSpecularPower = NULL;
    NiMaterialResource* pkGlossiness = NULL;
    NiMaterialResource* pkMatAmbient = NULL;
    NiMaterialResource* pkMatEmissive = NULL;

    NiMaterialResource* pkTexDiffuseAccum = NULL;
    NiMaterialResource* pkTexSpecularAccum = NULL;  
    
    NiMaterialResource* pkDiffuseAccum = NULL;
    NiMaterialResource* pkSpecularAccum = NULL;
    NiMaterialResource* pkOpacityAccum = NULL;
    
    NiMaterialResource* pkLightDiffuseAccum = NULL;
    NiMaterialResource* pkLightSpecularAccum = NULL;
    NiMaterialResource* pkLightAmbientAccum = NULL;

    LightingModeEnum eLightingMode = LIGHTING_E_A_D;

    if (!HandlePixelMaterialInitialValues(kContext, pkPixelDesc, 
        uiPixelLightCount, bSpecular, eLightingMode,
        pkMatDiffuse, pkMatSpecular, pkSpecularPower,
        pkGlossiness, pkMatAmbient, pkMatEmissive, pkTexDiffuseAccum,
        pkTexSpecularAccum, pkDiffuseAccum, pkSpecularAccum, pkOpacityAccum,
        pkLightDiffuseAccum, pkLightSpecularAccum, pkLightAmbientAccum))
    {                       
        return false;       
    }


    NiMaterialResource* apkUVSets[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    unsigned int uiNumStandardUVs = 0;
    unsigned int uiDynamicEffectCount = 0;

    if (!HandlePixelUVSets(kContext, pkPixelDesc, apkUVSets, 
        STANDARD_PIPE_MAX_UVS_FOR_TEXTURES, uiNumStandardUVs,
        uiDynamicEffectCount))
    {
        return false;
    }

    unsigned int uiTexturesApplied = 0;

    if (!HandlePreLightTextureApplication(kContext, pkPixelDesc,
        pkPixelWorldPos, pkPixelWorldNorm, pkPixelWorldBinormal, 
        pkPixelWorldTangent, pkPixelWorldViewVector, pkPixelTangentViewVector,
        pkMatDiffuse, pkMatSpecular, pkSpecularPower,
        pkGlossiness, pkMatAmbient, pkMatEmissive, pkOpacityAccum,
        pkLightAmbientAccum, pkLightDiffuseAccum, pkLightSpecularAccum,
        pkTexDiffuseAccum, pkTexSpecularAccum, uiTexturesApplied, apkUVSets,
        uiNumStandardUVs, uiDynamicEffectCount))
    {
        return false;
    }

    unsigned int uiShadowMapForLight = pkPixelDesc->GetSHADOWMAPFORLIGHT(); 
    unsigned int uiShadowTechnqiue = pkPixelDesc->GetSHADOWTECHNIQUE();

    if (!HandleLighting(kContext, bSpecular, uiPointLightCount, 
        uiDirLightCount, uiSpotLightCount, uiShadowMapForLight, 
        uiShadowTechnqiue, pkPixelWorldPos, pkPixelWorldNorm, 
        pkPixelWorldViewVector, pkSpecularPower, pkLightAmbientAccum, 
        pkLightDiffuseAccum, pkLightSpecularAccum))
    {
        return false;
    }

    if (!HandleColorAccumulation(kContext, pkPixelDesc, 
        pkMatEmissive, pkMatDiffuse, pkMatAmbient, pkMatSpecular, 
        pkLightSpecularAccum, pkLightDiffuseAccum, pkLightAmbientAccum,
        pkGlossiness, pkTexDiffuseAccum, pkTexSpecularAccum, eLightingMode,
        pkSpecularAccum, pkDiffuseAccum))
    {
        return false;
    }

    if (!HandlePostLightTextureApplication(kContext, pkPixelDesc,
        pkPixelWorldNorm, pkPixelWorldViewVector, pkOpacityAccum, 
        pkDiffuseAccum, pkSpecularAccum, pkGlossiness, 
        uiTexturesApplied, apkUVSets, uiNumStandardUVs, uiDynamicEffectCount))
    {
        return false;
    }

    if (!HandleFinalPixelOutputs(kContext, pkPixelDesc, pkDiffuseAccum, 
        pkSpecularAccum, pkOpacityAccum))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePixelInputs(Context& kContext, 
    NiStandardPixelProgramDescriptor* pkPixelDesc,
    NiMaterialResource*& pkPixelWorldPos,
    NiMaterialResource*& pkPixelWorldNorm,
    NiMaterialResource*& pkPixelWorldBinormal,
    NiMaterialResource*& pkPixelWorldTangent,
    NiMaterialResource*& pkPixelWorldViewVector,
    NiMaterialResource*& pkPixelTangentViewVector)
{
    if (pkPixelDesc->GetWORLDPOSITION())
    {
        pkPixelWorldPos = kContext.m_spInputs->AddOutputResource("float4", 
            "TexCoord", "", "WorldPos");
    }

    if (pkPixelDesc->GetWORLDNORMAL() ||  pkPixelDesc->GetWORLDNBT())
    {
        pkPixelWorldNorm = kContext.m_spInputs->AddOutputResource("float3", 
            "TexCoord", "", "WorldNormal");

        if (!NormalizeVector(kContext, pkPixelWorldNorm))
        {
            return false;
        }
    }
    
    if (pkPixelDesc->GetWORLDNBT())
    {
        pkPixelWorldBinormal = kContext.m_spInputs->AddOutputResource(
            "float3", "TexCoord", "", "WorldBinormal");
        pkPixelWorldTangent = kContext.m_spInputs->AddOutputResource(
            "float3", "TexCoord", "", "WorldTangent");

        if (!NormalizeVector(kContext, pkPixelWorldBinormal))
        {
            return false;
        }
        if (!NormalizeVector(kContext, pkPixelWorldTangent))
        {
            return false;
        }
    }

    if (pkPixelDesc->GetWORLDVIEW() != 0)
    {
        pkPixelWorldViewVector = kContext.m_spInputs->AddOutputResource(
            "float3", "TexCoord", "", "WorldView");

        if (!NormalizeVector(kContext, pkPixelWorldViewVector))
        {
            return false;
        }
    }

    if (pkPixelDesc->GetPARALLAXMAPCOUNT() != 0)
    {
        pkPixelTangentViewVector = kContext.m_spInputs->AddOutputResource(
            "float3", "TexCoord", "", "TangentSpaceView");
        
        if (!NormalizeVector(kContext, pkPixelTangentViewVector))
        {
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePixelMaterialInitialValues(Context& kContext,
    NiStandardPixelProgramDescriptor* pkPixelDesc,
    unsigned int uiPixelLightCount, 
    bool bSpecular,
    LightingModeEnum& eLightingMode,
    NiMaterialResource*& pkMatDiffuse,
    NiMaterialResource*& pkMatSpecular,
    NiMaterialResource*& pkSpecularPower,
    NiMaterialResource*&,
    NiMaterialResource*& pkMatAmbient,
    NiMaterialResource*& pkMatEmissive,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*& pkOpacityAccum,
    NiMaterialResource*& pkLightDiffuseAccum,
    NiMaterialResource*& pkLightSpecularAccum,
    NiMaterialResource*& pkLightAmbientAccum)
{
    AmbDiffEmissiveEnum eAmbDiffEmissive = (AmbDiffEmissiveEnum)
        pkPixelDesc->GetAMBDIFFEMISSIVE();

    eLightingMode = (LightingModeEnum) pkPixelDesc->GetLIGHTINGMODE();

    bool bPerVertexLighting = pkPixelDesc->GetPERVERTEXLIGHTING() == 1;

    if (pkPixelDesc->GetAPPLYMODE() != APPLY_REPLACE)
    {
        if (bPerVertexLighting)
        {
            // Vertex lights only or mix of pixel and vertex lights
            pkLightDiffuseAccum = kContext.m_spInputs->AddOutputResource(
                "float4", "TexCoord", "", "DiffuseAccum");

            if (!SplitColorAndOpacity(kContext, pkLightDiffuseAccum, 
                pkLightDiffuseAccum, pkOpacityAccum))
            {
                return false;
            }

            if (uiPixelLightCount != 0)

            {
                pkLightAmbientAccum = kContext.m_spInputs->AddOutputResource(
                    "float4", "TexCoord", "", "AmbientAccum");
            }

            if (bSpecular)
            {
                pkLightSpecularAccum = kContext.m_spInputs->AddOutputResource(
                    "float3", "TexCoord", "", "SpecularAccum");
            }
        }
        else
        {
            bool bApplyAmbient = pkPixelDesc->GetAPPLYAMBIENT() != 0;

            bool bApplyEmissive = pkPixelDesc->GetAPPLYEMISSIVE() != 0;

            // Pixel lights only
            if (bApplyAmbient)
            {
                pkLightAmbientAccum = AddOutputPredefined(
                    kContext.m_spUniforms, 
                    NiShaderConstantMap::SCM_DEF_AMBIENTLIGHT);
            }

            if (!HandleInitialSpecAmbDiffEmissiveColor(kContext, bSpecular, 
                eAmbDiffEmissive, eLightingMode, pkMatDiffuse, pkMatSpecular, 
                pkSpecularPower, pkMatAmbient, pkMatEmissive, pkOpacityAccum))
            {
                return false;
            }

            if (!bApplyEmissive)
            {
                pkMatEmissive = NULL;
            }
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePixelUVSets(Context& kContext, 
    NiStandardPixelProgramDescriptor* pkPixelDesc, 
    NiMaterialResource** ppkUVSets, unsigned int,
    unsigned int& uiNumStandardUVs, unsigned int& uiDynamicEffectCount)
{
    unsigned int uiTexIndex = 0;
    for (unsigned int ui = 0; ui < pkPixelDesc->GetInputUVCount(); 
        ui++)
    {
        ppkUVSets[uiTexIndex] = kContext.m_spInputs->AddOutputResource(
            "float2", "TexCoord", "", GenerateUVSetName(uiTexIndex));
        uiTexIndex++;
    }

    uiNumStandardUVs = uiTexIndex;
    uiDynamicEffectCount = pkPixelDesc->GetPROJLIGHTMAPCOUNT() + 
        pkPixelDesc->GetPROJSHADOWMAPCOUNT();

    if (pkPixelDesc->GetENVMAPTYPE() != TEXEFFECT_NONE && 
        pkPixelDesc->GetNORMALMAPCOUNT() == 0)
    {
        uiDynamicEffectCount++;
    }

    for (unsigned int ui = 0; ui < uiDynamicEffectCount; ui++)
    {
        ppkUVSets[uiTexIndex] = kContext.m_spInputs->AddOutputResource(
            "float4", "TexCoord", "", GenerateUVSetName(uiTexIndex));
        uiTexIndex++;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleVertexLightingAndMaterials(Context& kContext,
    NiStandardVertexProgramDescriptor* pkVertexDesc, 
    NiMaterialResource* pkWorldPos,
    NiMaterialResource* pkWorldNormal,
    NiMaterialResource* pkWorldView)
{    
    unsigned int uiPointLights = pkVertexDesc->GetPOINTLIGHTCOUNT();
    unsigned int uiDirLights = pkVertexDesc->GetDIRLIGHTCOUNT();
    unsigned int uiSpotLights = pkVertexDesc->GetSPOTLIGHTCOUNT();
    bool bSpecularOn = pkVertexDesc->GetSPECULAR() == 1;

    unsigned int uiNumPerVertexLights = uiPointLights + uiDirLights + 
        uiSpotLights; 

    bool bVertexOnlyLights = pkVertexDesc->GetVERTEXLIGHTSONLY() == 1;

    AmbDiffEmissiveEnum eAmbDiffEmissive = (AmbDiffEmissiveEnum)
        pkVertexDesc->GetAMBDIFFEMISSIVE();

    LightingModeEnum eLightingMode = (LightingModeEnum)
        pkVertexDesc->GetLIGHTINGMODE();

    ApplyMode eApplyMode = (ApplyMode) pkVertexDesc->GetAPPLYMODE();

    if (eApplyMode != APPLY_REPLACE)
    {
        if (uiNumPerVertexLights != 0 || bVertexOnlyLights)
        {
            NiMaterialResource* pkMatDiffuse = NULL;
            NiMaterialResource* pkMatSpecular = NULL;
            NiMaterialResource* pkSpecularPower = NULL;
            NiMaterialResource* pkMatAmbient = NULL;
            NiMaterialResource* pkMatEmissive = NULL;
            NiMaterialResource* pkOpacityAccum = NULL;

            if (bVertexOnlyLights)
            {
                if (!HandleInitialSpecAmbDiffEmissiveColor(kContext, 
                    bSpecularOn, eAmbDiffEmissive, eLightingMode,
                    pkMatDiffuse, pkMatSpecular, pkSpecularPower,
                    pkMatAmbient, pkMatEmissive, pkOpacityAccum))
                {
                    return false;
                }
            }
            
            NiMaterialResource* pkDiffuseAccum = NULL;
            NiMaterialResource* pkSpecularAccum = NULL;
            NiMaterialResource* pkAmbientAccum = NULL;

            if (eLightingMode == LIGHTING_E_A_D)
            {
                pkAmbientAccum = AddOutputPredefined(kContext.m_spUniforms, 
                    NiShaderConstantMap::SCM_DEF_AMBIENTLIGHT);

                if (pkWorldNormal && 
                    !NormalizeVector(kContext, pkWorldNormal))
                {
                    return false;
                }

                if (pkWorldView && 
                    !NormalizeVector(kContext, pkWorldView))
                {
                    return false;
                }

                if (!HandleLighting(kContext, bSpecularOn, 
                    uiPointLights, uiDirLights, uiSpotLights, 0, 0,
                    pkWorldPos, pkWorldNormal, pkWorldView, pkSpecularPower,
                    pkAmbientAccum, pkDiffuseAccum, pkSpecularAccum))
                {
                    return false;
                }
            }

            if (bVertexOnlyLights)
            {
                // Only vertex lights are used
                NiMaterialResource* pkDiffuseCoeff = NULL;
                NiMaterialResource* pkSpecularCoeff = NULL;

                if (!HandleShadingCoefficients(kContext, pkMatEmissive, 
                    pkMatDiffuse, pkMatAmbient, pkMatSpecular,
                    pkSpecularAccum, pkDiffuseAccum, pkAmbientAccum,
                    pkDiffuseCoeff, pkSpecularCoeff))
                {
                    return false;
                }

                NIASSERT(pkDiffuseCoeff && pkSpecularCoeff);

                NiMaterialNode* pkDiffuseCombineNode = 
                    GetAttachableNodeFromLibrary("CompositeFinalRGBAColor");
                kContext.m_spConfigurator->AddNode(pkDiffuseCombineNode);

                kContext.m_spConfigurator->AddBinding(pkDiffuseCoeff, 
                    "FinalColor", pkDiffuseCombineNode);
                if (pkOpacityAccum)
                {
                    kContext.m_spConfigurator->AddBinding(pkOpacityAccum, 
                        "FinalOpacity", pkDiffuseCombineNode);
                }

                NiMaterialResource* pkDiffuseColorAlpha = 
                    pkDiffuseCombineNode->GetOutputResourceByVariableName(
                    "OutputColor");

                NiMaterialResource* pkVertOut = 
                    kContext.m_spOutputs->AddInputResource(
                    pkDiffuseColorAlpha->GetType(), "TexCoord",
                    pkDiffuseColorAlpha->GetLabel(), "DiffuseAccum");

                kContext.m_spConfigurator->AddBinding(pkDiffuseColorAlpha,
                    pkVertOut);

                if (bSpecularOn)
                {
                    NiMaterialResource* pkVertOutRes = 
                        kContext.m_spOutputs->AddInputResource(
                        pkSpecularCoeff->GetType(), "TexCoord",
                        pkSpecularCoeff->GetLabel(), "SpecularAccum");

                    kContext.m_spConfigurator->AddBinding(pkSpecularCoeff,
                        pkVertOutRes);
                }
            }
            else 
            {
                // Both pixel and vertex lights are used
                // Gouraud calculations to be done in PS
                NIASSERT(pkDiffuseAccum && pkAmbientAccum);
                NIASSERT(!bSpecularOn || pkSpecularAccum);
                
                if (pkDiffuseAccum)
                {
                    NiMaterialResource* pkVertOut = 
                        kContext.m_spOutputs->AddInputResource(
                        pkDiffuseAccum->GetType(), "TexCoord",
                        pkDiffuseAccum->GetLabel(), "DiffuseAccum");

                    kContext.m_spConfigurator->AddBinding(pkDiffuseAccum,
                        pkVertOut);
                }

                if (pkAmbientAccum)
                {
                    NiMaterialResource* pkVertOut = 
                        kContext.m_spOutputs->AddInputResource(
                        pkDiffuseAccum->GetType(), "TexCoord",
                        pkDiffuseAccum->GetLabel(), "AmbientAccum");

                    kContext.m_spConfigurator->AddBinding(pkAmbientAccum,
                        pkVertOut);
                }

                if (pkSpecularAccum)
                {
                    NiMaterialResource* pkVertOut = 
                        kContext.m_spOutputs->AddInputResource(
                        pkSpecularAccum->GetType(), "TexCoord",
                        pkSpecularAccum->GetLabel(), "SpecularAccum");

                    kContext.m_spConfigurator->AddBinding(pkSpecularAccum,
                        pkVertOut);
                }
            }
        }
        else
        {
            // Only pixel lights are used
            bool bVertexColors = pkVertexDesc->GetVERTEXCOLORS() == VC_EXISTS;

            if (bVertexColors && (eAmbDiffEmissive == ADE_EMISSIVE ||
                (eAmbDiffEmissive == ADE_AMB_DIFF && 
                eLightingMode == LIGHTING_E_A_D)))
            {
                NiMaterialResource* pkVertIn = 
                    kContext.m_spInputs->AddOutputResource(
                    "float4", "Color", "", "VertexColors");

                NiMaterialResource* pkVertOut = 
                    kContext.m_spOutputs->AddInputResource(
                    pkVertIn->GetType(), pkVertIn->GetSemantic(),
                    pkVertIn->GetLabel(), "VertexColors");

                kContext.m_spConfigurator->AddBinding(pkVertIn, pkVertOut);
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleVertexUVSets(Context& kContext,
    NiStandardVertexProgramDescriptor* pkVertexDesc,
    NiMaterialResource* pkWorldPos,
    NiMaterialResource* pkWorldNormal,
    NiMaterialResource* pkWorldReflect)
{
    unsigned int uiNextUVSet = 0;
    NiMaterialResource* apkOutputUVs[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    memset(apkOutputUVs, 0, STANDARD_PIPE_MAX_UVS_FOR_TEXTURES * 
        sizeof(NiMaterialResource*));

    if (!HandleTextureUVSets(kContext, pkVertexDesc, apkOutputUVs,
        STANDARD_PIPE_MAX_UVS_FOR_TEXTURES, uiNextUVSet))
    {
         return false;
    }

    unsigned int uiProjLightMapCount = pkVertexDesc->GetPROJLIGHTMAPCOUNT();
    unsigned int uiProjLightMapTypes = pkVertexDesc->GetPROJLIGHTMAPTYPES();
    unsigned int uiProjShadowMapCount = pkVertexDesc->GetPROJSHADOWMAPCOUNT();
    unsigned int uiProjShadowMapTypes = pkVertexDesc->GetPROJSHADOWMAPTYPES();

    TexEffectType eEnvMapType = (TexEffectType) pkVertexDesc->GetENVMAPTYPE();
    
    if (!HandleTextureEffectUVSets(kContext, apkOutputUVs, 
        STANDARD_PIPE_MAX_UVS_FOR_TEXTURES, pkWorldPos, 
        pkWorldNormal, pkWorldReflect, eEnvMapType,
        uiProjLightMapCount, uiProjLightMapTypes, uiProjShadowMapCount,
        uiProjShadowMapTypes, uiNextUVSet))
    {
         return false;
    }

    // Bind the output texture array to output resources.
    for (unsigned int ui = 0; ui < uiNextUVSet; ui++)
    {
        NIASSERT(apkOutputUVs[ui] != NULL);
        NiMaterialResource* pkVertOutTexCoord = 
            kContext.m_spOutputs->AddInputResource(
            apkOutputUVs[ui]->GetType(), "TexCoord",
            "", GenerateUVSetName(ui));

        kContext.m_spConfigurator->AddBinding(apkOutputUVs[ui],
            pkVertOutTexCoord);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleFinalVertexOutputs(Context& kContext,
    NiStandardVertexProgramDescriptor* pkVertexDesc, 
    NiMaterialResource* pkWorldPos,
    NiMaterialResource* pkWorldNormal,
    NiMaterialResource* pkWorldReflect,
    NiMaterialResource* pkViewPos,
    NiMaterialResource*)
{    
    // Output vertex UV sets
    if (!HandleVertexUVSets(kContext, pkVertexDesc, pkWorldPos, pkWorldNormal,
        pkWorldReflect))
    {                                               
        return false;                               
    }
    

    Fog eFogType = (Fog) pkVertexDesc->GetFOGTYPE();

    // Handle Fogging
    if (!HandleCalculateFog(kContext, pkViewPos, eFogType))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleColorAccumulation(Context& kContext,
    NiStandardPixelProgramDescriptor* pkPixelDesc,
    NiMaterialResource* pkMatEmissive, 
    NiMaterialResource* pkMatDiffuse, 
    NiMaterialResource* pkMatAmbient, 
    NiMaterialResource* pkMatSpecular, 
    NiMaterialResource* pkLightSpecularAccum, 
    NiMaterialResource* pkLightDiffuseAccum, 
    NiMaterialResource* pkLightAmbientAccum,
    NiMaterialResource* pkGlossiness, 
    NiMaterialResource* pkTexDiffuseAccum, 
    NiMaterialResource*, 
    LightingModeEnum eLightingMode,
    NiMaterialResource*& pkSpecularAccum,
    NiMaterialResource*& pkDiffuseAccum)
{
    
    NiMaterialResource* pkDiffuseCoeff = NULL;
    NiMaterialResource* pkSpecularCoeff = NULL;
    
    // APPLY_REPLACE only uses the texture's color, all lighting, vertex,
    // and material colors are ignored.
    if (pkPixelDesc->GetAPPLYMODE() == APPLY_REPLACE)
    {
        pkDiffuseCoeff = NULL;
    }
    else 
    {
        // If vertex lighting was done and there are no pixel lights, then 
        // the Gouraud calculations have already been done
        NiUInt32 uiPerPixelLights = pkPixelDesc->GetPOINTLIGHTCOUNT() +
            pkPixelDesc->GetDIRLIGHTCOUNT() + 
            pkPixelDesc->GetSPOTLIGHTCOUNT();

        if (pkPixelDesc->GetPERVERTEXLIGHTING() == 1 && 
            uiPerPixelLights == 0)
        {
            pkDiffuseCoeff = pkLightDiffuseAccum;
            pkSpecularCoeff = pkLightSpecularAccum;
        }
        else 
        {
            if (eLightingMode == LIGHTING_E)
            {
                pkLightDiffuseAccum = NULL;
                pkLightAmbientAccum = NULL;
            }

            if (!HandleShadingCoefficients(kContext, pkMatEmissive, 
                pkMatDiffuse, pkMatAmbient, pkMatSpecular,
                pkLightSpecularAccum, pkLightDiffuseAccum,
                pkLightAmbientAccum, pkDiffuseCoeff, pkSpecularCoeff))
            {
                return false;
            }
        }
    }

    if (pkDiffuseCoeff && pkTexDiffuseAccum)
    {
        if (!MultiplyVector(kContext, pkDiffuseCoeff, pkTexDiffuseAccum, 
            pkDiffuseAccum))
        {
            return false;
        }

        if (m_bSaturateTextures && !SaturateVector(kContext, pkDiffuseAccum, 
                pkDiffuseAccum))
        {
            return false;
        }
    }
    else if (pkTexDiffuseAccum)
    {
        pkDiffuseAccum = pkTexDiffuseAccum;
    }
    else if (pkDiffuseCoeff)
    {
        pkDiffuseAccum = pkDiffuseCoeff;
    }
    else
    {
        return false;
    }

    pkSpecularAccum = pkSpecularCoeff;

    if (pkGlossiness && pkSpecularAccum)
    {
        if (!MultiplyVector(kContext, pkSpecularAccum, pkGlossiness, 
            pkSpecularAccum))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleFinalPixelOutputs(Context& kContext,
    NiStandardPixelProgramDescriptor* pkPixelDesc,
    NiMaterialResource* pkDiffuseAccum,
    NiMaterialResource* pkSpecularAccum,
    NiMaterialResource* pkOpacityAccum)
{
    // Add output color resource
    NiMaterialResource* pkPixelOutColor = 
        kContext.m_spOutputs->AddInputResource("float4", "Color", "", 
        "Color0");

    NiMaterialResource* pkFinalColor = NULL;
    NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
        "CompositeFinalRGBColor");
    kContext.m_spConfigurator->AddNode(pkNode);

    if (pkDiffuseAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkDiffuseAccum, 
            "DiffuseColor", pkNode);
    }

    if (pkSpecularAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkSpecularAccum, 
            "SpecularColor", pkNode);
    }

    pkFinalColor = pkNode->GetOutputResourceByVariableName("OutputColor");

    if (!HandleApplyFog(kContext, pkPixelDesc, pkFinalColor, pkFinalColor))
        return false;

    pkNode = GetAttachableNodeFromLibrary(
        "CompositeFinalRGBAColor");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkFinalColor, "FinalColor", pkNode);
    if (pkOpacityAccum)
    {
        HandleAlphaTest(kContext, pkPixelDesc, pkOpacityAccum);

        kContext.m_spConfigurator->AddBinding(pkOpacityAccum, 
            "FinalOpacity", pkNode);
    }
    
    kContext.m_spConfigurator->AddBinding("OutputColor", pkNode, 
        pkPixelOutColor);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiStandardMaterial::VerifyShaderPrograms(
    NiGPUProgram* pkVertexShader, NiGPUProgram*, 
    NiGPUProgram* pkPixelShader)
{
    unsigned int uiReturnCode = RC_SUCCESS;
    if (pkVertexShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_VERTEX;
    if (pkPixelShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_PIXEL;
    // No need to check geometry shader

    return uiReturnCode;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SetupTransformPipeline(Context& kContext, 
    NiMaterialResource* pkOutProjPos,
    NiStandardVertexProgramDescriptor* pkVertexDesc, bool bForceWorldView,
    bool bForceViewPos, NiMaterialResource*& pkWorldPos, 
    NiMaterialResource*& pkViewPos, NiMaterialResource*& pkProjectedPos,
    NiMaterialResource*& pkWorldNormal, NiMaterialResource*& pkWorldView)
{
    pkWorldPos = NULL;
    pkWorldNormal = NULL;
    pkWorldView = NULL;

    NiMaterialResource* pkWorldMatrix = NULL;

    if (!HandlePositionFragment(kContext, 
        (TransformType)pkVertexDesc->GetTRANSFORM(),
        pkWorldPos, pkWorldMatrix))
    {
        return false;
    }

    NiMaterialResource* pkWorldBiNormal = NULL;
    NiMaterialResource* pkWorldTangent = NULL;

    
    NormalType eNormalType =(NormalType)pkVertexDesc->GetNORMAL();

    if (!HandleNormalFragment(kContext, eNormalType, pkWorldMatrix, 
        pkWorldNormal, pkWorldBiNormal, pkWorldTangent))
    {
        return false;
    }

    if (!HandlePostWorldTransform(kContext, pkVertexDesc, 
        pkWorldPos, pkWorldNormal))
    {
        return false;
    }

    if (!HandleViewProjectionFragment(kContext, bForceViewPos, 
        pkWorldPos, pkProjectedPos, pkViewPos))
    {
        return false;
    }

    if (!pkProjectedPos || !pkOutProjPos)
        return false;

    kContext.m_spConfigurator->AddBinding(pkProjectedPos, 
        pkOutProjPos); 

    if (pkVertexDesc->GetOUTPUTWORLDPOS() == 1)
    {
        // Insert world position
        NiMaterialResource* pkWorldTexCoord = 
            kContext.m_spOutputs->AddInputResource("float4",
            "TexCoord", "", "WorldPos");
        kContext.m_spOutputs->AddOutputResource("float4",
            "TexCoord", "", "WorldPos");
        kContext.m_spConfigurator->AddBinding(pkWorldPos, 
            pkWorldTexCoord);
    }

    if (pkVertexDesc->GetOUTPUTWORLDNBT() == 1)
    {
        // Add normal
        if (eNormalType !=  NORMAL_NONE)
        {
            NiMaterialResource* pkWorldTexCoord = 
                kContext.m_spOutputs->AddInputResource("float3",
                "TexCoord", "", "WorldNormal");
            kContext.m_spOutputs->AddOutputResource("float3",
                "TexCoord", "", "WorldNormal");
            kContext.m_spConfigurator->AddBinding(pkWorldNormal,
                pkWorldTexCoord);
        }
        
        // Add binormal & tangent (normal added above)
        if (eNormalType ==  NORMAL_NBT)
        {
            NiMaterialResource* pkWorldTexCoord = 
                kContext.m_spOutputs->AddInputResource("float3",
                "TexCoord", "", "WorldBinormal");
            kContext.m_spOutputs->AddOutputResource("float3",
                "TexCoord", "", "WorldBinormal");
            kContext.m_spConfigurator->AddBinding(pkWorldBiNormal, 
                pkWorldTexCoord);

            pkWorldTexCoord = 
                kContext.m_spOutputs->AddInputResource("float3",
                "TexCoord", "", "WorldTangent");
            kContext.m_spOutputs->AddOutputResource("float3",
                "TexCoord", "", "WorldTangent");
            kContext.m_spConfigurator->AddBinding(pkWorldTangent, 
                pkWorldTexCoord);
        }
    }

    
    bool bOutputWorldView = pkVertexDesc->GetOUTPUTWORLDVIEW() == 1;
    bool bOutputTangentView = pkVertexDesc->GetOUTPUTTANGENTVIEW() == 1;

    if (bOutputWorldView || bOutputTangentView || bForceWorldView)
    {
        NiMaterialResource* pkTangentView = NULL;
       
        if (!HandleViewVectorFragment(kContext,
            pkWorldPos, pkWorldNormal, pkWorldBiNormal, pkWorldTangent,
            bOutputTangentView, pkWorldView, pkTangentView))
        {
            return false;
        }

        if (bOutputWorldView)
        {
            NIASSERT(pkWorldView);
            NiMaterialResource* pkOutputWorldView  = 
                kContext.m_spOutputs->AddInputResource("float3",
                "TexCoord", "WorldView", "WorldView");

            kContext.m_spConfigurator->AddBinding(pkWorldView, 
                pkOutputWorldView); 
        }

        if (bOutputTangentView)
        {
            NIASSERT(pkTangentView);
            NiMaterialResource* pkOutputTangentView  = 
                kContext.m_spOutputs->AddInputResource("float3",
                "TexCoord", "TangentSpaceView", "TangentSpaceView");

            kContext.m_spConfigurator->AddBinding(pkTangentView, 
                pkOutputTangentView); 
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleNormalFragment(Context& kContext, 
    NormalType eNormType,
    NiMaterialResource* pkWorldMatrix,
    NiMaterialResource*& pkVertWorldNorm,
    NiMaterialResource*& pkVertWorldBiNorm,
    NiMaterialResource*& pkVertWorldTangent)
{
    pkVertWorldNorm = NULL;
    pkVertWorldBiNorm = NULL;
    pkVertWorldTangent = NULL;

    // Handle the normals
    switch (eNormType)
    {
    case NORMAL_NONE:
        return true;
        break;
    case NORMAL_ONLY:
        {
            NiMaterialResource* pkLocalNorm = 
                kContext.m_spInputs->AddOutputResource("float3", "Normal",
                "Local", "Normal");
            NiMaterialNode* pkNormFrag = 
                GetAttachableNodeFromLibrary("TransformNormal");
            kContext.m_spConfigurator->AddNode(pkNormFrag);
            kContext.m_spConfigurator->AddBinding(pkLocalNorm, 
                pkNormFrag->GetInputResourceByVariableName("Normal"));
            kContext.m_spConfigurator->AddBinding(pkWorldMatrix, 
                pkNormFrag->GetInputResourceByVariableName("World"));
            pkVertWorldNorm = pkNormFrag->GetOutputResourceByVariableName(
                "WorldNrm");
        }
        return true;
        break;
    case NORMAL_NBT:
        {
            NiMaterialResource* pkNorm = 
                kContext.m_spInputs->AddOutputResource("float3", "Normal",
                "Local", "Normal");
            NiMaterialResource* pkBiNorm = 
                kContext.m_spInputs->AddOutputResource("float3", "Binormal",
                "Local", 
                "Binormal");
            NiMaterialResource* pkTangent = 
                kContext.m_spInputs->AddOutputResource("float3", "Tangent",
                "Local", "Tangent");
            NiMaterialNode* pkNormFrag = 
                GetAttachableNodeFromLibrary("TransformNBT");
            kContext.m_spConfigurator->AddNode(pkNormFrag);
            kContext.m_spConfigurator->AddBinding(pkNorm, 
                pkNormFrag->GetInputResourceByVariableName("Normal"));
            kContext.m_spConfigurator->AddBinding(pkBiNorm, 
                pkNormFrag->GetInputResourceByVariableName("Binormal"));
            kContext.m_spConfigurator->AddBinding(pkTangent, 
                pkNormFrag->GetInputResourceByVariableName("Tangent"));
            kContext.m_spConfigurator->AddBinding(pkWorldMatrix, 
                pkNormFrag->GetInputResourceByVariableName("World"));
            pkVertWorldNorm = pkNormFrag->GetOutputResourceByVariableName(
                "WorldNrm");
            pkVertWorldBiNorm = pkNormFrag->GetOutputResourceByVariableName(
                "WorldBinormal");
            pkVertWorldTangent = pkNormFrag->GetOutputResourceByVariableName(
                "WorldTangent");
        }
        return true;
        break;
    default:
        break;
    }

    NIASSERT(!"Error in fragment");
    return false;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePositionFragment(Context& kContext, 
    TransformType eTransType, 
    NiMaterialResource*& pkVertWorldPos,
    NiMaterialResource*& pkWorldMatrix)
{
    pkVertWorldPos = NULL;
    pkWorldMatrix = NULL;

    if (eTransType == TRANSFORM_DEFAULT)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", "Local", 
            "Position", 1);

        // Add constant map elements
        pkWorldMatrix = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_WORLD, 4);
        
        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs,
            "Position", pkProjFrag);

        kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
            "World", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");
    }
    else if (eTransType == TRANSFORM_SKINNED)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformSkinnedPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", 
            "Local", "Position");
        kContext.m_spInputs->AddOutputResource("float3", "BlendWeight", 
            "Local", "BlendWeights");
        kContext.m_spInputs->AddOutputResource("int4", "BlendIndices", 
            "Local", "BlendIndices");

        // Add constant map elements
        NiMaterialResource* pkBoneMatrix = AddOutputPredefined(
            kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3,
            3, 30);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs,
            "Position", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendWeights",
            kContext.m_spInputs, "BlendWeights", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendIndices", 
            kContext.m_spInputs, "BlendIndices", pkProjFrag);       

        kContext.m_spConfigurator->AddBinding(pkBoneMatrix,
            "Bones", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");

        pkWorldMatrix = 
            pkProjFrag->GetOutputResourceByVariableName("SkinBoneTransform");
    }
    else if (eTransType == TRANSFORM_INSTANCED)
    {
        NiMaterialNode* pkInstTransFrag = GetAttachableNodeFromLibrary(
            "ConstructInstanceTransform");
        kContext.m_spConfigurator->AddNode(pkInstTransFrag);
                
#ifdef _XENON
        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", "Local", 
            "Position", 1);
        kContext.m_spInputs->AddOutputResource("float4", "Position", "World", 
            "Position1", 1);
        kContext.m_spInputs->AddOutputResource("float4", "Position", "World", 
            "Position2", 1);
        kContext.m_spInputs->AddOutputResource("float4", "Position", "World", 
            "Position3", 1);
#else

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", "Local", 
            "Position", 1);
        kContext.m_spInputs->AddOutputResource("float4", "TexCoord", "World", 
            "Position1", 1);
        kContext.m_spInputs->AddOutputResource("float4", "TexCoord", "World", 
            "Position2", 1);
        kContext.m_spInputs->AddOutputResource("float4", "TexCoord", "World", 
            "Position3", 1);
#endif

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position1",
            kContext.m_spInputs, "Position1", pkInstTransFrag);
        kContext.m_spConfigurator->AddBinding("Position2",
            kContext.m_spInputs, "Position2", pkInstTransFrag);
        kContext.m_spConfigurator->AddBinding("Position3",
            kContext.m_spInputs, "Position3", pkInstTransFrag);

        pkWorldMatrix = pkInstTransFrag->GetOutputResourceByVariableName(
            "InstanceWorld");

        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs,
            "Position", pkProjFrag);

        kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
            "World", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");

    }
    else
    {
        NIASSERT(false && "Unhandled transform type!");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePostWorldTransform(Context&, 
    NiGPUProgramDescriptor*,
    NiMaterialResource*&,
    NiMaterialResource*&)
{
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleViewProjectionFragment(Context& kContext, 
    bool bForceViewPos, NiMaterialResource* pkVertWorldPos, 
    NiMaterialResource*& pkVertOutProjectedPos,
    NiMaterialResource*& pkVertOutViewPos)
{
    if (bForceViewPos)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "ProjectPositionWorldToViewToProj");
        if (!pkProjFrag)
        {
            NIASSERT(!"Error in fragment");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkProjFrag);

        NiMaterialResource* pkViewMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEW, 4);
        kContext.m_spConfigurator->AddBinding(pkViewMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ViewTransform"));

        NiMaterialResource* pkProjMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_PROJ, 4);
        kContext.m_spConfigurator->AddBinding(pkProjMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ProjTransform"));

        kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
            pkProjFrag->GetInputResourceByVariableName("WorldPosition"));

        pkVertOutViewPos = pkProjFrag->GetOutputResourceByVariableName(
            "ViewPos");

        pkVertOutProjectedPos = pkProjFrag->GetOutputResourceByVariableName(
            "ProjPos");
    }
    else
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "ProjectPositionWorldToProj");
        if (!pkProjFrag)
        {
            NIASSERT(!"Error in fragment");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkProjFrag);

        NiMaterialResource* pkViewProjMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEWPROJ, 4);

        kContext.m_spConfigurator->AddBinding(pkViewProjMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ViewProjection"));
        kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
            pkProjFrag->GetInputResourceByVariableName("WorldPosition"));
        
        pkVertOutProjectedPos = pkProjFrag->GetOutputResourceByVariableName(
            "ProjPos");

    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleViewVectorFragment(Context& kContext,
    NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
    NiMaterialResource* pkWorldBinorm, NiMaterialResource* pkWorldTangent,
    bool bComputeTangent, NiMaterialResource*& pkWorldViewVector, 
    NiMaterialResource*& pkTangentViewVector)
{
    pkWorldViewVector = NULL;
    pkTangentViewVector = NULL;
    
    NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
        "CalculateViewVector");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkWorldPos, 
        pkNode->GetInputResourceByVariableName("WorldPos"));

    NiMaterialResource* pkCameraPosition = AddOutputPredefined(
        kContext.m_spUniforms,
        NiShaderConstantMap::SCM_DEF_EYE_POS);

    kContext.m_spConfigurator->AddBinding(pkCameraPosition, 
        pkNode->GetInputResourceByVariableName("CameraPos")); 

    // This vector will not be normalized!
    pkWorldViewVector = 
        pkNode->GetOutputResourceByVariableName("WorldViewVector");
    
    if (bComputeTangent)
    {
        NIASSERT(pkWorldViewVector);
        NiMaterialNode* pkAttachableNode = GetAttachableNodeFromLibrary(
            "WorldToTangent");
        kContext.m_spConfigurator->AddNode(pkAttachableNode);

        kContext.m_spConfigurator->AddBinding(pkWorldViewVector, 
            pkAttachableNode->GetInputResourceByVariableName("VectorIn")); 

        kContext.m_spConfigurator->AddBinding(pkWorldNorm,
            pkAttachableNode->GetInputResourceByVariableName(
            "WorldNormalIn")); 

        kContext.m_spConfigurator->AddBinding(pkWorldBinorm, 
            pkAttachableNode->GetInputResourceByVariableName(
            "WorldBinormalIn")); 

        kContext.m_spConfigurator->AddBinding(pkWorldTangent, 
            pkAttachableNode->GetInputResourceByVariableName(
            "WorldTangentIn")); 

        // This vector will not be normalized!
        pkTangentViewVector = 
            pkAttachableNode->GetOutputResourceByVariableName("VectorOut");
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleReflectionVectorFragment(Context& kContext,
    NiMaterialResource* pkWorldNorm, bool bNormalizeNormal,
    NiMaterialResource* pkWorldViewVector,
    NiMaterialResource*& pkWorldReflectionVector)
{
    NIASSERT(pkWorldNorm && pkWorldViewVector);

    NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
        "WorldReflect");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkWorldNorm, 
        pkNode->GetInputResourceByVariableName("WorldNrm"));

    kContext.m_spConfigurator->AddBinding(pkWorldViewVector, 
        pkNode->GetInputResourceByVariableName("WorldViewVector")); 

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", 
        bNormalizeNormal ? "(true)" : "(false)"),
        pkNode->GetInputResourceByVariableName("NormalizeNormal"));


    // This vector will not be normalized!
    pkWorldReflectionVector = 
        pkNode->GetOutputResourceByVariableName("WorldReflect");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleInitialSpecAmbDiffEmissiveColor(
    Context& kContext, bool bSpecularOn, AmbDiffEmissiveEnum eADF, 
    LightingModeEnum eLightingMode, NiMaterialResource*& pkDiffuseColorRes,
    NiMaterialResource*& pkSpecularColorRes, 
    NiMaterialResource*& pkSpecularPowerRes,
    NiMaterialResource*& pkAmbientColorRes,
    NiMaterialResource*& pkEmissiveColorRes,
    NiMaterialResource*& pkOpacityRes)
{
    pkSpecularColorRes = NULL;
    pkDiffuseColorRes = NULL;
    pkAmbientColorRes = NULL;
    pkEmissiveColorRes = NULL;
    pkSpecularPowerRes = NULL;
    pkOpacityRes = NULL;

    if (bSpecularOn)
    {
        pkSpecularColorRes = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_MATERIAL_SPECULAR);
        
        pkSpecularPowerRes = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_MATERIAL_POWER);
    }

    bool bVertexColors = (eADF == ADE_EMISSIVE || 
        (eADF == ADE_AMB_DIFF && eLightingMode == LIGHTING_E_A_D));
    NiMaterialResource* pkVertexColorRes = NULL;
    if (bVertexColors)
    {
        pkVertexColorRes = kContext.m_spInputs->AddOutputResource("float4",
            "Color", "", "VertexColors");
    }

    switch(eADF)
    {
        case ADE_AMB_DIFF:
            {
                pkEmissiveColorRes = 
                    AddOutputPredefined(kContext.m_spUniforms,
                    NiShaderConstantMap::SCM_DEF_MATERIAL_EMISSIVE);
                if (eLightingMode == LIGHTING_E_A_D)
                {
                    pkDiffuseColorRes = pkVertexColorRes;
                    pkAmbientColorRes = pkVertexColorRes;
                }
            }
            break;
        case ADE_EMISSIVE:
            {
                pkEmissiveColorRes = pkVertexColorRes;
                if (eLightingMode == LIGHTING_E_A_D)
                {
                    pkDiffuseColorRes = 
                        AddOutputPredefined(kContext.m_spUniforms,
                        NiShaderConstantMap::SCM_DEF_MATERIAL_DIFFUSE);
                    pkAmbientColorRes = 
                        AddOutputPredefined(kContext.m_spUniforms,
                        NiShaderConstantMap::SCM_DEF_MATERIAL_AMBIENT);
                }
            }
            break;
        case ADE_IGNORE:
            {
                pkEmissiveColorRes = 
                    AddOutputPredefined(kContext.m_spUniforms,
                    NiShaderConstantMap::SCM_DEF_MATERIAL_EMISSIVE);

                if (eLightingMode == LIGHTING_E_A_D)
                {
                    pkDiffuseColorRes = 
                        AddOutputPredefined(kContext.m_spUniforms,
                        NiShaderConstantMap::SCM_DEF_MATERIAL_DIFFUSE);
                    pkAmbientColorRes = 
                        AddOutputPredefined(kContext.m_spUniforms,
                        NiShaderConstantMap::SCM_DEF_MATERIAL_AMBIENT);
                }
            }
            break;
        default:
            NIASSERT(!"Unknown AmbientDiffuseEmissive Enum value");
            break;
    }

    if (pkDiffuseColorRes == NULL)
    {
        // If no diffuse color is set (because the lighting mode is 
        // LIGHTING_E), then alpha should come from the emissive component.
        if (!SplitColorAndOpacity(kContext, pkEmissiveColorRes, 
            pkEmissiveColorRes, pkOpacityRes))
        {
            return false;
        }
    }
    else
    {
        if (!SplitColorAndOpacity(kContext, pkDiffuseColorRes, 
            pkDiffuseColorRes, pkOpacityRes))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleParallaxMap(Context& kContext, 
    NiMaterialResource*& pkUVSet, NiMaterialResource* pkTangentViewVector)
{
    NiMaterialResource* pkColor = NULL;
    NiMaterialResource* pkOpacity = NULL;
    if (!InsertTexture(kContext, MAP_PARALLAX, 0, TEXTURE_RGB_APPLY_MULTIPLY, 
        TEXTURE_SAMPLE_RGB, pkUVSet, pkColor, NULL, pkColor, pkOpacity))
    {
        return false;
    }

    
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateParallaxOffset");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(
        pkUVSet,
        pkNode->GetInputResourceByVariableName("TexCoord"));
    kContext.m_spConfigurator->AddBinding(pkColor, 
        pkNode->GetInputResourceByVariableName("Height"));

    NiMaterialResource* pkParallaxOffset = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_PARALLAX_OFFSET);

    kContext.m_spConfigurator->AddBinding(pkParallaxOffset, 
        pkNode->GetInputResourceByVariableName("OffsetScale"));

    if (pkTangentViewVector)
    {
        kContext.m_spConfigurator->AddBinding(pkTangentViewVector, 
            pkNode->GetInputResourceByVariableName("TangentSpaceEyeVec"));
    }

    pkUVSet = 
        pkNode->GetOutputResourceByVariableName("ParallaxOffsetUV");

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleBaseMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NiMaterialResource*& pkDiffuseColorAccum,
    NiMaterialResource*& pkOpacityAccum, bool bOpacityOnly)
{
    NiMaterialResource* pkOpacity = NULL;
    TextureMapSampleType eSample = TEXTURE_SAMPLE_RGBA;

    if (bOpacityOnly)
        eSample = TEXTURE_SAMPLE_A;

    if (!InsertTexture(kContext, MAP_BASE, 0, TEXTURE_RGB_APPLY_MULTIPLY, 
        eSample, pkUVSet, pkDiffuseColorAccum, NULL, 
        pkDiffuseColorAccum, pkOpacity))
    {
        return false;
    }
    
    if (pkOpacityAccum)
    {
        return MultiplyVector(kContext, pkOpacityAccum, pkOpacity, 
            pkOpacityAccum);
    }
    else
    {
        pkOpacityAccum = pkOpacity;
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleNormalMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NormalMapType eType, 
    NiMaterialResource*& pkWorldNormal, NiMaterialResource* pkWorldBinormal,
    NiMaterialResource* pkWorldTangent)
{
    NiFixedString kSamplerName;
    unsigned int uiOccurance = 0;
    if (!GetTextureNameFromTextureEnum(MAP_NORMAL,
        kSamplerName, uiOccurance))
    {
        return false;
    }

    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
        kSamplerName, TEXTURE_SAMPLER_2D, uiOccurance);
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("TextureRGBASample");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(
        pkUVSet, pkNode->GetInputResourceByVariableName("TexCoord"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", "(false)"), 
        "Saturate", pkNode);

    NiMaterialResource* pkSampledColor = 
        pkNode->GetOutputResourceByVariableName("ColorOut");
    if (!pkSampledColor)
    {
        NIASSERT(!"Error in material");
        return false;
    }

    pkNode = GetAttachableNodeFromLibrary("CalculateNormalFromColor");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkSampledColor, 
        pkNode->GetInputResourceByVariableName("NormalMap"));
    kContext.m_spConfigurator->AddBinding(pkWorldNormal, 
        pkNode->GetInputResourceByVariableName("WorldNormalIn"));
    kContext.m_spConfigurator->AddBinding(pkWorldBinormal, 
        pkNode->GetInputResourceByVariableName("WorldBinormalIn"));
    kContext.m_spConfigurator->AddBinding(pkWorldTangent, 
        pkNode->GetInputResourceByVariableName("WorldTangentIn"));

    char acString[10];
    NiSprintf(acString, 10, "(%d)", (unsigned int) eType);
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("int", acString), 
        pkNode->GetInputResourceByVariableName(
        "NormalMapType"));

    pkWorldNormal = 
        pkNode->GetOutputResourceByVariableName("WorldNormalOut");

    return pkWorldNormal != NULL;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleDarkMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NiMaterialResource*& pkDiffuseColorAccum,
    NiMaterialResource*&)
{
    NiMaterialResource* pkOpacityRes = NULL;
    return InsertTexture(kContext, MAP_DARK, 0, TEXTURE_RGB_APPLY_ADD, 
        TEXTURE_SAMPLE_RGB, pkUVSet, pkDiffuseColorAccum, NULL, 
        pkDiffuseColorAccum, pkOpacityRes);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::InsertTexture(Context& kContext, 
    const NiFixedString& kSamplerName, unsigned int uiOccurance, 
    TextureMapApplyType eApplyType, TextureMapSampleType eSampleType, 
    NiMaterialResource* pkUV, NiMaterialResource* pkInputColor, 
    NiMaterialResource* pkSampledRGBScalar, 
    NiMaterialResource*& pkOutputColor,
    NiMaterialResource*& pkOutputOpacity)
{
    NiMaterialNode* pkNode = NULL;
    bool bSuccess = true;

    bool bOpacity = false;
    bool bColor = true;
    TextureMapSamplerType eSamplerType = TEXTURE_SAMPLER_2D;
    switch(eSampleType)
    {
        case TEXTURE_SAMPLE_RGBA:
            pkNode = GetAttachableNodeFromLibrary("TextureRGBASample");
            bOpacity = true;
            break;
        case TEXTURE_SAMPLE_A:
            pkNode = GetAttachableNodeFromLibrary("TextureRGBASample");
            bOpacity = true;
            bColor = false;
            break;
        case TEXTURE_SAMPLE_RGB:
            pkNode = GetAttachableNodeFromLibrary("TextureRGBSample");
            break;
        case TEXTURE_SAMPLE_PROJ_RGB:
            pkNode = GetAttachableNodeFromLibrary(
                "TextureRGBProjectSample");
            break;
        case TEXTURE_SAMPLE_CUBE_RGB:
            pkNode = GetAttachableNodeFromLibrary(
                "TextureRGBCubeSample");
            eSamplerType = TEXTURE_SAMPLER_CUBE;
            break;
        default:
            bSuccess = false;
            break;
    }

    if (!bSuccess)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    NiMaterialResource* pkSamplerRes = InsertTextureSampler(
        kContext, kSamplerName, eSamplerType, uiOccurance);

    if (!pkSamplerRes)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);
    bSuccess &= kContext.m_spConfigurator->AddBinding(
        pkUV, pkNode->GetInputResourceByVariableName("TexCoord"));
    bSuccess &= kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", "(false)"), 
        "Saturate", pkNode);

    if (!bSuccess)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    NiMaterialResource* pkSampledColor =
        pkNode->GetOutputResourceByVariableName("ColorOut");


    if (bOpacity && pkSampledColor->GetType()== "float4")
    {
        if (!SplitColorAndOpacity(kContext, pkSampledColor, pkSampledColor, 
            pkOutputOpacity))
        {
            NIASSERT(!"Error in fragment");
            return false;
        }
    }
    else
    {
        pkOutputOpacity = NULL;
    }

    if (!bColor)
    {
        pkSampledColor = NULL;
    }

    if (pkSampledRGBScalar && pkSampledColor && !ScaleVector(kContext, 
        pkSampledColor, pkSampledRGBScalar, pkSampledColor))
    {
        return false;
    }

    if (pkInputColor && pkSampledColor)
    {

        switch (eApplyType)
        {
            case TEXTURE_RGB_APPLY_MULTIPLY:
                bSuccess &= MultiplyVector(kContext, pkInputColor, 
                    pkSampledColor, pkSampledColor);
                break;
            case TEXTURE_RGB_APPLY_ADD:
                bSuccess &= AddVector(kContext, pkInputColor, 
                    pkSampledColor, pkSampledColor);
                break;
            case TEXTURE_RGB_APPLY_LERP:
                NIASSERT(pkOutputOpacity);
                bSuccess &= LerpVector(kContext, pkInputColor, 
                    pkSampledColor, pkOutputOpacity, pkSampledColor);
                break;
            default:
                bSuccess = false;
                break;
        }
    
        if (m_bSaturateTextures)
        {
            bSuccess &= SaturateVector(kContext, pkSampledColor, 
                pkSampledColor);
        }
    }


    if (!bSuccess)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    if (bColor)
        pkOutputColor = pkSampledColor;

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::InsertTexture(Context& kContext, TextureMap eMap, 
    unsigned int uiOccurance, TextureMapApplyType eApplyType, 
    TextureMapSampleType eSamplerType, NiMaterialResource* pkUV,
    NiMaterialResource* pkInputColor, NiMaterialResource* pkSampledRGBScalar,
    NiMaterialResource*& pkOutputColor, NiMaterialResource*& pkOutputOpacity)
{
    NiFixedString kSamplerName;
    if (!GetTextureNameFromTextureEnum(eMap, kSamplerName, uiOccurance))
    {
        return false;
    }

    return InsertTexture(kContext, kSamplerName, uiOccurance, eApplyType, 
        eSamplerType, pkUV, pkInputColor, pkSampledRGBScalar,
        pkOutputColor, pkOutputOpacity);
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleDetailMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NiMaterialResource*& pkDiffuseColorAccum,
    NiMaterialResource*&)
{
    NiMaterialResource* pkOpacityRes = NULL;
    return InsertTexture(kContext, MAP_DETAIL, 0,
        TEXTURE_RGB_APPLY_MULTIPLY, TEXTURE_SAMPLE_RGB, pkUVSet, 
        pkDiffuseColorAccum, 
        kContext.m_spStatics->AddOutputConstant("float", "(2.0)"), 
        pkDiffuseColorAccum, pkOpacityRes); 
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleGlossMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NiMaterialResource*& pkGlossiness)
{
    NiMaterialResource* pkOpacityRes = NULL;
    return InsertTexture(kContext, MAP_GLOSS, 0, TEXTURE_RGB_APPLY_MULTIPLY, 
        TEXTURE_SAMPLE_RGB, pkUVSet, pkGlossiness, NULL, pkGlossiness,
        pkOpacityRes); 
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleBumpMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NiMaterialResource*& pkBumpOffset)
{
    NiMaterialResource* pkColorRes = NULL;
    NiMaterialResource* pkOpacityRes = NULL;
    if (!InsertTexture(kContext, MAP_BUMP, 0, TEXTURE_RGB_APPLY_MULTIPLY, 
        TEXTURE_SAMPLE_RGB, pkUVSet, pkColorRes, NULL, pkColorRes,
        pkOpacityRes))
    {
        return false;
    }

    NiMaterialNode* pkNode = 
    GetAttachableNodeFromLibrary("CalculateBumpOffset");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(
        pkColorRes, pkNode->GetInputResourceByVariableName("DuDv"));
    kContext.m_spConfigurator->AddBinding(
        AddOutputPredefined(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_DEF_BUMP_MATRIX), 
        pkNode->GetInputResourceByVariableName("BumpMatrix"));

    pkBumpOffset = pkNode->GetOutputResourceByVariableName("BumpOffset");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleGlowMap(Context& kContext, 
    NiMaterialResource* pkUVSet, NiMaterialResource*& pkColorAccum,
    NiMaterialResource*&)
{
    NiMaterialResource* pkOpacityRes = NULL;
    return InsertTexture(kContext, MAP_GLOW, 0, TEXTURE_RGB_APPLY_ADD, 
        TEXTURE_SAMPLE_RGB, pkUVSet, pkColorAccum, NULL, pkColorAccum,
        pkOpacityRes); 
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleCustomMaps(Context&, 
    NiStandardPixelProgramDescriptor*, 
    unsigned int&, NiMaterialResource**,
    unsigned int,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&)
{
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleEnvMap(Context& kContext, 
    TexEffectType eTexEffect,
    NiMaterialResource* pkUVResource,
    NiMaterialResource*&, 
    NiMaterialResource*& pkSpecularAccum, 
    NiMaterialResource* pkBumpOffset,
    NiMaterialResource* pkGlossiness)
{
    const NiFixedString& kSamplerName = 
        NiTextureEffect::GetTypeNameFromID(NiTextureEffect::ENVIRONMENT_MAP);

    if (pkBumpOffset)
    {
        NiMaterialNode* pkBumpNode = NULL;
        if (pkUVResource->GetType() == "float2")
        {
            pkBumpNode = GetAttachableNodeFromLibrary(
                "OffsetUVFloat2");
        }
        else if (pkUVResource->GetType() == "float3")
        {
            pkBumpNode = GetAttachableNodeFromLibrary(
                "OffsetUVFloat3");
        }
        else if (pkUVResource->GetType() == "float4")
        {
            pkBumpNode = GetAttachableNodeFromLibrary(
                "OffsetUVFloat4");
        }

        if (!pkBumpNode)
        {
            NIASSERT(!"Fragment error");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkBumpNode);
        kContext.m_spConfigurator->AddBinding(pkUVResource, 
            "TexCoordIn", pkBumpNode);
        kContext.m_spConfigurator->AddBinding(pkBumpOffset,
            "TexCoordOffset", pkBumpNode);
        pkUVResource = pkBumpNode->GetOutputResourceByVariableName(
            "TexCoordOut");
    }

    NiMaterialResource* pkSampledColor = NULL;
    NiMaterialResource* pkOpacity = NULL;
    if (eTexEffect == TEXEFFECT_SPHERICAL)
    {
        if (!InsertTexture(kContext, kSamplerName, 0, 
            TEXTURE_RGB_APPLY_MULTIPLY, TEXTURE_SAMPLE_RGB, pkUVResource, 
            pkGlossiness, NULL, pkSampledColor, pkOpacity))
        {
            return false;
        }
    }
    else if (eTexEffect == TEXEFFECT_DIFFUSE_CUBE)
    {
        if (!InsertTexture(kContext, kSamplerName, 0, 
            TEXTURE_RGB_APPLY_MULTIPLY, TEXTURE_SAMPLE_CUBE_RGB, pkUVResource,
            pkGlossiness, NULL, pkSampledColor, pkOpacity))
        {
            return false;
        }
    }
    else if (eTexEffect == TEXEFFECT_SPECULAR_CUBE)
    {
        if (!InsertTexture(kContext, kSamplerName, 0, 
            TEXTURE_RGB_APPLY_MULTIPLY, TEXTURE_SAMPLE_CUBE_RGB, pkUVResource,
            pkGlossiness, NULL, pkSampledColor, pkOpacity))
        {
            return false;
        }
    }
    else if (eTexEffect == TEXEFFECT_WORLD_PERSPECTIVE)
    {
        if (!InsertTexture(kContext, kSamplerName, 0, 
            TEXTURE_RGB_APPLY_MULTIPLY, TEXTURE_SAMPLE_PROJ_RGB, pkUVResource,
            pkGlossiness, NULL, pkSampledColor, pkOpacity))
        {
            return false;
        }
    }
    else if (eTexEffect == TEXEFFECT_WORLD_PARALLEL)
    {
        if (!InsertTexture(kContext, kSamplerName, 0, 
            TEXTURE_RGB_APPLY_MULTIPLY, TEXTURE_SAMPLE_RGB, pkUVResource, 
            pkGlossiness, NULL, pkSampledColor, pkOpacity))
        {
            return false;
        }
    }

    if (pkSpecularAccum)
    {
        if (!AddVector(kContext, pkSampledColor, pkSpecularAccum, 
            pkSpecularAccum))
        {
            return false;
        }
    }
    else 
    {
        pkSpecularAccum = pkSampledColor;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleDecalMap(Context& kContext, 
    NiMaterialResource* pkUVSet, unsigned int uiIdx, 
    NiMaterialResource*& pkDiffuseAccum, 
    NiMaterialResource*&)
{
    NiMaterialResource* pkOpacityRes = NULL;
    return InsertTexture(kContext, (TextureMap)(MAP_DECAL00 + uiIdx), 0,
        TEXTURE_RGB_APPLY_LERP, TEXTURE_SAMPLE_RGBA, pkUVSet, pkDiffuseAccum, 
        NULL, pkDiffuseAccum, pkOpacityRes); 
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleProjLightMap(Context& kContext, 
    NiMaterialResource* pkUVResource, unsigned int uiIdx, 
    bool bPerspective, bool bClipped, NiMaterialResource* pkWorldPos,
    NiMaterialResource*& pkDiffuseAccum, 
    NiMaterialResource*&)
{
    NiMaterialResource* pkClipScalar = NULL;
    if (bClipped)
    {
        NiMaterialNode* pkClipNode = GetAttachableNodeFromLibrary(
            "ClippingPlaneTest");
        kContext.m_spConfigurator->AddNode(pkClipNode);
        kContext.m_spConfigurator->AddBinding(pkWorldPos, 
            pkClipNode->GetInputResourceByVariableName("WorldPos"));
        kContext.m_spConfigurator->AddBinding(
            AddOutputObject(kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE,
            NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP, uiIdx, 
            "ProjLightMap"),
            pkClipNode->GetInputResourceByVariableName("WorldClipPlane"));

        pkClipScalar = pkClipNode->GetOutputResourceByVariableName("Scalar");
    }
    TextureMapSampleType eSamplerType = TEXTURE_SAMPLE_RGB;
    if (bPerspective)
        eSamplerType = TEXTURE_SAMPLE_PROJ_RGB;
    
    NiMaterialResource* pkOpacity = NULL;
    if (!InsertTexture(kContext, 
        NiTextureEffect::GetTypeNameFromID(NiTextureEffect::PROJECTED_LIGHT), 
        uiIdx, TEXTURE_RGB_APPLY_ADD, eSamplerType, pkUVResource, 
        pkDiffuseAccum, pkClipScalar, pkDiffuseAccum, pkOpacity))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleProjShadowMap(Context& kContext, 
    NiMaterialResource* pkUVResource, unsigned int uiIdx, 
    bool bPerspective, bool bClipped, NiMaterialResource* pkWorldPos,
    NiMaterialResource*& pkDiffuseAccum,
    NiMaterialResource*&)
{
    NiMaterialResource* pkClipScalar = NULL;
    if (bClipped)
    {
        NiMaterialNode* pkClipNode = GetAttachableNodeFromLibrary(
            "ClippingPlaneTest");
        kContext.m_spConfigurator->AddNode(pkClipNode);
        kContext.m_spConfigurator->AddBinding(pkWorldPos, 
            pkClipNode->GetInputResourceByVariableName("WorldPos"));
        kContext.m_spConfigurator->AddBinding(
            AddOutputObject(kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE,
            NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP, uiIdx, 
            "ProjShadowMap"),
            pkClipNode->GetInputResourceByVariableName("WorldClipPlane"));
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("bool", "(true)"),
            pkClipNode->GetInputResourceByVariableName("InvertClip"));
    
        pkClipScalar = pkClipNode->GetOutputResourceByVariableName("Scalar");
    }

    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext, 
        NiTextureEffect::GetTypeNameFromID(NiTextureEffect::PROJECTED_SHADOW),
        TEXTURE_SAMPLER_2D,
        uiIdx);
    NiMaterialNode* pkNode = NULL;
    
    
    if (bPerspective)
    {
        pkNode = GetAttachableNodeFromLibrary(
            "TextureRGBProjectSample");
    }
    else
    {
        pkNode = GetAttachableNodeFromLibrary("TextureRGBSample");
    }

    kContext.m_spConfigurator->AddNode(pkNode);
    kContext.m_spConfigurator->AddBinding(
        pkUVResource,
        pkNode->GetInputResourceByVariableName("TexCoord"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));

    NiMaterialResource* pkSampledColor = 
        pkNode->GetOutputResourceByVariableName("ColorOut");

    if (!MultiplyScalarSatAddVector(kContext, pkDiffuseAccum, pkSampledColor, 
        pkClipScalar, pkDiffuseAccum))
    {
        return false;
    }
 
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleCalculateFog(Context& kContext, 
    NiMaterialResource* pkViewPos, Fog eFogType)
{
    if (eFogType == FOG_NONE)
        return true;

    // Bind fog resources
    NiMaterialNode* pkFogNode = GetAttachableNodeFromLibrary(
        "CalculateFog");
    kContext.m_spConfigurator->AddNode(pkFogNode);

    NiMaterialResource* pkFogOut = kContext.m_spOutputs->AddInputResource(
        "float", "Fog", "", "FogOut");
    kContext.m_spConfigurator->AddBinding(
        pkFogNode->GetOutputResourceByVariableName("FogOut"), pkFogOut);

    unsigned int uiFogType = 0;
    bool bFogRange = false;
    if (eFogType == FOG_LINEAR)
    {
        // NiFogProperty::FOG_Z_LINEAR
        uiFogType = 3; // 3 == linear
        bFogRange = false;
    }
    else if (eFogType == FOG_SQUARED)
    {
        // NiFogProperty::FOG_RANGE_SQ
        uiFogType = 3; // 3 == linear
        bFogRange = true;
    }

    char acValue[32];
    NiSprintf(acValue, 32, "(%d)", uiFogType);
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("int", acValue),
        pkFogNode->GetInputResourceByVariableName("FogType"));

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", 
        bFogRange ? "(true)" : "(false)"),
        pkFogNode->GetInputResourceByVariableName("FogRange"));

    NiMaterialResource* pkFogDensity = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_FOG_DENSITY); 
    kContext.m_spConfigurator->AddBinding(pkFogDensity, 
        pkFogNode->GetInputResourceByVariableName("FogDensity"));

    NiMaterialResource* pkFogNearFar = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_FOG_NEARFAR); 
    kContext.m_spConfigurator->AddBinding(pkFogNearFar, 
        pkFogNode->GetInputResourceByVariableName("FogStartEnd"));

    kContext.m_spConfigurator->AddBinding(pkViewPos, 
        pkFogNode->GetInputResourceByVariableName("ViewPosition"));

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleTextureUVSets(Context& kContext, 
    NiStandardVertexProgramDescriptor* pkVertexDesc, 
    NiMaterialResource** apkOutputUVResources, size_t,
    unsigned int& uiNextUVIndex)
{
    NIASSERT(apkOutputUVResources != NULL);

    unsigned int uiVertexOutputUVCount = pkVertexDesc->GetOUTPUTUVCOUNT();

    unsigned int uiUVSet = UINT_MAX;
    
    int iHighestPassthruOutputUVSet = -1;
    bool abInputPassThruTexCoordsUsed[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    unsigned int auiInputTexCoordsMapping[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    bool abInputTexCoordsUsed[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    memset(abInputPassThruTexCoordsUsed, 0, 
        sizeof(abInputPassThruTexCoordsUsed));
    memset(auiInputTexCoordsMapping, 0, sizeof(auiInputTexCoordsMapping));
    memset(abInputTexCoordsUsed, 0, sizeof(abInputTexCoordsUsed));

    unsigned int ui = 0;
    for (; ui < uiVertexOutputUVCount; ui++)
    {
        TexGenOutput eTexOutput;
        pkVertexDesc->GetOutputUVUsage(ui, uiUVSet, eTexOutput);
        NIASSERT(uiUVSet < STANDARD_PIPE_MAX_UVS_FOR_TEXTURES);

        abInputTexCoordsUsed[uiUVSet] = true;

        if (eTexOutput == TEX_OUT_PASSTHRU)
        {
            if (abInputPassThruTexCoordsUsed[uiUVSet] == false)
            {
                iHighestPassthruOutputUVSet++;
                abInputPassThruTexCoordsUsed[uiUVSet] = true;
            }            
        }
    }

    // Create a tex coord mapping that ensure there are no gaps
    unsigned int uiInputUVSetCount = 0;
    for (ui = 0; ui < STANDARD_PIPE_MAX_UVS_FOR_TEXTURES; ui++)
    {
        if (abInputTexCoordsUsed[ui])
        {
            auiInputTexCoordsMapping[ui] = uiInputUVSetCount++;
        }
    }

    // Generate Per-Vertex Input UV's
    NiMaterialResource* apkInputUVs[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    memset(apkInputUVs, 0, STANDARD_PIPE_MAX_UVS_FOR_TEXTURES * 
        sizeof(NiMaterialResource*));

    for (ui = 0; ui < uiInputUVSetCount; ui++)
    {
        NiFixedString kVarName = GenerateUVSetName(ui);
        apkInputUVs[ui] = kContext.m_spInputs->AddOutputResource("float2", 
            "TexCoord", "", kVarName);
    }

    // Generate Per-Vertex Output UV's.
    // Provide texture transforms, etc.
    uiNextUVIndex = (unsigned int)(iHighestPassthruOutputUVSet + 1);
    for (ui = 0; ui < uiVertexOutputUVCount; ui++)
    {
        unsigned int uiGenericUVSet;
        TexGenOutput eTexOutput;
        pkVertexDesc->GetOutputUVUsage(ui, uiGenericUVSet, eTexOutput);

        // Map between uiGenericUVSet (the generic tex coord expected by the
        // transform) and the renderer UV set (the tex coord semantic that
        // will be used by the shader)
        uiUVSet = auiInputTexCoordsMapping[uiGenericUVSet];

        unsigned int uiUVOut = ui;
        if (eTexOutput != TEX_OUT_PASSTHRU)
        {
            uiUVOut = uiNextUVIndex;
            uiNextUVIndex++;
        }

        if (!HandleTextureUVSetGen(kContext, apkInputUVs[uiUVSet], 
            apkOutputUVResources[uiUVOut], GenerateUVSetName(ui), uiUVOut,
            eTexOutput))
        {
            return false;
        }
    }

    NIASSERT(uiNextUVIndex == uiVertexOutputUVCount);

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleTextureEffectUVSets(Context& kContext,
    NiMaterialResource** apkOutputUVResources,
    size_t stOutputUVMax,
    NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm, 
    NiMaterialResource* pkWorldReflection, TexEffectType eEnvironmentMapType,
    unsigned int uiProjectedLightCount, unsigned int uiProjectedLightTypes,
    unsigned int uiProjectedShadowCount, unsigned int uiProjectedShadowTypes,
    unsigned int& uiNextUVIndex)
{
    NI_UNUSED_ARG(stOutputUVMax);
    NiMaterialResource* pkUVResource = NULL;
    if (!HandleTextureEffectUVSetGen(kContext, pkWorldPos, pkWorldNorm, 
        pkWorldReflection, NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP,
        eEnvironmentMapType, 0, pkUVResource))
    {
        return false;
    }

    if (pkUVResource)
    {
        NIASSERT(uiNextUVIndex != stOutputUVMax);
        apkOutputUVResources[uiNextUVIndex] = pkUVResource;
        uiNextUVIndex++;
    }


    unsigned int uiMask = 1;
    for (unsigned int ui = 0; ui < uiProjectedLightCount; ui++)
    {
        bool bPerspective = (uiProjectedLightTypes & uiMask) != 0;
        uiMask *= 2;

        TexEffectType eProjectedLightType = bPerspective ? 
            TEXEFFECT_WORLD_PERSPECTIVE : TEXEFFECT_WORLD_PARALLEL;
        
        if (!HandleTextureEffectUVSetGen(kContext, pkWorldPos, pkWorldNorm, 
            pkWorldReflection, 
            NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP,
            eProjectedLightType, ui, pkUVResource))
        {
            return false;
        }

        if (pkUVResource)
        {
            NIASSERT(uiNextUVIndex != stOutputUVMax);
            apkOutputUVResources[uiNextUVIndex] = pkUVResource;
            uiNextUVIndex++;
        }
    }

    uiMask = 1;
    for (unsigned int ui = 0; ui < uiProjectedShadowCount; ui++)
    {
        bool bPerspective = (uiProjectedShadowTypes & uiMask) != 0;
        uiMask *= 2;

        TexEffectType eProjectedShadowType = bPerspective ? 
            TEXEFFECT_WORLD_PERSPECTIVE : TEXEFFECT_WORLD_PARALLEL;
        
        if (!HandleTextureEffectUVSetGen(kContext, pkWorldPos, pkWorldNorm, 
            pkWorldReflection, 
            NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP,
            eProjectedShadowType, ui, pkUVResource))
        {
            return false;
        }

        if (pkUVResource)
        {
            NIASSERT(uiNextUVIndex != stOutputUVMax);
            apkOutputUVResources[uiNextUVIndex] = pkUVResource;
            uiNextUVIndex++;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleTextureEffectUVSetGen(Context& kContext,
    NiMaterialResource* pkWorldPos,
    NiMaterialResource* pkWorldNorm, 
    NiMaterialResource* pkWorldReflection,
    NiShaderAttributeDesc::ObjectType eObjType,
    TexEffectType eEffectType, unsigned int uiOccurance,
    NiMaterialResource*& pkUVResource)
{
    pkUVResource = NULL;
    NiMaterialResource* pkSrcResource = NULL;

    switch (eEffectType)
    {
        case TEXEFFECT_NONE:
            break;
        case TEXEFFECT_WORLD_PARALLEL:
            pkSrcResource = pkWorldPos;
            break;
        case TEXEFFECT_WORLD_PERSPECTIVE:
            pkSrcResource = pkWorldPos;
            break;
        case TEXEFFECT_SPHERICAL:
            // Must normalize the reflection vector here 
            NormalizeVector(kContext, pkWorldReflection);
            pkSrcResource = pkWorldReflection;
            break;
        case TEXEFFECT_DIFFUSE_CUBE:
            pkSrcResource = pkWorldNorm;
            break;
        case TEXEFFECT_SPECULAR_CUBE:
            pkSrcResource = pkWorldReflection;
            break;
        default:
            NIASSERT(!"Error in fragment");
            return false;
    }

    if (pkSrcResource && eObjType != NiShaderAttributeDesc::OT_UNDEFINED)
    {
        NiMaterialNode* pkNode = 
            GetAttachableNodeFromLibrary("ProjectTextureCoordinates");
        kContext.m_spConfigurator->AddNode(pkNode);

        kContext.m_spConfigurator->AddBinding(
            pkSrcResource,
            pkNode->GetInputResourceByVariableName("TexCoord"));
        
        NiMaterialResource* pkTexTransform = 
            AddOutputObject(kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSFORM,
            eObjType, uiOccurance, 
            NiShaderAttributeDesc::GetObjectTypeAsString(eObjType));

        kContext.m_spConfigurator->AddBinding(
            pkTexTransform,
            pkNode->GetInputResourceByVariableName("TexTransform"));

        pkUVResource = 
            pkNode->GetOutputResourceByVariableName("TexCoordOut");
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePreLightTextureApplication(Context& kContext, 
    NiStandardPixelProgramDescriptor* pkPixelDesc, 
    NiMaterialResource*& pkWorldPos,
    NiMaterialResource*& pkWorldNormal,
    NiMaterialResource*& pkWorldBinormal,
    NiMaterialResource*& pkWorldTangent,
    NiMaterialResource*&,
    NiMaterialResource*& pkTangentViewVector,
    NiMaterialResource*& pkMatDiffuseColor,
    NiMaterialResource*& pkMatSpecularColor,
    NiMaterialResource*& pkMatSpecularPower,
    NiMaterialResource*& pkMatGlossiness,
    NiMaterialResource*& pkMatAmbientColor,
    NiMaterialResource*& pkMatEmissiveColor,
    NiMaterialResource*& pkOpacityAccum,
    NiMaterialResource*& pkAmbientLightAccum,
    NiMaterialResource*& pkDiffuseLightAccum,
    NiMaterialResource*& pkSpecularLightAccum,
    NiMaterialResource*& pkDiffuseTexAccum,
    NiMaterialResource*& pkSpecularTexAccum,
    unsigned int& uiTexturesApplied,
    NiMaterialResource** apkUVSets,
    unsigned int uiNumStandardUVs,
    unsigned int)
{
    unsigned int uiWhichTexture = 0;

    if (pkPixelDesc->GetPARALLAXMAPCOUNT()  == 1)
    {
        unsigned int uiUVSet = FindUVSetIndexForTextureEnum( 
            MAP_PARALLAX, pkPixelDesc);

        NiMaterialResource* pkUVSet = apkUVSets[uiUVSet];

        if (!HandleParallaxMap(kContext, pkUVSet, pkTangentViewVector))
        {
            return false;
        }
        uiWhichTexture++;

        apkUVSets[uiUVSet] = pkUVSet;
    }

    bool bNormalMap = pkPixelDesc->GetNORMALMAPCOUNT() != 0;
    if (bNormalMap)
    {
        NormalMapType eNormMapType = (NormalMapType)
            pkPixelDesc->GetNORMALMAPTYPE();

        if (!HandleNormalMap(kContext, 
            FindUVSetIndexForTextureEnum(MAP_NORMAL, 
            pkPixelDesc, apkUVSets), eNormMapType, pkWorldNormal, 
            pkWorldBinormal, pkWorldTangent))
        {
            return false;
        }
        uiWhichTexture++;
    }
    
    if (pkPixelDesc->GetDARKMAPCOUNT() == 1)
    {
        if (!HandleDarkMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_DARK, 
            pkPixelDesc, apkUVSets), pkDiffuseTexAccum,
            pkSpecularTexAccum))
        {
            return false;
        }

        uiWhichTexture++;
    }
    unsigned int uiTexEffectOffset = 0;

    if (pkPixelDesc->GetENVMAPTYPE() != TEXEFFECT_NONE && 
        pkPixelDesc->GetNORMALMAPCOUNT() == 0)
    {
        uiTexEffectOffset++;
    }

    unsigned int uiProjLightMapCount = pkPixelDesc->GetPROJLIGHTMAPCOUNT();

    if (uiProjLightMapCount > 0)
    {
        unsigned int uiMask = 1;
        for (unsigned int ui = 0; ui < uiProjLightMapCount; ui++)
        {

            bool bPerspective = (pkPixelDesc->GetPROJLIGHTMAPTYPES() & uiMask)
                != 0;

            bool bClipped = (pkPixelDesc->GetPROJLIGHTMAPCLIPPED() & uiMask)
                != 0;

            if (!HandleProjLightMap(kContext, 
                apkUVSets[uiNumStandardUVs + uiTexEffectOffset], ui,
                bPerspective, bClipped, pkWorldPos, pkDiffuseTexAccum,
                pkSpecularTexAccum))
            {
                return false;
            }
            uiTexEffectOffset++;
            uiMask *= 2;
        }
    }

    if (pkPixelDesc->GetBASEMAPCOUNT() == 1)
    {
         bool bAlphaOnly = pkPixelDesc->GetBASEMAPALPHAONLY() == 1;

         if (!HandleBaseMap(kContext, 
             FindUVSetIndexForTextureEnum( MAP_BASE, 
             pkPixelDesc, apkUVSets), pkDiffuseTexAccum, pkOpacityAccum, 
             bAlphaOnly))
         {
             return false;
         }
         uiWhichTexture++;
    }
    else if (pkDiffuseTexAccum == NULL)
    {
        // If there's no base map, must use a constant instead.
        pkDiffuseTexAccum = kContext.m_spStatics->AddOutputConstant(
            "float3", "(1.0f, 1.0f, 1.0f)");
    }

    if (pkPixelDesc->GetDETAILMAPCOUNT() == 1)
    {
        if (!HandleDetailMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_DETAIL, 
            pkPixelDesc, apkUVSets), pkDiffuseTexAccum,
            pkSpecularTexAccum))
        {
            return false;
        }

        uiWhichTexture++;
    }

    
    unsigned int uiDecalMapCount = pkPixelDesc->GetDECALMAPCOUNT();
    if (uiDecalMapCount > 0)
    {
        for (unsigned int ui = 0; ui < uiDecalMapCount; ui++)
        {
            if (!HandleDecalMap(kContext, 
                FindUVSetIndexForTextureEnum( 
                (TextureMap)(MAP_DECAL00 + ui), 
                pkPixelDesc, apkUVSets), ui, pkDiffuseTexAccum,
                pkSpecularTexAccum))
            {
                return false;
            }

            uiWhichTexture++;
        }
    }


    if (!HandleCustomMaps(kContext, pkPixelDesc, uiWhichTexture,
        apkUVSets, uiNumStandardUVs, pkMatDiffuseColor, pkMatSpecularColor,
        pkMatSpecularPower, pkMatGlossiness, pkMatAmbientColor,
        pkMatEmissiveColor, pkOpacityAccum, pkAmbientLightAccum,
        pkDiffuseLightAccum, pkSpecularLightAccum, pkDiffuseTexAccum,
        pkSpecularTexAccum))
    {
        return false;
    }
    
    unsigned int uiProjShadowMapCount = pkPixelDesc->GetPROJSHADOWMAPCOUNT();

    if (uiProjShadowMapCount > 0)
    {
        unsigned int uiMask = 1;
        for (unsigned int ui = 0; ui < uiProjShadowMapCount; ui++)
        {
            bool bPerspective = 
                (pkPixelDesc->GetPROJSHADOWMAPTYPES() & uiMask) != 0;

            bool bClipped =
                (pkPixelDesc->GetPROJSHADOWMAPCLIPPED() & uiMask) != 0;

            if (!HandleProjShadowMap(kContext, 
                apkUVSets[uiNumStandardUVs + uiTexEffectOffset], ui,
                bPerspective, bClipped, pkWorldPos, pkDiffuseTexAccum,
                pkSpecularTexAccum))
            {
                return false;
            }
            uiTexEffectOffset++;
            uiMask *= 2;
        }
    }

    if (pkPixelDesc->GetGLOSSMAPCOUNT() == 1)
    {
        if (!HandleGlossMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_GLOSS, 
            pkPixelDesc, apkUVSets), pkMatGlossiness))
        {
            return false;
        }
        uiWhichTexture++;
    }

    uiTexturesApplied = uiWhichTexture;
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandlePostLightTextureApplication(Context& kContext, 
    NiStandardPixelProgramDescriptor* pkPixelDesc,
    NiMaterialResource*& pkWorldNormal,
    NiMaterialResource* pkWorldView, 
    NiMaterialResource*&,
    NiMaterialResource*& pkDiffuseAccum,
    NiMaterialResource*& pkSpecularAccum,
    NiMaterialResource* pkGlossiness,
    unsigned int& uiTexturesApplied,
    NiMaterialResource** apkUVSets,
    unsigned int uiNumStandardUVs,
    unsigned int)
{
    unsigned int uiWhichTexture = 0;
    
    NiMaterialResource* pkBumpOffset = NULL;
    if (pkPixelDesc->GetBUMPMAPCOUNT() == 1)
    {
        if (!HandleBumpMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_BUMP, 
            pkPixelDesc, apkUVSets), pkBumpOffset))
        {
            return false;
        }

        uiWhichTexture++;
    }


    TexEffectType eEnvMapType = 
        (TexEffectType) pkPixelDesc->GetENVMAPTYPE();

    if (eEnvMapType != TEXEFFECT_NONE)
    {
        NiMaterialResource* pkEnvUV = NULL;

        // If the normal was not input per-vertex, it was computed in the 
        // pixel shader and thus all environment map calcs need to go through 
        // it.
        if (pkWorldNormal && 
            kContext.m_spConfigurator->GetNodeByName(
            "CalculateNormalFromColor") != NULL)
        {
            NiMaterialResource* pkWorldReflect = NULL;
            if (eEnvMapType == TEXEFFECT_SPHERICAL || 
                eEnvMapType == TEXEFFECT_SPECULAR_CUBE)
            {
                // Normal is normalized at this point!
                if (!HandleReflectionVectorFragment(kContext, pkWorldNormal, 
                    false, pkWorldView, pkWorldReflect))
                {
                    return NULL;
                }
            }

            if (!HandleTextureEffectUVSetGen(kContext, NULL,
                pkWorldNormal, pkWorldReflect,
                NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP, eEnvMapType,
                0, pkEnvUV))
            {
                return false;
            }
        }
        else
        {
            pkEnvUV = apkUVSets[uiNumStandardUVs];
        }
       
        if (!HandleEnvMap(kContext, eEnvMapType, pkEnvUV, 
            pkDiffuseAccum, pkSpecularAccum, pkBumpOffset, 
            pkGlossiness))
        {
            return false;
        }
    }   
    
    if (pkPixelDesc->GetGLOWMAPCOUNT() == 1)
    {
        if (!HandleGlowMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_GLOW, 
            pkPixelDesc, apkUVSets), pkDiffuseAccum,
            pkSpecularAccum))
        {
            return false;
        }
        uiWhichTexture++;
    }

    uiTexturesApplied += uiWhichTexture;
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleTextureUVSetGen(Context& kContext, 
    NiMaterialResource* pkInputUVResource, 
    NiMaterialResource*& pkOutputUVResource,
    const NiFixedString& kMapName, unsigned int uiExtra,
    TexGenOutput eOutputType)
{
    NiFixedString kOutTexCoordType = NULL;
    NiMaterialResource* pkTexCoordRes = NULL;

    pkOutputUVResource = pkTexCoordRes = pkInputUVResource;

    if (!pkTexCoordRes)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kOutTexCoordType = "float2";
    NIASSERT(kOutTexCoordType != NULL);

    NiFixedString kVarName = kMapName;
    
    if (eOutputType == TEX_OUT_TRANSFORM)
    {
        NiMaterialNode* pkNode = 
            GetAttachableNodeFromLibrary("TexTransformApply");
        kContext.m_spConfigurator->AddNode(pkNode);
        kContext.m_spConfigurator->AddBinding(pkTexCoordRes, 
            pkNode->GetInputResourceByVariableName("TexCoord"));
        
        char acName[32];
        NiSprintf(acName, 32, "%s_TexTransform", (const char*)
            kMapName);

        NiMaterialResource* pkTexTransform = 
            kContext.m_spUniforms->AddOutputResource("float4x4",
            "TexTransform", "", acName, 1, 
            NiMaterialResource::SOURCE_PREDEFINED,
            NiShaderAttributeDesc::OT_UNDEFINED, uiExtra);

        kContext.m_spConfigurator->AddBinding(pkTexTransform, 
            pkNode->GetInputResourceByVariableName("TexTransform"));

        pkOutputUVResource = 
            pkNode->GetOutputResourceByVariableName("TexCoordOut");
    }
    else if (eOutputType != TEX_OUT_PASSTHRU)
    {
        NIASSERT(!"Unsupported TexGenOutput type.");
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleLighting(Context& kContext, 
    bool bSpecular, unsigned int uiNumPoint, unsigned int uiNumDirectional, 
    unsigned int uiNumSpot, unsigned int uiShadowBitfield,
    unsigned int uiShadowTechnique, NiMaterialResource* pkWorldPos, 
    NiMaterialResource* pkWorldNorm, NiMaterialResource* pkViewVector, 
    NiMaterialResource* pkSpecularPower, NiMaterialResource*& pkAmbientAccum, 
    NiMaterialResource*& pkDiffuseAccum, NiMaterialResource*& pkSpecularAccum)
{
    unsigned int uiLight = 0;
    NiMaterialResource* pkShadow = NULL;

    
    unsigned int uiLightTypeCount = 0;
    unsigned int uiShadowLightTypeCount = 0;
    for (unsigned int ui = 0; ui < uiNumPoint; ui++)
    {
        pkShadow = NULL;
        unsigned int uiMask = NiTGetBitMask<unsigned int>(uiLight, 1);
        bool bShadow = NiTGetBit<unsigned int>(uiShadowBitfield, uiMask);
        if (bShadow && NiShadowManager::GetShadowManager())
        {
            if (!HandleShadow(kContext, uiLight, LIGHT_POINT, 
                uiShadowLightTypeCount, pkWorldPos, pkWorldNorm, 
                uiShadowTechnique, pkShadow))
            {
                return false;
            }

            if (!HandleLight(kContext, uiLight, LIGHT_POINT, 
                uiShadowLightTypeCount, pkWorldPos, pkWorldNorm, pkViewVector,
                pkSpecularPower, pkAmbientAccum, pkDiffuseAccum, 
                pkSpecularAccum, bSpecular, pkShadow))
            {
                return false;
            }

            uiShadowLightTypeCount++;
        }
        else
        {
            if (!HandleLight(kContext, uiLight, LIGHT_POINT, uiLightTypeCount,
                pkWorldPos, pkWorldNorm, pkViewVector, pkSpecularPower,
                pkAmbientAccum, pkDiffuseAccum, pkSpecularAccum, bSpecular,
                pkShadow))
            {
                return false;
            }
            uiLightTypeCount++;
        }


        uiLight++;
    }

    uiLightTypeCount = uiShadowLightTypeCount = 0;
    for (unsigned int ui = 0; ui < uiNumDirectional; ui++)
    {
        pkShadow = NULL;
        unsigned int uiMask = NiTGetBitMask<unsigned int>(uiLight, 1);
        bool bShadow = NiTGetBit<unsigned int>(uiShadowBitfield, uiMask);
        if (bShadow && NiShadowManager::GetShadowManager())
        {
            if (!HandleShadow(kContext, uiLight, LIGHT_DIR, 
                uiShadowLightTypeCount, pkWorldPos, pkWorldNorm, 
                uiShadowTechnique, pkShadow))
            {
                return false;
            }

            if (!HandleLight(kContext, uiLight, LIGHT_DIR, 
                uiShadowLightTypeCount, pkWorldPos, pkWorldNorm, pkViewVector,
                pkSpecularPower, pkAmbientAccum, pkDiffuseAccum,
                pkSpecularAccum, bSpecular, pkShadow))
            {
                return false;
            }
            uiShadowLightTypeCount++;
        }
        else
        {
            if (!HandleLight(kContext, uiLight, LIGHT_DIR, uiLightTypeCount, 
                pkWorldPos, pkWorldNorm, pkViewVector, pkSpecularPower, 
                pkAmbientAccum, pkDiffuseAccum, pkSpecularAccum, bSpecular, 
                pkShadow))
            {
                return false;
            }
            uiLightTypeCount++;
        }
        uiLight++;
    }

    uiLightTypeCount = uiShadowLightTypeCount = 0;
    for (unsigned int ui = 0; ui < uiNumSpot; ui++)
    {
        pkShadow = NULL;
        unsigned int uiMask = NiTGetBitMask<unsigned int>(uiLight, 1);
        bool bShadow = NiTGetBit<unsigned int>(uiShadowBitfield, uiMask);
        if (bShadow && NiShadowManager::GetShadowManager())
        {
            if (!HandleShadow(kContext, uiLight, LIGHT_SPOT, 
                uiShadowLightTypeCount, pkWorldPos, pkWorldNorm, 
                uiShadowTechnique, pkShadow))
            {
                return false;
            }

            if (!HandleLight(kContext, uiLight, LIGHT_SPOT, 
                uiShadowLightTypeCount, pkWorldPos, pkWorldNorm, pkViewVector,
                pkSpecularPower, pkAmbientAccum, pkDiffuseAccum,
                pkSpecularAccum, bSpecular, pkShadow))
            {
                return false;
            }
            uiShadowLightTypeCount++;
        }
        else
        {
            if (!HandleLight(kContext, uiLight, LIGHT_SPOT, uiLightTypeCount, 
                pkWorldPos, pkWorldNorm, pkViewVector, pkSpecularPower, 
                pkAmbientAccum, pkDiffuseAccum, pkSpecularAccum, bSpecular, 
                pkShadow))
            {
                return false;
            }
            uiLightTypeCount++;
        }
        uiLight++;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleShadow(Context& kContext, 
    unsigned int, LightType eType, unsigned int uiLightByType,
    NiMaterialResource* pkWorldPos, NiMaterialResource*,
    unsigned int uiShadowTechnique, NiMaterialResource*& pkShadow)
{
    char acValue[256];

    NiShadowTechnique* pkShadowTechnique = 
        NiShadowManager::GetKnownShadowTechnique(
        (unsigned short)uiShadowTechnique);

    if (!pkShadowTechnique)
        return false;

    NiMaterialNode* pkShadowNode = GetAttachableNodeFromLibrary(
            pkShadowTechnique->GetReadFragmentName(eType));
    kContext.m_spConfigurator->AddNode(pkShadowNode);
    NIASSERT(pkShadowNode);

    // WorldPos
    kContext.m_spConfigurator->AddBinding(pkWorldPos, 
        pkShadowNode->GetInputResourceByVariableName("WorldPos"));

    // LightType
    NiSprintf(acValue, sizeof(acValue), "(%d)", (unsigned int) eType);
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("int", acValue),
        pkShadowNode->GetInputResourceByVariableName("LightType"));

    NiShaderAttributeDesc::ObjectType eObjType = 
        NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT;
    const char* pcVariableModifier = NULL;
    switch (eType)
    {
    case LIGHT_DIR:
        eObjType = NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT;
        pcVariableModifier = "ShadowDir";
        break;
    case LIGHT_POINT:
        eObjType = NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT;
        pcVariableModifier = "ShadowPoint";
        break;
    case LIGHT_SPOT:
        eObjType = NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT;
        pcVariableModifier = "ShadowSpot";
        break;
    default:
        NIASSERT(!"Unknown enumeration");
        break;
    }

    // LightDirection
    if (eType == LIGHT_SPOT) 
    {
        NiMaterialResource* pkLightDir = AddOutputObject(
            kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION, eObjType,
            uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkLightDir, 
            pkShadowNode->GetInputResourceByVariableName("LightDirection"));
    }

    NiMaterialResource* pkLightPos = 
        pkShadowNode->GetInputResourceByVariableName("LightPos");
    if (pkLightPos)
    {
        // LightPos
        NiMaterialResource* pkLightPosObj = AddOutputObject(
            kContext.m_spUniforms,NiShaderConstantMap::SCM_OBJ_WORLDPOSITION, 
            eObjType, uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkLightPosObj, pkLightPos);
    }

    // ShadowMapSize
    NiMaterialResource* pkInputResource = 
        pkShadowNode->GetInputResourceByVariableName("ShadowMapSize");
    if ( pkInputResource)
    {
        NiMaterialResource* pkShadowMapSize = AddOutputObject(
            kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_SHADOWMAPTEXSIZE, eObjType, 
            uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkShadowMapSize, 
            pkInputResource);
    }

    // ShadowBias
    pkInputResource = 
        pkShadowNode->GetInputResourceByVariableName("ShadowBias");
    if (pkInputResource)
    {
        NiMaterialResource* pkShadowBias = AddOutputObject(
            kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_SHADOWBIAS, eObjType, 
            uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkShadowBias, 
            pkInputResource);
    }

    // ShadowVSMPower
    pkInputResource = 
        pkShadowNode->GetInputResourceByVariableName("ShadowVSMPowerEpsilon");
    if (pkInputResource)
    {
        NiMaterialResource* pkShadowVSMPowerEp = AddOutputObject(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_OBJ_SHADOW_VSM_POWER_EPSILON, eObjType, 
            uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkShadowVSMPowerEp, 
            pkInputResource);
    }

    // ShadowMap
    NiFixedString kSamplerName;
    TextureMap eMap = MAP_DIRSHADOW00;

    NIASSERT(uiLightByType < 8 && "Too many shadow lights");
    switch(eType)
    {
    case LIGHT_DIR:
        eMap = (NiStandardMaterial::TextureMap)
            (MAP_DIRSHADOW00 + uiLightByType);
        break;
    case LIGHT_SPOT:
        eMap = (NiStandardMaterial::TextureMap)
            (MAP_SPOTSHADOW00 + uiLightByType);
        break;
    case LIGHT_POINT:
        eMap = (NiStandardMaterial::TextureMap)
            (MAP_POINTSHADOW00 + uiLightByType);
        break;
    default:
        NIASSERT(!"Invalid many light type.");
    }
    
    if (!GetTextureNameFromTextureEnum(eMap, kSamplerName, uiLightByType))
    {
        return false;
    }
      
    NiMaterialResource* pkSamplerRes;
    if (eType == LIGHT_POINT && 
        pkShadowTechnique->GetUseCubeMapForPointLight())
    {
        pkSamplerRes = InsertTextureSampler(kContext,
            kSamplerName, TEXTURE_SAMPLER_CUBE, uiLightByType, eObjType);
    }
    else
    {
        NiMaterialResource* pkV2LProj = AddOutputObject(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_WORLDTOSHADOWMAPMATRIX, eObjType, 
            uiLightByType, pcVariableModifier);

        kContext.m_spConfigurator->AddBinding(pkV2LProj,
            pkShadowNode->GetInputResourceByVariableName(
            "WorldToLightProjMat"));

        pkSamplerRes = InsertTextureSampler(kContext,
            kSamplerName, TEXTURE_SAMPLER_2D, uiLightByType, eObjType);
    }

    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkShadowNode->GetInputResourceByVariableName("ShadowMap"));

    pkShadow = pkShadowNode->GetOutputResourceByVariableName(
        "ShadowOut");

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleLight(Context& kContext, unsigned int, 
    LightType eType, unsigned int uiLightByType, 
    NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm, 
    NiMaterialResource* pkViewVector, NiMaterialResource* pkSpecularPower, 
    NiMaterialResource*& pkAmbientAccum, NiMaterialResource*& pkDiffuseAccum, 
    NiMaterialResource*& pkSpecularAccum, bool bSpecular, 
    NiMaterialResource* pkShadow)
{
    NiMaterialNode* pkLightNode = GetAttachableNodeFromLibrary(
        "Light");
    kContext.m_spConfigurator->AddNode(pkLightNode);

    kContext.m_spConfigurator->AddBinding(pkWorldPos, 
        pkLightNode->GetInputResourceByVariableName("WorldPos"));
    kContext.m_spConfigurator->AddBinding(pkWorldNorm, 
        pkLightNode->GetInputResourceByVariableName("WorldNrm"));

    if (pkAmbientAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkAmbientAccum, 
            pkLightNode->GetInputResourceByVariableName("AmbientAccum"));
    }

    if (pkDiffuseAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkDiffuseAccum, 
            pkLightNode->GetInputResourceByVariableName("DiffuseAccum"));
    }

    if (pkSpecularAccum && bSpecular)
    {
        kContext.m_spConfigurator->AddBinding(pkSpecularAccum, 
            pkLightNode->GetInputResourceByVariableName("SpecularAccum"));
    }

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant(
        "bool", bSpecular ? "(true)" : "(false)"),
        pkLightNode->GetInputResourceByVariableName("SpecularEnable"));

    if (pkShadow)
    {
        kContext.m_spConfigurator->AddBinding(
            pkShadow, pkLightNode->GetInputResourceByVariableName("Shadow"));
    }

    char acValue[32];
    NiSprintf(acValue, 32, "(%d)", (unsigned int) eType);
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("int", acValue),
        pkLightNode->GetInputResourceByVariableName("LightType"));
    NiShaderAttributeDesc::ObjectType eObjType = 
        NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT;
    const char* pcVariableModifier = NULL;
    switch (eType)
    {
        case LIGHT_DIR:
            if (pkShadow)
            {
                eObjType = 
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT;
                pcVariableModifier = "ShadowDir";
            }
            else
            {
                eObjType = NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT;
                pcVariableModifier = "Dir";
            }
            break;
        case LIGHT_POINT:
            if (pkShadow)
            {
                eObjType = NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT;
                pcVariableModifier = "ShadowPoint";
            }
            else
            {
                eObjType = NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT;
                pcVariableModifier = "Point";
            }
            break;
        case LIGHT_SPOT:
            if (pkShadow)
            {
                eObjType = NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT;
                pcVariableModifier = "ShadowSpot";
            }
            else
            {
                eObjType = NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT;
                pcVariableModifier = "Spot";
            }

            break;
        default:
            NIASSERT(!"Unknown enumeration");
            break;
    }

    NiMaterialResource* pkRes = AddOutputObject(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_OBJ_AMBIENT, eObjType, uiLightByType,
        pcVariableModifier);

    kContext.m_spConfigurator->AddBinding(pkRes, 
        pkLightNode->GetInputResourceByVariableName("LightAmbient"));
    
    pkRes = AddOutputObject(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_OBJ_DIFFUSE, eObjType, uiLightByType,
        pcVariableModifier);

    kContext.m_spConfigurator->AddBinding(pkRes, 
        pkLightNode->GetInputResourceByVariableName("LightDiffuse"));

    pkRes = AddOutputObject(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_OBJ_SPECULAR, eObjType, uiLightByType,
        pcVariableModifier);
    kContext.m_spConfigurator->AddBinding(pkRes, 
        pkLightNode->GetInputResourceByVariableName("LightSpecular"));

    pkRes = AddOutputObject(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_OBJ_WORLDPOSITION, eObjType, uiLightByType,
        pcVariableModifier);
    kContext.m_spConfigurator->AddBinding(pkRes, 
        pkLightNode->GetInputResourceByVariableName("LightPos"));

    if (eType != LIGHT_DIR) // LIGHT_SPOT or LIGHT_POINT
    {
        pkRes = AddOutputObject(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_ATTENUATION, eObjType, uiLightByType,
            pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkRes, 
            pkLightNode->GetInputResourceByVariableName("LightAttenuation"));
    }

    if (eType != LIGHT_POINT) // LIGHT_SPOT or LIGHT_DIR
    {
        pkRes = AddOutputObject(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION, eObjType, 
            uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkRes, 
            pkLightNode->GetInputResourceByVariableName("LightDirection"));
    }

    if (eType == LIGHT_SPOT)
    {
        pkRes = AddOutputObject(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_SPOTATTENUATION, eObjType, 
            uiLightByType, pcVariableModifier);
        kContext.m_spConfigurator->AddBinding(pkRes, 
            pkLightNode->GetInputResourceByVariableName(
            "LightSpotAttenuation"));
    }

    if (bSpecular)
    {
        if (pkViewVector)
        {
            kContext.m_spConfigurator->AddBinding(pkViewVector, 
                pkLightNode->GetInputResourceByVariableName(
                "WorldViewVector"));
        }

        if (pkSpecularPower)
        {
            kContext.m_spConfigurator->AddBinding(pkSpecularPower, 
                pkLightNode->GetInputResourceByVariableName(
                "SpecularPower"));
        }

    }

    pkAmbientAccum = pkLightNode->GetOutputResourceByVariableName(
        "AmbientAccumOut");

    pkDiffuseAccum = pkLightNode->GetOutputResourceByVariableName(
        "DiffuseAccumOut");

    if (bSpecular)
    {
        pkSpecularAccum = pkLightNode->GetOutputResourceByVariableName(
            "SpecularAccumOut");
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleShadingCoefficients(Context& kContext,
    NiMaterialResource* pkMatEmissive, NiMaterialResource* pkMatDiffuse, 
    NiMaterialResource* pkMatAmbient,  NiMaterialResource* pkMatSpecular, 
    NiMaterialResource* pkLightSpecularAccum, 
    NiMaterialResource* pkLightDiffuseAccum, 
    NiMaterialResource* pkLightAmbientAccum,
    NiMaterialResource*& pkDiffuseCoeff, NiMaterialResource*& pkSpecularCoeff)
{
    NiMaterialNode* pkShadeNode = 
        GetAttachableNodeFromLibrary("ComputeShadingCoefficients");
    kContext.m_spConfigurator->AddNode(pkShadeNode);

    if (pkMatDiffuse == NULL)
    {
        pkMatDiffuse = kContext.m_spStatics->AddOutputConstant(
            "float3","(1.0, 1.0, 1.0)");
    }
    kContext.m_spConfigurator->AddBinding(pkMatDiffuse, 
        pkShadeNode->GetInputResourceByVariableName(
        "MatDiffuse"));

    if (pkMatSpecular == NULL)
    {
        pkMatSpecular = kContext.m_spStatics->AddOutputConstant(
            "float3","(1.0, 1.0, 1.0)");
    }
    kContext.m_spConfigurator->AddBinding(pkMatSpecular, 
        pkShadeNode->GetInputResourceByVariableName(
        "MatSpecular"));

    if (pkMatAmbient == NULL)
    {
        pkMatAmbient = kContext.m_spStatics->AddOutputConstant(
            "float3","(1.0, 1.0, 1.0)");
    }
    kContext.m_spConfigurator->AddBinding(pkMatAmbient, 
        pkShadeNode->GetInputResourceByVariableName(
        "MatAmbient"));

    if (pkMatEmissive == NULL)
    {
        pkMatEmissive = kContext.m_spStatics->AddOutputConstant(
            "float3","(0.0, 0.0, 0.0)");
    }
    kContext.m_spConfigurator->AddBinding(pkMatEmissive, 
        pkShadeNode->GetInputResourceByVariableName(
        "MatEmissive"));

    if (pkLightDiffuseAccum)
    {
         kContext.m_spConfigurator->AddBinding(pkLightDiffuseAccum, 
            pkShadeNode->GetInputResourceByVariableName(
            "LightDiffuseAccum"));
    }

    if (pkLightSpecularAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkLightSpecularAccum, 
            pkShadeNode->GetInputResourceByVariableName(
            "LightSpecularAccum"));
    }

    if (pkLightAmbientAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkLightAmbientAccum, 
            pkShadeNode->GetInputResourceByVariableName(
            "LightAmbientAccum"));
    }

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", m_bSaturateShading ? 
            "(true)" : "(false)"),  "Saturate", pkShadeNode);

    pkDiffuseCoeff = 
        pkShadeNode->GetOutputResourceByVariableName("Diffuse");
    pkSpecularCoeff = 
        pkShadeNode->GetOutputResourceByVariableName("Specular");

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleApplyFog(Context& kContext, 
    NiStandardPixelProgramDescriptor* pkPixDesc,
    NiMaterialResource* pkUnfoggedColor, 
    NiMaterialResource*& pkFogOutput)
{
    NiFixedString kPlatform = kContext.m_spConfigurator->GetPlatformString();
    if (kPlatform == "DX9")
    {
        // DX9 uses HLSL with varying shader targets
        // Fog should only be applied by the pixel shader in SM 3.0 or greater
        NiGPUProgramCache* pkCache = 
            m_aspProgramCaches[NiGPUProgram::PROGRAM_PIXEL];
        NiFixedString kShaderTarget = pkCache->GetShaderProfile();

    
        if (kShaderTarget != "ps_3_0")
        {
            pkFogOutput = pkUnfoggedColor;
            return true;
        }
    }

    // At this time, all other platforms require this fog calculation.
    if (pkPixDesc->GetFOGENABLED() != 0)
    {
        NiMaterialNode* pkFogNode = 
            GetAttachableNodeFromLibrary("ApplyFog");
        kContext.m_spConfigurator->AddNode(pkFogNode);

        NiMaterialResource* pkFogResource = 
            kContext.m_spInputs->AddOutputResource("float", "Fog", "", 
            "FogDepth");

        kContext.m_spConfigurator->AddBinding(
            pkFogResource, 
            pkFogNode->GetInputResourceByVariableName("FogAmount"));

        kContext.m_spConfigurator->AddBinding(pkUnfoggedColor,
            pkFogNode->GetInputResourceByVariableName("UnfoggedColor"));

        NiMaterialResource* pkFogColor = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_FOG_COLOR); 
        kContext.m_spConfigurator->AddBinding(pkFogColor, 
            pkFogNode->GetInputResourceByVariableName("FogColor"));

        pkFogOutput = 
            pkFogNode->GetOutputResourceByVariableName("FoggedColor"); 
    }
    else
    {
        pkFogOutput = pkUnfoggedColor;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::HandleAlphaTest(Context& kContext,
    NiStandardPixelProgramDescriptor* pkPixDesc,
    NiMaterialResource* pkAlphaTestInput)
{
    NiFixedString kPlatform = kContext.m_spConfigurator->GetPlatformString();
    // Only D3D10 requires alpha testing in the pixel shader
    if (kPlatform != "D3D10")
    {
        return true;
    }

    if (pkPixDesc->GetALPHATEST() != 0)
    {
        NiMaterialNode* pkAlphaTestNode = 
            GetAttachableNodeFromLibrary("ApplyAlphaTest");
        kContext.m_spConfigurator->AddNode(pkAlphaTestNode);

        NiMaterialResource* pkTestFunction = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_FUNC);
        kContext.m_spConfigurator->AddBinding(pkTestFunction, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestFunction"));

        NiMaterialResource* pkTestRef = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_REF);
        kContext.m_spConfigurator->AddBinding(pkTestRef, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestRef"));

        kContext.m_spConfigurator->AddBinding(pkAlphaTestInput,
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestValue"));
    }

    return true;
}
//---------------------------------------------------------------------------
NiFixedString NiStandardMaterial::GenerateUVSetName(unsigned int uiIndex)
{
    char acVarName[32];
    NiSprintf(acVarName, 32, "UVSet%d", uiIndex);
    return acVarName;
}
//---------------------------------------------------------------------------
unsigned int NiStandardMaterial::FindUVSetIndexForTextureIndex(
    unsigned int uiTexture,  NiStandardPixelProgramDescriptor* pkPixDesc)
{
    unsigned int uiUVIdx = pkPixDesc->GetUVSetForMap(uiTexture);
    return uiUVIdx;
}
//---------------------------------------------------------------------------
NiMaterialResource* NiStandardMaterial::FindUVSetIndexForTextureEnum(
    TextureMap eMap, NiStandardPixelProgramDescriptor* pkPixDesc, 
    NiMaterialResource** apkUVSets)
{
    unsigned int uiUVIdx = FindUVSetIndexForTextureEnum(eMap, pkPixDesc);
    return apkUVSets[uiUVIdx];
}
//---------------------------------------------------------------------------
unsigned int NiStandardMaterial::FindUVSetIndexForTextureEnum(
    TextureMap eMap, NiStandardPixelProgramDescriptor* pkPixDesc)
{
    int iTexture = MapIndexFromTextureEnum(eMap, pkPixDesc);
    if (iTexture == -1)
        return UINT_MAX;
    unsigned int uiUVIdx = pkPixDesc->GetUVSetForMap((unsigned int)iTexture);
    return uiUVIdx;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::NormalizeVector(Context& kContext, 
    NiMaterialResource*& pkVector)
{
    NiMaterialNode* pkNode = NULL;
    if (pkVector && pkVector->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "NormalizeFloat4");
    }
    else if (pkVector && pkVector->GetType() == "float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "NormalizeFloat3");
    }
    else if (pkVector && pkVector->GetType() == "float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "NormalizeFloat2");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector, 
        pkNode->GetInputResourceByVariableName("VectorIn"));

    pkVector =  pkNode->GetOutputResourceByVariableName("VectorOut");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::AddVector(Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    
    if (pkVector1 && pkVector1->GetType() == "float4" && pkVector2 && 
        pkVector2->GetType() =="float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat4");
    }
    else if (pkVector1 && pkVector1->GetType() == "float3" && pkVector2 && 
        pkVector2->GetType() =="float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat3");
    }
    else if (pkVector1 && pkVector1->GetType() == "float2" && pkVector2 && 
        pkVector2->GetType() =="float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat2");
    }
    else if (pkVector1 && pkVector1->GetType() == "float" && pkVector2 && 
        pkVector2->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat");
    }
    else if (pkVector1)
    {
        pkValue = pkVector1;
        return true;
    }
    else if (pkVector2)
    {
        pkValue = pkVector2;
        return true;
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));

    pkValue =  pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::MultiplyVector(Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkVector1 && pkVector1->GetType() == "float4" && pkVector2 && 
        pkVector2->GetType() =="float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat4");
    }
    else if (pkVector1 && pkVector1->GetType() == "float3" && pkVector2 && 
        pkVector2->GetType() =="float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat3");
    }
    else if (pkVector1 && pkVector1->GetType() == "float2" && pkVector2 && 
        pkVector2->GetType() =="float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat2");
    }
    else if (pkVector1 && pkVector1->GetType() == "float" && pkVector2 && 
        pkVector2->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat");
    }
    else if (pkVector1)
    {
        pkValue = pkVector1;
        return true;
    }
    else if (pkVector2)
    {
        pkValue = pkVector2;
        return true;
    }

    
    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::MultiplyAddVector(Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource* pkVector3, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkVector1 && pkVector2 && pkVector3)
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyAddFloat3");
    }

    
    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    TypeCastBind(kContext, pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    TypeCastBind(kContext, pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));
    TypeCastBind(kContext, pkVector3, 
        pkNode->GetInputResourceByVariableName("V3"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::LerpVector(Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource* pkLerpAmount, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkVector1 && pkVector1->GetType() == "float4" && pkVector2 && 
        pkVector2->GetType() =="float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat4");
    }
    else if (pkVector1 && pkVector1->GetType() == "float3" && pkVector2 && 
        pkVector2->GetType() =="float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat3");
    }
    else if (pkVector1 && pkVector1->GetType() == "float2" && pkVector2 && 
        pkVector2->GetType() =="float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat2");
    }
    else if (pkVector1 && pkVector1->GetType() == "float" && pkVector2 && 
        pkVector2->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));
    kContext.m_spConfigurator->AddBinding(pkLerpAmount, 
        pkNode->GetInputResourceByVariableName("LerpAmount"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::ScaleVector(Context& kContext, 
    NiMaterialResource* pkVector, NiMaterialResource* pkScale,
    NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (!pkScale && pkVector)
    {
        pkValue = pkVector;
        return true;
    }
    else if (pkVector && pkVector->GetType() == "float4" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "ScaleFloat4");
    }
    else if (pkVector && pkVector->GetType() == "float3" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "ScaleFloat3");
    }
    else if (pkVector && pkVector->GetType() == "float2" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "ScaleFloat2");
    }
    else if (pkVector && pkVector->GetType() == "float" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkScale, 
        pkNode->GetInputResourceByVariableName("Scale"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SaturateVector(Context& kContext, 
    NiMaterialResource* pkVector, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;

    if (pkVector && pkVector->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat4");
    }
    else if (pkVector && pkVector->GetType() == "float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat3");
    }
    else if (pkVector && pkVector->GetType() == "float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat2");
    }
    else if (pkVector && pkVector->GetType() == "float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector, 
        pkNode->GetInputResourceByVariableName("V1"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::MultiplyScalarSatAddVector(Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource* pkScale, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if ((pkVector1 && pkVector1->GetType() == "float3") || (pkVector2 && 
        pkVector2->GetType() =="float3"))
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyScalarSatAddFloat3");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    if (pkVector1)
    {
        kContext.m_spConfigurator->AddBinding(pkVector1, 
            pkNode->GetInputResourceByVariableName("V1"));
    }
    
    if (pkVector2)
    {
        kContext.m_spConfigurator->AddBinding(pkVector2, 
            pkNode->GetInputResourceByVariableName("V2"));
    }
    
    if (pkScale)
    {
        kContext.m_spConfigurator->AddBinding(pkScale, 
            pkNode->GetInputResourceByVariableName("Scalar"));
    }

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SplitColorAndOpacity(Context& kContext, 
    NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkColor,
    NiMaterialResource*& pkOpacity)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkColorAndOpacity && pkColorAndOpacity->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SplitColorAndOpacity");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkColorAndOpacity, 
        pkNode->GetInputResourceByVariableName("ColorAndOpacity"));

    pkColor = pkNode->GetOutputResourceByVariableName("Color");
    pkOpacity = pkNode->GetOutputResourceByVariableName("Opacity");
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::SplitRGBA(Context& kContext, 
    NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkRed,
    NiMaterialResource*& pkGreen, NiMaterialResource*& pkBlue,
    NiMaterialResource*& pkAlpha)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkColorAndOpacity && pkColorAndOpacity->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SplitRGBA");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkColorAndOpacity, 
        pkNode->GetInputResourceByVariableName("ColorAndOpacity"));

    pkRed = pkNode->GetOutputResourceByVariableName("Red");
    pkGreen = pkNode->GetOutputResourceByVariableName("Green");
    pkBlue = pkNode->GetOutputResourceByVariableName("Blue");
    pkAlpha = pkNode->GetOutputResourceByVariableName("Alpha");

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterial::TypeCastBind(Context& kContext, 
    NiMaterialResource* pkSrc, NiMaterialResource* pkDest)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkSrc  && pkSrc->GetType()  == "float" && 
        pkDest && pkDest->GetType() == "float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "FloatToFloat3");
    }
    else if (pkSrc  && pkSrc->GetType()  == "float" && 
        pkDest && pkDest->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "FloatToFloat4");
    }
    else if (pkSrc  && pkSrc->GetType()  == "float4" && 
        pkDest && pkDest->GetType() == "float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "Float4ToFloat");
    }
    else if (pkSrc  && pkSrc->GetType()  == "float3" && 
        pkDest && pkDest->GetType() == "float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "Float3ToFloat");
    }
    else if (pkSrc && pkDest && kContext.m_spConfigurator->CanBindTypes(
        pkSrc->GetType(), pkDest->GetType()))
    {
        return kContext.m_spConfigurator->AddBinding(pkSrc, pkDest);
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkSrc, 
        pkNode->GetInputResourceByVariableName("Input"));
    kContext.m_spConfigurator->AddBinding(
        pkNode->GetOutputResourceByVariableName("Output"), pkDest);
    return true;
}
//---------------------------------------------------------------------------
void NiStandardMaterial::AddDefaultFallbacks()
{
    AddShaderFallbackFunc(SplitPerPixelLights);
    AddShaderFallbackFunc(DropShadowMaps);
    AddShaderFallbackFunc(DropShadowMapsThenSplitPerPixelLights);
    AddShaderFallbackFunc(SplitPerVertexLights);
    AddShaderFallbackFunc(SplitTextureMaps);
    AddShaderFallbackFunc(DropShadowMapsThenSplitTextureMaps);
    AddShaderFallbackFunc(DropParallaxMap);
    AddShaderFallbackFunc(DropParallaxMapThenSplitLights);
    AddShaderFallbackFunc(DropShadowMapsThenDropParallaxMapThenSplitLights);

    NiFragmentMaterial::AddDefaultFallbacks();
}
//---------------------------------------------------------------------------
