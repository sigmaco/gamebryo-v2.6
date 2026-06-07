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

#ifndef NIPHYSXMESHMODIFIER_H
#define NIPHYSXMESHMODIFIER_H

#include "NiPhysXLibType.h"

#include "NiMeshLib.h"

class NiPhysXScene;

/**
    This virtual class is the base for mesh modifiers that must interface with
    the NiPhysX system.
        
    Derived from NiMeshModifier, this class adds functions for the modifier to
    respond when the properties of a PhysX simulation are changed in some way.
*/
class NIPHYSX_ENTRY NiPhysXMeshModifier : public NiMeshModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPhysXMeshModifier);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;
    
    /// @endcond

public:
    /// Constructor calls superclasses's constructor.
    NiPhysXMeshModifier();
    
    /// Destructor does nothing.
    virtual ~NiPhysXMeshModifier();
    
    /**
        Adjust to a change in the nature of the simulation.
        
        This function is called from NiPhysXScene when the modifier is
        first associated with the scene and then when meta data controlling
        simulation is changed by one of the following NiPhysXScene functions:
        SetPhysXScene, SetSceneXform, SetScaleFactor,
        SetAsynchronousSimulation, SetUpdateSrc and SetUpdateDest. The
        NiPhysXScene object argument should be inspected to
        identify any changes to the timing or other properties of the
        simulation that are relevant for this modifier.
        
        The pkScene argument will be NULL when the mesh is being removed
        from a PhysX scene.
    */
    virtual void SceneChanged(NiMesh* pkMesh, NiPhysXScene* pkScene) = 0;

protected:
        
};

// Define a NiMeshModifier smart pointer
NiSmartPointer(NiPhysXMeshModifier);

#endif // #ifndef NIPhysXMESHMODIFIER_H
