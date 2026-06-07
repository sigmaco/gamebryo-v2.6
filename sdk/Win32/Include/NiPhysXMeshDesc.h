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


#ifndef NIPHYSXMESHDESC_H
#define NIPHYSXMESHDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated

#pragma warning(disable: 4244) // conversion from 'type' to 'type',
                               // possible loss of data

#pragma warning(disable: 4245) // conversion from 'type' to 'type',
                               // signed/unsigned mismatch

#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxCooking.h>
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXMeshDesc objects hold mesh data for streaming.
    
    Applications will typically only touch these objects when working on tools
    that stream PhysX content, or when constructing NiPhysXActorDesc or
    NiPhysXClothDesc objects. 

    Internally, this class stores the name of the mesh, cooked mesh data if
    it is known, and any flags for creating the mesh. The flags, paging mode
    and name properties only affect shapes when they are created for an actor -
    they do not modify shapes that have already been attached to actors.
*/
class NIPHYSX_ENTRY NiPhysXMeshDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    /// @endcond

public:
    /// Constructor creates an object with no name or data.
    NiPhysXMeshDesc();
    
    /// Destructor deletes any data.
    ~NiPhysXMeshDesc();
    
    /// Returns true if this object represents a cloth mesh.
    bool GetIsCloth() const;

    /// Sets this object to represent a cloth mesh.
    void SetIsCloth();

    /// Returns true if this object represents a convex mesh.
    bool GetIsConvex() const;

    /// Sets this object to represent a convex mesh.
    void SetIsConvex();

    /// Returns true if this object represents a trishape mesh.
    bool GetIsTriMesh() const;

    /// Sets this object to represent a trishape mesh.
    void SetIsTriMesh();

    /**
        Get the platform for which this mesh was cooked.
        
        Returns true and sets kPLatform if there is platform information
        available, or returns false and leaves kPlatform unchanged if
        there is no knowledge of the platform.
    */
    bool GetPlatform(NxPlatform& kPlatform) const;

    /// Set the target platform.
    void SetPlatform(const NxPlatform kPlatform);
    
    /**
        Return true if the platform on which the application is executing is
        the same as the platform for which the mesh was cooked, false
        otherwise.
        
        The function returns true if there is no mesh information available. 
    */
    bool IsValidPlatform() const;
    
    /**
        Get mesh flags.
        
        These are the flags stored in a PhysX NxClothMeshDesc, NxConvexMeshDesc
        or NxTriangleMeshDesc object.
    */
    NxU32 GetFlags() const;

    /// Set the mesh flags.
    void SetFlags(const NxU32 uiFlags);

    /// Returns true if this mesh was cooked for PhysX hardware simulation.
    bool GetHardware() const;

    /// Set the flag indicating hardware cooking.
    void SetHardware(const bool bHardware);

    /// Get the mesh paging mode associated with the mesh.
    NxMeshPagingMode GetMeshPagingMode() const;

    /// Set the mesh paging mode associated with the mesh.
    void SetMeshPagingMode(const NxMeshPagingMode eMode);

    /// Get the name used for the mesh.
    const NiFixedString& GetName() const;
    
    /**
        Set the name to use for the mesh.

        The name is the file name for an extranlly streamed mesh. Names
        are used to control mesh sharing, so do not use the same name for two
        meshes that are not the exact same geometry.
    */
    void SetName(const NiFixedString& kName);

    /// Get the cooked mesh data and its size.
    void GetData(size_t& kSize, unsigned char** ppucData);

    /// Set the raw cooked mesh data.
    ///
    /// The data size must also be provided.
    void SetData(const size_t uiSize, unsigned char* pucData);

    /**
        Get the mapping between Gamebryo vertices and PhysX mesh vertices.
        
        The map is an array indexed by Gamebryo vertex indices that gives the
        PhysX mesh index for the corresponding vertex. This data is only
        created for cloth meshes by the Gamebryo art export tools, but
        applications could create and use it for the other meshes if desired.
    */
    void GetVertexMap(NiUInt16& usSize, const NiUInt16*& pusMap);
    
    /// Set the vertex map.
    ///
    /// See GetVertexMap for details.
    void SetVertexMap(const NiUInt16 usSize, NiUInt16* pusMap);

    /**
        Fill the contents of a PhysX cloth mesh descriptor object.
        
        This function takes the cooked data and calls
        NxPhysicsSDK::createClothMesh. If no mesh data is present, then the
        mesh name is used to look for a file containing the mesh data
        (see NiPhysXClothMesh for details on loading external meshes).
        If bKeepsMesh is false, the mesh data is deleted after the mesh is
        created. The mesh that results is also registered with the
        NiPhysXManager object.
    */
    virtual void ToClothDesc(NxClothDesc& kClothDesc, const bool bKeepData,
        const bool bCheckPlatform = false);

    /**
        Fill the contents of a PhysX convex mesh descriptor object.
        
        This function takes the cooked data and calls
        NxPhysicsSDK::createConvexMesh. If no mesh data is present, then the
        mesh name is used to look for a file containing the mesh data
        (see NiPhysXConvexMesh for details on loading external meshes).
        If bKeepsMesh is false, the mesh data is deleted after the mesh is
        created. The mesh that results is also registered with the
        NiPhysXManager object.
    */
    virtual void ToConvexDesc(NxConvexShapeDesc& kConvexDesc,
        const bool bKeepData, const bool bCheckPlatform = false);
        
    /**
        Fill the contents of a PhysX triangle mesh descriptor object.
        
        This function takes the cooked data and calls
        NxPhysicsSDK::createTriangleMesh. If no mesh data is present, then the
        mesh name is used to look for a file containing the mesh data
        (see NiPhysXTriangleMesh for details on loading external meshes).
        If bKeepsMesh is false, the mesh data is deleted after the mesh is
        created. The mesh that results is also registered with the
        NiPhysXManager object.
    */
    virtual void ToTriMeshDesc(NxTriangleMeshShapeDesc& kTriMeshDesc,
        const bool bKeepData, const bool bCheckPlatform = false);

 
    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    NxU32 m_uiMeshFlags;
    NiFixedString m_kMeshName;
    size_t m_uiMeshSize;
    unsigned char* m_pucMeshData;
    NxMeshPagingMode m_eMeshPagingMode;
    NiUInt16 m_usVertexMapSize;
    NiUInt16* m_pusVertexMap;

    // Gamebryo flags
    enum {
        IS_CONVEX_MASK = 0x01,
        IS_CLOTH_MASK  = 0x02,
        HARDWARE_MASK  = 0x04,
        COOKED_FOR_WIN32_MASK = 0x08,
        COOKED_FOR_PS3_MASK = 0x10,
        COOKED_FOR_XENON_MASK = 0x20
    };
    
    NiDeclareFlags(unsigned char);
};

NiSmartPointer(NiPhysXMeshDesc);

#include "NiPhysXMeshDesc.inl"

#endif // NIPHYSXMESHDESC_H

