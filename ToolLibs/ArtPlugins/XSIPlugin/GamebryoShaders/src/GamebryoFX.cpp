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

#include "NiFilename.h"

#include <xsi_graphicsequencer.h>
#include <xsi_graphicsequencercontext.h>
#include <xsi_parameter.h>
#include <xsi_shader.h>
#include <xsi_customproperty.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>
#include <xsi_application.h>
#include <xsi_griddata.h>
#include <xsi_imageclip2.h>
#include <xsi_preferences.h>

#include <vector>
#include <map>
#include <io.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace XSI;

namespace
{
    //---------------------------------------------------------------------------
    // Describes the parameters received from XSI based on the
    // GamebryoFX SPDL definition.
    //
    // All structures that are used to construct a PPG must be compiled without padding
    // between struct fields.  This is because a pointer to the PPG struct will be initialized
    // directly by type casting a void pointer as in the following:
    //
    //   MyPPG * my_ppg = (MyPPG*) in_pVoid;
    //
    // The pragma is not necessary under a 32 bit architecture since a PPG will only contain
    // floats (4 bytes), longs (4 bytes) and pointers (4 bytes).  Therefore structs will
    // never contain padding.  But, in a 64 bit architecture where pointers are 8 bytes,
    // if we don't force the compiler to remove the automatic padding, the struct may contain
    // padding.  This padding is not considered when initializing the void pointer (in_pVoid).
    // Therefore, the direct type casting would not work.
    //---------------------------------------------------------------------------
    #pragma pack(push, 1)
    struct GamebryoFXPPG
    {
        // General
        char *              selected_shader;
        LONG                nb_textures;
        LONG                refresh_flag;
        LONG                tspace_id[16];
        XSI_RTS_Texture     textures[16];
    };
    #pragma pack(pop)

    //---------------------------------------------------------------------------
    // Per-texture instance data.
    //---------------------------------------------------------------------------
    struct GamebryoFXTextureData
    {
        GamebryoFXTextureData();
        ~GamebryoFXTextureData();

        void ResetGLTexture();

        std::string     m_Name;
        GLuint          m_GLTexture;
        CGparameter     m_CGTexture;
    };

    //---------------------------------------------------------------------------
    // This is the per-shader instance data needed to generate the OpenGL calls.
    //---------------------------------------------------------------------------
    class GamebryoFXInstanceData
    {
    public:
        GamebryoFXInstanceData();
        ~GamebryoFXInstanceData();

        void Clear();

        bool InitUI(XSI_RTS_SystemParams* in_pSysParams, void* in_pParams, bool in_AllowCreate);
        bool FindParameter(const char *in_ParamName, Parameter& out_XSIParameter) const;
        bool BindParameters(XSI_RTS_SystemParams *in_pSysParams, GamebryoFXPPG *in_pPPG);
        void InitializeTechnique(Shader &in_Shader);
        void InitializeTweakables(Shader &in_Shader, CRefArray &in_OldParams);
        void InitializeLayout(Shader &in_Shader, GamebryoFXPPG *in_pPPG);
        bool IsLayoutInitialized() const;
        int FindTextureIndex(GamebryoFXPPG& in_PPG, const char* in_Name, int in_DefaultIndex);

        static std::string GetSelectedFileName(GamebryoFXPPG* l_pPPG);

        typedef std::vector<CgParameterCacheEntry> CGXSIParams;

        CGcontext                   m_Context;
        CGeffect                    m_Effect;
        XSI_RTS_FXSemantics_inuse   m_Semantics;
        CustomProperty              m_CPSet;
        std::string                 m_FileName;
        GamebryoFXTextureData       m_Textures[16];
        int                         m_TextureCount;
        CGXSIParams                 m_CGXSIParams;
        bool                        m_UIUpdateInProgress;
        bool                        m_LayoutInitialized;
        double                      m_CurrentTime;
    };

    //---------------------------------------------------------------------------
    // Automatically change the current directory. Helps find image files.
    //---------------------------------------------------------------------------
    class AutoChangeCurDir
    {
    public:
        AutoChangeCurDir(const char* in_SomeFilename)
        {
            char* cwd = _getcwd(NULL, 0);
            if (cwd)
            {
                m_PrevCurDir = cwd;
                free(cwd);
            }
            const char* newDir = NiFilename(in_SomeFilename).GetDir();
            _chdir(newDir);
        }
        ~AutoChangeCurDir()
        {
            if (m_PrevCurDir.size() > 0 )
                _chdir(m_PrevCurDir.c_str());
        }

    private:
        std::string m_PrevCurDir;
    };

    //---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
// XSI initialization callbacks.
//
// @param in_pSysParams             The system parameters
// @param in_pParams                The property page parameters
// @param io_pShaderInstanceData    The shader instance data
//---------------------------------------------------------------------------
RTSHADER_API bool GamebryoFX_Init(
    XSI_RTS_SystemParams * in_pSysParams,
    void *                 in_pParams,
    void **                io_pShaderInstanceData)
{
    // Note: do it in two steps to help debugging since io_pShaderInstanceData is void*.
    GamebryoFXInstanceData *pInstanceData = new GamebryoFXInstanceData;
    *io_pShaderInstanceData = pInstanceData;

    return true;
}

//---------------------------------------------------------------------------
// XSI termination callback.
//
// @param in_pSysParams             The system parameters
// @param in_pParams                The property page parameters
// @param io_pShaderInstanceData    The shader instance data
//---------------------------------------------------------------------------
RTSHADER_API bool GamebryoFX_Term(
    XSI_RTS_SystemParams *  in_pSysParams,
    void *                  in_pParams,
    void **                 io_pShaderInstanceData)
{
    GamebryoFXInstanceData *pInstanceData = (GamebryoFXInstanceData*) *io_pShaderInstanceData;
    if (!pInstanceData)
        return true;

    *io_pShaderInstanceData = NULL;
    delete pInstanceData;

    return true;
}

//---------------------------------------------------------------------------
// XSI requirement callback.
//
// @param in_pSysParams             The system parameters
// @param in_pParams                The property page parameters
// @param in_pGeom                  The geometry information
// @param io_pShaderInstanceData    The shader instance data
// @param in_lRequirement           The requirement ID
// @param out_pRequirementValue     The returned Value
//---------------------------------------------------------------------------
RTSHADER_API bool GamebryoFX_GetRequirements(
    XSI_RTS_SystemParams*         in_pSysParams,
    void*                         in_pParams,
    XSI_RTS_TriangulatedGeometry* in_pGeom,
    void**                        io_pShaderInstanceData,
    const ULONG                   in_lRequirement,
    ULONG*                        out_pRequirementValue)
{
    if (in_lRequirement == RTS_REQUIRE_PRIMARYINITUIGUID)
    {
        GUID *l_pGuid = (GUID *) out_pRequirementValue;
        GUID guid = {0x08C5D27C, 0x200B, 0x4fbd, 0x85, 0x77, 0xCA, 0x1D, 0xF3, 0x15, 0x3C, 0x11};
        *l_pGuid = guid;
    }
    else if (in_lRequirement == RTS_REQUIRE_SECONDARYINITUIGUID)
    {
        GUID *l_pGuid = (GUID *) out_pRequirementValue;
        GUID guid = {0x9558C41C, 0x6C3D, 0x404b, 0x9E, 0x22, 0xE3, 0x85, 0x57, 0x9D, 0x26, 0xD1};
        *l_pGuid = guid;
    }

    return true;
}

//---------------------------------------------------------------------------
// XSI UI callback.
//
// @param in_pSysParams          the system parameters.
// @param in_pParams             the property page parameters.
// @param io_pShaderInstanceData the shader instance data
//---------------------------------------------------------------------------
RTSHADER_API bool GamebryoFX_InitUI(
    XSI_RTS_SystemParams* in_pSysParams,
    void*                 in_pParams,
    void**                io_pShaderInstanceData)
{
    GamebryoFXInstanceData *pInstanceData = (GamebryoFXInstanceData*) *io_pShaderInstanceData;
    if (!pInstanceData)
        return true;

    return pInstanceData->InitUI(in_pSysParams, in_pParams, true);
}

//---------------------------------------------------------------------------
// XSI shader execution callback.
//
// @param in_pSysParams           the system parameters
// @param in_pParams              the property page parameters
// @param in_pGeom                the geometry information
// @param *io_pShaderInstanceData the shader instance data
//---------------------------------------------------------------------------
RTSHADER_API bool GamebryoFX_Execute(
    XSI_RTS_SystemParams*         in_pSysParams,
    void*                         in_pParams,
    XSI_RTS_TriangulatedGeometry* in_pGeom,
    void**                        io_pShaderInstanceData)
{
    GamebryoFXInstanceData * pInstanceData = (GamebryoFXInstanceData*) *io_pShaderInstanceData;
    if (!pInstanceData)
        return true;

    GamebryoFXPPG * l_pPPG = (GamebryoFXPPG*) in_pParams;
    if (!l_pPPG)
        return true;

    Cg_GetContext()->m_Context = pInstanceData->m_Context;
    cgGLSetManageTextureParameters(Cg_GetContext()->m_Context, CG_TRUE);

    pInstanceData->InitUI(in_pSysParams, in_pParams, !pInstanceData->IsLayoutInitialized());

    if (pInstanceData->m_Effect != NULL)
    {
        // Check the techniques.
        CString l_TechniqueName = pInstanceData->m_CPSet.GetParameterValue(L"Technique");
        CGtechnique l_Technique = cgGetNamedTechnique(
            pInstanceData->m_Effect, l_TechniqueName.GetAsciiString());
        if (!cgIsTechnique(l_Technique))
        {
            l_Technique = cgGetFirstTechnique(pInstanceData->m_Effect);
            CString l_TechniqueName;
            l_TechniqueName.PutAsciiString(cgGetTechniqueName(l_Technique));
            pInstanceData->m_CPSet.PutParameterValue(L"Technique", l_TechniqueName);
        }

        bool l_bParametersAreValid = true;
        {
            // Seems we need to update the parameter binding when the scene time changes... why?
            const double diff = fabs(pInstanceData->m_CurrentTime
                              - in_pSysParams->m_SceneData->m_fCurrentTime);
            bool l_bUpdateParams = (diff > 0.000001);
            pInstanceData->m_CurrentTime = in_pSysParams->m_SceneData->m_fCurrentTime;

            if ((in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_PARAM_CHANGE) || l_bUpdateParams)
                l_bParametersAreValid = pInstanceData->BindParameters(in_pSysParams, l_pPPG);
        }

        // set the parameters by semantics
        // Compute the model, view and projection matrices and push it to the semantics
        glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*) Cg_GetSemantics().m_Proj);

        // Bind all semantics now
        XSI_RTS_BindCommonSemantics(
            Cg_GetSemantics(),
            pInstanceData->m_Semantics,
            in_pSysParams,
            io_pShaderInstanceData,
            Cg_BindSemanticCallback);

        // now setup the texcoord bindings
        for (int loop = 0; loop < l_pPPG->nb_textures; loop++)
        {
            XSI_OGLHelper_TexCoordMap* texMap = XSI_OGLHelper_GetTextureCoordMap();
            if (texMap)
            {
                texMap->target[texMap->nbActiveUVs] = GL_TEXTURE0_ARB + loop;
                texMap->UVSet[texMap->nbActiveUVs]  = l_pPPG->tspace_id[loop];
                texMap->nbActiveUVs ++;
            }
        }


        if (l_bParametersAreValid && cgIsTechnique(l_Technique))
        {
            // passes
            CGpass l_Pass = cgGetFirstPass(l_Technique);
            while (l_Pass)
            {
                cgSetPassState(l_Pass);
                Gamebryo_XSI_OGLHelper_DrawGeometry(
                    in_pSysParams->m_pGLExt,
                    in_pSysParams->m_pGLExtFuncs,
                    in_pGeom,
                    1,
                    1,
                    1,
                    XSI_OGLHelper_GetTextureCoordMap(),
                    XSI_OGLHelper_GetVertexColorMap());
                cgResetPassState(l_Pass);
                l_Pass = cgGetNextPass(l_Pass);
            }
        }
        else
        {
            float errorcolor[4] = { 1, 0.5, 0, 1 };
            ::glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , errorcolor);
            ::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , errorcolor);
            ::glEnable(GL_LIGHTING);

            // Needed because we don't want to have visual regressions with old scenes.
            ::glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR );

            Gamebryo_XSI_OGLHelper_DrawGeometry(
                in_pSysParams->m_pGLExt,
                in_pSysParams->m_pGLExtFuncs,
                in_pGeom,
                1,
                1,
                1,
                XSI_OGLHelper_GetTextureCoordMap(),
                XSI_OGLHelper_GetVertexColorMap());
        }
    }
    else
    {
        float errorcolor[4] = { 1, 0, 0, 1 };
        ::glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , errorcolor);
        ::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , errorcolor);
        ::glEnable( GL_LIGHTING );

        // Needed because we don't want to have visual regressions with old scenes.
        ::glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR );

        Gamebryo_XSI_OGLHelper_DrawGeometry(
            in_pSysParams->m_pGLExt,
            in_pSysParams->m_pGLExtFuncs,
            in_pGeom,
            1,
            1,
            1,
            XSI_OGLHelper_GetTextureCoordMap(),
            XSI_OGLHelper_GetVertexColorMap());
    }

    XSI_OGLHelper_ResetTextureCoordMap();

    return TRUE;
}

namespace
{
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    GamebryoFXTextureData::GamebryoFXTextureData()
        : m_Name()
        , m_GLTexture(-1)
        , m_CGTexture(0)
    {
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    GamebryoFXTextureData::~GamebryoFXTextureData()
    {
        ResetGLTexture();
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    void GamebryoFXTextureData::ResetGLTexture()
    {
        if (m_GLTexture != -1)
        {
            glDeleteTextures(1, &m_GLTexture);
            m_GLTexture = -1;
        }
    }

    //---------------------------------------------------------------------------
    // Initialize this Gamebryo FX per-instance data.
    //---------------------------------------------------------------------------
    GamebryoFXInstanceData::GamebryoFXInstanceData()
        : m_Context(cgCreateContext())
        , m_Effect(NULL)
        , m_Semantics()
        , m_CPSet()
        , m_FileName()
        , m_Textures()
        , m_TextureCount(0)
        , m_CGXSIParams()
        , m_UIUpdateInProgress(false)
        , m_LayoutInitialized(false)
        , m_CurrentTime(0.0)
    {
        cgGLRegisterStates(m_Context);
        XSI_RTS_InitializeSemantics_inuse(m_Semantics);
    }

    //---------------------------------------------------------------------------
    // Deallocate this Gamebryo FX per-instance data.
    //---------------------------------------------------------------------------
    GamebryoFXInstanceData::~GamebryoFXInstanceData()
    {
        PPGLayout l_PPGLayout = m_CPSet.GetPPGLayout();
        l_PPGLayout.Clear();

        if (m_Effect)
        {
            cgDestroyEffect(m_Effect);
            m_Effect = NULL;
        }

        if (m_Context)
        {
            cgDestroyContext(m_Context);
            m_Context = NULL;
        }

        for (int loop = 0; loop < m_TextureCount; ++loop)
            m_Textures[loop].ResetGLTexture();

        // The following code is taken from the XSI 7.0 version.
        {
		    // ok this code here is required to cleanup the custom pset 
		    // so that it gets deleted from the scene correctly
		    CString l_PsetName = m_CPSet.GetUniqueName();

		    // 1. At this point, the custom pset should have been deleted
		    // by the shader delete task BUT it's not the case since we still
		    // have a reference to it with the m_CPSet cache, so firstly, we
		    // remove this reference
		    m_CPSet.ResetObject();

		    // 2. Then we invoke the deleteobj command so that we 
		    // get rid of the custom pset for good
		    CValueArray args(1);
		    args[0]= l_PsetName;
		    CValue retval;
		    Application gApp;
		    CStatus st = gApp.ExecuteCommand(L"DeleteObj", args, retval);
        }

        m_FileName.resize(0);

        Clear();
    }

    //---------------------------------------------------------------------------
    // Reinitialize this Gamebryo FX per-instance data to a re-usable state.
    //---------------------------------------------------------------------------
    void GamebryoFXInstanceData::Clear()
    {
        m_CGXSIParams.resize(0);
        m_TextureCount = 0;
    }

    //---------------------------------------------------------------------------
    // Find the XSI parameter of the given name.
    //---------------------------------------------------------------------------
    bool GamebryoFXInstanceData::FindParameter(
        const char *in_ParamName,
        Parameter& out_XSIParameter) const
    {
        CString l_XSIParamName;
        l_XSIParamName.PutAsciiString(in_ParamName);
        out_XSIParameter = m_CPSet.GetParameter(l_XSIParamName);
        return out_XSIParameter.IsValid();
    }

    //---------------------------------------------------------------------------
    // Find a texture by its parameter name.
    //---------------------------------------------------------------------------
    int GamebryoFXInstanceData::FindTextureIndex(
        GamebryoFXPPG& in_PPG,
        const char* in_Name,
        int in_DefaultIndex)
    {
        for (int loop = 0; loop < in_PPG.nb_textures; loop++)
            if (m_Textures[loop].m_Name.compare(in_Name) == 0)
                return loop;

        return in_DefaultIndex;
    }

    //---------------------------------------------------------------------------
    // Initializes the UI (PPGLayout).
    //---------------------------------------------------------------------------
    bool GamebryoFXInstanceData::InitUI(
        XSI_RTS_SystemParams* in_pSysParams,
        void*                 in_pParams,
        bool                  in_AllowCreate)
    {
        if (!in_pSysParams->m_pShaderHandle)
            return false;

        Shader l_Shader = *( (CRef *) in_pSysParams->m_pShaderHandle);
        if (!m_CPSet.IsValid())
        {
            m_CPSet = l_Shader.GetProperties().GetItem(L"GamebryoFX_Parameters" );
            if (!m_CPSet.IsValid() && in_AllowCreate)
            {
                m_CPSet = l_Shader.AddProperty(L"CustomProperty",false,L"GamebryoFX_Parameters");

                /// ?!?! Taken from XSI 7.
			    CString keywords(L"General|Rendering");
			    CString l_fullname = m_CPSet.GetFullName();
			    CString l_code(L"SetValue \"");
			    l_code += l_fullname;
			    l_code += L".InspectorKeywords\", \"General|Rendering\"";
                Application app;
                app.ExecuteScriptCode(l_code, L"VBScript");
            }
        }

        // Make sure we're not being called while in the middle of another call!
        if (m_UIUpdateInProgress)
            return true;

        // Define Shader's attributes
        GamebryoFXPPG * l_pPPG = (GamebryoFXPPG*) in_pParams;

        // Verify if selected shader name changed.
        bool nameChanged = false;
        if (in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_PARAM_CHANGE)
        {
            if (l_pPPG->selected_shader)
            {
                if (m_FileName.compare(l_pPPG->selected_shader))
                    nameChanged = true;
            }
            else if (m_FileName.size() > 0)
            {
                nameChanged = true;
            }
        }

        // Verify if the PPG changed significantly.
        if (!m_Effect || nameChanged || !cgIsEffect(m_Effect) || l_pPPG->nb_textures == -1)
        {
            // we're in trouble, this is an horrible hack, if this happens it's because of some
            // bizarre notification order that triggers this while still in load mode in the
            // ActiveDB.
            CString l_Name = m_CPSet.GetFullName();
            if (l_Name == L"")
                return false;

            m_UIUpdateInProgress = true;

            CgContext* cgContext = Cg_GetContext();
            cgContext->m_Context = m_Context;
            if (cgIsContext(m_Context))
                cgGLSetManageTextureParameters(cgContext->m_Context, CG_TRUE);

            bool bRebuildParam = false;

            if ((in_pSysParams->m_lShaderFlags & RTS_SHADER_FLAG_PARAM_CHANGE) && m_Effect != 0)
            {
                Clear();
                // Destroy all effects in this context.
                cgDestroyEffect(m_Effect);
                m_Effect = NULL;
                bRebuildParam = true;
            }

            if (!m_Effect)
            {
                std::string szSelectedFileName;
                if (l_pPPG->selected_shader)
                    szSelectedFileName = GamebryoFXInstanceData::GetSelectedFileName(l_pPPG);
                if (szSelectedFileName.compare(m_FileName) != 0)
                    bRebuildParam = true;

                // Limit the scope of the current directory change.
                {
                    const char* fileName = szSelectedFileName.c_str();
                    AutoChangeCurDir autoChangeCurDir(fileName);

                    // create the effect
                    if (szSelectedFileName.find(".cgfx") != std::string::npos
                    ||  szSelectedFileName.find(".CGFX") != std::string::npos)
                    {
                        m_Effect = cgCreateEffectFromFile(cgContext->m_Context, fileName, NULL);
                        bRebuildParam = true;
                    }
                    else if (szSelectedFileName.find(".fx") != std::string::npos
                    ||       szSelectedFileName.find(".FX") != std::string::npos)
                    {
                        // Note: .fx Includes .fxl files
                        m_Effect = cgCreateEffectFromFile(cgContext->m_Context, fileName, NULL);
                        bRebuildParam = true;
                    }
                    else if (szSelectedFileName.find(".nsf") != std::string::npos
                    ||       szSelectedFileName.find(".NSF") != std::string::npos)
                    {
                        m_Effect = cgCreateEffectFromFile(cgContext->m_Context, fileName, NULL);
                        bRebuildParam = true;
                    }
                }

                m_FileName = szSelectedFileName;

                if (!m_Effect)
                {
                    const char * listing = cgGetLastListing(cgContext->m_Context);
                    Logf(
                        siErrorMsg,
                        "GamebryoFX: unable to create effect from \"%s\". Context:\n%s",
                        szSelectedFileName.c_str(),
                        (listing && listing[0]) ? listing : "(no context given)");
                }
                else if(in_AllowCreate)
                {
                    if(bRebuildParam || (m_CPSet.GetParameters().GetCount() == 0))
                    {
                        PPGLayout l_PPGLayout = m_CPSet.GetPPGLayout();
                        l_PPGLayout.Clear();

                        if (m_Effect)
                        {
                            Parameter technique = m_CPSet.GetParameter(L"Technique");
                            if (!technique.IsValid())
                            {
                                CStatus status = m_CPSet.AddParameter(
                                    L"Technique",
                                    CValue::siString,
                                    siPersistable,
                                    L"Technique",
                                    L"Technique",
                                    CValue(),
                                    technique);
                            }

                            InitializeTechnique(l_Shader);

                            if (l_pPPG->nb_textures == -2)
                            {
							    for (int loop = 0; loop < 16; loop++)
							    {
                                    m_Textures[loop].m_GLTexture = -1;
                                    m_Textures[loop].m_Name.clear();
							    }
                                m_TextureCount = 0;
                            }

                            CRefArray l_ParamArray;
                            for (int loop = 0; loop <  m_CPSet.GetParameters().GetCount(); loop++)
                                l_ParamArray.Add(m_CPSet.GetParameters()[loop]);
                
                            InitializeTweakables(l_Shader, l_ParamArray);

                            for (int loop = 0; loop <  l_ParamArray.GetCount(); loop++)
                            {
                                Parameter param = l_ParamArray[loop];
                                Logf(
                                    siInfoMsg,
                                    "Removed param: \"%s\".",
                                    param.GetName().GetAsciiString());
                                m_CPSet.GetParameters().Remove(l_ParamArray[loop]);
                            }

                            InitializeLayout(l_Shader, l_pPPG);
                        }
                    }
                    else
                    {
                        CRefArray l_Params =  m_CPSet.GetParameters();
					    int currenttexture = 0;
                        for (int loop = 0; loop < l_Params.GetCount(); loop++)
                        {
                            Parameter l_Param = (Parameter) l_Params[loop];
                            CValue l_Value = l_Param.GetValue();

                            if (l_Param.GetValueType() == CValue::siString)
                            {
                                CString l_ValueString = l_Value;
                                if (strcmp(l_ValueString.GetAsciiString(), "[sampler2D]")   == 0
                                ||  strcmp(l_ValueString.GetAsciiString(), "[samplerCUBE]") == 0)
                                {
                                    const char* name = l_Param.GetName().GetAsciiString();
                                    m_Textures[currenttexture++].m_Name = name;
                                }
                            }
                        }

                        InitializeLayout(l_Shader, l_pPPG);

                        if (in_AllowCreate)
                        {
                            // Just making sure the value will be different...
                            // in case we get called multiple time between the times the PPG
                            // structure gets updated.
                            l_Shader.PutParameterValue(L"refresh_flag", ++(l_pPPG->refresh_flag));
                        }
                    }
                }
            }

            // Flush the parameter cache.
            Clear();

            // scan the semantics data
            XSI_RTS_InitializeSemantics_inuse(m_Semantics);

            if (m_Effect)
            {
                CGparameter l_Param = cgGetFirstEffectParameter(m_Effect);
                while (l_Param)
                {
                    const char *l_pSemantic = cgGetParameterSemantic(l_Param);
                    if (l_pSemantic == NULL || l_pSemantic[0] == 0)
                        l_pSemantic = cgGetParameterName(l_Param);

                    bool l_bHasSemantic = XSI_RTS_RegisterSemantics(
                        Cg_GetSemantics(), m_Semantics, (char *)l_pSemantic, l_Param);
                    if (!l_bHasSemantic)
                    {
                        CGparameterclass l_ParamClass = cgGetParameterClass(l_Param);
                        switch (l_ParamClass)
                        {
                            case CG_PARAMETERCLASS_SAMPLER:
                            {
                                m_Textures[m_TextureCount++].m_CGTexture = l_Param;
                                break;
                            }

                            case CG_PARAMETERCLASS_SCALAR:
                            {
                                CgParameterCacheEntry entry;
                                entry.m_CgParameter = l_Param;
                                FindParameter(
                                    cgGetParameterName(l_Param),
                                    entry.m_XSIParameter[0]);
                                m_CGXSIParams.push_back(entry);
                                break;
                            }

                            case CG_PARAMETERCLASS_VECTOR:
                            {
                                CgParameterCacheEntry entry;
                                entry.m_CgParameter = l_Param;

                                char l_ParamName[2048];
                                sprintf(l_ParamName, "%s_X", cgGetParameterName(l_Param));
                                FindParameter(l_ParamName, entry.m_XSIParameter[0]);
                                sprintf(l_ParamName, "%s_Y", cgGetParameterName(l_Param));
                                FindParameter(l_ParamName, entry.m_XSIParameter[1]);
                                sprintf(l_ParamName, "%s_Z", cgGetParameterName(l_Param));
                                FindParameter(l_ParamName, entry.m_XSIParameter[2]);
                                sprintf(l_ParamName, "%s_W", cgGetParameterName(l_Param));
                                FindParameter(l_ParamName, entry.m_XSIParameter[3]);

                                m_CGXSIParams.push_back(entry);
                                break;
                            }
                        }
                    }

                    l_Param = cgGetNextParameter(l_Param);
                }
            }

            m_UIUpdateInProgress = false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Returns the name of the file selected in the ppg combobox
    //---------------------------------------------------------------------------
    std::string GamebryoFXInstanceData::GetSelectedFileName(GamebryoFXPPG* l_pPPG)
    {
        std::string fullShaderPath;
        if (l_pPPG->selected_shader && l_pPPG->selected_shader[0])
        {
            // Retrieve the path to the folder containing all the shaders.
            // Typically, this will point to "%EGB_PATH%\Sdk\Win32\Shaders".
            char* pPath = getenv ("EGB_SHADER_LIBRARY_PATH");
            if (pPath)
            {
                fullShaderPath = pPath;
                fullShaderPath += "\\data\\";
                fullShaderPath += l_pPPG->selected_shader;
            }
            else
            {
                fullShaderPath = l_pPPG->selected_shader;
            }
        }

        return fullShaderPath;
    }

    //---------------------------------------------------------------------------
    // Bind CG parameters to XSI parameters. Creates the XSI parameters.
    //---------------------------------------------------------------------------
    bool GamebryoFXInstanceData::BindParameters(
        XSI_RTS_SystemParams *in_pSysParams,
        GamebryoFXPPG *in_pPPG)
    {
        double frame = CTime().GetFrameRate() * in_pSysParams->m_SceneData->m_fCurrentTime;

        //  Bind non textures first.
        for (int loop = 0; loop < (int) m_CGXSIParams.size(); loop++)
        {
            CGparameter l_Param = m_CGXSIParams[loop].m_CgParameter;

            CString l_ParamName;
            l_ParamName.PutAsciiString(cgGetParameterName(l_Param));

            CValue           l_XSIParamValue  = m_CPSet.GetParameterValue(l_ParamName, frame);
            CGparameterclass l_ParamClass = cgGetParameterClass(l_Param);
            CGtype           l_ParamType  = cgGetParameterType(l_Param);

            // check by class
            if(l_ParamClass == CG_PARAMETERCLASS_SCALAR)
            {
                switch (l_ParamType)
                {
                    case CG_HALF:
                    case CG_FLOAT:
                    case CG_FIXED:
                    {
                        float l_fValue = l_XSIParamValue;
                        cgSetParameterValuefc(l_Param, 1, &l_fValue);
                        break;
                    }

                    case CG_BOOL:
                    case CG_INT:
                    {
                        int l_nValue = (LONG) l_XSIParamValue;
                        cgSetParameterValueic(l_Param, 1, &l_nValue);
                        break;
                    }

                    default:
                    {
                        Logf(siErrorMsg, "GamebryoFX: parameter \"%s\" (%s) is not supported.",
                            l_ParamName.GetAsciiString(), cgGetTypeString(l_ParamType));
                        break;
                    }
                }
            }
            else if (l_ParamClass == CG_PARAMETERCLASS_OBJECT)
            {
                switch (l_ParamType)
                {
                    case CG_STRING:
                    {
                        CString l_String = l_XSIParamValue;
                        cgSetStringParameterValue(l_Param, l_String.GetAsciiString());
                        break;
                    }

                    case CG_TEXTURE:
                    {
                        // Do nothing, but it is supported.
                        // The texture will be handled by th etexture binding code below.
                        break;
                    }

                    default:
                    {
                        Logf(siErrorMsg, "GamebryoFX: parameter \"%s\" (%s) is not supported.",
                            l_ParamName.GetAsciiString(), cgGetTypeString(l_ParamType));
                        break;
                    }
                }
            }
            else if (l_ParamClass == CG_PARAMETERCLASS_VECTOR
            ||       l_ParamClass == CG_PARAMETERCLASS_MATRIX)
            {
                int    l_Columns = cgGetParameterColumns(l_Param);
                int    l_Rows = cgGetParameterRows(l_Param);
                int    l_nbValues = l_Columns * l_Rows;

                GridData    l_Grid(l_XSIParamValue);
                CValueArray l_XSIParamValues;

                if (l_Grid.IsValid())
                {
                    l_XSIParamValues = l_Grid.GetData();
                }
                else
                {
                    l_XSIParamValues.Add(m_CPSet.GetParameterValue(
                        l_ParamName + CString(L"_X"), frame));
                    l_XSIParamValues.Add(m_CPSet.GetParameterValue(
                        l_ParamName + CString(L"_Y"), frame));
                    l_XSIParamValues.Add(m_CPSet.GetParameterValue(
                        l_ParamName + CString(L"_Z"), frame));

                    if (l_Columns > 3)
                        l_XSIParamValues.Add(m_CPSet.GetParameterValue(
                            l_ParamName + CString(L"_W"), frame));
                }

                l_ParamType = cgGetParameterBaseType(l_Param);
                switch (l_ParamType)
                {
                    case CG_HALF:
                    case CG_FLOAT:
                    case CG_FIXED:
                    {
                        std::vector<float> values(l_nbValues);
                        for (int loop = 0; loop < l_nbValues; ++loop)
                        {
                            values.push_back(l_XSIParamValues[loop]);
                        }
                        cgSetParameterValuefc(l_Param, l_nbValues, &values[0]);
                        break;
                    }

                    case CG_INT:
                    case CG_BOOL:
                    {
                        std::vector<int> values;
                        for (int loop = 0; loop < l_nbValues; loop++)
                        {
                            values.push_back((LONG) l_XSIParamValues[loop]);
                        }
                        cgSetParameterValueic(l_Param, l_nbValues, &values[0]);
                        break;
                    }

                    default:
                    {
                        Logf(siErrorMsg, "GamebryoFX: parameter \"%s\" (%s) is not supported.",
                            l_ParamName.GetAsciiString(), cgGetTypeString(l_ParamType));
                        break;
                    }
                }
            }
        }

        // Bind textures.
        int l_CurrentTexture = 0;
        for (int loop = 0; loop < m_TextureCount; loop++)
        {
            CGparameter      l_Param      = m_Textures[loop].m_CGTexture;
            CGparameterclass l_ParamClass = cgGetParameterClass(l_Param);
            if (l_ParamClass == CG_PARAMETERCLASS_SAMPLER)
            {
                CGtype l_ParamType = cgGetParameterType(l_Param);
                if (l_ParamType == CG_SAMPLER2D)
                {
                    // Find the right index.
                    const char* l_pParamName = cgGetParameterName(l_Param);
                    l_CurrentTexture = FindTextureIndex(*in_pPPG, l_pParamName, l_CurrentTexture);
                    if (l_CurrentTexture >= in_pPPG->nb_textures)
                    {
                        // Force an update when some teture cannot be bound,
                        // expecting that they will be valid soon.
                        m_CurrentTime = 0.0;
                        continue;
                    }

                    CGannotation l_Anno = cgGetNamedParameterAnnotation(l_Param, "image");
                    if (cgIsAnnotation(l_Anno))
                    {
                        // procedurally generated images
                        if (strcmp("checkerboard", cgGetStringAnnotationValue(l_Anno)) == 0)
                        {
                            m_Textures[l_CurrentTexture].ResetGLTexture();
                            glGenTextures(1, &m_Textures[l_CurrentTexture].m_GLTexture);
                            cgGLSetupSampler(l_Param, m_Textures[l_CurrentTexture].m_GLTexture);

                            int frequency = 20;
                            CGannotation ann = cgGetNamedParameterAnnotation(l_Param,"frequency");
                            if (ann)
                            {
                                int count = 0;
                                const int *vals = cgGetIntAnnotationValues(ann, &count);
                                assert(count == 1);
                                if (count >= 1)
                                    frequency = *vals;
                            }

                            float color0[3] = { 1, 1, 1 };
                            ann = cgGetNamedParameterAnnotation(l_Param, "color0");
                            if (ann)
                            {
                                int count = 0;
                                const float *vals = cgGetFloatAnnotationValues(ann, &count);
                                assert(count == 3);
                                if (count >= 3)
                                {
                                    color0[0] = vals[0];
                                    color0[1] = vals[1];
                                    color0[2] = vals[2];
                                }
                            }

                            float color1[3] = { 0, 0, 0 };
                            ann = cgGetNamedParameterAnnotation(l_Param, "color1");
                            if (ann)
                            {
                                int count = 0;
                                const float *vals = cgGetFloatAnnotationValues(ann, &count);
                                assert(count == 3);
                                if (count >= 3)
                                {
                                    color1[0] = vals[0];
                                    color1[1] = vals[1];
                                    color1[2] = vals[2];
                                }
                            }

                            static const int RES = 256;
                            std::vector<float> data(RES * RES * 4);
                            float *dp = &data[0];
                            for (int i = 0; i < RES; ++i)
                            {
                                for (int j = 0; j < RES; ++j)
                                {
                                    float u = float(i) / float(RES);
                                    float v = float(j) / float(RES);
                                    u *= frequency;
                                    v *= frequency;
                                    if ((int(u) + int(v)) & 1)
                                    {
                                        *dp++ = color0[0];
                                        *dp++ = color0[1];
                                        *dp++ = color0[2];
                                    }
                                    else
                                    {
                                        *dp++ = color1[0];
                                        *dp++ = color1[1];
                                        *dp++ = color1[2];
                                    }
                                    *dp++ = 1.;
                                }
                            }

                            glEnable(GL_TEXTURE_2D);
                            glBindTexture(
                                GL_TEXTURE_2D,
                                m_Textures[l_CurrentTexture].m_GLTexture);
                            glTexImage2D(
                                GL_TEXTURE_2D,
                                0,
                                GL_RGBA,
                                RES,
                                RES,
                                0,
                                GL_RGBA,
                                GL_FLOAT,
                                &data[0]);

                            glDisable(GL_TEXTURE_2D);
                        }
                    }
                    else if (in_pPPG->textures[l_CurrentTexture].m_lDirty
                    ||       m_Textures[l_CurrentTexture].m_GLTexture == -1)
                    {
                        m_Textures[l_CurrentTexture].ResetGLTexture();
                        glGenTextures(1, &m_Textures[l_CurrentTexture].m_GLTexture);
                        cgGLSetupSampler(l_Param, m_Textures[l_CurrentTexture].m_GLTexture);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, m_Textures[l_CurrentTexture].m_GLTexture);

                        int	l_iDataType = GL_UNSIGNED_BYTE;
                        int l_iTextureTarget = GL_TEXTURE_2D;
                        GLenum format = (in_pPPG->textures[l_CurrentTexture].m_lNbChannels == 3)
                                      ? GL_RGB : GL_RGBA;
                        int nbChannels = in_pPPG->textures[l_CurrentTexture].m_lNbChannels;
                        Logf(
                            siInfoMsg,
                            "GamebryoFX: binding 2D texture \"%s\" (%s).",
                            l_pParamName,
                            cgGetTypeString(l_ParamType));

                        if (in_pPPG->textures[l_CurrentTexture].m_lPixelType == 64)
                        {
                            // check for floating point texture support
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

                        ::gluBuild2DMipmaps(
                            GL_TEXTURE_2D,
                            nbChannels,
                            in_pPPG->textures[l_CurrentTexture].m_lWidth,
                            in_pPPG->textures[l_CurrentTexture].m_lHeight,
                            format,
                            l_iDataType,
                            in_pPPG->textures[l_CurrentTexture].m_pBuffer);

                        glDisable(GL_TEXTURE_2D);
                    }

                    l_CurrentTexture ++;
                }
                else if (l_ParamType == CG_SAMPLERCUBE)
                {
                    // find the right index
                    const char* l_pParamName = cgGetParameterName(l_Param);
                    l_CurrentTexture = FindTextureIndex(*in_pPPG, l_pParamName, l_CurrentTexture);
                    if (l_CurrentTexture >= in_pPPG->nb_textures)
                    {
                        // Force an update when some teture cannot be bound,
                        // expecting that they will be valid soon.
                        m_CurrentTime = 0.0;
                        continue;
                    }

                    CGannotation annot = cgGetNamedParameterAnnotation(l_Param, "image");
                    if (cgIsAnnotation(annot))
                    {
                        // Nothing... ?
                    }
                    else if (in_pPPG->textures[l_CurrentTexture].m_lDirty
                    ||       m_Textures[l_CurrentTexture].m_GLTexture == -1)
                    {
                        GLint sides[6] =
                        {
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
                        };

                        m_Textures[l_CurrentTexture].ResetGLTexture();

                        Logf(
                            siInfoMsg,
                            "GamebryoFX: binding cubic texture \"%s\" (%s).",
                            l_pParamName, cgGetTypeString(l_ParamType));
                        glGenTextures(1, &m_Textures[l_CurrentTexture].m_GLTexture);
                        cgGLSetupSampler(l_Param, m_Textures[l_CurrentTexture].m_GLTexture);
                        glEnable(GL_TEXTURE_CUBE_MAP_ARB);
                        glBindTexture(
                            GL_TEXTURE_CUBE_MAP_ARB,
                            m_Textures[l_CurrentTexture].m_GLTexture);

                        int	l_iDataType = GL_UNSIGNED_BYTE;
                        int l_iTextureTarget = GL_TEXTURE_2D;
                        GLenum format = (in_pPPG->textures[l_CurrentTexture].m_lNbChannels == 3)
                                      ? GL_RGB : GL_RGBA;
                        int nbChannels = in_pPPG->textures[l_CurrentTexture].m_lNbChannels;

                        if (in_pPPG->textures[l_CurrentTexture].m_lPixelType == 64)
                        {
                            //
                            // check for floating point texture support
                            //
                            if (in_pSysParams->m_pGLExt->_GL_ATI_texture_float)
                            {
                                format = GL_RGBA_FLOAT32_ATI;
                                l_iDataType = GL_FLOAT;
                            }

                            if (in_pSysParams->m_pGLExt->_GL_NV_float_buffer)
                            {
                                format = GL_FLOAT_RGB32_NV;
                                l_iDataType = GL_FLOAT;
                            }
                        }

                        const int width  = in_pPPG->textures[l_CurrentTexture].m_lWidth / 6;
                        const int height = in_pPPG->textures[l_CurrentTexture].m_lHeight;
                        const int linesize = in_pPPG->textures[l_CurrentTexture].m_lNbChannels
                                           * ((l_iDataType == GL_FLOAT) ? 4 : 1)
                                           * width;
                        unsigned char *l_pSideBuffer = new unsigned char[height * linesize];
                        for (int loop = 0; loop < 6; loop++)
                        {
                            const int skip = linesize * loop;
                            unsigned char *pSrc = in_pPPG->textures[l_CurrentTexture].m_pBuffer
                                                + skip;

                            // Note: we're copying upside-down, for some unknown reason...?
                            unsigned char *pDst = l_pSideBuffer + (linesize * (height-1));
                            for (int loop2 = 0; loop2 < height; loop2++)
                            {
                                memcpy(pDst, pSrc, linesize);
                                pDst -= linesize;
                                pSrc += linesize * 6;
                            }

                            ::gluBuild2DMipmaps(
                                sides[loop],
                                nbChannels,
                                width,
                                height,
                                format,
                                l_iDataType,
                                l_pSideBuffer);
                        }

                        delete[] l_pSideBuffer;

                        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                    }

                    l_CurrentTexture ++;
                }
                else
                {
                    Application l_App;
                    Logf(siWarningMsg, "GamebryoFX: parameter \"%s\" (%s) is not supported.",
                        cgGetParameterName(l_Param), cgGetTypeString(l_ParamType));
                }
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    void GamebryoFXInstanceData::InitializeTechnique(Shader &in_Shader)
    {
        // Check the techniques.
        CGtechnique l_Technique = cgGetFirstTechnique(m_Effect);
        CString l_TechniqueName;
        l_TechniqueName.PutAsciiString(cgGetTechniqueName(l_Technique));

        // Grab the old technique name, go through the existing techniques
        // and see if they match.
        CString l_OldTechniqueName = m_CPSet.GetParameterValue(L"Technique");
        if (!(l_Technique = cgGetNamedTechnique(m_Effect, l_OldTechniqueName.GetAsciiString())))
        {
            // Set the name of the technique to be used.
            m_CPSet.PutParameterValue(L"Technique", l_TechniqueName);
        }
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    struct ltstr
    {
        bool operator()(const CString s1, const CString s2) const
        {
            return wcscmp(s1.GetWideString(), s2.GetWideString()) < 0;
        }
    };

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    void GamebryoFXInstanceData::InitializeTweakables(
        Shader &in_Shader,
        CRefArray &in_OldParams)
    {
        int  l_CurrentTarget = 0;
        bool l_bProceduralTextures[16] =
        {
            false,false,false,false,
            false,false,false,false,
            false,false,false,false,
            false,false,false,false
        };

	    for(int loop = 0; loop < 16; loop++)
            m_Textures[loop].ResetGLTexture();

        // create a mapping of (display names) to (ports' old image clips)
        std::map<CString, ImageClip2, ltstr> mapOldDNToImgClip;

        wchar_t sTexture[32];
        for (int i = 0; i < 16; i++)
        {
            swprintf(sTexture, sizeof(sTexture) / sizeof(sTexture[0]), L"Texture_%d", i);
            CString csDisplayName = in_Shader.GetPortDisplayName(sTexture);
            CRef refImageClip = in_Shader.GetParameter(sTexture).GetSource();
            ImageClip2 icTmp;
            if (refImageClip.IsA(icTmp.GetClassID()))
            {
                ImageClip2 icImageClip(refImageClip);
                CString csImageClipName = icImageClip.GetFullName();
                if (!csDisplayName.IsEmpty() && !csImageClipName.IsEmpty())
                {
                    mapOldDNToImgClip[csDisplayName] = icImageClip;
                }
            }

            // Need to disconnect all ports anyways.
            //
            // NOOOOOOOOOOOOOOOO! This invalidates the PPG which leads to a rapid crash.
            // XSI V7 also disabled this code... o we're in good company. Sorta.
            //
            // CRef ref;
            // in_Shader.GetParameter(sTexture).Disconnect(ref);
        }

        XSI_RTS_FXSemantics_inuse l_SemanticsInUse;
        int nbTextures = 0;

        CGparameter l_Param = cgGetFirstEffectParameter(m_Effect);
        while (l_Param)
        {
            const char* l_pSemantic = cgGetParameterSemantic(l_Param);
            CGtype l_ParamType  = cgGetParameterType(l_Param);

            // use the registration mechanism to see if that semantic is handled by the host app.
            bool l_IsSemanticSupported = false;
            if (l_pSemantic)
            {
               l_IsSemanticSupported = XSI_RTS_RegisterSemantics(
                   Cg_GetSemantics(),
                   l_SemanticsInUse,
                   (char *)l_pSemantic,
                   NULL);
            }

            if (!l_IsSemanticSupported)
            {
                bool             l_bSupported = false;
                CGparameterclass l_ParamClass = cgGetParameterClass(l_Param);

                CString l_ParamName;

                const char *l_pCgParamName = cgGetParameterName(l_Param);
                l_ParamName.PutAsciiString(l_pCgParamName);

                // check by class
                if (l_ParamClass == CG_PARAMETERCLASS_SCALAR)
                {
                    Parameter           l_XSIParameter;
                    CValue::DataType    l_XSIParameterType;
                    CValue              l_XSIParamValue;
                    CValue              l_XSIUIMin, l_XSIUIMax;
                    bool                l_bUIMinMax = false;

                    switch (l_ParamType)
                    {
                        case CG_HALF:
                        case CG_FLOAT:
                        case CG_FIXED:
                        {
                            l_XSIParameterType = CValue::siFloat;

                            float l_fValue;
                            cgGetParameterValuefc(l_Param, 1, &l_fValue);
                            l_XSIParamValue = l_fValue;

                            CGannotation uiMin = cgGetNamedParameterAnnotation(l_Param, "UIMin");
                            CGannotation uiMax = cgGetNamedParameterAnnotation(l_Param, "UIMax");
                            if (cgIsAnnotation(uiMin) && cgIsAnnotation(uiMax))
                            {
                                int l_NbValues;
                                l_fValue = cgGetFloatAnnotationValues(uiMin ,&l_NbValues)[0];
                                l_XSIUIMin = l_fValue;
                                l_fValue = cgGetFloatAnnotationValues(uiMax ,&l_NbValues)[0];
                                l_XSIUIMax = l_fValue;
                                l_bUIMinMax = true;
                            }

                            l_bSupported = true;
                            break;
                        }

                        case CG_INT:
                        {
                            l_XSIParameterType = CValue::siInt4;

                            int l_nValue;
                            cgGetParameterValueic(l_Param, 1, &l_nValue);
                            l_XSIParamValue = (LONG)l_nValue;

                            CGannotation uiMin = cgGetNamedParameterAnnotation(l_Param, "UIMin");
                            CGannotation uiMax = cgGetNamedParameterAnnotation(l_Param, "UIMax");
                            if (cgIsAnnotation(uiMin) && cgIsAnnotation(uiMax))
                            {
                                int l_NbValues;
                                l_nValue = cgGetIntAnnotationValues(uiMin ,&l_NbValues)[0];
                                l_XSIUIMin = (LONG) l_nValue;
                                l_nValue = cgGetIntAnnotationValues(uiMax ,&l_NbValues)[0];
                                l_XSIUIMax = (LONG) l_nValue;
                                l_bUIMinMax = true;
                            }

                            l_bSupported = true;
                            break;
                        }

                        case CG_BOOL:
                        {
                            l_XSIParameterType = CValue::siBool;

                            int l_nValue;
                            cgGetParameterValueic(l_Param, 1, &l_nValue);
                            l_XSIParamValue = l_nValue ? true : false;

                            l_bSupported = true;
                            break;
                        }
                    }

                    if (l_bSupported)
                    {
                        l_XSIParameter = m_CPSet.GetParameter(l_ParamName);
                        if (l_XSIParameter.IsValid()      == FALSE
                        ||  l_XSIParameter.GetValueType() != l_XSIParameterType)
                        {
                            if (l_bUIMinMax)
                            {
                                m_CPSet.AddParameter(
                                    l_ParamName,
                                    l_XSIParameterType,
                                    siPersistable|siAnimatable,
                                    l_ParamName,
                                    l_ParamName,
                                    l_XSIParamValue,
                                    l_XSIUIMin,
                                    l_XSIUIMax,
                                    l_XSIUIMin,
                                    l_XSIUIMax,
                                    l_XSIParameter);
                            }
                            else
                            {
                                m_CPSet.AddParameter(
                                    l_ParamName,
                                    l_XSIParameterType,
                                    siPersistable|siAnimatable,
                                    l_ParamName,
                                    l_ParamName,
                                    l_XSIParamValue,
                                    l_XSIParameter);
                            }
                        }
                        else
                        {
                            in_OldParams.Remove(l_XSIParameter);
                        }
                    }
                    else
                    {
                        Logf(siWarningMsg, "GamebryoFX: parameter \"%s\" (%s) is not supported.",
                            l_ParamName.GetAsciiString(), cgGetTypeString(l_ParamType));
                    }
                }
                else if (l_ParamClass == CG_PARAMETERCLASS_SAMPLER)
                {
                    nbTextures++;

                    CString l_XSIParameterSamplerType;
                    if (l_ParamType == CG_SAMPLER2D)
                    {
                        l_XSIParameterSamplerType.PutAsciiString("[sampler2D]");
                        l_bSupported = true;
                    }
                    else if (l_ParamType == CG_SAMPLERCUBE)
                    {
                        l_XSIParameterSamplerType.PutAsciiString("[samplerCUBE]");
                        l_bSupported = true;
                    }

                    CGannotation annot = cgGetNamedParameterAnnotation(l_Param, "image");
                    if (cgIsAnnotation(annot))
                    {
                        // procedurally generated images
                        if (strcmp("checkerboard", cgGetStringAnnotationValue(annot)) == 0)
                        {
                            l_bSupported = true;
                            l_bProceduralTextures[l_CurrentTarget] = true;
                            l_CurrentTarget++;
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        // Grab the name of the texture port we're setting up.
                        CString l_TextureName(L"Texture_");
                        l_TextureName += CString(CValue((LONG)l_CurrentTarget));

                        // Grab the display name to be associated with this port.
                        CString l_DisplayName = l_ParamName;
					    m_Textures[l_CurrentTarget].m_Name = cgGetParameterName(l_Param);

                        // If this display name occurred in the previous setting,
                        // then use it for this port.
                        if (mapOldDNToImgClip.find(l_DisplayName) != mapOldDNToImgClip.end())
                        {
                            // We've disconnected all port.
                            // Reconnect this one to the saved ImageClip2.
                            CValueArray args2(3);
                            // The image clip of interest.
                            args2[0] = mapOldDNToImgClip[l_DisplayName].GetFullName();
                            args2[1] = in_Shader.GetFullName() + CString(L".") + l_TextureName;
                            args2[2] = true;

                            CValue retval;
                            Application app;
                            app.ExecuteCommand(L"SIConnectShaderToCnxPoint", args2, retval);
                        }
                        else 
                        {
                            // Try to load from the ResourceName;
                            // if that doesn't work, load a default.
                            CGstateassignment stateAss =
                                cgGetFirstSamplerStateAssignment(l_Param);
                            if (stateAss)
                            {
                                CGparameter  texParam =
                                    cgGetTextureStateAssignmentValue(stateAss);
                                CGannotation annot = cgGetNamedParameterAnnotation(
                                    texParam, "ResourceName");
                                if (cgIsAnnotation(annot))
                                {
                                    CString l_XSIResourceName;
                                    l_XSIResourceName.PutAsciiString(
                                        cgGetStringAnnotationValue(annot));
                                    Logf(
                                        siInfoMsg,
                                        "GamebryoFX: initializing texture \"%s\" (%s).",
                                        cgGetParameterName(texParam),
                                        cgGetTypeString(l_ParamType));

                                    CString l_ShaderName;
                                    l_ShaderName.PutAsciiString(
                                        cgGetParameterName(texParam));
                                    m_Textures[l_CurrentTarget].m_Name =
                                        cgGetParameterName(l_Param);

                                    if (_access(l_XSIResourceName.GetAsciiString(), 4) != 0)
                                    {
                                        CString l_Filename;
                                        char l_FXPath[2048];
                                        sprintf(l_FXPath, "%s", m_FileName);

                                        // convert all / to \

                                        int strcount = (int) strlen(l_FXPath);
                                        for(int strloop = 0; strloop < strcount; strloop++)
                                        {
                                            if(l_FXPath[strloop] == '/')
                                                l_FXPath[strloop] = '\\';
                                        }

                                        char *l_pTerminator = strrchr(l_FXPath, '\\');
                                        if(l_pTerminator == NULL)
                                            l_pTerminator = strrchr(l_FXPath, '/');

                                        if(l_pTerminator != NULL)
                                            *(l_pTerminator+1) = '\0';

                                        l_Filename.PutAsciiString(l_FXPath);
                                        l_Filename += l_XSIResourceName;
                                        l_XSIResourceName = l_Filename;

                                        if(_access(l_XSIResourceName.GetAsciiString(), 4) != 0)
                                        {
                                            Logf(
                                                siWarningMsg,
                                                "The resource %s was not found,"
                                                " replacing with default image.",
                                                l_XSIResourceName.GetAsciiString());

									        Application app;
									        CString pathToNoIcon = app.GetInstallationPath(siFactoryPath);
									        CString noIcon;
									        noIcon.PutAsciiString("/rscr/noIcon.pic");
									        pathToNoIcon += noIcon;
									        l_XSIResourceName = pathToNoIcon;
                                        }
                                    }

                                    Application app;
                                    CValue retval;
                                    {
                                        Preferences preferences = app.GetPreferences();
                                        XSI::CValue l_AutoInspect;
                                        preferences.GetPreferenceValue(
                                            L"Interaction.autoinspect", l_AutoInspect);
                                        preferences.SetPreferenceValue(
                                            L"Interaction.autoinspect", false);

                                        CValueArray args(1);
                                        args[0] = l_XSIResourceName ;
                                        app.ExecuteCommand(L"CreateImageClip", args, retval);

                                        preferences.SetPreferenceValue(
                                            L"Interaction.autoinspect", l_AutoInspect);
                                    }

                                    ImageClip2 newClip(retval);

                                    CValueArray args2(3);
                                    args2[0] = newClip.GetFullName();
                                    args2[1] = in_Shader.GetFullName() + L"." + l_TextureName;
                                    args2[2] = true;

                                    app.ExecuteCommand(
                                        L"SIConnectShaderToCnxPoint", args2, retval);
                                }
                            }
                        }

                        Parameter l_XSIParameter;
                        l_XSIParameter = m_CPSet.GetParameter(l_ParamName);
                        if (l_XSIParameter.IsValid() == FALSE
                        ||  l_XSIParameter.GetValueType() != CValue::siString)
                        {
                            m_CPSet.AddParameter(
                                l_ParamName,
                                CValue::siString,
                                siPersistable|siAnimatable,
                                l_ParamName,
                                l_ParamName,
                                l_XSIParameterSamplerType,
                                l_XSIParameter);
                        }
                        else
                        {
                            in_OldParams.Remove(l_XSIParameter);
                        }

                        // fill the texture name in for display in the rendertree
                        if (CStatus::OK!=in_Shader.PutPortDisplayName(l_TextureName, l_ParamName))
                        {
                            Logf(
                                siWarningMsg,
                                "Cannot set display port name for \"%s\" to \"%s\".",
                                l_TextureName.GetAsciiString(), l_ParamName.GetAsciiString());
                        }

                        l_CurrentTarget ++;
                    }
                }
                else if (l_ParamClass == CG_PARAMETERCLASS_OBJECT)
                {
                    Parameter        l_XSIParameter;
                    CValue::DataType l_XSIParameterType;
                    CValue           l_XSIParamValue;
                    bool             l_bIsTexture = false;

                    if(l_ParamType == CG_STRING)
                    {
                        l_XSIParameterType = CValue::siString;
                        CString l_String;
                        l_String.PutAsciiString(cgGetStringParameterValue(l_Param));
                        l_XSIParamValue = l_String;
                        l_bSupported = true;
                    }
                    else if(l_ParamType == CG_TEXTURE)
                    {
                        l_bSupported = true;
                        l_bIsTexture = true;
                    }

                    if(l_bSupported)
                    {
                        // Skip tetures because we bind to samplers.
                        if(!l_bIsTexture)
                        {
                            l_XSIParameter = m_CPSet.GetParameter(l_ParamName);
                            if (l_XSIParameter.IsValid() == FALSE
                            ||  l_XSIParameter.GetValueType() != l_XSIParameterType)
                            {
                                m_CPSet.AddParameter(
                                    l_ParamName,
                                    l_XSIParameterType,
                                    siPersistable|siAnimatable,
                                    l_ParamName,
                                    l_ParamName,
                                    l_XSIParamValue,
                                    l_XSIParameter);
                            }
                            else
                            {
                                in_OldParams.Remove(l_XSIParameter);
                            }
                        }
                    }
                    else
                    {
                        Logf(siWarningMsg, "GamebryoFX: parameter \"%s\" (%s) is not supported.",
                            l_ParamName.GetAsciiString(), cgGetTypeString(l_ParamType));
                    }
                }
                else if (l_ParamClass == CG_PARAMETERCLASS_VECTOR
                ||       l_ParamClass == CG_PARAMETERCLASS_MATRIX)
                {
                    Parameter           l_XSIParameter;
                    CValue::DataType    l_XSIParameterType;
                    CValueArray         l_XSIParamValue;
                    int                 l_Columns = cgGetParameterColumns(l_Param);
                    int                 l_Rows = cgGetParameterRows(l_Param);
                    int                 l_nbValues = l_Columns * l_Rows;

                    l_ParamType = cgGetParameterBaseType(l_Param);
                    if ((l_ParamType == CG_HALF) ||
                        (l_ParamType == CG_FLOAT) ||
                        (l_ParamType == CG_FIXED))
                    {
                        l_XSIParameterType = CValue::siFloat;
                        float l_fValue[16];
                        cgGetParameterValuefc(l_Param, l_nbValues, l_fValue);
                        for(int loop = 0; loop < l_nbValues; loop++)
                        {
                            l_XSIParamValue.Add(l_fValue[loop]);
                        }
                        l_bSupported = true;
                    }
                    else if (l_ParamType == CG_INT)
                    {
                        l_XSIParameterType = CValue::siInt4;
                        int l_nValue[16];
                        cgGetParameterValueic(l_Param, l_nbValues, l_nValue);
                        for(int loop = 0; loop < l_nbValues; loop++)
                        {
                            l_XSIParamValue.Add((LONG)l_nValue[loop]);
                        }
                        l_bSupported = true;
                    }
                    else if (l_ParamType == CG_BOOL)
                    {
                        int l_nValue[16];
                        l_XSIParameterType = CValue::siBool;

                        cgGetParameterValueic(l_Param, l_nbValues, l_nValue);
                        for(int loop = 0; loop < l_nbValues; loop++)
                        {
                            l_XSIParamValue.Add(l_nValue[loop] ? true : false);
                        }
                        l_bSupported = true;
                    }

                    if (l_bSupported)
                    {
                        // check if we use the grid by default or if we have a UIWidget annotation
                        CGannotation annot = cgGetNamedParameterAnnotation(l_Param, "UIWidget");
                        if(annot || (l_Rows == 1))
                        {
                            CString l_ParamNameWithSuffix;

                            for(int loop = 0; loop < l_nbValues; loop++)
                            {
                                wchar_t suffixes[4] = {'X', 'Y', 'Z', 'W'};

                                l_ParamNameWithSuffix = l_ParamName;
                                l_ParamNameWithSuffix += L"_";
                                l_ParamNameWithSuffix += suffixes[loop];

                                l_XSIParameter = m_CPSet.GetParameter(l_ParamNameWithSuffix);
                                if (l_XSIParameter.IsValid() == FALSE
                                ||  l_XSIParameter.GetValueType() != l_XSIParameterType)
                                {
                                    m_CPSet.AddParameter(
                                        l_ParamNameWithSuffix,
                                        l_XSIParameterType,
                                        siPersistable|siAnimatable,
                                        l_ParamNameWithSuffix,
                                        l_ParamNameWithSuffix,
                                        l_XSIParamValue[loop],
                                        CValue(),
                                        CValue(),
                                        CValue(-100.0f),
                                        CValue(100.0f),
                                        l_XSIParameter);
                                }
                                else
                                {
                                    in_OldParams.Remove(l_XSIParameter);
                                }
                            }
                        }
                        else
                        {
                            l_XSIParameter = m_CPSet.GetParameter(l_ParamName);
                            if (l_XSIParameter.IsValid() == FALSE
                            ||  l_XSIParameter.GetValueType() != CValue::siRef)
                            {
                                l_XSIParameter = m_CPSet.AddGridParameter(l_ParamName);
                            }

                            GridData l_Grid( l_XSIParameter.GetValue() );
                            l_Grid.PutColumnCount(l_Columns);
                            l_Grid.PutRowCount(l_Rows);

                            // set the labels
                            if(l_Rows == 1)
                            {
                                wchar_t *l_ColumnLabels[4] =
                                {
                                    L"   X   ",
                                    L"   Y   ",
                                    L"   Z   ",
                                    L"   W   "
                                };
                                for(int loop = 0; loop < l_Columns; loop++)
                                {
                                    l_Grid.PutColumnLabel(loop, l_ColumnLabels[loop]);
                                }
                                l_Grid.PutRowLabel(0, L"Value ");
                            }
                            else
                            {
                                wchar_t *l_ColumnLabels[4] =
                                {
                                    L"    0    ",
                                    L"    1    ",
                                    L"    2    ", 
                                    L"    3    "
                                };
                                for(int loop = 0; loop < l_Columns; loop++)
                                {
                                    l_Grid.PutColumnLabel(loop, l_ColumnLabels[loop]);
                                }
                                for(int loop = 0; loop < l_Rows; loop++)
                                {
                                    l_Grid.PutRowLabel(loop, l_ColumnLabels[loop]);
                                }
                            }

                            l_Grid.PutData(l_XSIParamValue);
                        }
                    }
                }

                if(!l_bSupported)
                {
                    Logf(siWarningMsg, "GamebryoFX: \"%s\" (%s) could not be created.",
                        cgGetParameterName(l_Param), cgGetTypeString(l_ParamType));
                }
            }
            else
            {
                Logf(siInfoMsg, "GamebryoFX: \"%s\" (%s) is bound to the semantic \"%s\".",
                    cgGetParameterName(l_Param),
                    cgGetTypeString(l_ParamType), l_pSemantic);
            }

            l_Param = cgGetNextParameter(l_Param);
        }

        CParameterRefArray l_Params = in_Shader.GetParameters();
        for(int loop = 0; loop < l_CurrentTarget; loop++)
        {
            CString texParam(L"Texture_");
            texParam += CString(loop);
            Parameter l_Param = l_Params.GetItem(texParam);
            l_Param.PutCapabilityFlag(siReadOnly, l_bProceduralTextures[loop]);
        }

        in_Shader.PutParameterValue(L"nb_textures", (LONG)nbTextures);
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    bool GamebryoFXInstanceData::IsLayoutInitialized() const
    {
        return m_LayoutInitialized;
    }

    //---------------------------------------------------------------------------
    // Create the UI based on CG semantics.
    //---------------------------------------------------------------------------
    void GamebryoFXInstanceData::InitializeLayout(
        Shader &in_Shader,
        GamebryoFXPPG *in_pPPG)
    {
        PPGLayout           l_PPGLayout = m_CPSet.GetPPGLayout();
        PPGLayout           l_ShaderPPGLayout = in_Shader.GetPPGLayout();
        CParameterRefArray  l_Params = m_CPSet.GetParameters();
        int                 l_CurrentTarget = 0;
        CValueArray         l_TechniqueNames;

        // clear the layout
        l_PPGLayout.Clear();

        // check the techniques
        CGtechnique l_Technique = cgGetFirstTechnique(m_Effect);
        while (l_Technique)
        {
            if (cgValidateTechnique(l_Technique) == CG_FALSE)
            {
                Logf(siWarningMsg, "GamebryoFX: technique \"%s\" did not validate.",
                    cgGetTechniqueName(l_Technique));
            }

            CString l_TechniqueName;
            l_TechniqueName.PutAsciiString(cgGetTechniqueName(l_Technique));
            l_TechniqueNames.Add(l_TechniqueName);
            l_TechniqueNames.Add(l_TechniqueName);

            l_Technique = cgGetNextTechnique(l_Technique);
        }

        l_PPGLayout.AddEnumControl(L"Technique", l_TechniqueNames);

        CGparameter l_Param = cgGetFirstEffectParameter(m_Effect);
        XSI_RTS_FXSemantics_inuse l_SemanticsInUse;
        int nbTextures = 0;

        while (l_Param)
        {
            const char* l_pSemantic = cgGetParameterSemantic(l_Param);
            // use the registration mechanism to see if that semantic is handled by the host app
            bool l_IsSemanticSupported = false;
            if (l_pSemantic)
            {
                l_IsSemanticSupported = XSI_RTS_RegisterSemantics(
                    Cg_GetSemantics(), l_SemanticsInUse, (char *)l_pSemantic, NULL);
            }

            // check the visibility option which defaults to visible
            int l_uiVisible = 1;
            CGannotation l_VisibleAnnotation = cgGetNamedParameterAnnotation(
                                                    l_Param, "UIVisible");
            if(cgIsAnnotation(l_VisibleAnnotation))
            {
                int count = 0;
                const int *vals = cgGetBooleanAnnotationValues(l_VisibleAnnotation, &count);
                if (count > 0)
                    l_uiVisible = vals[0];
            }

            if ( l_uiVisible && (!l_IsSemanticSupported) )
            {
                CGparameterclass l_ParamClass = cgGetParameterClass(l_Param);
                CString          l_ParamName;
                CString          l_UIName;
                CGannotation     annot = cgGetNamedParameterAnnotation(l_Param, "UIName");

                l_ParamName.PutAsciiString(cgGetParameterName(l_Param));

                if(annot)
                    l_UIName.PutAsciiString(cgGetStringAnnotationValue(annot));
                else
                    l_UIName.PutAsciiString(cgGetParameterName(l_Param));

                // check by class
                if(l_ParamClass == CG_PARAMETERCLASS_SCALAR)
                {
                    Parameter			l_XSIParameter = l_Params.GetItem(l_ParamName);
                    l_PPGLayout.AddItem(l_XSIParameter.GetName(), l_UIName);
                }
                else if(l_ParamClass == CG_PARAMETERCLASS_SAMPLER)
                {
                    CGannotation annot;

                    annot = cgGetNamedParameterAnnotation(l_Param, "image");
                    Logf(siInfoMsg, "GamebryoFX: initializing sampler \"%s\" (%s).",
                        cgGetParameterName(l_Param),
                        cgGetTypeString(cgGetParameterType(l_Param)));
                    nbTextures ++;
                    const char* l_pParamName = cgGetParameterName(l_Param);

                    // find the right index
                    l_CurrentTarget = 0;
                    for(int loop = 0; loop < 16; loop++)
                    {
                        if(m_Textures[loop].m_Name.empty())
                        {
                            l_CurrentTarget = loop;
                            break;
                        }
                        else if(m_Textures[loop].m_Name.compare(l_pParamName) == 0)
                        {
                            l_CurrentTarget = loop;
                            break;
                        }
                    }

                    if (cgIsAnnotation(annot))
                    {
                        // procedurally generated images
                        if(strcmp("checkerboard", cgGetStringAnnotationValue(annot)) == 0)
                        {
                            CString l_TextureName(L"Texture_");
                            l_TextureName += CString(l_CurrentTarget);
                            PPGItem l_TexturePPGItem = l_ShaderPPGLayout.GetItem(l_TextureName);
                            l_TexturePPGItem.PutLabel(L"PROCEDURAL (checkerboard)");
                        }
                    }
                    else
                    {
                        CGstateassignment stateAss = cgGetFirstSamplerStateAssignment(l_Param);
                        if (stateAss)
                        {
                            CGparameter  texParam = cgGetTextureStateAssignmentValue(stateAss);
                            CGannotation annot    = cgGetNamedParameterAnnotation(
                                texParam, "ResourceName");
                            if (cgIsAnnotation(annot))
                            {
                                CString l_TextureName(L"Texture_");
                                l_TextureName += CString(l_CurrentTarget);
                                CString l_XSIResourceName;
                                l_XSIResourceName.PutAsciiString(
                                    cgGetStringAnnotationValue(annot));
                                Logf(siInfoMsg, "GamebryoFX: initializing texture \"%s\" (%s).",
                                    cgGetParameterName(texParam),
                                    cgGetTypeString(cgGetParameterType(texParam)));

                                CString l_ShaderName;
                                l_ShaderName.PutAsciiString(cgGetParameterName(texParam));

                                PPGItem l_TexPPGItem = l_ShaderPPGLayout.GetItem(l_TextureName);
                                l_TexPPGItem.PutLabel(l_ShaderName);

                                // Fill the texture name in for display in the rendertree.
                                // Not done in V7, trying without.
                            }
                        }
                    }
                }
                else if (l_ParamClass == CG_PARAMETERCLASS_OBJECT)
                {
                    CGtype l_ParamType = cgGetParameterType(l_Param);
                    if(l_ParamType == CG_STRING)
                    {
                        Parameter l_XSIParameter = l_Params.GetItem(l_ParamName);
                        l_PPGLayout.AddString(
                            l_XSIParameter.GetName(), CValue(), true, (LONG) 40);
                        l_PPGLayout.AddSpacer(0, 5);
                    }
                }
                else if (l_ParamClass == CG_PARAMETERCLASS_VECTOR)
                {
                    CGannotation annot = cgGetNamedParameterAnnotation(
                        l_Param, "UIWidget");
                    if(annot)
                    {
                        int l_Columns = cgGetParameterColumns(l_Param);
                        bool alpha = ((l_Columns < 4) ? false : true);
                        l_PPGLayout.AddColor(l_ParamName + CString(L"_X"), l_ParamName, alpha);
                    }
                    else
                    {
                        PPGItem l_Group = l_PPGLayout.AddGroup(l_UIName);
                        Parameter l_XSIParameter;
                        l_XSIParameter = l_Params.GetItem(l_ParamName + CString(L"_X"));
                        if(l_XSIParameter.IsValid())
                            l_PPGLayout.AddItem(l_ParamName + CString(L"_X"), L"X");
                        l_XSIParameter = l_Params.GetItem(l_ParamName + CString(L"_Y"));
                        if(l_XSIParameter.IsValid())
                            l_PPGLayout.AddItem(l_ParamName + CString(L"_Y"), L"Y");
                        l_XSIParameter = l_Params.GetItem(l_ParamName + CString(L"_Z"));
                        if(l_XSIParameter.IsValid())
                            l_PPGLayout.AddItem(l_ParamName + CString(L"_Z"), L"Z");
                        l_XSIParameter = l_Params.GetItem(l_ParamName + CString(L"_W"));
                        if(l_XSIParameter.IsValid())
                            l_PPGLayout.AddItem(l_ParamName + CString(L"_W"), L"W");
                        l_XSIParameter = l_Params.GetItem(l_ParamName);
                        if(l_XSIParameter.IsValid())
                            l_PPGLayout.AddItem(l_ParamName, l_ParamName);
                        l_PPGLayout.EndGroup();
                    }
                }
            }

            l_Param = cgGetNextParameter(l_Param);
        }

        m_LayoutInitialized = true;
    }
}
