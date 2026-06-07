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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoTexHelpers.h"
#include "CrosswalkGamebryoMatMap.h"
#include "CrosswalkGamebryoTexMap.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoHelpers.h"

#include "XSIMaterial.h"
#include "XSIShader.h"
#include "ConnectionPoint.h"
#include "ShaderConnectionPoint.h"
#include "variantparameter.h"
#include "custompset.h"

#include "NiTexturingProperty.h"
#include "NiTextureEffect.h"
#include "NiSourceTexture.h"
#include "NiSourceCubeMap.h"

#include "NiMaterialToolkit.h"
#include "NiShaderDesc.h"
#include "NiMesh.h"
#include "NiMaterialLibrary.h"

#include "NiBooleanExtraData.h"
#include "NiIntegerExtraData.h"
#include "NiIntegersExtraData.h"
#include "NiFloatExtraData.h"
#include "NiFloatsExtraData.h"
#include "NiStringExtraData.h"

namespace
{
    //---------------------------------------------------------------------------
    // Extract names of shaders/materials    
    //---------------------------------------------------------------------------
    void GetShaderNames(const char* pcShaderName, std::vector<std::string>* pvszShaderNames)
    {        
        // Clear container before filling up.
        pvszShaderNames->clear(); 

        NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
        
        for (unsigned int ui = 0; ui < pkToolkit->GetLibraryCount(); ui++)
        {
            NiMaterialLibrary* pkLib = pkToolkit->GetLibraryAt(ui);
            if (pkLib)
            {   
                for (unsigned int uj = 0; uj < pkLib->GetMaterialCount(); uj++)
                {
                    NiString strShaderName = pkLib->GetMaterialName(uj);

                    // Skip empty entries.
                    if (strShaderName.IsEmpty())
                        continue;

                    strShaderName = strShaderName.Left(MAX_PATH);
                    pvszShaderNames->push_back(std::string((const char*)strShaderName));
                }
            }
        }
    }   

    //---------------------------------------------------------------------------
    // Convert array of values to the proper type for NiExtraData.
    //---------------------------------------------------------------------------
    template <class T>
    NiIntegersExtraData* ConvertIntegersParam(SI_Int in_Count, const T* in_Values)
    {
        std::vector<int> values;
        values.reserve(in_Count);
        for (SI_Int i = 0; i < in_Count; ++i)
            values.push_back((int) in_Values[i]);
        return NiNew NiIntegersExtraData(in_Count, &values[0]);
    }

    //---------------------------------------------------------------------------
    NiFloatsExtraData* ConvertFloatsParam(SI_Int in_Count, const double* in_Values)
    {
        std::vector<float> values;
        values.reserve(in_Count);
        for (SI_Int i = 0; i < in_Count; ++i)
            values.push_back((float) in_Values[i]);
        return NiNew NiFloatsExtraData(in_Count, &values[0]);
    }

    //---------------------------------------------------------------------------
}

namespace epg
{        
    //---------------------------------------------------------------------------
    // Convert FX and CgFX programs.
    // Return true on success. Doing nothing is considered successful.
    //---------------------------------------------------------------------------
    bool ConvertFXShader(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_GamebryoFXShader)
    {       
        // Get toolkit.
        NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();

        // Retrieve the material directory from the shader library env. variable.
        // (Typical value: "%EGB_PATH%\Sdk\Win32\Shaders")
        char* pPath = getenv("EGB_SHADER_LIBRARY_PATH");
        if (pPath==NULL)
            return false;            
        std::string szShaderDir(pPath);
        szShaderDir += "\\data\\";
    
        // Initialize material toolkit to that directory.
        // Note: This is not necessary, but could be useful to define custom directory.
        pkToolkit->SetMaterialDirectory(szShaderDir.c_str());
        pkToolkit->UpdateMaterialDirectory();
            
        // Get FX file name from PPG.
        std::string szFileName = GetStringParameter(in_GamebryoFXShader, "selected_shader");
        size_t pos = szFileName.find_last_of(".");
        if (pos==std::string::npos)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Realtime FX shader \"%s\" cannot be found, used in material \"%s\".",
                szFileName.c_str(),
                in_Material.Name().GetText());

            return false;
        }

        std::string szShaderName = szFileName.substr(0, pos);

        // Make sure shader name is in there.
        /* !?!?! not even used anywhere in the code...?
        std::vector<std::string> vszShaderNames;
        GetShaderNames( szShaderName.c_str(), &vszShaderNames );
        */
             
        // Attach the proxy shader.
        NiMaterialPtr spMaterial = NiMaterialLibrary::CreateMaterial(szShaderName.c_str());
        if (!spMaterial)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Material \"%s\" not found.", szShaderName.c_str());
        }
   
        // Get shader's custom parameters
        // Get property page's custom parameters
        CSLCustomPSet* pCGFXParams = FindCustomProperties(
            in_GamebryoFXShader,
            "GamebryoFX_Parameters");
        if (!pCGFXParams)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Cannot find FX parameters in the FX shader \"%s\".",
                szShaderName.c_str());
        }

        // Define extra data.
        ExtraDataGroup* pExtraDataGroup  = new ExtraDataGroup(szShaderName.c_str(), pCGFXParams);
        int                     parameterCount   = pCGFXParams->GetParameterCount();
        CSLVariantParameter**   ppFXParams       = pCGFXParams->GetParameterList();
        int                     nextSamplerIndex = 0;
        int                     nextShaderMap    = 0;
        for (int i = 0; i < parameterCount; ++i)
        {
            CSLVariantParameter* pFXParam = ppFXParams[i];
            if (pFXParam == NULL)
                continue; 

            std::string     fxParamName = pFXParam->GetName();
            SI_TinyVariant* pVariant    = pFXParam->GetValue();
            NiExtraDataPtr  pExtraData;

            switch (pVariant->variantType)
            {
                case SI_VT_BOOL:
                    pExtraData = NiNew NiBooleanExtraData(pVariant->boolVal != 0);
                    break;
                case SI_VT_BYTE:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->bVal);
                    break;
                case SI_VT_UBYTE:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->ubVal);
                    break;
                case SI_VT_SHORT:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->sVal);
                    break;
                case SI_VT_USHORT:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->usVal);
                    break;
                case SI_VT_INT:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->nVal);
                    break;
                case SI_VT_UINT:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->unVal);
                    break;
                case SI_VT_LONG:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->lVal);
                    break;
                case SI_VT_ULONG:
                    pExtraData = NiNew NiIntegerExtraData(pVariant->ulVal);
                    break;
                case SI_VT_FLOAT:
                    pExtraData = NiNew NiFloatExtraData(pVariant->fVal);
                    break;
                case SI_VT_DOUBLE:
                    pExtraData = NiNew NiFloatExtraData((float) pVariant->dVal);
                    break;
                case SI_VT_PBOOL:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_boolVal);
                    break;
                case SI_VT_PBYTE:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_bVal);
                    break;
                case SI_VT_PUBYTE:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_ubVal);
                    break;
                case SI_VT_PSHORT:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_sVal);
                    break;
                case SI_VT_PUSHORT:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_usVal);
                    break;
                case SI_VT_PINT:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_nVal);
                    break;
                case SI_VT_PUINT:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_unVal);
                    break;
                case SI_VT_PLONG:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_lVal);
                    break;
                case SI_VT_PULONG:
                    pExtraData = ConvertIntegersParam(pVariant->numElems, pVariant->p_ulVal);
                    break;
                case SI_VT_PFLOAT:
                    pExtraData = NiNew NiFloatsExtraData(pVariant->numElems, pVariant->p_fVal);
                    break;
                case SI_VT_PDOUBLE:
                    pExtraData = ConvertFloatsParam(pVariant->numElems, pVariant->p_dVal);
                    break;
                case SI_VT_PCHAR:
                case SI_VT_PPCHAR:
                {
                    // Note: in XSI (Crosswalk) the shader textures are tagged as
                    //       having a value of "[sampler2D]", so detect them here.
                    bool bSampler2D = (0 == strcmp(pVariant->p_cVal, "[sampler2D]"));
                    if ( bSampler2D )
                    {
                        const int samplerIndex = nextSamplerIndex++;
                        const int shaderMapIndex = nextShaderMap;
                       
                        // FX shader can have optional textures.
                        // When nothing is connected to a texture, simply ignore it
                        // and move on since we cannot know if the texture is opptional
                        // or not. We simply emit an information message.
                        char texParamName[64];
                        sprintf(texParamName, "Texture_%d", samplerIndex);
                        CSLShaderConnectionPoint* texConn = epg::FindConnectionPoint(
                            in_GamebryoFXShader, texParamName);
                        if (!texConn)
                        {
                            io_Context.Logf(
                                LOG_INFO,
                                "Gamebryo FX shader \"%s\" use a texture sampler \"%s\""
                                " that has no image assigned: assuming the texture is optional.",
                                in_Material.Name().GetText(),
                                fxParamName.c_str());
                            break;
                        }

                        
                        NiTexturingProperty::MapEnum mapType;
                        if (0 == fxParamName.compare("BaseMap")
                        ||  0 == fxParamName.compare("BaseSampler"))
                        {
                            mapType = NiTexturingProperty::BASE_INDEX;
                        }
                        else if (0 == fxParamName.compare("DarkMap")
                        ||       0 == fxParamName.compare("DarkSampler"))
                        {
                            mapType = NiTexturingProperty::DARK_INDEX;
                        }
                        else if (0 == fxParamName.compare("GlowMap")
                        ||       0 == fxParamName.compare("GlowSampler"))
                        {
                            mapType = NiTexturingProperty::GLOW_INDEX;
                        }
                        else
                        {
                            mapType = NiTexturingProperty::SHADER_BASE;

                            // Magic extra data format for shader texture in Gamebryo.
                            pExtraData = NiNew NiIntegerExtraData(shaderMapIndex);
                            fxParamName += "Index";
                            nextShaderMap++;
                        }

                        const size_t expectedPos = fxParamName.length() - strlen("_cubeIndex");
                        const bool isCubeMap = (fxParamName.find("_cubeIndex") == expectedPos);
                        if (!ConvertRealtimeTexture_FX(
                            io_Context,
                            in_Material,
                            in_GamebryoFXShader,
                            texParamName,
                            mapType,
                            samplerIndex,
                            shaderMapIndex,
                            isCubeMap))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        pExtraData = NiNew NiStringExtraData(pVariant->p_cVal);
                    }
                    break;
                }
                default:
                    return io_Context.Logf(
                        LOG_WARNING,
                        "Invalid extra data type found in parameter \"%s\" of material \"%s\".",
                        fxParamName.c_str(),
                        in_Material.Name().GetText());
            }

            // Save name.
            if (pExtraData != NULL)
                pExtraData->SetName(fxParamName.c_str());

            // Save extra data into the group.
            pExtraDataGroup->AddEntry(pExtraData);
        }

        pExtraDataGroup->CombineParameters();

        // Apply material map.
        MaterialMap& materialMap = io_Context.GetMaterialMap();
        materialMap.AddMaterial(in_Material, *spMaterial);
        materialMap.AddExtraDataGroup(in_Material, pExtraDataGroup);

        return  true;
    }
    
    //---------------------------------------------------------------------------
}
