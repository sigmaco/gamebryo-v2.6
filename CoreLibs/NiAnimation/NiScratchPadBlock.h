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

#ifndef NISCRATCHPADBLOCK_H
#define NISCRATCHPADBLOCK_H

#include "NiAnimationLibType.h"

class NiEvaluatorSPData;

// *** begin Emergent internal use only ***

// Defines the signature for all scratch pad fill functions.
typedef bool (*NiScratchPadFillFunc)(float fTime, 
    NiEvaluatorSPData* pkEvalSPData);

// Enumeration of the various types of memory blocks held 
// within the scratch pad.

enum NiScratchPadBlock
{
    SPBFILLDATA = 0,
    SPBEVALUATORSPDATA,
    SPBBSPLINEBASISDATA, 
    SPBLINEARCOLORSEGMENT, 
    SPBBSPLINECOLORSEGMENT, 
    SPBCONSTANTBOOLSEGMENT, 
    SPBCUBICFLOATSEGMENT, 
    SPBBSPLINEFLOATSEGMENT, 
    SPBCUBICPOINT3SEGMENT, 
    SPBBSPLINEPOINT3SEGMENT, 
    SPBSLERPROTSEGMENT, 
    SPBSQUADROTSEGMENT, 
    SPBBSPLINEROTSEGMENT, 
    SPBMAXSCRATCHPADBLOCKS
};

// *** end Emergent internal use only ***

#endif
