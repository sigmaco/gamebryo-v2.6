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

#ifndef NIUPDATEPROCESS_H
#define NIUPDATEPROCESS_H

#include "NiRTTI.h"
#include "NiRefObject.h"
#include "NiSmartPointer.h"
#include "NiSyncArgs.h"

class NiSPWorkflowManager;
class NiMeshUpdateProcess;
class NiUnitTestUpdateProcess;
class NiAVObject;

class NIMAIN_ENTRY NiUpdateProcess : public NiRefObject
{
    NiDeclareRootRTTI(NiUpdateProcess);
public:
    virtual ~NiUpdateProcess();

    virtual void PreUpdate(NiAVObject* pkObject) = 0;
    virtual void PostUpdate(NiAVObject* pkObject) = 0;

    inline float GetTime() const;
    inline void SetTime(float fTime);

    inline bool GetUpdateControllers() const;
    inline void SetUpdateControllers(bool bUpdateControllers);

    // Specifies whether or not mesh modifiers should be submitted during or
    // following update. Setting this to false will allow an update pass to be
    // completed without requiring the updated mesh objects to have their 
    // modifiers completed.
    bool GetSubmitModifiers() const;
    void SetSubmitModifiers(bool bSubmitModifiers);

    /// Get the workflow manager held by this class.  Declared in NiMesh. 
    inline NiSPWorkflowManager* GetWorkflowManager() const;
protected:
    NiSPWorkflowManager* m_pkWorkflowManager;
    float m_fTime;
    bool m_bUpdateControllers;
    bool m_bSubmitModifiers;

    inline void SetWorkflowManager(NiSPWorkflowManager* pkManager);
private:
    // Note that the constructors are declared private so that no one may
    // successfully derive from NiUpdateProcess. Applications should derive
    // from NiMeshUpdateProcess, declared here as a friend, so that mesh
    // modifiers are appropriately flushed during an Update.
    friend class NiMeshUpdateProcess;

    // Additionally, declare a testing class as friend so that we can test
    // core NiMain functionality without having to depend on NiMesh.
    friend class NiUnitTestUpdateProcess;

    NiUpdateProcess();
};

NiSmartPointer(NiUpdateProcess);


// This class holds data for mesh modifiers and other parallel, synchronized
// tasks.
class NIMAIN_ENTRY NiUpdateSyncArgs : public NiSyncArgs
{
public:
    NiUpdateSyncArgs(NiUpdateProcess& kUpdateProcess);

    NiUpdateProcess& m_kUpdateProcess;

private:
    NiUpdateSyncArgs & operator=( const NiUpdateSyncArgs & );

};


#include "NiUpdateProcess.inl"

#endif

