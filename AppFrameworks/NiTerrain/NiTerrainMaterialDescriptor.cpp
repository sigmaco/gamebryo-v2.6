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

#include "NiTerrainPCH.h"

#include "NiTerrainMaterialDescriptor.h"

//---------------------------------------------------------------------------
NiTerrainMaterialDescriptor::NiTerrainMaterialDescriptor()
{
}
//---------------------------------------------------------------------------
void NiTerrainMaterialDescriptor::SetLayerCaps(NiUInt32 uiLayerIndex, 
    NiUInt32 uiUVIndex, bool bEnableDetailMap, bool bEnableNormalMap, 
    bool bEnableParallaxMap, bool bEnableDistributionMask)
{
    switch (uiLayerIndex)
    {
    case 0:
        SetLAYER0_UVINDEX(uiUVIndex);
        SetLAYER0_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER0_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER0_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER0_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 1:
        SetLAYER1_UVINDEX(uiUVIndex);
        SetLAYER1_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER1_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER1_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER1_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 2:
        SetLAYER2_UVINDEX(uiUVIndex);
        SetLAYER2_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER2_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER2_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER2_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 3:
        SetLAYER3_UVINDEX(uiUVIndex);
        SetLAYER3_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER3_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER3_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER3_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 4:
        SetLAYER4_UVINDEX(uiUVIndex);
        SetLAYER4_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER4_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER4_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER4_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 5:
        SetLAYER5_UVINDEX(uiUVIndex);
        SetLAYER5_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER5_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER5_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER5_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 6:
        SetLAYER6_UVINDEX(uiUVIndex);
        SetLAYER6_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER6_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER6_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER6_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    case 7:
        SetLAYER7_UVINDEX(uiUVIndex);
        SetLAYER7_NORMAL_ENABLED(bEnableNormalMap);
        SetLAYER7_PARALLAX_ENABLED(bEnableParallaxMap);
        SetLAYER7_DETAIL_ENABLED(bEnableDetailMap);
        SetLAYER7_DISTMASK_ENABLED(bEnableDistributionMask);
        break;

    default:
        NIASSERT(!"Unsupported layer index.");
    }
}
//---------------------------------------------------------------------------
bool NiTerrainMaterialDescriptor::RequiresViewVector()
{
    bool bLayer0Req = (GetLAYER0_PARALLAX_ENABLED() == 1);
    bool bLayer1Req = (GetLAYER1_PARALLAX_ENABLED() == 1);
    bool bLayer2Req = (GetLAYER2_PARALLAX_ENABLED() == 1);
    bool bLayer3Req = (GetLAYER3_PARALLAX_ENABLED() == 1);
    bool bLayer4Req = (GetLAYER4_PARALLAX_ENABLED() == 1);
    bool bLayer5Req = (GetLAYER5_PARALLAX_ENABLED() == 1);
    bool bLayer6Req = (GetLAYER6_PARALLAX_ENABLED() == 1);
    bool bLayer7Req = (GetLAYER7_PARALLAX_ENABLED() == 1);

    return bLayer0Req || bLayer1Req || bLayer2Req || bLayer3Req ||
        bLayer4Req || bLayer5Req || bLayer6Req || bLayer7Req;
}
