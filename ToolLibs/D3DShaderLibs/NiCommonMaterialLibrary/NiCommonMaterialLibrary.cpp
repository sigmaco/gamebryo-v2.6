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
#include "NiCommonMaterialLibPCH.h"

#include "NiCommonMaterialLibrary.h"
#include "NiFlatWireframeMaterial.h"
#include "TextureBlendMaterial.h"
#include "AdditiveLightMapsMaterial.h"

NiCommonMaterialLibrary* NiCommonMaterialLibrary::ms_pkThis = NULL;
unsigned int NiCommonMaterialLibrary::ms_uiMaterialDesc = 0;

//---------------------------------------------------------------------------
NiCommonMaterialLibrary::NiCommonMaterialLibrary() :
    NiMaterialLibrary("NiCommonMaterialLibrary")
{
    memset(m_apkShaderDesc, 0, sizeof(NiShaderDesc*)*NUM_COMMON_MATERIALS);

    m_apkShaderDesc[0] = NiNew NiShaderDesc();
    m_apkShaderDesc[0]->SetName("NiFlatWireframeMaterial");
    m_apkShaderDesc[0]->SetConfigurableMaterial(true);
    m_apkShaderDesc[0]->SetDescription("Material that renders"
        " in wireframe with no lighting. It can safely handle skinned and"
        " unskinned rendering.");
    m_apkShaderDesc[0]->SetBonesPerPartition(30);

    m_apkShaderDesc[1] = NiNew NiShaderDesc();
    m_apkShaderDesc[1]->SetName("TextureBlendMaterial");
    m_apkShaderDesc[1]->SetConfigurableMaterial(true);
    m_apkShaderDesc[1]->SetBinormalTangentMethod(
        NiShaderRequirementDesc::NBT_METHOD_ATI);
    m_apkShaderDesc[1]->SetBinormalTangentUVSource(
        NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT);
    m_apkShaderDesc[1]->SetDescription("Adds the ability to blend 4 textures"
        "(BlendMap0 - BlendMap3) by a 5th texture(ControlMap). "
        "The RGBA values of the control map define the weights for the "
        "BlendMaps. (R = BlendMap0, G = BlendMap1, B = BlendMap2, "
        "A = BlendMap3).");
    m_apkShaderDesc[1]->SetBonesPerPartition(30);

    m_apkShaderDesc[2] = NiNew NiShaderDesc();
    m_apkShaderDesc[2]->SetName("AdditiveLightMapsMaterial");
    m_apkShaderDesc[2]->SetConfigurableMaterial(true);
    m_apkShaderDesc[2]->SetDescription("Material that adds the capability of "
        "additive projected light maps, instead of the default standard "
        "material behavior.");
    m_apkShaderDesc[2]->SetBonesPerPartition(30);

    NiShaderAttributeDesc* pkAttribDesc = NULL;
    for (unsigned int ui = 0; ui < 4; ui++)
    {
        pkAttribDesc = NiNew NiShaderAttributeDesc();
        
        char acName[256];
        NiSprintf(acName, 256, "BlendMap%d", ui);
        
        pkAttribDesc->SetName(acName);
        pkAttribDesc->SetType(NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE);
        pkAttribDesc->SetHidden(false);
        pkAttribDesc->SetValue_Texture(ui, NULL);
        m_apkShaderDesc[1]->AddAttribute(pkAttribDesc);
    }

    pkAttribDesc = NiNew NiShaderAttributeDesc();
    
    char acName[256];
    NiSprintf(acName, 256, "ControlMap");

    pkAttribDesc->SetName(acName);
    pkAttribDesc->SetType(NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE);
    pkAttribDesc->SetHidden(false);
    pkAttribDesc->SetValue_Texture(4, NULL);
    m_apkShaderDesc[1]->AddAttribute(pkAttribDesc);

}
//---------------------------------------------------------------------------
NiCommonMaterialLibrary::~NiCommonMaterialLibrary()
{
    for (unsigned int ui = 0; ui < NUM_COMMON_MATERIALS; ui++)
        NiDelete m_apkShaderDesc[ui];
}
//---------------------------------------------------------------------------
NiMaterial* NiCommonMaterialLibrary::GetMaterial(const NiFixedString& kName)
{
    if (kName == "NiFlatWireframeMaterial")
        return NiNew NiFlatWireframeMaterial();
    if (kName == "TextureBlendMaterial")
        return NiNew TextureBlendMaterial();
    if (kName == "AdditiveLightMapsMaterial")
        return NiNew AdditiveLightMapsMaterial();

    return NULL;
}
//---------------------------------------------------------------------------
const char* NiCommonMaterialLibrary::GetMaterialName(unsigned int ui)
{
    NIASSERT(ui < NUM_COMMON_MATERIALS);
    NIASSERT(m_apkShaderDesc[ui]);
    return m_apkShaderDesc[ui]->GetName();
}
//---------------------------------------------------------------------------
unsigned int NiCommonMaterialLibrary::GetMaterialCount() const
{
    return NUM_COMMON_MATERIALS;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiCommonMaterialLibrary::GetFirstMaterialDesc()
{
    ms_uiMaterialDesc = 0;
    return ms_pkThis->m_apkShaderDesc[ms_uiMaterialDesc];
}
//---------------------------------------------------------------------------
NiShaderDesc* NiCommonMaterialLibrary::GetNextMaterialDesc()
{
    ms_uiMaterialDesc++;
   
    if (ms_uiMaterialDesc == NUM_COMMON_MATERIALS)
        return NULL;
    else
        return ms_pkThis->m_apkShaderDesc[ms_uiMaterialDesc];
}
//---------------------------------------------------------------------------
void NiCommonMaterialLibrary::_SDMInit()
{
    if (!ms_pkThis)
    {
        ms_pkThis = NiNew NiCommonMaterialLibrary();
    }
}
//---------------------------------------------------------------------------
void NiCommonMaterialLibrary::_SDMShutdown()
{
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}
//---------------------------------------------------------------------------
NiCommonMaterialLibrary* NiCommonMaterialLibrary::Create()
{
    if (!ms_pkThis)
    {
        ms_pkThis = NiNew NiCommonMaterialLibrary();
    }

    return ms_pkThis;
}
//---------------------------------------------------------------------------
