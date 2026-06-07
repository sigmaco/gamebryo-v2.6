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

#if !defined(_WII)
#ifndef NISKINNINGKERNELDATA_H
#define NISKINNINGKERNELDATA_H

enum ESkinningKernelDataFlags
{
    FLAG_WPB_3      = 0x00000001,
    FLAG_WPB_4      = 0x00000002,
    FLAG_BI_08      = 0x00000004,
    FLAG_BI_16      = 0x00000008,
    FLAG_BW_16      = 0x00000010,
    FLAG_BW_32      = 0x00000020,
    FLAG_POS_16_3   = 0x00000040,
    FLAG_POS_16_4   = 0x00000080,
    FLAG_POS_32     = 0x00000100,
    FLAG_NBT_16_3   = 0x00000200,
    FLAG_NBT_16_4   = 0x00000400,
    FLAG_NBT_32     = 0x00000800,
};
// SKDC + Number of weights per bone + Blend indicies format +
//      Blend weights format + Position format + NBT format
enum ESkinningKernelDataCombination
{
    SKDC_INVALID       = 0x00000000,
    SKDC_3_08_32_16_4_16_4 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_16_4,
    SKDC_3_16_32_16_4_16_4 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_16_4,
    SKDC_3_08_32_32_16_4 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_4,
    SKDC_3_16_32_32_16_4 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_4,
    SKDC_3_08_32_16_4_32 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_32,
    SKDC_3_16_32_16_4_32 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_32,
    SKDC_3_08_32_32_32 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_32,
    SKDC_3_16_32_32_32 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_32,
    SKDC_4_08_32_16_4_16_4 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_16_4,
    SKDC_4_08_16_16_4_16_4 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_16_4 | FLAG_NBT_16_4,
    SKDC_4_16_32_16_4_16_4 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_16_4,
    SKDC_4_16_16_16_4_16_4 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_16_4 | FLAG_NBT_16_4,
    SKDC_4_08_32_32_16_4 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_4,
    SKDC_4_08_16_32_16_4 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_32 | FLAG_NBT_16_4,
    SKDC_4_16_32_32_16_4 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_4,
    SKDC_4_16_16_32_16_4 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_32 | FLAG_NBT_16_4,
    SKDC_4_08_32_16_4_32 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_32,
    SKDC_4_08_16_16_4_32 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_16_4 | FLAG_NBT_32,
    SKDC_4_16_32_16_4_32 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_4 | FLAG_NBT_32,
    SKDC_4_16_16_16_4_32 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_16_4 | FLAG_NBT_32,
    SKDC_4_08_32_32_32 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_32,
    SKDC_4_08_16_32_32 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_32 | FLAG_NBT_32,
    SKDC_4_16_32_32_32 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_32,
    SKDC_4_16_16_32_32 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_32 | FLAG_NBT_32,
#if _PS3
    SKDC_3_08_32_16_3_32 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_32,
    SKDC_3_16_32_16_3_32 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_32,
    SKDC_4_08_32_16_3_32 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_32,
    SKDC_4_08_16_16_3_32 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_16_3 | FLAG_NBT_32,
    SKDC_4_16_32_16_3_32 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_32,
    SKDC_4_16_16_16_3_32 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_16_3 | FLAG_NBT_32,
    SKDC_3_08_32_16_3_16_3 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_16_3,
    SKDC_3_16_32_16_3_16_3 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_16_3,
    SKDC_4_08_32_16_3_16_3 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_16_3,
    SKDC_4_08_16_16_3_16_3 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_16_3 | FLAG_NBT_16_3,
    SKDC_4_16_32_16_3_16_3 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_16_3 | FLAG_NBT_16_3,
    SKDC_4_16_16_16_3_16_3 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_16_3 | FLAG_NBT_16_3,
    SKDC_3_08_32_32_16_3 = 
        FLAG_WPB_3 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_3,
    SKDC_3_16_32_32_16_3 = 
        FLAG_WPB_3 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_3,
    SKDC_4_08_32_32_16_3 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_3,
    SKDC_4_08_16_32_16_3 = 
        FLAG_WPB_4 | FLAG_BI_08 | FLAG_BW_16 | FLAG_POS_32 | FLAG_NBT_16_3,
    SKDC_4_16_32_32_16_3 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_32 | FLAG_POS_32 | FLAG_NBT_16_3,
    SKDC_4_16_16_32_16_3 = 
        FLAG_WPB_4 | FLAG_BI_16 | FLAG_BW_16 | FLAG_POS_32 | FLAG_NBT_16_3,
#endif
};
/**
    A structure that is used to store what is the make up of the input streams
    for this skinning modifier. 
    
    This information is needed for the skinning kernel and the deform vertex 
    function in this modifier.
*/
class NiSkinningKernelData
{
public:

    inline NiSkinningKernelData()
    {
        m_eCombination = SKDC_INVALID;
    }
    inline ~NiSkinningKernelData()
    {
    }

    inline ESkinningKernelDataCombination GetCombination() const
    {
        return m_eCombination;
    }

    inline void SetCombination(NiUInt32 uiNumWeightsPerBone, 
        NiUInt32 uiBlendIndicesStride, NiUInt32 uiBlendWeightsStride,
        NiUInt32 uiPositionStride, NiUInt32 uiNBTStride)
    {
        NiUInt32 uiCombination = 0;
        uiCombination |= (uiNumWeightsPerBone == 3) ? FLAG_WPB_3 : FLAG_WPB_4;
        uiCombination |= (uiBlendIndicesStride == (sizeof(NiUInt8) * 4)) ? 
            FLAG_BI_08 : FLAG_BI_16;
        uiCombination |= 
            (uiBlendWeightsStride == (sizeof(float) * uiNumWeightsPerBone)) ? 
            FLAG_BW_32 : FLAG_BW_16;
        uiCombination |= 
            (uiPositionStride == (sizeof(float) * 3)) ? FLAG_POS_32 :
            (uiPositionStride == (sizeof(NiFloat16) * 4)) ? FLAG_POS_16_4 :
            FLAG_POS_16_3;
        uiCombination |= 
            (uiNBTStride == (sizeof(float) * 3)) ? FLAG_NBT_32 :
            (uiNBTStride == (sizeof(NiFloat16) * 4)) ? FLAG_NBT_16_4 :
            FLAG_POS_16_3;

        m_eCombination = (ESkinningKernelDataCombination)uiCombination;
    }

private:
    /// The stride of the data stream.
    ESkinningKernelDataCombination m_eCombination;
};

#endif // #ifndef NISKINNINGKERNELDATA_H
#endif