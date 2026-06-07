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

#include <NiTerrainPCH.h>

#include "NiTerrainMaterialPixelDescriptor.h"
#include "NiTerrainMaterialDescriptor.h"
#include "NiTerrainMaterial.h"

//---------------------------------------------------------------------------
NiUInt32 NiTerrainMaterialPixelDescriptor::GetLayerUVIndex(NiUInt32 uiLayer)
{    
    switch (uiLayer)
    {
    case 0:
        return GetLAYER0_UVINDEX();

    case 1:
        return GetLAYER1_UVINDEX();

    case 2:
        return GetLAYER2_UVINDEX();

    case 3:
        return GetLAYER3_UVINDEX();

    case 4:
        return GetLAYER4_UVINDEX();

    case 5:
        return GetLAYER5_UVINDEX();

    case 6:
        return GetLAYER6_UVINDEX();

    case 7:
        return GetLAYER7_UVINDEX();

    default:
        NIASSERT(!"Unsupported layer index.");
        return 0;
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainMaterialPixelDescriptor::GetBlendMapUVIndex(
    NiUInt32 uiBlendMap)
{
    switch (uiBlendMap)
    {
    case 0:
        return GetBLENDMAP0_UVINDEX();        

    case 1:
        return GetBLENDMAP1_UVINDEX();        

    default:
        NIASSERT(!"Unsupported blend map index.");
        return 0;
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterialPixelDescriptor::
    SetLayerCapabilitiesFromMaterialDescriptor(
        NiTerrainMaterialDescriptor* pkMaterialDesc, NiUInt32 uiLayer)
{
    NiUInt32 uiValueInMaterial;

    switch (uiLayer)
    {
    case 0:
        uiValueInMaterial = pkMaterialDesc->GetLAYER0_NORMAL_ENABLED();
        SetLAYER0_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER0_PARALLAX_ENABLED();
        SetLAYER0_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER0_DISTMASK_ENABLED();
        SetLAYER0_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER0_DETAIL_ENABLED();
        SetLAYER0_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER0_UVINDEX();
        SetLAYER0_UVINDEX(uiValueInMaterial);
        break;

    case 1:
        uiValueInMaterial = pkMaterialDesc->GetLAYER1_NORMAL_ENABLED();
        SetLAYER1_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER1_PARALLAX_ENABLED();
        SetLAYER1_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER1_DISTMASK_ENABLED();
        SetLAYER1_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER1_DETAIL_ENABLED();
        SetLAYER1_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER1_UVINDEX();
        SetLAYER1_UVINDEX(uiValueInMaterial);
        break;

    case 2:
        uiValueInMaterial = pkMaterialDesc->GetLAYER2_NORMAL_ENABLED();
        SetLAYER2_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER2_PARALLAX_ENABLED();
        SetLAYER2_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER2_DISTMASK_ENABLED();
        SetLAYER2_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER2_DETAIL_ENABLED();
        SetLAYER2_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER2_UVINDEX();
        SetLAYER2_UVINDEX(uiValueInMaterial);
        break;

    case 3:
        uiValueInMaterial = pkMaterialDesc->GetLAYER3_NORMAL_ENABLED();
        SetLAYER3_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER3_PARALLAX_ENABLED();
        SetLAYER3_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER3_DISTMASK_ENABLED();
        SetLAYER3_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER3_DETAIL_ENABLED();
        SetLAYER3_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER3_UVINDEX();
        SetLAYER3_UVINDEX(uiValueInMaterial);
        break;

    case 4:
        uiValueInMaterial = pkMaterialDesc->GetLAYER4_NORMAL_ENABLED();
        SetLAYER4_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER4_PARALLAX_ENABLED();
        SetLAYER4_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER4_DISTMASK_ENABLED();
        SetLAYER4_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER4_DETAIL_ENABLED();
        SetLAYER4_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER4_UVINDEX();
        SetLAYER4_UVINDEX(uiValueInMaterial);
        break;

    case 5:
        uiValueInMaterial = pkMaterialDesc->GetLAYER5_NORMAL_ENABLED();
        SetLAYER5_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER5_PARALLAX_ENABLED();
        SetLAYER5_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER5_DISTMASK_ENABLED();
        SetLAYER5_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER5_DETAIL_ENABLED();
        SetLAYER5_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER5_UVINDEX();
        SetLAYER5_UVINDEX(uiValueInMaterial);
        break;

    case 6:
        uiValueInMaterial = pkMaterialDesc->GetLAYER6_NORMAL_ENABLED();
        SetLAYER6_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER6_PARALLAX_ENABLED();
        SetLAYER6_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER6_DISTMASK_ENABLED();
        SetLAYER6_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER6_DETAIL_ENABLED();
        SetLAYER6_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER6_UVINDEX();
        SetLAYER6_UVINDEX(uiValueInMaterial);
        break;

    case 7:
        uiValueInMaterial = pkMaterialDesc->GetLAYER7_NORMAL_ENABLED();
        SetLAYER7_NORMAL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER7_PARALLAX_ENABLED();
        SetLAYER7_PARALLAX_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER7_DISTMASK_ENABLED();
        SetLAYER7_DISTMASK_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER7_DETAIL_ENABLED();
        SetLAYER7_DETAIL_ENABLED(uiValueInMaterial);

        uiValueInMaterial = pkMaterialDesc->GetLAYER7_UVINDEX();
        SetLAYER7_UVINDEX(uiValueInMaterial);
        break;        

    default:
        NIASSERT(!"Layer index exceeded max number of supported layers.");
    }
}
//---------------------------------------------------------------------------
bool NiTerrainMaterialPixelDescriptor::SupportsDetailMap(NiUInt32 uiLayerIndex)
{
    switch (uiLayerIndex)
    {
    case 0:
        return GetLAYER0_DETAIL_ENABLED() == 1;        

    case 1:
        return GetLAYER1_DETAIL_ENABLED() == 1;

    case 2:
        return GetLAYER2_DETAIL_ENABLED() == 1;

    case 3:
        return GetLAYER3_DETAIL_ENABLED() == 1;

    case 4:
        return GetLAYER4_DETAIL_ENABLED() == 1;

    case 5:
        return GetLAYER5_DETAIL_ENABLED() == 1;

    case 6:
        return GetLAYER6_DETAIL_ENABLED() == 1;

    case 7:
        return GetLAYER7_DETAIL_ENABLED() == 1;

    default:
        NIASSERT(!"Unsupported layer index.");
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiTerrainMaterialPixelDescriptor::SupportsNormalMap(NiUInt32 uiLayerIndex)
{
    switch (uiLayerIndex)
    {
    case 0:
        return GetLAYER0_NORMAL_ENABLED() == 1;        

    case 1:
        return GetLAYER1_NORMAL_ENABLED() == 1;

    case 2:
        return GetLAYER2_NORMAL_ENABLED() == 1;

    case 3:
        return GetLAYER3_NORMAL_ENABLED() == 1;

    case 4:
        return GetLAYER4_NORMAL_ENABLED() == 1;

    case 5:
        return GetLAYER5_NORMAL_ENABLED() == 1;

    case 6:
        return GetLAYER6_NORMAL_ENABLED() == 1;

    case 7:
        return GetLAYER7_NORMAL_ENABLED() == 1;

    default:
        NIASSERT(!"Unsupported layer index.");
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterialPixelDescriptor::SetNormalEnabled(
    NiUInt32 uiLayerIndex, bool bEnabled)
{
    switch (uiLayerIndex)
    {
    case 0:
        SetLAYER0_NORMAL_ENABLED(bEnabled);
        break;

    case 1:
        SetLAYER1_NORMAL_ENABLED(bEnabled);
        break;

    case 2:
        SetLAYER2_NORMAL_ENABLED(bEnabled);
        break;

    case 3:
        SetLAYER3_NORMAL_ENABLED(bEnabled);
        break;

    case 4:
        SetLAYER4_NORMAL_ENABLED(bEnabled);
        break;

    case 5:
        SetLAYER5_NORMAL_ENABLED(bEnabled);
        break;

    case 6:
        SetLAYER6_NORMAL_ENABLED(bEnabled);
        break;

    case 7:
        SetLAYER7_NORMAL_ENABLED(bEnabled);
        break;

    default:
        NIASSERT(!"Unsupported layer index.");
        break;
    }
}
//---------------------------------------------------------------------------
bool NiTerrainMaterialPixelDescriptor::SupportsDistributionMask(
    NiUInt32 uiLayerIndex)
{
    switch (uiLayerIndex)
    {
    case 0:
        return GetLAYER0_DISTMASK_ENABLED() == 1;        

    case 1:
        return GetLAYER1_DISTMASK_ENABLED() == 1;

    case 2:
        return GetLAYER2_DISTMASK_ENABLED() == 1;

    case 3:
        return GetLAYER3_DISTMASK_ENABLED() == 1;

    case 4:
        return GetLAYER4_DISTMASK_ENABLED() == 1;

    case 5:
        return GetLAYER5_DISTMASK_ENABLED() == 1;

    case 6:
        return GetLAYER6_DISTMASK_ENABLED() == 1;

    case 7:
        return GetLAYER7_DISTMASK_ENABLED() == 1;

    default:
        NIASSERT(!"Unsupported layer index.");
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiTerrainMaterialPixelDescriptor::SupportsDistributionMask()
{
    bool bLayer0 = GetLAYER0_DISTMASK_ENABLED() == 1;
    bool bLayer1 = GetLAYER1_DISTMASK_ENABLED() == 1;
    bool bLayer2 = GetLAYER2_DISTMASK_ENABLED() == 1;
    bool bLayer3 = GetLAYER3_DISTMASK_ENABLED() == 1;
    bool bLayer4 = GetLAYER4_DISTMASK_ENABLED() == 1;
    bool bLayer5 = GetLAYER5_DISTMASK_ENABLED() == 1;
    bool bLayer6 = GetLAYER6_DISTMASK_ENABLED() == 1;
    bool bLayer7 = GetLAYER7_DISTMASK_ENABLED() == 1;

    return bLayer0 || bLayer1 || bLayer2 || bLayer3 || bLayer4 ||
        bLayer5 || bLayer6 || bLayer7;
}
//---------------------------------------------------------------------------
void NiTerrainMaterialPixelDescriptor::SetDistributionMaskEnabled(
    NiUInt32 uiLayerIndex, bool bEnabled)
{
    switch (uiLayerIndex)
    {
    case 0:
        SetLAYER0_DISTMASK_ENABLED(bEnabled);
        break;

    case 1:
        SetLAYER1_DISTMASK_ENABLED(bEnabled);
        break;

    case 2:
        SetLAYER2_DISTMASK_ENABLED(bEnabled);
        break;

    case 3:
        SetLAYER3_DISTMASK_ENABLED(bEnabled);
        break;

    case 4:
        SetLAYER4_DISTMASK_ENABLED(bEnabled);
        break;

    case 5:
        SetLAYER5_DISTMASK_ENABLED(bEnabled);
        break;

    case 6:
        SetLAYER6_DISTMASK_ENABLED(bEnabled);
        break;

    case 7:
        SetLAYER7_DISTMASK_ENABLED(bEnabled);
        break;

    default:
        NIASSERT(!"Unsupported layer index.");
        break;
    }
}
//---------------------------------------------------------------------------
bool NiTerrainMaterialPixelDescriptor::SupportsParallaxMap(
    NiUInt32 uiLayerIndex)
{
    switch (uiLayerIndex)
    {
    case 0:
        return GetLAYER0_PARALLAX_ENABLED() == 1;        

    case 1:
        return GetLAYER1_PARALLAX_ENABLED() == 1;

    case 2:
        return GetLAYER2_PARALLAX_ENABLED() == 1;

    case 3:
        return GetLAYER3_PARALLAX_ENABLED() == 1;

    case 4:
        return GetLAYER4_PARALLAX_ENABLED() == 1;

    case 5:
        return GetLAYER5_PARALLAX_ENABLED() == 1;

    case 6:
        return GetLAYER6_PARALLAX_ENABLED() == 1;

    case 7:
        return GetLAYER7_PARALLAX_ENABLED() == 1;

    default:
        NIASSERT(!"Unsupported layer index.");
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterialPixelDescriptor::SetParallaxEnabled(
    NiUInt32 uiLayerIndex, bool bEnabled)
{
    switch (uiLayerIndex)
    {
    case 0:
        SetLAYER0_PARALLAX_ENABLED(bEnabled);
        break;

    case 1:
        SetLAYER1_PARALLAX_ENABLED(bEnabled);
        break;

    case 2:
        SetLAYER2_PARALLAX_ENABLED(bEnabled);
        break;

    case 3:
        SetLAYER3_PARALLAX_ENABLED(bEnabled);
        break;

    case 4:
        SetLAYER4_PARALLAX_ENABLED(bEnabled);
        break;

    case 5:
        SetLAYER5_PARALLAX_ENABLED(bEnabled);
        break;

    case 6:
        SetLAYER6_PARALLAX_ENABLED(bEnabled);
        break;

    case 7:
        SetLAYER7_PARALLAX_ENABLED(bEnabled);
        break;

    default:
        NIASSERT(!"Unsupported layer index.");
        break;
    }
}
//---------------------------------------------------------------------------
NiString NiTerrainMaterialPixelDescriptor::ToString()
{
    NiString kResult;

    ToStringWORLDNBT(kResult, false);
    ToStringNUM_TEXCOORDS(kResult, false);
    ToStringNUM_LAYERS(kResult, false);

    ToStringLAYER0_NORMAL_ENABLED(kResult, false);
    ToStringLAYER0_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER0_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER0_DETAIL_ENABLED(kResult, false);
    ToStringLAYER0_UVINDEX(kResult, false);

    ToStringLAYER1_NORMAL_ENABLED(kResult, false);
    ToStringLAYER1_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER1_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER1_DETAIL_ENABLED(kResult, false);
    ToStringLAYER1_UVINDEX(kResult, false);

    ToStringLAYER2_NORMAL_ENABLED(kResult, false);
    ToStringLAYER2_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER2_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER2_DETAIL_ENABLED(kResult, false);
    ToStringLAYER2_UVINDEX(kResult, false);

    ToStringLAYER3_NORMAL_ENABLED(kResult, false);
    ToStringLAYER3_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER3_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER3_DETAIL_ENABLED(kResult, false);
    ToStringLAYER3_UVINDEX(kResult, false);

    ToStringLAYER4_NORMAL_ENABLED(kResult, false);
    ToStringLAYER4_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER4_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER4_DETAIL_ENABLED(kResult, false);
    ToStringLAYER4_UVINDEX(kResult, false);

    ToStringLAYER5_NORMAL_ENABLED(kResult, false);
    ToStringLAYER5_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER5_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER5_DETAIL_ENABLED(kResult, false);
    ToStringLAYER5_UVINDEX(kResult, false);

    ToStringLAYER6_NORMAL_ENABLED(kResult, false);
    ToStringLAYER6_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER6_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER6_DETAIL_ENABLED(kResult, false);
    ToStringLAYER6_UVINDEX(kResult, false);

    ToStringLAYER7_NORMAL_ENABLED(kResult, false);
    ToStringLAYER7_PARALLAX_ENABLED(kResult, false);
    ToStringLAYER7_DISTMASK_ENABLED(kResult, false);
    ToStringLAYER7_DETAIL_ENABLED(kResult, false);
    ToStringLAYER7_UVINDEX(kResult, false);
    
    return kResult;
}
