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

#ifndef NICOMPUTILITY_H
#define NICOMPUTILITY_H

#include "NiMainLibType.h"
#include <NiMemObject.h>

class NIMAIN_ENTRY NiCompUtility : public NiMemObject
{
    public:
        class NiCompArray : public NiMemObject
        {
            public:
                short* m_asArray; // The array of compressed values
                float m_fOffset; // Used to recenter the compressed value
                float m_fHalfRange; // Used to rescale the compressed value
                unsigned int m_uiNumValues; // The number of compressed values.
        };

        static NiCompArray CompressFloatArray(
            const float* pfSrcArray, unsigned int uiSrcCount);

        static void DecompressFloatArray(const short* psSrcArray,
            unsigned int uiSrcCount, float fOffset, float fHalfRange,
            float* pfDestArray, unsigned int uiDestCount);

    protected:
        static void GetMinMax(const float* pfArray, 
            unsigned int uiCount, float& fMin, float& fMax);
};
#endif
