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

#include <stdarg.h>

//---------------------------------------------------------------------------
// XSI plugin callbacks.
//---------------------------------------------------------------------------

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Cg_Initialize();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Cg_Cleanup();
        break;
    }

    return TRUE;
}

RTSHADER_API unsigned int RTS_GetVersion()
{
    return XSI_RTSHADER_VERSION;	
}

//---------------------------------------------------------------------------
// Logging functions.
//---------------------------------------------------------------------------
void Log(siSeverityType in_Type, const char* in_Text)
{
#ifndef _DEBUG
    // Note: we tag debug message with the info severity type.
    if (in_Type == siInfoMsg)
        return;
#endif

    Application app;
    CString msg;
    msg.PutAsciiString(in_Text);
    app.LogMessage(msg, in_Type);

#ifdef _DEBUG
    OutputDebugString(in_Text);
#endif
}

void Logf(siSeverityType in_Type, const char* in_Text, ...)
{
    va_list args;
    va_start(args, in_Text);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), in_Text, args);
    Log(in_Type, buffer);
}

//---------------------------------------------------------------------------
// XSI 6.5 / 7.0 compatibility layer. Yes, they changed the signature
// of a fundamental function needed for real-time shaders. Woopeee.
//---------------------------------------------------------------------------
typedef __declspec(dllimport) bool (*DrawGeometryFunc)(
    XGSExtensionsGL *           pGLExt,
    XGSGLExtensionsFuncs *      pGLExtFuncs,
    void *                      in_pGeometry, 
    bool                        in_bNormals,
    bool                        in_bColors,
    bool                        in_bUV,
    XSI_OGLHelper_TexCoordMap * in_pTexCoordMap,
    XSI_OGLHelper_CavColorMap * in_pCavCoordMap);

bool Gamebryo_XSI_OGLHelper_DrawGeometry(
	XGSExtensionsGL *           pGLExt,
	XGSGLExtensionsFuncs *      pGLExtFuncs,
	void *                      in_pGeometry, 
	bool                        in_bNormals,
	bool                        in_bColors,
	bool                        in_bUV,
	XSI_OGLHelper_TexCoordMap * in_pTexCoordMap,
	XSI_OGLHelper_CavColorMap * in_pCavCoordMap)
{
    static const char xsi65FuncName[] = 
        "?XSI_OGLHelper_DrawGeometry@@YA_NPAUtagXGSExtensionsGL@@"
        "PAUtagXGSGLExtensionsFuncs@@PAUtagXSI_RTS_TriangulatedGeo"
        "metry@@_N33PAUXSI_OGLHelper_TexCoordMap@@PAUXSI_OGLHelper_CavColorMap@@@Z";
    static const char xsi70FuncName[] = "?XSI_OGLHelper_DrawGeome"
        "try@@YA_NPAUtagXGSExtensionsGL@@PAUtagXGSGLExtensionsFun"
        "cs@@PAUtagXSI_RTS_Primitive           @@_N33PAUXSI_OGLHe"
        "lper_TexCoordMap@@PAUXSI_OGLHelper_CavColorMap@@@Z";
    static DrawGeometryFunc s_DrawGeoFunc;

    if (!s_DrawGeoFunc)
    if (HMODULE modOGLHelper = ::GetModuleHandle("XSI_OGLHELPER.DLL"))
    if ((s_DrawGeoFunc = (DrawGeometryFunc)::GetProcAddress(modOGLHelper, xsi65FuncName)) == 0)
    if ((s_DrawGeoFunc = (DrawGeometryFunc)::GetProcAddress(modOGLHelper, xsi70FuncName)) == 0)
        return false;

    return s_DrawGeoFunc(
        pGLExt,
        pGLExtFuncs,
        in_pGeometry, 
        in_bNormals,
        in_bColors,
        in_bUV,
        in_pTexCoordMap,
        in_pCavCoordMap);
}

//---------------------------------------------------------------------------
// CG helper functions.
//---------------------------------------------------------------------------
static CgContext           g_Context;
static XSI_RTS_FXSemantics g_Semantics;

// Provide an option for 5.0 behaviour with the SIVIEW related semantics
typedef std::set<CGparameter> CGParamSet;
static bool       g_b50Semantics = ( getenv( "XSI_50_SEMANTICS" ) != NULL );
static CGParamSet g_setRowMajorParams;

CgContext* Cg_GetContext()
{
    return &g_Context;
}

void Cg_Initialize()
{
    g_Context.m_Context = NULL;
    g_Context.m_currentProgram = NULL;
    cgSetErrorCallback(Cg_CheckError);
    XSI_RTS_InitializeSemantics(g_Semantics);
}

void Cg_Cleanup()
{
}

void Cg_CheckError()
{
    CGerror err = cgGetError();

    if(err != CG_NO_ERROR)
    {
        const char* error   = cgGetErrorString(err);
        const char* listing = Cg_GetContext()->m_Context
                            ? cgGetLastListing(Cg_GetContext()->m_Context)
                            : 0;

        Log(siErrorMsg, error);
        if (listing)
            Logf(siErrorMsg, "Context:\n%s", listing);

#ifdef _DEBUG
        OutputDebugString(error);
        if (listing)
            OutputDebugString(listing);
#endif
    }
}

XSI_RTS_FXSemantics& Cg_GetSemantics()
{
    return g_Semantics;
}

void Cg_BindMatrix(CgMatrixBind *in_pBindingData)
{
    if(in_pBindingData->variable != NULL)
    {
        CGGLenum matrixType;
        CGGLenum transform = CG_GL_MATRIX_IDENTITY;

        float	finalMatrix[4][4];

        switch(in_pBindingData->matrix)
        {
            case 0: matrixType = CG_GL_MODELVIEW_PROJECTION_MATRIX; break;
            case 1: matrixType = CG_GL_MODELVIEW_MATRIX; break;
            case 2: matrixType = CG_GL_PROJECTION_MATRIX; break;

            case 3:	XSI_OGLHelper_MatrixTrackView(finalMatrix);
                break;
            case 4:	XSI_OGLHelper_MatrixTrackWorld(finalMatrix);
                break;
            case 5: memcpy ( finalMatrix, in_pBindingData->manual_matrix, sizeof(float)*16); 
                break;
        }

        if(in_pBindingData->invert)
        {
            transform = CG_GL_MATRIX_INVERSE;
        }

        if(in_pBindingData->transpose)
        {
            if(in_pBindingData->invert)
            {
                transform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
            }
            else
            {
                transform = CG_GL_MATRIX_TRANSPOSE;
            }
        }

        if(in_pBindingData->matrix < 3)
        {
            cgGLSetStateMatrixParameter(in_pBindingData->variable, matrixType, transform);
        }
        else
        {	
            if(in_pBindingData->invert)
                XSI_RTS_MatrixInvert(finalMatrix, finalMatrix);

            if(in_pBindingData->transpose)
                XSI_RTS_MatrixTranspose(finalMatrix, finalMatrix);

            float matrix[16];
            int x,y;

            switch(in_pBindingData->cgType)
            {
                case 0:  x = 1; y = 1; break;
                case 1:  x = 1; y = 2; break;
                case 2:  x = 1; y = 3; break;
                case 3:  x = 1; y = 4; break;
                case 4:  x = 2; y = 1; break;
                case 5:  x = 2; y = 2; break;
                case 6:  x = 2; y = 3; break;
                case 7:  x = 2; y = 4; break;
                case 8:  x = 3; y = 1; break;
                case 9:  x = 3; y = 2; break;
                case 10: x = 3; y = 3; break;
                case 11: x = 3; y = 4; break;
                case 12: x = 4; y = 1; break;
                case 13: x = 4; y = 2; break;
                case 14: x = 4; y = 3; break;
                case 15: x = 4; y = 4; break;
            }

            for(int loopy = 0; loopy < y; loopy++)
            {
                for(int loopx = 0; loopx < x; loopx++)
                {
                    matrix[loopx + (loopy * x)] = finalMatrix[loopy][loopx];
                }
            }

            cgGLSetMatrixParameterfr(in_pBindingData->variable, matrix);
        }
    }
}

void Cg_BindLight(CgLightTrackerBind *in_pBindingData)
{
    float fValues[4];
    float pvector[4];
    int   type;
    float direction[4];

    int   id = 0, idcheck = 0;
    bool  enabled = FALSE;

    // let's scan the for a light that is enabled

    for(id = 0; id < 7; id++)
    {
        if(glIsEnabled(GL_LIGHT0 + id))			
        {	
            if(idcheck == in_pBindingData->LightID)
            {
                break;
            }
            else
            {
                idcheck ++;
            }
        }
    }


    if(id < 7)
    {
        enabled = glIsEnabled(GL_LIGHT0 + id) ? TRUE : FALSE;
    }
    else
    {
        enabled = FALSE;
    }

    // track ambient
    if(in_pBindingData->Ambient && in_pBindingData->Constant_Register_Ambient)
    {
        if(enabled)
        {
            glGetLightfv(GL_LIGHT0 + id, GL_AMBIENT, fValues);
        }
        else
        {
            fValues[0] = fValues[1] = fValues[2] = fValues[3] = 0.0f;			
        }
        cgGLSetParameter4fv(in_pBindingData->Constant_Register_Ambient, fValues);
    }

    // track diffuse 
    if(in_pBindingData->Diffuse && in_pBindingData->Constant_Register_Diffuse)
    {
        if(enabled)
        {
            glGetLightfv(GL_LIGHT0 + id, GL_DIFFUSE, fValues);
        }
        else
        {
            fValues[0] = fValues[1] = fValues[2] = fValues[3] = 0.0f;			
        }
        cgGLSetParameter4fv(in_pBindingData->Constant_Register_Diffuse, fValues);
    }

    // track specular
    if(in_pBindingData->Specular && in_pBindingData->Constant_Register_Specular)
    {
        if(enabled)
        {
            glGetLightfv(GL_LIGHT0 + id, GL_SPECULAR, fValues);
        }
        else
        {
            fValues[0] = fValues[1] = fValues[2] = fValues[3] = 0.0f;			
        }
        cgGLSetParameter4fv(in_pBindingData->Constant_Register_Specular, fValues);
    }

    // track position (and type)
    glGetLightfv(GL_LIGHT0 + id, GL_POSITION, pvector);

    if(pvector[3] == 0.0f)
    {
        type = 0; // DIRECTIONAL

        // track direction for directional light
        if(in_pBindingData->Direction && in_pBindingData->Constant_Register_Direction)
        {
            cgGLSetParameter4fv(in_pBindingData->Constant_Register_Direction, pvector);
        }
    }
    else
    {
        // track position for other lights
        if(in_pBindingData->Position && in_pBindingData->Constant_Register_Position)
        {
            cgGLSetParameter4fv(in_pBindingData->Constant_Register_Position, pvector);
        }

        // track cutoff (and type)
        glGetLightfv(GL_LIGHT0 + id, GL_SPOT_CUTOFF, fValues);

        if(fValues[0] >= 180.0f)
        {
            type = 1; // POINT
        }
        else
        {
            type = 2; // SPOT

            // track spot direction
            glGetLightfv(GL_LIGHT0 + id, GL_SPOT_DIRECTION, direction);

            // track direction for directional light
            if(in_pBindingData->Direction && in_pBindingData->Constant_Register_Direction)
            {
                cgGLSetParameter4fv(in_pBindingData->Constant_Register_Direction, direction);
            }

            // track falloff
            if(in_pBindingData->Falloff && in_pBindingData->Constant_Register_Falloff)
            {
                glGetLightfv(GL_LIGHT0 + id, GL_SPOT_EXPONENT, fValues);
                cgGLSetParameter4fv(in_pBindingData->Constant_Register_Falloff, fValues);
            }

            // track cone
            if(in_pBindingData->Cone && in_pBindingData->Constant_Register_Cone)
            {
                glGetLightfv(GL_LIGHT0 + id, GL_SPOT_CUTOFF, fValues);
                cgGLSetParameter4fv(in_pBindingData->Constant_Register_Cone, fValues);
            }
        }
    }

    if(in_pBindingData->Type && in_pBindingData->Constant_Register_Type)
    {
        fValues[0] = fValues[1] = fValues[2] = fValues[3] = (float) type;
        cgGLSetParameter4fv(in_pBindingData->Constant_Register_Type, fValues);
    }

    if(in_pBindingData->Attenuation && in_pBindingData->Constant_Register_Attenuation)
    {
        glGetLightfv(GL_LIGHT0 + id, GL_CONSTANT_ATTENUATION, &fValues[0]);
        glGetLightfv(GL_LIGHT0 + id, GL_LINEAR_ATTENUATION, &fValues[1]);
        glGetLightfv(GL_LIGHT0 + id, GL_QUADRATIC_ATTENUATION, &fValues[2]);
        cgGLSetParameter4fv(in_pBindingData->Constant_Register_Attenuation, fValues);
    }
}

void Cg_BindSemanticCallback(
    void* in_pContext,
    void* in_pParameterHandle,
    void* in_pValue)
{
    CGparameter l_Parameter = (CGparameter) in_pParameterHandle;
    float*      l_pValue = (float*) in_pValue;

    CGparameterclass l_ParamClass= cgGetParameterClass(l_Parameter);
    switch(l_ParamClass)
    {
        case CG_PARAMETERCLASS_SCALAR:
            cgSetParameter1f(l_Parameter, l_pValue[0]);
            break;
        case CG_PARAMETERCLASS_VECTOR:
            cgSetParameter4f(l_Parameter, l_pValue[0], l_pValue[1], l_pValue[2], l_pValue[3]);
            break;
        case CG_PARAMETERCLASS_MATRIX:
            if ( g_b50Semantics && 
                (g_setRowMajorParams.find(l_Parameter) != g_setRowMajorParams.end()) )
                cgGLSetMatrixParameterfr(l_Parameter, l_pValue);
            else
                cgGLSetMatrixParameterfc(l_Parameter, l_pValue);
            break;
        case CG_PARAMETERCLASS_STRUCT:
            break;
        case CG_PARAMETERCLASS_ARRAY:
            break;
        case CG_PARAMETERCLASS_SAMPLER:
            break;
        case CG_PARAMETERCLASS_OBJECT:
            break;
    }
}

bool Use50Semantics()
{
    return g_b50Semantics;
}

void AddRowMajorParameter(CGparameter in_param)
{
    if (g_b50Semantics)
        g_setRowMajorParams.insert(in_param);
}

void ClearRowMajorParameters()
{
    if (g_b50Semantics)
        g_setRowMajorParams.clear();
}
