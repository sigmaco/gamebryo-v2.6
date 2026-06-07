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

#include "NiTexturingProperty.h"
#include "NiSourceTexture.h"

NiImplementRTTI(NiTexturingProperty, NiProperty);

NiTexturingPropertyPtr NiTexturingProperty::ms_spDefault;
NiFixedString NiTexturingProperty::ms_akMapIDtoNames[INDEX_MAX];

//---------------------------------------------------------------------------
NiTexturingProperty::NiTexturingProperty() :
    m_uFlags(0),
    m_kMaps(DECAL_BASE + 1, 1),
    m_pkShaderMaps(NULL)
{
    // must NULL these out to avoid "holes" in the array that might not be
    // NULL (when NiTArray allocated space, it is not zeroed)
    m_kMaps.SetAt(BASE_INDEX, 0);
    m_kMaps.SetAt(DARK_INDEX, 0);
    m_kMaps.SetAt(DETAIL_INDEX, 0);
    m_kMaps.SetAt(GLOSS_INDEX, 0);
    m_kMaps.SetAt(GLOW_INDEX, 0);
    m_kMaps.SetAt(BUMP_INDEX, 0);
    m_kMaps.SetAt(NORMAL_INDEX, 0);
    m_kMaps.SetAt(PARALLAX_INDEX, 0);
    m_kMaps.SetAt(DECAL_BASE, 0);

    InitializeDecalCount();
    SetMultiTexture(NULL);
    SetApplyMode(APPLY_MODULATE);
}
//---------------------------------------------------------------------------
NiTexturingProperty::~NiTexturingProperty()
{
    for (unsigned int i = 0; i < m_kMaps.GetSize(); i++)
        NiDelete m_kMaps.GetAt(i);

    if (m_pkShaderMaps)
    {
        for (unsigned int i = 0; i < m_pkShaderMaps->GetSize(); i++)
            NiDelete m_pkShaderMaps->GetAt(i);

        NiDelete m_pkShaderMaps;
    }
}
//---------------------------------------------------------------------------
NiTexturingProperty::NiTexturingProperty(const char* pcTextureName) :
    m_uFlags(0),
    m_kMaps(DECAL_BASE + 1, 1),
    m_pkShaderMaps(NULL)
{
    NiTexture* pkTexture = (NiTexture*)NiSourceTexture::Create(pcTextureName);
    Map* pkMap = NiNew Map(pkTexture, 0);
    m_kMaps.SetAt(BASE_INDEX, pkMap);
    m_kMaps.SetAt(DARK_INDEX, 0);
    m_kMaps.SetAt(DETAIL_INDEX, 0);
    m_kMaps.SetAt(GLOSS_INDEX, 0);
    m_kMaps.SetAt(GLOW_INDEX, 0);
    m_kMaps.SetAt(BUMP_INDEX, 0);
    m_kMaps.SetAt(NORMAL_INDEX, 0);
    m_kMaps.SetAt(PARALLAX_INDEX, 0);
    m_kMaps.SetAt(DECAL_BASE, 0);

    InitializeDecalCount();
    SetMultiTexture(NULL);
    SetApplyMode(APPLY_MODULATE);
}
//---------------------------------------------------------------------------
NiTexturingProperty::NiTexturingProperty(NiPixelData* pkPixelData) :
    m_uFlags(0),
    m_kMaps(DECAL_BASE + 1, 1),
    m_pkShaderMaps(NULL)
{
    NiTexture* pkTexture = (NiTexture*)NiSourceTexture::Create(pkPixelData);
    Map* pkMap = NiNew Map(pkTexture, 0);
    m_kMaps.SetAt(BASE_INDEX, pkMap);
    m_kMaps.SetAt(DARK_INDEX, 0);
    m_kMaps.SetAt(DETAIL_INDEX, 0);
    m_kMaps.SetAt(GLOSS_INDEX, 0);
    m_kMaps.SetAt(GLOW_INDEX, 0);
    m_kMaps.SetAt(BUMP_INDEX, 0);
    m_kMaps.SetAt(NORMAL_INDEX, 0);
    m_kMaps.SetAt(PARALLAX_INDEX, 0);
    m_kMaps.SetAt(DECAL_BASE, 0);

    InitializeDecalCount();
    SetMultiTexture(NULL);
    SetApplyMode(APPLY_MODULATE);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::SetMultiTexture(Map* pkMap)
{
    bool bMultiTexture = (pkMap != NULL);

    if (!bMultiTexture)
    {
        unsigned int uiSize = m_kMaps.GetSize();
        for (unsigned int i = 1; i < uiSize; i++)
        {
            if (m_kMaps.GetAt(i) != NULL)
            {
                bMultiTexture = true;
                break;
            }
        }
    }

    SetBit(bMultiTexture, MULTITEXTURE_MASK);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::SetMap(unsigned int uiIndex, Map* pkMap)
{
    if (uiIndex >= DECAL_BASE)
    {
        SetDecalMap(uiIndex - DECAL_BASE, pkMap);
    }
    else
    {
        m_kMaps.SetAt(uiIndex, pkMap);
        if (uiIndex != 0)
        {
            SetMultiTexture(pkMap);
        }
    }
}
//---------------------------------------------------------------------------
void NiTexturingProperty::SetDecalMap(unsigned int uiIndex, Map* pkMap)
{
    unsigned int uiDecalIndex = uiIndex + DECAL_BASE;
    if (uiDecalIndex < m_kMaps.GetSize())
    {
        Map* pkOld = m_kMaps.GetAt(uiDecalIndex);
        if(pkMap)
        {
            if(!pkOld)
                IncrementDecalCount();
        }
        else if(pkOld)
        {
            DecrementDecalCount();
        }

        m_kMaps.SetAt(uiDecalIndex, pkMap);
        NiDelete pkOld;
    }
    else
    {
        m_kMaps.SetAtGrow(uiDecalIndex, pkMap);
        if(pkMap)
            IncrementDecalCount();
    }

    SetMultiTexture(pkMap);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::SetShaderMap(unsigned int uiIndex, 
    NiTexturingProperty::ShaderMap* pkMap)
{
    if (!m_pkShaderMaps)
    {
        m_pkShaderMaps = NiNew NiShaderMapArray;
        NIASSERT(m_pkShaderMaps);
    }

    if (uiIndex < m_pkShaderMaps->GetSize())
    {
        ShaderMap* pkOld = m_pkShaderMaps->GetAt(uiIndex);

        m_pkShaderMaps->SetAt(uiIndex, pkMap);

        NiDelete pkOld;
    }
    else if (pkMap)
    {
        m_pkShaderMaps->SetAtGrow(uiIndex, pkMap);
    }
}

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTexturingProperty);
//---------------------------------------------------------------------------
void NiTexturingProperty::CopyMembers(NiTexturingProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;

    unsigned int uiMax = m_kMaps.GetSize();
    pkDest->m_kMaps.SetSize(uiMax);
    for (unsigned int i = 0; i < uiMax; i++)
    {
        if(m_kMaps.GetAt(i))
        {
            if (i == BUMP_INDEX)
            {
                pkDest->m_kMaps.SetAtGrow(i, NiNew BumpMap(
                    *((BumpMap*)(m_kMaps.GetAt(i)))));
            }
            else if (i == PARALLAX_INDEX)
            {
                pkDest->m_kMaps.SetAtGrow(i, NiNew ParallaxMap(
                    *((ParallaxMap*)(m_kMaps.GetAt(i)))));
            }
            else
            {
                pkDest->m_kMaps.SetAtGrow(i, NiNew Map(*m_kMaps.GetAt(i)));
            }
        }
    }

    if (m_pkShaderMaps)
    {
        if (!pkDest->m_pkShaderMaps)
            pkDest->m_pkShaderMaps = NiNew NiShaderMapArray;

        uiMax = m_pkShaderMaps->GetSize();
        NIASSERT(pkDest->m_pkShaderMaps);
        pkDest->m_pkShaderMaps->SetSize(uiMax);

        for (unsigned int i = 0; i < uiMax; i++)
        {
            if (m_pkShaderMaps->GetAt(i))
            {
                pkDest->m_pkShaderMaps->SetAtGrow(i, NiNew ShaderMap(
                        *((ShaderMap*)(m_pkShaderMaps->GetAt(i)))));
            }
        }
    }
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTexturingProperty);
//---------------------------------------------------------------------------
void NiTexturingProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 2))
    {
        // In 20.1.0.2 and later, this value is located in the flags. 
        ApplyMode eApply;
        NiStreamLoadEnum(kStream, eApply);

        // Handle deprecated apply modes
        if (eApply == APPLY_DEPRECATED || eApply == APPLY_DEPRECATED2)
            eApply = APPLY_MODULATE;
        SetApplyMode(eApply);
    }
    else
    {
        NiStreamLoadBinary(kStream, m_uFlags);

        // Variable should not be streamed in; return to initialized value.
        InitializeDecalCount();
    }

    unsigned int uiListSize;
    NiStreamLoadBinary(kStream, uiListSize);

    // We must account for holes in the array - most datasets won't have
    // them, but a few might to allow for insertion of additional decals
    // in a specific order.
    unsigned int uiRealListSize = uiListSize;
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 5) &&
        uiListSize > NORMAL_INDEX)
    {
        uiRealListSize += DECAL_BASE - NORMAL_INDEX;
    }

    m_kMaps.SetSize(uiRealListSize);

    kStream.SetNumberOfLinkIDs(uiListSize);

    for (unsigned int i = 0; i < uiListSize; i++)
    {
        unsigned int uiArrayIndex = i;

        Map* pkTemp = 0;
        NiBool bHasMap;
        NiStreamLoadBinary(kStream, bHasMap);

        if (i == BUMP_INDEX)
        {
            kStream.SetNumberOfLinkIDs((unsigned int)bHasMap);

            if (bHasMap)
            {
                // We need to allocate and read a bump map in this case,
                // not the base-class map.
                pkTemp = NiNew BumpMap();
                NIASSERT(pkTemp);
                pkTemp->LoadBinary(kStream);
            }
        
            m_kMaps.SetAt(uiArrayIndex, pkTemp);

            continue;
        }
        if (i == PARALLAX_INDEX &&
            kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 5))
        {
            kStream.SetNumberOfLinkIDs((unsigned int)bHasMap);

            if (bHasMap)
            {
                // We need to allocate and read a bump map in this case,
                // not the base-class map.
                pkTemp = NiNew ParallaxMap();
                NIASSERT(pkTemp);
                pkTemp->LoadBinary(kStream);
            }
        
            m_kMaps.SetAt(uiArrayIndex, pkTemp);

            continue;
        }
        kStream.SetNumberOfLinkIDs((unsigned int)bHasMap);

        // If the slot is not the bump map, then load a regular map
        if (bHasMap)
        {
            pkTemp = NiNew Map();
            NIASSERT(pkTemp);
            pkTemp->LoadBinary(kStream);
        }
            
        m_kMaps.SetAtGrow(uiArrayIndex, pkTemp);
    }

    NiStreamLoadBinary(kStream, uiListSize);
    kStream.SetNumberOfLinkIDs(uiListSize);

    if (uiListSize)
    {
        m_pkShaderMaps = NiNew NiShaderMapArray(uiListSize, 1);
        NIASSERT(m_pkShaderMaps);

        for (unsigned int i = 0; i < uiListSize; i++)
        {
            NiBool bHasMap;
            NiStreamLoadBinary(kStream, bHasMap);
            kStream.SetNumberOfLinkIDs((unsigned int)bHasMap);

            ShaderMap* pkTemp = 0;
            if (bHasMap)
            {
                pkTemp = NiNew ShaderMap();
                NIASSERT(pkTemp);
                pkTemp->LoadBinary(kStream);
            }
            
            m_pkShaderMaps->SetAt(i, pkTemp);
        }
    }

    SetMultiTexture(NULL);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);

    // link images
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    InitializeDecalCount();
    unsigned int i = 0;
       
    for (i = 0; i < uiSize; i++)
    {
        if (kStream.GetNumberOfLinkIDs())
        {
            NIASSERT(m_kMaps.GetAt(i));
            m_kMaps.GetAt(i)->SetTexture(
                (NiTexture*)kStream.GetObjectFromLinkID());

            // If this map is a decal, then increment the number of decals
            if (i >= DECAL_BASE)
                IncrementDecalCount();
        }
    }

    // Moved DecalMap down two slots, should shift any values down as well.
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 5))
    {
        NiTexturingProperty::Map* pkNormalMap = NULL;
        NiTexturingProperty::Map* pkParallaxMap = NULL; 
        if (m_kMaps.GetSize() > NORMAL_INDEX)
            pkNormalMap = GetNormalMap();
        if (m_kMaps.GetSize() > PARALLAX_INDEX)
            pkParallaxMap = GetParallaxMap();

        if (pkNormalMap != NULL || pkParallaxMap != NULL)
        {
            unsigned int uiShift = 0;
            if (pkNormalMap) 
                ++uiShift;

            if (pkParallaxMap)
                ++uiShift;

            if (GetDecalMapCount() > 0 && uiShift != 0)
            {
                for (unsigned int ui = m_kMaps.GetSize() - 1; ui >= DECAL_BASE;
                    ui--)
                {
                    m_kMaps.SetAtGrow(ui + uiShift, m_kMaps.GetAt(ui));
                }
            }

            if (pkNormalMap)
            {
                m_kMaps.SetAtGrow(DECAL_BASE, pkNormalMap);
                m_kMaps.SetAt(NORMAL_INDEX, 0);
                IncrementDecalCount();
            }
            else
            {
                m_kMaps.SetAt(DECAL_BASE, NULL);
            }

            if (pkParallaxMap)
            {
                m_kMaps.SetAtGrow(DECAL_BASE + 1, pkParallaxMap);
                m_kMaps.SetAt(PARALLAX_INDEX, 0);
                IncrementDecalCount();
            }
            else if (uiShift == 2)
            {
                m_kMaps.SetAt(DECAL_BASE + 1, NULL);
            }
        }
    }


    // link shader images
    uiSize = kStream.GetNumberOfLinkIDs();
    for (i = 0; i < uiSize; i++)
    {
        if (kStream.GetNumberOfLinkIDs())
        {
            NIASSERT(m_pkShaderMaps && m_pkShaderMaps->GetAt(i));
            m_pkShaderMaps->GetAt(i)->SetTexture(
                (NiTexture*)kStream.GetObjectFromLinkID());
        }
    }
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::RegisterStreamables(NiStream& kStream)
{
    if ( !NiProperty::RegisterStreamables(kStream) )
        return false;

    // register images
    for (unsigned int i = 0; i < m_kMaps.GetSize(); i++)
    {
        Map* pkMap = m_kMaps.GetAt(i);
        if ( pkMap && pkMap->GetTexture())
            pkMap->GetTexture()->RegisterStreamables(kStream);
    }

    // register shader textures
    if (m_pkShaderMaps)
    {
        for (unsigned int i = 0; i < m_pkShaderMaps->GetSize(); i++)
        {
            ShaderMap* pkMap = m_pkShaderMaps->GetAt(i);

            if (pkMap && pkMap->GetTexture())
                pkMap->GetTexture()->RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTexturingProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    unsigned int uiListSize = m_kMaps.GetSize();
    NiStreamSaveBinary(kStream, uiListSize);

    // we must account for holes in the array - most datasets won't have
    // them, but a few might to allow for insertion of additional decals
    // in a specific order.
    for (unsigned int i = 0; i < uiListSize; i++)
    {
        Map* pkMap = m_kMaps.GetAt(i);
        NiBool bHasMap = (pkMap != NULL);
        NiStreamSaveBinary(kStream, bHasMap);
        if (bHasMap)
            pkMap->SaveBinary(kStream);
    }

    // save the optional shader maps
    if (m_pkShaderMaps)
    {
        uiListSize = m_pkShaderMaps->GetSize();
        NiStreamSaveBinary(kStream, uiListSize);

        for (unsigned int i = 0; i < uiListSize; i++)
        {
            ShaderMap* pkMap = m_pkShaderMaps->GetAt(i);
            NiBool bHasMap = (pkMap != NULL);
            NiStreamSaveBinary(kStream, bHasMap);

            if (bHasMap)
                pkMap->SaveBinary(kStream);
        }
    }
    else
    {
        uiListSize = 0;
        NiStreamSaveBinary(kStream, uiListSize);
    }
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiTexturingProperty* pkTex = (NiTexturingProperty*) pkObject;

    if (m_uFlags != pkTex->m_uFlags)
        return false;

    unsigned int uiList1Size = m_kMaps.GetSize();
    unsigned int uiList2Size = pkTex->m_kMaps.GetSize();

    if (uiList1Size != uiList2Size)
        return false;

    for (unsigned int i = 0; i < uiList1Size; i++)
    {
        Map* pkMap1 = m_kMaps.GetAt(i);
        Map* pkMap2 = pkTex->m_kMaps.GetAt(i);

        if (pkMap1 && pkMap2)
        {
            if (i == BUMP_INDEX)
            {
                if (!((BumpMap*)pkMap1)->IsEqual((BumpMap*)pkMap2))
                    return false;
            }
            else if (i == PARALLAX_INDEX)
            {
                if (!((ParallaxMap*)pkMap1)->IsEqual((ParallaxMap*)pkMap2))
                    return false;
            }
            else
            {
                if (!pkMap1->IsEqual(pkMap2))
                    return false;
            }
        }
        else if (pkMap1 || pkMap2)
            return false;
    }

    if (m_pkShaderMaps)
    {
        if (!pkTex->m_pkShaderMaps)
            return false;

        uiList1Size = m_pkShaderMaps->GetSize();
        uiList2Size = pkTex->m_pkShaderMaps->GetSize();

        if (uiList1Size != uiList2Size)
            return false;

        for (unsigned int i = 0; i < uiList1Size; i++)
        {
            ShaderMap* pkMap1 = m_pkShaderMaps->GetAt(i);
            ShaderMap* pkMap2 = pkTex->m_pkShaderMaps->GetAt(i);

            if (pkMap1 && pkMap2)
            {
                if (!pkMap1->IsEqual(pkMap2))
                    return false;
            }
            else if (pkMap1 || pkMap2)
                return false;
        }
    }
    else if (pkTex->m_pkShaderMaps)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTexturingProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiTexturingProperty::ms_RTTI.GetName()));

    unsigned int uiListSize = m_kMaps.GetSize();

    // we must account for holes in the array - most datasets won't have
    // them, but a few might to allow for insertion of additional decals
    // in a specific order.
    for(unsigned int i = 0; i < uiListSize; i++)
    {
        Map* pkMap = m_kMaps.GetAt(i);

        if(pkMap)
        {
            switch(i)
            {
            case BASE_INDEX:
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Base Map", i));
                break;
            case DARK_INDEX:
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Dark Map", i));
                break;
            case DETAIL_INDEX:
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Detail Map", i));
                break;
            case GLOSS_INDEX:
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Gloss Map", i));
                break;
            case GLOW_INDEX:
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Glow Map", i));
                break;
            case BUMP_INDEX:
            {
                BumpMap* pkBump = (BumpMap*)pkMap;
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Bump Map", i));

                pkStrings->Add(NiGetViewerString("m_fLumaScale", 
                    pkBump->GetLumaScale()));
                pkStrings->Add(NiGetViewerString("m_fLumaOffset", 
                    pkBump->GetLumaOffset()));
                pkStrings->Add(NiGetViewerString("m_fBumpMat00", 
                    pkBump->GetBumpMat00()));
                pkStrings->Add(NiGetViewerString("m_fBumpMat01", 
                    pkBump->GetBumpMat01()));
                pkStrings->Add(NiGetViewerString("m_fBumpMat10", 
                    pkBump->GetBumpMat10()));
                pkStrings->Add(NiGetViewerString("m_fBumpMat11", 
                    pkBump->GetBumpMat11()));
                break;
            }
            case NORMAL_INDEX:
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Normal Map", i));
                break;
            case PARALLAX_INDEX:
            {
                ParallaxMap* pkParallax = (ParallaxMap*)pkMap;
                pkStrings->
                    Add(NiGetViewerString("MultiTexture Parallax Map", i));

                pkStrings->Add(NiGetViewerString("m_fParallaxOffset", 
                    pkParallax->GetOffset()));
                break;
            }
            default:
            {
                char acBuf[64];
                NiSprintf(acBuf, 64, "MultiTexture Decal Map %d", 
                    i - DECAL_BASE);
                pkStrings->Add(NiGetViewerString(acBuf, i));
                break;
            }
            }

            char acPrefix[64];
            NiSprintf(acPrefix, 64, "m_spTexture ");
            pkStrings->Add(NiGetViewerString(
                acPrefix,(NiTexture*)pkMap->GetTexture()));
            NiSprintf(acPrefix, 64, "Clamp Mode ");
            pkStrings->Add(GetViewerString(acPrefix, pkMap->GetClampMode()));
            NiSprintf(acPrefix, 64, "Filter Mode");
            pkStrings->Add(GetViewerString(acPrefix, pkMap->GetFilterMode()));
            NiSprintf(acPrefix, 64, "Texture Coord Index ");
            pkStrings->Add(NiGetViewerString(acPrefix, 
                pkMap->GetTextureIndex()));
        }
    }

    if (m_pkShaderMaps)
    {
        uiListSize = m_pkShaderMaps->GetSize();

        for (unsigned int i = 0; i < uiListSize; i++)
        {
            ShaderMap* pkMap = m_pkShaderMaps->GetAt(i);

            if (pkMap)
            {
                pkStrings->Add(NiGetViewerString("Shader Map", i));
                pkStrings->Add(NiGetViewerString("m_uiID", pkMap->GetID()));

                char acPrefix[64];
                NiSprintf(acPrefix, 64,"m_spTexture ");
                pkStrings->Add(NiGetViewerString(
                    acPrefix,(NiTexture*)pkMap->GetTexture()));
                NiSprintf(acPrefix, 64, "Clamp Mode ");
                pkStrings->Add(GetViewerString(acPrefix, 
                    pkMap->GetClampMode()));
                NiSprintf(acPrefix, 64, "Filter Mode");
                pkStrings->Add(GetViewerString(acPrefix, 
                    pkMap->GetFilterMode()));
                NiSprintf(acPrefix, 64, "Texture Coord Index ");
                pkStrings->Add(NiGetViewerString(acPrefix, 
                    pkMap->GetTextureIndex()));
            }
        }
    }
}
//---------------------------------------------------------------------------
char* NiTexturingProperty::GetViewerString(const char* pcPrefix,
    ClampMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, stLen);

    switch (eMode)
    {
    case CLAMP_S_CLAMP_T:
        NiSprintf(pcString, stLen, "%s = CLAMP_S_CLAMP_T", pcPrefix);
        break;
    case CLAMP_S_WRAP_T:
        NiSprintf(pcString, stLen, "%s = CLAMP_S_WRAP_T", pcPrefix);
        break;
    case WRAP_S_CLAMP_T:
        NiSprintf(pcString, stLen, "%s = WRAP_S_CLAMP_T", pcPrefix);
        break;
    case WRAP_S_WRAP_T:
        NiSprintf(pcString, stLen, "%s = WRAP_S_WRAP_T", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;    }

    return pcString;
}
//---------------------------------------------------------------------------
char* NiTexturingProperty::GetViewerString(const char* pcPrefix,
    FilterMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 30;
    char* pcString = NiAlloc(char, stLen);

    switch (eMode)
    {
    case FILTER_NEAREST:
        NiSprintf(pcString, stLen, "%s = FILTER_NEAREST", pcPrefix);
        break;
    case FILTER_BILERP:
        NiSprintf(pcString, stLen, "%s = FILTER_BILERP", pcPrefix);
        break;
    case FILTER_TRILERP:
        NiSprintf(pcString, stLen, "%s = FILTER_TRILERP", pcPrefix);
        break;
    case FILTER_NEAREST_MIPNEAREST:
        NiSprintf(pcString, stLen, "%s = FILTER_NEAREST_MIPNEAREST", pcPrefix);
        break;
    case FILTER_NEAREST_MIPLERP:
        NiSprintf(pcString, stLen, "%s = FILTER_NEAREST_MIPLERP", pcPrefix);
        break;
    case FILTER_BILERP_MIPNEAREST:
        NiSprintf(pcString, stLen, "%s = FILTER_BILERP_MIPNEAREST", pcPrefix);
        break;
    case FILTER_ANISOTROPIC:
        NiSprintf(pcString, stLen, "%s = FILTER_ANISOTROPIC", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;

    }

    return pcString;
}
//---------------------------------------------------------------------------
char* NiTexturingProperty::GetViewerString(const char* pcPrefix,
    ApplyMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, stLen);

    switch (eMode)
    {
    case APPLY_REPLACE:
        NiSprintf(pcString, stLen, "%s = APPLY_REPLACE", pcPrefix);
        break;
    case APPLY_DECAL:
        NiSprintf(pcString, stLen, "%s = APPLY_DECAL", pcPrefix);
        break;
    case APPLY_MODULATE:
        NiSprintf(pcString, stLen, "%s = APPLY_MODULATE", pcPrefix);
        break;
    default:
        NiSprintf(pcString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiTexturingProperty::_SDMInit()
{
    ms_spDefault = NiNew NiTexturingProperty;
    ms_akMapIDtoNames[BASE_INDEX] = "Base";
    ms_akMapIDtoNames[DARK_INDEX] = "Dark";
    ms_akMapIDtoNames[DETAIL_INDEX] = "Detail";
    ms_akMapIDtoNames[GLOSS_INDEX] = "Gloss";
    ms_akMapIDtoNames[GLOW_INDEX] = "Glow";
    ms_akMapIDtoNames[BUMP_INDEX] = "Bump";
    ms_akMapIDtoNames[NORMAL_INDEX] = "Normal";
    ms_akMapIDtoNames[PARALLAX_INDEX] = "Parallax";
    ms_akMapIDtoNames[DECAL_BASE] = "Decal";
    ms_akMapIDtoNames[SHADER_BASE] = "Shader";
}
//---------------------------------------------------------------------------
void NiTexturingProperty::_SDMShutdown()
{
    ms_spDefault = 0;
    ms_akMapIDtoNames[BASE_INDEX] = NULL;
    ms_akMapIDtoNames[DARK_INDEX] = NULL;
    ms_akMapIDtoNames[DETAIL_INDEX] = NULL;
    ms_akMapIDtoNames[GLOSS_INDEX] = NULL;
    ms_akMapIDtoNames[GLOW_INDEX] = NULL;
    ms_akMapIDtoNames[BUMP_INDEX] = NULL;
    ms_akMapIDtoNames[NORMAL_INDEX] = NULL;
    ms_akMapIDtoNames[PARALLAX_INDEX] = NULL;
    ms_akMapIDtoNames[DECAL_BASE] = NULL;
    ms_akMapIDtoNames[SHADER_BASE] = NULL;
}
//---------------------------------------------------------------------------
const NiFixedString& NiTexturingProperty::GetMapNameFromID(unsigned int uiID)
{
    NIASSERT(uiID < INDEX_MAX);
    return ms_akMapIDtoNames[uiID];
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::GetMapIDFromName(const NiFixedString& kName, 
    unsigned int& uiID)
{
    for (unsigned int ui = 0; ui < INDEX_MAX; ui++)
    {
        if (ms_akMapIDtoNames[ui].EqualsNoCase(kName))
        {
            uiID = ui;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiTexturingProperty::Map
//---------------------------------------------------------------------------
NiTexturingProperty::Map::Map() :
    m_uFlags(0),
    m_usMaxAnisotropy(1),
    m_pkTextureTransform(NULL)
{
    SetClampMode(WRAP_S_WRAP_T);
    SetFilterMode(FILTER_BILERP);
    SetTextureIndex(0);
}
//---------------------------------------------------------------------------
NiTexturingProperty::Map::Map(NiTexture* pkTexture, 
    unsigned int uiIndex, ClampMode eClampMode, FilterMode eFilterMode,
    NiTextureTransform* pkTextureTransform) : 
    m_uFlags(0),
    m_usMaxAnisotropy(1),
    m_spTexture(pkTexture),
    m_pkTextureTransform(pkTextureTransform)
{
    SetClampMode(eClampMode);
    SetFilterMode(eFilterMode);
    SetTextureIndex(uiIndex);
}
//---------------------------------------------------------------------------
NiTexturingProperty::Map::Map(const Map& kObj) :
    m_uFlags(kObj.m_uFlags),
    m_usMaxAnisotropy(kObj.m_usMaxAnisotropy),
    m_spTexture(kObj.m_spTexture)
{
    if (kObj.m_pkTextureTransform)
    {
        m_pkTextureTransform = NiNew NiTextureTransform(
            kObj.m_pkTextureTransform->GetTranslate(), 
            kObj.m_pkTextureTransform->GetRotate(),
            kObj.m_pkTextureTransform->GetScale(), 
            kObj.m_pkTextureTransform->GetCenter(), 
            kObj.m_pkTextureTransform->GetTransformMethod());
    }
    else
    {
        m_pkTextureTransform = NULL;
    }
}
//---------------------------------------------------------------------------
NiTexturingProperty::Map::~Map()
{
    NiDelete m_pkTextureTransform;
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::Map::operator==(const Map& kObj) const
{
    if (((m_spTexture == 0) && (kObj.m_spTexture != 0)) ||
        ((m_spTexture != 0) && !m_spTexture->IsEqual(kObj.m_spTexture)) ||
        (m_uFlags != kObj.m_uFlags) ||
        (m_usMaxAnisotropy != kObj.m_usMaxAnisotropy))
    {
        return false;
    }

    if (m_pkTextureTransform != NULL && kObj.m_pkTextureTransform != NULL)
    {
        if (*m_pkTextureTransform != *kObj.m_pkTextureTransform)
            return false;
    }
    else if (m_pkTextureTransform == NULL && kObj.m_pkTextureTransform != NULL)
    {
        return false;
    }
    else if (m_pkTextureTransform != NULL && kObj.m_pkTextureTransform == NULL)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTexturingProperty::Map::LoadBinary(NiStream& kStream)
{
    kStream.ReadLinkID();
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 2))
    {
        // In 20.1.0.2 and later, these values are located in the flags. 
        ClampMode eClamp;
        NiStreamLoadEnum(kStream, eClamp);
        SetClampMode(eClamp);

        FilterMode eFilter;
        NiStreamLoadEnum(kStream, eFilter);
        SetFilterMode(eFilter);

        unsigned int uiTexCoord;
        NiStreamLoadBinary(kStream, uiTexCoord);
        SetTextureIndex(uiTexCoord);
    }
    else
    {
        NiStreamLoadBinary(kStream, m_uFlags);
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 5, 0, 4))
    {
        m_usMaxAnisotropy = 1;
    }
    else
    {
        NiStreamLoadBinary(kStream, m_usMaxAnisotropy);
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 4))
    {
        short sLandK;
        NiStreamLoadBinary(kStream, sLandK);
        NiStreamLoadBinary(kStream, sLandK);
    }

    m_pkTextureTransform = NULL;

    NiBool bTextureTransform;
    NiStreamLoadBinary(kStream, bTextureTransform);

    // Load the Texture transform if one is needed
    if (bTextureTransform == 1)
    {
        m_pkTextureTransform = NiNew NiTextureTransform();
        m_pkTextureTransform->LoadBinary(kStream);
    }
}
//---------------------------------------------------------------------------
void NiTexturingProperty::Map::SaveBinary(NiStream& kStream)
{
    kStream.SaveLinkID(m_spTexture);
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_usMaxAnisotropy);

    // Check for the Texture Transform and Save if Necessary
    NiBool bTextureTransform = (m_pkTextureTransform != NULL);
    NiStreamSaveBinary(kStream, bTextureTransform);
    
    if (m_pkTextureTransform != NULL)
    {
        m_pkTextureTransform->SaveBinary(kStream);
    }

}
//---------------------------------------------------------------------------
bool NiTexturingProperty::Map::IsEqual(Map* pkObject)
{
    return (*this == *pkObject);
}

//---------------------------------------------------------------------------
// NiTexturingProperty::BumpMap
//---------------------------------------------------------------------------
bool NiTexturingProperty::BumpMap::operator==(
    const BumpMap& kObj) const
{
    if (!(*((Map*)this) == *((Map*)&kObj)))
        return false;

    if ((m_fLumaScale != kObj.m_fLumaScale) ||
        (m_fLumaOffset != kObj.m_fLumaOffset) ||
        (m_fBumpMat00 != kObj.m_fBumpMat00) ||
        (m_fBumpMat01 != kObj.m_fBumpMat01) ||
        (m_fBumpMat10 != kObj.m_fBumpMat10) ||
        (m_fBumpMat11 != kObj.m_fBumpMat11))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::BumpMap::operator!=(
    const BumpMap& kObj) const
{
    if (!(*((Map*)this) != *((Map*)&kObj)))
        return true;

    if ((m_fLumaScale != kObj.m_fLumaScale) ||
        (m_fLumaOffset != kObj.m_fLumaOffset) ||
        (m_fBumpMat00 != kObj.m_fBumpMat00) ||
        (m_fBumpMat01 != kObj.m_fBumpMat01) ||
        (m_fBumpMat10 != kObj.m_fBumpMat10) ||
        (m_fBumpMat11 != kObj.m_fBumpMat11))
        return true;

    return false;
}
//---------------------------------------------------------------------------
void NiTexturingProperty::BumpMap::LoadBinary(NiStream& kStream)
{
    Map::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fLumaScale);
    NiStreamLoadBinary(kStream, m_fLumaOffset);
    NiStreamLoadBinary(kStream, m_fBumpMat00);
    NiStreamLoadBinary(kStream, m_fBumpMat01);
    NiStreamLoadBinary(kStream, m_fBumpMat10);
    NiStreamLoadBinary(kStream, m_fBumpMat11);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::BumpMap::SaveBinary(NiStream& kStream)
{
    Map::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fLumaScale);
    NiStreamSaveBinary(kStream, m_fLumaOffset);
    NiStreamSaveBinary(kStream, m_fBumpMat00);
    NiStreamSaveBinary(kStream, m_fBumpMat01);
    NiStreamSaveBinary(kStream, m_fBumpMat10);
    NiStreamSaveBinary(kStream, m_fBumpMat11);
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::BumpMap::IsEqual(BumpMap* pkObject)
{
    return (*this == *pkObject);
}

//---------------------------------------------------------------------------
// NiTexturingProperty::ShaderMap
//---------------------------------------------------------------------------
bool NiTexturingProperty::ShaderMap::operator==(
    const ShaderMap& kObj) const
{
    if (!(*((Map*)this) == *((Map*)&kObj)))
        return false;

    if (m_uiID != kObj.m_uiID)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::ShaderMap::operator!=(
    const ShaderMap& kObj) const
{
    if (!(*((Map*)this) != *((Map*)&kObj)))
        return true;

    if (m_uiID != kObj.m_uiID)
        return true;

    return false;
}
//---------------------------------------------------------------------------
void NiTexturingProperty::ShaderMap::LoadBinary(NiStream& kStream)
{
    Map::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiID);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::ShaderMap::SaveBinary(NiStream& kStream)
{
    Map::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiID);
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::ShaderMap::IsEqual(ShaderMap* pkObject)
{
    return (*this == *pkObject);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// NiTexturingProperty::ParallaxMap
//---------------------------------------------------------------------------
bool NiTexturingProperty::ParallaxMap::operator==(
    const ParallaxMap& kObj) const
{
    if (!(*((Map*)this) == *((Map*)&kObj)))
        return false;

    if (m_fOffset != kObj.m_fOffset)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::ParallaxMap::operator!=(
    const ParallaxMap& kObj) const
{
    if (!(*((Map*)this) != *((Map*)&kObj)))
        return true;

    if (m_fOffset != kObj.m_fOffset)
        return true;

    return false;
}
//---------------------------------------------------------------------------
void NiTexturingProperty::ParallaxMap::LoadBinary(NiStream& kStream)
{
    Map::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fOffset);
}
//---------------------------------------------------------------------------
void NiTexturingProperty::ParallaxMap::SaveBinary(NiStream& kStream)
{
    Map::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fOffset);
}
//---------------------------------------------------------------------------
bool NiTexturingProperty::ParallaxMap::IsEqual(ParallaxMap* pkObject)
{
    return (*this == *pkObject);
}
//---------------------------------------------------------------------------
