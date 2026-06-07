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


#ifndef NI_XSI_STD_MATERIAL_H
#define NI_XSI_STD_MATERIAL_H

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "NiXSIMaps.h"              // for niMaterialMap enum
#include "NiXSIShader.h"            // for niShaderSet

#include "xsi_rtshaders.h"          // Softimage - for XSI_RTS_FXSemantics_inuse

#include <NiPoint3.h>
#include <NiMesh.h>
#include <NiTStridedRandomAccessIterator.h>
#include <NiStandardMaterial.h>     // for NiStandardMaterial
#include "NiMaterialNodeLibrary.h"  // for NiMaterialNodeLibrary
#include "NiStandardMaterialDescriptor.h" // for NiStandardMaterialDescriptor


#include <vector>
using namespace std;

// Forward references
class NiShader;


// Redefinition...
#define CG_NB_LIGHTS	7

//---------------------------------------------------------------------------
//! Class NiXSIStdMaterial
//! Main Gamebryo XSI shader class.
//---------------------------------------------------------------------------

class NiXSIStdMaterial
{
private:
    niShaderSet                 m_eShaderSet; // for interrogation

    // Shader containers
    vector<NiXSIShader*>        m_vpShaders;

    NiMaterialNodeLibraryPtr    m_spLibrary;
    NiStandardMaterialPtr       m_spStandardMaterial;
    NiStandardMaterialDescriptor* m_pkDescriptor;
    NiShaderPtr                 m_spShader;

    static char*                m_silighttype[CG_NB_LIGHTS];
    static char*                m_silightambient[CG_NB_LIGHTS];
    static char*                m_silightdiffuse[CG_NB_LIGHTS];
    static char*                m_silightspecular[CG_NB_LIGHTS];
    static char*                m_silightdirection[CG_NB_LIGHTS];
    static char*                m_silightposition[CG_NB_LIGHTS];
    static char*                m_silightfalloff[CG_NB_LIGHTS];
    static char*                m_silightattenuation[CG_NB_LIGHTS];
    static char*                m_silightcone[CG_NB_LIGHTS];

public:
    NiXSIStdMaterial();
   ~NiXSIStdMaterial();

    void                Gamebryo_Init();
    void                Gamebryo_Shutdown();

    // Maps to realtime plugin callbacks
    bool                Set_ShaderSet( niShaderSet eShaderSet );
    bool                Set_ShaderSet( std::string szProg_vertex, std::string szProg_fragment );
    bool                Terminate();

    // Vector Access
    int                 Get_nShaders()                          { return (int)m_vpShaders.size(); }
    NiXSIShader*        Get_pShader(int zi)
        { return (zi<(int)m_vpShaders.size())? m_vpShaders[zi] : NULL; }
    void                Add_pShader(NiXSIShader* p)             { m_vpShaders.push_back(p); }
    void                Clear_Shaders()                         { m_vpShaders.clear(); }

    niShaderSet         Get_eShaderSet()                        { return m_eShaderSet; }
    void                Set_eShaderSet(niShaderSet e)           { m_eShaderSet=e; }

    bool                Init( int nMethod );

    // Shader execution functions
    bool                Execute_Embeded( XSI_RTS_SystemParams			*in_pSysParams,
	                                     void							*in_pParams,
	                                     XSI_RTS_TriangulatedGeometry	*in_pGeom,
	                                     void							**io_pShaderInstanceData );
    static bool         Execute_Cg(    XSI_RTS_SystemParams			*in_pSysParams,
	                                   void							*in_pParams,
	                                   XSI_RTS_TriangulatedGeometry	*in_pGeom,
	                                   void							**io_pShaderInstanceData );
    static bool         Execute_Direct( XSI_RTS_SystemParams			*in_pSysParams,
	                                    void							*in_pParams,
	                                    XSI_RTS_TriangulatedGeometry	*in_pGeom,
	                                    void							**io_pShaderInstanceData );
    static bool         BindMatrices( XSI_RTS_SystemParams			    *in_pSysParams,
	                                  void							*in_pParams,
	                                  XSI_RTS_TriangulatedGeometry	*in_pGeom,
	                                  void							**io_pShaderInstanceData,
                                      niShaderType                    eShaderType );
    static bool         BindLights( XSI_RTS_SystemParams			    *in_pSysParams,
	                                void							*in_pParams,
	                                XSI_RTS_TriangulatedGeometry	*in_pGeom,
	                                void							**io_pShaderInstanceData,
                                    niShaderType                    eShaderType );
    static bool         BindTexture( niMaterialMap                     eMap,
                                     XSI_RTS_SystemParams			*in_pSysParams,
	                                 void							*in_pParams,
	                                 XSI_RTS_TriangulatedGeometry	*in_pGeom,
	                                 void							**io_pShaderInstanceData,
                                     niShaderType                    eShaderType,
                                     bool                            bApplyOpenGL,
                                     bool                            bApplyCg );
    static bool         TexCoord_GL(   XSI_RTS_SystemParams			*in_pSysParams,
                                        void							*in_pParams,
                                        XSI_RTS_TriangulatedGeometry	*in_pGeom,
                                        void							**io_pShaderInstanceData,
                                        LONG		                    target,
                                        LONG		                    uvtype,
                                        LONG		                    nbComponents  );
    static bool         Shade_GL( XSI_RTS_SystemParams			        *in_pSysParams,
                                     void							    *in_pParams,
                                     XSI_RTS_TriangulatedGeometry	    *in_pGeom,
                                     void							    **io_pShaderInstanceData,
                                     bool                               bLighting );
    static bool         Draw_GL( XSI_RTS_SystemParams			        *in_pSysParams,
                                   void							        *in_pParams,
                                   XSI_RTS_TriangulatedGeometry	        *in_pGeom,
                                   void							        **io_pShaderInstanceData,
                                   LONG                                 in_nBlending,
                                   LONG                                 in_nBlendFuncSrc,
                                   LONG                                 in_nBlendFuncDst,
                                   LONG                                 in_nCullingMode,
                                   LONG                                 in_nDepthTest,
                                   LONG                                 in_nDepthWrite,
                                   LONG                                 in_nReset );

    // Utility function ----------------------
    bool                CgProgram_Extract( XSI_RTS_SystemParams*          in_pSysParams,
                                            void*                         in_pParams,
                                            XSI_RTS_TriangulatedGeometry* in_pGeom,
                                            void**                        io_pShaderInstanceData );
    bool                CgProgram_Apply( XSI_RTS_SystemParams*            in_pSysParams,
                                            void*                         in_pParams,
                                            XSI_RTS_TriangulatedGeometry* in_pGeom,
                                            void**                        io_pShaderInstanceData,
                                            NiShader*                     pShader );

    // Utility function to set descriptor's bitfield
    static bool         IsMapEnabled( NiStandardMaterial::TextureMap eMap,
                                       NiStandardMaterialDescriptor* pkPixDesc );
    static bool         SetMapEnabled( NiStandardMaterial::TextureMap eMap,
                                        bool bEnabled,
                                         NiStandardMaterialDescriptor* pkPixDesc );

    // Main interface ----------------------------------

    NiMaterialNodeLibraryPtr    Get_spLibrary()                 { return m_spLibrary; }
    void                        Set_spLibrary(NiMaterialNodeLibraryPtr sp)
                                                                { m_spLibrary=sp; }

    NiStandardMaterialPtr       Get_spStandardMaterial()        { return m_spStandardMaterial; }
    void                        Set_spStandardMaterial(NiStandardMaterialPtr sp)
                                                                { m_spStandardMaterial=sp; }

    NiStandardMaterialDescriptor* Get_pkDescriptor()            { return m_pkDescriptor; }
    void                        Set_pkDescriptor(NiStandardMaterialDescriptor* p)
                                                                { m_pkDescriptor=p; }

    NiShaderPtr                 Get_spShader()                  { return m_spShader; }
    void                        Set_spShader(NiShaderPtr sp)    { m_spShader=sp; }
};

//===========================================================================
#endif // NI_XSI_STD_MATERIAL_H
