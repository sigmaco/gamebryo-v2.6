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


#ifndef NIPHYSXCLOTHMESH_H
#define NIPHYSXCLOTHMESH_H

#include "NiPhysXMemStream.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXLibType.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

class NiPhysXMeshDesc;

/**
    NiPhysXClothMesh is a container class for PhysX NxClothMesh objects.
    
    This class associates a name with the mesh and provides functionality to
    load a mesh from an external file. It also stores a NiPhysXMeshDesc object
    to facilitate mesh sharing in streamed content.

    Objects of this type are stored in the NiPhysXManager object, which is the
    central location for collision mesh storage in Gamebryo-PhysX.
    Applications rarely create one of these objects - they are created
    automatically by NiPhysXMeshDesc objects when PhysX content is
    instantiated.
*/
class NIPHYSX_ENTRY NiPhysXClothMesh : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    // @endcond

public:
    /// The constructor sets the name to the NULL string and the mesh is set
    /// to stream inline.
    NiPhysXClothMesh();

    /// Destructor.
    virtual ~NiPhysXClothMesh();

    /// Get the name for the mesh.
    ///
    /// If streamed externally, this is the platform-independent filename.
    const NiFixedString& GetName() const;

    /// Set the name for the mesh.
    ///
    /// If streamed externally, this is the platform-independent filename.
    void SetName(const NiFixedString& kName);

    /// Get the NxClohMesh object that this object is controlling.
    NxClothMesh* GetMesh() const;

    /// Set the NxClohMesh object that this object is controlling.
    void SetMesh(NxClothMesh* pkMesh);

    /**
        Get the flag controlling streaming.

        If streamed inline, the cooked mesh data is stored in the NIF file.
        Otherwise, the mesh name is presumed to be a file name, which, when
        modified according to the platform specific directories in
        NiPhysXManager, gives the location of the cooked mesh data.
    */
    const bool GetStreamInline() const;

    /// Set the flag controlling streaming.
    void SetStreamInline(const bool bVal);

    /// Get the Gamebryo-PhysX mesh descriptor related to this mesh.
    ///
    /// The mesh descriptor is used in NiPhysXClothDesc.
    NiPhysXMeshDesc* GetMeshDesc() const;

    /// Set the Gamebryo-PhysX mesh descriptor related to this mesh.
    void SetMeshDesc(NiPhysXMeshDesc* pkDesc);

    /// Get the area of the cloth mesh in it's un-deformed state.
    ///
    /// This area is in PhysX units.
    float GetArea();

    /**
        Create a NiPhysXClothMesh from a given filename.

        The file must be a cooked NxClothMesh object, typically streamed
        directly out of NxCookClothMesh. The platform dependent pathname stored
        in NiPhysXManager is used to resolve the file location. A
        NiPhysXFileStream object is then created from the file and passed to
        the PhysX SDK to create the mesh. The name of the resulting mesh is
        set to the passed filename. The pkDesc argument is the NiPhysXMeshDesc
        that should be associated with this mesh (typically the source of
        the information used to create the mesh). This function returns 0 if
        anything goes wrong in this process. 
    */
    static NiPhysXClothMesh* Create(NiPhysXMeshDesc* pkDesc,
        const NiFixedString& kName);

    /** 
        Create a NiPhysXClothMesh from a given memory buffer.

        The kStream object must be the result of a call to NxCookClothMesh.
        Applications will typically use this function if they are cooking a
        PhysX mesh at runtime or in an authoring tool. This memory stream is
        passed directly to the PhysX SDK to create the mesh. The name of the
        resulting mesh is set to the kName. The pkDesc argument is the
        NiPhysXMeshDesc that should be associated with this mesh (typically the
        source of the information used to create the mesh). This function
        returns 0 if anything goes wrong in this process. 
    */
    static NiPhysXClothMesh* Create(NiPhysXMeshDesc* pkDesc,
        NiPhysXMemStream& kStream, const NiFixedString& kName);

protected:
    bool CreateMesh();
    
    NiFixedString m_kName;    
    NxClothMesh* m_pkMesh;
    bool m_bStreamInline;
    NiPhysXMeshDesc* m_pkMeshDesc;
    
    float m_fArea;
};

NiSmartPointer(NiPhysXClothMesh);

#include "NiPhysXClothMesh.inl"

#endif // #ifdef NIPHYSXCLOTHMESH_H

