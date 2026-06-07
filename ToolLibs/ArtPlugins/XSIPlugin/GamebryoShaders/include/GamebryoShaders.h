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

#ifndef GAMEBRYO_SHADERS_H
#define GAMEBRYO_SHADERS_H

#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#include <assert.h>
#include <stdio.h>
#include <set>

#include <sicppsdk.h>

// This absolutely needs to be first
#include <xsi_ref.h>
#include <xsi_decl.h>
#include <xsi_shader.h>
#include <xsi_customproperty.h>
#include <xsi_parameter.h>
#include <xsi_time.h>
#include <xsi_application.h>

using namespace XSI;

#include <XSI_rtshaders.h>
#include <XSI_OGLHelper.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <GL/glu.h>

#include "NiXSIMaps.h"

//---------------------------------------------------------------------------
// CG data.
//---------------------------------------------------------------------------
struct CgContext
{
    CGprogram			m_currentProgram;
    CGcontext			m_Context;
};

struct CgLightTrackerBind
{
    LONG			LightID;
    LONG			Type; 
    CGparameter		Constant_Register_Type;
    LONG			Diffuse; 
    CGparameter		Constant_Register_Diffuse;
    LONG			Specular; 
    CGparameter		Constant_Register_Specular;
    LONG			Ambient; 
    CGparameter		Constant_Register_Ambient;
    LONG			Position; 
    CGparameter		Constant_Register_Position;
    LONG			Direction; 
    CGparameter		Constant_Register_Direction;
    LONG			Falloff; 
    CGparameter		Constant_Register_Falloff;
    LONG			Attenuation; 
    CGparameter		Constant_Register_Attenuation;
    LONG			Cone; 
    CGparameter		Constant_Register_Cone;
};

struct CgMatrixBind
{
    CGparameter	variable;
    LONG		matrix;
    LONG		cgType;
    LONG		transpose;
    LONG		invert;
    float		*manual_matrix;
};

class CgParameterCacheEntry
{
public:
    CgParameterCacheEntry() {};
    ~CgParameterCacheEntry() {};

    CGparameter		m_CgParameter;
    Parameter		m_XSIParameter[4];
};

void Cg_BindSemanticCallback(void* in_pContext, void* in_pParameterHandle, void* in_pValue);

//---------------------------------------------------------------------------
// Use 5.0 semantics (for backward compatibility? Needed?)
//---------------------------------------------------------------------------
void AddRowMajorParameter(CGparameter in_param);
void ClearRowMajorParameters();
bool Use50Semantics();

//---------------------------------------------------------------------------
// Logging functions.
//---------------------------------------------------------------------------
void Log( siSeverityType, const char* in_Text);
void Logf(siSeverityType, const char* in_Text, ...);

//---------------------------------------------------------------------------
// XSI 6.5 / 7.0 compatibility layer. Yes, they changed the signature
// of a fundamental function needed for real-time shaders. Woopeee.
//---------------------------------------------------------------------------
bool Gamebryo_XSI_OGLHelper_DrawGeometry(
	XGSExtensionsGL *           pGLExt,
	XGSGLExtensionsFuncs *      pGLExtFuncs,
	void *                      in_pGeometry, 
	bool                        in_bNormals,
	bool                        in_bColors,
	bool                        in_bUV,
	XSI_OGLHelper_TexCoordMap * in_pTexCoordMap,
	XSI_OGLHelper_CavColorMap * in_pCavCoordMap);

//---------------------------------------------------------------------------
// These functions should not have any name mangling
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
    //---------------------------------------------------------------------------
    // XSI callback for version compatibility checks.
    //---------------------------------------------------------------------------
    RTSHADER_API unsigned int RTS_GetVersion();

    //---------------------------------------------------------------------------
    // CG helper functions.
    //---------------------------------------------------------------------------
    CgContext* CGENTRY Cg_GetContext();
    void CGENTRY Cg_Initialize();
    void CGENTRY Cg_Cleanup();
    void CGENTRY Cg_CheckError();
    void CGENTRY Cg_BindMatrix(CgMatrixBind *in_pBindingData);
    void CGENTRY Cg_BindLight(CgLightTrackerBind *in_pBindingData);
    XSI_RTS_FXSemantics& CGENTRY Cg_GetSemantics();

    //---------------------------------------------------------------------------
    // GamebryoMaterial XI callbacks.
    //---------------------------------------------------------------------------
    RTSHADER_API bool GamebryoMaterial_Init(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoMaterial_Term(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoMaterial_Execute(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        XSI_RTS_TriangulatedGeometry* in_pGeom,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoMaterial_GetRequirements(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        XSI_RTS_TriangulatedGeometry* in_pGeom,
        void** io_pShaderInstanceData,
        ULONG in_lRequirement,
        ULONG* out_pRequirementValue);

    //---------------------------------------------------------------------------
    // GamebryoFX
    //---------------------------------------------------------------------------
    RTSHADER_API bool GamebryoFX_Init(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoFX_InitUI(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoFX_Term(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoFX_Execute(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        XSI_RTS_TriangulatedGeometry* in_pGeom,
        void** io_pShaderInstanceData);

    RTSHADER_API bool GamebryoFX_GetRequirements(
        XSI_RTS_SystemParams* in_pSysParams,
        void* in_pParams,
        XSI_RTS_TriangulatedGeometry* in_pGeom,
        void** io_pShaderInstanceData,
        ULONG in_lRequirement,
        ULONG* out_pRequirementValue);

    //---------------------------------------------------------------------------
} // end of extern "C"

//---------------------------------------------------------------------------

#endif // __cplusplus
#endif // GAMEBRYO_SHADERS_H
