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

#ifndef NIMESHHWINSTANCE_H
#define NIMESHHWINSTANCE_H

#include <NiAVObject.h>
#include <NiRenderObject.h>

class NiInstancingMeshModifier;

/**
    This class represents an object that can be used to represent an instance
    of a hardware instanced mesh in a scene graph.
    
    NiMeshHWInstance objects
    work with a scene graph just like NiAVObjects, meaning they inherit their 
    transform from their parent nodes. This allows an application to populate
    a scene graph with NiMeshHWInstance objects that represent the instances
    of a mesh, and work with them like any other NiAVObject. 
*/
class NIMESH_ENTRY NiMeshHWInstance : public NiAVObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiMeshHWInstance);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond
public:
    
    /**
        Constructor that allows the master mesh to be specified.
    
        Note this does not inform the master mesh that a NiMeshHWInstance is
        referencing it. NiInstancingUtilities::AddMeshInstance() will still
        need to be called.
    */
    NiMeshHWInstance(NiMesh* pkMesh);
    
    /// Default constructor.
    NiMeshHWInstance();
    
    /// Virtual destructor.
    virtual ~NiMeshHWInstance();
    
    /// Returns the instanced mesh that this object represents.
    NiMesh* GetMesh();

    /// @cond EMERGENT_INTERNAL

    /// Update methods.
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);

    /// Update the world bounds.
    virtual void UpdateWorldBound();

    /// Cloning methods.
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// Specifies the instanced mesh that this object represents.
    void SetMesh(NiMesh* pkMesh);
    
    /// @endcond

protected:
    /// The master mesh that this object represents.
    NiMesh* m_pkMasterMesh;
    
    /// A pointer to the NiInstancingMeshModifier attached to the master mesh.
    /// This pointer provides a fast way to inform the mesh modifier that the 
    /// transforms of one of its instances have changed.
    NiInstancingMeshModifier* m_pkMeshModifier;
};

NiSmartPointer(NiMeshHWInstance);



#endif // #ifndef NIMESHHWINSTANCE_H
