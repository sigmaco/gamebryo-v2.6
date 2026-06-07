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

#ifndef NICULLINGPROCESS_H
#define NICULLINGPROCESS_H

#include "NiRTTI.h"
#include "NiFrustumPlanes.h"
#include "NiVisibleArray.h"
#include "NiSyncArgs.h"

class NiAVObject;
class NiCamera;
class NiRenderObject;
class NiSPWorkflowManager;

class NIMAIN_ENTRY NiCullingProcess : public NiRefObject
{
    NiDeclareRootRTTI(NiCullingProcess);
public:
    virtual ~NiCullingProcess();

    // Construct the potentially visible set of objects in the scene.
    //
    // The input array provides storage for the potentially visible set of
    // objects. This array may be shared by other NiCullingProcess objects.
    // Because the array may be shared, the application is responsible for
    // initially clearing the array before starting any culling processes
    // that share it.
    //
    // If the NiCullingProcess constructor was passed a non-null visible set
    // pointer and you want Cull to use that set, pass null to the
    // visible set pointer of Cull. If you pass a non-null visible set
    // pointer to Cull, the function will use that set instead of the
    // one passed to the NiCullingProcess constructor. If the
    // NiCullingProcess constructor is passed a null visible set pointer
    // and if Cull is passed a null visible set pointer, Cull does
    // nothing.
    //
    // This function is considered "atomic" in the sense that (1) the input
    // camera and scene should not be modified and (2) the input array must
    // exist during the lifetime of the function call.
    void Cull(const NiCamera* pkCamera, NiAVObject* pkScene,
        NiVisibleArray* pkVisibleSet);

    // Access to the visible set, camera, and frustum currently in use by
    // Process. The frustum is copied by Process so that derived classes may
    // overwrite it as needed without changing the camera's copy.
    inline void SetVisibleSet(NiVisibleArray* pkVisibleSet);
    inline NiVisibleArray* GetVisibleSet();
    inline const NiCamera* GetCamera() const;
    inline const NiFrustum& GetFrustum() const;
    void SetFrustum(const NiFrustum& kFrustum);
    inline const NiFrustumPlanes& GetFrustumPlanes() const;

    // Get the workflow manager held by this class. Note that 
    // NiSPWorkflowManager is declared in the NiFloodgate library meaning that
    // NiCullingProcess itself can do nothing with the manager other than
    // store its address.
    inline NiSPWorkflowManager* GetWorkflowManager();

    // Specifies whether or not mesh modifiers should be submitted during
    // culling. Setting this to false will allow a culling pass to be
    // completed without requiring the visible mesh objects to be rendered or
    // have their modifiers completed.
    inline bool GetSubmitModifiers() const;
    inline void SetSubmitModifiers(bool bSubmitModifiers);

protected:
    // Append is called each time a potentially visible geometry object is
    // encountered during the Process call. The base version simply adds
    // the object to the visible array, so it is made inline. This is the
    // most common case for subclasses, as well. However, it is not the only
    // case, so we must allow for virtual behavior; for example, the portal
    // system maintains a set of unique potentially visible objects, so its
    // Append must detect if the input object is already in the potentially
    // visible set.
    // As a result, we use a "mixture" of virtual and non-virtual behavior.
    // This base class declares and defines an inline function that handles
    // most cases directly - inside of this non-virtual function, we test a
    // boolean flag - if this flag is set "true", then we call the virtual
    // member AppendVirtual instead of the code in the inline function.
    // Thus, we gain the performance benefit of an inline function for most
    // culling process classes, but allow for virtual overrides as required
    // by subclasses.
    // NiRenderObject::OnVisible is the only class needing to call this
    // function publicly outside of the NiCullingProcess class hierarchy.
    friend class NiRenderObject;
    friend class NiGeometry;
    inline void Append(NiRenderObject& kVisible);

    // Previous versions of Gamebryo declared Process as a public virtual.
    // The current version uses the non-virtual method, Cull, as the entry
    // point into the culling process. Applications should call Cull rather 
    // than direct calls to Process.
    virtual void Process(const NiCamera* pkCamera, NiAVObject* pkScene,
        NiVisibleArray* pkVisibleSet) = 0;

    // *** begin Emergent internal use only ***
    // The following methods are used by Gamebryo to handle flushing of 
    // mesh modifiers and other activities after the call to Process has
    // completed inside Cull. It is not recommended that they be overridden.
    // They are provided as pure virtual functions to avoid circular
    // dependencies with NiFloodgate and NiMesh.
    virtual void PreProcess() = 0;
    virtual void PostProcess() = 0;
    // *** end Emergent internal use only ***

    // If this is overridden in a subclass, the subclass version _must_ not
    // call the base class Append, or else a recursive loop will result
    inline virtual void AppendVirtual(NiRenderObject&) { /* */ }

    // This function is called only by NiAVObject::Cull during a recursive
    // traversal started by Process(const NiCamera*, NiAVObject*,
    // NiVisibleArray*).
    friend class NiAVObject;
    virtual void Process(NiAVObject* pkObject);

    // The bool that declares a given class's need to use the virtual version
    // of Append, AppendVirtual. It is declared const so that it can only be
    // set at construction time.
    const bool m_bUseVirtualAppend;

    // The potentially visible objects in the input scene to Process.
    NiVisibleArray* m_pkVisibleSet;

    // Pointer to workflow manager. See comments above for GetWorkflowManager.
    NiSPWorkflowManager* m_pkWorkflowManager;

    // The camera and frustum currently in use by Process.
    const NiCamera* m_pkCamera;
    NiFrustum m_kFrustum;

    NiFrustumPlanes m_kPlanes;

    // Indicates whether or not mesh modifiers should be submitted during
    // culling. Setting this to false will allow a culling pass to be
    // completed without requiring the visible mesh objects to be rendered or
    // have their modifiers completed.
    bool m_bSubmitModifiers;

private:
    // Note that the constructors are declared private so that no one may
    // successfully derive from NiCullingProcess. Applications should derive
    // from NiMeshCullingProcess, declared here as a friend, so that mesh
    // modifiers are appropriately flushed after calling Cull.
    friend class NiMeshCullingProcess;

    // Additionally, declare a testing class as friend so that we can test
    // core NiMain functionality without having to depend on NiMesh.
    friend class NiUnitTestCullingProcess;

    // The input NiVisibleArray provides storage for the potentially visible 
    // set of objects. This array may be shared by other NiCullingProcess 
    // objects.
    //
    // Because the array may be shared, the application is responsible for
    // initially clearing the array before starting any culling processes
    // that share it.
    //
    // If the input array is deleted but the NiCullingProcess object using
    // it persists, the application is responsible for not using the
    // NiCullingProcess object. To be safe in this situation, use the
    // SetVisibleSet function to reset the internal visible set pointer to
    // NULL.
    //
    // The input array can be NULL, in which case the storage for the
    // potentially visible set must be provided via the Process function
    // described later in this class.
    NiCullingProcess(NiVisibleArray* pkVisibleSet,
        NiSPWorkflowManager* pkWorkflowManager);

    // This is designed to allow only subclasses to change the value of
    // m_bUseVirtualAppend (see discussion of Append and AppendVirtual below)
    NiCullingProcess(NiVisibleArray* pkVisibleSet, 
        NiSPWorkflowManager* pkWorkflowManager, bool bUseVirtualAppend);

    // Disable warning C4512
    NiCullingProcess & operator=( const NiCullingProcess & );
};

NiSmartPointer(NiCullingProcess);


// This class holds data for mesh modifiers and other parallel, synchronized
// tasks.
class NIMAIN_ENTRY NiCullingSyncArgs : public NiSyncArgs
{
public:
    NiCullingSyncArgs(NiCullingProcess& kUpdateProcess);

    NiCullingProcess& m_kCullingProcess;

private:
    NiCullingSyncArgs & operator=( const NiCullingSyncArgs & );
};


#include "NiCullingProcess.inl"

#endif
