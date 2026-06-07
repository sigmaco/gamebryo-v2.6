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

#include "NiXSIShader.h"    // for NiXSIShader & NiXSIStdMaterial
#include "resource.h"       // for IDF_SHADERFILE_* IDs
#include "windows.h"        // for HMODULE, LoadResource, et...



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Class NiXSIShader
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NiXSIShader::NiXSIShader( niShaderType eShaderType, bool bShowDebug, std::string szCodeFile )
{
    m_eShaderType   = eShaderType;  
    m_bActive       = (szCodeFile.empty())? false : true;                 
	m_bShowDebug    = bShowDebug;  
    m_eProfile      = (m_eShaderType == NI_SHADER_VERTEX) ? CG_PROFILE_ARBVP1 : CG_PROFILE_ARBFP1;
    m_szCodeFile    = szCodeFile;      
    m_szCodeSource  = "";      
}

NiXSIShader::NiXSIShader( niShaderType eShaderType, bool bShowDebug, unsigned int nResID )
{
    m_eShaderType   = eShaderType;  
    m_bActive       = false;                 
	m_bShowDebug    = bShowDebug;  
    m_eProfile      = (m_eShaderType == NI_SHADER_VERTEX) ? CG_PROFILE_ARBVP1 : CG_PROFILE_ARBFP1;
    m_szCodeFile    = "";      
    m_szCodeSource  = "";      
    if ( LoadFromResource(nResID) )
        m_bActive = true;   
}

NiXSIShader::NiXSIShader()
{
    Reset(); // initializes everything
}

NiXSIShader::NiXSIShader( const NiXSIShader& b )
{
    m_eShaderType   = b.m_eShaderType;
    m_bActive       = b.m_bActive;
	m_bShowDebug    = b.m_bShowDebug;
    m_eProfile      = b.m_eProfile;
    m_szCodeFile    = b.m_szCodeFile;
    m_szCodeSource  = b.m_szCodeSource;    
}

NiXSIShader& NiXSIShader::operator=( const NiXSIShader& b )
{
    m_eShaderType   = b.m_eShaderType;
    m_bActive       = b.m_bActive;
	m_bShowDebug    = b.m_bShowDebug;
    m_eProfile      = b.m_eProfile;
    m_szCodeFile    = b.m_szCodeFile;
    m_szCodeSource  = b.m_szCodeSource;    

    return *this;
}

void NiXSIShader::Reset() 
{
    m_eShaderType   = NI_SHADER_VERTEX;  
    m_bActive       = false;                 
	m_bShowDebug    = false;              
    m_eProfile      = (m_eShaderType == NI_SHADER_VERTEX) ? CG_PROFILE_ARBVP1 : CG_PROFILE_ARBFP1;
    m_szCodeFile    = "";
    m_szCodeSource  = "";    
}

//! Apply shader to current geometry
void NiXSIShader::Apply() 
{
    // ...
}

//! Load source from resource
bool NiXSIShader::LoadFromResource( unsigned int nResID ) 
{        
    HMODULE     hModule     = GetModuleHandle( "GamebryoShaders.dll" );  
    
    std::string sResName;    
    switch( nResID )
    {        
        case IDF_SHADEFILE_VERTEX_01_PHONG: 
            sResName = "#1010";
            break;
        case IDF_SHADEFILE_FRAGMENT_01_PHONG: 
            sResName = "#1011";
            break;
        case IDF_SHADEFILE_VERTEX_02_DIFFUSEMAP1: 
            sResName = "#1012";
            break;
        case IDF_SHADEFILE_FRAGMENT_02_DIFFUSEMAP1: 
            sResName = "#1013";
            break;
        case IDF_SHADEFILE_VERTEX_03_DIFFUSEMAP2: 
            sResName = "#1014";
            break;
        case IDF_SHADEFILE_FRAGMENT_03_DIFFUSEMAP2: 
            sResName = "#1015";
            break;
        case IDF_SHADEFILE_VERTEX_04_BUMPMAP: 
            sResName = "#1016";
            break;
        case IDF_SHADEFILE_FRAGMENT_04_BUMPMAP: 
            sResName = "#1017";
            break;
        case IDF_SHADEFILE_VERTEX_00_PASSTHROUGH: 
            sResName = "#1018";
            break;
        case IDF_SHADEFILE_FRAGMENT_00_PASSTHROUGH: 
            sResName = "#1019";
            break;    
    }    
    HRSRC       hResource   = FindResource( hModule, sResName.c_str(), RT_RCDATA );
    if( !hResource ) 
        return false;
    
    DWORD dwSize    = SizeofResource(hModule, hResource);
    HGLOBAL hGlobal = LoadResource(hModule, hResource);
    if( hGlobal ) 
    {
        LPVOID pData = LockResource(hGlobal);
        if(pData) 
        {
	        // Cast pData to a char * and you have your shader
	        char* shader_code = (char*)pData;
    	            
	        // Do not forget that shader_code is not a zero-terminated string!
	        // Use dwSize to handle that.          
    		char* szTemp = new char[dwSize+1]; // one more

            for ( unsigned int i=0 ; i<dwSize ; i++ )
                szTemp[i] = shader_code[i];               
            szTemp[dwSize] = '\0'; // add zero termination
            m_szCodeSource = szTemp; // pass to std::string          
            
            delete [] szTemp; // cleanup
        }
    }
   
    return true;
}

//===========================================================================
// End of nixsishader.cpp