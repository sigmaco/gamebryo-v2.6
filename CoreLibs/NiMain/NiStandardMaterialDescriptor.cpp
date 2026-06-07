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

#include "NiStandardMaterialDescriptor.h"
#include "NiShadowManager.h"
#include "NiShadowGenerator.h"
#include "NiShadowTechnique.h"
#include "NiAmbientLight.h"
#include "NiDirectionalLight.h"
#include "NiPointLight.h"
#include "NiSpotLight.h"
#include "NiCommonSemantics.h"
#include "NiRenderObject.h"
#include "NiRenderObjectMaterialOption.h"
#include "NiVertexColorProperty.h"
#include "NiSpecularProperty.h"
#include "NiFogProperty.h"
#include "NiTextureEffect.h"

//---------------------------------------------------------------------------
NiStandardMaterialDescriptor::NiStandardMaterialDescriptor()
{
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::AddLight(NiLight* pkLight, 
    unsigned int uiWhichLight, const NiRenderObject* pkGeometry)
{
    if (uiWhichLight < NiStandardMaterial::STANDARD_PIPE_MAX_LIGHTS)
    {
        bool bFound = false;
        if (NiIsKindOf(NiSpotLight, pkLight))
        {
            unsigned int uiSpotLights = GetSPOTLIGHTCOUNT() + 1;
            SetSPOTLIGHTCOUNT(uiSpotLights);
            bFound = true;
        }
        else if (NiIsKindOf(NiPointLight, pkLight))
        {
            unsigned int uiPointLights = GetPOINTLIGHTCOUNT() + 1;
            SetPOINTLIGHTCOUNT(uiPointLights);
            bFound = true;
        }
        else if (NiIsKindOf(NiDirectionalLight, pkLight))
        {
            unsigned int uiDirLights = GetDIRLIGHTCOUNT() + 1;
            SetDIRLIGHTCOUNT(uiDirLights);
            bFound = true;
        }

        NiShadowGenerator* pkShadowGen = pkLight->GetShadowGenerator();
        if (bFound && pkShadowGen && NiShadowManager::GetShadowManager() &&
            NiShadowManager::GetActive())
        {
            if (!pkShadowGen->GetActive())
                return true;

            if (pkShadowGen->IsUnaffectedReceiverNode(pkGeometry))
                return true;

            bool bFoundShadowMap = false;
            const unsigned int uiShadowMapCount =
                pkShadowGen->GetShadowMapCount();
            for (unsigned int uiShadowMap = 0; uiShadowMap < uiShadowMapCount;
                uiShadowMap++)
            {
                if (pkShadowGen->GetOwnedShadowMap(uiShadowMap))
                {
                    bFoundShadowMap = true;
                    break;
                }
            }
            if (!bFoundShadowMap)
            {
                return true;
            }

            NiShadowTechnique* pkTechnique = pkShadowGen->GetShadowTechnique();

            if (pkTechnique)
            {
                unsigned short usActiveSlot = 
                    pkTechnique->GetActiveTechniqueSlot();

                // If this assert is hit it means the light is assigned a 
                // NiShadowTechnique that is not one of the active 
                // NiShadowTechniques.
                NIASSERT (usActiveSlot < 
                    NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );
                            
                SetLightInfo(uiWhichLight, true);

                // Only use the assigned shadow technique from the light if the
                // NiShadowTechnique has a higher priority than the one already
                // applied. Note: NiShadowTechnique priority is defined by the
                // slot the NiShadowTechnique is assigned to. 
                if ( usActiveSlot > GetSHADOWTECHNIQUE())
                    SetSHADOWTECHNIQUE(usActiveSlot);
            }
        }
        
        return bFound;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::GetLightType(unsigned int uiWhichLight, 
    NiStandardMaterial::LightType& eLightType)
{
    unsigned int uiPointLights = GetPOINTLIGHTCOUNT();
    unsigned int uiDirLights = GetDIRLIGHTCOUNT();
    unsigned int uiSpotLights = GetSPOTLIGHTCOUNT();

    if (uiWhichLight > uiPointLights + uiDirLights + uiSpotLights)
    {
        return false;
    }
    else if (uiWhichLight > uiPointLights + uiDirLights)
    {
        eLightType = NiStandardMaterial::LIGHT_SPOT;
        return true;
    }
    else if (uiWhichLight > uiPointLights)
    {
        eLightType = NiStandardMaterial::LIGHT_DIR;
        return true;
    }
    else
    {
        eLightType = NiStandardMaterial::LIGHT_POINT;
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::GetLightInfo(unsigned int uiWhichLight, 
    bool& bShadowed)
{
    unsigned int uiPointLights = GetPOINTLIGHTCOUNT();
    unsigned int uiDirLights = GetDIRLIGHTCOUNT();
    unsigned int uiSpotLights = GetSPOTLIGHTCOUNT();

    if (uiWhichLight > uiPointLights + uiDirLights + uiSpotLights)
    {
       return false;
    }

    unsigned int uiShadowBits = GetSHADOWMAPFORLIGHT();
    bShadowed = NiTGetBit< unsigned int >(uiShadowBits, 1 << uiWhichLight);

    return true;
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetLightInfo(unsigned int uiWhichLight, 
    bool bShadowed)
{
    unsigned int uiShadowBits = GetSHADOWMAPFORLIGHT();
    NiTSetBit< unsigned int >(uiShadowBits, bShadowed, 
        1 << uiWhichLight);

    SetSHADOWMAPFORLIGHT(uiShadowBits);

}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetTextureUsage(
    unsigned int uiWhichTexture, unsigned int uiInputUV,
    NiStandardMaterial::TexGenOutput eTextureOutput)
{
    switch (uiWhichTexture)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        SetMAP00(uiInputUV); 
        SetMAP00TEXOUTPUT(eTextureOutput);
        break;
    case 1:
        SetMAP01(uiInputUV);
        SetMAP01TEXOUTPUT(eTextureOutput);
        break;
    case 2:
        SetMAP02(uiInputUV); 
        SetMAP02TEXOUTPUT(eTextureOutput);
        break;
    case 3:
        SetMAP03(uiInputUV); 
        SetMAP03TEXOUTPUT(eTextureOutput);
        break;
    case 4:
        SetMAP04(uiInputUV); 
        SetMAP04TEXOUTPUT(eTextureOutput);
        break;
    case 5:
        SetMAP05(uiInputUV); 
        SetMAP05TEXOUTPUT(eTextureOutput);
        break;
    case 6:
        SetMAP06(uiInputUV); 
        SetMAP06TEXOUTPUT(eTextureOutput);
        break;
    case 7:
        SetMAP07(uiInputUV); 
        SetMAP07TEXOUTPUT(eTextureOutput);
        break;
    case 8:
        SetMAP08(uiInputUV); 
        SetMAP08TEXOUTPUT(eTextureOutput);
        break;
    case 9:
        SetMAP09(uiInputUV); 
        SetMAP09TEXOUTPUT(eTextureOutput);
        break;
    case 10:
        SetMAP10(uiInputUV); 
        SetMAP10TEXOUTPUT(eTextureOutput);
        break;
    case 11:
        SetMAP11(uiInputUV); 
        SetMAP11TEXOUTPUT(eTextureOutput);
        break;
    }
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::GetTextureUsage(
    unsigned int uiWhichTexture, unsigned int& uiInputUV,
    NiStandardMaterial::TexGenOutput& eTextureOutput)
{
    switch (uiWhichTexture)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        uiInputUV = GetMAP00();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP00TEXOUTPUT();
        break;
    case 1:
        uiInputUV = GetMAP01();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP01TEXOUTPUT();
        break;
    case 2:
        uiInputUV = GetMAP02();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP02TEXOUTPUT();
        break;
    case 3:
        uiInputUV = GetMAP03();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP03TEXOUTPUT();
        break;
    case 4:
        uiInputUV = GetMAP04();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP04TEXOUTPUT();
        break;
    case 5:
        uiInputUV = GetMAP05();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP05TEXOUTPUT();
        break;
    case 6:
        uiInputUV = GetMAP06();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP06TEXOUTPUT();
        break;
    case 7:
        uiInputUV = GetMAP07();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP07TEXOUTPUT();
        break;
    case 8:
        uiInputUV = GetMAP08();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP08TEXOUTPUT();
        break;
    case 9:
        uiInputUV = GetMAP09();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP09TEXOUTPUT();
        break;
    case 10:
        uiInputUV = GetMAP10();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP10TEXOUTPUT();
        break;
    case 11:
        uiInputUV = GetMAP11();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP11TEXOUTPUT();
        break;
    }
}
//---------------------------------------------------------------------------
unsigned int NiStandardMaterialDescriptor::GetStandardTextureCount()
{
    unsigned int uiCount = 
        GetPARALLAXMAPCOUNT() + 
        GetBASEMAPCOUNT() +        
        GetNORMALMAPCOUNT() +      
        GetDARKMAPCOUNT() +        
        GetDETAILMAPCOUNT() +      
        GetBUMPMAPCOUNT() +        
        GetGLOSSMAPCOUNT()+       
        GetGLOWMAPCOUNT() +
        GetCUSTOMMAP00COUNT() + 
        GetCUSTOMMAP01COUNT() + 
        GetCUSTOMMAP02COUNT() + 
        GetCUSTOMMAP03COUNT() + 
        GetCUSTOMMAP04COUNT() + 
        GetDECALMAPCOUNT();
    return uiCount;
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetProjectedLight(unsigned int uiWhichLight,
    bool bClipped, NiStandardMaterial::TexEffectType eType)
{
    NIASSERT(eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL ||
        eType == NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
    NIASSERT(uiWhichLight < (1<<PROJLIGHTMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJLIGHTMAPCLIPPED();
    NiTSetBit<unsigned int>(uiField, bClipped, 1 << uiWhichLight);
    SetPROJLIGHTMAPCLIPPED(uiField);

    bool bPersp = true;
    if (eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL)
        bPersp = false;

    uiField = GetPROJLIGHTMAPTYPES();
    NiTSetBit<unsigned int>(uiField, bPersp, 1 << uiWhichLight);
    SetPROJLIGHTMAPTYPES(uiField);

}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::GetProjectedLight(unsigned int uiWhichLight,
    bool& bClipped, NiStandardMaterial::TexEffectType& eType)
{
    NIASSERT(uiWhichLight < (1<<PROJLIGHTMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJLIGHTMAPCLIPPED();
    bClipped = NiTGetBit<unsigned int>(uiField, 1 << uiWhichLight);

    bool bPersp = true;   
    uiField = GetPROJLIGHTMAPTYPES();
    bPersp = NiTGetBit<unsigned int>(uiField, 1 << uiWhichLight);

    if (!bPersp)
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL;
    else
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE;
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetProjectedShadow(
    unsigned int uiWhichShadow, bool bClipped,
    NiStandardMaterial::TexEffectType eType)
{
    NIASSERT(eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL ||
        eType == NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
    NIASSERT(uiWhichShadow < (1<<PROJSHADOWMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJSHADOWMAPCLIPPED();
    NiTSetBit<unsigned int>(uiField, bClipped, 1 << uiWhichShadow);
    SetPROJSHADOWMAPCLIPPED(uiField);

    bool bPersp = true;
    if (eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL)
        bPersp = false;

    uiField = GetPROJSHADOWMAPTYPES();
    NiTSetBit<unsigned int>(uiField, bPersp, 1 << uiWhichShadow);
    SetPROJSHADOWMAPTYPES(uiField);

}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::GetProjectedShadow(
    unsigned int uiWhichShadow, bool& bClipped, 
    NiStandardMaterial::TexEffectType& eType)
{
    NIASSERT(uiWhichShadow < (1<<PROJSHADOWMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJSHADOWMAPCLIPPED();
    bClipped = NiTGetBit<unsigned int>(uiField, 1 << uiWhichShadow);

    bool bPersp = true;   
    uiField = GetPROJSHADOWMAPTYPES();
    bPersp = NiTGetBit<unsigned int>(uiField, 1 << uiWhichShadow);

    if (!bPersp)
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL;
    else
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE;
}
//---------------------------------------------------------------------------
NiString NiStandardMaterialDescriptor::ToString()
{
    NiString kString;
    // The enumerations that are used here are the last entry of each
    // of the 32-bit words that comprise the 128-bit bitfield.  This
    // function will thus print out the entire bitfield.
    ToStringCUSTOMMAP03COUNT(kString, true);
    ToStringPROJLIGHTMAPCLIPPED(kString, true);
    ToStringMAP05TEXOUTPUT(kString, true);
    ToStringALPHATEST(kString, true);
    return kString;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetTransformDescriptor(
    const NiRenderObject* pkMesh, bool& bHardwareSkinned)
{
    if (pkMesh->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_SKINNED()))
    {
        SetTRANSFORM(NiStandardMaterial::TRANSFORM_SKINNED);
        bHardwareSkinned = true;
    }
    else if (pkMesh->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_INSTANCED()))
    {
        SetTRANSFORM(NiStandardMaterial::TRANSFORM_INSTANCED);
    }
    else
    {
        SetTRANSFORM(NiStandardMaterial::TRANSFORM_DEFAULT);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetNBTDescriptor(
    const NiRenderObject* pkMesh, bool bHardwareSkinned, bool& bNormals,
    bool& bNBTs)
{
    NiFixedString kNormal;
    NiFixedString kBinormal;
    NiFixedString kTangent;
    if (bHardwareSkinned)
    {
        kNormal = NiCommonSemantics::NORMAL_BP();
        kBinormal = NiCommonSemantics::BINORMAL_BP();
        kTangent = NiCommonSemantics::TANGENT_BP();
    }
    else
    {
        kNormal = NiCommonSemantics::NORMAL();
        kBinormal = NiCommonSemantics::BINORMAL();
        kTangent = NiCommonSemantics::TANGENT();
    }

    bNormals = pkMesh->ContainsData(kNormal);

    bool bTangents = false;
    bool bBinormals = false;
    if (bNormals)
    {
        for (unsigned int uiSemanticIndex = 0; 
            uiSemanticIndex < 8 && (!bTangents || !bBinormals); 
            uiSemanticIndex++)
        {
            if (pkMesh->ContainsData(kBinormal, uiSemanticIndex))
                bBinormals = true;

            if (pkMesh->ContainsData(kTangent, uiSemanticIndex))
                bTangents = true;
        }
    }

    bNBTs = bNormals && bBinormals && bTangents;

    unsigned int uiNormalSlots = 0;
    if (bNBTs)
    {
        SetNORMAL(NiStandardMaterial::NORMAL_NBT);
        uiNormalSlots = 3;
    }
    else if (bNormals)
    {
        SetNORMAL(NiStandardMaterial::NORMAL_ONLY);
        uiNormalSlots = 1;
    }
    else
    {
        SetNORMAL(NiStandardMaterial::NORMAL_NONE);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetVertexColorDescriptor(
    const NiRenderObject* pkMesh, bool& bVertexColors)
{
    bVertexColors = pkMesh->ContainsData(NiCommonSemantics::COLOR());
    SetVERTEXCOLORS(bVertexColors ? 1 : 0);
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetVertexColorPropertyDescriptor(
    const NiRenderObject*, const NiPropertyState* pkPropState,
    bool bVertexColors)
{
    NiVertexColorProperty* pkVCProp = pkPropState->GetVertexColor();
    if (pkVCProp && bVertexColors)
    {
        if (pkVCProp->GetSourceMode() == 
            NiVertexColorProperty::SOURCE_AMB_DIFF)
        {
            SetAMBDIFFEMISSIVE(NiStandardMaterial::ADE_AMB_DIFF);
        }
        else if (pkVCProp->GetSourceMode() == 
            NiVertexColorProperty::SOURCE_EMISSIVE)
        {
            SetAMBDIFFEMISSIVE(NiStandardMaterial::ADE_EMISSIVE);
        }
        else if (pkVCProp->GetSourceMode() == 
            NiVertexColorProperty::SOURCE_IGNORE)
        {
            SetAMBDIFFEMISSIVE(NiStandardMaterial::ADE_IGNORE);
        }
        else
        {
            NIASSERT(!"Should never get here!");
        }

    }
    else // Always src_ignore if no vertex colors
    {
        SetAMBDIFFEMISSIVE(NiStandardMaterial::ADE_IGNORE);
    }

    if (pkVCProp)
    {
        SetLIGHTINGMODE(pkVCProp->GetLightingMode());
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetSpecularPropertyDescriptor(
    const NiRenderObject*, const NiPropertyState* pkPropState,
    bool& bSpecularEnabled)
{
    NiSpecularProperty* pkSpecProp = pkPropState->GetSpecular();
    
    bSpecularEnabled = false;
    if (pkSpecProp)
    {
        bSpecularEnabled = pkSpecProp->GetSpecular();
        if (bSpecularEnabled)
            SetSPECULAR(true);
        else 
            SetSPECULAR(false);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetMaterialPropertyDescriptor(
    const NiRenderObject*, const NiPropertyState*)
{
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetFogPropertyDescriptor(
    const NiRenderObject*, const NiPropertyState* pkPropState)
{
    NiFogProperty* pkFogProp = pkPropState->GetFog();

    if (pkFogProp)
    {
        if (pkFogProp->GetFog())
        {
            switch(pkFogProp->GetFogFunction())
            {
            case NiFogProperty::FOG_Z_LINEAR:
                SetFOGTYPE(NiStandardMaterial::FOG_LINEAR);
                break;
            case NiFogProperty::FOG_RANGE_SQ:
                SetFOGTYPE(NiStandardMaterial::FOG_SQUARED);
                break;
            default:
                SetFOGTYPE(NiStandardMaterial::FOG_NONE);
                break;
            }
        }
        else
        {
            SetFOGTYPE(NiStandardMaterial::FOG_NONE);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetAlphaPropertyDescriptor(
    const NiRenderObject*, const NiPropertyState* pkPropState)
{
    NiAlphaProperty* pkAlphaProp = pkPropState->GetAlpha();

    if (pkAlphaProp)
    {
        if (pkAlphaProp->GetAlphaTesting() &&
            pkAlphaProp->GetTestMode() != NiAlphaProperty::TEST_ALWAYS)
        {
            SetALPHATEST(true);
        }
        else
        {
            SetALPHATEST(false);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetBaseMapDescriptor(
    const NiTexturingProperty* pkTexProp,
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount)
{
    if (pkTexProp->ValidMapExists(NiTexturingProperty::BASE_INDEX))
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetBaseMap();
        SetBASEMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_BASE] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetNormalMapDescriptor(
    const NiTexturingProperty* pkTexProp,
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount)
{
    if (pkTexProp->ValidMapExists(NiTexturingProperty::NORMAL_INDEX) && 
        GetNORMAL() == NiStandardMaterial::NORMAL_NBT)
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetNormalMap();
        SetNORMALMAPCOUNT(1);
        SetNORMALMAPTYPE(NiStandardMaterial::NORMAL_MAP_STANDARD);
        ppkActiveMaps[NiStandardMaterial::MAP_NORMAL] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetParallaxMapDescriptor(
    const NiTexturingProperty* pkTexProp, 
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount)
{
    if (pkTexProp->ValidMapExists(
        NiTexturingProperty::PARALLAX_INDEX) && 
        GetNORMAL() == NiStandardMaterial::NORMAL_NBT)
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetParallaxMap();
        SetPARALLAXMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_PARALLAX] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetDarkMapDescriptor(
    const NiTexturingProperty* pkTexProp, 
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount)
{
    
    if (pkTexProp->ValidMapExists(
        NiTexturingProperty::DARK_INDEX))
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetDarkMap();
        SetDARKMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_DARK] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetDetailMapDescriptor(
    const NiTexturingProperty* pkTexProp, 
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount)
{        
    if (pkTexProp->ValidMapExists(
        NiTexturingProperty::DETAIL_INDEX))
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetDetailMap();
        SetDETAILMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_DETAIL] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetBumpMapDescriptor(
    const NiTexturingProperty* pkTexProp, 
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount, bool bEnvMap)
{
    if (pkTexProp->ValidMapExists(
        NiTexturingProperty::BUMP_INDEX) && bEnvMap)
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetBumpMap();
        SetBUMPMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_BUMP] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetGlossMapDescriptor(
    const NiTexturingProperty* pkTexProp, 
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount, bool bEnvMap, bool bSpecularEnabled)
{
    if (pkTexProp->ValidMapExists(
        NiTexturingProperty::GLOSS_INDEX) && 
        (bSpecularEnabled || bEnvMap))
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetGlossMap();
        SetGLOSSMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_GLOSS] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetGlowMapDescriptor(
    const NiTexturingProperty* pkTexProp, 
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount)
{
    if (pkTexProp->ValidMapExists(NiTexturingProperty::GLOW_INDEX))
    {
        const NiTexturingProperty::Map* pkMap = pkTexProp->GetGlowMap();
        SetGLOWMAPCOUNT(1);
        ppkActiveMaps[NiStandardMaterial::MAP_GLOW] = pkMap;
        uiCurrentMapCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetDecalMapsDescriptor(
    const NiTexturingProperty* pkTexProp,
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount, NiUInt32 uiDecalMapMax)
{
    // Get total number of decal maps, ignoring invalid ones, and
    // honor decal map limitations.
    unsigned int uiDecalCount = 
        NiMin((int)pkTexProp->GetDecalMapCount(), (int)uiDecalMapMax);

    for (unsigned int ui = 0; ui < uiDecalCount; ui++)
    {
        if (!pkTexProp->ValidMapExists(
            NiTexturingProperty::DECAL_BASE, ui))
        {
            uiDecalCount--;
            continue;
        }

        const NiTexturingProperty::Map* pkMap = pkTexProp->GetDecalMap(ui);
        if (GetDECALMAPCOUNT() == 
            uiDecalMapMax)
        {
            uiDecalCount = uiDecalMapMax;
            break;
        }
        else if (pkMap)
        {
            ppkActiveMaps[NiStandardMaterial::MAP_DECAL00 + ui] = pkMap;
            uiCurrentMapCount++;
        }
    }
    SetDECALMAPCOUNT(uiDecalCount);
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetShaderMapsDescriptor(
    const NiTexturingProperty* pkTexProp,
    const NiTexturingProperty::Map** ppkActiveMaps, 
    NiUInt32& uiCurrentMapCount, NiUInt32 uiShaderMapMax)
{
    unsigned int uiShaderCount = pkTexProp->GetShaderArrayCount();
    for (unsigned int ui = 0; ui < uiShaderCount && 
        ui < uiShaderMapMax; ui++)
    {
        if (!pkTexProp->ValidMapExists(
            NiTexturingProperty::SHADER_BASE, ui))
        {
            continue;
        }

        const NiTexturingProperty::Map* pkMap = 
            pkTexProp->GetShaderMap(ui);
        if (pkMap)
        {
            
            switch (ui)
            {
            case 0:
                SetCUSTOMMAP00COUNT(1);
                break;
            case 1:
                SetCUSTOMMAP01COUNT(1);
                break;
            case 2:
                SetCUSTOMMAP02COUNT(1);
                break;
            case 3:
                SetCUSTOMMAP03COUNT(1);
                break;
            case 4:
                SetCUSTOMMAP04COUNT(1);
                break;
            }

            ppkActiveMaps[NiStandardMaterial::MAP_CUSTOM00 + ui] = pkMap;
            uiCurrentMapCount++;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetTexuringPropertyDescriptor(
    const NiRenderObject* pkMesh, const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState,
    const NiTexturingProperty::Map** ppkActiveMaps,
    NiUInt32 uiMapMax, NiUInt32 uiDecalMapMax, NiUInt32 uiShaderMapMax,
    bool bSpecularEnabled)
{

    NiTexturingProperty* pkTexProp = pkPropState->GetTexturing();

    // Texturing Property
    if (pkTexProp)
    {
        // Handle adding textures to the shader description.
        // Note that the order in which these textures are handled MUST
        // match the order in which they were declared in the descriptor.
        // This will guarantee that the UVSets for Map## will correspond 
        // to the correct semantic texture map later on.

        // Count number of texture sets
        NiUInt32 uiNumInputTextureSets = pkMesh->GetSemanticCount(
            NiCommonSemantics::TEXCOORD());

        switch (pkTexProp->GetApplyMode())
        {
            case NiTexturingProperty::APPLY_REPLACE:
                SetAPPLYMODE(NiStandardMaterial::APPLY_REPLACE);
                break;
            default:
            case NiTexturingProperty::APPLY_MODULATE:
                SetAPPLYMODE(NiStandardMaterial::APPLY_MODULATE);
                break;
        }

        unsigned int uiCurrentMapCount = 0;

        // It is customary in Gamebryo to skip all textures if there
        // are no texture coordinates in the geometry.
        if (uiNumInputTextureSets != 0)
        {
            SetParallaxMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount);
            SetBaseMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount);
            SetNormalMapDescriptor(pkTexProp, ppkActiveMaps, 
                uiCurrentMapCount);
            SetDarkMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount);
            SetDetailMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount);

            bool bEnvMap = false;
            if (pkEffectState && pkEffectState->GetEnvironmentMap())
                bEnvMap = true;

            SetBumpMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount, bEnvMap);
            SetGlossMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount, bSpecularEnabled, bEnvMap);
            SetGlowMapDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount);
            SetDecalMapsDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount, uiDecalMapMax);
            SetShaderMapsDescriptor(pkTexProp, ppkActiveMaps,
                uiCurrentMapCount, uiShaderMapMax);

            // Set up and organize the UV coordinates
            SetTextureMapUVsDescriptor(pkMesh, ppkActiveMaps,
                uiMapMax, uiNumInputTextureSets, uiCurrentMapCount);
        }


    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetTextureMapUVsDescriptor(
    const NiRenderObject*,
    const NiTexturingProperty::Map** ppkActiveMaps,
    NiUInt32 uiMapMax, NiUInt32 uiNumInputTextureSets,
    NiUInt32 uiMaxUsedMap)
{
    NI_UNUSED_ARG(uiMaxUsedMap);
    // First find out how many UV sets are used as-is
    NiUInt32* puiSrcUVSetUsedDirectly = NiStackAlloc(NiUInt32, uiMapMax);
    memset(puiSrcUVSetUsedDirectly, 0, 
        uiMapMax*sizeof(unsigned int));  

    unsigned int uiTexTransforms = 0;

    // Also find out how many UV sets are used, whether as-is or
    // transformed
    bool* pbSrcUVSetUsed = NiStackAlloc(bool, uiMapMax);
    memset(pbSrcUVSetUsed, 0, uiMapMax * sizeof(bool));  
    unsigned int uiTexCoordsUsed = 0;

    unsigned int uiWhichMap = 0;

    for (unsigned int ui = 0; ui < uiMapMax; ui++)
    {
        const NiTexturingProperty::Map* pkMap = ppkActiveMaps[ui];
        if (!pkMap || uiNumInputTextureSets == 0)
            continue;

        NiStandardMaterial::TexGenOutput eTexOutput = 
            NiStandardMaterial::TEX_OUT_PASSTHRU;
        unsigned int uiSrcIndex = pkMap->GetTextureIndex();

        if (pkMap->GetTextureTransform() != NULL)
        {
            eTexOutput = NiStandardMaterial::TEX_OUT_TRANSFORM;
            uiTexTransforms++;
        }
        else
        {
            puiSrcUVSetUsedDirectly[uiSrcIndex]++;
        }

        if (!pbSrcUVSetUsed[uiSrcIndex])
        {
            uiTexCoordsUsed++;
            pbSrcUVSetUsed[uiSrcIndex] = true;
        }

        NIASSERT(uiSrcIndex < uiMapMax);

        SetTextureUsage(uiWhichMap, uiSrcIndex, eTexOutput);
        uiWhichMap++;
    }
    NIASSERT(uiWhichMap == uiMaxUsedMap);

    SetINPUTUVCOUNT(uiTexCoordsUsed);

    NiStackFree(puiSrcUVSetUsedDirectly);
    NiStackFree(pbSrcUVSetUsed);
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetLightsDescriptor(
    const NiRenderObject* pkMesh, 
    const NiDynamicEffectState* pkEffectState)
{
    if (GetAPPLYMODE() == NiStandardMaterial::APPLY_MODULATE)
    {
        unsigned int uiLightCount = 0;

        // Add lights in the order Point, Directional, Spot. This is 
        // required because all of NiStandardMaterial assumes lights have 
        // been added in this order.

        // Add Point Lights
        NiDynEffectStateIter kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == 
                NiDynamicEffect::POINT_LIGHT ||
                pkLight->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT))
            {
                AddLight(pkLight, uiLightCount, pkMesh);
                uiLightCount++;
            }
        }

        // Add Directional Lights
        kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkLight->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT))
            {
                AddLight(pkLight, uiLightCount, pkMesh);
                uiLightCount++;
            }
        }

        // Add Spot Lights
        kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::SPOT_LIGHT ||
                pkLight->GetEffectType() == 
                NiDynamicEffect::SHADOWSPOT_LIGHT))
            {
                AddLight(pkLight, uiLightCount, pkMesh);
                uiLightCount++;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetEnvMapDescriptor(
    const NiRenderObject*, 
    const NiDynamicEffectState* pkEffectState)
{
    SetENVMAPTYPE(NiStandardMaterial::TEXEFFECT_NONE);

    NiTextureEffect* pkEnvMap = pkEffectState->GetEnvironmentMap();
    if (pkEnvMap && pkEnvMap->GetSwitch() == true)
    {
        NiTextureEffect::TextureType eTexType = 
            pkEnvMap->GetTextureType();
        NiTextureEffect::CoordGenType eCoordType = 
            pkEnvMap->GetTextureCoordGen();

        if (eTexType == NiTextureEffect::ENVIRONMENT_MAP)
        {
            switch(eCoordType)
            {
                case NiTextureEffect::WORLD_PARALLEL:
                    SetENVMAPTYPE(
                        NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL);
                    break;
                case NiTextureEffect::WORLD_PERSPECTIVE:
                    SetENVMAPTYPE(
                        NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
                    break;
                case NiTextureEffect::SPHERE_MAP:
                    SetENVMAPTYPE( 
                        NiStandardMaterial::TEXEFFECT_SPHERICAL);
                    break;
                case NiTextureEffect::SPECULAR_CUBE_MAP:
                    SetENVMAPTYPE( 
                        NiStandardMaterial::TEXEFFECT_SPECULAR_CUBE);
                    break;
                case NiTextureEffect::DIFFUSE_CUBE_MAP:
                    SetENVMAPTYPE(
                        NiStandardMaterial::TEXEFFECT_DIFFUSE_CUBE);
                    break;
                default:
                    NIASSERT(!"Unknown NiTextureEffect::CoordGenType");
                    break;
            }
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetProjLightMapDescriptor(
    const NiRenderObject*, 
    const NiDynamicEffectState* pkEffectState)
{
    NiDynEffectStateIter kProjLightIter = 
        pkEffectState->GetProjLightHeadPos();
    unsigned int uiProjLightCount = 0;
    while (kProjLightIter != NULL && uiProjLightCount < 3 )
    {
        NiTextureEffect* pkEffect = pkEffectState->GetNextProjLight(
            kProjLightIter);  

        if (pkEffect && pkEffect->GetSwitch() == true &&
            pkEffect->GetTextureType() == NiTextureEffect::PROJECTED_LIGHT)
        {
            bool bClipped = pkEffect->GetClippingPlaneEnable();
            
            NiTextureEffect::CoordGenType eCoordType = 
                pkEffect->GetTextureCoordGen();
            
            switch(eCoordType)
            {
            case NiTextureEffect::WORLD_PARALLEL:
                SetProjectedLight(uiProjLightCount, bClipped, 
                    NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL);
                uiProjLightCount++;
                break;
            case NiTextureEffect::WORLD_PERSPECTIVE:
                SetProjectedLight(uiProjLightCount, bClipped, 
                    NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
                uiProjLightCount++;
                break;
            case NiTextureEffect::SPHERE_MAP:
            case NiTextureEffect::SPECULAR_CUBE_MAP:
            case NiTextureEffect::DIFFUSE_CUBE_MAP:
            default:
                NIASSERT(!"Unknown NiTextureEffect::CoordGenType");
                break;
            }
        }
    }
    SetPROJLIGHTMAPCOUNT(uiProjLightCount);
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetProjShadowMapDescriptor(
    const NiRenderObject*, 
    const NiDynamicEffectState* pkEffectState)
{
    NiDynEffectStateIter kProjShadowIter = 
        pkEffectState->GetProjShadowHeadPos();
    unsigned int uiProjShadowCount = 0;
    while (kProjShadowIter != NULL && uiProjShadowCount < 3)
    {
        NiTextureEffect* pkEffect = pkEffectState->GetNextProjShadow(
            kProjShadowIter);  

        if (pkEffect && pkEffect->GetSwitch() == true &&
            pkEffect->GetTextureType() == 
            NiTextureEffect::PROJECTED_SHADOW)
        {
            bool bClipped = pkEffect->GetClippingPlaneEnable();

            NiTextureEffect::CoordGenType eCoordType = 
                pkEffect->GetTextureCoordGen();

            if (pkEffect && pkEffect->GetSwitch() == true &&
                pkEffect->GetTextureType() == 
                NiTextureEffect::PROJECTED_SHADOW)
            {
                switch(eCoordType)
                {
                case NiTextureEffect::WORLD_PARALLEL:
                    SetProjectedShadow(uiProjShadowCount, bClipped,
                        NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL);
                    uiProjShadowCount++;
                    break;
                case NiTextureEffect::WORLD_PERSPECTIVE:
                    SetProjectedShadow(uiProjShadowCount, bClipped,
                        NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
                    uiProjShadowCount++;
                    break;
                case NiTextureEffect::SPHERE_MAP:
                case NiTextureEffect::SPECULAR_CUBE_MAP:
                case NiTextureEffect::DIFFUSE_CUBE_MAP:
                default:
                    NIASSERT(!"Unknown NiTextureEffect::CoordGenType");
                    break;
                }
            }
        }
    }
    SetPROJSHADOWMAPCOUNT(uiProjShadowCount);
    return true;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::SetShadowTechniqueDescriptor(
    const NiRenderObject*, const NiDynamicEffectState* pkEffectState)
{
    if (pkEffectState)
    {
        unsigned int uiShadowTechniqueSlot = 0;

        // Search lights in the order Point, Directional, Spot. This is 
        // required because all of NiStandardMaterial assumes lights have 
        // been added in this order.

        // Search Point Lights
        NiDynEffectStateIter kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::POINT_LIGHT ||
                pkLight->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT))
            {
                NiShadowGenerator* pkGenerator = 
                    pkLight->GetShadowGenerator();

                if (pkGenerator && pkGenerator->GetActive())
                {
                    unsigned short usActiveSlot = pkGenerator->
                        GetShadowTechnique()->GetActiveTechniqueSlot();

                    // If this assert is hit it means the light is assigned
                    // a NiShadowTechnique that is not one of the active 
                    // NiShadowTechniques.
                    NIASSERT (usActiveSlot < 
                        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );

                    // Only use the assigned shadow technique from the 
                    // light if the NiShadowTechnique has a higher priority
                    // than the one already applied. 
                    // Note: NiShadowTechnique priority is defined by the 
                    // slot the NiShadowTechnique is assigned to. 
                    uiShadowTechniqueSlot = 
                        NiMax(usActiveSlot, uiShadowTechniqueSlot);
                }
            }
        }


        // Search Directional Lights
        kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkLight->GetEffectType() == NiDynamicEffect::SHADOWDIR_LIGHT))
            {
                NiShadowGenerator* pkGenerator = 
                    pkLight->GetShadowGenerator();

                if (pkGenerator && pkGenerator->GetActive())
                {
                    unsigned short usActiveSlot = pkGenerator->
                        GetShadowTechnique()->GetActiveTechniqueSlot();

                    // If this assert is hit it means the light is assigned
                    // a NiShadowTechnique that is not one of the active 
                    // NiShadowTechniques.
                    NIASSERT (usActiveSlot < 
                        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );

                    // Only use the assigned shadow technique from the 
                    // light if the NiShadowTechnique has a higher priority
                    // than the one already applied. 
                    // Note: NiShadowTechnique priority is defined by the 
                    // slot the NiShadowTechnique is assigned to. 
                    uiShadowTechniqueSlot = 
                        NiMax(usActiveSlot, uiShadowTechniqueSlot);
                }
            }
        }

        // Search Spot Lights
        kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::SPOT_LIGHT ||
                pkLight->GetEffectType() == NiDynamicEffect::SHADOWSPOT_LIGHT))
            {
                NiShadowGenerator* pkGenerator = 
                    pkLight->GetShadowGenerator();

                if (pkGenerator && pkGenerator->GetActive())
                {
                    unsigned short usActiveSlot = pkGenerator->
                        GetShadowTechnique()->GetActiveTechniqueSlot();

                    // If this assert is hit it means the light is assigned
                    // a NiShadowTechnique that is not one of the active 
                    // NiShadowTechniques.
                    NIASSERT (usActiveSlot < 
                        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );

                    // Only use the assigned shadow technique from the 
                    // light if the NiShadowTechnique has a higher priority
                    // than the one already applied. 
                    // Note: NiShadowTechnique priority is defined by the 
                    // slot the NiShadowTechnique is assigned to. 
                    uiShadowTechniqueSlot = 
                        NiMax(usActiveSlot, uiShadowTechniqueSlot);
                }
            }
        }

        SetSHADOWTECHNIQUE(uiShadowTechniqueSlot);
    }
    return true;
}
//---------------------------------------------------------------------------
