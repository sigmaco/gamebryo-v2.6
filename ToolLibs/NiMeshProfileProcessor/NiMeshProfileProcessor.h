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

#ifndef NIMESHPROFILEPROCESSOR_H
#define NIMESHPROFILEPROCESSOR_H

#include "NiMeshProfileProcessorLibType.h"
#include "NiMeshProfileErrorHandler.h"
#include "NiComponentInput.h"
#include "NiOpProfile.h"
#include "NiOpProfileFragment.h"
#include <NiInterleaveProfile.h>
#include <NiRenderObject.h>
#include <NiTSet.h>
#include <NiDataStreamRef.h>
#include <NiToolPipelineCloneHelper.h>
#include <NiInterleaver.h>

/// @cond EMERGENT_INTERNAL
/**
    Singleton that provides static methods for registering profiles,
    finalizing scenegraphs, etc. This class is used 
    directly by the MeshFinalize plugin as well as by tools and exporters.

    @note Tools and exporters should, as a policy, take care of creation
    and destruction of the singleton.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiMeshProfileProcessor : public NiMemObject
{
public:
    enum Strict
    {
        // Format of the stream is strictly enforced
        STRICT_FORMAT       = 0x04,

        // If requires own stream is strictly enforced
        STRICT_INTERLEAVE   = 0x08,
    };

    /**
        @name Constructors
    */
    //@{
    /// Default Constructor
    NiMeshProfileProcessor();
    //@}

    /// Destructor.
    ~NiMeshProfileProcessor();

    /// Initializes the singleton.
    static NiMeshProfileProcessor* CreateMeshProfileProcessor();

    /// Destroys the singleton.
    static void DestroyMeshProfileProcessor();

    /// Retrieves the singleton.
    static NiMeshProfileProcessor* GetMeshProfileProcessor();

    /// Finalizes a number of scenegraphs. Includes interleaving.
    static void Finalize(NiTLargeObjectArray<NiObjectPtr>& kTopObjects,
        NiSystemDesc::RendererID eRenderer);

    /// Finalizes a scenegraph. Includes interleaveing
    static void Finalize(NiMesh* pkMesh, NiSystemDesc::RendererID eRenderer);

    /// Returns the names of all available profiles for the given renderer.
    static void GetAvailableProfiles(NiTPrimitiveSet<const char*>& kProfiles,
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_GENERIC);

    /// Resets the NiMeshProfileProcessor's state. This includes clearing out
    /// the clone helper, clearing error messages, state, etc.
    static void Reset();

    /// Retrieves the name of the profile that will be used.
    static const char* GetProfileName(NiMesh* pkMesh);

    /// Retrieves a NiOpProfile by name and renderer.
    static const NiOpProfile* GetProfile(const char* pcName, 
        NiSystemDesc::RendererID eRenderer);

    /// Adds a profile to the NiMeshProfileProcessor library.
    static void AddProfile(NiOpProfile* pkProfile);
    
    /// Retrieves the error handler used by the NiMeshProfileProcessor.
    static NiMeshProfileErrorHandler& GetErrorHandler();

    typedef void (*ProgressInitCallback)(unsigned int uiSize);
    typedef void (*ProgressCallback)(NiMeshProfileErrorHandler&
        kMessageHandler);

    /// Allows setting a callback to initalize progress steps required
    static void SetProgressInitCallback(ProgressInitCallback pfnCallback);

    /// Retrieves the currently set ProgressInitCallback
    static void GetProgressInitCallback(ProgressInitCallback& pfnCallback);

    /// Allows setting a callback that is triggered when progress steps are
    /// taken.
    static void SetProgressCallback(ProgressCallback pfnCallback);

    /// Retrieves the currently set ProgressCallback
    static void GetProgressCallback(ProgressCallback& pfnCallback);

private:
    static void CreateMeshToProfileMap(
        NiToolPipelineCloneHelper& kCloneHelper,
        NiTPointerMap<NiMesh*, NiOpProfile*>& kMeshToProfile,
        NiSystemDesc::RendererID eRenderer);


    // Helper function to find a profile
    NiOpProfile* FindProfile(const char* pcName, 
        NiSystemDesc::RendererID eRenderer);

    // Helper function to find the correct profile given a mesh
    NiOpProfile* GetProfile(NiMesh* pkMesh,
        NiSystemDesc::RendererID eRenderer);

    // Retrieves the profile named "Default", or returns NULL if it does not
    // exist.
    NiOpProfile* GetDefaultProfile(NiSystemDesc::RendererID eRenderer);

    static NiMeshProfileProcessor* ms_pkThis;

    // Binary version of all the profiles
    NiTObjectArray<NiOpProfilePtr> m_kProfiles;

    NiMeshProfileErrorHandler m_kReportedErrors;

    ProgressInitCallback m_pfnProgressInitCallback;
    ProgressCallback m_pfnProgressCallback;
};

#endif  // #ifndef NIMESHPROFILEPROCESSOR_H
