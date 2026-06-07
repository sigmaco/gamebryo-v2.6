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
#include <NiParticlePCH.h>

#include "NiPSSimulatorCollidersKernel.h"
#include <NiPSColliderHelpers.h>
#include <NiMemoryDefines.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPSSimulatorCollidersKernel)
{
    // Get input streams.
    const NiPSSimulatorCollidersKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPSSimulatorCollidersKernelStruct>(0);
    const NiUInt8* pucIData = kWorkload.GetInput<NiUInt8>(1);
    const float* pfILastUpdateTimes = kWorkload.GetInput<float>(2);
    const NiPoint3* pkIPositions = kWorkload.GetInput<NiPoint3>(3);
    const NiPoint3* pkIVelocities = kWorkload.GetInput<NiPoint3>(4);
    const NiUInt32* puiIFlags = kWorkload.GetInput<NiUInt32>(5);

    // Get output streams.
    NiPoint3* pkOPositions = kWorkload.GetOutput<NiPoint3>(0);
    NiPoint3* pkOVelocities = kWorkload.GetOutput<NiPoint3>(1);
    NiUInt32* puiOFlags = kWorkload.GetOutput<NiUInt32>(2);
    float* pfOLastUpdateTimes = kWorkload.GetOutput<float>(3);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    // Get collider count.
    const NiUInt32 uiNumColliders = pkIStruct->m_uiColliderCount;

    // This task should not be submitted when there are no colliders.
    NIASSERT((pkIStruct->m_stDataBufferSize > 0) && (uiNumColliders > 0));

    // Allocate space for an array of collider pointers, and for a
    // dynamically updated copy of it
    const NiUInt8** pucAvailable =
        NiStackAlloc(const NiUInt8*, uiNumColliders);
    const NiUInt8** pucNotYetCollided =
        NiStackAlloc(const NiUInt8*, uiNumColliders);

    // Fill array of available colliders
    size_t stDataOffset = 0;
    for (unsigned int ui = 0; ui < uiNumColliders; ++ui)
    {
        pucAvailable[ui] = pucIData + stDataOffset;

        NiUInt32 uiColliderType = *((NiUInt32*) (pucIData + stDataOffset));
        stDataOffset += 16 + pkIStruct->m_astDataRanges[uiColliderType];
    }
    NIASSERT(stDataOffset == pkIStruct->m_stDataBufferSize);

    // Iterate over all particles, detecting and handling collisions for each.
    static const NiUInt32 uiUnknownCollider = (NiUInt32) -1;
    static const NiUInt32 uiNoCollider = (NiUInt32) -2;
    bool bDirty = true;
    for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
    {
        if (bDirty)
        {
            for (NiUInt32 uj = 0; uj < uiNumColliders; ++uj)
                pucNotYetCollided[uj] = pucAvailable[uj];
            bDirty = false;
        }
        
        // Detect collisions.
        float fCollisionTime = pkIStruct->m_fCurrentTime;
        NiPoint3 kCollisionPoint(0.0f, 0.0f, 0.0f);
        NiUInt32 uiClosestCollider = uiUnknownCollider;
        NiUInt32 uiRemaining = uiNumColliders;

        float fILastUpdateTime = pfILastUpdateTimes[ui];
        NiPoint3 kIPosition = pkIPositions[ui];
        NiPoint3 kIVelocity = pkIVelocities[ui];
        NiUInt32 uiIFlags = puiIFlags[ui];

        // Allow one collision per collider per particle per step
        while (uiRemaining && (uiClosestCollider == uiUnknownCollider))
        {
            for (NiUInt32 uj = 0; uj < uiRemaining; uj++)
            {
                // Get collider type. The collider type is contained in
                // pucIData immediately followed by the data struct for that
                // collider.
                const NiUInt8* pucCurrentCollider = pucNotYetCollided[uj];
                NiUInt32 uiColliderType =*((NiUInt32*)pucCurrentCollider);
                pucCurrentCollider += 16;

                // Determine the closest collision.
                switch (uiColliderType)
                {
                    case NiPSColliderDefinitions::COLLIDER_PLANAR:
                    {
                        const NiPSColliderDefinitions::PlanarColliderData*
                            pkPlanarData = (const
                            NiPSColliderDefinitions::PlanarColliderData*)
                            (pucCurrentCollider);
                        if (NiPSColliderHelpers::DetectPlanarCollision(
                            fCollisionTime,
                            kCollisionPoint,
                            fILastUpdateTime,
                            kIPosition,
                            kIVelocity,
                            *pkPlanarData))
                        {
                            uiClosestCollider = uj;
                        }
                        break;
                    }
                    case NiPSColliderDefinitions::COLLIDER_SPHERICAL:
                    {
                        const NiPSColliderDefinitions::SphericalColliderData*
                            pkSphericalData = (const
                            NiPSColliderDefinitions::SphericalColliderData*)
                            (pucCurrentCollider);
                        if (NiPSColliderHelpers::DetectSphericalCollision(
                            fCollisionTime,
                            kCollisionPoint,
                            fILastUpdateTime,
                            kIPosition,
                            kIVelocity,
                            *pkSphericalData))
                        {
                            uiClosestCollider = uj;
                        }
                        break;
                    }
                }
            }
            
            // If a collision was found, handle it.
            if (uiClosestCollider != uiUnknownCollider)
            {
                // Get collider type. The collider type is contained in
                // pucIData immediately followed by the data struct for that
                // collider.
                const NiUInt8* pucCurrentCollider =
                    pucNotYetCollided[uiClosestCollider];
                NiUInt32 uiColliderType = *((NiUInt32*)pucCurrentCollider);
                pucCurrentCollider += 16;

                // Handle the collision.
                switch (uiColliderType)
                {
                    case NiPSColliderDefinitions::COLLIDER_PLANAR:
                    {
                        const NiPSColliderDefinitions::PlanarColliderData*
                            pkPlanarData = (const
                            NiPSColliderDefinitions::PlanarColliderData*)
                            pucCurrentCollider;
                        NiPSColliderHelpers::HandlePlanarCollision(
                            pkOPositions[ui],
                            pkOVelocities[ui],
                            puiOFlags[ui],
                            pfOLastUpdateTimes[ui],
                            fCollisionTime,
                            kCollisionPoint,
                            kIVelocity,
                            uiIFlags,
                            fILastUpdateTime,
                            *pkPlanarData,
                            pkIStruct->m_fCurrentTime);
                        break;
                    }
                    case NiPSColliderDefinitions::COLLIDER_SPHERICAL:
                    {
                        const NiPSColliderDefinitions::SphericalColliderData*
                            pkSphericalData = (const
                            NiPSColliderDefinitions::SphericalColliderData*)
                            pucCurrentCollider;
                        NiPSColliderHelpers::HandleSphericalCollision(
                            pkOPositions[ui],
                            pkOVelocities[ui],
                            puiOFlags[ui],
                            pfOLastUpdateTimes[ui],
                            fCollisionTime,
                            kCollisionPoint,
                            kIVelocity,
                            uiIFlags,
                            fILastUpdateTime,
                            *pkSphericalData,
                            pkIStruct->m_fCurrentTime);
                        break;
                    }
                    default:
                        // Unknown collider type. Copy input data to output
                        // data buffers.
                        pkOPositions[ui] = kIPosition;
                        pkOVelocities[ui] = kIVelocity;
                        puiOFlags[ui] = uiIFlags;
                        pfOLastUpdateTimes[ui] = fILastUpdateTime;
                        break;
                }
                
                // Get ready for the next check
                fILastUpdateTime = pfOLastUpdateTimes[ui];
                kIPosition = pkOPositions[ui];
                kIVelocity = pkOVelocities[ui];
                uiIFlags = puiOFlags[ui];

                // Remove the collider from consideration
                pucNotYetCollided[uiClosestCollider] =
                    pucNotYetCollided[--uiRemaining];
                bDirty = true;

                uiClosestCollider = uiUnknownCollider;
            }
            else
            {
                // No collision. Copy input data to output data buffers.
                pkOPositions[ui] = kIPosition;
                pkOVelocities[ui] = kIVelocity;
                puiOFlags[ui] = uiIFlags;
                pfOLastUpdateTimes[ui] = fILastUpdateTime;
                uiClosestCollider = uiNoCollider;
            }
        }
    }
    
    NiStackFree(pucAvailable);
    NiStackFree(pucNotYetCollided);
}
NiSPEndKernelImpl(NiPSSimulatorCollidersKernel)
//---------------------------------------------------------------------------
