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

#include "MaxImmerse.h"
#include "NiMAXShader.h"
#include "NiMAXShaderDialog.h"
#include "NiMAXShaderClassDesc.h"
#include "MiViewerPlugin.h"
#include "MiUtils.h"

//---------------------------------------------------------------------------
NiMAXShaderClassDesc NIShaderCD;

//---------------------------------------------------------------------------
// The order of the following parameters is:
//
//      the permanent position independent id of the parameter
//      the internal name
//      the param type
//      table size - only if the param type is one of the table types
//      flags - P_ANIMATABLE indicates the parameter can be animated
//      the localized name for the parameters (in the string table)
//
// these mandatory fields are followed by a bunch of optional fields
// (e.g. p_default) which are terminated by and "end"

static ParamBlockDesc2 shader_param_blk ( shader_params, 
                                         _T("shaderParameters"),  
                                         0, &NIShaderCD, 
                                         P_AUTO_CONSTRUCT, 0, 
    // params
    sk_amb, _T("ambient"), TYPE_RGBA, P_ANIMATABLE, IDS_NI_AMBIENT, 
        p_default, Color(1.0f, 0.0f, 0.0f), 
        end,
    sk_diff, _T("diffuse"), TYPE_RGBA, P_ANIMATABLE, IDS_NI_DIFFUSE, 
        p_default, Color(0.0f, 1.0f, 0.0f), 
        end,
    sk_spec, _T("specular"), TYPE_RGBA, P_ANIMATABLE, IDS_NI_SPECULAR, 
        p_default, Color(0.0f, 0.0f, 1.0f), 
        end,
    sk_emit, _T("emittance"), TYPE_RGBA, P_ANIMATABLE, IDS_NI_EMITTANCE, 
        p_default, Color(0.0f, 0.0f, 0.0f), 
        end,
    sk_shininess, _T("shininess"), TYPE_FLOAT, P_ANIMATABLE, 
                IDS_NI_SHININESS, 
        p_default, 10.0f,
        p_range,    0.0f, 2500.0f,
        p_ui,       TYPE_SPINNER, EDITTYPE_FLOAT, 
                        IDC_SHININESS_EDIT, IDC_SHININESS_SPIN, 1.0f,
        end,
    sk_alpha, _T("alpha"), TYPE_PCNT_FRAC, P_ANIMATABLE, IDS_NI_ALPHA, 
        p_default, 1.0f,
        p_range,    0.0f, 1.0f,
        end,
    sk_bump_magnitude, _T("Magnitude"), TYPE_FLOAT, P_ANIMATABLE, 
                IDS_NI_BUMP_MAGNITUDE, 
        p_default, 1.0f,
        p_range,    -2.0f, 2.0f,
        p_ui,       TYPE_SPINNER, EDITTYPE_FLOAT, 
                        IDC_BUMP_MAGNITUDE_EDIT, IDC_BUMP_MAGNITUDE_SPIN,
                        0.01f,
        end,
    sk_luma_scale, _T("Luma Scale"), TYPE_FLOAT, P_ANIMATABLE, 
                IDS_NI_LUMA_SCALE, 
        p_default, 1.0f,
        p_range,    0.0f, 1.0f,
        end,
    sk_luma_offset, _T("Luma Offset"), TYPE_FLOAT, P_ANIMATABLE, 
                IDS_NI_LUMA_OFFSET, 
        p_default, 0.0f,
        p_range,    0.0f, 1.0f,
        end, 
    sk_test_ref, _T("TestRef"), TYPE_INT, P_ANIMATABLE, IDS_NI_TEST_REF, 
        p_default, 0,
        p_range,    0, 255,
        end,
    sk_srcBlend, _T("srcBlend"), TYPE_INT, 0,   IDS_NI_SRC_BLEND,   
        p_default,      6, 
        p_ui,           TYPE_INTLISTBOX, IDC_ALPHA_SRC_MODE, 0, 
        end, 
    sk_destBlend, _T("destBlend"), TYPE_INT, 0, IDS_NI_DEST_BLEND,  
        p_default,      7, 
        p_ui,           TYPE_INTLISTBOX, IDC_ALPHA_DEST_MODE, 0, 
        end,
    sk_testmode, _T("TestMode"), TYPE_INT, 0,   IDS_NI_TEST_MODE,   
        p_default,      4, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEST_MODE2, 0, 
        end, 
    sk_alphatest_enable, _T("AlphaTestEnable"), TYPE_BOOL, P_ANIMATABLE, 
                                            IDS_NI_ALPHATESTENABLE,     
        p_default,      FALSE, 
        p_ui,           TYPE_SINGLECHEKBOX, IDC_ALPHATESTING_ON, 
        end,
    sk_applyMode, _T("ApplyMode"), TYPE_INT, 0, IDS_NI_APPLY_MODE,  
        p_default,      2, 
        p_ui,           TYPE_INTLISTBOX, IDC_APPLY_MODE, 0, 
        end, 
    sk_srcVertexMode, _T("SrcVertexMode"), TYPE_INT, 0, 
                                              IDS_NI_SRC_VERTEX_MODE,   
        p_default,      2, 
        p_ui,           TYPE_INTLISTBOX, IDC_SRC_MODE, 0, 
        end, 
    sk_lightingMode, _T("LightingMode"), TYPE_INT, 0,   IDS_NI_LIGHTING_MODE,
        p_default,      1, 
        p_ui,           TYPE_INTLISTBOX, IDC_LIGHT_MODE, 0, 
        end, 
    sk_vertexColors_enable, _T("VertexColorsEnable"), TYPE_BOOL, 
                                    P_ANIMATABLE, IDS_NI_VERTEXCOLORENABLE,
        p_default,      TRUE, 
        p_ui,           TYPE_SINGLECHEKBOX, IDC_VERTEXCOLORS_ON, 
        end,
    sk_specular_enable, _T("SpecularEnable"), TYPE_BOOL, P_ANIMATABLE, 
                                                    IDS_NI_SPECULARENABLE,  
        p_default,      FALSE, 
        p_ui,           TYPE_SINGLECHEKBOX, IDC_SPECULAR_ON, 
        end,
    sk_no_sorter, _T("NoSorter"), TYPE_BOOL, P_ANIMATABLE, IDS_NI_NOSORTER,
        p_default,      FALSE, 
        p_ui,           TYPE_SINGLECHEKBOX, IDC_NOSORTER, 
        end,

    sk_alphaMode, _T("alphaMode"), TYPE_INT, 0, IDS_NI_ALPHA_MODE,
        p_default,      0, // Automatic
        p_range,        0,  5,
        p_ui,           TYPE_RADIO, 6, IDC_ALPHA_AUTOMATIC, IDC_ALPHA_NONE,
                                       IDC_ALPHA_STANDARD, 
                                       IDC_ALPHA_ADDITIVE, IDC_ALPHA_MULT,
                                       IDC_ALPHA_ADVANCED,
        end,
    sk_base_export, _T("BaseTextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_BASE,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_BASE, 0, 
        end, 
    sk_dark_export, _T("DarkTextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_DARK,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_DARK, 0, 
        end, 
    sk_detail_export, _T("DetailTextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_DETAIL,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_DETAIL, 0, 
        end, 
    sk_decal1_export, _T("Decal1TextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_DECAL1,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_DECAL1, 0, 
        end, 
    sk_decal2_export, _T("Decal2TextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_DECAL2,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_DECAL2, 0, 
        end, 
    sk_gloss_export, _T("GlossTextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_GLOSS,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_GLOSS, 0, 
        end, 
    sk_glow_export, _T("GlowTextureExport"), TYPE_INT, 0,   
        IDS_NI_TEXPORT_GLOW,   
        p_default,      0, 
        p_ui,           TYPE_INTLISTBOX, IDC_TEXTURE_EXPORT_GLOW, 0, 
        end, 
    sk_shader_name, _T("CustomShader"), TYPE_STRING, 0, 
        IDS_NI_SHADER_NAME,
        p_default,      NI_DEFAULT_SHADER_NAME,
        end,
    sk_shader_viewer_technique, _T("ShaderViewerTechnique"), TYPE_INT, 0, 
        IDS_NI_VIEWER_TECHNIQUE_NAME,
        p_default,      NI_DEFAULT_SHADER_TECHNIQUE_INDEX,
        end,
    sk_shader_export_technique, _T("ShaderExportTechnique"), TYPE_INT, 0, 
        IDS_NI_EXPORT_TECHNIQUE_NAME,
        p_default,      NI_DEFAULT_SHADER_TECHNIQUE_INDEX,
        end,
    sk_use_normal_maps, _T("UseNormalMaps"), TYPE_BOOL, P_ANIMATABLE, 
        IDS_NI_USENORMALMAPS,    
        p_default,      FALSE, 
        p_ui,           TYPE_SINGLECHEKBOX, IDC_NORMAL_MAP_CHECK, 
        end,
    sk_normal_map_technique, _T("NormalMapTechnique"), TYPE_INT, 0, 
        IDS_NI_NORMALMAP_TECHNIQUE_NAME,
        p_default,      0,
        end,
    sk_parallax_offset, _T("ParallaxOffset"), TYPE_FLOAT, 0,
        IDS_NI_PARALLAX_OFFSET, 
        p_default,  0.05f,
        p_range,    0.0f, 10.0f,
        p_ui,       TYPE_SPINNER, EDITTYPE_FLOAT, 
                    IDC_PARALLAX_OFFSET_EDIT, IDC_PARALLAX_OFFSET_SPIN, 0.01f,
        end,
    end
);
//---------------------------------------------------------------------------
// Create the dialog panels themselves
ShaderParamDlg* NiMAXShader::CreateParamDialog(HWND hOldRollup, 
    HWND hwMtlEdit, IMtlParams *imp, StdMat2* theMtl, int rollupOpen, int) 
{
    CHECK_MEMORY();
    Interval v;
    Update(imp->GetTime(),v);
    
    CHECK_MEMORY();
    NiMAXShaderDialog *pDlg = NiExternalNew NiMAXShaderDialog(hwMtlEdit, imp);
    CHECK_MEMORY();
    pDlg->SetThings( theMtl, this  );
    CHECK_MEMORY();
    TCHAR* dlgTemplate;

    dlgTemplate = MAKEINTRESOURCE(IDD_NETIMMERSE_SHADER);


    CHECK_MEMORY();
    // If it already exists, replace it
    if ( hOldRollup ) 
    {
        pDlg->m_hRollup = imp->ReplaceRollupPage( 
            hOldRollup,
            hInstance,
            dlgTemplate,
            GamebryoShaderDlgProc, 
            GetString(IDS_NETIMMERSE_DESC),
            (LPARAM)pDlg , 
            rollupOpen);
    } 
    else
    {
        // otherwise, add one
        pDlg->m_hRollup = imp->AddRollupPage( 
            hInstance,
            dlgTemplate,
            GamebryoShaderDlgProc, 
            GetString(IDS_NETIMMERSE_DESC), 
            (LPARAM)pDlg , 
            rollupOpen);
    }

    CHECK_MEMORY();
    return (ShaderParamDlg*)pDlg;   
}

//---------------------------------------------------------------------------
// Update the UI
RefResult NiMAXShader::NotifyRefChanged(Interval, 
                                        RefTargetHandle hTarget, 
                                        PartID&, 
                                        RefMessage message ) 
{
    switch (message) 
    {
        case REFMSG_CHANGE:
            m_validInterval.SetEmpty();
            if (hTarget == m_pblock)
            {
                // update UI if paramblock changed, possibly from scripter
                ParamID changingParam = m_pblock->LastNotifyParamID();
                // reload the dialog if present
                if (m_pDlg)
                    m_pDlg->UpdateDialog(changingParam);

                // notify our dependents that we've changed
                NotifyChanged();
            }
            break;
    }
    CHECK_MEMORY();

    return(REF_SUCCEED);
}

//---------------------------------------------------------------------------
ClassDesc * 
GetGamebryoShaderDesc()
{ 
    return &NIShaderCD; 
}
//---------------------------------------------------------------------------
NiMAXShader::NiMAXShader() 
{ 
    m_pblock = NULL; 
    m_pDlg = NULL; 
    m_curTime = 0;
    m_validInterval.SetEmpty(); 
    m_diffuse = m_decal = m_bump = m_gloss = m_glow = Color(0.0f, 0.0f, 0.0f);
    m_reflection = Color(1.0f, 1.0f, 1.0f);
    m_dark = m_detail = Color(1.0f, 1.0f, 1.0f);
}

//---------------------------------------------------------------------------
// Cloning and coping standard parameters
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiMAXShader::CopyStdParams( Shader* pFrom )
{
    CHECK_MEMORY();
    // We don't want to see this parameter copying in macrorecorder
    macroRecorder->Disable(); 

    SetAmbientClr( pFrom->GetAmbientClr(0,0), m_curTime );
    SetDiffuseClr( pFrom->GetDiffuseClr(0,0), m_curTime );

    macroRecorder->Enable();
    m_validInterval.SetEmpty();  
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
RefTargetHandle NiMAXShader::Clone( RemapDir &remap )
{
    CHECK_MEMORY();
    NiMAXShader* pNew = NiExternalNew NiMAXShader();

    pNew->ReplaceReference(0, remap.CloneRef(m_pblock));
    pNew->m_validInterval.SetEmpty();    
    pNew->m_ambient = m_ambient;
    pNew->m_diffuse = m_diffuse;
    pNew->m_specular = m_specular;
    pNew->m_emittance = m_emittance;
    pNew->m_fShininess = m_fShininess;
    pNew->m_fBumpMagnitude = m_fBumpMagnitude;
    pNew->m_fLumaOffset = m_fLumaOffset;
    pNew->m_fLumaScale = m_fLumaScale;
    pNew->m_fParallaxOffset = m_fParallaxOffset;
    pNew->m_fAlpha = m_fAlpha;
    pNew->m_ucTestRef = m_ucTestRef;
    pNew->m_iSrcBlend = m_iSrcBlend;
    pNew->m_iDestBlend = m_iDestBlend;
    pNew->m_iAlphaTestMode = m_iAlphaTestMode;
    pNew->m_bAlphaTestOn = m_bAlphaTestOn;
    pNew->m_iApplyMode = m_iApplyMode;
    pNew->m_iSrcVertexMode = m_iSrcVertexMode;
    pNew->m_iLightingMode = m_iLightingMode;
    pNew->m_bVertexColorsOn = m_bVertexColorsOn;
    pNew->m_bSpecularOn = m_bSpecularOn;
    pNew->m_bNoSorter = m_bNoSorter;
    pNew->m_iAlphaMode = m_iAlphaMode;

    CHECK_MEMORY();
    return (RefTargetHandle) pNew;
}

//---------------------------------------------------------------------------
// Shader state
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiMAXShader::Update(TimeValue t, Interval &valid) 
{
    CHECK_MEMORY();
    float fTemp;
    Point3 p;
    int iTemp;

    if (!m_validInterval.InInterval(t)) 
    {
        m_validInterval.SetInfinite();

        m_pblock->GetValue( sk_amb, t, p, m_validInterval );
        Color kCol = Color(p.x,p.y,p.z);
        m_ambient = Bound(kCol);

        m_pblock->GetValue( sk_diff, t, p, m_validInterval );
        kCol = Color(p.x,p.y,p.z);
        m_diffuse = Bound(kCol);

        m_pblock->GetValue( sk_spec, t, p, m_validInterval );
        kCol = Color(p.x,p.y,p.z);
        m_specular = Bound(kCol);

        m_pblock->GetValue( sk_emit, t, p, m_validInterval );
        kCol = Color(p.x,p.y,p.z);
        m_emittance = Bound(kCol);
        
        m_pblock->GetValue( sk_shininess, t, fTemp, m_validInterval );
        m_fShininess = Bound(fTemp, 0.0f, 2500.0f);

        m_pblock->GetValue( sk_alpha, t, fTemp, m_validInterval );
        m_fAlpha = Bound(fTemp);

        m_pblock->GetValue( sk_bump_magnitude, t, fTemp, m_validInterval );
        m_fBumpMagnitude = Bound(fTemp, -2.0f, 2.0f);
        m_pblock->GetValue( sk_luma_offset, t, fTemp, m_validInterval) ;
        m_fLumaOffset = Bound(fTemp, 0.0f, 1.0f);
        m_pblock->GetValue( sk_luma_scale, t, fTemp, m_validInterval );
        m_fLumaScale = Bound(fTemp, 0.0f, 1.0f);
        m_pblock->GetValue( sk_parallax_offset, t, fTemp, m_validInterval) ;
        m_fParallaxOffset = Bound(fTemp, 0.0f, 10.0f);
        
        m_pblock->GetValue( sk_test_ref, t, iTemp, m_validInterval);
        m_ucTestRef = (unsigned char)iTemp;

        m_iSrcBlend = m_pblock->GetInt(sk_srcBlend);

        m_iDestBlend = m_pblock->GetInt(sk_destBlend);

        m_iAlphaMode = m_pblock->GetInt(sk_alphaMode);

        m_pblock->GetValue( sk_testmode, t, iTemp, m_validInterval);
        m_iAlphaTestMode = iTemp;

        m_bAlphaTestOn = m_pblock->GetInt(sk_alphatest_enable) ? 
            true : false;

        m_pblock->GetValue( sk_applyMode, t, iTemp, m_validInterval);
        m_iApplyMode = iTemp;

        m_pblock->GetValue( sk_srcVertexMode, t, iTemp, m_validInterval);
        m_iSrcVertexMode = iTemp;

        m_pblock->GetValue( sk_lightingMode, t, iTemp, m_validInterval);
        m_iLightingMode = iTemp;

        m_bVertexColorsOn = 
            m_pblock->GetInt(sk_vertexColors_enable) ? true : false;
        m_bSpecularOn = m_pblock->GetInt(sk_specular_enable) ? true : false;
        m_bNoSorter = m_pblock->GetInt(sk_no_sorter) ? true : false;

        m_curTime = t;
    }

    valid &= m_validInterval;
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXShader::Reset()
{
    CHECK_MEMORY();
    // Create and intialize paramblock2
    NIShaderCD.MakeAutoParamBlocks(this);   
   // NITexExportCD.MakeAutoParamBlocks(this);

    m_validInterval.SetEmpty();
    SetAmbientClr( Color(1.0f,0.0f,0.0f), 0 );
    SetDiffuseClr( Color(0.0f,1.0f,0.0f), 0 );
    NiStdMat * pkMat = GetMtl();
    if (pkMat)
    {
        Color color = pkMat->GetDiffuse(0);
        Color defaultColor = Color(0.588235f, 0.588235f, 0.588235f);

        if (NiOptimize::CloseTo(color.r, defaultColor.r) && 
            NiOptimize::CloseTo(color.g, defaultColor.g) &&
            NiOptimize::CloseTo(color.b, defaultColor.b))
        {
            pkMat->SetDiffuse(Color(1.0f, 1.0f, 1.0f), 0);
        }
        color = pkMat->GetAmbient(0);
        if (NiOptimize::CloseTo(color.r, defaultColor.r) && 
            NiOptimize::CloseTo(color.g, defaultColor.g) &&
            NiOptimize::CloseTo(color.b, defaultColor.b))
        {
            pkMat->SetAmbient(Color(0.0f, 0.0f, 0.0f), 0);
        }
        
    }
    SetSpecularClr( Color(0.0f,0.0f,1.0f), 0 );
    SetEmittance( Color(0.0f,0.0f,0.0f), 0 );
    SetShininess( 10.0f,0);   
    SetAlpha( 1.0f,0);   
    SetTestRef( 0, 0);
    SetSrcBlend( NiAlphaProperty::ALPHA_SRCALPHA, 0);
    SetDestBlend( NiAlphaProperty::ALPHA_INVSRCALPHA, 0); 
    SetAlphaMode(0, 0);         // automatic
    SetAlphaTestMode( 4, 0);    // greater
    SetAlphaTestOn(false, 0);
    SetTextureApplyMode(2, 0);  // modulate
    SetSourceVertexMode(2, 0);  // amb_diff
    SetLightingMode(1, 0);      // e_a_d
    SetVertexColorsOn(true, 0);
    SetSpecularOn(false, 0);
    SetNoSorter(false, 0);

    CHECK_MEMORY();
}


//---------------------------------------------------------------------------
// Shader load/save
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define SHADER_VERS_CHUNK 0x6500

IOResult NiMAXShader::Save(ISave *isave) 
{ 
   CHECK_MEMORY();
   ULONG nb;

    isave->BeginChunk(SHADER_VERS_CHUNK);
    int version = NI_MAX_SHADER_VERSION;
    isave->Write(&version,sizeof(version),&nb);         
    isave->EndChunk();
    CHECK_MEMORY();

    return IO_OK;
}       

//---------------------------------------------------------------------------
// This class exists as a callback that runs through the enumerated 
// dependents, looking for a Material. It stores the Material if it finds it
// and returns. This is done to fit in with Max's scheme of finding 
// dependents
class CheckForMapsDlg : public DependentEnumProc 
{
    public:     
        BOOL bfound;
        ReferenceMaker *m_pkme;
        ReferenceMaker *m_pkmap;
        CheckForMapsDlg(ReferenceMaker *m) 
        {
            bfound = FALSE;
            m_pkme = m;
            m_pkmap = NULL;
        }
        int proc(ReferenceMaker *rmaker) 
        {
            if (rmaker==m_pkme) 
                return 0;
            if (rmaker->SuperClassID()==SClass_ID(MATERIAL_CLASS_ID)) 
            {
                bfound = TRUE;
                m_pkmap = rmaker;
                return 1;
            }
            return 0;
        }
};
//---------------------------------------------------------------------------
// This callback exists solely to initialize values that did not exist in
// the previous version of the NiMAXShader
class NiMAXShaderCB: public PostLoadCallback 
{
    public:
        NiMAXShader *m_pks;
        int iloadVersion;
        NiMAXShaderCB(NiMAXShader *NiNewS, int loadVers) 
        { 
            m_pks = NiNewS; 
            iloadVersion = loadVers; 
        }
        inline void proc(ILoad *) 
        {
            m_pks->AddRedefineShaderCustAttribCallback();
            
            if (iloadVersion < 2)
            {
                // set values that were previously stored outside the PB
                // In order to locate the Material, we must search 
                // through all references to our shader. This is done in 
                // the CheckForMapsDlg class.
                CheckForMapsDlg kcheck(m_pks);

                m_pks->DoEnumDependents(&kcheck);

                // Make sure that we have found the map in the enumerations
                // Make sure that the map is not equal to null
                if(kcheck.bfound && kcheck.m_pkmap != NULL)
                {
                    // Should probably check to make sure this is a StdMat 
                    // before we cast
                    StdMat* pkmaterial = ((StdMat*) (kcheck.m_pkmap));
                    // Make sure we survived the cast
                    if(pkmaterial != NULL)
                    {
                        // Get the bump map amount from the previous version
                        // setup the current bump map magnitude based on the
                        // old amount
                        float fBumpAmt = pkmaterial->GetTexmapAmt(S_BUMP, 0);
                        m_pks->SetBumpMagnitude(fBumpAmt );
                    }
                }
                CHECK_MEMORY();
            }

            if (iloadVersion < 4)
            {
                if (m_pks->GetNormalMapTechnique() == 
                    NiMAXShader::MAX_NBT_COUNT)
                {
                    m_pks->SetNormalMapTechnique(NiMAXShader::MAX_NBT_ATI);
                }
            }

            if (iloadVersion < 5)
            {
                int iOldApplyMode = m_pks->GetTextureApplyMode(0);
                if (iOldApplyMode > 2)
                {
                    // if the apply mode was HILIGHT or HILIGHT2, these
                    // options are deprecated.
                    m_pks->SetTextureApplyMode(2, 0);
                }
            }

            if (iloadVersion < 6)
            {
                CheckForMapsDlg kcheck(m_pks);

                m_pks->DoEnumDependents(&kcheck);

                // Make sure that we have found the map in the enumerations
                // Make sure that the map is not equal to null
                if(kcheck.bfound && kcheck.m_pkmap != NULL)
                {
                    // Should probably check to make sure this is a StdMat 
                    // before we cast
                    StdMat* pkmaterial = ((StdMat*) (kcheck.m_pkmap));
                    // Make sure we survived the cast
                    if(pkmaterial != NULL)
                    {
                        unsigned int uiShift = 2;
                        for (unsigned int ui = SHADER_NTEXMAPS - 1;
                            ui >= S_NORMALMAP; ui--)
                        {
                            Texmap* pkMap = pkmaterial->GetSubTexmap(ui); 
                            BOOL bOnA = pkmaterial->SubTexmapOn(ui);
                            pkmaterial->SetSubTexmap(ui + uiShift, pkMap);
                            pkmaterial->SetSubTexmap(ui, NULL);
                            pkmaterial->EnableMap(ui + uiShift, bOnA);
#ifdef NIDEBUG
                            BOOL bOnB = 
#endif
                                pkmaterial->SubTexmapOn(ui + uiShift);

                            NIASSERT(bOnA == bOnB);
                        }
                    }
                }
            }
        }
};

//---------------------------------------------------------------------------

IOResult NiMAXShader::Load(ILoad *iload) 
{ 
    ULONG nb;
    int id;
    int version = 0;

    IOResult res;
    while (IO_OK==(res=iload->OpenChunk())) 
    {
        switch(id = iload->CurChunkID())  
        {
            case SHADER_VERS_CHUNK:
                res = iload->Read(&version,sizeof(version), &nb);
                break;
        }
        iload->CloseChunk();
        if (res!=IO_OK) 
            return res;
    }

    CHECK_MEMORY();
    iload->RegisterPostLoadCallback(NiExternalNew NiMAXShaderCB(
        this, version));
    if(version < NI_MAX_SHADER_VERSION)
    {
        iload->SetObsolete();
    }
    return IO_OK;
}

//---------------------------------------------------------------------------
NiString NiMAXShader::GetShaderName()
{
    CHECK_MEMORY();
    return NiString(m_pblock->GetStr(sk_shader_name));
}
//---------------------------------------------------------------------------
void NiMAXShader::SetShaderName(NiString strWhichShader)
{
    m_pblock->SetValue(sk_shader_name, 0, (TCHAR*)(const char*)
        strWhichShader);
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
unsigned int NiMAXShader::GetViewerShaderTechnique()
{
    CHECK_MEMORY();
    return (unsigned int) m_pblock->GetInt(sk_shader_viewer_technique);
}
//---------------------------------------------------------------------------
unsigned int NiMAXShader::GetExportShaderTechnique()
{
    CHECK_MEMORY();
    return (unsigned int) m_pblock->GetInt(sk_shader_export_technique);
}
//---------------------------------------------------------------------------
void NiMAXShader::SetViewerShaderTechnique(unsigned int uiWhichTechnique)
{
    CHECK_MEMORY();
    m_pblock->SetValue(sk_shader_viewer_technique, 0, (int) uiWhichTechnique);
}
//---------------------------------------------------------------------------
void NiMAXShader::SetExportShaderTechnique(unsigned int uiWhichTechnique)
{
   CHECK_MEMORY();
   m_pblock->SetValue(sk_shader_export_technique, 0, (int) uiWhichTechnique);
}
//---------------------------------------------------------------------------

BOOL NiMAXShader::GetUseNormalMap()
{
    CHECK_MEMORY();
    return (BOOL) m_pblock->GetInt(sk_use_normal_maps);
}
//---------------------------------------------------------------------------

void NiMAXShader::SetUseNormalMap(BOOL bUse)
{
    m_pblock->SetValue(sk_use_normal_maps, 0, (int) bUse);
}
//---------------------------------------------------------------------------

NiMAXShader::NBT_Method NiMAXShader::GetNormalMapTechnique()
{
    CHECK_MEMORY();
    return (NBT_Method) m_pblock->GetInt(sk_normal_map_technique);
}
//---------------------------------------------------------------------------
void NiMAXShader::SetNormalMapTechnique(NBT_Method eWhichTechnique)
{
    if (eWhichTechnique < MAX_NBT_COUNT)
        m_pblock->SetValue(sk_normal_map_technique, 0, (int) eWhichTechnique);
}
//---------------------------------------------------------------------------
unsigned int NiMAXShader::ConvertNiMAXShaderToTechnique(
    unsigned int uiWhichTechnique)
{
    if (uiWhichTechnique == 0)
        return (unsigned int)NiShader::DEFAULT_IMPLEMENTATION;
    else
        return uiWhichTechnique - 1;
}
//---------------------------------------------------------------------------
