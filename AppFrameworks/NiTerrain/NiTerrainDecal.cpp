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
#include "NiTerrainDecal.h"

#include <NiMath.h>

//---------------------------------------------------------------------------
NiTerrainDecal::NiTerrainDecal() :
    m_spMesh(NULL),
    m_spTexture(NULL),
    m_kPosition(0.0f, 0.0f, 0.0f),
    m_fTimer(0.0f),
    m_fDecayTime(-1.0f),
    m_fTimedAlpha(1.0f),
    m_fDistanceAlpha(1.0f),
    m_fFadingDistance(-1.0f),
    m_fDepthBiasOffset(0.01f),
    m_ucNumberOfBlocksCovered(0),
    m_ucNumberOfHBlocks(0),
    m_uiSize(0),
    m_uiNumberOfVertices(0),
    m_uiNumberOfIndices(0),
    m_fRatio(1.0f),
    m_bRequiresUpdate(false),
    m_bUseTimer(false)
{
}
//---------------------------------------------------------------------------
NiTerrainDecal::~NiTerrainDecal()
{
    this->DetachChild(m_spMesh);
    m_spMesh = NULL;
    m_spTexture = NULL;
}
//---------------------------------------------------------------------------
void NiTerrainDecal::CreateDecal(NiTexture* pkTexture, NiPoint3 kPosition, 
    NiTerrainSector* pkSector, NiUInt32 uiSize, float fRatio, 
    float fTimeOfDeath, float fDecayTime, float fDepthBiasOffset)
{
    m_fTimer = fTimeOfDeath;
    m_fRatio = fRatio;
    m_uiSize = uiSize;
    m_kPosition = kPosition;
    m_spTexture = pkTexture;
    m_fDecayTime = fDecayTime;
    m_fDepthBiasOffset = fDepthBiasOffset;

    NiTransform kTerrainTransform = pkSector->GetSectorData()
        ->GetWorldTransform();
    
    NiPoint3 kModelPosition = kTerrainTransform.m_Rotate.Inverse() * (
        (kPosition - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale);

    if (m_fTimer > 0.0f)
        m_bUseTimer = true;
    
    // First we need to work out the required size of the streams
    NiUInt32 uiSectorSize = pkSector->GetSectorData()->GetSectorSize();

    // Workout the min and max of the decal
    NiPoint3 kLowerCorner = NiPoint3(
        NiFloor(kModelPosition.x - uiSize * 0.5f),
        NiFloor(kModelPosition.y - uiSize * 0.5f),
        0.0f);
    NiPoint3 kUpperCorner = NiPoint3(
        ceil(kModelPosition.x + uiSize * 0.5f),
        ceil(kModelPosition.y + uiSize * 0.5f),
        0.0f);

    float fSizeX = kUpperCorner.x - kLowerCorner.x;
    float fSizeY = kUpperCorner.y - kLowerCorner.y;
        
    NiPoint3 kSectorLowerCorner;
    pkSector->GetVertexAt(kSectorLowerCorner, NiIndex::ZERO, 
        pkSector->GetSectorData()->GetNumLOD());
    NiPoint3 kSectorUpperCorner;
    pkSector->GetVertexAt(kSectorUpperCorner, 
		NiIndex(uiSectorSize, uiSectorSize), 
		pkSector->GetSectorData()->GetNumLOD());

    // Work out the corresponding indices
    NiUInt32 uiMinX = (NiUInt32)NiClamp((NiInt32)(((kLowerCorner.x - 
        kSectorLowerCorner.x) * 
        uiSectorSize / (kSectorUpperCorner.x - 
        kSectorLowerCorner.x)) + 0.5f),
        0, uiSectorSize);
    NiUInt32 uiMinY = (NiUInt32)NiClamp((NiInt32)(((kLowerCorner.y - 
        kSectorLowerCorner.y) * 
        uiSectorSize / (kSectorUpperCorner.y - 
        kSectorLowerCorner.y)) + 0.5f),
        0, uiSectorSize);
    
    NiUInt32 uiMaxX = (NiUInt32)NiClamp((NiInt32)(((kUpperCorner.x - 
        kSectorLowerCorner.x) * 
        uiSectorSize / (kSectorUpperCorner.x - 
        kSectorLowerCorner.x)) + 0.5f),
        0, uiSectorSize);
    NiUInt32 uiMaxY = (NiUInt32)NiClamp((NiInt32)(((kUpperCorner.y - 
        kSectorLowerCorner.y) * 
        uiSectorSize / (kSectorUpperCorner.y - 
        kSectorLowerCorner.y)) + 0.5f),
        0, uiSectorSize);

    // work out the kLeaves to check for regions
    NiUInt32 uiBlockSize =  pkSector->GetSectorData()->GetBlockSize();

    NiTPrimitiveArray<NiTerrainDataLeaf*> kLeaves;
    NiIndex kLeafIndex;
    NiUInt32 uiBlocksPerRow = 1 << pkSector->GetSectorData()->GetNumLOD();
    
    for (NiUInt32 uiY = uiMinY - uiMinY % uiBlockSize; 
        uiY < uiMaxY; 
        uiY += uiBlockSize)
    {
        for (NiUInt32 uiX = uiMinX - uiMinX % uiBlockSize; 
            uiX < uiMaxX; 
            uiX += uiBlockSize)
        {
            NiTerrainDataLeaf* pkToAdd = pkSector->GetLeaf(
				pkSector->GetLeafOffset(pkSector->GetSectorData()->GetNumLOD()) +
                uiX / uiBlockSize + 
                (uiY / uiBlockSize) * uiBlocksPerRow);
            
            if (pkToAdd)
                kLeaves.Add(pkToAdd);
        }
    }

    m_uiNumberOfVertices = 0;

    // Calculate number of vertices.
    NiIndex kMinimumIndex(uiMinX, uiMinY);
    NiIndex kMaximumIndex(uiMaxX, uiMaxY);
    NiTerrainDataLeaf* pkLeaf = NULL;
    m_uiNumberOfIndices = 0;

    for (NiUInt32 uiLeafNum = 0; uiLeafNum < kLeaves.GetSize(); ++uiLeafNum)
    {
        pkLeaf = kLeaves[uiLeafNum];
                
        NiIndex kThisIndex;
        NIASSERT(pkLeaf);
        pkLeaf->GetBottomLeftIndex(kThisIndex);

        NiIndex kThisMin(
            (NiUInt32)NiMax((int)kThisIndex.x, (int)kMinimumIndex.x),
            (NiUInt32)NiMax((int)kThisIndex.y, (int)kMinimumIndex.y));
        NiIndex kThisMax(
            (NiUInt32)NiMin((int)(kThisIndex.x + uiBlockSize),
            (int)kMaximumIndex.x),
            (NiUInt32)NiMin((int)(kThisIndex.y + uiBlockSize),
            (int)kMaximumIndex.y));
        
        m_uiNumberOfVertices += (1 + kThisMax.x - kThisMin.x) * 
            (1 + kThisMax.y - kThisMin.y);
    }    
    
    m_uiNumberOfIndices = NiUInt32(fSizeX * fSizeY) * 6;

    // We can now create the streams and get the iterators:
    CreateStreams();

     // Create the iterators
    NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = 
        m_kPositionLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint2> kUVIter = 
        m_kUVLock.begin<NiPoint2>();
    NiTStridedRandomAccessIterator<NiUInt16> kIndexIter = 
        m_kIndexLock.begin<NiUInt16>();

    NiUInt32 uiAddedInd = 0;
    NiUInt32 uiLeafVertexOffset = 0;

    // Work out the values for each affected kLeaves
    for (NiUInt32 i = 0; i < kLeaves.GetSize(); i++)
    {
        pkLeaf = kLeaves[i];

        NiIndex kLeafMinIndex, leafMaxIndex;
        NiIndex kMinIndex(uiMinX, uiMinY);
        NiIndex kMaxIndex(uiMaxX, uiMaxY);
        NIASSERT(pkLeaf);
        pkLeaf->GetBottomLeftIndex(kLeafMinIndex);
        
        if (kMinIndex.x < kLeafMinIndex.x)
            kMinIndex.x = kLeafMinIndex.x;
        if (kMinIndex.y < kLeafMinIndex.y)
            kMinIndex.y = kLeafMinIndex.y;
        if (kMaxIndex.x > kLeafMinIndex.x + uiBlockSize)
            kMaxIndex.x = kLeafMinIndex.x + uiBlockSize;
        if (kMaxIndex.y > kLeafMinIndex.y + uiBlockSize)
            kMaxIndex.y = kLeafMinIndex.y + uiBlockSize;

        NiIndex kLocalMinIndex;
        NiIndex kLocalMaxIndex;

        pkLeaf->IndexToLocal(kMinIndex, kLocalMinIndex);
        pkLeaf->IndexToLocal(kMaxIndex, kLocalMaxIndex);

        NiTerrainPositionRandomAccessIterator kOriPositionIter;
        NiTStridedRandomAccessIterator<NiPoint2> kOriUVIter;
        NiTStridedRandomAccessIterator<NiUInt16> kOriIndIter16;
        NiTStridedRandomAccessIterator<NiUInt32> kOriIndIter32;

        pkLeaf->GetPositionIterator(kOriPositionIter);
        pkLeaf->GetUVIterator(kOriUVIter);
        if (pkSector->GetUsingShortIndexBuffer())
            pkLeaf->GetIndexIterator(kOriIndIter16);
        else
            pkLeaf->GetIndexIterator(kOriIndIter32);

        NiUInt32 uiIndexSize = pkSector->GetSectorData()->GetBlockSize() *
            pkSector->GetSectorData()->GetBlockSize() * 6;

        NiUInt32 uiHalfSectorSize = 
            pkSector->GetSectorData()->GetSectorSize() >> 1;
        NiUInt32 uiRowSize = 1 + kLocalMaxIndex.x - kLocalMinIndex.x;

        // First copy the indices
        for (NiUInt32 uiIndex = 0; uiIndex < uiIndexSize; uiIndex += 3)
        {
            for (NiUInt8 ucTriPoint = 0; ucTriPoint < 3; ++ucTriPoint)
            {
                NiUInt32 uiIndexValue;
                if (pkSector->GetUsingShortIndexBuffer())
                    uiIndexValue = kOriIndIter16[uiIndex + ucTriPoint];
                else
                    uiIndexValue = kOriIndIter32[uiIndex + ucTriPoint];

                NiPoint3 kPoint;
                kOriPositionIter.GetHighDetail(uiIndexValue, kPoint);

                // Since the values are in model space, we can directly 
                // translate back to an index.
                NiInt32 iX = static_cast<NiInt32>(kPoint.x) + uiHalfSectorSize;
                NiInt32 iY = static_cast<NiInt32>(kPoint.y) + uiHalfSectorSize;
                NIASSERT(iX >= 0);
                NIASSERT(iY >= 0);
                NiIndex kPointIndex((NiUInt32)iX, (NiUInt32)iY);

                // Ensure the point inside our clipping region on this leaf
                if (kPointIndex.x < kMinIndex.x ||
                    kPointIndex.y < kMinIndex.y ||
                    kPointIndex.x > kMaxIndex.x ||
                    kPointIndex.y > kMaxIndex.y)
                {
                    // Undo any indexes we wrote
                    uiAddedInd -= ucTriPoint;
                    break;
                }                

                NiUInt32 uiVertexPosition = 
                    (kPointIndex.x - kMinIndex.x) + 
                    (kPointIndex.y - kMinIndex.y) * uiRowSize;

                NIASSERT(uiVertexPosition < 
                    NiUInt32(1 + kMaxIndex.y - kMinIndex.y) * uiRowSize);
                
                uiVertexPosition += uiLeafVertexOffset;

                NIASSERT(uiVertexPosition < m_uiNumberOfVertices);
                NIASSERT(uiAddedInd < m_uiNumberOfIndices);

                NIASSERT(uiVertexPosition < USHRT_MAX);
                kIndexIter[uiAddedInd++] = (NiUInt16)uiVertexPosition;

                // We have completed all triangles, may as well quit
                if (uiAddedInd == m_uiNumberOfIndices)
                {
                    // We need to break out of both loops
                    uiIndex = uiIndexSize;
                    break;
                }
            }
        }
        
        // Now populate the position and UVs
        NiIndex kVertexLocalIndex(
            kMinIndex.x - kLeafMinIndex.x,
            kMinIndex.y - kLeafMinIndex.y);

        NiUInt32 uiOriIter = 0;
        NiUInt32 uiNewIter = uiLeafVertexOffset;

        for (NiUInt32 y = kMinIndex.y; y <= kMaxIndex.y; ++y)
        {
            kVertexLocalIndex.x = kMinIndex.x - kLeafMinIndex.x ;
            for (NiUInt32 x = kMinIndex.x; x <= kMaxIndex.x; ++x)
            {
                uiOriIter = kVertexLocalIndex.x + kVertexLocalIndex.y * 
                    pkSector->GetSectorData()->GetBlockWidthInVerts();

                NiIndex kSectorIndex = NiIndex(x,y);

                float u = fRatio * (1.0f - 
                        float(uiMaxX - kSectorIndex.x) / 
                        float(uiMaxX - uiMinX)) - (fRatio * 0.5f - 0.5f);
                float v = fRatio * (1.0f - 
                        float(uiMaxY - kSectorIndex.y) / 
                        float(uiMaxY - uiMinY)) - (fRatio * 0.5f - 0.5f);

                NiPoint3 kOriPos; 
                kOriPositionIter.GetHighDetail(uiOriIter, kOriPos);
                kPositionIter[uiNewIter] = kOriPos - kModelPosition;
                kUVIter[uiNewIter] = NiPoint2(u,v);

                ++uiNewIter;
                ++kVertexLocalIndex.x;
            }
            ++kVertexLocalIndex.y;
        }
        // We add the decal to the block
        pkLeaf->GetQuadMesh()->AddDecal(this);
        uiLeafVertexOffset += (1 + kMaxIndex.y - kMinIndex.y) * uiRowSize;
    }

    m_kPositionLock.Unlock();
    m_kIndexLock.Unlock();
    m_kUVLock.Unlock();

    // Setup the mesh properties
    CreateProperties();

    m_spMesh->SetTranslate(NiPoint3::ZERO);
    
    this->AttachChild(m_spMesh);
    this->SetTranslate(kPosition);
    this->SetRotate(kTerrainTransform.m_Rotate);
    this->SetScale(kTerrainTransform.m_fScale);
    
    this->Update(0.0f);
}
//---------------------------------------------------------------------------
NiUInt8 NiTerrainDecal::UpdateDecal(NiTerrainSector* pkSector, 
    float fAccumTime)
{
    if (m_bUseTimer)
    {
        if (m_fTimer <= fAccumTime)
            return 1;

        if (m_fDecayTime <= fAccumTime)
        {
            float fTimeTillDeath = m_fTimer - fAccumTime;
            if (fTimeTillDeath > 0)
            {
                m_fTimedAlpha = fTimeTillDeath / (m_fTimer - m_fDecayTime);
            }
            
        }
    }   

    m_spMesh->Update(fAccumTime);
    
    if (!m_bRequiresUpdate)
        return 0;

    m_bRequiresUpdate = false;
    m_spMesh = NULL;

    CreateDecal(m_spTexture, m_kPosition, pkSector, m_uiSize, m_fRatio, 
        m_fTimer, m_fDecayTime);

    return 2;
}
//---------------------------------------------------------------------------
void NiTerrainDecal::SetRequiresUpdate()
{
    m_bRequiresUpdate = true;
}
//---------------------------------------------------------------------------
void NiTerrainDecal::SetDecayStartTime(float fDecayTime)
{
    m_fDecayTime = fDecayTime;
}
//---------------------------------------------------------------------------
void NiTerrainDecal::SetFadingDistance(float fMaxDistance)
{
    m_fFadingDistance = fMaxDistance;
}
//---------------------------------------------------------------------------
NiBool NiTerrainDecal::GetRequiresUpdate()
{
    return m_bRequiresUpdate;
}
//---------------------------------------------------------------------------
NiMeshPtr NiTerrainDecal::GetMesh()
{
    return m_spMesh;
}
//---------------------------------------------------------------------------
void NiTerrainDecal::CalculateAlphaDegeneration(NiPoint3 kCamPos)
{
    if (m_fFadingDistance < 0)
        return;

    float fDistanceFromDecal = (kCamPos - m_kPosition).Length();
    float fDifference = fDistanceFromDecal - m_fFadingDistance * 0.5f;    

    if (fDifference <= m_fFadingDistance * 0.5f && fDistanceFromDecal > 
        m_fFadingDistance/2)
    {
        m_fDistanceAlpha = 1.0f - fDifference / (m_fFadingDistance * 0.5f);
    }
    else if (fDifference > m_fFadingDistance * 0.5f)
    {
        m_fDistanceAlpha = 0.0f;
    }  

    NiMaterialProperty* pkMaterial = 
        NiDynamicCast(NiMaterialProperty, 
        m_spMesh->GetProperty(NiMaterialProperty::GetType()));
    NIASSERT(pkMaterial);

    pkMaterial->SetAlpha(m_fDistanceAlpha * m_fTimedAlpha);
    m_spMesh->UpdateProperties();

}
//---------------------------------------------------------------------------
void NiTerrainDecal::UpdateZFightingOffset(const NiCamera* pkCamera)
{
    NiPoint3 kDirection = pkCamera->GetWorldLocation() - this->GetTranslate();
    
    float fLength = kDirection.Length();
    float fRatio = (fLength - this->GetWorldBound().GetRadius());
   
    float fScale = NiClamp(this->GetScale(), 1.0f, 100.0f);    
    if (fRatio <= 0)
    {
        kDirection = NiPoint3::UNIT_Z;
    }
    else
    {
        // We divide the ratio by the scale to give sensible step in the 
        // position offset
        kDirection.z = NiClamp(fRatio / fScale, 1.0f, 10.0f);
        kDirection.x = 0.0f;
        kDirection.y = 0.0f;    
    }   

    m_spMesh->SetTranslate(kDirection * m_fDepthBiasOffset);
    this->Update(0.0f);
}
//---------------------------------------------------------------------------
void NiTerrainDecal::CreateStreams()
{
    m_spMesh = NiNew NiMesh();
    m_spMesh->SetName("NiTerrainDecal");
    m_spMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
    
    m_spMesh->SetSubmeshCount(1);
    NiBound kModelBound;
    kModelBound.SetCenterAndRadius(NiPoint3(0,0,0), (float)(m_uiSize) / 2.0f);
    m_spMesh->SetModelBound(kModelBound);
    
    // Create the streams
    // position stream
    m_kPositionLock = m_spMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, 
        m_uiNumberOfVertices,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, false, true);

    // Texture stream
    m_kUVLock = m_spMesh->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 0,
        NiDataStreamElement::F_FLOAT32_2, 
        m_uiNumberOfVertices,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, false, true);

    // Index stream
    NIASSERT(m_uiNumberOfVertices < USHRT_MAX);
    m_kIndexLock = m_spMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, 
        m_uiNumberOfIndices,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX, false, true);
}
//---------------------------------------------------------------------------
void NiTerrainDecal::CreateProperties()
{
    NIASSERT(m_spMesh);
    NIASSERT(m_spTexture);
    // Create the texturing property
    NiTexturingPropertyPtr spTextProp = NiNew NiTexturingProperty();
    spTextProp->SetBaseTexture(m_spTexture);
    spTextProp->SetBaseFilterMode(NiTexturingProperty::FILTER_BILERP);
    spTextProp->SetApplyMode(NiTexturingProperty::APPLY_DECAL);
    spTextProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    // We then add an alpha property
    // create the alpha property
    NiAlphaPropertyPtr spAlpha = NiNew NiAlphaProperty();
    spAlpha->SetAlphaBlending(true);
    spAlpha->SetNoSorter(false);
    spAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    spAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    
    NiMaterialPropertyPtr spMatProp = NiNew NiMaterialProperty();
    // Alpha is ignored, will need to make a modifier
    spMatProp->SetAlpha(m_fTimedAlpha * m_fDistanceAlpha);
    spMatProp->SetAmbientColor(NiColor(1.0f, 1.0f, 1.0f));
    spMatProp->SetDiffuseColor(NiColor(1.0f, 1.0f, 1.0f));
    spMatProp->SetSpecularColor(NiColor(1.0f, 1.0f, 1.0f));
    spMatProp->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
    spMatProp->SetShineness(10);
    
    // A Z buffer property to determine the order of drawing
    // create z buffer property
    NiZBufferPropertyPtr spZBuffer = NiNew NiZBufferProperty();
    spZBuffer->SetZBufferTest(true);
    spZBuffer->SetZBufferWrite(false);
    spZBuffer->SetTestFunction(NiZBufferProperty::TEST_LESSEQUAL);

    // Finally a color property
    NiVertexColorProperty *pkVertexColorProperty = 
        NiNew NiVertexColorProperty();
    pkVertexColorProperty->SetSourceMode
        (NiVertexColorProperty::SOURCE_IGNORE);
    pkVertexColorProperty->SetLightingMode
        (NiVertexColorProperty::LIGHTING_E);

    // Attach the properties
    m_spMesh->AttachProperty(spTextProp);
    m_spMesh->AttachProperty(pkVertexColorProperty);
    m_spMesh->AttachProperty(spZBuffer);
    m_spMesh->AttachProperty(spAlpha);
    m_spMesh->AttachProperty(spMatProp);
}
