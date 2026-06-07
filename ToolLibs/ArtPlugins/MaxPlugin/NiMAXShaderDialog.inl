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
//  NiMAXShaderDialog inline functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
inline int NiMAXShaderDialog::FindSubTexFromHWND(HWND hw) 
{
    for (long i=0; i<NMBUTS; i++) 
    {
        if (hw == m_texMBut[i]->GetHwnd()) 
            return texmapFromMBut[i];
    }   
    return -1;
}
//---------------------------------------------------------------------------
inline Class_ID NiMAXShaderDialog::ClassID()
{ 
    return NISHADER_CLASSID; 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::DeleteThis() 
{
    NiDelete this; 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::SetThing(ReferenceTarget *m)
{ 
    m_pMtl = (StdMat2*)m; 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::SetThings( StdMat2* theMtl, Shader* theShader )
{   
    m_pShader = (NiMAXShader *) theShader; 
    if (m_pShader)
        m_pShader->SetParamDlg(this); 
    m_pMtl = theMtl; 
}
//---------------------------------------------------------------------------
inline ReferenceTarget* NiMAXShaderDialog::GetThing()
{ 
    return (ReferenceTarget*) m_pMtl; 
}
//---------------------------------------------------------------------------
inline Shader* NiMAXShaderDialog::GetShader()
{ 
    return m_pShader; 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::SetTime(TimeValue t) 
{ 
    if (!m_pShader->m_validInterval.InInterval(t)) 
    {
        Interval v;
        m_pShader->Update(t,v);
        LoadDialog(TRUE); 
    }
    m_curTime = t; 
}       
//---------------------------------------------------------------------------
// Is there a keyframe at this time?
inline BOOL NiMAXShaderDialog::KeyAtCurTime(int id) 
{ 
    return m_pShader->KeyAtTime(id,m_curTime); 
} 
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::ActivateDlg( BOOL dlgOn )
{ 
    m_isActive = dlgOn; 
}
//---------------------------------------------------------------------------
inline HWND NiMAXShaderDialog::GetHWnd()
{ 
    return m_hRollup; 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::NotifyChanged()
{ 
    m_pShader->NotifyChanged(); 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::ReloadDialog()
{ 
    Interval v; 
    m_pShader->Update(m_pMtlPar->GetTime(), v); 
    LoadDialog(FALSE);
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::UpdateDialog( ParamID )
{ 
    ReloadDialog(); 
}
//---------------------------------------------------------------------------
inline void NiMAXShaderDialog::UpdateMtlDisplay()
{ 
    m_pMtlPar->MtlChanged(); 
}
//---------------------------------------------------------------------------
// Start editing a color swatch
inline void NiMAXShaderDialog::SelectEditColor(int i) 
{ 
    m_colorSwatch[ i ]->EditThis(FALSE); 
}
//---------------------------------------------------------------------------

