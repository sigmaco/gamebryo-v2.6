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
#include "NiMainPCH.h"

#include "NiCloningProcess.h"
#include "NiMaterialLibrary.h"
#include "NiRenderObject.h"
#include "NiRenderer.h"
#include "NiShaderFactory.h"
#include "NiSingleShaderMaterial.h"
#include "NiTNodeTraversal.h"
#include "NiFixedString.h"
#include <NiAVObject.h>
#include <NiSystem.h>

bool NiRenderObject::ms_bStreamingAppliesDefaultMaterial = true;

NiImplementRTTI(NiRenderObject, NiAVObject);

//---------------------------------------------------------------------------
NiRenderObject::NiRenderObject() : 
    m_kMaterials(0), 
    m_uiActiveMaterial((unsigned int) NO_MATERIAL), 
    m_bMaterialNeedsUpdateDefault(false)
{
}
//---------------------------------------------------------------------------
NiRenderObject::~NiRenderObject()
{
    NiUInt32 uiMaterialCount = m_kMaterials.GetSize();
    for (NiUInt32 i = 0; i < uiMaterialCount; i++)
        m_kMaterials.GetAt(i).Reinitialize();
    m_kMaterials.RemoveAll();

}
//---------------------------------------------------------------------------
bool NiRenderObject::ApplyMaterial(NiMaterial* pkMaterial, 
   NiUInt32 uiMaterialExtraData)
{
    // Ensure material is not already applied to geometry
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
            return false;
    }

    if (!pkMaterial)
        return false;

    NiMaterialInstance kMaterialInstance(pkMaterial, uiMaterialExtraData);
    kMaterialInstance.SetMaterialNeedsUpdate(true);

    m_kMaterials.Add(kMaterialInstance);

    return true;
}
//---------------------------------------------------------------------------
bool NiRenderObject::ApplyMaterial(const NiFixedString& kName, 
    NiUInt32 uiMaterialExtraData)
{
    NiMaterial* pkMaterial = NiMaterial::GetMaterial(kName);
    if (!pkMaterial)
    {
        pkMaterial = NiMaterialLibrary::CreateMaterial(kName);
    }

    return ApplyMaterial(pkMaterial, uiMaterialExtraData);
}
//---------------------------------------------------------------------------
bool NiRenderObject::ApplyAndSetActiveMaterial(NiMaterial* pkMaterial, 
    NiUInt32 uiMaterialExtraData)
{
    // Ensure material is not already applied to geometry
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
        {
            m_uiActiveMaterial = i;
            return true;
        }
    }

    if (!pkMaterial)
    {
        m_uiActiveMaterial = (unsigned int) NO_MATERIAL;
        return true;
    }

    NiMaterialInstance kMaterialInstance(pkMaterial, uiMaterialExtraData);
    kMaterialInstance.SetMaterialNeedsUpdate(true);

    m_uiActiveMaterial = m_kMaterials.GetSize();
    m_kMaterials.Add(kMaterialInstance);

    return true;
}
//---------------------------------------------------------------------------
bool NiRenderObject::SetActiveMaterial(const NiMaterial* pkMaterial)
{
    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
        {
            m_uiActiveMaterial = i;
            return true;
        }
    }

    m_uiActiveMaterial = (unsigned int) NO_MATERIAL;
    return false;
}
//---------------------------------------------------------------------------
bool NiRenderObject::ApplyAndSetActiveMaterial(const NiFixedString& kName, 
                                       NiUInt32 uiMaterialExtraData)
{
    NiMaterial* pkMaterial = NiMaterial::GetMaterial(kName);
    if (!pkMaterial)
    {
        pkMaterial = NiMaterialLibrary::CreateMaterial(kName);
    }

    return ApplyAndSetActiveMaterial(pkMaterial, uiMaterialExtraData);
}
//---------------------------------------------------------------------------
bool NiRenderObject::RemoveMaterial(NiMaterial* pkMaterial)
{
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    NiUInt32 uiIndex = UINT_MAX;
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
        {
            uiIndex = i;
            break;
        }
    }
    if (uiIndex == UINT_MAX)
        return false;

#if defined(NIDEBUG)
    const NiMaterial* pkOldActiveMaterial = GetActiveMaterial();
#endif

    if (uiIndex == m_uiActiveMaterial)
    {
        // Removing active material
        // Set no new active material
        m_uiActiveMaterial = (unsigned int) NO_MATERIAL;
#if defined(NIDEBUG)
        pkOldActiveMaterial = NULL;
#endif
    }
    else if (m_uiActiveMaterial == m_kMaterials.GetSize())
    {
        // Active material will be moved; update the active material to 
        // reflect this
        m_uiActiveMaterial = uiIndex;
    }

    m_kMaterials.GetAt(uiIndex).Reinitialize();
    m_kMaterials.RemoveAt(uiIndex);

    NIASSERT(pkOldActiveMaterial == GetActiveMaterial());

    return true;
}
//---------------------------------------------------------------------------
bool NiRenderObject::IsMaterialApplied(const NiMaterial* pkMaterial) const
{
    if (pkMaterial == NULL)
        return false;

    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiRenderObject::SetMaterialNeedsUpdate(bool bNeedsUpdate)
{
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        kMatInst.SetMaterialNeedsUpdate(bNeedsUpdate);
    }
}
//---------------------------------------------------------------------------
class RecursiveSetMaterialNeedsUpdateFunctor
{
public:
    RecursiveSetMaterialNeedsUpdateFunctor(bool bDirty)
    {
        m_bDirty = bDirty;
    }

    inline void operator() (NiAVObject* pkObject)
    {
        if (NiIsKindOf(NiRenderObject, pkObject))
        {
            ((NiRenderObject*) pkObject)->SetMaterialNeedsUpdate(m_bDirty);
        }
    }

protected:
    bool m_bDirty;
};
//-------------------------------------------------------------------------
class RecursiveSetMaterialNeedsUpdateDefaultFunctor
{
public:
    RecursiveSetMaterialNeedsUpdateDefaultFunctor(bool bDirty)
    {
        m_bDirty = bDirty;
    }

    inline void operator() (NiAVObject* pkObject)
    {
        if (NiIsKindOf(NiRenderObject, pkObject))
        {
            ((NiRenderObject*) pkObject)->
                SetMaterialNeedsUpdateDefault(m_bDirty);
        }
    }

protected:
    bool m_bDirty;
};
//-------------------------------------------------------------------------
void NiRenderObject::RecursiveSetMaterialNeedsUpdate(NiAVObject* pkObject,
    bool bDirty)
{
    RecursiveSetMaterialNeedsUpdateFunctor kFunctor(bDirty);
    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
}
//-------------------------------------------------------------------------
void NiRenderObject::RecursiveSetMaterialNeedsUpdateDefault(
    NiAVObject* pkObject,
    bool bDirty)
{
    RecursiveSetMaterialNeedsUpdateDefaultFunctor kFunctor(bDirty);
    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
}
//-------------------------------------------------------------------------
bool NiRenderObject::ComputeCachedShader(NiUInt32 uiMaterial)
{
    NiMaterialInstance& kInst = m_kMaterials.GetAt(uiMaterial);
    return kInst.GetCurrentShader(this, m_spPropertyState, 
        m_spEffectState) != NULL;   
}
//---------------------------------------------------------------------------
void NiRenderObject::ClearCachedShaderAndSetNeedsUpdate(NiUInt32 uiMaterial)
{
    NiMaterialInstance& kInst = m_kMaterials.GetAt(uiMaterial);
    kInst.ClearCachedShader();
    kInst.SetMaterialNeedsUpdate(true);
}
//---------------------------------------------------------------------------
NiShader* NiRenderObject::GetShader() const
{
    if (m_uiActiveMaterial >= m_kMaterials.GetSize())
        return NULL;

    const NiMaterialInstance& kInst = m_kMaterials.GetAt(m_uiActiveMaterial);

    return kInst.GetCachedShader(reinterpret_cast<const NiRenderObject*>(this),
        m_spPropertyState, m_spEffectState);
}
//---------------------------------------------------------------------------
NiShader* NiRenderObject::GetShaderFromMaterial()
{
    if (m_uiActiveMaterial >= m_kMaterials.GetSize())
        return NULL;

    NiMaterialInstance& kInst = m_kMaterials.GetAt(m_uiActiveMaterial);

    return kInst.GetCurrentShader(this, m_spPropertyState, m_spEffectState);
}
//---------------------------------------------------------------------------
void NiRenderObject::SetShader(NiShader* pkShader)
{
    NiSingleShaderMaterial* pkMaterial = 
        NiSingleShaderMaterial::Create(pkShader);

    // Apply pkMaterial even if it's NULL
    ApplyAndSetActiveMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiRenderObject::PurgeRendererData(NiRenderer* pkRenderer)
{
    NiAVObject::PurgeRendererData(pkRenderer);

    unsigned int uiCount = GetMaterialCount();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClearCachedShaderAndSetNeedsUpdate(ui);
    }
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiRenderObject::CopyMembers(
    NiRenderObject* pkDest, 
    NiCloningProcess& kCloning)
{
    NiAVObject::CopyMembers(pkDest, kCloning);

    pkDest->m_spPropertyState = m_spPropertyState;
    pkDest->m_spEffectState = m_spEffectState;

    pkDest->SetMaterialNeedsUpdateDefault(m_bMaterialNeedsUpdateDefault);
}
//---------------------------------------------------------------------------
void NiRenderObject::ProcessClone(NiCloningProcess& kCloning)
{
    NiAVObject::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned && pkClone->IsKindOf(GetRTTI()));
    if (bCloned && pkClone->IsKindOf(GetRTTI()))
    {
        NiRenderObject* pkRenderObject = (NiRenderObject*) pkClone;

        for (NiUInt32 i = 0; i < GetMaterialCount(); i++)
        {
            const NiMaterialInstance* pkInst = GetMaterialInstance(i);
            pkRenderObject->ApplyMaterial(pkInst->GetMaterial(), 
                pkInst->GetMaterialExtraData());
        }
        pkRenderObject->SetActiveMaterial(GetActiveMaterial());
    }
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiRenderObject::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    // Load materials
    NiUInt32 uiNumMaterials = 0;
    NiStreamLoadBinary(kStream, uiNumMaterials);
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        NiFixedString kName;
        kStream.LoadFixedString(kName);

        NiUInt32 uiMaterialExtraData;
        NiStreamLoadBinary(kStream, uiMaterialExtraData);

        bool bSuccess = ApplyMaterial(kName, uiMaterialExtraData);
        if (!bSuccess)
        {
            NILOG("Material %s not found during "
                "file loading.\n", (const char*)kName);
        }
    }

    // Load active material index
    NiStreamLoadBinary(kStream, m_uiActiveMaterial);
    if (uiNumMaterials == 0)
        m_uiActiveMaterial = (unsigned int) NO_MATERIAL;

    // Load default material dirty flag
    NiBool bMaterialNeedsUpdateDefault = false;
    NiStreamLoadBinary(kStream, bMaterialNeedsUpdateDefault);
    m_bMaterialNeedsUpdateDefault =
        NIBOOL_IS_TRUE(bMaterialNeedsUpdateDefault);

    // If there are no materials applied, then we attach the 
    // renderer default material.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (ms_bStreamingAppliesDefaultMaterial && 
        GetMaterialCount() == 0 && pkRenderer)
    {
        ApplyAndSetActiveMaterial(pkRenderer->GetDefaultMaterial());
    }
}
//---------------------------------------------------------------------------
void NiRenderObject::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiRenderObject::RegisterStreamables(NiStream& kStream)
{
    if (!NiAVObject::RegisterStreamables(kStream))
        return false;

    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        NiMaterial* pkMaterial = m_kMaterials.GetAt(i).GetMaterial();
        if (pkMaterial)
        {
            kStream.RegisterFixedString(pkMaterial->GetName());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiRenderObject::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    // Save off materials
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();
    NiStreamSaveBinary(kStream, uiNumMaterials);
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterial* pkMaterial = m_kMaterials.GetAt(i).GetMaterial();
        kStream.SaveFixedString(pkMaterial->GetName());

        NiUInt32 uiExtraData = 
            m_kMaterials.GetAt(i).GetMaterialExtraData();
        NiStreamSaveBinary(kStream, uiExtraData);
    }
    NiStreamSaveBinary(kStream, m_uiActiveMaterial);
    NiBool bMaterialNeedsUpdateDefault = m_bMaterialNeedsUpdateDefault;
    NiStreamSaveBinary(kStream, bMaterialNeedsUpdateDefault);

}
//---------------------------------------------------------------------------
bool NiRenderObject::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;

    // Compare material count
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();

    NiRenderObject* pkRendObj = (NiRenderObject*) pkObject;

    if (uiNumMaterials != pkRendObj->m_kMaterials.GetSize())
        return false;

    // Compare materials
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterial* pkThisMaterial = m_kMaterials.GetAt(i).GetMaterial();
        const NiMaterial* pkThatMaterial = 
            pkRendObj->m_kMaterials.GetAt(i).GetMaterial();

        if (pkThisMaterial->GetName() != pkThatMaterial->GetName())
            return false;

        if (m_kMaterials.GetAt(i).GetMaterialExtraData() != 
            pkRendObj->m_kMaterials.GetAt(i).GetMaterialExtraData())
        {
            return false;
        }
    }

    // Compare material flags
    if (m_uiActiveMaterial != pkRendObj->m_uiActiveMaterial ||
        m_bMaterialNeedsUpdateDefault != 
        pkRendObj->m_bMaterialNeedsUpdateDefault)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiRenderObject::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiAVObject::GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
bool NiRenderObject::RequiresMaterialOption(const NiFixedString&) const
{
    return false;
}
//---------------------------------------------------------------------------
