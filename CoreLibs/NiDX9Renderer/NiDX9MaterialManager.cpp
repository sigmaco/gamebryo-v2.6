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

// Precompiled Header
#include "NiD3DRendererPCH.h"

#include "NiDX9MaterialManager.h"
#include <NiMaterialProperty.h>

//---------------------------------------------------------------------------
NiDX9MaterialManager::MaterialEntry::MaterialEntry(
    const NiMaterialProperty& kMat)
{
    m_uiRevID = 0; 

    bool bUpdated = Update(kMat);
    NI_UNUSED_ARG(bUpdated);
    NIASSERT(bUpdated);
}
//---------------------------------------------------------------------------
bool NiDX9MaterialManager::MaterialEntry::Update(
    const NiMaterialProperty& kMat)
{
    unsigned int uiRevID = kMat.GetRevisionID();

    if (m_uiRevID != uiRevID)
    {
        m_uiRevID = uiRevID;
    
        const NiColor& kEmissive = kMat.GetEmittance();
        const NiColor& kAmbient = kMat.GetAmbientColor();
        const NiColor& kDiffuse = kMat.GetDiffuseColor();
        const NiColor& kSpecular = kMat.GetSpecularColor();
        float fAlpha = kMat.GetAlpha();

        m_kD3DMat.Emissive.r = kEmissive.r; 
        m_kD3DMat.Emissive.g = kEmissive.g; 
        m_kD3DMat.Emissive.b = kEmissive.b; 
        m_kD3DMat.Emissive.a = fAlpha; 

        m_kD3DMat.Ambient.r = kAmbient.r; 
        m_kD3DMat.Ambient.g = kAmbient.g; 
        m_kD3DMat.Ambient.b = kAmbient.b; 
        m_kD3DMat.Ambient.a = fAlpha; 

        m_kD3DMat.Diffuse.r = kDiffuse.r; 
        m_kD3DMat.Diffuse.g = kDiffuse.g; 
        m_kD3DMat.Diffuse.b = kDiffuse.b; 
        m_kD3DMat.Diffuse.a = fAlpha; 

        m_kD3DMat.Specular.r = kSpecular.r; 
        m_kD3DMat.Specular.g = kSpecular.g; 
        m_kD3DMat.Specular.b = kSpecular.b; 
        m_kD3DMat.Specular.a = fAlpha; 

        m_kD3DMat.Power = kMat.GetShineness(); 

        return true;
    }
    else
        return false;
}
//---------------------------------------------------------------------------
NiDX9MaterialManager::NiDX9MaterialManager()
{
    // Initializations will be done when NiDX9RenderState calls Initialize()
}
//---------------------------------------------------------------------------
NiDX9MaterialManager::~NiDX9MaterialManager()
{
    PurgeMaterial(NiMaterialProperty::GetDefault());
}
//---------------------------------------------------------------------------
void NiDX9MaterialManager::Initialize()
{
    m_pkCurrentMaterial = NULL;

    SetCurrentMaterial(*NiMaterialProperty::GetDefault());

    m_bChanged = true;
}
//---------------------------------------------------------------------------
D3DMATERIAL9* NiDX9MaterialManager::GetCurrentMaterial()
{
    return m_pkCurrentMaterial->GetD3DMaterial();
}
//---------------------------------------------------------------------------
void NiDX9MaterialManager::SetCurrentMaterial(NiMaterialProperty& kMat)
{
    MaterialEntry* pkNew = (MaterialEntry*)kMat.GetRendererData();

    if (pkNew == NULL)
    {
        pkNew = NiNew MaterialEntry(kMat);

        kMat.SetRendererData(pkNew);
        m_bChanged = true;
    }

    NIASSERT(pkNew);

    if (pkNew->Update(kMat))
        m_bChanged = true;

    if (pkNew != m_pkCurrentMaterial)
    {
        pkNew->GetD3DMaterial();

        m_bChanged = true;
        m_pkCurrentMaterial = pkNew;
    }
}
//---------------------------------------------------------------------------
void NiDX9MaterialManager::UpdateToDevice(LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (m_bChanged)
    {
        NIASSERT(m_pkCurrentMaterial);
    
        m_bChanged = false;

        pkD3DDevice9->SetMaterial(m_pkCurrentMaterial->GetD3DMaterial());
    }
}
//---------------------------------------------------------------------------
void NiDX9MaterialManager::PurgeMaterial(NiMaterialProperty* pkMaterial)
{
    if (pkMaterial)
    {
        MaterialEntry* pkEntry = 
            (MaterialEntry*)pkMaterial->GetRendererData();

        if (pkEntry)
        {
            pkMaterial->SetRendererData(NULL);

            if (pkEntry == m_pkCurrentMaterial)
                m_pkCurrentMaterial = NULL;

            NiDelete pkEntry;
        }
    }
}
//---------------------------------------------------------------------------

