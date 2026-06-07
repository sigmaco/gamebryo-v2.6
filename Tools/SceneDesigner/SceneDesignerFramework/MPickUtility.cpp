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
#include "SceneDesignerFrameworkPCH.h"

#include "MPickUtility.h"
#include "MEntity.h"
#include "MRenderer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPickUtility::MPickUtility() : m_pkPick(NULL), m_pkSceneRootPointerName(NULL)
{
    m_pkPick = NiNew NiPick();
    m_pkPick->SetReturnNormal(true);
    m_pkSceneRootPointerName = NiNew NiFixedString("Scene Root Pointer");
    m_pmPickedObjectToEntity = new Hashtable();
    m_pmRegisteredPolicies = new ArrayList();
    m_pmDefaultPolicy = new MPickPolicy();
}
//---------------------------------------------------------------------------
void MPickUtility::Do_Dispose(bool)
{
    NiDelete m_pkSceneRootPointerName;
    NiDelete m_pkPick;
}
//---------------------------------------------------------------------------
bool MPickUtility::PerformPick(MScene* pmScene, const NiPoint3& kOrigin,
    const NiPoint3& kDir, bool bIncludeFrozenEntities)
{
    MVerifyValidInstance;

    MAssert(pmScene != NULL, "Null scene provided to function!");

    m_pmPickedObjectToEntity->Clear();
    m_pkPick->ClearResultsArray();

    bool bFoundIntersection = false;
    MEntity* amEntities[] = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];

        // Do not pick on hidden entities. Only pick on frozen entities if
        // requested.
        if (!pmEntity->Hidden &&
            (bIncludeFrozenEntities || !pmEntity->Frozen))
        {
            MPickPolicy* pmPolicy = FindPolicy(pmEntity);
            bFoundIntersection = pmPolicy->PickOnEntity(pmEntity, kOrigin,
                kDir, m_pkPick, m_pmPickedObjectToEntity) || 
                bFoundIntersection;
        }
    }
    // Finally, sort the results by distance
    m_pkPick->GetResults().SortResults();

    return bFoundIntersection;
}
//---------------------------------------------------------------------------
const NiPick* MPickUtility::GetNiPick()
{
    MVerifyValidInstance;

    return m_pkPick;
}
//---------------------------------------------------------------------------
MEntity* MPickUtility::GetEntityFromPickedObject(
    NiAVObject* pkPickedObject)
{
    MVerifyValidInstance;

    return dynamic_cast<MEntity*>(m_pmPickedObjectToEntity->Item[
        __box((unsigned int) PtrToUint(pkPickedObject))]);
}
//---------------------------------------------------------------------------
MEntity* MPickUtility::GetEntityFromViewCoordinates(MScene* pmScene,
    MViewport* pmViewport, int iX, int iY)
{
        NiPoint3 kOrigin, kDir;
        NiViewMath::MouseToRay((float) iX, (float) iY, 
            pmViewport->Width, pmViewport->Height,
            pmViewport->GetNiCamera(), kOrigin, kDir);

        PerformPick(pmScene, kOrigin, kDir, false);

        const NiPick* pkPick = GetNiPick();

        const NiPick::Results& kPickResults = pkPick->GetResults();

        if (kPickResults.GetSize() > 0)
        {        
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                return GetEntityFromPickedObject(pkPickedObject);
            }
        }

        return NULL;

}
//---------------------------------------------------------------------------
MPickUtility::MPickPolicy* MPickUtility::FindPolicy(MEntity* pmEntity)
{
    // if the entity contains one of the registered exception components,
    // use the associated picking policy
    for (int i = 0; i < m_pmRegisteredPolicies->Count; i++)
    {
        MPickPolicy* pmCurrentPolicy = 
            static_cast<MPickPolicy*>(m_pmRegisteredPolicies->Item[i]);
        Guid mTemplateID = pmCurrentPolicy->mTemplateID;
        if (pmEntity->GetComponentByTemplateID(mTemplateID) != NULL)
        {
            return pmCurrentPolicy;
        }
    }
    return m_pmDefaultPolicy;
}
//---------------------------------------------------------------------------
void MPickUtility::AddPolicy(MPickUtility::MPickPolicy* pmPolicy)
{
    // replace the existing policy for this templateID if it exists
    bool bFound = false;
    for (int i = 0; i < m_pmRegisteredPolicies->Count; i++)
    {
        MPickPolicy* pmRegisteredPolicy = 
            static_cast<MPickPolicy*>(m_pmRegisteredPolicies->Item[i]);
        if ((pmPolicy != NULL) && (pmPolicy != NULL) &&
            (pmRegisteredPolicy->mTemplateID == pmPolicy->mTemplateID))
        {
            m_pmRegisteredPolicies->Item[i] = pmPolicy;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        m_pmRegisteredPolicies->Add(pmPolicy);
    }
}
//---------------------------------------------------------------------------
void MPickUtility::RemovePolicy(MPickUtility::MPickPolicy* pmPolicy)
{
    for (int i = 0; i < m_pmRegisteredPolicies->Count; i++)
    {
        MPickPolicy* pmRegisteredPolicy = 
            static_cast<MPickPolicy*>(m_pmRegisteredPolicies->Item[i]);
        if ((pmPolicy != NULL) && (pmPolicy != NULL) && 
            (pmPolicy == pmRegisteredPolicy))
        {
            m_pmRegisteredPolicies->RemoveAt(i);
            break;
        }
    }
}
//---------------------------------------------------------------------------
bool MPickUtility::MPickPolicy::PickOnEntity(MEntity* pmEntity, 
    const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick* pkPick, 
    Hashtable* pmPickedObjectToEntity)
{
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    bool bFoundIntersection = false;

    unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
    for (unsigned int uiSceneRoot = 0; uiSceneRoot < uiSceneRootCount;
        uiSceneRoot++)
    {
        NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(
            uiSceneRoot);
        if (pkSceneRoot)
        {
            pkPick->SetTarget(pkSceneRoot);
            bFoundIntersection = pkPick->PickObjects(
                kOrigin, kDir, true) || bFoundIntersection;

            const NiPick::Results& kResults = pkPick->GetResults();
            for (unsigned int ui = 0; ui < kResults.GetSize(); ui++)
            {
                NiPick::Record* pkRecord = kResults.GetAt(ui);
                if (pkRecord)
                {
                    Object* pmObj = __box((size_t)
                        pkRecord->GetAVObject());
                    if (!pmPickedObjectToEntity->Contains(pmObj))
                    {
                        pmPickedObjectToEntity->Item[pmObj] = 
                            pmEntity;
                    }
                }
            }
        }
    }

    return bFoundIntersection;
}
//---------------------------------------------------------------------------
