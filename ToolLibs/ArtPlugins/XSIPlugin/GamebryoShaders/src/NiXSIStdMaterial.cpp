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
  

#include "NiXSIStdMaterial.h"                   // for NiXSIStdMaterial
#include "NiXSIShader.h"                        // for NiXSIShader & NiXSIStdMaterial
#include "NiXSIPPG.h"                           // for property page definitions

#include <NiStandardMaterial.h>                 // for NiStandardMaterial
#include <NiShader.h>                           // for NiShader
#include "NiShaderFactory.h"                    // for NiShaderFactory
#include <NiStandardMaterialDescriptor.h>
#include "NiMaterialNodeLibrary.h"              // for NiMaterialNodeLibrary
#include "NiStandardPixelProgramDescriptor.h"   // for NiStandardPixelProgramDescriptor
#include "NiImageConverter.h"                   // for NiImageConverter
#include "NiImageConverter.h"                   // for NiImageConverter
#include "NiDevImageConverter.h"                // for NiDevImageConverter
#include "NiSourceTexture.h"                    // for NiSourceTexture

#include "resource.h"                           // for IDF_SHADEFILE_* definitions

#include <GamebryoShaders.h>
#include "CrosswalkGamebryoInit.h"

void GamebryoMaterial_DisposeXSIParams_v2(
    GamebryoMaterialInstanceData* in_pInstanceData);

void GamebryoMaterial_InitXSIParams_v2(
    GamebryoMaterialInstanceData *in_pInstanceData,
    CRef *in_pShaderHandle);

char* NiXSIStdMaterial::m_silighttype[CG_NB_LIGHTS] =
{
    "silighttype_0",
    "silighttype_1",
    "silighttype_2",
    "silighttype_3",
    "silighttype_4",
    "silighttype_5",
    "silighttype_6"
};

char* NiXSIStdMaterial::m_silightambient[CG_NB_LIGHTS] =
{
    "silightambient_0",
    "silightambient_1",
    "silightambient_2",
    "silightambient_3",
    "silightambient_4",
    "silightambient_5",
    "silightambient_6"
};

char* NiXSIStdMaterial::m_silightdiffuse[CG_NB_LIGHTS] =
{
    "silightdiffuse_0",
    "silightdiffuse_1",
    "silightdiffuse_2",
    "silightdiffuse_3",
    "silightdiffuse_4",
    "silightdiffuse_5",
    "silightdiffuse_6"
};

char* NiXSIStdMaterial::m_silightspecular[CG_NB_LIGHTS] =
{
    "silightspecular_0",
    "silightspecular_1",
    "silightspecular_2",
    "silightspecular_3",
    "silightspecular_4",
    "silightspecular_5",
    "silightspecular_6"
};

char* NiXSIStdMaterial::m_silightdirection[CG_NB_LIGHTS] =
{
    "silightdirection_0",
    "silightdirection_1",
    "silightdirection_2",
    "silightdirection_3",
    "silightdirection_4",
    "silightdirection_5",
    "silightdirection_6"
};

char* NiXSIStdMaterial::m_silightposition[CG_NB_LIGHTS] =
{
    "silightposition_0",
    "silightposition_1",
    "silightposition_2",
    "silightposition_3",
    "silightposition_4",
    "silightposition_5",
    "silightposition_6"
};

char* NiXSIStdMaterial::m_silightfalloff[CG_NB_LIGHTS] =
{
    "silightfalloff_0",
    "silightfalloff_1",
    "silightfalloff_2",
    "silightfalloff_3",
    "silightfalloff_4",
    "silightfalloff_5",
    "silightfalloff_6"
};

char* NiXSIStdMaterial::m_silightattenuation[CG_NB_LIGHTS] =
{
    "silightattenuation_0",
    "silightattenuation_1",
    "silightattenuation_2",
    "silightattenuation_3",
    "silightattenuation_4",
    "silightattenuation_5",
    "silightattenuation_6"
};

char* NiXSIStdMaterial::m_silightcone[CG_NB_LIGHTS] =
{
    "silightcone_0",
    "silightcone_1",
    "silightcone_2",
    "silightcone_3",
    "silightcone_4",
    "silightcone_5",
    "silightcone_6"
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 
// Class NiXSIStdMaterial
// 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NiXSIStdMaterial::NiXSIStdMaterial()
{
    m_spLibrary             = NULL; 
    m_spStandardMaterial    = NULL;
    m_pkDescriptor          = NULL;
}

NiXSIStdMaterial::~NiXSIStdMaterial()
{
   //...
}

void NiXSIStdMaterial::Gamebryo_Init()
{
    epg::InitializeGamebryo("GamebryoShaders");

/*
    NiImageConverter::SetImageConverter( NiNew NiDevImageConverter );
    NiTexture::SetMipmapByDefault( true );
    NiSourceTexture::SetUseMipmapping( true );

    // Setup the material shader generation folder prior to creating the renderer.
    //NiMaterial::SetDefaultWorkingDirectory("../../Data/Shaders/Generated");
    // temp directory - change later...
    NiMaterial::SetDefaultWorkingDirectory( "C:\\temp\\generated_shaders\\" ); 

    // Create a default material using the specified material node library. 
    // If bAutoCreateCaches is true, a cache is created using the static renderer pointer.
    if ( m_spLibrary == NULL )
        m_spLibrary = NiNew NiMaterialNodeLibrary( 1 ); //usVersion
    
    // Create a default material using the specified material node library. 
    // If bAutoCreateCaches is true, a cache is created using the static renderer pointer.
    // >>> We don't have a RENDERER, so we set bAutoCreateCaches to false <<<
    if ( m_spStandardMaterial == NULL )
        m_spStandardMaterial = NiNew NiStandardMaterial( m_spLibrary, false ); //bAutoCreateCaches

    if ( m_pkDescriptor != NULL )
        m_pkDescriptor = NiNew NiStandardMaterialDescriptor();
*/
}

void NiXSIStdMaterial::Gamebryo_Shutdown()
{ 
/*
    if ( m_spLibrary != NULL )
    {
        NiDelete m_spLibrary; 
        m_spLibrary           = NULL; // reset ptr
    }

    if ( m_spStandardMaterial != NULL )    
    {
        NiDelete m_spStandardMaterial;
        m_spStandardMaterial  = NULL; // reset ptr
    }

    if ( m_pkDescriptor != NULL )    
    {
        NiDelete m_pkDescriptor;
        m_pkDescriptor  = NULL; // reset ptr
    }
*/
    epg::ShutdownGamebryo("GamebryoShaders", true);
}

// Initialization by shader set - flexible way to hard-code predefined shaders.
bool NiXSIStdMaterial::Set_ShaderSet( niShaderSet eShaderSet )
{
    // Clear old shader sets - if they exists
    Terminate();

    m_eShaderSet = eShaderSet; // for interrogation

    // Add new shader set
    switch ( eShaderSet )
    {
        case NI_SHADERSET_00_PASSTHROUGH:
            Add_pShader(new NiXSIShader(
                NI_SHADER_VERTEX,
                true,
                IDF_SHADEFILE_VERTEX_00_PASSTHROUGH));
            Add_pShader(new NiXSIShader(
                NI_SHADER_FRAGMENT,
                true,
                IDF_SHADEFILE_FRAGMENT_00_PASSTHROUGH));
            break;
        case NI_SHADERSET_01_PHONG: // 0 - basic phong + 3lights - no maps
            Add_pShader(new NiXSIShader(
                NI_SHADER_VERTEX,
                true,
                IDF_SHADEFILE_VERTEX_01_PHONG));
            Add_pShader(new NiXSIShader(
                NI_SHADER_FRAGMENT,
                true,
                IDF_SHADEFILE_FRAGMENT_01_PHONG));
            break;
        case NI_SHADERSET_02_DIFFUSEMAP1: // 1 - basic phong + 3 lights + diffuse maps 1
            Add_pShader(new NiXSIShader(
                NI_SHADER_VERTEX,
                true,
                IDF_SHADEFILE_VERTEX_02_DIFFUSEMAP1));
            Add_pShader(new NiXSIShader(
                NI_SHADER_FRAGMENT,
                true,
                IDF_SHADEFILE_FRAGMENT_02_DIFFUSEMAP1));
            break;
        case NI_SHADERSET_03_DIFFUSEMAP2: // 2 - basic phong + 3 lights + diffuse maps 2
            Add_pShader(new NiXSIShader(
                NI_SHADER_VERTEX,
                true,
                IDF_SHADEFILE_VERTEX_03_DIFFUSEMAP2));
            Add_pShader(new NiXSIShader(
                NI_SHADER_FRAGMENT,
                true,
                IDF_SHADEFILE_FRAGMENT_03_DIFFUSEMAP2));
            break;
        case NI_SHADERSET_04_BUMPMAP: // 3 - basic phong + 3 lights + bump maps
            Add_pShader(new NiXSIShader(
                NI_SHADER_VERTEX,
                true,
                IDF_SHADEFILE_VERTEX_04_BUMPMAP));
            Add_pShader(new NiXSIShader(
                NI_SHADER_FRAGMENT,
                true,
                IDF_SHADEFILE_FRAGMENT_04_BUMPMAP));
            break;
    }

    return true;
}

// Initialization by program names.
// Example:
//    std::string     szPath          = "C:\\XSIPlugin\\GamebryoShaders\\src\\";
//    std::string     szProg_vertex   = szPath; szProg_vertex   += "gb_vertex_01.cg";
//    std::string     szProg_fragment = szPath; szProg_fragment += "gb_fragment_01.cg";
bool NiXSIStdMaterial::Set_ShaderSet( std::string szProg_vertex, std::string szProg_fragment )
{
    if ( !szProg_vertex.empty() )
        Add_pShader(new NiXSIShader(NI_SHADER_VERTEX, true, szProg_vertex));
    if ( !szProg_fragment.empty() )
        Add_pShader(new NiXSIShader(NI_SHADER_FRAGMENT, true, szProg_fragment));

    return true;
}

bool NiXSIStdMaterial::Terminate()
{
    // Terminate for each shader
    for (int i = 0; i < Get_nShaders(); i++)
    {
        NiXSIShader* pShader = Get_pShader(i);
        if (pShader == NULL)
            continue;

        // ...

        // Remove from memory
        delete pShader;
    }

    // Clear container
    Clear_Shaders();

    return true;
}

//---------------------------------------------------------------------------
// Execute_Embeded
// Execution callback.
//---------------------------------------------------------------------------
bool NiXSIStdMaterial::Execute_Embeded(
    XSI_RTS_SystemParams* in_pSysParams, 
    void*                           in_pParams, 
    XSI_RTS_TriangulatedGeometry*   in_pGeom, 
    void**                          io_pShaderData )
{
	GamebryoMaterialInstanceData* pInstanceData = (GamebryoMaterialInstanceData*) *io_pShaderData;
	GamebryoMaterialPPG*          l_pPPG        = (GamebryoMaterialPPG*) in_pParams;            

    assert( pInstanceData != NULL );

    // If something changed, define new shader
    if (m_spShader == NULL)
        if (!CgProgram_Extract(in_pSysParams, in_pParams, in_pGeom, io_pShaderData))
            return false;        

    // Just use previously-created one
    CgProgram_Apply(in_pSysParams, in_pParams, in_pGeom, io_pShaderData, m_spShader);

    return true;
}

//---------------------------------------------------------------------------
// CgProgram_Extract
// Extract Cg program from Gamebryo.
//
// The pipeline for generating a compiled shader is as follows:
//
// 1) Generate the descriptor for the geometry object. This will produce an 
//    NiMaterialDescriptor object, which is a 128-bit bitfield that encodes 
//    all relevant information about the state of the object as it pertains 
//    to generating its shade-tree. This process is encapsulated in the 
//    GenerateDescriptor method. A common checklist of items that can be 
//    examined would be:
//     * Is the object skinned?
//     * What per-vertex data exists on the object?
//     * Is the object lit? How many lights are affecting it? What types 
//       of lights are affecting it?
//     * What textures are applied to the object? What UV sets do they use?
//     * Are there tangent-space bases per-vertex?
//     * Is the object affected by specularity?
//
// 2) Search through the NiShader cache to determine if this material configuration 
//    has been encountered before. This is usually one of the first things done in 
//    the GenerateShader method. If this material configuration has been encountered 
//    before, simply re-use the previously generated shader.
//
// 3) Search through the list of failed shaders to see if this material configuration 
//    is even possible for the current hardware. If so, return NULL and the renderer 
//    will use its internal error shader.
//
// 4) Break the material descriptor into one or more sets of NiGPUProgramDescriptors. 
//    These classes are 128-bit bitfields that encapsulate all that needs to be done 
//    to generate the correct NiGPUProgram for a given pass in this material 
//    configuration. The decomposition of material descriptor to program descriptors 
//    is performed in the GenerateShaderDescArray method. 
//
// 5) Create the appropriate NiShader derived class for the current renderer.
//
// 6) Determine the packing requirements for that shader.
//
// 7) For each set of NiGPUProgramDescriptors in this material instance, generate 
//    a rendering pass object.
//         a) Determine how this pass will blend with the previous pass (SetAlphaOverride method)
//         b) Generate the GPU programs for the pass (GenerateShaderPrograms method). 
//            See Creating the Shade Tree for more information.
//             1. Generate the vertex program (GenerateVertexShaderProgram method)
//             2. Generate the pixel program (GeneratePixelShaderProgram method)
//             3. For each program generated, create an NiShaderConstantMap 
//                and add all uniform variables to it (AddResourceToShaderConstantMap method).
//             4. If an error occurred, return the appropriate error code.
//
// 8) If successful in the previous steps, initialize the new shader and return. 
//    Otherwise, call an appropriate fallback handler and retry.
//
//---------------------------------------------------------------------------
bool NiXSIStdMaterial::CgProgram_Extract(
    XSI_RTS_SystemParams* in_pSysParams, 
    void*                           in_pParams, 
    XSI_RTS_TriangulatedGeometry*   in_pGeom, 
    void**                          io_pShaderData )
{
	GamebryoMaterialInstanceData* pInstanceData   = (GamebryoMaterialInstanceData*) *io_pShaderData;
	GamebryoMaterialPPG*          l_pPPG          = (GamebryoMaterialPPG*) in_pParams;            
    NiShader*                     pShader_out     = NULL;
    NiUInt32                      mapCount = 0;
    NiString                      kDescString_00, kDescString_01, kDescString_02; // Debugging.
    
    assert( pInstanceData != NULL );
    
    // Convert PPG map index to Std Material enum
    NiStandardMaterial::TextureMap eMap_to_StdMap[SI_NUM_MATERIALMAPS] =
    {
        NiStandardMaterial::MAP_BASE,               // SI_MATERIALMAP_BASE,                 // 0
        NiStandardMaterial::MAP_DARK,               // SI_MATERIALMAP_DARK,                 // 1
        NiStandardMaterial::MAP_DETAIL,             // SI_MATERIALMAP_DETAIL,               // 2
        NiStandardMaterial::MAP_DECAL00,            // SI_MATERIALMAP_DECAL_01,             // 3
        NiStandardMaterial::MAP_DECAL01,            // SI_MATERIALMAP_DECAL_02,             // 4
        NiStandardMaterial::MAP_DECAL02,            // SI_MATERIALMAP_DECAL_03,             // 5
        NiStandardMaterial::MAP_NORMAL,             // SI_MATERIALMAP_NORMAL,               // 6
        NiStandardMaterial::MAP_PARALLAX,           // SI_MATERIALMAP_PARALLAX,             // 7
        NiStandardMaterial::MAP_ENV,                // SI_MATERIALMAP_ENVIRONMENT,          // 8
        NiStandardMaterial::MAP_BUMP,               // SI_MATERIALMAP_BUMP,                 // 9
        NiStandardMaterial::MAP_GLOSS,              // SI_MATERIALMAP_GLOSS,                // 10
        NiStandardMaterial::MAP_PROJECTED_LIGHT00,  // SI_MATERIALMAP_PROJECTEDLIGHT_01,    // 11
        NiStandardMaterial::MAP_PROJECTED_LIGHT01,  // SI_MATERIALMAP_PROJECTEDLIGHT_02,    // 12
        NiStandardMaterial::MAP_PROJECTED_LIGHT02,  // SI_MATERIALMAP_PROJECTEDLIGHT_03,    // 13
        NiStandardMaterial::MAP_PROJECTED_SHADOW00, // SI_MATERIALMAP_PROJECTEDSHADOW_01,   // 14
        NiStandardMaterial::MAP_PROJECTED_SHADOW01, // SI_MATERIALMAP_PROJECTEDSHADOW_02,   // 15
        NiStandardMaterial::MAP_PROJECTED_SHADOW02, // SI_MATERIALMAP_PROJECTEDSHADOW_03,   // 16
        NiStandardMaterial::MAP_GLOW                // SI_MATERIALMAP_GLOW,                 // 17
    };
                                                                              
    //--------------------------------------------------------------
    // 1) Generate the NiStandardMaterialDescriptor necessary to 
    //    map the individual state entries in XSI to what is 
    //    expected in Gamebryo. 
    //--------------------------------------------------------------   
    
    // Add enabled map descriptors        
    for (int j = 0; j < SI_NUM_MATERIALMAPS; j++)
    {
        // Enable or disable a specific Map enumeration in a pixel descriptor
        bool bEnabled = (l_pPPG->Tex[j].Map_Enabled)? true : false;
        if ( !SetMapEnabled(eMap_to_StdMap[j],bEnabled,m_pkDescriptor) )
            return false;           

        // Get name - for debugging
        std::string szMapName = l_pPPG->Map_Image[j].m_pchPath;

        // Uncomment these lines to get a human-readable version of the 
        // material description
        kDescString_00 = m_pkDescriptor->ToString();

        // Creates a property and creates a base map using an NiTexture created 
        // from the given image filename. 
        NiTexturingProperty* pTexProp = NiNew NiTexturingProperty(l_pPPG->Map_Image[j].m_pchPath);
        NiTexturingProperty::Map* pkMap = NiNew NiTexturingProperty::Map();
        pkMap->SetClampMode( NiTexturingProperty::WRAP_S_WRAP_T );
        pkMap->SetFilterMode( NiTexturingProperty::FILTER_BILERP );
        pkMap->SetTextureIndex( 0 );
        // UV Transform
        if ( l_pPPG->Tex[j].TexCoord_Enabled && l_pPPG->Tex[j].TexCoord_transform )
        {
            typedef NiTextureTransform::TransformMethod TransformMethod;

            NiPoint2 kTranslate(l_pPPG->Tex[j].TexCoord_transx, l_pPPG->Tex[j].TexCoord_transy);
            float    fRotate = 0.0f;
            NiPoint2 kScale(l_pPPG->Tex[j].TexCoord_rotx, l_pPPG->Tex[j].TexCoord_roty);
            NiPoint2 kCenter(0.0f, 0.0f); 
            // check that one...we need a XSI_TRANSFORM
            TransformMethod     eMethod = NiTextureTransform::MAX_TRANSFORM; 
            NiTextureTransform* pNiTexTransform =
                NiNew NiTextureTransform(kTranslate, fRotate, kScale, kCenter, eMethod);

            pkMap->SetTextureTransform( pNiTexTransform );
        }            
        // Set a single map (NOTE: can not set shader maps)
        pTexProp->SetMap( mapCount, pkMap );

        // not sure what to feed to this... Needed?
        const NiTexturingProperty::Map** activeMaps = NULL; 

        // For projected/env maps, create dummy mesh data      
        NiRenderObject*       pkMesh        = NULL; 
        NiDynamicEffectState* pkEffectState = NULL;
        NiDynamicEffect*      pEffect       = NULL;
        if (j == SI_MATERIALMAP_ENVIRONMENT
        ||  j == SI_MATERIALMAP_PROJECTEDLIGHT_01
        ||  j == SI_MATERIALMAP_PROJECTEDLIGHT_02
        ||  j == SI_MATERIALMAP_PROJECTEDLIGHT_03
        ||  j == SI_MATERIALMAP_PROJECTEDSHADOW_01
        ||  j == SI_MATERIALMAP_PROJECTEDSHADOW_02
        ||  j == SI_MATERIALMAP_PROJECTEDSHADOW_03)
        {
            pkMesh          = NULL; 
            pkEffectState   = NiNew NiDynamicEffectState();
            pkEffectState->AddEffect( NiNew NiDynamicEffect() );
        }

        // Uncomment these lines to get a human-readable version of the 
        // material description
        kDescString_01 = m_pkDescriptor->ToString();

        switch (j)
        {
            case SI_MATERIALMAP_BASE:                            // 0         
            {                                    
                if (!m_pkDescriptor->SetBaseMapDescriptor(pTexProp,activeMaps,mapCount))
                    return false;
                break;
            }
            case SI_MATERIALMAP_DARK:                            // 1
            {
                if (!m_pkDescriptor->SetDarkMapDescriptor(pTexProp,activeMaps,mapCount))
                    return false;
                break;
            }
            case SI_MATERIALMAP_DETAIL:                          // 2
            {
                if (!m_pkDescriptor->SetDetailMapDescriptor(pTexProp,activeMaps,mapCount))
                    return false;
                break;
            }
            case SI_MATERIALMAP_DECAL_01:                        // 3           
            case SI_MATERIALMAP_DECAL_02:                        // 4
            case SI_MATERIALMAP_DECAL_03:                        // 5
            {
            //  if (!m_pkDescriptor->SetDecalMapDescriptor(pTexProp,activeMaps,mapCount))
            //      return false;
                break;
            }
            case SI_MATERIALMAP_NORMAL:                          // 6
            {
                if (!m_pkDescriptor->SetNormalMapDescriptor(pTexProp,activeMaps,mapCount))
                    return false;
                break;
            }
            case SI_MATERIALMAP_PARALLAX:                        // 7
            {
                if (!m_pkDescriptor->SetParallaxMapDescriptor(pTexProp,activeMaps,mapCount))
                    return false;
                break;
            }
            case SI_MATERIALMAP_ENVIRONMENT:                     // 8
            {               
                if (!m_pkDescriptor->SetEnvMapDescriptor(pkMesh,pkEffectState)) 
                    return false;
                break;
            }
            case SI_MATERIALMAP_BUMP:                            // 9
            {
                const bool isEnv = false;
                if (!m_pkDescriptor->SetBumpMapDescriptor(pTexProp,activeMaps,mapCount,isEnv))
                    return false;
                break;
            }
            case SI_MATERIALMAP_GLOSS:                           // 10
            {
                bool env  = (l_pPPG->Tex[SI_MATERIALMAP_ENVIRONMENT].Map_Enabled)
                          ? true : false;
                bool spec = (l_pPPG->cKs.m_red>0 || l_pPPG->cKs.m_green>0 || l_pPPG->cKs.m_blue>0)
                          ? true : false;
                if (!m_pkDescriptor->SetGlossMapDescriptor(pTexProp,activeMaps,mapCount,env,spec))
                    return false;
                break;
            }
            case SI_MATERIALMAP_PROJECTEDLIGHT_01:               // 11
            case SI_MATERIALMAP_PROJECTEDLIGHT_02:               // 12
            case SI_MATERIALMAP_PROJECTEDLIGHT_03:               // 13
            {                
                if (!m_pkDescriptor->SetProjLightMapDescriptor(pkMesh,pkEffectState)) 
                    return false;
                break;
            }
            case SI_MATERIALMAP_PROJECTEDSHADOW_01:              // 14
            case SI_MATERIALMAP_PROJECTEDSHADOW_02:              // 15
            case SI_MATERIALMAP_PROJECTEDSHADOW_03:              // 16   
            {               
                if (!m_pkDescriptor->SetProjShadowMapDescriptor(pkMesh,pkEffectState)) 
                    return false;
                break;
            }    
            case SI_MATERIALMAP_GLOW:                            // 17
            {
                if (!m_pkDescriptor->SetGlowMapDescriptor(pTexProp,activeMaps,mapCount))
                    return false;
                break;
            }
        }
    }

    // Uncomment these lines to get a human-readable version of the 
    // material description
    kDescString_02 = m_pkDescriptor->ToString();

    //--------------------------------------------------------------
    // 2) Modify NiStandardMaterial so that you can hand it a 
    //    NiStandardMaterialDescriptor and it will generate 
    //    any pixel and vertex shader code necessary to render 
    //    the object. 
    // >> This is partially done in NiFragmentMaterial::GenerateShader
    //--------------------------------------------------------------   

    // Inherited from NiFragmentMaterial. Creates the default fallback functions.
    m_spStandardMaterial->AddDefaultFallbacks();

    //--------------------------------------------------------------
    // NiGPUProgramCache
    // The NiGPUProgramCache is a system designed to contain NiGPUPrograms 
    // that have been compiled with an additive material system. In such 
    // a material system, it is possible to have NiShaders that re-use 
    // previously generated GPU programs. Since compilation of GPU programs 
    // is an expensive operation, the cache can store the results of previous 
    // compilations from previous application runs. Once a program is compiled, 
    // it is inserted into the cache, and can optionally be saved to disk. 
    // 
    // The primary interfaces to this class are the FindCachedProgram and 
    // GenerateProgram methods. FindCachedProgram  is used to search the cache 
    // for a program matching the existing name. The method returns two objects, 
    // the GPU program (if found) and a set of the uniform resources used by 
    // that program. GenerateProgram is used to actually compile a high-level 
    // shader language program. 
    // 
    //Each renderer implements its own derived program cache class. 
    // The DX9 renderer and Xbox 360 renderers have the NiD3DGPUProgramCache, 
    // which compiles HLSL programs. If an application does not wish to 
    // manually create program caches, the NiRenderer::SetDefaultProgramCache 
    // methods will instantiate the correct NiGPUProgramCache for the 
    //renderer and set it on a given NiFragmentMaterial. In general, 
    // applications use the simpler SetDefaultProgramCache method, which 
    //in turn calls the more complex method with default arguments. 
    // These default arguments are: 
    // * NiFragmentMaterial::GetDefaultAutoSaveProgramCache(),  
    // * NiFragmentMaterial::GetDefaultWriteDebugProgramData(), 
    // * NiFragmentMaterial::GetDefaultLoadProgramCacheOnCreation(), 
    // * NiFragmentMaterial::GetDefaultLockProgramCache(), and 
    // * NiMaterial::GetDefaultWorkingDirectory().
    //--------------------------------------------------------------


    //--------------------------------------------------------------   
    // 4) Break the material descriptor into one or more sets of NiGPUProgramDescriptors. 
    //    These classes are 128-bit bitfields that encapsulate all that needs to be done 
    //    to generate the correct NiGPUProgram for a given pass in this material 
    //    configuration. The decomposition of material descriptor to program descriptors 
    //    is performed in the GenerateShaderDescArray method. 
    //--------------------------------------------------------------   
    
    //--------------------------------------------------------------   
    // 5) Create the appropriate NiShader derived class for the current renderer.
    //--------------------------------------------------------------   
    
    //--------------------------------------------------------------   
    // 6) Determine the packing requirements for that shader.
    //--------------------------------------------------------------   


    //--------------------------------------------------------------   
    // 7) For each set of NiGPUProgramDescriptors in this material instance, generate 
    //    a rendering pass object.
    //         a) Determine how this pass will blend with the previous pass (SetAlphaOverride)
    //         b) Generate the GPU programs for the pass (GenerateShaderPrograms method). 
    //            See Creating the Shade Tree for more information.
    //             1. Generate the vertex program (GenerateVertexShaderProgram method)
    //             2. Generate the pixel program (GeneratePixelShaderProgram method)
    //             3. For each program generated, create an NiShaderConstantMap 
    //                and add all uniform variables to it (AddResourceToShaderConstantMap method).
    //             4. If an error occurred, return the appropriate error code.
    //--------------------------------------------------------------   


    //--------------------------------------------------------------   
    // 8) If successful in the previous steps, initialize the new shader and return. 
    //    Otherwise, call an appropriate fallback handler and retry.
    //--------------------------------------------------------------   

/*
    // Inherited from NiMaterial. Compute the NiShader needed to render this
    // piece of geometry.
    NiShader* m_spStandardMaterial->GetCurrentShader( const NiRenderObject* pkGeometry, 
                                                    const NiPropertyState* pkState, 
                                                    const NiDynamicEffectState* pkEffects, 
                                                    unsigned int uiMaterialExtraData);
*/

/*
    // Utility method for generating the NiGPUProgram  objects needed to render
    // a given object without creating the NiShader. 
    m_spStandardMaterial->PrecacheGPUPrograms( const NiRenderObject* pkGeometry, 
                                                const NiPropertyState* pkState, 
                                                const NiDynamicEffectState* pkEffects, 
                                                unsigned int uiMaterialExtraData );
*/
   


    return true;
}

//---------------------------------------------------------------------------
// NiXSIStdMaterial::CgProgram_Apply
// Apply Cg program previously extrcated from Gamebryo.
//---------------------------------------------------------------------------
bool NiXSIStdMaterial::CgProgram_Apply(
    XSI_RTS_SystemParams* in_pSysParams,
    void* in_pParams,					    
    XSI_RTS_TriangulatedGeometry* in_pGeom,
    void** io_pShaderData,
    NiShader* pShader)
{
	GamebryoMaterialInstanceData* pInstanceData = (GamebryoMaterialInstanceData*) *io_pShaderData;
	GamebryoMaterialPPG*          l_pPPG        = (GamebryoMaterialPPG*) in_pParams;	
	int						      loop;

    // Change Shader Set - if required
    bool            bChangedShaderSet   = false;
    niShaderSet     eShaderSet          = pInstanceData->m_XSIStdMaterial.Get_eShaderSet();
    niMaterialMap   eMap                = (niMaterialMap)l_pPPG->ziPreviewTexture;
    bool            bActiveTexEnabled   = (l_pPPG->Tex[eMap].Map_Enabled)? true : false;
    if (bActiveTexEnabled == FALSE
    &&  eShaderSet != NI_SHADERSET_01_PHONG)
    {
        pInstanceData->m_XSIStdMaterial.Set_ShaderSet( NI_SHADERSET_01_PHONG ); // no textures
        bChangedShaderSet = true;        
    } 

    if (bActiveTexEnabled == TRUE
    &&  eShaderSet != NI_SHADERSET_02_DIFFUSEMAP1)
    {
        pInstanceData->m_XSIStdMaterial.Set_ShaderSet( NI_SHADERSET_02_DIFFUSEMAP1 ); // textured
        bChangedShaderSet = true;        
    }

    // If shader changed, delete buffered program string
    if (bChangedShaderSet
    &&  pInstanceData->m_pProgramString)
    {
	    free( pInstanceData->m_pProgramString );
        pInstanceData->m_pProgramString = NULL; // reset - Important!
    }
    
    // Execute for each shader program...
    for (int i = 0; i < pInstanceData->m_XSIStdMaterial.Get_nShaders(); i++)
    {
        NiXSIShader* pNiXSIShader = pInstanceData->m_XSIStdMaterial.Get_pShader(i);
        if (pNiXSIShader == NULL)
            continue;

        // Start of shader processing...
        //************************************************************
                        
	    CRef shader = *( (CRef *) in_pSysParams->m_pShaderHandle);
	    bool bRecompiled = false;

	    Cg_GetContext()->m_Context = pInstanceData->m_Context;	    
               
	    // If program exist, just bind it
	    if( pInstanceData->m_Program[i] != NULL && !bChangedShaderSet )
	    {
		    cgSetErrorCallback( NULL );
		    cgGLEnableProfile( pNiXSIShader->Get_eProfile() );	
		    cgGLBindProgram( pInstanceData->m_Program[i] );	
		    
            if( cgGetError() != CG_NO_ERROR )
			    pInstanceData->m_Program[i] = NULL;
		    
            cgSetErrorCallback( Cg_CheckError );
	    }

        // Compile source code, if something has changed
	    if(	(pNiXSIShader->Get_bCode())  &&
		    (pNiXSIShader->Get_bActive()) &&
		    (	bChangedShaderSet ||
                (pInstanceData->m_Program[i] == NULL) || 
			    (in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_PARAM_CHANGE) ||
			    (in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_TREE_CHANGE) ||
			    !cgIsProgram(pInstanceData->m_Program[i])))
	    {
		    if( pNiXSIShader->Get_bActive() )
		    {
			    if(	bChangedShaderSet ||
                    (pInstanceData->m_Program[i] == NULL) || 
				    (pInstanceData->m_pProgramString == NULL) || 
				    (in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_TREE_CHANGE) ) 
			    {
				    Log(siInfoMsg, "GamebryoFX: compiling Cg Program.");

				    if(	cgIsProgram(pInstanceData->m_Program[i]) && (pInstanceData->m_Context != NULL) )
				    {
					    cgDestroyContext(pInstanceData->m_Context);
					    pInstanceData->m_Context = cgCreateContext();
					    Cg_GetContext()->m_Context = pInstanceData->m_Context;
					    pInstanceData->m_ContextCount ++;
				    }

                    // Create program
                    if ( !pNiXSIShader->Get_szCodeFile().empty() ) // from file
                    {
	                    pInstanceData->m_Program[i] = cgCreateProgramFromFile(
                            Cg_GetContext()->m_Context, CG_SOURCE, 
                            pNiXSIShader->Get_szCodeFile().c_str(), 
                            pNiXSIShader->Get_eProfile(), "main", 0 );
                    }
                    else if ( !pNiXSIShader->Get_szCodeSource().empty() ) // from source
                    {
				        pInstanceData->m_Program[i] = cgCreateProgram(
                            Cg_GetContext()->m_Context, CG_SOURCE, 
                            pNiXSIShader->Get_szCodeSource().c_str(), 
                            pNiXSIShader->Get_eProfile(), "main", NULL );
                    }

                    // Save original Cg source program
                    if (cgIsProgramCompiled(pInstanceData->m_Program[i]))
                    {
                        pNiXSIShader->Set_szCodeSource(
                            cgGetProgramString(pInstanceData->m_Program[i], CG_PROGRAM_SOURCE));
                    }

                    // Show debug information
				    Logf(siInfoMsg, "GamebryoFX: %s.",
                        cgIsProgramCompiled(pInstanceData->m_Program[i])
                        ? "Program is compiled" : "Program is not compiled");
				    Logf(siInfoMsg, "GamebryoFX: LAST LISTING----%s----",
                        cgGetLastListing(Cg_GetContext()->m_Context));
				    Logf(siInfoMsg,"GamebryoFX: ---- PROGRAM BEGIN ----\n%s---- PROGRAM END ----",
                        cgGetProgramString(pInstanceData->m_Program[i],CG_COMPILED_PROGRAM));
			    }

			    bRecompiled = true;

                // Delete old string allocation &copy new one
			    if ( pInstanceData->m_pProgramString )
				    free( pInstanceData->m_pProgramString );                
			    pInstanceData->m_pProgramString = strdup( pNiXSIShader->Get_szCodeSource().c_str() );

                // Load resulting program
			    cgGLLoadProgram( pInstanceData->m_Program[i] );
		    }
	    }
    
        // Try to bind parameters
	    if( pNiXSIShader->Get_bActive() && (pInstanceData->m_Program[i] != NULL) )
	    {
		    cgGLEnableProfile( pNiXSIShader->Get_eProfile() );	
		    cgGLBindProgram( pInstanceData->m_Program[i] );	
		    Cg_GetContext()->m_currentProgram = pInstanceData->m_Program[i];

		    if((bRecompiled) || (in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_TREE_CHANGE))
		    {
                GamebryoMaterial_InitXSIParams_v2(
                    pInstanceData,
                    (CRef *) in_pSysParams->m_pShaderHandle);
    
			    // clear the set we use to track where the SIVIEW params are stored in CG
			    ClearRowMajorParameters();

			    // scan the semantics data
			    XSI_RTS_InitializeSemantics_inuse(pInstanceData->m_Semantics);

			    CGparameter l_Parameter = cgGetFirstParameter(pInstanceData->m_Program[i], CG_PROGRAM);
			    while(l_Parameter)
			    {
				    const char* l_pSemantic = cgGetParameterSemantic(l_Parameter);
				    if (l_pSemantic == NULL || strlen(l_pSemantic) == 0)
				    {
					    l_pSemantic = cgGetParameterName(l_Parameter);
				    }
    				
				    if (Use50Semantics()
                    &&  (stricmp(l_pSemantic, "siview")   == 0
                    ||   stricmp(l_pSemantic, "siviewi")  == 0
                    ||   stricmp(l_pSemantic, "view")     == 0
                    ||   stricmp(l_pSemantic, "viewi")    == 0
                    ||   stricmp(l_pSemantic, "sieyepos") == 0))
				    {
					    AddRowMajorParameter(l_Parameter);
				    }

				    XSI_RTS_RegisterSemantics(
                        Cg_GetSemantics(),
                        pInstanceData->m_Semantics,
                        (char *)l_pSemantic,
                        l_Parameter);
				    l_Parameter = cgGetNextParameter(l_Parameter);
			    }
		    }
	    }
	    else if( pNiXSIShader->Get_bActive() && (pInstanceData->m_Program[i] == NULL))
	    {
		    Cg_GetContext()->m_currentProgram = NULL;
	    }
        
        // OpenGL bit *************************************

	    // Compute the model, view and projection matrices and push it to the semantics
	    glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) Cg_GetSemantics().m_Proj );

	    //// Bind all semantics now
	    XSI_RTS_BindCommonSemantics(Cg_GetSemantics(), 
								    pInstanceData->m_Semantics,
								    in_pSysParams,
								    pInstanceData,
								    Cg_BindSemanticCallback);

	    // Push default uniforms
        CGprogram prog = Cg_GetContext()->m_currentProgram;
	    if( prog )
	    {
		    pInstanceData->siviewportsize = cgGetNamedParameter(prog, "siviewportsize");

		    for (int k = 0; k < CG_NB_LIGHTS; k++)
		    {
			    pInstanceData->silight[k].silighttype        = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silighttype[k]);
			    pInstanceData->silight[k].silightambient     = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightambient[k]);
			    pInstanceData->silight[k].silightdiffuse     = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightdiffuse[k]);
			    pInstanceData->silight[k].silightspecular    = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightspecular[k]);
			    pInstanceData->silight[k].silightdirection   = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightdirection[k]);
			    pInstanceData->silight[k].silightposition    = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightposition[k]);
			    pInstanceData->silight[k].silightfalloff     = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightfalloff[k]);
			    pInstanceData->silight[k].silightattenuation = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightattenuation[k]);
			    pInstanceData->silight[k].silightcone        = cgGetNamedParameter(
                    prog, NiXSIStdMaterial::m_silightcone[k]);
		    }

		    // push viewport size as float4v
		    float l_fvViewport[4];		    
		    glGetFloatv ( GL_VIEWPORT, l_fvViewport );
		    if(pInstanceData->siviewportsize)
			    cgGLSetParameter4fv(pInstanceData->siviewportsize, l_fvViewport);

            // Bind lights
            if (!NiXSIStdMaterial::BindLights(in_pSysParams, in_pParams,
                    in_pGeom, io_pShaderData, pNiXSIShader->Get_eShaderType()))
            {
                    return false;
            }
            
            // Bind Matrices & Textures 
            if ( pNiXSIShader->Get_eShaderType() == NI_SHADER_VERTEX ) // vertex program
            {
                // Bind Matrices
                if (!NiXSIStdMaterial::BindMatrices(in_pSysParams, in_pParams,
                        in_pGeom, io_pShaderData, pNiXSIShader->Get_eShaderType()))
                {
                    return false;
                }
            }

            // Bind (active) Texture
            const bool applyOpenGL = true;
            const bool applyCg = true;
            if (!BindTexture((niMaterialMap)l_pPPG->ziPreviewTexture, in_pSysParams,
                    in_pParams, in_pGeom, io_pShaderData, pNiXSIShader->Get_eShaderType(),
                    applyOpenGL, applyCg))
            {
                return false;
            }
        
            // Bind shading parameters
            if (pNiXSIShader->Get_eShaderType() == NI_SHADER_FRAGMENT) // vertex program
            {
                // ambient
                cgGLSetParameter3fv(cgGetNamedParameter(prog, "cKa"), &l_pPPG->cKa.m_red); 
                // diffuse
                cgGLSetParameter3fv(cgGetNamedParameter(prog, "cKd"), &l_pPPG->cKd.m_red); 
                // specular
                cgGLSetParameter3fv(cgGetNamedParameter(prog, "cKs"), &l_pPPG->cKs.m_red); 
                // emmitance
                cgGLSetParameter3fv(cgGetNamedParameter(prog, "cKe"), &l_pPPG->cKe.m_red); 

                cgGLSetParameter1f(cgGetNamedParameter(prog, "fShininess"), l_pPPG->Shininess);
                cgGLSetParameter1f(cgGetNamedParameter(prog, "fAlpha"),     l_pPPG->Alpha);
            }
            
            // Everything else ---------------------------------------------------
 
		    bool	bAllParamsAreGood = false;
		    while( !bAllParamsAreGood )
		    {
			    CTime	time;

			    double frame = time.GetFrameRate() * in_pSysParams->m_SceneData->m_fCurrentTime;
			    bAllParamsAreGood = true;

			    for(loop = 0; loop < pInstanceData->m_lNbXSIParam; loop ++)
			    {
                    CgParameterCacheEntry* pCacheEntry = pInstanceData->m_pXSIParams[loop];
				    if(pCacheEntry->m_XSIParameter[0].IsValid())
				    {
					    float value = 0.0f;
                        XSI::CValue xsiValue = pCacheEntry->m_XSIParameter[0].GetValue(frame);
                        CValue::DataType dataType = pCacheEntry->m_XSIParameter[0].GetValueType();
					    if(dataType == siFloat)
					    {
						    value = xsiValue;
					    }
					    else if(dataType == siDouble)
					    {
						    value = (float) ((double) xsiValue);
					    }
					    else if(dataType == siInt4)
					    {
						    value = (float) ((LONG) xsiValue);
					    }
					    else if(dataType == siBool)
					    {
						    value = (float) ((bool) xsiValue);
					    }

					    cgGLSetParameter1f(pCacheEntry->m_CgParameter, value);
				    }
				    else
				    {
					    bAllParamsAreGood = false;
                        GamebryoMaterial_InitXSIParams_v2(
                            pInstanceData,
                            (CRef *) in_pSysParams->m_pShaderHandle);
					    break;
				    }
			    }
		    } 
   
	    }

    } // end of shader loop ***************************************************    


	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Utility functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Utility function to extract flag into descriptor.
bool NiXSIStdMaterial::IsMapEnabled( NiStandardMaterial::TextureMap eMap, 
                                       NiStandardMaterialDescriptor* pkDescriptor )
{
    switch (eMap)
    {
        case NiStandardMaterial::MAP_MAX: 
        default:
            NIASSERT(!"Should not get here!");
            return false;
        case NiStandardMaterial::MAP_DECAL02: 
            return (pkDescriptor->GetDECALMAPCOUNT() > 2);
        case NiStandardMaterial::MAP_DECAL01: 
            return (pkDescriptor->GetDECALMAPCOUNT() > 1);
        case NiStandardMaterial::MAP_DECAL00: 
            return (pkDescriptor->GetDECALMAPCOUNT() > 0);
        case NiStandardMaterial::MAP_CUSTOM04: 
            return pkDescriptor->GetCUSTOMMAP04COUNT() == 1;
        case NiStandardMaterial::MAP_CUSTOM03:
            return pkDescriptor->GetCUSTOMMAP03COUNT() == 1;
        case NiStandardMaterial::MAP_CUSTOM02: 
            return pkDescriptor->GetCUSTOMMAP02COUNT() == 1;
        case NiStandardMaterial::MAP_CUSTOM01: 
            return pkDescriptor->GetCUSTOMMAP01COUNT() == 1;
        case NiStandardMaterial::MAP_CUSTOM00: 
            return pkDescriptor->GetCUSTOMMAP00COUNT() == 1;
        case NiStandardMaterial::MAP_GLOW: 
            return pkDescriptor->GetGLOWMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_GLOSS: 
            return pkDescriptor->GetGLOSSMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_BUMP: 
            return pkDescriptor->GetBUMPMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_DETAIL:
            return pkDescriptor->GetDETAILMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_DARK: 
            return pkDescriptor->GetDARKMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_NORMAL: 
            return pkDescriptor->GetNORMALMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_BASE: 
            return pkDescriptor->GetBASEMAPCOUNT() == 1;
        case NiStandardMaterial::MAP_PARALLAX: 
            return pkDescriptor->GetPARALLAXMAPCOUNT() == 1;
    }
}

// Utility function to extract flag into descriptor.
bool NiXSIStdMaterial::SetMapEnabled( NiStandardMaterial::TextureMap eMap, 
                                        bool bEnabled, 
                                        NiStandardMaterialDescriptor* pkDescriptor )
{
    if (IsMapEnabled(eMap, pkDescriptor) == bEnabled)
        return true;

    unsigned int uiNewState = 1;
    if (!bEnabled)
        uiNewState = 0;

    switch (eMap)
    {
        case NiStandardMaterial::MAP_MAX: 
        default:
            NIASSERT(!"Should not get here!");
            return false;
            break;
        case NiStandardMaterial::MAP_DECAL02: 
        case NiStandardMaterial::MAP_DECAL01: 
        case NiStandardMaterial::MAP_DECAL00: 
            {
                unsigned int uiDecalCount = pkDescriptor->GetDECALMAPCOUNT();
                if (bEnabled)
                    uiDecalCount++;
                else
                    uiDecalCount--;
                
                pkDescriptor->SetDECALMAPCOUNT(uiDecalCount);
            }
            break;
        case NiStandardMaterial::MAP_CUSTOM04: 
            pkDescriptor->SetCUSTOMMAP04COUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_CUSTOM03:
            pkDescriptor->SetCUSTOMMAP03COUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_CUSTOM02: 
            pkDescriptor->SetCUSTOMMAP02COUNT(uiNewState);
            break;
        case NiStandardMaterial::MAP_CUSTOM01: 
            pkDescriptor->SetCUSTOMMAP01COUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_CUSTOM00: 
            pkDescriptor->SetCUSTOMMAP00COUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_GLOW: 
            pkDescriptor->SetGLOWMAPCOUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_GLOSS: 
            pkDescriptor->SetGLOSSMAPCOUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_BUMP: 
            pkDescriptor->SetBUMPMAPCOUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_DETAIL:
            pkDescriptor->SetDETAILMAPCOUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_DARK: 
            pkDescriptor->SetDARKMAPCOUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_NORMAL: 
            pkDescriptor->SetNORMALMAPCOUNT(uiNewState); 
            break;
        case NiStandardMaterial::MAP_BASE: 
            pkDescriptor->SetBASEMAPCOUNT(uiNewState);  
            break;
        case NiStandardMaterial::MAP_PARALLAX: 
            pkDescriptor->SetPARALLAXMAPCOUNT(uiNewState);  
            break;
    }

    return true;
}


//---------------------------------------------------------------------------
// XSI Parameters
//---------------------------------------------------------------------------

void GamebryoMaterial_DisposeXSIParams_v2( GamebryoMaterialInstanceData* in_pInstanceData )
{
	CRef	releaseRef;

	for( int i=0 ; i<in_pInstanceData->m_lNbXSIParam ; i++ )
	{
		delete in_pInstanceData->m_pXSIParams[i];
	}

	free( in_pInstanceData->m_pXSIParams );

	in_pInstanceData->m_lNbXSIParam     = 0;
	in_pInstanceData->m_pXSIParams      = NULL;
}

void GamebryoMaterial_InitXSIParams_v2(
    GamebryoMaterialInstanceData * in_pInstanceData,
    CRef * in_pShaderHandle)
{
	Shader    shader     = *((CRef *) in_pShaderHandle);
	CRefArray properties = shader.GetProperties();

    GamebryoMaterial_DisposeXSIParams_v2(in_pInstanceData);
	int	count = properties.GetCount();
	for (int j = 0; j < count; j++)
	{
		Property property = properties[j];
		CParameterRefArray	params = property.GetParameters();

		int paramCount = params.GetCount();		
		for (int i = 0; i < paramCount; i++)
		{
			Parameter parameter = params[i];

			wchar_t			unicode_bigname[2048];
			char			ansi_param[2048];

#ifdef UNIX
		    swprintf(unicode_bigname, sizeof(unicode_bigname), L"%S_%S",
                property.GetName().GetWideString(), parameter.GetName().GetWideString());
#else
		    swprintf(unicode_bigname, L"%s_%s",
                property.GetName().GetWideString(), parameter.GetName().GetWideString());
#endif
			wcstombs(ansi_param, unicode_bigname, 2048);

            CGprogram prog = Cg_GetContext()->m_currentProgram;
			CGparameter cgParam = cgGetNamedParameter(prog, ansi_param);
			if(cgParam)
			{
                in_pInstanceData->m_pXSIParams = (CgParameterCacheEntry **)
                    realloc(in_pInstanceData->m_pXSIParams,
                    sizeof(CgParameterCacheEntry *) * (in_pInstanceData->m_lNbXSIParam + 1));

                CgParameterCacheEntry** pEntries = in_pInstanceData->m_pXSIParams;

                pEntries[in_pInstanceData->m_lNbXSIParam] = new CgParameterCacheEntry();
                pEntries[in_pInstanceData->m_lNbXSIParam]->m_CgParameter = cgParam;
                pEntries[in_pInstanceData->m_lNbXSIParam]->m_XSIParameter[0] = parameter;

                in_pInstanceData->m_lNbXSIParam ++;
			}
		}
	}
}
