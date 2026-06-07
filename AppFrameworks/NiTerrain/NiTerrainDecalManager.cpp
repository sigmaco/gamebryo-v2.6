// EPIPHANY GAMES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a non-disclosure 
// agreement with Epiphany Games and may not be copied or disclosed
// except in accordance with the terms of that agreement.
//
//      Copyright (c) 2005-2007 Epiphany Games.
//      All Rights Reserved.
//
// http://www.epiphanygames.com.au
//
// $Id: NiTerrainDecalManager.cpp 828 2008-01-21 07:06:29Z lewis $

#include "NiTerrainPCH.h"
#include "NiTerrainDecal.h"
#include "NiTerrainDecalManager.h"

//---------------------------------------------------------------------------
NiTerrainDecalManager::NiTerrainDecalManager()
{
}
//---------------------------------------------------------------------------
NiTerrainDecalManager::~NiTerrainDecalManager()
{
   RemoveAll();
}
//---------------------------------------------------------------------------
void NiTerrainDecalManager::AddDecal(NiTerrainDecal* pkDecal)
{
    // This code stores the decal in a list, and the list iterator pointing
    // to pkDecal in a map, keyed on pkDecal, for fast deletion operations
    pkDecal->IncRefCount();

    m_kDecalList.AddTail(pkDecal);
    m_kDecalMap.SetAt(pkDecal, m_kDecalList.GetTailPos());
}
//---------------------------------------------------------------------------
void NiTerrainDecalManager::RemoveDecal(NiTerrainDecal* pkDecal)
{
    // Get the list iterator corresponding to pkDecal
    NiTListIterator kIter = NULL;
    if (m_kDecalMap.GetAt(pkDecal, kIter))
    {
        // Remove the decal from both the list and the map
        NIVERIFY(kIter != NULL);
        m_kDecalList.RemovePos(kIter);
        m_kDecalMap.RemoveAt(pkDecal);
    }

    pkDecal->DecRefCount();
}
//---------------------------------------------------------------------------
void NiTerrainDecalManager::RemoveAll()
{
    NiTListIterator kIter;

    kIter = m_kDecalList.GetHeadPos();

    while (kIter)
    {
        // Since we increment the ref count of the decals, we need to remove 
        // all decals manually and decrement the ref count of each decal.
        RemoveDecal(m_kDecalList.GetHead());

        kIter = m_kDecalList.GetHeadPos();
    }

    // Making sure all items have been removed
    m_kDecalMap.RemoveAll();
    m_kDecalList.RemoveAll();
}
//---------------------------------------------------------------------------
void NiTerrainDecalManager::UpdateDecals(NiTerrainSector* pkSector, 
    float fDeltaTime)
{
    NiTListIterator kIter;

    kIter = m_kDecalList.GetHeadPos();

    while (kIter)
    {
        NiTerrainDecal* pkDecal = m_kDecalList.GetNext(kIter);
        NiUInt8 ucRet = pkDecal->UpdateDecal(pkSector, fDeltaTime);

        // The timer has come to an end, we do not want to display this decal 
        // again it should therefore be destroyed
        if (ucRet == 1)
            RemoveDecal(pkDecal);
    }
}
//---------------------------------------------------------------------------
void NiTerrainDecalManager::Cull(NiCullingProcess &kCuller)
{
    NiTListIterator kIter;

    kIter = m_kDecalList.GetHeadPos();

    while (kIter)
    {
        NiTerrainDecal* pkDecal = m_kDecalList.GetNext(kIter);

        const NiCamera *pkCamera = kCuller.GetCamera();

        NiPoint3 kCamPos = pkCamera->GetWorldLocation();
        pkDecal->CalculateAlphaDegeneration(kCamPos);
        pkDecal->UpdateZFightingOffset(pkCamera);

        const NiBound kBound = pkDecal->GetMesh()->GetWorldBound();
        int iWhichSide = 0;

        NiFrustumPlanes kFrustumPlanes;
        kFrustumPlanes.Set(*pkCamera);
        NiPlane kPlane;

        bool bIsVisible = true;
        for (int i = 0; i < NiFrustumPlanes::MAX_PLANES; ++i)
        {
            // find the distance to this plane 
            kPlane = kFrustumPlanes.GetPlane(i);
            iWhichSide = kPlane.WhichSide(
                kBound.GetCenter() + (kPlane.GetNormal() * 
                kBound.GetRadius())
                );

            if (iWhichSide == NiPlane::NEGATIVE_SIDE) 
            {
                bIsVisible = false;
                break;
            }
        }

        if (bIsVisible)
        {
            kCuller.GetVisibleSet()->Add(*(pkDecal->GetMesh()));
        }
    }
}
