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

#ifndef NIPHYSXSHAREDDATA_H
#define NIPHYSXSHAREDDATA_H

#include "NiPhysXSharedDataLibType.h"

#include <NiSharedData.h>
#include <NiPhysX.h>

// A shared data object to contain PhysX Props.
class NIPHYSXSHAREDDATA_ENTRY NiPhysXSharedData : public NiSharedData
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiPhysXSharedData();

    /// Virtual destructor.
    virtual ~NiPhysXSharedData();

    // Get and set the PhysX props
    NiUInt32 GetPropCount();
    NiPhysXProp* GetPropAt(NiUInt32 uiIndex);
    void AddProp(NiPhysXProp* pkProp);
    void RemoveProp(NiPhysXProp* pkProp);

    // Get and set the PhysX scene
    NiPhysXScene* GetScene();
    void SetScene(NiPhysXScene* pkScene);

    class StateInfo : public NiMemObject
    {
    public:
        NiFixedString m_kStateName;
        float m_fStateTime;
    };

    NiUInt32 GetStateCount();
    StateInfo& GetStateAt(NiUInt32 uiIndex);
    void AddState(const StateInfo& kState);
    void RemoveState(const StateInfo& kState);

protected:
    NiTObjectArray<NiPhysXPropPtr> m_kProps;
    NiPhysXScenePtr m_spScene;
    
    NiTPrimitiveArray<StateInfo*> m_kStates;
};

NiSmartPointer(NiPhysXSharedData);

#endif  // #ifndef NIPHYSXSHAREDDATA_H
