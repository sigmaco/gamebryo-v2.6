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

#include "NiDX9LightManager.h"
#include "NiDX9RenderState.h"
#include "NiD3DRendererHeaders.h"

#include <NiAmbientLight.h>
#include <NiDirectionalLight.h>
#include <NiDynamicEffectState.h>
#include <NiPointLight.h>
#include <NiSpotLight.h>
#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>

const float NiDX9LightManager::LightEntry::ms_fRangeMax = 
    (float)(sqrt(FLT_MAX));

//---------------------------------------------------------------------------
bool NiDX9LightManager::LightEntry::Update(const NiLight& kLight)
{
    unsigned int uiRevID = kLight.GetRevisionID();

    bool bAttenuation = false;

    if (m_uiRevID != uiRevID)
    {
        m_uiRevID = uiRevID;

        memset(&m_kD3DLight, 0, sizeof(D3DLIGHT9));

        NiDynamicEffect::EffectType eType = kLight.GetEffectType();

        NIASSERT(eType != NiDynamicEffect::AMBIENT_LIGHT);

        switch (eType)
        {
        case NiDynamicEffect::DIR_LIGHT :
        case NiDynamicEffect::SHADOWDIR_LIGHT :
            {
                m_kD3DLight.Type = D3DLIGHT_DIRECTIONAL;            

                const NiPoint3& kDir 
                    = ((NiDirectionalLight*)(&kLight))->GetWorldDirection();

                m_kD3DLight.Direction.x = kDir.x;        
                m_kD3DLight.Direction.y = kDir.y;        
                m_kD3DLight.Direction.z = kDir.z;        
            }
            break;
        case NiDynamicEffect::POINT_LIGHT :
        case NiDynamicEffect::SHADOWPOINT_LIGHT :
            {
                m_kD3DLight.Type = D3DLIGHT_POINT;            

                const NiPoint3& kPos 
                    = ((NiPointLight*)(&kLight))->GetWorldLocation();

                m_kD3DLight.Position.x = kPos.x;         
                m_kD3DLight.Position.y = kPos.y;         
                m_kD3DLight.Position.z = kPos.z;         

                bAttenuation = true;
            }
            break;
        case NiDynamicEffect::SPOT_LIGHT :
        case NiDynamicEffect::SHADOWSPOT_LIGHT :
            {
                m_kD3DLight.Type = D3DLIGHT_SPOT;            

                NiSpotLight* pkSpot = (NiSpotLight*)&kLight;
                const NiPoint3& kPos = pkSpot->GetWorldLocation();
                const NiPoint3& kDir = pkSpot->GetWorldDirection();

                m_kD3DLight.Position.x = kPos.x;         
                m_kD3DLight.Position.y = kPos.y;         
                m_kD3DLight.Position.z = kPos.z;         

                m_kD3DLight.Direction.x = kDir.x;        
                m_kD3DLight.Direction.y = kDir.y;        
                m_kD3DLight.Direction.z = kDir.z;        

                // We use the half-angle in NI, D3D uses the full angle,
                // so we multiply by 2 in the conversion to radians
                m_kD3DLight.Theta = 0.0f;
                m_kD3DLight.Phi = pkSpot->GetSpotAngle()*NI_PI/90.0f;
                
                m_kD3DLight.Falloff = pkSpot->GetSpotExponent();          

                bAttenuation = true;
            }
            break;
        };

        if (bAttenuation)
        {
            // we assume that bAttenuation is only set if the kLight is a
            // point kLight or a spot kLight
            NiPointLight* pkPoint = (NiPointLight*)&kLight;

            m_kD3DLight.Attenuation0 = pkPoint->GetConstantAttenuation();     
            m_kD3DLight.Attenuation1 = pkPoint->GetLinearAttenuation();     
            m_kD3DLight.Attenuation2 = pkPoint->GetQuadraticAttenuation();     

            m_kD3DLight.Range = ms_fRangeMax;    
        }

        float fDimmer = kLight.GetDimmer();
        const NiColor& kAmbient = kLight.GetAmbientColor() * fDimmer;
        const NiColor& kDiffuse = kLight.GetDiffuseColor() * fDimmer;
        const NiColor& kSpecular = kLight.GetSpecularColor() * fDimmer;

        m_kD3DLight.Ambient.r = kAmbient.r;         
        m_kD3DLight.Ambient.g = kAmbient.g;         
        m_kD3DLight.Ambient.b = kAmbient.b;         
        m_kD3DLight.Ambient.a = 1.0f;

        m_kD3DLight.Diffuse.r = kDiffuse.r;         
        m_kD3DLight.Diffuse.g = kDiffuse.g;         
        m_kD3DLight.Diffuse.b = kDiffuse.b;         
        m_kD3DLight.Diffuse.a = 1.0f;

        m_kD3DLight.Specular.r = kSpecular.r;         
        m_kD3DLight.Specular.g = kSpecular.g;         
        m_kD3DLight.Specular.b = kSpecular.b;         
        m_kD3DLight.Specular.a = 1.0f;

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4312)
#endif

NiDX9LightManager::NiDX9LightManager(NiDX9RenderState* pkRenderState,
    LPDIRECT3DDEVICE9 pkD3DDevice9) :
    m_pkInvalidState((const NiDynamicEffectState*)UINT_MAX)
{
    NIASSERT(pkRenderState);
    NIASSERT(pkD3DDevice9);

    m_pkRenderState = pkRenderState;
    m_pkD3DDevice9 = pkD3DDevice9;
    D3D_POINTER_REFERENCE(m_pkD3DDevice9);

    m_uiNextAvailableIndex = 0;
    memset((void*)m_auiValidIndices, 0, 
        LIGHTINDEXARRAYSIZE * sizeof(unsigned int));

    InitRenderModes();
}

#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif
//---------------------------------------------------------------------------
void NiDX9LightManager::InitRenderModes()
{
    D3DCOLOR kD3DAmbient = 0x00000000;
    m_pkRenderState->SetRenderState(D3DRS_AMBIENT, kD3DAmbient);
    m_pkRenderState->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_eApply = NiTexturingProperty::APPLY_REPLACE;
    m_bAmbientDiffuse = false;

    m_eSourceMode = NiVertexColorProperty::SOURCE_IGNORE;

    m_pkRenderState->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 
        D3DMCS_MATERIAL);
    m_pkRenderState->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 
        D3DMCS_MATERIAL);
    m_pkRenderState->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 
        D3DMCS_MATERIAL);

    m_pkLastState = m_pkInvalidState;
    m_bLastAmbientDiffuse = false;
}
//---------------------------------------------------------------------------
NiDX9LightManager::~NiDX9LightManager()
{
    while (m_kLights.GetCount())
    {
        NiLight* pkLight;
        LightEntry* pkLightEntry;

        NiTMapIterator kIter = m_kLights.GetFirstPos();
        m_kLights.GetNext(kIter, pkLight, pkLightEntry);
        
        m_kLights.RemoveAt(pkLight);
        pkLight->SetRendererData(NULL);
        MarkIndexAsAvailable(pkLightEntry->GetIndex());

        NiDelete pkLightEntry;
    }

    while (m_kDeviceLights.GetSize())
        m_kDeviceLights.RemoveHead();

    if (m_pkD3DDevice9)
    {
        NiDX9Renderer::ReleaseDevice(m_pkD3DDevice9);
        m_pkD3DDevice9 = 0;
    }
}
//---------------------------------------------------------------------------
void NiDX9LightManager::NextFrame()
{
    // Disable all active lights
    while (m_kDeviceLights.GetSize())
    {
        NiLight* pkActive = m_kDeviceLights.RemoveHead();
        LightEntry* pkLightEntry = (LightEntry*)pkActive->GetRendererData();
        m_pkD3DDevice9->LightEnable(pkLightEntry->GetIndex(), FALSE);
        pkLightEntry->SetActive(false);
    }

    m_pkLastState = m_pkInvalidState;
}
//---------------------------------------------------------------------------
void NiDX9LightManager::SetState(const NiDynamicEffectState* pkState, 
    const NiTexturingProperty* pkTex, const NiVertexColorProperty* pkColor)
{
    m_eApply = pkTex->GetApplyMode();

    // If the apply mode is replace, and we have a texture, then we just
    // want to disable vertex colors and lighting and return to ensure that
    // we don't have a chance of computing expensive and unused vertex colors
    const NiTexturingProperty::Map* pkBase = pkTex->GetBaseMap();
    if (pkBase && pkBase->GetTexture() && 
        (m_eApply == NiTexturingProperty::APPLY_REPLACE))
    {
        m_pkRenderState->SetRenderState(D3DRS_LIGHTING, false);

        return;
    }

    NiVertexColorProperty::SourceVertexMode eSource = 
        pkColor->GetSourceMode();
    NiVertexColorProperty::LightingMode eLighting = 
        pkColor->GetLightingMode();

    bool bLighting = false;

    if (eLighting == NiVertexColorProperty::LIGHTING_E)
    {
        switch (eSource)
        {
        case NiVertexColorProperty::SOURCE_IGNORE:
        case NiVertexColorProperty::SOURCE_AMB_DIFF:
            bLighting = true;
            m_bAmbientDiffuse = false;
            if (m_eSourceMode == NiVertexColorProperty::SOURCE_EMISSIVE)
            {
                m_pkRenderState->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_pkRenderState->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_pkRenderState->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_eSourceMode = NiVertexColorProperty::SOURCE_IGNORE;
            }
            break;

        case NiVertexColorProperty::SOURCE_EMISSIVE:
            bLighting = false;
            break;
        }
    }
    else
    {
        if (eSource != m_eSourceMode)
        {
            switch (eSource)
            {
            case NiVertexColorProperty::SOURCE_IGNORE:
                m_pkRenderState->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_pkRenderState->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_pkRenderState->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                break;
            
            case NiVertexColorProperty::SOURCE_AMB_DIFF:
                m_pkRenderState->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_pkRenderState->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 
                    D3DMCS_COLOR1);
                m_pkRenderState->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 
                    D3DMCS_COLOR1);
                break;
            
            case NiVertexColorProperty::SOURCE_EMISSIVE:
                m_pkRenderState->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 
                    D3DMCS_COLOR1);
                m_pkRenderState->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                m_pkRenderState->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 
                    D3DMCS_MATERIAL);
                break;
            }
            m_eSourceMode = eSource;
        }
        bLighting = true;
        m_bAmbientDiffuse = true;
    }

    if (bLighting)
    {
        m_pkRenderState->SetRenderState(D3DRS_LIGHTING, true);

        if (m_pkLastState == m_pkInvalidState ||
            (pkState != m_pkLastState) || 
            (m_bAmbientDiffuse != m_bLastAmbientDiffuse))
        {
            UpdateLights(pkState);
            m_pkLastState = pkState;
            m_bLastAmbientDiffuse = m_bAmbientDiffuse; 
        }
    }
    else
    {
        m_pkRenderState->SetRenderState(D3DRS_LIGHTING, false);
    }
}
//---------------------------------------------------------------------------
void NiDX9LightManager::RemoveLight(NiLight& kLight)
{
    LightEntry* pkLightEntry = (LightEntry*)kLight.GetRendererData();
    if (pkLightEntry)
    {
        LightEntry* pkStoredLightEntry;
        m_kLights.GetAt(&kLight, pkStoredLightEntry);

        NIASSERT(pkStoredLightEntry == pkLightEntry);

        m_pkD3DDevice9->LightEnable(pkLightEntry->GetIndex(), FALSE);
        pkLightEntry->SetActive(false);

        m_kLights.RemoveAt(&kLight);
        m_kDeviceLights.Remove(&kLight);

        kLight.SetRendererData(NULL);

        MarkIndexAsAvailable(pkLightEntry->GetIndex());
        NiDelete pkLightEntry;
    }
    else if (m_kLights.GetAt(&kLight, pkLightEntry))
    {
        m_kLights.RemoveAt(&kLight);
        m_kDeviceLights.Remove(&kLight);

        kLight.SetRendererData(NULL);

        MarkIndexAsAvailable(pkLightEntry->GetIndex());
    }        
}
//---------------------------------------------------------------------------
bool NiDX9LightManager::IsLit() const
{ 
    return (m_pkRenderState->GetRenderState(D3DRS_LIGHTING) != FALSE); 
}
//---------------------------------------------------------------------------
void NiDX9LightManager::UpdateLights(const NiDynamicEffectState* pkState)
{
    // add to this as we progress
    NiColorA kAmbient(0.0f, 0.0f, 0.0f, 1.0f);

    // Mark all active lights for removal
    NiTListIterator kLightIter = m_kDeviceLights.GetHeadPos();
    while (kLightIter)
    {
        NiLight* pkLight = m_kDeviceLights.GetNext(kLightIter);
        LightEntry* pkLightEntry = (LightEntry*)pkLight->GetRendererData();
        pkLightEntry->SetRemoval(true);
    }

    if (m_bAmbientDiffuse)
    {
        NiDynEffectStateIter pkIter = NULL;
    
        if (pkState)
            pkIter = pkState->GetLightHeadPos();

        unsigned int uiCount = 0;
        while (pkIter && uiCount < 8)
        {
            NiLight* pkLight = pkState->GetNextLight(pkIter);

            if (!pkLight->GetSwitch() || pkLight->GetDimmer() < 0.01f)
                continue;

            if (pkLight->GetEffectType() == NiDynamicEffect::AMBIENT_LIGHT)
            {
                const NiColor& kLightAmb = pkLight->GetAmbientColor();
                float fDimmer = pkLight->GetDimmer();
                kAmbient.r += kLightAmb.r*fDimmer;
                kAmbient.g += kLightAmb.g*fDimmer;
                kAmbient.b += kLightAmb.b*fDimmer;
            }
            else
            {
                LightEntry* pkLightEntry = 
                    (LightEntry*)pkLight->GetRendererData();
                bool bChanged = false;

                if (pkLightEntry)
                {
                    bChanged = pkLightEntry->Update(*pkLight);
                }
                else
                {
                    unsigned int uiIndex = GetNextAvailableIndex();
                    if (uiIndex == 0xffffffff)
                        continue;

                    pkLightEntry = NiNew LightEntry(*pkLight, uiIndex);

                    m_kLights.SetAt(pkLight, pkLightEntry);
                    pkLight->SetRendererData(pkLightEntry);

                    bChanged = true;
                }

                if (bChanged)
                {
                    // update kLight info to device
                    m_pkD3DDevice9->SetLight(pkLightEntry->GetIndex(), 
                        pkLightEntry->GetD3DLight());
                }

                // Add light to DeviceLights, but don't turn it on quite yet
                if (pkLightEntry->GetActive() == false)
                {
                    m_kDeviceLights.AddHead(pkLight);
                }

                // Mark this light so it's not removed
                pkLightEntry->SetRemoval(false);
                uiCount++;
            }
        }
    }

    // Remove all lights that are no longer active
    kLightIter = m_kDeviceLights.GetHeadPos();
    while (kLightIter)
    {
        NiTListIterator kPrevIter = kLightIter;
        NiLight* pkLight = m_kDeviceLights.GetNext(kLightIter);
        LightEntry* pkLightEntry = (LightEntry*)pkLight->GetRendererData();
        if (pkLightEntry->GetRemoval())
        {
            m_pkD3DDevice9->LightEnable(pkLightEntry->GetIndex(), FALSE);
            pkLightEntry->SetActive(false);
            m_kDeviceLights.RemovePos(kPrevIter);
        }
    }

    // Enable all lights that are active and not yet enabled
    kLightIter = m_kDeviceLights.GetHeadPos();
    while (kLightIter)
    {
        NiLight* pkLight = m_kDeviceLights.GetNext(kLightIter);
        LightEntry* pkLightEntry = (LightEntry*)pkLight->GetRendererData();
        if (!pkLightEntry->GetRemoval() && !pkLightEntry->GetActive())
        {
            m_pkD3DDevice9->LightEnable(pkLightEntry->GetIndex(), TRUE);
            pkLightEntry->SetActive(true);
        }
    }

    // Ensure ambient lights are capped to 1.0f.
    unsigned int uiA = (unsigned int)(kAmbient.a * 255.0f);
    if (uiA > 255)
        uiA = 255;
    unsigned int uiR = (unsigned int)(kAmbient.r * 255.0f);
    if (uiR > 255)
        uiR = 255;
    unsigned int uiG = (unsigned int)(kAmbient.g * 255.0f);
    if (uiG > 255)
        uiG = 255;
    unsigned int uiB = (unsigned int)(kAmbient.b * 255.0f);
    if (uiB > 255)
        uiB = 255;

    D3DCOLOR kD3DAmbient =
        (((unsigned char)(uiA)) << 24) |
        (((unsigned char)(uiR)) << 16) |
        (((unsigned char)(uiG)) << 8) |
        ((unsigned char)(uiB));

    m_pkRenderState->SetRenderState(D3DRS_AMBIENT, kD3DAmbient);
}
//---------------------------------------------------------------------------
void NiDX9LightManager::ReleaseLights()
{
    NiTMapIterator kPos = m_kLights.GetFirstPos();
    while (kPos)
    {
        NiLight* pkLight;
        LightEntry* pkLightEntry = 0;
        m_kLights.GetNext(kPos, pkLight, pkLightEntry);

        NIASSERT(pkLightEntry == pkLight->GetRendererData());

        if (pkLightEntry == 0)
            continue;

        m_pkD3DDevice9->LightEnable(pkLightEntry->GetIndex(), FALSE);
        pkLightEntry->SetActive(false);

        m_kLights.RemoveAt(pkLight);
        m_kDeviceLights.Remove(pkLight);

        pkLight->SetRendererData(NULL);

        NiDelete pkLightEntry;
    }
}
//---------------------------------------------------------------------------
unsigned int NiDX9LightManager::GetNextAvailableIndex()
{
    const unsigned int uiSize = 8 * sizeof(unsigned int);
    const unsigned int uiNumBits = uiSize * LIGHTINDEXARRAYSIZE;

    bool bFound = false;
    unsigned int uiIndex = m_uiNextAvailableIndex;
    while (!bFound)
    {
        // Check the bit of the next available index
        unsigned int uiArrayIndex = uiIndex / uiSize;
        unsigned int uiBitPosition = uiIndex % uiSize;

        if ((m_auiValidIndices[uiArrayIndex] & (1 << uiBitPosition)) == 0)
        {
            // Set the bit to indicate it is used
            m_auiValidIndices[uiArrayIndex] |= (1 << uiBitPosition);
            bFound = true;
        }
        else
        {
            uiIndex++;
            if (uiIndex == uiNumBits)
                uiIndex = 0;
            
            if (uiIndex == m_uiNextAvailableIndex)
                break;
        }
    }

    if (!bFound)
        return 0xffffffff;
    
    m_uiNextAvailableIndex = (uiIndex + 1) % uiNumBits;

    return uiIndex;
}
//---------------------------------------------------------------------------
