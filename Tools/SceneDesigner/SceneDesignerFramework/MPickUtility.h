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

#pragma once

#include "MDisposable.h"
#include "MScene.h"
#include "MViewport.h"

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPickUtility : public MDisposable
    {
    public:
        // allows us to map template ID to an overridden pick algorithm
        __gc class MPickPolicy
        {
        public:
            Guid mTemplateID;
            virtual bool PickOnEntity(MEntity* pmEntity, 
                const NiPoint3& kOrigin, const NiPoint3& kDir, 
                NiPick* pkPick, Hashtable* pmPickedObjectToEntity);
        };

        MPickUtility();

        bool PerformPick(MScene* pmScene, const NiPoint3& kOrigin,
            const NiPoint3& kDir, bool bIncludeFrozenEntities);
        const NiPick* GetNiPick();

        MEntity* GetEntityFromPickedObject(NiAVObject* pkPickedObject);

        MEntity* GetEntityFromViewCoordinates(MScene* pmScene, 
            MViewport* pmViewport, int iX, int iY);

        MPickPolicy* FindPolicy(MEntity* pmEntity);
        void AddPolicy(MPickPolicy* pmPolicy);
        void RemovePolicy(MPickPolicy* pmPolicy);

    private:
        NiPick* m_pkPick;
        NiFixedString* m_pkSceneRootPointerName;
        Hashtable* m_pmPickedObjectToEntity;
        ArrayList* m_pmRegisteredPolicies;
        MPickPolicy* m_pmDefaultPolicy;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
