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


#include "NiTerrainPhysXPCH.h"
#include "NiTerrainPhysXUtils.h"

const NiUInt32 NiTerrainPhysXUtils::ms_uiCurrentVersion = 0x0100;
NiFixedString NiTerrainPhysXUtils::ms_kStaticFriction;
NiFixedString NiTerrainPhysXUtils::ms_kDynamicFriction;
NiFixedString NiTerrainPhysXUtils::ms_kRestitution;

NiTerrainPhysXUtils::NiTerrainPhysXUtils()
{

}
//---------------------------------------------------------------------------
NiTerrainPhysXUtils::~NiTerrainPhysXUtils()
{
    m_kMaterialIndexMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiTerrainPhysXUtils::SDM_Init()
{
    if (ms_kStaticFriction.GetLength() == 0)
    {
        ms_kStaticFriction = "NX_STATICFRICTION";
        ms_kDynamicFriction = "NX_DYNAMICFRICTION";
        ms_kRestitution = "NX_RESTITUTION";
    }
}
//---------------------------------------------------------------------------
void NiTerrainPhysXUtils::SDM_Shutdown()
{
    if (ms_kStaticFriction)
    {
        ms_kStaticFriction = NULL;
        ms_kDynamicFriction = NULL;
        ms_kRestitution = NULL;
    }
}
//---------------------------------------------------------------------------
bool NiTerrainPhysXUtils::CreatePhysXActor(
    NiPhysXScene* spScene, 
	NxActorDesc kActorDesc, NiTPrimitiveArray<NxActor*>& kActorArray,
    NiTerrainDataLeaf* pkQuadLeaf)
{   
    if (pkQuadLeaf->GetChildAt(0) == 0)
    {
        NxActor* pkActor = BuildPhysXData(spScene, kActorDesc, pkQuadLeaf);
        if (pkActor)
        {            
            kActorArray.Add(pkActor);
            return true;
        }
        return false;
    }
    
    for (NiUInt32 i = 0; i < 4; ++i)
	{
        if (!CreatePhysXActor(spScene, kActorDesc, kActorArray, 
            pkQuadLeaf->GetChildAt(i)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainPhysXUtils::CreatePhysXActor(
    NiTerrainSector* pkSector,
    NiPhysXScene* spScene, NxActorDesc kActorDesc, 
    NiTPrimitiveArray<NxActor*>& kActorArray, bool bCreateAlways,
    NiUInt8* puiErrorCode, bool bLoadOutOfDate)
{
    NiUInt8 uiDummyRC;
    if (puiErrorCode == 0)
        puiErrorCode = &uiDummyRC;

    if (!pkSector)
    {
        *puiErrorCode |= RC_INVALID_SECTOR;
        return false;
    }

    // Check if we have all levels of detail loaded:
	if (pkSector->GetSectorData()->GetHighestLoadedLOD() != 
        (NiInt32)pkSector->GetSectorData()->GetNumLOD())
	{
		// Cannot perform this type of colision 
        *puiErrorCode |= RC_INVALID_SECTOR;
		return false;
	}
    
    // only attempt to load the file if create always is set to false
    if (!bCreateAlways)
    {
        if (LoadPhysXActors(pkSector, spScene, kActorArray, kActorDesc, 
            puiErrorCode, bLoadOutOfDate))
        {
            return true;
        }
    }

    // We didn't find any cooked data so we are going to build it     
    if (!CreatePhysXActor(spScene, kActorDesc, kActorArray, 
        pkSector->GetLeaf(0)))
    {
        return false;
    }

    *puiErrorCode |= RC_BUILT_DATA;
    return true;
    
}
//---------------------------------------------------------------------------
NxActor* NiTerrainPhysXUtils::BuildPhysXData(NiPhysXScene* pkScene, 
	NxActorDesc kActorDesc, NiTerrainDataLeaf* pkQuadLeaf)
{
    if (!pkQuadLeaf)
        return NULL;
    
    NiTerrainPositionRandomAccessIterator kOriPositionIter;
    pkQuadLeaf->GetPositionIterator(kOriPositionIter);
    NiTStridedRandomAccessIterator<NiUInt16> kOriIndIter16;
    NiTStridedRandomAccessIterator<NiUInt32> kOriIndIter32;
    bool bUseIndex16 = false;
    NiTerrainSector* pkSector = pkQuadLeaf->GetContainingSector();
    
    if (pkSector->GetUsingShortIndexBuffer())
    {
        pkQuadLeaf->GetIndexIterator(kOriIndIter16);
        bUseIndex16 = true;
    }
    else
    {
        pkQuadLeaf->GetIndexIterator(kOriIndIter32);
    }
    
    NiUInt32 uiBlockSize = pkQuadLeaf->GetWidthInVerts();
    NiUInt32 uiBlockWidth = pkQuadLeaf->GetBlockSize();
    NiUInt32 uiNumVerts = uiBlockSize * uiBlockSize;
    
    NiIndex kLeafMinIndex;   
    pkQuadLeaf->GetBottomLeftIndex(kLeafMinIndex);
    NiIndex kLeafMaxIndex(kLeafMinIndex.x + uiBlockSize,
        kLeafMinIndex.y + uiBlockSize);
    
    // Determine extents
    float fHeightMinZ = FLT_MAX;
    float fHeightMaxZ = -FLT_MAX;
    float fMaxX = -FLT_MAX;
    float fMinX = FLT_MAX;
    float fMaxY = -FLT_MAX;
    float fMinY = FLT_MAX;

    NiUInt32 uiMaxX = uiBlockSize;
    NiUInt32 uiMaxY = uiBlockSize;

    for (NiUInt32 y = 0; y < uiMaxY; ++y)
    {
        for (NiUInt32 x = 0; x < uiMaxX; ++x)
        {
            NiPoint3 kHiDetail;
            kOriPositionIter.GetHighDetail(y * uiBlockSize + x, kHiDetail);

            if (fMaxX < kHiDetail.x)
                fMaxX = kHiDetail.x;
            if (fMinX > kHiDetail.x)
                fMinX = kHiDetail.x;
            if (fMaxY < kHiDetail.y)
                fMaxY = kHiDetail.y;
            if (fMinY > kHiDetail.y)
                fMinY = kHiDetail.y;
            if (fHeightMinZ > kHiDetail.z)
                fHeightMinZ = kHiDetail.z;
            if (fHeightMaxZ < kHiDetail.z)
                fHeightMaxZ = kHiDetail.z;
        }
    }
      
    // The PhysX mesh data
    // Iterate over the vertices that will represent the height field
    // and retrieve the min z- and max z-value to be able to get the best 
    // resolution of the height data.     
    float fMiddleHeight = (fHeightMaxZ + fHeightMinZ) / 2.0f;
    float fMinMaxDistanceHalf = (fHeightMaxZ - fHeightMinZ) / 2.0f;
    float fThirtyTwoKb = 32767.0f;

    // Create the NxHeightField data and set the sample data on it.
    NxHeightFieldDesc kHeightFieldDesc;    
    kHeightFieldDesc.nbColumns = uiBlockSize;
    kHeightFieldDesc.nbRows = uiBlockSize;
    kHeightFieldDesc.convexEdgeThreshold = 0;
        
    //Allocate storage for samples.
    kHeightFieldDesc.samples = NiAlloc(NxU32, uiNumVerts);
    kHeightFieldDesc.sampleStride = sizeof(NxU32);
    char* pucCurrentByte = (char*)kHeightFieldDesc.samples;
    
    for (NiUInt32 y = 0; y < uiMaxY; ++y)
    {
        for (NiUInt32 x = 0; x < uiMaxX; ++x)
        {
            NiUInt32 uiIndex = y * uiBlockSize + x;
            
            NiPoint3 kHiDetail;
            kOriPositionIter.GetHighDetail(uiIndex, kHiDetail);
            float fZ = kHiDetail.z;
            
            NxHeightFieldSample* pkCurrentSample = 
                (NxHeightFieldSample*)pucCurrentByte;

            // Calculate how this vertex z-value is relative the min and max 
            // z-values and scaled into the resolution of an NxI16.
            NxI16 sHeight = (NxI16)(fThirtyTwoKb * 
                (fZ - fMiddleHeight) / fMinMaxDistanceHalf);

            pkCurrentSample->height = sHeight;
            
            // Work out the sample tessalation
            // We don't deal with the border points as they would be outside 
            // the sample tile.
            if (y != 32 && x != 32)
            {
                NiUInt32 uiIndexValue[3];
                NiUInt32 uiIndStreamIndex = (y * uiBlockWidth + x) * 6;
                if (bUseIndex16)
                {
                    uiIndexValue[0] = kOriIndIter16[uiIndStreamIndex];
                    uiIndexValue[1] = kOriIndIter16[uiIndStreamIndex + 1];
                    uiIndexValue[2] = kOriIndIter16[uiIndStreamIndex + 2];
                }
                else
                {
                    uiIndexValue[0] = kOriIndIter32[uiIndStreamIndex];
                    uiIndexValue[1] = kOriIndIter32[uiIndStreamIndex + 1];
                    uiIndexValue[2] = kOriIndIter32[uiIndStreamIndex + 2];
                }

                // now that we have the indexvalue for the tile points we can
                // work out the tesselation

                NiUInt32 uiCount = 0;
                while (uiIndexValue[0] + 1 != uiIndexValue[1] && uiCount < 3)
                {
                    NiUInt32 uiTemp = uiIndexValue[0];
                    uiIndexValue[0] = uiIndexValue[1];
                    uiIndexValue[1] = uiIndexValue[2];
                    uiIndexValue[2] = uiTemp;
                    uiCount++;
                }
                
                if (uiIndexValue[0] + 1 == uiIndexValue[1])
                {
                    if (uiIndexValue[0] + uiBlockSize == uiIndexValue[2])
                        pkCurrentSample->tessFlag = 1;
                    else
                        pkCurrentSample->tessFlag = 0;
                }
                else
                {
                    // This is a default case that should not be hit
                    pkCurrentSample->tessFlag = 1;
                }
                                
            }

            NiUInt32 usMatIndex0, usMatIndex1; 
            GetDominantPhysXMaterialIndex(pkScene, pkQuadLeaf, x, y, 
                pkCurrentSample->tessFlag, usMatIndex0, usMatIndex1); 
            
            pkCurrentSample->materialIndex0 = usMatIndex0;
            pkCurrentSample->materialIndex1 = usMatIndex1;
                                    
            pucCurrentByte += kHeightFieldDesc.sampleStride;            
        }
    } 

    NiTransform kTransform = pkSector->GetSectorData()->GetWorldTransform();
        
    NiPoint3 kWorldCenter = kTransform * 
        NiPoint3(fMinX, fMinY, fMiddleHeight);
    NxVec3 kCenterPoint;
    NiPhysXTypes::NiPoint3ToNxVec3(kWorldCenter, kCenterPoint);

    NxScene* pkNxScene = pkScene->GetPhysXScene();

    NxHeightField* pkHeightField = 
        pkNxScene->getPhysicsSDK().createHeightField(kHeightFieldDesc);
    
    //Data has been copied therefore free the buffers.
    NiFree(kHeightFieldDesc.samples);

    NxHeightFieldShapeDesc kHeightFieldShapeDesc;
    kHeightFieldShapeDesc.heightField = pkHeightField; 
    kHeightFieldShapeDesc.heightScale = (kTransform.m_fScale *
        fMinMaxDistanceHalf / fThirtyTwoKb) / pkScene->GetScaleFactor();
    kHeightFieldShapeDesc.rowScale = (kTransform.m_fScale *
        (fMaxX - fMinX) / NxReal(uiBlockWidth)) / pkScene->GetScaleFactor();
    kHeightFieldShapeDesc.columnScale = (kTransform.m_fScale *
        (fMaxY - fMinY) / NxReal(uiBlockWidth)) / pkScene->GetScaleFactor();
    kHeightFieldShapeDesc.materialIndexHighBits = 0;            
    kHeightFieldShapeDesc.holeMaterial = 100;
    kHeightFieldShapeDesc.shapeFlags =
        NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;

    // Setup a transformation that tranforms the height field's local
    // up vector/y-axis so it's parallel with the world up vector/z-axis.
    // Also add the translation to translate the height field to the 
    // appriopriate position. We as well need to transform according to the 
    // terrain's location.
    NxMat34 kPose;
    kPose.t = kCenterPoint / pkScene->GetScaleFactor();
    
    NiMatrix3 kRotation = NiMatrix3(NiPoint3(0.0f, 1.0f, 0.0f), 
        NiPoint3(0.0f, 0.0f, 1.0f),
        NiPoint3(1.0f, 0.0f, 0.0f));
    kRotation = kTransform.m_Rotate * kRotation;
    NiPhysXTypes::NiMatrix3ToNxMat33(kRotation, kPose.M);
    
    kActorDesc.name = pkQuadLeaf->GetQuadMesh()->GetName();
    kActorDesc.shapes.pushBack(&kHeightFieldShapeDesc);
    kActorDesc.globalPose = kPose;

    NxActor* pkActor = pkNxScene->createActor(kActorDesc);
      
    if (pkActor)
        return pkActor;

    return NULL;
}
//---------------------------------------------------------------------------
void NiTerrainPhysXUtils::GetDominantPhysXMaterialIndex(NiPhysXScene* pkScene,
	NiTerrainDataLeaf* pkQuadLeaf, NiUInt32 x, NiUInt32 y, 
    bool bTessFlag, NiUInt32& uiMatIndex0, NiUInt32& usMatIndex1)
{
    // Get the leaf that holds the surface mask
    NiTerrainDataLeaf* pkParentLeaf  = pkQuadLeaf;
    NiUInt32 uiScale = 0;
    
    while (pkParentLeaf && pkParentLeaf->GetSurfaceCount() == 0)
    {
        pkParentLeaf = pkParentLeaf->GetParent();
        ++uiScale;
    }
    if (!pkParentLeaf)
        return;

    // Calculate the scaling for the mask by remembering how high you had
    // to count to the parent (Can use GetLevel of both parent and current 
    // leaf to quickly get this)

    // Calculate offset by comparing the BottomLeft Indices of parent and
    // current leaf.
    NiIndex kLeafIndex;
    NiIndex kParentIndex;
    pkQuadLeaf->GetBottomLeftIndex(kLeafIndex);
    pkParentLeaf->GetBottomLeftIndex(kParentIndex);

    NiIndex kOffset(kLeafIndex.x - kParentIndex.x, 
        kLeafIndex.y - kParentIndex.y);
    NiIndex kCurrentIndex(x + kLeafIndex.x, y + kLeafIndex.y);
    
    float fParentWidth = (float)
        ((pkQuadLeaf->GetWidthInVerts() - 1) * NiPow(2, (float)uiScale));
    
    if (kCurrentIndex.x >= fParentWidth || kCurrentIndex.y >= fParentWidth)
        return;

    float fMaxSum0 = 0.0f;
    float fCurrentBlendedValue0 = 0.0f;
    float fMaxSum1 = 0.0f;
    float fCurrentBlendedValue1 = 0.0f;
    for (NiUInt32 ui = 0; ui < pkParentLeaf->GetSurfaceCount(); ++ui)
    {
        const NiSurface* pkSurface = pkParentLeaf->GetSurface(ui);
        const NiSurfaceMask* pkCurrentMask = 
            pkParentLeaf->GetSurfaceMask(pkSurface);

        NiUInt16 usFloorX = (NiUInt16)((kCurrentIndex.x / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));
        NiUInt16 usFloorY = (NiUInt16)((kCurrentIndex.y / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));

        NiUInt16 usCeilX = (NiUInt16)(((kCurrentIndex.x + 1) / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));
        NiUInt16 usCeilY = (NiUInt16)(((kCurrentIndex.y + 1) / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));   

        if (usFloorX == usCeilX)
            ++usCeilX;

        if (usFloorY == usCeilY)
            ++usCeilY;
                
        NiUInt32 uiSum0 = 0;
        NiUInt32 uiSum1 = 0;
        NiUInt8 ucCount0 = 0;
        NiUInt8 ucCount1 = 0;
        for (NiInt32 uj = usFloorY; uj < usCeilY; ++uj)
        {
            for (NiInt32 uk = usFloorX; uk < usCeilX; ++uk)
            {

                if (!bTessFlag)
                {
                    // in this case the line equation is y = x therefore
                    // is y > to x then we are in triangle 1 else we are in 
                    // triangle 0;
                    if (usFloorY >= usFloorX)
                    {
                        uiSum1 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount1;
                    }
                    else
                    {
                        uiSum0 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount0;
                    }
                }
                else
                {
                    // in this case the line equation is y = -x + 1 therefore
                    // is y > to -x + 1 then we are in triangle 0 else we are 
                    // in triangle 1;
                    if (usFloorY < - usFloorX + 1)
                    {
                        uiSum1 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount1;
                    }
                    else
                    {
                        uiSum0 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount0;
                    }
                }
                
            }
        }

        NiUInt32 uiPriority;
        pkParentLeaf->GetSurfacePriority(pkSurface, uiPriority);

        // if one ucCount = 0 then the mask is smaller than the terrain and 
        // one of the triangles will not be given a material therefore both
        // triangle should have the same material
        if (ucCount0 == 0)
        {
            uiSum0 = uiSum1;
            ucCount0 = ucCount1;
        }
        else if (ucCount1 == 0)
        {
            uiSum1 = uiSum0;
            ucCount1 = ucCount0;
        }

        float fSum0 = (float)(uiSum0) / (ucCount0);
        float fSum1 = (float)(uiSum1) / (ucCount1);

        float fAvailValue0 = 255 - fCurrentBlendedValue0;
        if (fSum0 > fAvailValue0)
            fSum0 = fAvailValue0;

        fCurrentBlendedValue0 += fSum0;

        float fAvailValue1 = 255 - fCurrentBlendedValue1;
        if (fSum1 > fAvailValue1)
            fSum1 = fAvailValue1;

        fCurrentBlendedValue1 += fSum1;

        if (fSum0 > fMaxSum0)
        {
            fMaxSum0 = fSum0;
            uiMatIndex0 = GetPhyXMaterialIndex(pkScene, pkSurface);
        }

        if (fSum1 > fMaxSum1)
        {
            fMaxSum1 = fSum1;
            usMatIndex1 = GetPhyXMaterialIndex(pkScene, pkSurface);
        }
        
    }    
}
//---------------------------------------------------------------------------
void NiTerrainPhysXUtils::GetDominantSurface(NiTerrainDataLeaf* pkQuadLeaf,
    const NiTMap<const NiSurface*, NiUInt16>& kSurfaceMapIndex,
    NiUInt32 x, NiUInt32 y, bool bTessFlag, NiUInt16& uiMatIndex0, 
    NiUInt16& uiMatIndex1)
{
    // Get the leaf that holds the surface mask
    NiTerrainDataLeaf* pkParentLeaf  = pkQuadLeaf;
    NiUInt32 uiScale = 0;
    
    while (pkParentLeaf && pkParentLeaf->GetSurfaceCount() == 0)
    {
        pkParentLeaf = pkParentLeaf->GetParent();
        ++uiScale;
    }
    if (!pkParentLeaf)
        return;

    // Calculate the scaling for the mask by remembering how high you had
    // to count to the parent (Can use GetLevel of both parent and current 
    // leaf to quickly get this)

    // Calculate offset by comparing the BottomLeft Indices of parent and
    // current leaf.
    NiIndex kLeafIndex;
    NiIndex kParentIndex;
    pkQuadLeaf->GetBottomLeftIndex(kLeafIndex);
    pkParentLeaf->GetBottomLeftIndex(kParentIndex);

    NiIndex kOffset(kLeafIndex.x - kParentIndex.x, 
        kLeafIndex.y - kParentIndex.y);
    NiIndex kCurrentIndex(x + kLeafIndex.x, y + kLeafIndex.y);
    
    float fParentWidth = 
        ((pkQuadLeaf->GetWidthInVerts() - 1) * NiPow(2, (float)uiScale));
    
    if (kCurrentIndex.x >= fParentWidth || kCurrentIndex.y >= fParentWidth)
        return;

    float fMaxSum0 = 0.0f;
    float fCurrentBlendedValue0 = 0.0f;
    float fMaxSum1 = 0.0f;
    float fCurrentBlendedValue1 = 0.0f;
    for (NiUInt32 ui = 0; ui < pkParentLeaf->GetSurfaceCount(); ++ui)
    {
        const NiSurface* pkSurface = pkParentLeaf->GetSurface(ui);
        const NiSurfaceMask* pkCurrentMask = 
            pkParentLeaf->GetSurfaceMask(pkSurface);

        NiUInt16 usFloorX = (NiUInt16)((kCurrentIndex.x / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));
        NiUInt16 usFloorY = (NiUInt16)((kCurrentIndex.y / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));

        NiUInt16 usCeilX = (NiUInt16)(((kCurrentIndex.x + 1) / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));
        NiUInt16 usCeilY = (NiUInt16)(((kCurrentIndex.y + 1) / fParentWidth) * 
            float(pkCurrentMask->GetWidth()));   

        if (usFloorX == usCeilX)
            ++usCeilX;

        if (usFloorY == usCeilY)
            ++usCeilY;
                
        NiUInt32 uiSum0 = 0;
        NiUInt32 uiSum1 = 0;
        NiUInt8 ucCount0 = 0;
        NiUInt8 ucCount1 = 0;
        for (NiInt32 uj = usFloorY; uj < usCeilY; ++uj)
        {
            for (NiInt32 uk = usFloorX; uk < usCeilX; ++uk)
            {

                if (!bTessFlag)
                {
                    // in this case the line equation is y = x therefore
                    // is y > to x then we are in triangle 1 else we are in 
                    // triangle 0;
                    if (usFloorY >= usFloorX)
                    {
                        uiSum1 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount1;
                    }
                    else
                    {
                        uiSum0 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount0;
                    }
                }
                else
                {
                    // in this case the line equation is y = -x + 1 therefore
                    // is y > to -x + 1 then we are in triangle 0 else we are 
                    // in triangle 1;
                    if (usFloorY < - usFloorX + 1)
                    {
                        uiSum1 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount1;
                    }
                    else
                    {
                        uiSum0 += pkCurrentMask->GetAt(uk, uj);
                        ++ucCount0;
                    }
                }
                
            }
        }

        NiUInt32 uiPriority;
        pkParentLeaf->GetSurfacePriority(pkSurface, uiPriority);

        // if one ucCount = 0 then the mask is smaller than the terrain and 
        // one of the triangles will not be given a material therefore both
        // triangle should have the same material
        if (ucCount0 == 0)
        {
            uiSum0 = uiSum1;
            ucCount0 = ucCount1;
        }
        else if (ucCount1 == 0)
        {
            uiSum1 = uiSum0;
            ucCount1 = ucCount0;
        }

        float fSum0 = (float)(uiSum0) / (ucCount0);
        float fSum1 = (float)(uiSum1) / (ucCount1);

        float fAvailValue0 = 255 - fCurrentBlendedValue0;
        if (fSum0 > fAvailValue0)
            fSum0 = fAvailValue0;

        fCurrentBlendedValue0 += fSum0;

        float fAvailValue1 = 255 - fCurrentBlendedValue1;
        if (fSum1 > fAvailValue1)
            fSum1 = fAvailValue1;

        fCurrentBlendedValue1 += fSum1;

        if (fSum0 > fMaxSum0)
        {
            fMaxSum0 = fSum0;
            NiUInt16 usMat;
            kSurfaceMapIndex.GetAt(pkSurface, usMat); 
            uiMatIndex0 = usMat;
        }

        if (fSum1 > fMaxSum1)
        {
            fMaxSum1 = fSum1;
            NiUInt16 usMat;
            kSurfaceMapIndex.GetAt(pkSurface, usMat); 
            uiMatIndex1 = usMat;
        }        
    }
}
//---------------------------------------------------------------------------
NiUInt16 NiTerrainPhysXUtils::GetPhyXMaterialIndex(NiPhysXScene* pkScene, 
    const NiSurface* pkSurface)
{
    NiUInt16 usIndex = 0;

    // check if we have already created the physx material
    if (m_kMaterialIndexMap.GetAt(pkSurface, usIndex))    
        return usIndex;

    // create the surface material
    // Setup the NxMaterial. if no Meta data is found then we will
    // set to use the default material values.
    NxMaterialDesc kMatDesc;
    kMatDesc.setToDefault();

    float fValue = 0;
    float fWeight = 0;
    if (pkSurface->GetMetaData().Get(ms_kRestitution, fValue, fWeight))
    {
        kMatDesc.restitution = fValue;         
    }

    if (pkSurface->GetMetaData().Get(ms_kDynamicFriction, fValue, fWeight))
    {
        kMatDesc.dynamicFriction = fValue;
    }

    if (pkSurface->GetMetaData().Get(ms_kStaticFriction, fValue, fWeight))
    {
        kMatDesc.staticFriction = fValue;
    }
    
    NxMaterial *pkNewMaterial = 
        pkScene->GetPhysXScene()->createMaterial(kMatDesc);
    
    usIndex = pkNewMaterial->getMaterialIndex();

    m_kMaterialIndexMap.SetAt(pkSurface, usIndex);
    
    return usIndex;
}
//---------------------------------------------------------------------------
NiString NiTerrainPhysXUtils::GetPhysXFilename(
    const NiFixedString &kSectorPath)
{
    NiString kFileName = kSectorPath;
    kFileName.Concatenate("\\quadtree.tpx");
    return kFileName;
}
//---------------------------------------------------------------------------
bool NiTerrainPhysXUtils::LoadPhysXActors(
    NiTerrainSector* pkSector, NiPhysXScene* pkScene, 
    NiTPrimitiveArray<NxActor*>& kActorArray, NxActorDesc kActorDesc,
    NiUInt8* puiErrorCode, bool bLoadOutOfDate)
{
    NiUInt8 ucDummyRC = 0;
    if (!puiErrorCode)
        puiErrorCode = &ucDummyRC;

    // Work out the file name to load according to the sector
    NiFixedString kSectorPath = pkSector->GetSectorPath();
    NiString kFileName = GetPhysXFilename(kSectorPath);

    // Clean up the path, which may have some odd nuances that break it across platforms
    kFileName.Replace("\\\\", "\\");
    kFileName.Replace("/\\", "/");
    kFileName.Replace("//", "/");

    // check the file exist
    if (!NiFile::Access(kFileName, NiFile::READ_ONLY))
    {
        *puiErrorCode |= RC_FILENOTFOUND;
        return false;
    }   

    // Attempt to gain access to the file in this mode!
    NiFile* pkFile = NiFile::GetFile(kFileName, NiFile::READ_ONLY);
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    pkFile->SetEndianSwap(!bPlatformLittle);

    // first read the info from the file header
    NiUInt32 uiFileIterator = 0;

    // Header variables
    NiUInt32 uiFormatVersion = 0;
    bool bUpToDate = true;
    NiUInt32 uiNumSurfaces = 0;
    NiUInt32 uiNumBlocks = 0;

    NiStreamLoadBinary(*pkFile, uiFormatVersion);

    if (uiFormatVersion != ms_uiCurrentVersion)
    {
        *puiErrorCode |= RC_INVALID_FORMAT;
        NiDelete pkFile;
        return false;
    }

    NiStreamLoadBinary(*pkFile, bUpToDate);
    
    if (!bUpToDate && !bLoadOutOfDate)
    {
        *puiErrorCode |= RC_OUTOFDATE;
        NiDelete pkFile;
        return false;
    }

    if (!bUpToDate)
        *puiErrorCode |= RC_OUTOFDATE;

    NiStreamLoadBinary(*pkFile, uiNumSurfaces);
    NiStreamLoadBinary(*pkFile, uiNumBlocks);

    uiFileIterator += 3 * sizeof(NiUInt32) + sizeof(bool); 
    
    NiTMap<NiUInt16, NiFixedString> kPackageMap;
    NiTMap<NiUInt16, NiFixedString> kSurfaceMap;
    for (NiUInt32 uiIter = 0; uiIter < uiNumSurfaces; ++uiIter)
    {
        // read the surface map index        
        NiUInt16 usIndex;
        NiFixedString kPackName;
        NiFixedString kSurfName;
        
        NiStreamLoadBinary(*pkFile, usIndex);
        NiFixedString::LoadCStringAsFixedString(*pkFile, kPackName);
        NiFixedString::LoadCStringAsFixedString(*pkFile, kSurfName);
        
        kPackageMap.SetAt(usIndex, kPackName);
        kSurfaceMap.SetAt(usIndex, kSurfName);
    }

    for (NiUInt32 uiIter = 0; uiIter < uiNumBlocks; ++uiIter)
    {
        // read all blocks
        NxActor* pkActor = LoadBlockData(pkSector, pkScene, pkFile,
            kPackageMap, kSurfaceMap, kActorDesc);

        if (!pkActor)
        {
            for (NiUInt32 ui = 0; ui < kActorArray.GetSize(); ++ui)
            {
                pkScene->GetPhysXScene()->releaseActor(*kActorArray[ui]);
            }
            kActorArray.RemoveAll();
            *puiErrorCode |= RC_FILE_ERROR;
            NiDelete pkFile;
            return false;
        }

        kActorArray.Add(pkActor);
    }

    NiDelete pkFile;

    *puiErrorCode |= RC_LOADED;
    return true;
}
//---------------------------------------------------------------------------
NxActor* NiTerrainPhysXUtils::LoadBlockData(NiTerrainSector* pkSector, 
    NiPhysXScene* pkScene, NiFile* pkFile, 
    const NiTMap<NiUInt16, NiFixedString>& kPackageMap,
    const NiTMap<NiUInt16, NiFixedString>& kSurfaceMap,
    NxActorDesc kActorDesc)
{
    //NxActorDesc kActorDesc;
    NxHeightFieldDesc kHeightFieldDesc;
    NxHeightFieldShapeDesc kHeightFieldShapeDesc;

    // read the block header
    // first the actor descriptor values
    NiFixedString kActorName;
    NiFixedString::LoadCStringAsFixedString(*pkFile, kActorName);
    kActorDesc.name = kActorName;
    NiTransform kTransform;
    
    //We can not read the transformation in with a single NiStreamLoadBinary call,
    //and although NiTransform has a LoadBinary, it works on NiStream, not NiFile.
    //Thus, load the transformation in one element at a time
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            float entry;
            NiStreamLoadBinary(*pkFile, entry);
            kTransform.m_Rotate.SetEntry(row, col, entry);
        }
    }
    NiStreamLoadBinary(*pkFile, kTransform.m_Translate.x);
    NiStreamLoadBinary(*pkFile, kTransform.m_Translate.y);
    NiStreamLoadBinary(*pkFile, kTransform.m_Translate.z);
    NiStreamLoadBinary(*pkFile, kTransform.m_fScale);
    
    // Position needs to be converted to physx space 
    NxVec3 kCenterPoint;
    NiPhysXTypes::NiPoint3ToNxVec3(kTransform.m_Translate, kCenterPoint);
    NxMat34 kPose;
    kPose.t = kCenterPoint / pkScene->GetScaleFactor();    
    NiMatrix3 kRotation = NiMatrix3(NiPoint3(0.0f, 1.0f, 0.0f), 
        NiPoint3(0.0f, 0.0f, 1.0f),
        NiPoint3(1.0f, 0.0f, 0.0f));
    kRotation = kTransform.m_Rotate * kRotation;
    NiPhysXTypes::NiMatrix3ToNxMat33(kRotation, kPose.M);
    kActorDesc.globalPose = kPose;
    
    // then the heightfieldshape descriptor values
    float fHScale;
    float fRScale;
    float fCScale;
    NiUInt32 uiHighbits;
    NiUInt32 uiHole;
    NiStreamLoadBinary(*pkFile, fHScale);
    NiStreamLoadBinary(*pkFile, fRScale);
    NiStreamLoadBinary(*pkFile, fCScale);
    NiStreamLoadBinary(*pkFile, uiHighbits);
    NiStreamLoadBinary(*pkFile, uiHole);
    
    kHeightFieldShapeDesc.heightScale = fHScale / pkScene->GetScaleFactor();
    kHeightFieldShapeDesc.rowScale = fRScale / pkScene->GetScaleFactor();
    kHeightFieldShapeDesc.columnScale = fCScale / pkScene->GetScaleFactor();
    kHeightFieldShapeDesc.materialIndexHighBits = (NxMaterialIndex)uiHighbits; 
    kHeightFieldShapeDesc.holeMaterial = (NxMaterialIndex)uiHole;
    
    // finally the height field descriptor values
    NiUInt32 uiColumn;
    NiUInt32 uiRow;
    float fThickness;
    float fVertical;
    float fConvex;
    NiStreamLoadBinary(*pkFile, uiColumn);
    NiStreamLoadBinary(*pkFile, uiRow);    
    NiStreamLoadBinary(*pkFile, fThickness);
    NiStreamLoadBinary(*pkFile, fVertical);
    NiStreamLoadBinary(*pkFile, fConvex);
    
    kHeightFieldDesc.nbColumns = uiColumn;
    kHeightFieldDesc.nbRows = uiRow;
    kHeightFieldDesc.thickness = fThickness;
    kHeightFieldDesc.verticalExtent = fVertical;
    kHeightFieldDesc.convexEdgeThreshold = fConvex;

    // We can now load the samples

    //Allocate storage for samples.
    kHeightFieldDesc.samples = 
        NiAlloc(NxU32, kHeightFieldDesc.nbColumns * kHeightFieldDesc.nbRows);
    kHeightFieldDesc.sampleStride = sizeof(NiUInt32);
    char* pucCurrentByte = (char*)kHeightFieldDesc.samples;

    NiSurfacePalette* pkPalette = pkSector->GetSurfacePalette();
    
    for (NiUInt32 y = 0; y < kHeightFieldDesc.nbColumns; ++y)
    {
        for (NiUInt32 x = 0; x < kHeightFieldDesc.nbRows; ++x)
        {
            NxHeightFieldSample* pkCurrentSample = 
                (NxHeightFieldSample*)pucCurrentByte;

            NiUInt16 usHeight;
            bool bTessFlag;
            NiStreamLoadBinary(*pkFile, usHeight);
            NiStreamLoadBinary(*pkFile, bTessFlag);
            
            pkCurrentSample->height = usHeight;
            pkCurrentSample->tessFlag = bTessFlag;
            NiUInt16 usSurfaceIndex0;
            NiUInt16 usSurfaceIndex1;

            NiStreamLoadBinary(*pkFile, usSurfaceIndex0);
            NiStreamLoadBinary(*pkFile, usSurfaceIndex1);

            // using the surface map work out which material we are using
            NiFixedString kSurfaceName;
            NiFixedString kPackageName;
            kSurfaceMap.GetAt(usSurfaceIndex0, kSurfaceName);
            kPackageMap.GetAt(usSurfaceIndex0, kPackageName);
            
            NiUInt16 usMatIndex0;
            NiUInt16 usMatIndex1;

            if (kPackageName && kSurfaceName)
            {
                usMatIndex0 = GetPhyXMaterialIndex(pkScene, 
                    pkPalette->GetSurface(kPackageName, kSurfaceName));
            }
            else
            {
                // we can't find the surface. set the material to the default 
                // value
                usMatIndex0 = 0;
            }

            if (usSurfaceIndex0 == usSurfaceIndex1)
            {
                usMatIndex1 = usMatIndex0;
            }
            else
            {
                kSurfaceMap.GetAt(usSurfaceIndex1, kSurfaceName);
                kPackageMap.GetAt(usSurfaceIndex1, kPackageName);
                
                if (kPackageName && kSurfaceName)
                {
                    usMatIndex1 = GetPhyXMaterialIndex(pkScene, 
                        pkPalette->GetSurface(kPackageName, kSurfaceName));
                }
                else
                {
                    // we can't find the surface. set the material to the 
                    // default value
                    usMatIndex1 = 0;
                }
            }

            pkCurrentSample->materialIndex0 = usMatIndex0;
            pkCurrentSample->materialIndex1 = usMatIndex1;
            
            // last unused bit
            bool ucReserved;
            NiStreamLoadBinary(*pkFile, ucReserved);
            pkCurrentSample->unused = ucReserved;

            pucCurrentByte += kHeightFieldDesc.sampleStride;
        }
    } 

    NxScene* pkNxScene = pkScene->GetPhysXScene();
    NxHeightField* pkHeightField = 
        pkNxScene->getPhysicsSDK().createHeightField(kHeightFieldDesc);
    
    //Data has been copied therefore free the buffers.
    NiFree(kHeightFieldDesc.samples);
    
    kHeightFieldShapeDesc.heightField = pkHeightField; 
    kHeightFieldShapeDesc.shapeFlags =
        NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;

    kActorDesc.shapes.pushBack(&kHeightFieldShapeDesc);    
    NxActor* pkActor = pkNxScene->createActor(kActorDesc);

    return pkActor;
}
//---------------------------------------------------------------------------
bool NiTerrainPhysXUtils::SavePhysXData(NiTerrainSector* pkSector)
{
    // Work out the file name to load according to the sector
    NiFixedString kSectorPath = pkSector->GetSectorPath();
    NiString kFileName = GetPhysXFilename(kSectorPath);
    
    // Clean up the path, which may have some odd nuances that break it across platforms
    kFileName.Replace("\\\\", "\\");
    kFileName.Replace("/\\", "/");
    kFileName.Replace("//", "/");
           
    NiFile* pkFile = NiFile::GetFile(kFileName, NiFile::WRITE_ONLY);
    
    //If the NiFile::GetFile fails then save fails.
    if (pkFile == NULL || !*pkFile)
        return false;

    NiUInt32 uiNumLOD = pkSector->GetSectorData()->GetNumLOD();

    NiTMap<const NiSurface*, NiUInt16> kSurfaceMapIndex;
    
    // We will be saving the surface index according to the surface loaded
    // for this terrain.
    NiTPrimitiveSet<NiSurfacePackage*> kPackages;
    pkSector->GetSurfacePalette()->GetLoadedPackages(kPackages);
    NiUInt16 usNumSurface = 0;
    for(NiUInt32 i = 0; i < kPackages.GetSize(); ++i)
    {
        const NiSurfacePackage* pkCurrentPackage = kPackages.GetAt(i);
        NiTPrimitiveSet<NiSurface*> kSurfaces;
        pkCurrentPackage->GetLoadedSurfaces(kSurfaces);

        for(NiUInt32 j = 0; j < kSurfaces.GetSize(); ++j)
        {
            NiSurface* pkCurrentSurface = kSurfaces.GetAt(j);
            kSurfaceMapIndex.SetAt(pkCurrentSurface, usNumSurface);
            usNumSurface++;
        }
    }
    
    // Now write the header
    bool bUptodate = true;
    
    NiUInt32 uiNumSurfaces = kSurfaceMapIndex.GetCount();
    NiUInt32 uiNumBlocks = (NiUInt32)(NiPow(2.0f, 2.0f * (float)uiNumLOD));
    NiStreamSaveBinary(*pkFile, ms_uiCurrentVersion);
    NiStreamSaveBinary(*pkFile, bUptodate);
    NiStreamSaveBinary(*pkFile, uiNumSurfaces);
    NiStreamSaveBinary(*pkFile, uiNumBlocks);

    // Now write the surface index map
    NiTMapIterator kIter = kSurfaceMapIndex.GetFirstPos();
    while (kIter)
    {
        const NiSurface* pkSurface;
        NiUInt16 usIndex;
        kSurfaceMapIndex.GetNext(kIter, pkSurface, usIndex);
        NiStreamSaveBinary(*pkFile, usIndex);
        NiFixedString::SaveFixedStringAsCString(*pkFile, 
            pkSurface->GetPackage()->GetName());
        NiFixedString::SaveFixedStringAsCString(*pkFile, 
            pkSurface->GetName());
    }

    for (NiUInt32 usLeafID = 0; usLeafID < uiNumBlocks; ++usLeafID)
    {
        if (!SaveBlockData(pkFile, kSurfaceMapIndex,
            pkSector->GetLeaf(pkSector->GetLeafOffset(uiNumLOD) + usLeafID)))
        {
            NiDelete pkFile;
            return false;
        }
    }

    NiDelete pkFile;
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainPhysXUtils::SaveBlockData(NiFile* pkFile, 
    const NiTMap<const NiSurface*, NiUInt16>& kSurfaceMapIndex,
    NiTerrainDataLeaf* pkQuadLeaf)
{
    if (!pkQuadLeaf)
        return false;
    
    NiTerrainPositionRandomAccessIterator kOriPositionIter;
    pkQuadLeaf->GetPositionIterator(kOriPositionIter);
    NiTStridedRandomAccessIterator<NiUInt16> kOriIndIter16;
    NiTStridedRandomAccessIterator<NiUInt32> kOriIndIter32;
    bool bUseIndex16 = false;
    NiTerrainSector* pkSector = pkQuadLeaf->GetContainingSector();
    
    if (pkSector->GetUsingShortIndexBuffer())
    {
        pkQuadLeaf->GetIndexIterator(kOriIndIter16);
        bUseIndex16 = true;
    }
    else
    {
        pkQuadLeaf->GetIndexIterator(kOriIndIter32);
    }
    
    NiUInt32 uiBlockSize = pkQuadLeaf->GetWidthInVerts();
    NiUInt32 uiBlockWidth = pkQuadLeaf->GetBlockSize();

    NiIndex kLeafMinIndex;   
    pkQuadLeaf->GetBottomLeftIndex(kLeafMinIndex);
    NiIndex kLeafMaxIndex(kLeafMinIndex.x + uiBlockSize,
        kLeafMinIndex.y + uiBlockSize);
    
    // Determine extents
    float fHeightMinZ = FLT_MAX;
    float fHeightMaxZ = -FLT_MAX;
    float fMaxX = -FLT_MAX;
    float fMinX = FLT_MAX;
    float fMaxY = -FLT_MAX;
    float fMinY = FLT_MAX;

    NiUInt32 uiMaxX = uiBlockSize;
    NiUInt32 uiMaxY = uiBlockSize;

    for (NiUInt32 y = 0; y < uiMaxY; ++y)
    {
        for (NiUInt32 x = 0; x < uiMaxX; ++x)
        {
            NiPoint3 kHiDetail;
            kOriPositionIter.GetHighDetail(y * uiBlockSize + x, kHiDetail);

            if (fMaxX < kHiDetail.x)
                fMaxX = kHiDetail.x;
            if (fMinX > kHiDetail.x)
                fMinX = kHiDetail.x;
            if (fMaxY < kHiDetail.y)
                fMaxY = kHiDetail.y;
            if (fMinY > kHiDetail.y)
                fMinY = kHiDetail.y;
            if (fHeightMinZ > kHiDetail.z)
                fHeightMinZ = kHiDetail.z;
            if (fHeightMaxZ < kHiDetail.z)
                fHeightMaxZ = kHiDetail.z;
        }
    }
      
    // The PhysX mesh data
    // Iterate over the vertices that will represent the height field
    // and retrieve the min z- and max z-value to be able to get the best 
    // resolution of the height data.     
    float fMiddleHeight = (fHeightMaxZ + fHeightMinZ) / 2.0f;
    float fMinMaxDistanceHalf = (fHeightMaxZ - fHeightMinZ) / 2.0f;
    float fThirtyTwoKb = 32767.0f;

    // Write block header
    NiFixedString::SaveFixedStringAsCString(*pkFile,
        pkQuadLeaf->GetQuadMesh()->GetName());
    NiTransform kTransform = pkSector->GetSectorData()->GetWorldTransform();        
    NiPoint3 kWorldCenter = kTransform * 
        NiPoint3(fMinX, fMinY, fMiddleHeight);
    kTransform.m_Translate = kWorldCenter;
    NiStreamSaveBinary(*pkFile, kTransform);
        
    float fHeightScale = 
        kTransform.m_fScale * fMinMaxDistanceHalf / fThirtyTwoKb;
    float fRowScale = 
        kTransform.m_fScale * (fMaxX - fMinX) / float(uiBlockWidth);
    float fColScale = 
        kTransform.m_fScale * (fMaxY - fMinY) / float(uiBlockWidth);
    NiStreamSaveBinary(*pkFile, fHeightScale);
    NiStreamSaveBinary(*pkFile, fRowScale);
    NiStreamSaveBinary(*pkFile, fColScale);
    NiUInt32 uiMatHighBits = 0;
    NiUInt32 uiHoleMat = 100;
    NiStreamSaveBinary(*pkFile, uiMatHighBits);
    NiStreamSaveBinary(*pkFile, uiHoleMat);
    
    float fThickness = 0.0f;
    NiStreamSaveBinary(*pkFile, uiBlockSize);
    NiStreamSaveBinary(*pkFile, uiBlockSize);
    NiStreamSaveBinary(*pkFile, fThickness);
    NiStreamSaveBinary(*pkFile, fThickness);
    NiStreamSaveBinary(*pkFile, fThickness);
        
    for (NiUInt32 y = 0; y < uiMaxY; ++y)
    {
        for (NiUInt32 x = 0; x < uiMaxX; ++x)
        {
            NiUInt32 uiIndex = y * uiBlockSize + x;
            
            NiPoint3 kHiDetail;
            kOriPositionIter.GetHighDetail(uiIndex, kHiDetail);
            float fZ = kHiDetail.z;
            
            // Calculate how this vertex z-value is relative the min and max 
            // z-values and scaled into the resolution of an NxI16.
            NiUInt16 usHeight = (NiUInt16)(fThirtyTwoKb * 
                (fZ - fMiddleHeight) / fMinMaxDistanceHalf);

            NiStreamSaveBinary(*pkFile, usHeight);
                        
            // Work out the sample tessalation
            // We don't deal with the border points as they would be outside 
            // the sample tile.
            bool bTessFlag = false;
            if (y != 32 && x != 32)
            {
                NiUInt32 uiIndexValue[3];
                NiUInt32 uiIndStreamIndex = (y * uiBlockWidth + x) * 6;
                if (bUseIndex16)
                {
                    uiIndexValue[0] = kOriIndIter16[uiIndStreamIndex];
                    uiIndexValue[1] = kOriIndIter16[uiIndStreamIndex + 1];
                    uiIndexValue[2] = kOriIndIter16[uiIndStreamIndex + 2];
                }
                else
                {
                    uiIndexValue[0] = kOriIndIter32[uiIndStreamIndex];
                    uiIndexValue[1] = kOriIndIter32[uiIndStreamIndex + 1];
                    uiIndexValue[2] = kOriIndIter32[uiIndStreamIndex + 2];
                }

                // now that we have the indexvalue for the tile points we can
                // work out the tesselation

                NiUInt32 uiCount = 0;
                while (uiIndexValue[0] + 1 != uiIndexValue[1] && uiCount < 3)
                {
                    NiUInt32 uiTemp = uiIndexValue[0];
                    uiIndexValue[0] = uiIndexValue[1];
                    uiIndexValue[1] = uiIndexValue[2];
                    uiIndexValue[2] = uiTemp;
                    uiCount++;
                }
                
                if (uiIndexValue[0] + 1 == uiIndexValue[1])
                {
                    if (uiIndexValue[0] + uiBlockSize == uiIndexValue[2])
                        bTessFlag = true;
                    else
                        bTessFlag = false;
                }
                else
                {
                    // This is a default case that should not be hit
                    bTessFlag = true;
                }                
                                
            }
            NiStreamSaveBinary(*pkFile, bTessFlag);

            NiUInt16 usMatIndex0, usMatIndex1; 
            GetDominantSurface(pkQuadLeaf, kSurfaceMapIndex, x, y,
                bTessFlag, usMatIndex0, usMatIndex1); 
            
            NiStreamSaveBinary(*pkFile, usMatIndex0); 
            NiStreamSaveBinary(*pkFile, usMatIndex1); 
            bool bUnused = false;

            NiStreamSaveBinary(*pkFile, bUnused);
                        
        }
    } 

    return true;
}
