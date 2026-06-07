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

#ifndef NI_XSI_SHADER_H
#define NI_XSI_SHADER_H

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "xsi_rtshaders.h"          // for XSI_RTS_FXSemantics_inuse
#include <vector>
#include "NiXSIMaps.h"              // for niMaterialMap 

// Shader type
enum niShaderType
{
    NI_SHADER_VERTEX,       // 0
    NI_SHADER_FRAGMENT,     // 1

    NI_NUM_SHADERS          // 2 *** NUM ***
};

// Predefined shader sets
enum niShaderSet
{
    NI_SHADERSET_00_PASSTHROUGH,        // 0 - passthrough vertex and pixel shaders
    NI_SHADERSET_01_PHONG,              // 1 - flat shaded vertex shader only
    NI_SHADERSET_02_DIFFUSEMAP1,        // 2 - basic phong + 3lights - no maps
    NI_SHADERSET_03_DIFFUSEMAP2,        // 3 - basic phong + 3 lights + bump + diffuse maps
    NI_SHADERSET_04_BUMPMAP,            // 4 - Gamebryo encapsulation

    NI_NUM_SHADERSETS,                  // 5 *** num ***
};

//---------------------------------------------------------------------------
//! Class NiXSIShader
//! Used to encapsulate behaviors states and parameters for each Cg Shader.
//---------------------------------------------------------------------------

class NiXSIShader 
{
private:
    // Basic flags
    niShaderType                m_eShaderType;  //! vertex or fragment shader
    bool		                m_bActive;      //! applicaton control
	bool		                m_bShowDebug;   //! debug info in cgc  
	CGprofile		            m_eProfile;     //! ARBVertex, ARBFragment or NV20/30

    std::string                 m_szCodeFile;   //! shader file name     
    std::string                 m_szCodeSource; //! shader source     
   
public:
    NiXSIShader( niShaderType eShaderType, 
        bool bShowDebug, 
        std::string szCodeFile );   // load from file
    NiXSIShader( niShaderType eShaderType, 
        bool bShowDebug, 
        unsigned int nResID );      // load from resource
    NiXSIShader();
    NiXSIShader( const NiXSIShader& b );

    NiXSIShader&                operator=( const NiXSIShader& b );
    
    void Reset();
    bool Init( niShaderType eShaderType, 
        bool bShowDebug, 
        std::string szCodeFile );
    void Apply(); // apply shader to current geometry

    bool Get_bCode() 
    { 
        return ( !m_szCodeFile.empty() || !m_szCodeSource.empty() )? true : false; 
    }

    bool LoadFromResource( unsigned int nResID );

    // Main interface -----------------------------------

    niShaderType                Get_eShaderType()                   { return m_eShaderType; }
    void                        Set_eShaderType(niShaderType e)     { m_eShaderType=e; } 
                                
    bool		                Get_bActive()                       { return m_bActive; }      
    void 		                Set_bActive(bool b)                 { m_bActive=b; } 
                                
    bool		                Get_bShowDebug()                    { return m_bShowDebug; }   
    void 		                Set_bShowDebug(bool b)              { m_bShowDebug=b; }  
	                            
    CGprofile		            Get_eProfile()                      { return m_eProfile; }    
    void 		                Set_eProfile(CGprofile e)           { m_eProfile=e; }    
                                
    std::string                 Get_szCodeFile()                    { return m_szCodeFile; }      
    void                        Set_szCodeFile(std::string sz)      { m_szCodeFile=sz; }       

    std::string                 Get_szCodeSource()                  { return m_szCodeSource; }
    void                        Set_szCodeSource(std::string sz)    { m_szCodeSource=sz; }     
};


//===========================================================================
#endif // NI_XSI_SHADER_H