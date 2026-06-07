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

#include <GamebryoShaders.h>

#include <xsi_graphicsequencer.h>
#include <xsi_graphicsequencercontext.h>
using namespace XSI;

#include "NiXSIShader.h"    // for NiXSIShader & NiXSIStdMaterial
#include "NiXSIMaps.h"      // for maps enums
#include "NiXSIPPG.h"       // for prperty page definitions
  

#ifdef linux
# define stricmp(a,b) strcasecmp(a,b)
#endif



// Structure to keep temp texture parameters
// >> Later, we may want to add this to the above-defined PPG
#pragma pack(push, 1)
typedef struct tagTexturePPG
{	
    LONG            target;
	LONG			projection;
	LONG			refmap;
	LONG			wrapS;
	LONG			wrapT;
	LONG			minification;

    // Parameters from OGL13Texture   
	LONG			minFilter;
	float			lodBias;
	float			anisotropy;
	LONG			compression;
	LONG			format;
} TexturePPG;
#pragma pack(pop)
static TexturePPG s_options_ppg[SI_NUM_MATERIALMAPS];


//---------------------------------------------------------------------------
// XSI Parameters
//---------------------------------------------------------------------------

void GamebryoMaterial_DisposeXSIParams( GamebryoMaterialInstanceData* in_pInstanceData )
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

void GamebryoMaterial_InitXSIParams( GamebryoMaterialInstanceData *in_pInstanceData, 
                                    CRef	*in_pShaderHandle )
{
	Shader		shader = *((CRef *) in_pShaderHandle);
	CRefArray	properties = shader.GetProperties();

    GamebryoMaterial_DisposeXSIParams(in_pInstanceData);
	int	count = properties.GetCount();

	for( int j=0 ; j<count ; j++ )
	{
		Property property = properties[j];
		CParameterRefArray	params = property.GetParameters();

		int paramCount = params.GetCount();		

		for( int i=0 ; i<paramCount ; i++ )
		{
			Parameter parameter = params[i];

			wchar_t			unicode_bigname[2048];
			char			ansi_param[2048];

#ifdef UNIX
		swprintf(unicode_bigname, 
            sizeof(unicode_bigname),
            L"%S_%S", 
            property.GetName().GetWideString(), 
            parameter.GetName().GetWideString());
#else
		swprintf(unicode_bigname, 
            L"%s_%s", 
            property.GetName().GetWideString(), 
            parameter.GetName().GetWideString());
#endif
			wcstombs(ansi_param, unicode_bigname, 2048);

			CGparameter cgParam = cgGetNamedParameter(Cg_GetContext()->m_currentProgram, ansi_param);
			if(cgParam)
			{
				in_pInstanceData->m_pXSIParams = 
                    (CgParameterCacheEntry **) realloc(in_pInstanceData->m_pXSIParams, 
					sizeof(CgParameterCacheEntry *) * (in_pInstanceData->m_lNbXSIParam + 1));

				in_pInstanceData->m_pXSIParams[in_pInstanceData->m_lNbXSIParam] = 
                    new CgParameterCacheEntry();
				in_pInstanceData->m_pXSIParams[in_pInstanceData->m_lNbXSIParam]->m_CgParameter = 
                    cgParam;
				in_pInstanceData->m_pXSIParams[in_pInstanceData->m_lNbXSIParam]->m_XSIParameter[0] = 
                    parameter;

				in_pInstanceData->m_lNbXSIParam ++;
			}
		}
	}
}

 
//---------------------------------------------------------------------------
// GamebryoMaterial_Init
// Initialization callback.
//---------------------------------------------------------------------------

RTSHADER_API bool GamebryoMaterial_Init( XSI_RTS_SystemParams* in_pSysParams, 
                                          void* in_pParams, void** io_pShaderInstanceData )
{
    // Create and initialize instance data members ----------------------------
	GamebryoMaterialInstanceData* pInstanceData = new GamebryoMaterialInstanceData();
	pInstanceData->m_Context        = cgCreateContext();
	pInstanceData->m_Program[0]     = NULL;
    pInstanceData->m_Program[1]     = NULL;
	*io_pShaderInstanceData         = pInstanceData;
	pInstanceData->m_lNbXSIParam    = 0;
	pInstanceData->m_pXSIParams     = NULL;
	pInstanceData->siviewportsize   = NULL;
	pInstanceData->m_pProgramString = NULL;
	for( int i=0 ; i<CG_NB_LIGHTS ; i++ )
	{
		pInstanceData->silight[i].silighttype        = NULL;
		pInstanceData->silight[i].silightambient     = NULL;
		pInstanceData->silight[i].silightdiffuse     = NULL;
		pInstanceData->silight[i].silightspecular    = NULL;
		pInstanceData->silight[i].silightposition    = NULL;
		pInstanceData->silight[i].silightdirection   = NULL;
		pInstanceData->silight[i].silightfalloff     = NULL;
		pInstanceData->silight[i].silightattenuation = NULL;
		pInstanceData->silight[i].silightcone        = NULL;
	}

    // Maps
    for( int i=0 ; i<SI_NUM_MATERIALMAPS ; i++ )
	{
		pInstanceData->m_Handle[i] = -1;
		pInstanceData->m_MipMap[i] = 0;

        // Initialize s_options_ppg array             
        s_options_ppg[i].target         = 0;
        s_options_ppg[i].projection     = -1;
        s_options_ppg[i].refmap         = 0;
        s_options_ppg[i].wrapS          = 1;
        s_options_ppg[i].wrapT          = 1;
      
        s_options_ppg[i].minification   = 1;
	    s_options_ppg[i].minFilter      = 1;
	    s_options_ppg[i].lodBias        = 0;
	    s_options_ppg[i].anisotropy     = 1;	    
	    s_options_ppg[i].compression    = 1;
	    s_options_ppg[i].format         = 0;
	}

    // Load initial SHADER SET
    pInstanceData->m_XSIStdMaterial.Set_ShaderSet( NI_SHADERSET_01_PHONG ); //basic phong, no maps
    pInstanceData->m_XSIStdMaterial.Gamebryo_Init();
   

	return true;
}

//---------------------------------------------------------------------------
// GamebryoMaterial_Term
// Termination callback.
//---------------------------------------------------------------------------

RTSHADER_API bool GamebryoMaterial_Term( XSI_RTS_SystemParams* in_pSysParams, 
                                         void* in_pParams, void** io_pShaderInstanceData )
{
	GamebryoMaterialInstanceData* pInstanceData = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;

	if ( pInstanceData != NULL )
	{

		if ( pInstanceData->m_Context != NULL )
		{
			cgDestroyContext(pInstanceData->m_Context);
		}

		if ( pInstanceData->m_pProgramString )
			free( pInstanceData->m_pProgramString );

        GamebryoMaterial_DisposeXSIParams( pInstanceData );
    
        // Maps 
        for( int i=0 ; i<SI_NUM_MATERIALMAPS ; i++ )
		{
			if ( pInstanceData->m_Handle[i] != -1 )
			{
				::glDeleteTextures( 1, (const unsigned int*)&pInstanceData->m_Handle[i] );
			}
		}
                
        // Terminate shader manager
        pInstanceData->m_XSIStdMaterial.Terminate();
        pInstanceData->m_XSIStdMaterial.Gamebryo_Shutdown();

		// Instance data itself
		delete pInstanceData;
		Cg_GetContext()->m_currentProgram = NULL;
	}

	return true;
}

//---------------------------------------------------------------------------
// GamebryoMaterial_Execute
// Execution callback.
//---------------------------------------------------------------------------

enum niExecMode
{
    NI_EXECMODE_EMBEDED,                // 0
    NI_EXECMODE_CG,                     // 1
    NI_EXECMODE_OPENGL,                 // 2

    NI_NUM_EXECMODES                    // 3 **** NUM ****
};

RTSHADER_API bool GamebryoMaterial_Execute( XSI_RTS_SystemParams* in_pSysParams, 
                                            void* in_pParams,					    
	                                        XSI_RTS_TriangulatedGeometry* in_pGeom, 
                                            void** io_pShaderInstanceData )
{    
    GamebryoMaterialPPG*            l_pPPG          = (GamebryoMaterialPPG*)in_pParams;		
    niMaterialMap                   eMap            = (niMaterialMap)l_pPPG->ziPreviewTexture;
    niExecMode                      eExecMode       = NI_EXECMODE_OPENGL;
  //niExecMode                      eExecMode       = NI_EXECMODE_EMBEDED; 
    GamebryoMaterialInstanceData*   pInstanceData   = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;
    

    // Choose between Cg or direct OpenGL texturing
    switch ( eExecMode )
    {
        case NI_EXECMODE_EMBEDED:
        {
            if ( !pInstanceData->m_XSIStdMaterial.Execute_Embeded(
                in_pSysParams,in_pParams,in_pGeom,io_pShaderInstanceData) )
            {
                return false;
            }

            break;
        }
        case NI_EXECMODE_CG:
        {
            if ( !NiXSIStdMaterial::Execute_Cg(
                in_pSysParams,in_pParams,in_pGeom,io_pShaderInstanceData) )
            {
                return false;
            }

            break;
        }
        case NI_EXECMODE_OPENGL: // direct OpenGL texturing
        {
            if ( !NiXSIStdMaterial::Execute_Direct(
                in_pSysParams,in_pParams,in_pGeom,io_pShaderInstanceData) )
            {
                return false;
            }

            // Apply texture coordinates
            if ( l_pPPG->Tex[eMap].TexCoord_Enabled )
            {
                if ( !NiXSIStdMaterial::TexCoord_GL(in_pSysParams,
                                                   in_pParams,
                                                   in_pGeom,
                                                   io_pShaderInstanceData,
                                                   0,    //! target=0, 
                                                   0,    //! uvtype="Explicit",
                                                   0) )  //! nbComponents="1D" (unused parameter)
                    return false;        
            }

            // Shade with material color
            if ( !NiXSIStdMaterial::Shade_GL(
                in_pSysParams,in_pParams,in_pGeom,io_pShaderInstanceData,true) ) //bLighting
            {
                return false;
            }

            break;
        }
    }
   
    // Draw XSI geometry
    NiXSIStdMaterial::Draw_GL( in_pSysParams,				    //! The system parameters
                               in_pParams,					//! The property page parameters
                               in_pGeom,					//! The geometry information
                               io_pShaderInstanceData, 	    //! The shader instance data
                               0,                           // in_nBlending,                
                               1,                           // in_nBlendFuncSrc,            
                               0,                           // in_nBlendFuncDst,            
                               0,                           // in_nCullingMode,             
                               1,                           // in_nDepthTest,               
                               1,                           // in_nDepthWrite,              
                               1 );                         // in_nReset 

    return true;
}

//---------------------------------------------------------------------------
// Execute_Cg
// Execution callback.
//---------------------------------------------------------------------------

bool NiXSIStdMaterial::Execute_Cg(XSI_RTS_SystemParams* in_pSysParams, 
                                  void* in_pParams,					    
                                  XSI_RTS_TriangulatedGeometry* in_pGeom, 
                                  void** io_pShaderInstanceData )
{
	GamebryoMaterialInstanceData* pInstanceData = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;
	GamebryoMaterialPPG*          l_pPPG = (GamebryoMaterialPPG*) in_pParams;	
	int						        loop;

    // Change Shader Set - if required
    bool            bChangedShaderSet   = false;
    niShaderSet     eShaderSet          = pInstanceData->m_XSIStdMaterial.Get_eShaderSet();
    niMaterialMap   eMap                = (niMaterialMap)l_pPPG->ziPreviewTexture;
    bool            bActiveTexEnabled   = (l_pPPG->Tex[eMap].Map_Enabled)? true : false;
    if ( (bActiveTexEnabled==FALSE) && 
        (eShaderSet!=NI_SHADERSET_01_PHONG) ) // shader PPG checbox not enabled...
    {
        pInstanceData->m_XSIStdMaterial.Set_ShaderSet( NI_SHADERSET_01_PHONG ); // no textures
        bChangedShaderSet = true;        
    }    
    if ( (bActiveTexEnabled==TRUE) && 
        (eShaderSet!=NI_SHADERSET_02_DIFFUSEMAP1) ) // not already running shader set #2...
    {
        pInstanceData->m_XSIStdMaterial.Set_ShaderSet( NI_SHADERSET_02_DIFFUSEMAP1 ); // textured
        bChangedShaderSet = true;        
    }
    // If shader changed, delete buffered program string
    if ( bChangedShaderSet==true && pInstanceData->m_pProgramString )
    {
	    free( pInstanceData->m_pProgramString );
        pInstanceData->m_pProgramString = NULL; // reset - Important!
    }
    
    // Execute for each shader program...
    for ( int i=0 ; i<pInstanceData->m_XSIStdMaterial.Get_nShaders() ; i++ )
    {
        NiXSIShader* pNiXSIShader = pInstanceData->m_XSIStdMaterial.Get_pShader( i );
        if ( pNiXSIShader == NULL ) // precaution...
            continue;

        // Start of shader processing...
        //************************************************************
                        
	    CRef					shader = *( (CRef *) in_pSysParams->m_pShaderHandle);
    
	    bool					bRecompiled = false;

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
	                    pInstanceData->m_Program[i] = 
                            cgCreateProgramFromFile( Cg_GetContext()->m_Context, CG_SOURCE, 
                                                    pNiXSIShader->Get_szCodeFile().c_str(), 
                                                    pNiXSIShader->Get_eProfile(), "main", 0 );
                    }
                    else if ( !pNiXSIShader->Get_szCodeSource().empty() ) // from source
                    {
				        pInstanceData->m_Program[i] = 
                            cgCreateProgram( Cg_GetContext()->m_Context, CG_SOURCE, 
                                            pNiXSIShader->Get_szCodeSource().c_str(), 
                                            pNiXSIShader->Get_eProfile(), "main", NULL );
                    }

                    // Save original Cg source program
                    if ( cgIsProgramCompiled(pInstanceData->m_Program[i]) )
                        pNiXSIShader->Set_szCodeSource( 
                        cgGetProgramString(
                        pInstanceData->m_Program[i],CG_PROGRAM_SOURCE) );                     

                    // Show debug information
				    Logf(siInfoMsg, "GamebryoFX: %s\n",
                        cgIsProgramCompiled(pInstanceData->m_Program[i]) ? 
                        "Program is compiled" : "Program is not compiled");
				    Logf(siInfoMsg, "GamebryoFX: LAST LISTING----%s----\n",
                        cgGetLastListing(Cg_GetContext()->m_Context));
				    Logf(siInfoMsg, "GamebryoFX: ---- PROGRAM BEGIN ----\n%s---- PROGRAM END ----\n",
                        cgGetProgramString(pInstanceData->m_Program[i], CG_COMPILED_PROGRAM));
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
                GamebryoMaterial_InitXSIParams(pInstanceData, 
                    (CRef *) in_pSysParams->m_pShaderHandle);
    
			    // clear the set we use to track where the SIVIEW params are stored in CG
			    ClearRowMajorParameters();

			    // scan the semantics data
			    XSI_RTS_InitializeSemantics_inuse(pInstanceData->m_Semantics);

			    CGparameter l_Parameter = cgGetFirstParameter(pInstanceData->m_Program[i], CG_PROGRAM);
			    while(l_Parameter)
			    {
				    char *l_pSemantic = (char *) cgGetParameterSemantic(l_Parameter);
				    if((l_pSemantic == NULL) || (strlen(l_pSemantic) == 0))
				    {
					    l_pSemantic = (char *) cgGetParameterName(l_Parameter);
				    }
    				
				    if (   (Use50Semantics()) && 
					       (   ( stricmp(l_pSemantic, "siview") == 0 )
						     || ( stricmp(l_pSemantic, "siviewi") == 0 )
						     || ( stricmp(l_pSemantic, "view") == 0 )
						     || ( stricmp(l_pSemantic, "viewi") == 0 ) 
						     || ( stricmp(l_pSemantic, "sieyepos") == 0 ) 
					       ) 
				       )
				    {
					    AddRowMajorParameter(l_Parameter);
				    }

				    XSI_RTS_RegisterSemantics(Cg_GetSemantics(), 
                        pInstanceData->m_Semantics, 
                        l_pSemantic, l_Parameter);
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
	    if( Cg_GetContext()->m_currentProgram )
	    {
		    pInstanceData->siviewportsize = cgGetNamedParameter(
                Cg_GetContext()->m_currentProgram, "siviewportsize");

		    for( int k=0 ; k<CG_NB_LIGHTS ; k++ )
		    {
			    pInstanceData->silight[k].silighttype        = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silighttype[k]);
			    pInstanceData->silight[k].silightambient     = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightambient[k]);
			    pInstanceData->silight[k].silightdiffuse     = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightdiffuse[k]);
			    pInstanceData->silight[k].silightspecular    = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightspecular[k]);
			    pInstanceData->silight[k].silightdirection   = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightdirection[k]);
			    pInstanceData->silight[k].silightposition    = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightposition[k]);
			    pInstanceData->silight[k].silightfalloff     = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightfalloff[k]);
			    pInstanceData->silight[k].silightattenuation = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightattenuation[k]);
			    pInstanceData->silight[k].silightcone        = 
                    cgGetNamedParameter(
                    Cg_GetContext()->m_currentProgram, NiXSIStdMaterial::m_silightcone[k]);
		    }

		    // push viewport size as float4v
		    float				l_fvViewport[4];		    
		    glGetFloatv ( GL_VIEWPORT, l_fvViewport );
		    if(pInstanceData->siviewportsize)
			    cgGLSetParameter4fv(pInstanceData->siviewportsize, (const float *) l_fvViewport);

            // Bind lights
            if ( !NiXSIStdMaterial::BindLights(in_pSysParams,
                                            in_pParams,
                                            in_pGeom,
                                            io_pShaderInstanceData,
                                            pNiXSIShader->Get_eShaderType()) )
                    return false;                     	                    
            
            // Bind Matrices & Textures 
            if ( pNiXSIShader->Get_eShaderType() == NI_SHADER_VERTEX ) // vertex program
            {
                // Bind Matrices
                if ( !NiXSIStdMaterial::BindMatrices(in_pSysParams,
                                        in_pParams,
                                        in_pGeom,
                                        io_pShaderInstanceData,
                                        pNiXSIShader->Get_eShaderType() ) )
                    return false;                     	                                       
            }

            // Bind (active) Texture
            if ( !BindTexture(   (niMaterialMap)l_pPPG->ziPreviewTexture, // active texture
                                                    in_pSysParams,
                                                    in_pParams,
                                                    in_pGeom,
                                                    io_pShaderInstanceData,
                                                    pNiXSIShader->Get_eShaderType(),
                                                    true,    // bApplyOpenGL,   
                                                    true) )  // bApplyCg        
                return false;                     	               
        
            // Bind shading parameters
            if ( pNiXSIShader->Get_eShaderType() == NI_SHADER_FRAGMENT ) // vertex program
            {
                // Bind the color        
                cgGLSetParameter3fv( 
                    cgGetNamedParameter(Cg_GetContext()->m_currentProgram, "cKa"), 
                    (const float*) &l_pPPG->cKa ); // ambient
                cgGLSetParameter3fv( 
                    cgGetNamedParameter(Cg_GetContext()->m_currentProgram, "cKd"), 
                    (const float*) &l_pPPG->cKd ); // diffuse   
                cgGLSetParameter3fv( 
                    cgGetNamedParameter(Cg_GetContext()->m_currentProgram, "cKs"), 
                    (const float*) &l_pPPG->cKs ); // specular
                cgGLSetParameter3fv( 
                    cgGetNamedParameter(Cg_GetContext()->m_currentProgram, "cKe"), 
                    (const float*) &l_pPPG->cKe ); // emmitance

                // Bind shininess + alpha
                cgGLSetParameter1f( 
                    cgGetNamedParameter(Cg_GetContext()->m_currentProgram, "fShininess"),   
                    l_pPPG->Shininess );   // shininess                               
                cgGLSetParameter1f( 
                    cgGetNamedParameter(Cg_GetContext()->m_currentProgram, "fAlpha"),
                    l_pPPG->Alpha );       // alpha                
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
				    if(pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].IsValid())
				    {
					    float value = 0.0f;

					    int type = pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].GetValueType();

					    if(pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].GetValueType() == siFloat)
					    {
						    value = pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].GetValue(frame);
					    }
					    else if(pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].GetValueType() == siDouble)
					    {
						    value = (float) ((double) pInstanceData->
                                m_pXSIParams[loop]->m_XSIParameter[0].GetValue(frame));
					    }
					    else if(pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].GetValueType() == siInt4)
					    {
						    value = (float) ((LONG) pInstanceData->
                                m_pXSIParams[loop]->m_XSIParameter[0].GetValue(frame));
					    }
					    else if(pInstanceData->m_pXSIParams[loop]->m_XSIParameter[0].GetValueType() == siBool)
					    {
						    value = (float) ((bool) pInstanceData->
                                m_pXSIParams[loop]->m_XSIParameter[0].GetValue(frame));
					    }

					    cgGLSetParameter1f(pInstanceData->m_pXSIParams[loop]->m_CgParameter, value);
				    }
				    else
				    {
					    bAllParamsAreGood = false;
                        GamebryoMaterial_InitXSIParams(
                            pInstanceData, (CRef *) in_pSysParams->m_pShaderHandle);
					    break;
				    }
			    }
		    } 
   
	    }

    } // end of shader loop ***************************************************    


	return true;
}

//---------------------------------------------------------------------------
// Execute_Direct
// Execution callback.
//---------------------------------------------------------------------------

bool NiXSIStdMaterial::Execute_Direct( XSI_RTS_SystemParams* in_pSysParams, 
                                        void* in_pParams,					
	                                    XSI_RTS_TriangulatedGeometry* in_pGeom, 
                                        void** io_pShaderInstanceData )
{
	GamebryoMaterialInstanceData*   pInstanceData       = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;
	GamebryoMaterialPPG*            l_pPPG              = (GamebryoMaterialPPG*) in_pParams;	
	assert( pInstanceData != NULL );
    niMaterialMap                   eMap                = (niMaterialMap)l_pPPG->ziPreviewTexture;
    bool bActiveTexEnabled   = (l_pPPG->Tex[eMap].Map_Enabled)? true : false;

    // Stop here if map is not enabled
    if ( !bActiveTexEnabled )
        return true;

	if ( in_pSysParams->m_pGLExt->_GL_ARB_multitexture )
	{
		in_pSysParams->m_pGLExtFuncs->glActiveTextureARB(
            s_options_ppg[eMap].target + GL_TEXTURE0_ARB );
		in_pSysParams->m_pGLExtFuncs->glClientActiveTextureARB(
            s_options_ppg[eMap].target + GL_TEXTURE0_ARB );
	}

	int nbTextureUnits;
	::glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &nbTextureUnits );


	if( s_options_ppg[eMap].target < nbTextureUnits )
	{
		::glEnable( GL_TEXTURE_2D );
	}
                                                    
	if(	(pInstanceData->m_Handle[eMap] == -1) || (l_pPPG->Map_Image[eMap].m_lDirty) || 
		(pInstanceData->m_MipMap[eMap] != s_options_ppg[eMap].minFilter) || 
		(pInstanceData->m_Compression[eMap] != s_options_ppg[eMap].compression) )
	{
		pInstanceData->m_MipMap[eMap]           = s_options_ppg[eMap].minFilter;
		pInstanceData->m_Compression[eMap]      = s_options_ppg[eMap].compression;
		static int CheckCompressionExtension    = -1;

		if( s_options_ppg[eMap].compression )
		{   
			CheckCompressionExtension = in_pSysParams->m_pGLExt->_GL_ARB_texture_compression;
		}
		else
		{
			CheckCompressionExtension = 0;
		}

		int	numberOfChannels;
		
		if( CheckCompressionExtension )
		{
			numberOfChannels = l_pPPG->Map_Image[eMap].m_lNbChannels == 4 ? 
                GL_COMPRESSED_RGBA_ARB : GL_COMPRESSED_RGB_ARB;	
		}
		else
		{
			numberOfChannels = l_pPPG->Map_Image[eMap].m_lNbChannels;	
		}


		GLenum			format = s_options_ppg[eMap].format;
		int				nbChannels;
		unsigned char	*newBuffer = NULL;

		if( (s_options_ppg[eMap].format > 0) && (!in_pSysParams->m_pGLExt->_GL_NV_texture_shader))
			format = 0;

		switch(format)
		{
			case 0 :	format = (l_pPPG->Map_Image[eMap].m_lNbChannels == 3) ? GL_RGB : GL_RGBA;
						nbChannels = l_pPPG->Map_Image[eMap].m_lNbChannels;
						break;
			case 1 :	format = GL_DSDT_NV;
						nbChannels = GL_DSDT_NV;
						break;
			case 2 :	format = GL_DSDT_NV;
						nbChannels = GL_DSDT_NV;
						break;

		}
		
		// Delete the current handle if needed
		if(pInstanceData->m_Handle[eMap] != -1)
		{
			::glDeleteTextures(1, (unsigned int *) &pInstanceData->m_Handle[eMap]);
		}

		int	l_iDataType = GL_UNSIGNED_BYTE;
		int l_iTextureTarget = GL_TEXTURE_2D;

		if ( l_pPPG->Map_Image[eMap].m_lPixelType == 64 )
		{
			//
			// check if the texture dimensions are power of 2
			//

			ULONG lNewHeight, lNewWidth;
			lNewWidth = XSI_OGLHelper_NearestPowerOf2(l_pPPG->Map_Image[eMap].m_lWidth);
			lNewHeight = XSI_OGLHelper_NearestPowerOf2(l_pPPG->Map_Image[eMap].m_lHeight);	

			if ( ( lNewWidth != l_pPPG->Map_Image[eMap].m_lWidth ) || 
                ( lNewHeight != l_pPPG->Map_Image[eMap].m_lHeight ))
			{
				l_iTextureTarget = GL_TEXTURE_RECTANGLE_NV;	// same value for ATI
			}

			//
			// check for floating point texture support
			//
			if ( in_pSysParams->m_pGLExt->_GL_ATI_texture_float )
			{
				format = GL_RGBA_FLOAT32_ATI;
				l_iDataType = GL_FLOAT;
			}

			if ( in_pSysParams->m_pGLExt->_GL_NV_float_buffer )
			{
				format = GL_FLOAT_RGB32_NV;
				l_iDataType = GL_FLOAT;
			}
		}

		// Create a new handle
		::glGenTextures(1, (unsigned int *) &pInstanceData->m_Handle[eMap]);

		// Set the current texture to this one
		::glBindTexture(l_iTextureTarget, pInstanceData->m_Handle[eMap]);

		// Load the texture 
		if(( s_options_ppg[eMap].minFilter >= 2 ) && ( l_pPPG->Map_Image[eMap].m_lPixelType != 64))
		{
			::gluBuild2DMipmaps(l_iTextureTarget, numberOfChannels, 
				l_pPPG->Map_Image[eMap].m_lWidth, 
                l_pPPG->Map_Image[eMap].m_lHeight, 
                format, 
                l_iDataType, 
                l_pPPG->Map_Image[eMap].m_pBuffer);
		}
		else
		{
			ULONG lNewHeight, lNewWidth;
			lNewWidth = XSI_OGLHelper_NearestPowerOf2(l_pPPG->Map_Image[eMap].m_lWidth);
			lNewHeight = XSI_OGLHelper_NearestPowerOf2(l_pPPG->Map_Image[eMap].m_lHeight);	
			BYTE *buffer = l_pPPG->Map_Image[eMap].m_pBuffer;
	
			//check if we need to resize
			BYTE * pScaledTex = NULL;
			if(((lNewWidth != l_pPPG->Map_Image[eMap].m_lWidth) || 
                (lNewHeight != l_pPPG->Map_Image[eMap].m_lHeight)) && 
                ( l_iDataType != GL_FLOAT))
			{
				long size = l_iDataType == GL_UNSIGNED_BYTE ? sizeof(BYTE) : sizeof(FLOAT);
				pScaledTex = (BYTE*)malloc(
                    lNewWidth * lNewHeight * l_pPPG->Map_Image[eMap].m_lNbChannels * size);
				assert(	pScaledTex != NULL );

				::gluScaleImage(format, 
								(GLint)l_pPPG->Map_Image[eMap].m_lWidth, 
								(GLint)l_pPPG->Map_Image[eMap].m_lHeight, 
								l_iDataType, 
								l_pPPG->Map_Image[eMap].m_pBuffer, 
								(GLint)lNewWidth, 
								(GLint)lNewHeight, 
								l_iDataType, 
								pScaledTex);

				buffer = pScaledTex;
			} 
            else 
            {
				//
				// for floating point textures we do not resize
				//

				lNewWidth = l_pPPG->Map_Image[eMap].m_lWidth;
				lNewHeight = l_pPPG->Map_Image[eMap].m_lHeight;	
			}

			if((s_options_ppg[eMap].format == 1) || (s_options_ppg[eMap].format == 2))
			{
				char	*pixelComponent = (char *)buffer ;
				int		loop, endloop = l_pPPG->Map_Image[eMap].m_lWidth * l_pPPG->Map_Image[eMap].m_lHeight;

				newBuffer = (unsigned char *) malloc(endloop * 2);
				unsigned char *tmpBuffer = newBuffer;
				buffer = newBuffer;

				if(s_options_ppg[eMap].format == 1)
				{
					for(loop = 0; loop < endloop; loop ++)
					{
						*tmpBuffer = *pixelComponent;
						tmpBuffer ++; pixelComponent ++;
						*tmpBuffer = *pixelComponent;
						tmpBuffer ++; pixelComponent ++;
						pixelComponent ++;

						if(l_pPPG->Map_Image[eMap].m_lNbChannels == 4)
							pixelComponent ++;
					}
				}

				if(s_options_ppg[eMap].format == 2)
				{
					for(loop = 0; loop < endloop; loop ++)
					{
						*tmpBuffer = *pixelComponent;
						tmpBuffer ++; pixelComponent ++;
						pixelComponent ++;
						*tmpBuffer = *pixelComponent;
						tmpBuffer ++; pixelComponent ++;

						if(l_pPPG->Map_Image[eMap].m_lNbChannels == 4)
							pixelComponent ++;
					}
				}
			}			

			::glTexImage2D(	l_iTextureTarget, 
							0, 
							nbChannels,
							(GLint)lNewWidth,
							(GLint)lNewHeight,
							0,
							format,
							l_iDataType,
							buffer);

			if(pScaledTex != NULL)
				free(pScaledTex);

			if(newBuffer != NULL)
				free(newBuffer);
		}			

	
	}
	else
	{
		// Set the current texture to this one
		::glBindTexture( GL_TEXTURE_2D, pInstanceData->m_Handle[eMap] );
	}

	// disable all existing texgens
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_TEXTURE_GEN_R );
	glDisable( GL_TEXTURE_GEN_Q );

	// ApplyMode (Modulation)
    // Gamebryo parameters: "Replace"   = 0
	//                      "Decal"     = 1
	//                      "Modulate"  = 2
	switch( l_pPPG->Tex[eMap].Map_ApplyMode )
	{
        case 0:	
            ::glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );	
            break;
        case 1:	
            ::glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );	
            break;
		case 2:	
            ::glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	
            break;
	}

	// MAG filtering    
    // Gamebryo parameters:
    //      "Nearest"   = 0
	//      "Linear"    = 1
	//      "Trilinear" = 2 (mip-map interpolation)
	switch( l_pPPG->Tex[eMap].Map_ApplyMode )
	{	
		case 0: 
            ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_NEAREST ); 
            break;
		case 1: 
        case 2: 
            ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_LINEAR ); 
            break;
	}

	if ( l_pPPG->Map_Image[eMap].m_lPixelType == 64)
	{
		// nVidia and ATI floating point textures do not support mipmaps or filtering
		// on most of their chipsets.
		// Here we force filtering to nearest for all floating point textures
		//
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST ); 
	} 
    else 
    {	 
        // MIN filtering
        // Gamebryo parameters:
        //      "Nearest"   = 0
	    //      "Linear"    = 1
	    //      "Trilinear" = 2 (mip-map interpolation)
		switch( l_pPPG->Tex[eMap].Map_ApplyMode )
		{	
			case 0: 
                ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST ); 
                break;
			case 1: 
                ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_LINEAR ); 
                break;
			case 2: 
                ::glTexParameteri( GL_TEXTURE_2D, 
                    GL_TEXTURE_MIN_FILTER , 
                    ((l_pPPG->Tex[eMap].Map_MipMapEnabled)? GL_LINEAR : GL_LINEAR) );
                break;
		}
	}

	// Lod bias
	if( in_pSysParams->m_pGLExt->_GL_EXT_texture_lod_bias )
	{
		::glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, 
            GL_TEXTURE_LOD_BIAS_EXT, 
            s_options_ppg[eMap].lodBias);
	}	

	// Anisotropic level
	if ( in_pSysParams->m_pGLExt->_GL_EXT_texture_filter_anisotropic )
	{
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, s_options_ppg[eMap].anisotropy);
	}

	// Map our texture coordinates
	XSI_OGLHelper_GetTextureCoordMap()->target[XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs] = 
        GL_TEXTURE0_ARB + s_options_ppg[eMap].target;
	XSI_OGLHelper_GetTextureCoordMap()->UVSet[XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs] = 
        l_pPPG->Tex[eMap].Map_TexSpace;
	XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs ++;
   

	return TRUE;
}

//---------------------------------------------------------------------------
// GamebryoMaterial_GetRequirements
// Requirement callback.
//---------------------------------------------------------------------------

RTSHADER_API bool GamebryoMaterial_GetRequirements( XSI_RTS_SystemParams* in_pSysParams, 
                                                    void* in_pParams,					
	                                                XSI_RTS_TriangulatedGeometry* in_pGeom, 
                                                    void** io_pShaderInstanceData,		
	                                                ULONG in_lRequirement, 
                                                    ULONG* out_pRequirementValue	)
{

    return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Utility binding functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool NiXSIStdMaterial::BindMatrices( XSI_RTS_SystemParams* in_pSysParams, 
                                    void* in_pParams,					
	                                XSI_RTS_TriangulatedGeometry* in_pGeom,						
	                                void** io_pShaderInstanceData, 
                                    niShaderType eShaderType )
{
    GamebryoMaterialInstanceData*     pInstanceData   = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;
	GamebryoMaterialPPG*              l_pPPG          = (GamebryoMaterialPPG*) in_pParams;		

    // Bind the proper matrix entries
    // 0 = Combined projection, view and world
    // 1 = Combined view and world
    // 2 = Projection
    // 3 = View 
    // 4 = World
    // 5 = Manual Entry 
    //
    LONG		ziMatrix[7]     ={  4,          // mModel
                                    3,          // mView,
                                    2,          // mProjection,
                                    0,          // mModelViewProj,
                                    1,          // mModelView
                                    1,          // mModelViewI 
                                    1 };        // mModelViewIT 
    std::string szMatrix[7]     ={  "mModel",
                                    "mView",            
                                    "mProjection",
                                    "mModelViewProj",
                                    "mModelView",
                                    "mModelViewI",
                                    "mModelViewIT" };
    bool		bInvert[7]      ={  false,      // mModel
                                    false,      // mView,
                                    false,      // mProjection,
                                    false,      // mModelViewProj,
                                    false,      // mModelView
                                    true,       // mModelViewI
                                    true };     // mModelViewIT 
    bool		bTranspose[7]   ={  false,      // mModel
                                    false,      // mView,
                                    false,      // mProjection,
                                    false,      // mModelViewProj,
                                    false,      // mModelView
                                    false,      // mModelViewI
                                    true };     // mModelViewIT 
    for ( int j=0 ; j<7 ; j++ )
    {
        CgMatrixBind	l_BindData;
        l_BindData.variable	= 
            cgGetNamedParameter( Cg_GetContext()->m_currentProgram, szMatrix[j].c_str() );
        if(l_BindData.variable)
        {
            l_BindData.cgType			= 15;   // Float4x4 (zero-based)
            l_BindData.matrix			= ziMatrix[j];
            l_BindData.transpose		= bTranspose[j];  // no transpose
            l_BindData.invert			= bInvert[j];     // no inversion
            l_BindData.manual_matrix	= NULL; // no manual matrix

            Cg_BindMatrix(&l_BindData);
        }
    }    

    return true;
}

bool NiXSIStdMaterial::BindLights
(
	XSI_RTS_SystemParams			*in_pSysParams,					//! system parameters
	void							*in_pParams,					//! property page parameters
	XSI_RTS_TriangulatedGeometry	*in_pGeom,						//! geometry information
	void							**io_pShaderInstanceData,		//! shader instance data
    niShaderType                    eShaderType                     //! vertex or fragment program
)
{
    GamebryoMaterialInstanceData*     pInstanceData   = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;
	GamebryoMaterialPPG*              l_pPPG          = 
        (GamebryoMaterialPPG*) in_pParams;		
	
    CgLightTrackerBind	l_lightBind;
    for( int k=0 ; k<CG_NB_LIGHTS ; k++ )
    {
	    bool l_bStop = true;
	    if(pInstanceData->silight[k].silighttype)
	    {
		    l_lightBind.Type = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Type = pInstanceData->silight[k].silighttype;
	    }
	    else
	    {
		    l_lightBind.Type = false;
	    }

	    if(pInstanceData->silight[k].silightambient)
	    {
		    l_lightBind.Ambient = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Ambient = pInstanceData->silight[k].silightambient;
	    }
	    else
	    {
		    l_lightBind.Ambient = false;
	    }

	    if(pInstanceData->silight[k].silightdiffuse)
	    {
		    l_lightBind.Diffuse = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Diffuse = pInstanceData->silight[k].silightdiffuse;
	    }
	    else
	    {
		    l_lightBind.Diffuse = false;
	    }

	    if(pInstanceData->silight[k].silightspecular)
	    {
		    l_lightBind.Specular = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Specular = pInstanceData->silight[k].silightspecular;
	    }
	    else
	    {
		    l_lightBind.Specular = false;
	    }

	    if(pInstanceData->silight[k].silightdirection)
	    {
		    l_lightBind.Direction = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Direction = pInstanceData->silight[k].silightdirection;
	    }
	    else
	    {
		    l_lightBind.Direction = false;
	    }

	    if(pInstanceData->silight[k].silightposition)
	    {
		    l_lightBind.Position = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Position = pInstanceData->silight[k].silightposition;
	    }
	    else
	    {
		    l_lightBind.Position    = false;
	    }

	    if(pInstanceData->silight[k].silightfalloff)
	    {
		    l_lightBind.Falloff     = true;
		    l_bStop                 = false;
		    l_lightBind.Constant_Register_Falloff = pInstanceData->silight[k].silightfalloff;
	    }
	    else
	    {
		    l_lightBind.Falloff     = false;
	    }

	    if(pInstanceData->silight[k].silightattenuation)
	    {
		    l_lightBind.Attenuation = true;
		    l_bStop                 = false;
		    l_lightBind.Constant_Register_Attenuation = pInstanceData->silight[k].silightattenuation;
	    }
	    else
	    {
		    l_lightBind.Attenuation = false;
	    }

	    if(pInstanceData->silight[k].silightcone)
	    {
		    l_lightBind.Cone = true;
		    l_bStop = false;
		    l_lightBind.Constant_Register_Cone = pInstanceData->silight[k].silightcone;
	    }
	    else
	    {
		    l_lightBind.Cone = false;
	    }

	    if(l_bStop)
		    break;

	    l_lightBind.LightID = k;

	    Cg_BindLight(&l_lightBind);
    }

    return true;
}

static std::string szMapParamNames_bOn[SI_NUM_MATERIALMAPS] ={
    "bDiffusemap",            // SI_MATERIALMAP_BASE,                            // 0         
    "bDarkmap",               // SI_MATERIALMAP_DARK,                            // 1
    "bDetailmap",             // SI_MATERIALMAP_DETAIL,                          // 2
    "bDecalmap_01",           // SI_MATERIALMAP_DECAL_01,                        // 3
    "bDecalmap_02",           // SI_MATERIALMAP_DECAL_02,                        // 4
    "bDecalmap_03",           // SI_MATERIALMAP_DECAL_03,                        // 5
    "bNormalmap",             // SI_MATERIALMAP_NORMAL,                          // 6
    "bParallaxmap",           // SI_MATERIALMAP_PARALLAX,                        // 7
    "bEnvironmentmap",        // SI_MATERIALMAP_ENVIRONMENT,                     // 8
    "bBumpmap",               // SI_MATERIALMAP_BUMP,                            // 9
    "bGlossmap",              // SI_MATERIALMAP_GLOSS,                           // 10
    "bProjectedlightmap_01",  // SI_MATERIALMAP_PROJECTEDLIGHT_01,               // 11
    "bProjectedlightmap_02",  // SI_MATERIALMAP_PROJECTEDLIGHT_02,               // 12
    "bProjectedlightmap_03",  // SI_MATERIALMAP_PROJECTEDLIGHT_03,               // 13
    "bProjectedshadowmap_01", // SI_MATERIALMAP_PROJECTEDSHADOW_01,              // 14
    "bProjectedshadowmap_02", // SI_MATERIALMAP_PROJECTEDSHADOW_02,              // 15
    "bProjectedshadowmap_03", // SI_MATERIALMAP_PROJECTEDSHADOW_03,              // 16   
    "bGlowmap"                // SI_MATERIALMAP_GLOW,                            // 17 
                                                            };
static std::string szMapParamNames[SI_NUM_MATERIALMAPS] = {   
    "diffusemap",             // SI_MATERIALMAP_BASE,                            // 0         
    "diffusemap",             // SI_MATERIALMAP_DARK,                            // 1
    "diffusemap",             // SI_MATERIALMAP_DETAIL,                          // 2
    "diffusemap",             // SI_MATERIALMAP_DECAL_01,                        // 3
    "diffusemap",             // SI_MATERIALMAP_DECAL_02,                        // 4
    "diffusemap",             // SI_MATERIALMAP_DECAL_03,                        // 5
    "normalmap",              // SI_MATERIALMAP_NORMAL,                          // 6
    "normalmap",              // SI_MATERIALMAP_PARALLAX,                        // 7
    "normalmap",              // SI_MATERIALMAP_ENVIRONMENT,                     // 8
    "normalmap",              // SI_MATERIALMAP_BUMP,                            // 9
    "diffusemap",             // SI_MATERIALMAP_GLOSS,                           // 10
    "diffusemap",             // SI_MATERIALMAP_PROJECTEDLIGHT_01,               // 11
    "diffusemap",             // SI_MATERIALMAP_PROJECTEDLIGHT_02,               // 12
    "diffusemap",             // SI_MATERIALMAP_PROJECTEDLIGHT_03,               // 13
    "diffusemap",             // SI_MATERIALMAP_PROJECTEDSHADOW_01,              // 14
    "diffusemap",             // SI_MATERIALMAP_PROJECTEDSHADOW_02,              // 15
    "diffusemap",             // SI_MATERIALMAP_PROJECTEDSHADOW_03,              // 16   
    "diffusemap"              // SI_MATERIALMAP_GLOW,                            // 17 
                                                            };
bool NiXSIStdMaterial::BindTexture( niMaterialMap eMap, 
                                    XSI_RTS_SystemParams* in_pSysParams,					
	                                  void* in_pParams, 
                                      XSI_RTS_TriangulatedGeometry* in_pGeom,						
	                                  void** io_pShaderInstanceData, 
                                      niShaderType eShaderType,                    
                                      bool bApplyOpenGL, 
                                      bool bApplyCg )
{
    GamebryoMaterialInstanceData*     pInstanceData   = 
        (GamebryoMaterialInstanceData*) *io_pShaderInstanceData;
	GamebryoMaterialPPG*              l_pPPG          = 
        (GamebryoMaterialPPG*) in_pParams;		
	int                                 ziTexUnit       = 0; //OLD:eMap;
    
    // Presence flag
    bool bOn = (l_pPPG->Tex[eMap].Map_Enabled==TRUE && 
        l_pPPG->Map_Image[eMap].m_lBufferSize>0)? true : false;
    cgGLSetParameter1f( cgGetNamedParameter(
        Cg_GetContext()->m_currentProgram, szMapParamNames_bOn[eMap].c_str()), bOn );
              
    // Skip disabled maps
	if( !bOn ) // no error...
		return true;
                      
    //------------------------------------------------
    // OpenGL stuff
    //------------------------------------------------

    if ( bApplyOpenGL )
    {
	    if ( in_pSysParams->m_pGLExt->_GL_ARB_multitexture )
	    {
		    in_pSysParams->m_pGLExtFuncs->glActiveTextureARB(GL_TEXTURE0_ARB + ziTexUnit);
		    in_pSysParams->m_pGLExtFuncs->glClientActiveTextureARB(GL_TEXTURE0_ARB + ziTexUnit);
	    }

	    ::glEnable( GL_TEXTURE_2D );

	    if ( (pInstanceData->m_Handle[eMap] == -1) || 
            (l_pPPG->Map_Image[eMap].m_lDirty))
	    {
		    static int CheckCompressionExtension = -1;

		    if(CheckCompressionExtension == -1)
		    {   
			    if (strstr((char*)glGetString(GL_EXTENSIONS), "GL_ARB_texture_compression") != NULL)
			    {
				    if(getenv("XSI_RTS_USE_TEXTURE_COMPRESSION"))
					    CheckCompressionExtension = 1;
				    else
					    CheckCompressionExtension = 0;
			    }
			    else
			    {
				    CheckCompressionExtension = 0;
			    }
		    }

		    int	numberOfChannels;
			
		    if(CheckCompressionExtension)
		    {
			    numberOfChannels = l_pPPG->Map_Image[eMap].m_lNbChannels == 4 ? 
                    GL_COMPRESSED_RGBA_ARB : GL_COMPRESSED_RGB_ARB;	
		    }
		    else
		    {
			    numberOfChannels = l_pPPG->Map_Image[eMap].m_lNbChannels;	
		    }

		    GLenum format=3;
		    if( l_pPPG->Map_Image[eMap].m_lNbChannels == 4 )
			    format = GL_RGBA;
		    else if( l_pPPG->Map_Image[eMap].m_lNbChannels == 3 )
			    format = GL_RGB;
			
		    // Delete the current handle if needed
		    if( pInstanceData->m_Handle[eMap] != -1 )
		    {
			    ::glDeleteTextures(1, (unsigned int *) &pInstanceData->m_Handle[eMap]);
		    }

		    // Create a new handle
		    ::glGenTextures( 1, (unsigned int *) &pInstanceData->m_Handle[eMap] );

		    //Set the current texture to this one
		    ::glBindTexture( GL_TEXTURE_2D, pInstanceData->m_Handle[eMap] );

		    // Load the texture 
		    if( s_options_ppg[eMap].minification >= 2 )
		    {
			    ::gluBuild2DMipmaps( GL_TEXTURE_2D, numberOfChannels, 
				                     l_pPPG->Map_Image[eMap].m_lWidth, 
                                     l_pPPG->Map_Image[eMap].m_lHeight, 
                                     format, 
                                     GL_UNSIGNED_BYTE, 
                                     l_pPPG->Map_Image[eMap].m_pBuffer );
		    }
		    else
		    {
			    ULONG   lNewHeight, lNewWidth;
			    BYTE*   pScaledTex = NULL;

			    lNewWidth   = XSI_OGLHelper_NearestPowerOf2(l_pPPG->Map_Image[eMap].m_lWidth);
			    lNewHeight  = XSI_OGLHelper_NearestPowerOf2(l_pPPG->Map_Image[eMap].m_lHeight);	
		
			    //check if we need to resize
			    if( (lNewWidth != l_pPPG->Map_Image[eMap].m_lWidth) || 
                    (lNewHeight != l_pPPG->Map_Image[eMap].m_lHeight) )
			    {
				    pScaledTex = (BYTE*)malloc(lNewWidth * lNewHeight * 
                        l_pPPG->Map_Image[eMap].m_lNbChannels * sizeof(BYTE));
				    assert(	pScaledTex != NULL );

				    ::gluScaleImage(format, 
								    (GLint)l_pPPG->Map_Image[eMap].m_lWidth, 
								    (GLint)l_pPPG->Map_Image[eMap].m_lHeight, 
								    GL_UNSIGNED_BYTE, 
								    l_pPPG->Map_Image[eMap].m_pBuffer, 
								    (GLint)lNewWidth, 
								    (GLint)lNewHeight, 
								    GL_UNSIGNED_BYTE, 
								    pScaledTex);

				    ::glTexImage2D(	GL_TEXTURE_2D, 
								    0, 
								    numberOfChannels,
								    lNewWidth,
								    lNewHeight,
								    0,
								    format,
								    GL_UNSIGNED_BYTE,
								    pScaledTex);

				    if(pScaledTex != NULL)	
					    free(pScaledTex);
			    }
			    else
			    {
				    ::glTexImage2D(	GL_TEXTURE_2D, 
								    0, 
								    numberOfChannels,
								    (GLint)l_pPPG->Map_Image[eMap].m_lWidth,
								    (GLint)l_pPPG->Map_Image[eMap].m_lHeight,
								    0,
								    format,
								    GL_UNSIGNED_BYTE,
								    l_pPPG->Map_Image[eMap].m_pBuffer);
			    }
		    }			

		    pInstanceData->m_MipMap[eMap] = 0; //OLD: s_options_ppg[eMap].minification;
		
	    }
	    else
	    {
		    // Set the current texture to this one
		    ::glBindTexture( GL_TEXTURE_2D, pInstanceData->m_Handle[eMap] );
	    }

	    // Set the texture parameters now
        // ApplyMode (Modulation)
        // Gamebryo parameters: "Replace"   = 0
	    //                      "Decal"     = 1
	    //                      "Modulate"  = 2
	    switch( l_pPPG->Tex[eMap].Map_ApplyMode )
	    {
            case 0:	
                ::glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );	
                break;
            case 1:	
                ::glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );	
                break;
		    case 2:	
                ::glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	
                break;
	    }

	    // Wrap in S direction / T Direction  (*Note might change)
	    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 
            s_options_ppg[eMap].wrapS ? GL_REPEAT : GL_CLAMP);
	    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 
            s_options_ppg[eMap].wrapT ? GL_REPEAT : GL_CLAMP);

	    // Set quality
        // Gamebryo parameters:
        //      "Nearest"   = 0
	    //      "Linear"    = 1
	    //      "Trilinear" = 2 (mip-map interpolation)	    
	    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
            l_pPPG->Tex[eMap].Map_ApplyMode ? GL_LINEAR : GL_NEAREST );

	    switch( l_pPPG->Tex[eMap].Map_ApplyMode )
	    {
		    case 0 : 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
                break;
		    case 1 : 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
                break;
		    case 2 : 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                    (l_pPPG->Tex[eMap].Map_MipMapEnabled)? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR ); 
                break;
	    };

	    if( s_options_ppg[eMap].refmap )
	    {
		    GLfloat params[1] = { GL_SPHERE_MAP };

		    ::glTexGenfv( GL_S, GL_TEXTURE_GEN_MODE, params );
		    ::glTexGenfv( GL_T, GL_TEXTURE_GEN_MODE, params );
		    ::glEnable( GL_TEXTURE_GEN_S );
		    ::glEnable( GL_TEXTURE_GEN_T );
	    }
	    else
	    {
		    ::glDisable( GL_TEXTURE_GEN_S );
		    ::glDisable( GL_TEXTURE_GEN_T );
	    }

	    ::glDisable( GL_TEXTURE_GEN_R );
	    ::glDisable( GL_TEXTURE_GEN_Q );

	    // Map our texture coordinates

        // target
	    if( in_pSysParams->m_pGLExt->_GL_ARB_multitexture )
	    {
		    XSI_OGLHelper_GetTextureCoordMap()->
                target[XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs] = 
                GL_TEXTURE0_ARB + ziTexUnit;
	    }
	    else
	    {
		    XSI_OGLHelper_GetTextureCoordMap()->
                target[XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs] = 
                GL_TEXTURE;
	    }
        // UVSet
        XSI_OGLHelper_GetTextureCoordMap()->
            UVSet[XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs] = 
            l_pPPG->Tex[eMap].Map_TexSpace; //s_options_ppg[eMap].projection;
	    XSI_OGLHelper_GetTextureCoordMap()->nbActiveUVs++;
    }

    //------------------------------------------------
    // Cg stuff
    //------------------------------------------------

    if ( bApplyCg )        
    {
        // Presence flag
        CGparameter nParam = cgGetNamedParameter( 
            Cg_GetContext()->m_currentProgram, szMapParamNames[eMap].c_str() );
        if ( nParam )       
        {                   
            // associate the parameter handles for sampler with OpenGL texture objects
            cgGLSetTextureParameter( nParam, eMap );
        
            // Enable texture parameters
            // Note: cgGLEnableTextureParameter calls glActiveTexture and glBindTexture to 
            //       bind the correct texture object 
            //      (specified earlier with cgGLSetTextureParameter) 
            //       into the compiled fragment program’s appropriate texture unit.
            cgGLEnableTextureParameter( nParam );                
        }              
    } // end of Cg part


	return true;
}

//---------------------------------------------------------------------------
// Apply texture coordinates
//---------------------------------------------------------------------------

bool NiXSIStdMaterial::TexCoord_GL( XSI_RTS_SystemParams* in_pSysParams, void* in_pParams,				
	                                  XSI_RTS_TriangulatedGeometry* 
                                      in_pGeom,	void** io_pShaderInstanceData,   
                                      LONG target,          //! texture target: 0 for now...
	                                  LONG uvtype,          
	                                  LONG nbComponents )   //! 1D=0, 2D=1, 3D=2, 4D=3

{
    GamebryoMaterialPPG*    l_pPPG              = (GamebryoMaterialPPG*) in_pParams;   
    niMaterialMap           eMap                = (niMaterialMap)l_pPPG->ziPreviewTexture;
    bool                    bActiveTexEnabled   = (l_pPPG->Tex[eMap].Map_Enabled)? true : false;
	

    // Check if TexCoordEnable flag is ON
    if ( !l_pPPG->Tex[eMap].TexCoord_Enabled )
        return true; // no error...

	in_pSysParams->m_pGLExtFuncs->glActiveTextureARB( GL_TEXTURE0_ARB + target );
	in_pSysParams->m_pGLExtFuncs->glClientActiveTextureARB( GL_TEXTURE0_ARB + target );

	// generate UV if needed
	switch( uvtype )
	{
		case 0: // Nothing to do
			break;
		case 1:	
            ::glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			::glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			::glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			::glTexGenf( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			break;
		case 2: 
            ::glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
			::glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
			::glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
			::glTexGenf( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
			break;
		case 3: 
            ::glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
			::glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
			break;
		case 4: if(in_pSysParams->m_pGLExt->_GL_NV_texgen_reflection)
		    {
			    ::glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV );
			    ::glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV );
			    ::glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV );
		    }
		    break;
		case 5: if(in_pSysParams->m_pGLExt->_GL_NV_texgen_reflection)
		    {
			    ::glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_NV );
			    ::glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_NV );
			    ::glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_NV );
		    }
		    break;
	}

	
	// set what texture coord will go through
	if( uvtype > 0 )
	{
		::glEnable( GL_TEXTURE_GEN_S );
		::glDisable( GL_TEXTURE_GEN_T );
		::glDisable( GL_TEXTURE_GEN_R );
		::glDisable( GL_TEXTURE_GEN_Q );

		if( nbComponents > 0 )
		{
			::glEnable( GL_TEXTURE_GEN_T );
		}
		if( nbComponents > 1 )
		{
			::glEnable( GL_TEXTURE_GEN_R );
		}
		if( nbComponents > 2 )
		{
			::glEnable( GL_TEXTURE_GEN_Q );
		}

	}
	else
	{
		::glDisable( GL_TEXTURE_GEN_S );
		::glDisable( GL_TEXTURE_GEN_T );
		::glDisable( GL_TEXTURE_GEN_R );
		::glDisable( GL_TEXTURE_GEN_Q );
	}

	int		TextureTarget;

	if( ::glIsEnabled(GL_TEXTURE_CUBE_MAP_ARB) )
	{
		TextureTarget = GL_TEXTURE_CUBE_MAP_ARB;
	}
	else
	{
		TextureTarget = GL_TEXTURE_2D;
	}
	// set wrap modes
	switch( l_pPPG->Tex[eMap].TexCoord_uwrap )
	{
		case 0:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT); 
            break;
		case 1:		
            if(in_pSysParams->m_pGLExt->_GL_ARB_texture_mirrored_repeat)
			{
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); 
			}
			else if(in_pSysParams->m_pGLExt->_GL_IBM_texture_mirrored_repeat)
			{
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_IBM); 
			}
			break;
		case 2:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP); 
            break;
		case 3:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
			break;
		case 4:	
            if(in_pSysParams->m_pGLExt->_GL_ATI_texture_mirror_once)
			    ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_ATI); 
            break;
		case 5:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
            break;
	}

	switch( l_pPPG->Tex[eMap].TexCoord_vwrap )
	{
		case 0:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT); 
            break;
		case 1:		
            if(in_pSysParams->m_pGLExt->_GL_ARB_texture_mirrored_repeat)
			{
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); 
			}
			else if(in_pSysParams->m_pGLExt->_GL_IBM_texture_mirrored_repeat)
			{
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_IBM); 
			}
			break;
		case 2:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP); 
            break;
		case 3:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
			break;
		case 4:		
            if(in_pSysParams->m_pGLExt->_GL_ATI_texture_mirror_once)
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_ATI); 
            break;
		case 5:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
            break;
	}

	switch( l_pPPG->Tex[eMap].TexCoord_wwrap )
	{
		case 0:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT); 
            break;
		case 1:		
            if(in_pSysParams->m_pGLExt->_GL_ARB_texture_mirrored_repeat)
			{
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT_ARB); 
			}
			else if(in_pSysParams->m_pGLExt->_GL_IBM_texture_mirrored_repeat)
			{
				::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT_IBM); 
			}
			break;
		case 2:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP); 
            break;
		case 3:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER); 
			break;
		case 4:		
            if( in_pSysParams->m_pGLExt->_GL_ATI_texture_mirror_once )
			    ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_R, GL_MIRROR_CLAMP_ATI); 
            break;
		case 5:		
            ::glTexParameteri(TextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
            break;
	}

	// set transforms now
	if ( l_pPPG->Tex[eMap].TexCoord_transform )
	{
		float	fmTransforms[16];

		::glMatrixMode( GL_TEXTURE );
		XSI_OGLHelper_ComputeTransforms(fmTransforms, 
			                            l_pPPG->Tex[eMap].TexCoord_scalex, 
                                        l_pPPG->Tex[eMap].TexCoord_scaley, 
                                        l_pPPG->Tex[eMap].TexCoord_scalez,
			                            l_pPPG->Tex[eMap].TexCoord_rotx,   
                                        l_pPPG->Tex[eMap].TexCoord_roty,   
                                        l_pPPG->Tex[eMap].TexCoord_rotz, 
			                            l_pPPG->Tex[eMap].TexCoord_transx, 
                                        l_pPPG->Tex[eMap].TexCoord_transy, 
                                        l_pPPG->Tex[eMap].TexCoord_transz );
		::glLoadMatrixf( fmTransforms );
		::glMatrixMode( GL_MODELVIEW );
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// Shade with material color.
//---------------------------------------------------------------------------

bool NiXSIStdMaterial::Shade_GL( XSI_RTS_SystemParams* in_pSysParams, void* in_pParams,					
	                               XSI_RTS_TriangulatedGeometry* in_pGeom, 
                                   void** io_pShaderInstanceData, 
                                   bool bLighting )
{
	GamebryoMaterialPPG*    l_pPPG              = (GamebryoMaterialPPG*) in_pParams;   
    niMaterialMap           eMap                = (niMaterialMap)l_pPPG->ziPreviewTexture;
    bool                    bActiveTexEnabled   = (l_pPPG->Tex[eMap].Map_Enabled)? true : false;

	::glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,      (float*)&l_pPPG->cKa );
	::glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,      (float*)&l_pPPG->cKd );
	::glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,     (float*)&l_pPPG->cKs );
	::glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,     (float*)&l_pPPG->cKe );
	::glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,     l_pPPG->Shininess );

	if( bLighting )
		::glEnable( GL_LIGHTING );
	else
	{
		::glDisable( GL_LIGHTING );
		::glColor4fv( (float*)&l_pPPG->cKd );
	}


	// needed because we don't want to have visual regressions with old scenes
	::glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR );


	return true;
}

//---------------------------------------------------------------------------
// Draw XSI geometry with OpenGL.
//---------------------------------------------------------------------------

bool NiXSIStdMaterial::Draw_GL( XSI_RTS_SystemParams* in_pSysParams, void* in_pParams,				
	                              XSI_RTS_TriangulatedGeometry* in_pGeom, 
                                  void** io_pShaderInstanceData,	
                                  LONG in_nBlending,               // 0
	                              LONG in_nBlendFuncSrc,           // 1
	                              LONG in_nBlendFuncDst,           // 0
	                              LONG in_nCullingMode,            // 0
	                              LONG in_nDepthTest,              // 1
	                              LONG in_nDepthWrite,             // 1
	                              LONG in_nReset )                 // 1

{
	bool			        result  = TRUE;
	GamebryoMaterialPPG*  l_pPPG  = (GamebryoMaterialPPG*) in_pParams;   

	int				OGLBlendFuncSrc[9] = 
	{
		GL_ZERO,
		GL_ONE,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_SRC_ALPHA_SATURATE
	};

	int				OGLBlendFuncDst[8] = 
	{
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
	};

    

    
	// Set the blending
	if(in_nBlending)
	{
		::glEnable(GL_BLEND);
		::glBlendFunc(OGLBlendFuncSrc[in_nBlendFuncSrc], OGLBlendFuncDst[in_nBlendFuncDst]);
	}
	else
	{
		::glDisable(GL_BLEND);
	}

	// Set the culling
	if(in_nCullingMode > 0)
	{
		::glEnable( GL_CULL_FACE );
		if(in_nCullingMode == 1)
		{
			::glCullFace(GL_BACK);
		}
		else if(in_nCullingMode == 2)
		{
			::glCullFace(GL_FRONT);
		}
		else if(in_nCullingMode == 3)
		{
			::glCullFace(GL_FRONT_AND_BACK);
		}
	}
	else
	{
		::glDisable(GL_CULL_FACE);
	}

	// Set the depth test
	if (in_nDepthTest)
	{
		::glEnable( GL_DEPTH_TEST );
	}
	else
	{
		::glDisable( GL_DEPTH_TEST );
	}

	if(in_nDepthWrite)
	{
		::glDepthMask ( GL_TRUE );
	}
	else
	{
		::glDepthMask ( GL_FALSE );
	}



	// Draw the geometry
	result = Gamebryo_XSI_OGLHelper_DrawGeometry(in_pSysParams->m_pGLExt,
										in_pSysParams->m_pGLExtFuncs,
										in_pGeom,
										1, 
										1,
										1,
										XSI_OGLHelper_GetTextureCoordMap(),
										XSI_OGLHelper_GetVertexColorMap());


	if(in_nReset)
	{
		//********************************************************************
		// Reset all states
		// This is slow but it is safe
		//********************************************************************
		XSI_OGLHelper_ResetTextureCoordMap();

		int loop;

		if(in_pSysParams->m_pGLExt->_GL_ARB_multitexture)
		{
			int nbTextureUnits;

			if(in_pSysParams->m_pGLExt->_GL_ARB_fragment_program)
			{
				::glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &nbTextureUnits);
			} 
			else if(in_pSysParams->m_pGLExt->_GL_ARB_multitexture)
			{
				::glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &nbTextureUnits);
			}

			for(loop = 0; loop < nbTextureUnits; loop++)
			{
				in_pSysParams->m_pGLExtFuncs->glActiveTextureARB(GL_TEXTURE0_ARB + loop);
				in_pSysParams->m_pGLExtFuncs->glClientActiveTextureARB(GL_TEXTURE0_ARB + loop);
				::glMatrixMode(GL_TEXTURE);
				::glLoadIdentity();
				::glMatrixMode(GL_MODELVIEW);
			}
		}
		else
		{
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();
			::glMatrixMode(GL_MODELVIEW);
		}

		glPopAttrib();
		glPopClientAttrib();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		// We don't have the same define on all platforms/machines.
#ifdef GL_CLIENT_ALL_ATTRIB_BITS
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
#else
		glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);
#endif

	}


	return result;
}

//===========================================================================
// End Of File - GamebryoMaterial.cpp


