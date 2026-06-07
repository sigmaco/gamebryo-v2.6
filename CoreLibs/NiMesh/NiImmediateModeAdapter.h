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

#include "NiMesh.h"
#include "NiCamera.h"
#include "NiRect.h"
#include "NiZBufferProperty.h"
#include "NiSmartPointer.h"
#include "NiColor.h"

//---------------------------------------------------------------------------
#ifndef NIIMMEDIATEMODEADAPTER_H
#define NIIMMEDIATEMODEADAPTER_H

/**
    Adapter to NiMesh.
    
    It provides an API to submit simple geometry 
    (vertices and colors) and draw it immediately. It is meant to have
    a similar API to the immediate mode of OpenGL. This class is
    designed to be used for visualizing debug geometry, such as AI paths,
    collision volumes, light positions, scene cameras, etc...

    The data for different geometry types is submitted depending on the
    primitive type. The unindexed and indexed appends can be used
    regardless of whether the geometry itself was created as indexed
    or unindexed. However, it is best to be consistent, if possible.

    When making multiple calls to the indexed append methods, the indices
    must be local to that append call. In other words, the only vertices
    that can referred to by an index are ones that are given in the
    same append call.
*/
class NIMESH_ENTRY NiImmediateModeAdapter : public NiRefObject
{
public:
    /// Constructor
    /// @param uiInitialNumVerts The initial size of the vertex buffer.
    /// @param uiInitialNumIndices The initial size of the index buffer.
    NiImmediateModeAdapter(
        unsigned int uiInitialNumVerts = 1024,
        unsigned int uiInitialNumIndices = 1024);

    /// Virtual destructor.
    virtual ~NiImmediateModeAdapter();

    /// Error code
    enum NiIMError
    {
        /// No error.
        NIIMERROR_OK = 0,

        /// Renderer has not been created.
        NIIMERROR_NO_RENDERER,

        /// No render target is currently bound.
        NIIMERROR_NO_BOUND_RENDER_TARGET,

        /// Begin was already called.
        NIIMERROR_REDUNDANT_BEGIN,

        /// End was already called.
        NIIMERROR_ALREADY_ENDED,

        /// Illegal state change between begin/end pair.
        NIIMERROR_ILLEGAL_STATE_CHANGE,

        /// Illegal append outside begin/end pair or an illegal
        /// number of primitives (i.e. appending only two verts
        /// for PRIMITIVE_TRIANGLES)
        NIIMERROR_ILLEGAL_APPEND,

        /// Camera not set.
        NIIMERROR_NO_CAMERA,

        NIIMERROR_MAX_ERROR
    };

    /**
        Begins the submission of a geometry primitive of a given type.

        @param ePrimitiveType Type of primitive to create.
        @param bIsIndexedPrimitive Optionally specifies whether or not this 
        primitive is indexed.  Any append function (indexed or unindexed) 
        can be used with both indexed and unindexed primitives.
        @return Returns success.  This function might fail if there
        is no valid renderer, the camera has not been set, or if
        BeginDrawing has already been called.
    */
    bool BeginDrawing(NiPrimitiveType::Type ePrimitiveType, 
        bool bIsIndexedPrimitive = true);

    /// @name Functions valid between BeginDrawing and EndDrawing.
    /// @note All of the functions in this group can be safely called
    /// between BeginDrawing and EndDrawing.
    //@{

    /// Is the adapter currently between a BeginDrawing/EndDrawing pair?
    inline bool IsDrawing() const;

    /// Is this primitive indexed?
    inline bool IsIndexed() const;

    /// Unindexed single primitive add, with default color.
    void Append(const NiPoint3* pkVert0);
    
    /// Unindexed single primitive add, with default color.
    void Append(const NiPoint3* pkVert0, const NiPoint3* pkVert1);
    
    /// Unindexed single primitive add, with default color.
    void Append(
        const NiPoint3* pkVert0, 
        const NiPoint3* pkVert1,
        const NiPoint3* pkVert2);
    
    /// Unindexed single primitive add, with default color.
    void Append(
        const NiPoint3* pkVert0, 
        const NiPoint3* pkVert1,
        const NiPoint3* pkVert2, 
        const NiPoint3* pkVert3);

    /// Unindexed add multiple, with default color.
    void Append(unsigned int uiNumVerts, const NiPoint3* pkVerts);
    
    /// Unindexed add multiple, per-vertex color.
    void Append(
        unsigned int uiNumVerts, 
        const NiPoint3* pkVerts,
        const NiColorA* pkColors);
    
    /// Indexed add multiple, with default color.
    void Append(
        unsigned int uiNumVerts, 
        const NiPoint3* pkVerts,
        unsigned int uiNumIndices, 
        const unsigned int* puiIndices);
    
    /// Indexed add multiple, per-vertex color.
    void Append(
        unsigned int uiNumVerts, 
        const NiPoint3* pkVerts,
        unsigned int uiNumIndices, 
        const unsigned int* puiIndices,
        const NiColorA* pkColors);

    /// Returns the last error state from a prior BeginDrawing or 
    /// EndDrawing call.
    NiIMError GetLastError() const;
    
    /// Clears the last reported error.
    inline void ClearLastError();

    /// Gets the z-buffer compare value (default is enabled).
    inline bool GetZBufferCompare() const;
    
    /// Gets the ZBufferWrite values (default is enabled).
    inline bool GetZBufferWrite() const;
    
    /// Gets the world transform (default is the identity).
    inline const NiTransform& GetWorldTransform() const;

    /// Gets the current camera.
    inline const NiCamera* GetCurrentCamera() const;

    /// Gets the number of vertices used between begin/end.
    inline unsigned int GetNumVerticesUsed() const;
    
    /// Gets the number of indices used between begin/end.
    inline unsigned int GetNumIndicesUsed() const;

    /// Gets the vertex buffer size.
    inline unsigned int GetVertexBufferSize() const;
    
    /// Gets the index buffer size.
    inline unsigned int GetIndexBufferSize() const;

    /// Set the default color to be used if not specified (default is
    /// NiColorA(0,0,0,1)).
    inline void SetCurrentColor(const NiColorA& kColor);
    
    /// Gets the current color.
    inline const NiColorA& GetCurrentColor() const;

    /// Gets the primitive type.
    inline NiPrimitiveType::Type GetPrimitiveType() const;

    //@}

    /// Finish drawing this primitive.  
    /// @return Returns true if successful.
    bool EndDrawing();

    /// @name Other state-related functions
    /// @note These state setting functions are only valid outside
    /// of a BeginDrawing/EndDrawing scope.  They will fail silently
    /// if called illegally and will set the NIIMERROR_ILLEGAL_STATE_CHANGE
    /// error state.
    //@{

    /// Set the size of the vertex buffer.
    void SetNumMaxVertices(unsigned int uiMaxVerts);
    
    /// Set the size of the index buffer.
    void SetNumMaxIndices(unsigned int uiMaxIndices);

    /// Convenience method for setting the current camera state to a 
    /// projection camera.
    /// @note One of the set camera functions must be called before drawing.
    void SetCurrentCamera(NiCamera* pkCamera);
    
    /// Convenience method for setting the current camera state to a 
    /// screen space camera.
    /// @note One of the set camera functions must be called before drawing.
    void SetScreenSpaceCameraData(const NiRect<float>* pkViewPort = NULL);
    
    /// Sets the world transform.
    void SetWorldTransform(const NiTransform& kTransform);
    
    /// Sets the ZBuffer property.
    void SetZBufferProperty(bool bCompareEnable, bool bWriteEnable);

    //@}

protected:

    /// Initializes the object
    void Initialize(unsigned int uiInitialNumVerts, 
        unsigned uiInitialNumIndices);

    /// Returns false if the number of verts and indices is invalid for
    /// this primitive type.
    inline bool IsValidAppend(unsigned int uiVertices,
        unsigned int uiIndices);
    inline bool IsValidStateChange();
    bool IsValidRendererState(NiRenderer* pkRenderer);
    inline void SubmitIfCantFit(unsigned int uiNumVerts,
        unsigned int uiNumIndices);
    inline void CheckAlphaBlending(const NiColorA& kColor);

    /// Begin an internal draw call after a silent submission.
    bool BeginDrawingInternal();

    /// Constructed mesh.
    NiMeshPtr m_spMesh;

    /// Default rendering color.
    NiColorA m_kDefaultColor;

    /// Initial size of the vertex buffer.
    unsigned int m_uiInitialVertexBufferSize;
    
    /// Initial size of the index buffer.
    unsigned int m_uiInitialIndexBufferSize;

    /// Number of verts between current begin/end sequence.
    unsigned int m_uiVerts;
    
    /// Number of indices between current begin/end sequence.
    unsigned int m_uiIndices;

    /// Number of verts used for this primitive, not including the 
    /// current begin/end.
    unsigned int m_uiTotalVerts;
    
    /// Number of indices used for this primitive, not including the 
    /// current begin/end.
    unsigned int m_uiTotalIndices;
    
    /// Indicates if the object is indexed.
    bool m_bIsIndexed;

    /// Managed stream reference.
    NiDataStreamPtr m_spIndexStream;
    
    /// Position data stream.
    NiDataStreamPtr m_spPositionStream;
    
    /// Color data stream.
    NiDataStreamPtr m_spColorStream;
    
    /// Positions.
    NiPoint3* m_pkPositionArray;
    
    /// Colors.
    NiColorA* m_pkColorArray;
    
    /// Index values.
    unsigned int* m_puiIndexArray;

    /// Indicates if the object is opaque.
    bool m_bIsOpaque;

    /// Last encountered error.
    NiIMError m_eLastError;

    /// Indicates if the adapter is in a valid draw state, e.g. after
    /// a successful call to BeginDrawing.
    bool m_bInDrawState;

    /// Cached cameras.
    bool m_bUseProjectedCamera;
    
    /// Cached cameras.
    bool m_bUseNullWindowCoords;

    /// Camera.
    NiCameraPtr m_spCamera;

    /// Window coordinates.
    NiRect<float> m_kWindowCoords;
};

#include "NiImmediateModeAdapter.inl"

NiSmartPointer(NiImmediateModeAdapter);

#endif // #ifdef NIIMMEDIATEMODEADAPTER_H

