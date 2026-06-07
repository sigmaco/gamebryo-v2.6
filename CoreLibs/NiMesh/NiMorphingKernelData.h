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

#ifndef NIMORPHINGKERNELDATA_H
#define NIMORPHINGKERNELDATA_H

enum EMorphingKernelDataFlags
{
    FLAG_POINTS_16    = 0x00000001,
    FLAG_POINTS_32    = 0x00000002,
    FLAG_TARGETS_16   = 0x00000004,
    FLAG_TARGETS_32   = 0x00000008,
    FLAG_NORMALIZE    = 0x00000010,
};
// MKDC + Morph weights format + Points format
enum EMorphingKernelDataCombination
{
    MKDC_INVALID       = 0x00000000,
    
    MKDC_16_16 = FLAG_POINTS_16 | FLAG_TARGETS_16,
    MKDC_32_16 = FLAG_POINTS_32 | FLAG_TARGETS_16,
    MKDC_16_32 = FLAG_POINTS_16 | FLAG_TARGETS_32,
    MKDC_32_32 = FLAG_POINTS_32 | FLAG_TARGETS_32,
    MKDC_16_16_N = FLAG_POINTS_16 | FLAG_TARGETS_16 | FLAG_NORMALIZE,
    MKDC_32_16_N = FLAG_POINTS_32 | FLAG_TARGETS_16 | FLAG_NORMALIZE,
    MKDC_16_32_N = FLAG_POINTS_16 | FLAG_TARGETS_32 | FLAG_NORMALIZE,
    MKDC_32_32_N = FLAG_POINTS_32 | FLAG_TARGETS_32 | FLAG_NORMALIZE,
};
/**
    A structure that is used to store what is the make up of the input streams
    for this morphing modifier. 
    
    This information is needed for the morphing kernel and the deform vertex 
    function in this modifier.
*/
class NiMorphingKernelData
{
public:

    inline NiMorphingKernelData()
    {
        m_eCombination = MKDC_INVALID;
    }
    inline ~NiMorphingKernelData()
    {
    }

    inline EMorphingKernelDataCombination GetCombination() const
    {
        return m_eCombination;
    }

    inline void SetCombination(NiUInt32 uiSourceComponentSize,
        NiUInt32 uiTargetComponentSize, bool bNormalize)
    {
        NiUInt32 uiCombination = 0;
        uiCombination |= 
            (uiSourceComponentSize == sizeof(float)) ? FLAG_POINTS_32 : FLAG_POINTS_16;

        uiCombination |= 
            (uiTargetComponentSize == sizeof(float)) ? FLAG_TARGETS_32 : FLAG_TARGETS_16;
        
        if (bNormalize)
            uiCombination |= FLAG_NORMALIZE;

        m_eCombination = (EMorphingKernelDataCombination)uiCombination;
    }

private:
    /// The stride of the data stream.
    EMorphingKernelDataCombination m_eCombination;
};

#endif // #ifndef NIMORPHINGKERNELDATA_H
