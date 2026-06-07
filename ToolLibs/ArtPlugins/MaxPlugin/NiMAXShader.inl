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

//---------------------------------------------------------------------------
//  NiMAXShader inline functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline Class_ID NiMAXShader::ClassID() 
{ 
    return NISHADER_CLASSID; 
}
//---------------------------------------------------------------------------
inline SClass_ID NiMAXShader::SuperClassID() 
{ 
    return SHADER_CLASS_ID; 
}
//---------------------------------------------------------------------------
inline TSTR NiMAXShader::GetName() 
{ 
    return GetString( IDS_CLASSNAME ); 
}
//---------------------------------------------------------------------------
inline void NiMAXShader::GetClassName(TSTR& s) 
{ 
    s = GetName(); 
}  
//---------------------------------------------------------------------------
inline void NiMAXShader::DeleteThis()
{ 
    NiDelete this; 
}       

//---------------------------------------------------------------------------
// Tell MAX what standard parameters that we support
inline ULONG NiMAXShader::SupportStdParams()
{ 
    return SHADER_PARAMS; 
}
//---------------------------------------------------------------------------
inline void NiMAXShader::ConvertParamBlk(ParamBlockDescID *, 
                                              int, 
                                              IParamBlock * )
{
};
//---------------------------------------------------------------------------
// Texture map channel support
inline long NiMAXShader::nTexChannelsSupported()
{ 
    return 20; 
}
//---------------------------------------------------------------------------
inline TSTR NiMAXShader::GetTexChannelName( long nTex )
{ 
    return GetString( texNameIDS[ nTex ] ); 
}
//---------------------------------------------------------------------------
inline TSTR NiMAXShader::GetTexChannelInternalName( long nTex )
{ 
    return GetString( intertexNameIDS[ nTex ] ); 
}
//---------------------------------------------------------------------------
inline long NiMAXShader::ChannelType( long nChan ) 
{ 
    return chanType[nChan]; 
}
//---------------------------------------------------------------------------
inline long NiMAXShader::StdIDToChannel( long stdID )
{ 
    return stdIDToChannel[stdID]; 
}
//---------------------------------------------------------------------------
// Find out if we have a key at time value t
inline BOOL NiMAXShader::KeyAtTime(int,TimeValue) 
{ 

    // return m_pblock->KeyFrameAtTime(id,t); 

    return false;
}
//---------------------------------------------------------------------------
// Get the requirements for this material (supersampling, etc)
inline ULONG NiMAXShader::GetRequirements( int )
{ 
    return MTLREQ_PHONG; 
}
//---------------------------------------------------------------------------
inline void NiMAXShader::SetSrcBlend(int iVal, TimeValue t)
{ 
    m_iSrcBlend = iVal;
    m_pblock->SetValue( sk_srcBlend, t, iVal);
}
//---------------------------------------------------------------------------
inline ShaderParamDlg* NiMAXShader::GetParamDlg(int)
{ 
    return (ShaderParamDlg*) m_pDlg; 
}
//---------------------------------------------------------------------------
inline void NiMAXShader::SetParamDlg( ShaderParamDlg* NiNewDlg, int)
{ 
    m_pDlg = (NiMAXShaderDialog*)NiNewDlg; 
}
//---------------------------------------------------------------------------
inline int NiMAXShader::NumSubs() 
{ 
    return 1; 
}  
//---------------------------------------------------------------------------
inline Animatable* NiMAXShader::SubAnim(int i)
{ 
    return (i==0)? m_pblock : NULL; 
}
//---------------------------------------------------------------------------
inline TSTR NiMAXShader::SubAnimName(int)
{ 
    return TSTR(GetString( IDS_PARAMETERS)); 
};
//---------------------------------------------------------------------------
inline int NiMAXShader::SubNumToRefNum(int subNum) 
{ 
    return subNum;  
}

//---------------------------------------------------------------------------
inline int NiMAXShader::NumRefs() 
{ 
    return 1; 
}
//---------------------------------------------------------------------------
inline RefTargetHandle NiMAXShader::GetReference(int i)
{ 
    if (i==0) 
        return m_pblock; 
    else return NULL;
}
//---------------------------------------------------------------------------
inline void NiMAXShader::SetReference(int i, RefTargetHandle rtarg) 
{ 
    if (i==0) 
        m_pblock = (IParamBlock2*)rtarg; 
}

//---------------------------------------------------------------------------
inline void NiMAXShader::NotifyChanged()
{ 
    NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE); 
}


//---------------------------------------------------------------------------
inline int  NiMAXShader::NumParamBlocks() 
{ 
    return 1; 
}
//---------------------------------------------------------------------------
inline IParamBlock2* NiMAXShader::GetParamBlock(int i) 
{ 
    if(i== 0)
        return m_pblock;
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline IParamBlock2* NiMAXShader::GetParamBlockByID(BlockID id) 
{ 
    if(m_pblock->ID() == id)
        return m_pblock;
    else
        return NULL;
} 
//---------------------------------------------------------------------------
inline int NiMAXShader::GetSrcBlend(int, BOOL)
{ 
    return m_iSrcBlend;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetSrcBlend( TimeValue t)
{
    return m_pblock->GetInt(sk_srcBlend, t);
}       


//---------------------------------------------------------------------------
inline void NiMAXShader::SetDestBlend(int iVal, TimeValue t)
{ 
    m_iDestBlend = iVal;
    m_pblock->SetValue( sk_destBlend, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetDestBlend(int, BOOL)
{ 
    return m_iDestBlend;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetDestBlend( TimeValue t)
{
    return m_pblock->GetInt(sk_destBlend, t);
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetAlphaMode(int iVal, TimeValue t)
{ 
    m_iAlphaMode = iVal;
    m_pblock->SetValue( sk_alphaMode, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetAlphaMode(int, BOOL)
{ 
    return m_iAlphaMode;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetAlphaMode( TimeValue t)
{
    return m_pblock->GetInt(sk_alphaMode, t);
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetAlphaTestMode(int iVal, TimeValue t)
{ 
    m_iAlphaTestMode = iVal;
    m_pblock->SetValue( sk_testmode, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetAlphaTestMode(int, BOOL)
{ 
    return m_iAlphaTestMode;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetAlphaTestMode( TimeValue t)
{
    return m_pblock->GetInt(sk_testmode, t);
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetAlphaTestOn(int iVal, TimeValue t)
{ 
    m_bAlphaTestOn = iVal ? true : false;
    m_pblock->SetValue( sk_alphatest_enable, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetAlphaTestOn(int, BOOL)
{ 
    return m_bAlphaTestOn;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetAlphaTestOn( TimeValue t)
{
    return m_pblock->GetInt(sk_alphatest_enable, t);
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetTestRef(unsigned char uc, TimeValue t)
{ 
    m_ucTestRef = uc; 
    m_pblock->SetValue( sk_test_ref, t, uc); 
}

//---------------------------------------------------------------------------
inline unsigned char NiMAXShader::GetTestRef(int, BOOL)
{ 
    return m_ucTestRef;
}   

//---------------------------------------------------------------------------
inline unsigned char NiMAXShader::GetTestRef( TimeValue t)
{
    return (unsigned char) m_pblock->GetInt(sk_test_ref, t);
}       


//---------------------------------------------------------------------------
inline void NiMAXShader::SetGlossiness(float, TimeValue)
{ 
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetGlossiness(int, BOOL)
{ 
    return 40.0f; 
}   

//---------------------------------------------------------------------------
inline float NiMAXShader::GetGlossiness( TimeValue )
{
    return 40.0f;  
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetDiffuseClr(Color c, TimeValue t)        
{ 
    m_diffuse = c; 
    m_pblock->SetValue( sk_diff, t, c); 
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetDiffuseClr(int, BOOL)
{ 
    return m_diffuse;
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetDiffuseClr(TimeValue t)
{ 
    return m_pblock->GetColor(sk_diff, t); 
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetAmbientClr(Color c, TimeValue t)
{
    m_ambient = c; 
    m_pblock->SetValue( sk_amb, t, c); 
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetAmbientClr(int, BOOL)
{ 
    return m_ambient;
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetAmbientClr(TimeValue t)
{ 
    return m_pblock->GetColor(sk_amb, t);
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSpecularClr(Color c, TimeValue t)
{
    m_specular = c; 
    m_pblock->SetValue( sk_spec, t, c); 
}
        
//---------------------------------------------------------------------------
inline Color NiMAXShader::GetSpecularClr(int, BOOL)
{ 
    return m_specular;
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetSpecularClr(TimeValue t)
{ 
    return m_pblock->GetColor(sk_spec, t);
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetEmittance(Color c, TimeValue t)     
{ 
    m_emittance = c; 
    m_pblock->SetValue( sk_emit, t, c); 
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetEmittance(int, BOOL)
{ 
    return m_emittance;
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetEmittance(TimeValue t)
{ 
    return m_pblock->GetColor(sk_emit, t); 
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetShininess(float fVal, TimeValue t)
{ 
    m_fShininess = fVal;
    m_pblock->SetValue( sk_shininess, t, fVal);
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetShininess(int, BOOL)
{ 
    return m_fShininess;
}   

//---------------------------------------------------------------------------
inline float NiMAXShader::GetShininess( TimeValue t)
{
    return m_pblock->GetFloat(sk_shininess, t);
}       

//---------------------------------------------------------------------------

inline void NiMAXShader::SetBumpMagnitude(float fBumpMagnitude)
{
    m_fBumpMagnitude = fBumpMagnitude;
    m_pblock->SetValue( sk_bump_magnitude, 0, fBumpMagnitude);
}

//---------------------------------------------------------------------------

inline void NiMAXShader::SetLumaOffset(float fLumaOffset)
{
    m_fLumaOffset = fLumaOffset;
    m_pblock->SetValue( sk_luma_offset, 0, fLumaOffset);
}

//---------------------------------------------------------------------------

inline void NiMAXShader::SetLumaScale(float fLumaScale)
{
    m_fLumaScale = fLumaScale;
    m_pblock->SetValue( sk_luma_scale, 0, fLumaScale);
}
//---------------------------------------------------------------------------

inline void NiMAXShader::SetParallaxOffset(float fParallaxOffset)
{
    m_fParallaxOffset = fParallaxOffset;
    m_pblock->SetValue( sk_parallax_offset, 0, fParallaxOffset);
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetBumpMagnitude()
{
   return m_fBumpMagnitude;
    //    return m_pblock->GetFloat(sk_bump_magnitude);
}
//---------------------------------------------------------------------------

inline float NiMAXShader::GetLumaOffset()
{
   return m_fLumaOffset;
    //    return m_pblock->GetFloat(sk_luma_offset);
}
//---------------------------------------------------------------------------

inline float NiMAXShader::GetLumaScale()
{
    return m_fLumaScale;
    //return m_pblock->GetFloat(sk_luma_scale);
}
//---------------------------------------------------------------------------

inline float NiMAXShader::GetParallaxOffset()
{
   return m_fParallaxOffset;
    //    return m_pblock->GetFloat(sk_parallax_offset);
}
//---------------------------------------------------------------------------
inline void NiMAXShader::SetAlpha(float fVal, TimeValue t)
{ 
    m_fAlpha = fVal;
    m_pblock->SetValue( sk_alpha, t, fVal);
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetAlpha(int, BOOL)
{ 
    return m_fAlpha;
}   

//---------------------------------------------------------------------------
inline float NiMAXShader::GetAlpha( TimeValue t)
{
    return m_pblock->GetFloat(sk_alpha, t);
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSpecularLevel(float, TimeValue)
{
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetSpecularLevel(int, BOOL)
{ 
    return 1.0f; 
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetSpecularLevel(TimeValue)
{ 
    return 1.0f; 
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSelfIllum(float, TimeValue) 
{
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetSelfIllum(int, BOOL)
{ 
    return 0.0f; 
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSelfIllumClrOn( BOOL )
{
}

//---------------------------------------------------------------------------
inline BOOL NiMAXShader::IsSelfIllumClrOn()
{ 
    return FALSE; 
}

//---------------------------------------------------------------------------
inline BOOL NiMAXShader::IsSelfIllumClrOn(int, BOOL)
{ 
    return FALSE; 
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSelfIllumClr(Color, TimeValue)
{
}

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetSelfIllumClr(int, BOOL)
{ 
    return Color(0,0,0); 
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetSelfIllum(TimeValue)
{ 
    return 0.0f;
}       

//---------------------------------------------------------------------------
inline Color NiMAXShader::GetSelfIllumClr(TimeValue)
{ 
    return Color(0,0,0);
}       

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSoftenLevel(float, TimeValue)
{
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetSoftenLevel(int, BOOL)
{ 
    return 0.0f; 
}

//---------------------------------------------------------------------------
inline float NiMAXShader::GetSoftenLevel(TimeValue)
{ 
    return  0.0f; 
}
//---------------------------------------------------------------------------

inline void NiMAXShader::SetTextureApplyMode(int iVal, TimeValue t)
{
    m_iApplyMode = iVal;
    m_pblock->SetValue( sk_applyMode, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetTextureApplyMode(int, BOOL)
{ 
    return m_iApplyMode;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetTextureApplyMode(TimeValue t)
{
    return m_pblock->GetInt(sk_applyMode, t);
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetSourceVertexMode(int iVal, TimeValue t)
{
    m_iSrcVertexMode = iVal;
    m_pblock->SetValue( sk_srcVertexMode, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetSourceVertexMode(int, BOOL)
{ 
    return m_iSrcVertexMode;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetSourceVertexMode(TimeValue t)
{
    return m_pblock->GetInt(sk_srcVertexMode, t);
}

//---------------------------------------------------------------------------
inline void NiMAXShader::SetLightingMode(int iVal, TimeValue t)
{
    m_iLightingMode = iVal;
    m_pblock->SetValue( sk_lightingMode, t, iVal);
}

//---------------------------------------------------------------------------
inline int NiMAXShader::GetLightingMode(int, BOOL)
{ 
    return m_iLightingMode;
}   

//---------------------------------------------------------------------------
inline int NiMAXShader::GetLightingMode(TimeValue t)
{
    return m_pblock->GetInt(sk_lightingMode, t);
}



//---------------------------------------------------------------------------
inline void NiMAXShader::SetVertexColorsOn(int iVal, TimeValue t)
{
    m_bVertexColorsOn = iVal ? true : false;
    m_pblock->SetValue( sk_vertexColors_enable, t, iVal);
}
//---------------------------------------------------------------------------
inline int NiMAXShader::GetVertexColorsOn(int, BOOL)
{ 
    return m_bVertexColorsOn;
}   
//---------------------------------------------------------------------------
inline int NiMAXShader::GetVertexColorsOn(TimeValue t)
{
    return m_pblock->GetInt(sk_vertexColors_enable, t);
}       
//---------------------------------------------------------------------------
inline void NiMAXShader::SetSpecularOn(int iVal, TimeValue t)
{
    m_bSpecularOn = iVal ? true : false;
    m_pblock->SetValue( sk_specular_enable, t, iVal);
}
//---------------------------------------------------------------------------
inline int NiMAXShader::GetSpecularOn(int, BOOL)
{ 
    return m_bSpecularOn;
}   
//---------------------------------------------------------------------------
inline int NiMAXShader::GetSpecularOn(TimeValue t)
{
    return m_pblock->GetInt(sk_specular_enable, t);
}       
//---------------------------------------------------------------------------
inline void NiMAXShader::SetNoSorter(int iVal, TimeValue t)
{
    m_bNoSorter = iVal ? true : false;
    m_pblock->SetValue( sk_no_sorter, t, iVal);
}
//---------------------------------------------------------------------------
inline int NiMAXShader::GetNoSorter(int, BOOL)
{ 
    return m_bNoSorter;
}   
//---------------------------------------------------------------------------
inline int NiMAXShader::GetNoSorter(TimeValue t)
{
    return m_pblock->GetInt(sk_no_sorter, t);
}       
//---------------------------------------------------------------------------
inline void NiMAXShader::SetLockDS(BOOL)
{ 
}
//---------------------------------------------------------------------------
inline BOOL NiMAXShader::GetLockDS()
{ 
    return FALSE; 
}
//---------------------------------------------------------------------------
inline void NiMAXShader::SetLockAD(BOOL)
{
}
//---------------------------------------------------------------------------
inline BOOL NiMAXShader::GetLockAD()
{ 
    return FALSE; 
}
//---------------------------------------------------------------------------
inline void NiMAXShader::SetLockADTex(BOOL)
{ 
}
//---------------------------------------------------------------------------
inline BOOL NiMAXShader::GetLockADTex()
{ 
    return FALSE; 
}
//---------------------------------------------------------------------------
inline float NiMAXShader::Bound(float fX, float fMin, float fMax)
{ 
    return fX < fMin? fMin:( fX > fMax? fMax : fX); 
}
//---------------------------------------------------------------------------
inline Color NiMAXShader::Bound( Color& c )
{ 
    return Color( Bound(c.r), Bound(c.g), Bound(c.b) ); 
}
//---------------------------------------------------------------------------
inline NiMAXShader::TextureExportSetting NiMAXShader::
    GetTextureExportSetting(int map)
{
    switch(map)
    {
        case S_BASE:
            return (TextureExportSetting)(m_pblock->GetInt(sk_base_export));
            break;
        case S_DARK:
            return (TextureExportSetting)(m_pblock->GetInt(sk_dark_export));
            break;
        case S_DETAIL:
            return (TextureExportSetting)(m_pblock->GetInt(sk_detail_export));
            break;
        case S_DECAL:
            return (TextureExportSetting)(m_pblock->GetInt(sk_decal1_export));
            break;
        case S_DECAL2:
            return (TextureExportSetting)(m_pblock->GetInt(sk_decal2_export));
            break;
        case S_GLOW:
            return (TextureExportSetting)(m_pblock->GetInt(sk_glow_export));
            break;
        case S_GLOSS:
            return (TextureExportSetting)(m_pblock->GetInt(sk_gloss_export));
            break;
        default:
            return NI_GLOBAL;
            break;
    }
}
