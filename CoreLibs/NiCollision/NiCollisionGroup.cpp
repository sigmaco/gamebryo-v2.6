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
#include "NiCollisionPCH.h"

#include <NiAVObject.h>
#include "NiCollisionData.h"
#include "NiCollisionGroup.h"
#include "NiCollisionMetrics.h"
#include "NiCollisionTraversals.h"
#include "NiCollisionUtils.h"

//---------------------------------------------------------------------------
NiCollisionGroup::NiCollisionGroup() : m_kColliders(1, 1), m_kCollidees(1, 1)
{
}
//---------------------------------------------------------------------------
NiCollisionGroup::~NiCollisionGroup()
{
    unsigned int i;
    for (i = 0; i < m_kColliders.GetSize(); i++)
        NiDelete m_kColliders.GetAt(i);
    
    for (i = 0; i < m_kCollidees.GetSize(); i++)
        NiDelete m_kCollidees.GetAt(i);

    m_kColliders.RemoveAll();
    m_kCollidees.RemoveAll();
}
//---------------------------------------------------------------------------
void NiCollisionGroup::AddCollider(NiAVObject* pkObject, 
    bool bCreateCollisionData, int iMaxDepth, int iBinSize)
{
    Add(pkObject, m_kColliders, bCreateCollisionData, iMaxDepth, iBinSize);
}
//---------------------------------------------------------------------------
void NiCollisionGroup::AddCollidee(NiAVObject* pkObject, 
    bool bCreateCollisionData, int iMaxDepth, int iBinSize)
{
    Add(pkObject, m_kCollidees, bCreateCollisionData, iMaxDepth, iBinSize);
}
//---------------------------------------------------------------------------
void NiCollisionGroup::Add(NiAVObject* pkObject, NiRecordArray& kBin, 
    bool bCreateCollisionData, int iMaxDepth, int iBinSize)
{
    if (bCreateCollisionData)
        NiCollisionTraversals::CreateCollisionData(pkObject);

    Record* pkRecord = NiNew Record(pkObject, pkObject, iMaxDepth, iBinSize);
    NIASSERT(pkRecord);
    // push onto end of the array
    kBin.Add(pkRecord);
}
//---------------------------------------------------------------------------
void NiCollisionGroup::Remove(NiAVObject* pkObject)
{
    // Specific "Remove" function (collider, collidee) SHOULD be used instead
    unsigned int i;
    for (i = 0; i < m_kColliders.GetSize(); i++)
    {
        Record* pkRecord = m_kColliders.GetAt(i);
        if (pkRecord && pkRecord->GetAVObject() == pkObject)
        {
            m_kColliders.RemoveAtAndFill(i);
            NiDelete pkRecord;
            return;
        }
    }
    for (i = 0; i < m_kCollidees.GetSize(); i++)
    {
        Record* pkRecord = m_kCollidees.GetAt(i);
        if (pkRecord && pkRecord->GetAVObject() == pkObject)
        {
            m_kCollidees.RemoveAtAndFill(i);
            NiDelete pkRecord;
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionGroup::RemoveCollider(NiAVObject* pkObject)
{
    for (unsigned int i = 0; i < m_kColliders.GetSize(); i++)
    {
        Record* pkRecord = m_kColliders.GetAt(i);
        if (pkRecord && pkRecord->GetAVObject() == pkObject)
        {
            m_kColliders.RemoveAtAndFill(i);
            NiDelete pkRecord;
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionGroup::RemoveCollidee(NiAVObject* pkObject)
{
    for (unsigned int i = 0; i < m_kCollidees.GetSize(); i++)
    {
        Record* pkRecord = m_kCollidees.GetAt(i);
        if (pkRecord && pkRecord->GetAVObject() == pkObject)
        {
            m_kCollidees.RemoveAtAndFill(i);
            NiDelete pkRecord;
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionGroup::RemoveAll()
{
    unsigned int i;
    for (i = 0; i < m_kColliders.GetSize(); i++)
        NiDelete m_kColliders.RemoveAt(i);

    m_kColliders.RemoveAll();

    for (i = 0; i < m_kCollidees.GetSize(); i++)
        NiDelete m_kCollidees.RemoveAt(i);

    m_kCollidees.RemoveAll();
}
//---------------------------------------------------------------------------
void NiCollisionGroup::UpdateWorldData()
{
    // Standard BVs are automatically updated for rendering, and OBBs
    // are generated on the fly, so we only need to manually update the
    // collider ABVs, some of which may have been automatically generated
    for (unsigned int i = 0; i < m_kColliders.GetSize(); i++)
    {
        NiAVObject* pkObj = m_kColliders.GetAt(i)->GetAVObject();

        NiCollisionData* pkData = NiGetCollisionData(pkObj);

        if (pkData)
            pkData->UpdateWorldData();
    }
}
//---------------------------------------------------------------------------
bool NiCollisionGroup::TestCollisions(float fDeltaTime)
{
    // NOTE: User responsible for calling UpdateWorldABVs()

    NIMETRICS_COLLISION_SCOPETIMER(TEST_COLLISION_TIME);
    unsigned int uiMetricsComparisions = 0;

    // test only the colliders against everything else
    for (unsigned int i0 = 0; i0 < m_kColliders.GetSize(); i0++)
    {
        Record* pkRecord0 = m_kColliders.GetAt(i0);

        NiAVObject* pkObj0 = pkRecord0->GetAVObject();
        NiCollisionData* pkData0 = NiGetCollisionData(pkObj0);
                    
        if (pkData0 && pkData0->GetCollisionMode() == NiCollisionData::NOTEST
            && pkData0->GetPropagationMode() != 
            NiCollisionData::PROPAGATE_ALWAYS)
        {
            continue;
        }

        // test against other colliders
        for (unsigned int i1 = i0 + 1; i1 < m_kColliders.GetSize(); i1++)
        {
            Record* pkRecord1 = m_kColliders.GetAt(i1);
            NiAVObject* pkObj1 = pkRecord1->GetAVObject();
            NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);

            if (pkData1 && pkData1->GetCollisionMode() == 
                NiCollisionData::NOTEST && pkData1->GetPropagationMode() !=
                NiCollisionData::PROPAGATE_ALWAYS)
            {
                continue;
            }
            
            uiMetricsComparisions++;
            if (NiCollisionTraversals::TestCollisions(fDeltaTime, *pkRecord0,
                *pkRecord1))
            {
                NIMETRICS_COLLISION_ADDVALUE(TEST_COMPARISIONS, 
                    uiMetricsComparisions);
                return true;
            }
        }
        // test against all collidees
        for (unsigned int i2 = 0; i2 < m_kCollidees.GetSize(); i2++)
        {
            Record* pkRecord1 = m_kCollidees.GetAt(i2);
            NiAVObject* pkObj1 = pkRecord1->GetAVObject();
            NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);

            if (pkData1 && pkData1->GetCollisionMode() == 
                NiCollisionData::NOTEST && pkData1->GetPropagationMode() !=
                NiCollisionData::PROPAGATE_ALWAYS)
            {
                continue;
            }
            
            uiMetricsComparisions++;
            if (NiCollisionTraversals::TestCollisions(fDeltaTime, 
                *pkRecord0, *pkRecord1))
            {
                NIMETRICS_COLLISION_ADDVALUE(TEST_COMPARISIONS, 
                    uiMetricsComparisions);
                return true;
            }
        }
    }

    NIMETRICS_COLLISION_ADDVALUE(TEST_COMPARISIONS, 
        uiMetricsComparisions);
    return false;
}
//---------------------------------------------------------------------------
void NiCollisionGroup::FindCollisions(float fDeltaTime)
{
    // NOTE: User responsible for calling UpdateWorldABVs()

    NIMETRICS_COLLISION_SCOPETIMER(FIND_COLLISION_TIME);
    unsigned int uiMetricsComparisions = 0;

    // test only the colliders against all other objects
    for (unsigned int i0 = 0; i0 < m_kColliders.GetSize(); i0++)
    {
        Record* pkRecord0 = m_kColliders.GetAt(i0);
        NiAVObject* pkObj0 = pkRecord0->GetAVObject();
        NiCollisionData* pkData0 = NiGetCollisionData(pkObj0);

        if (pkData0 && pkData0->GetCollisionMode() == NiCollisionData::NOTEST
            && pkData0->GetPropagationMode() != 
            NiCollisionData::PROPAGATE_ALWAYS)
        {
            continue;
        }
               
        // test against other colliders
        for (unsigned int i1 = i0+1; i1 < m_kColliders.GetSize(); i1++)
        {
            Record* pkRecord1 = m_kColliders.GetAt(i1);
            NiAVObject* pkObj1 = pkRecord1->GetAVObject();
            NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);

            if (pkData1 && pkData1->GetCollisionMode() == 
                NiCollisionData::NOTEST && pkData1->GetPropagationMode() !=
                NiCollisionData::PROPAGATE_ALWAYS)
            {
                continue;
            }

            uiMetricsComparisions++;
            NiCollisionTraversals::FindCollisions(fDeltaTime, *pkRecord0,
                *pkRecord1);
        }

        // test against all collidees
        for (unsigned int i2 = 0; i2 < m_kCollidees.GetSize(); i2++)
        {
            Record* pkRecord1 = m_kCollidees.GetAt(i2);
            NiAVObject* pkObj1 = pkRecord1->GetAVObject();
            NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);

            if (pkData1 && pkData1->GetCollisionMode() == 
                NiCollisionData::NOTEST && pkData1->GetPropagationMode() !=
                NiCollisionData::PROPAGATE_ALWAYS)
            {
                continue;
            }
            
            uiMetricsComparisions++;
            NiCollisionTraversals::FindCollisions(fDeltaTime, *pkRecord0,
                *pkRecord1);
        }
    }

    NIMETRICS_COLLISION_ADDVALUE(FIND_COMPARISIONS, uiMetricsComparisions)
}
//---------------------------------------------------------------------------
bool NiCollisionGroup::IsCollidee(const NiAVObject* pkObject) const
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kCollidees.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (m_kCollidees.GetAt(uiIdx)->GetAVObject() == pkObject)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiCollisionGroup::IsCollider(const NiAVObject* pkObject) const
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kColliders.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (m_kColliders.GetAt(uiIdx)->GetAVObject() == pkObject)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
