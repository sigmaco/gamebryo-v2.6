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

#ifndef NICALCULATEBONEMATRICESKERNEL_H
#define NICALCULATEBONEMATRICESKERNEL_H

#include <NiSPKernelMacros.h>
#include <NiTransform.h>

/**
    The NiCBMKernelData structure is used to pass fixed input data to the
    NiCalculateBoneMatricesKernel Floodgate kernel. Applications should never
    use this structure.
*/
struct NiCBMKernelData
{
    /// World to skin transform used for calculate bone matrices.
    NiTransform m_kWorldToSkin;

    /// Skinning mode (hardware or software).
    bool m_bSoftwareSkinned;
};

/** 
    The NiCalculateBoneMatricesKernel is a Floodgate kernel class used
    to compute the transformed bone matrices for hardware and software
    skinning. 

    It is used by the NiSkinningMeshModifier class and should never
    need to be used directly by an application.
*/
NiSPDeclareKernel(NiCalculateBoneMatricesKernel)

#endif

