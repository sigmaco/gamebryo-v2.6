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

#ifndef NIMESHSCREENELEMENTS_H
#define NIMESHSCREENELEMENTS_H

#include "NiMesh.h"
#include "NiRenderer.h"
#include <NiTexturingProperty.h>
#include <NiZBufferProperty.h>

/**
    2D primitives that are derived from NiMesh, 
    and inherit the characteristics of that class, including the ability to 
    have transforms, shaders, properties and effects applied. 

    They are a versatile tool for rendering elements in screen space, 
    but they require some special handling.
    
    Internally, NiMeshScreenElements are only safe for 16-bit counts, so
    keep all vertex and index counts below 65535.
*/
class NIMESH_ENTRY NiMeshScreenElements : public NiMesh
{
public:
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiMeshScreenElements);
    NiDeclareStream;
    NiDeclareViewerStrings;
    
    /// @endcond

public:
    /**
        Constructs a new NiMeshScreenElements object. 

        The initial storage quantities for polygons 
        (iMaxPQuantity and iPGrowBy), vertices (iMaxVQuantity and
        iVGrowBy), and triangles (iMaxTQuantity and iTGrowBy) have default
        values for the common case of a single axis-aligned rectangle on the
        screen.  The growth quantities for the containers also have default
        values.  If you know how much storage is required for your application,
        provide these values to avoid memory reallocations during the lifetime
        of this object.

        The uiAccessMask parameter describes the access mask for the streams.
        All streams in the elements will share the same access mask.

        The bForceFixedFunctionColors allows the creation of an 
        NiMeshScreenElements object that will work with the fixed-function
        DX9 pipeline, though it may not work with other renderers.
    */
    NiMeshScreenElements(
        bool bWantNormals, 
        bool bWantColors,
        NiUInt16 usNumTextureSets, 
        NiInt32 iMaxPQuantity = 1,
        NiInt32 iPGrowBy = 1, 
        NiInt32 iMaxVQuantity = 4,
        NiInt32 iVGrowBy = 1,
        NiInt32 iMaxTQuantity = 2,
        NiInt32 iTGrowBy = 1,
        NiUInt8 uiAccessMask = NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        bool bForceFixedFunctionColors = false);

    /// Default constructor.
    NiMeshScreenElements();

    /**
        Static factory method.
    */
    static NiMeshScreenElements* Create(
        NiRenderer* pkRenderer, 
        const float fXOffset, 
        const float fYOffset,
        const unsigned int uiWidth, 
        const unsigned int uiHeight,
        const NiRenderer::DisplayCorner eCorner, 
        const bool bForceSafeZone = true);

    /// @name Converting from NiScreenTexture
    /**
        The following methods are meant to ease the porting of old code that
        used NiScreenTexture. 
        
        The signatures and arguments to these functions match those in
        the deprecated class NiScreenTexture.
        
        It is not intended that these methods will be used for newly-developed
        code, which should instead use the Insert and Set methods below.
    */
    //@{
    /**
        Factory Method.

        Creates an NiMeshScreenElements using the supplied NiTexture object. 
        A new NiTexturingProperty is created, and the NiTexture passed in is 
        set as the base map. The base clamp mode is set to CLAMP_S_CLAMP_T, 
        and the apply mode is set to the value provided in eMode, which
        defaults to APPLY_REPLACE. 
    */
    static NiMeshScreenElements* Create(NiTexture* pkTexture,
        NiTexturingProperty::ApplyMode eMode = 
        NiTexturingProperty::APPLY_REPLACE);

    /**
        Insert screen-space rectangle.
        
        This takes screen-space coordinates and converts to Normalized Device
        Coordinates (NDC).
        
        @note If the render target is not the default back buffer, the
        conversion will not be correct. In this case set the screen width and 
        height directly.
    */
    void AddNewScreenRect(
        short sTop, 
        short sLeft, 
        unsigned short usWidth, 
        unsigned short usHeight, 
        unsigned short usTexTop, 
        unsigned short usTexLeft,
        NiColorA kColor = NiColorA::WHITE,
        unsigned int uiScreenWidth = 0, unsigned int uiScreenHeight = 0);
    //@}

    /// Destructor.
    virtual ~NiMeshScreenElements();

    /**
        @name Batch Updating
        
        These methods help performance of a chain of updates
        by locking and unlocking at the outermost scale rather
        than at each individual update.
        
        If you know that you are going to be doing many updates in a row,
        these can give a significant performance enhancement, since each
        stream is only locked once at the BeginBatchUpdate() call and unlocked
        once at the EndBatchUpdate() call. Otherwise, each stream is likely
        to be locked and unlocked for each get/set call to modify
        the on-screen polygons.
    */
    //@{
    /**
        Begins a batch update and sets the local cache of the data for
        each stream in the MeshScreenElements.

        @param uiLockMask Mask used to lock the data streams.
    */
    void BeginBatchUpdate(NiUInt8 uiLockMask = NiDataStream::LOCK_WRITE);

    ///  Ends a batch update and unlocks all data streams.
    void EndBatchUpdate();
    //@}

    /**
        @name Primitive Data Member Access
    
        The growth values should be positive.

        Internally, this is ensured by setting them to one if the requested
        values are nonpositive.
    */
    //@{
    inline NiInt32 GetMaxPQuantity() const;
    inline void SetPGrowBy(NiInt32 iPGrowBy);
    inline NiInt32 GetPGrowBy() const;
    inline NiInt32 GetMaxVQuantity() const;
    inline void SetVGrowBy(NiInt32 iVGrowBy);
    inline NiInt32 GetVGrowBy() const;
    inline NiInt32 GetMaxTQuantity() const;
    inline void SetTGrowBy(NiInt32 iTGrowBy);
    inline NiInt32 GetTGrowBy() const;
    //@}

    /**
        The validity of a polygon handle may be tested with this function.
        
        Polygon management is transparent to the user. All polygon operations
        involve polygon handles, which are integer valued.
    */  
    bool IsValid(NiInt32 iPolygon) const;

    /**
        Polygon insertion.
        
        The number of vertices must be positive.  An
        optional triangle index array may be passed in.  If no array is passed
        in (number of triangles is zero and array is NULL), the requested
        polygon is assumed to be convex and a triangle fan is generated for
        the polygon.  If an array is requested, the requested polygon can be
        as complex as you like, even a collection of disjoint polygons.  The
        input array is a const pointer, this class copies the values and the
        caller is responsible for deleting the array in the event it was
        dynamically allocated.  The return value is an opaque handle for the
        created polygon.  See the comments for IsValid(...) regarding validity
        of the handle.  On insertion, all you get is storage for the vertices
        and their attributes.  You must fill them in with the Set* calls
        listed later in the class interface.
    */
    NiInt32 Insert(NiUInt16 usNumVertices,
        NiUInt16 usNumTriangles = 0,
        const NiUInt32* auiTriList = 0);

    /**
        Remove the polygon at the index specified.  

        @param iPolygon Index of the polygon to remove.

        @return The return value is 'true' iff the input polygon
            handle is valid.  After removal of one or more polygons, the 
            polygon handles that your application has stored for the remaining 
            polygons are still valid.  
        
        @note The handles are reused.  If you store a
            handle to a polygon, remove the polygon, and then insert a new
            polygon, the handle you stored might be valid, but it would be for
            a polygon you are unaware of.
    */
    bool Remove(NiInt32 iPolygon);

    /**
        Removes all polygons.  

        @note The handles are reused.  If you store a
            handle to a polygon, remove the polygon, and then insert a new
            polygon, the handle you stored might be valid, but it would be for
            a polygon you are unaware of.
    */
        void RemoveAll();

    /// Gets the number of polygons in the container.
    inline NiInt32 GetNumPolygons() const;

    /// Gets the number of vertices in the specified polygon.  
    /// @return The return value is positive for a valid polygon handle; 
    /// otherwise, the return value is zero.
    NiInt32 GetNumVertices(NiInt32 iPolygon) const;

    /// Gets the number of triangles in the specified polygon.  
    /// @return The return value is positive for a valid polygon handle; 
    /// otherwise, the return value is zero.
    NiInt32 GetNumTriangles(NiInt32 iPolygon) const;

    /**
        @name Set and Get Vertices and Attributes
        
        Set and get the polygon vertices.  
        
        The return Boolean value
        indicates the success of the operation.  It is false if the input
        polygon handle is invalid, if the input vertex index is out of range,
        or if the input number of texture sets is invalid.

        The vertices (x,y) are in normalized
        display coordinates (NDC); a point on the screen satisfies 0 <= x <= 1
        and 0 <= y <= 1.  You may use NDC values outside the unit square;
        for example, this is useful if you have GUI components that may be
        translated by the user on/off the screen.  The {Set,Get}Vertices
        functions require arrays with (at least) the same number of vertices
        that the polygon has.  The {Set,Get}Rectangle functions are for
        axis-aligned rectangles.
    
        If you have a known width w and height h, and you want to use screen
        coordinates (x,y) relative to w and h, you may set a vertex using
        SetVertex(polygon, vertex, NiPoint2(x/w, y/h)).  The ratios x/w and
        y/w should be computed using floating-point arithmetic.
    */
    //@{
    bool SetVertex(NiInt32 iPolygon, NiInt32 iVertex, const NiPoint2& kValue);
    bool GetVertex(NiInt32 iPolygon, NiInt32 iVertex, NiPoint2& kValue) const;
    bool SetVertices(NiInt32 iPolygon, const NiPoint2* akValue);
    bool GetVertices(NiInt32 iPolygon, NiPoint2* akValue) const;
    bool SetRectangle(NiInt32 iPolygon, float fLeft, float fTop, float fWidth,
        float fHeight);
    bool GetRectangle(NiInt32 iPolygon, float& fLeft, float& fTop, 
        float& fWidth, float& fHeight) const;
    //@}

    /**
        @name Set and Get the Vertex Normals
        
        The {Set,Get}Normals functions that
        take array inputs require arrays with (at least) the same number of
        normals as the number of polygon vertices.  The last {Set,Get}Normals
        pair is for axis-aligned rectangles.
    */
    //@{
    bool SetNormal(NiInt32 iPolygon, NiInt32 iVertex, const NiPoint3& kValue);
    bool GetNormal(NiInt32 iPolygon, NiInt32 iVertex, NiPoint3& kValue) const;
    bool SetNormals(NiInt32 iPolygon, const NiPoint3* akValue);
    bool SetNormals(NiInt32 iPolygon, const NiPoint3& kCommonValue);
    bool GetNormals(NiInt32 iPolygon, NiPoint3* akValue) const;
    bool SetNormals(
        NiInt32 iPolygon, 
        const NiPoint3& kUpperLeft,
        const NiPoint3& kLowerLeft, 
        const NiPoint3& kLowerRight,
        const NiPoint3& kUpperRight);
    bool GetNormals(
        NiInt32 iPolygon, 
        NiPoint3& kUpperLeft,
        NiPoint3& kLowerLeft,
        NiPoint3& kLowerRight,
        NiPoint3& kUpperRight) const;
    //@}

    /**
        @name Set and Get the Vertex Colors
        
        The {Set,Get}Colors functions that
        take array inputs require arrays with (at least) the same number of
        colors as the number of polygon vertices.  The last {Set,Get}Colors
        pair is for axis-aligned rectangles.
    */
    //@{
    bool SetColor(NiInt32 iPolygon, NiInt32 iVertex, const NiColorA& kValue);
    bool GetColor(NiInt32 iPolygon, NiInt32 iVertex, NiColorA& kValue) const;
    bool SetColors(NiInt32 iPolygon, const NiColorA* akValue);
    bool SetColors(NiInt32 iPolygon, const NiColorA& kCommonValue);
    bool GetColors(NiInt32 iPolygon, NiColorA* akValue) const;
    bool SetColors(
        NiInt32 iPolygon, 
        const NiColorA& kUpperLeft,
        const NiColorA& kLowerLeft, 
        const NiColorA& kLowerRight,
        const NiColorA& kUpperRight);
    bool GetColors(NiInt32 iPolygon, 
        NiColorA& kUpperLeft, 
        NiColorA& kLowerLeft,
        NiColorA& kLowerRight, 
        NiColorA& kUpperRight) const;
    //@}

    /**
        @name Set and Get the Texture Coordinates
        
        The {Set,Get}Textures functions
        that take array inputs require arrays with (at least) the same number
        of texture coordinates as the number of polygon vertices. The
        SetTextures function that takes four floating-point values is for
        axis-aligned rectangles for which the texture subimage mapped to it is
        an axis-aligned rectangle.  The last {Set,Get}Textures pair is for
        axis-aligned rectangles.
    */
    //@{
    bool SetTexture(NiInt32 iPolygon, NiInt32 iVertex, NiUInt16 usSet,
        const NiPoint2& kValue);
    bool GetTexture(NiInt32 iPolygon, NiInt32 iVertex, NiUInt16 usSet,
        NiPoint2& kValue) const;
    bool SetTextures(NiInt32 iPolygon, NiUInt16 usSet,
        const NiPoint2* akValue);
    bool GetTextures(NiInt32 iPolygon, NiUInt16 usSet, NiPoint2* akValue)
        const;
    bool SetTextures(
        NiInt32 iPolygon, 
        NiUInt16 usSet, 
        float fLeft,
        float fTop, 
        float fRight, 
        float fBottom);
    bool SetTextures(NiInt32 iPolygon,
        NiUInt16 usSet,
        const NiPoint2& kUpperLeft, 
        const NiPoint2& kLowerLeft,
        const NiPoint2& kLowerRight, 
        const NiPoint2& kUpperRight);
    bool GetTextures(NiInt32 iPolygon, 
        NiUInt16 usSet,
        NiPoint2& kUpperLeft, 
        NiPoint2& kLowerLeft, 
        NiPoint2& kLowerRight,
        NiPoint2& kUpperRight) const;
    //@}

    /**
        @name Access to Vertex and Attribute Arrays

        Access to vertex and attribute arrays may be accomplished by
        retrieving a particular stream using one of the methods provided.
        To determine the start offset into the vertex and attribute
        arrays, the GetPolyVertexRange can be used. To determine the start
        offset into the index array, the GetPolyIndexRange is used. Note,
        the application should unlock the stream before the next
        NiMeshScreenElements method call.
    
        @note It is not possible to modify the number of indices
        directly.  Instead, you must remove the old polygon and insert a new
        one.
    */
    //@{
    inline bool GetPolyVertexRange(NiUInt32 uiPolygon, NiUInt32& uiVertStart,
        NiUInt32& uiVertRange) const;
    inline bool GetPolyIndexRange(NiUInt32 uiPolygon, NiUInt32& uiIndexStart,
        NiUInt32& uiIndexRange) const;
    inline NiDataStream* GetVertexDataStream();
    inline const NiDataStream* GetVertexDataStream() const ;
    inline NiDataStream* GetColorDataStream();
    inline const NiDataStream* GetColorDataStream() const ;
    inline NiDataStream* GetNormalDataStream();
    inline const NiDataStream* GetNormalDataStream() const ;
    NiDataStream* GetTexCoordDataStream(unsigned int uiSet);
    const NiDataStream* GetTexCoordDataStream(unsigned int uiSet) const ;
    inline NiDataStream* GetIndexDataStream();
    inline const NiDataStream* GetIndexDataStream() const ;
    //@}

    /**
        @name Bounds Updating
        
        Updates the model-space bounding volume.
        
        The bound-needs-update flag is automatically set when polygons are
        inserted or removed and when vertices are modified.
    */
    //@{
    inline bool BoundNeedsUpdate() const;
    void UpdateBound();
    //@}

    /**
        Resizing to support the UI system.
        
        The input scales should be positive. The returned Boolean value is
        'true' iff the operation was successful (the inputs are positive).
    */
    bool Resize (float fXScale, float fYScale);
    
    /// Returns the number of texture sets
    virtual NiUInt16 GetTextureSets() const;

    /// Gets the active vertex count.
    inline NiUInt32 GetActiveVertexCount() const;
    
    /// Gets the active index count.
    inline NiUInt32 GetActiveIndexCount() const;
    
    /// Gets the active triangle count.
    inline NiUInt32 GetActiveTriangleCount() const;

    /// @cond EMERGENT_INTERNAL

    /// Resizes vertex and index streams
    void ResizeAttributeStreams(NiUInt16 usNewMaxQuantity);
    void ResizeIndexStream(NiUInt16 usNewMaxIQuantity);

    /// @name Methods used to convert from defunct NiScreenElements
    //@{
    void SetMaxPQuantity(NiUInt16 usQuantity);
    void SetPQuantity(NiUInt16 usQuantity);
    void SetMaxVQuantity(NiUInt16 usQuantity);
    void SetMaxTQuantity(NiUInt16 usQuantity);
    void ResetCacheData();
    void SetPolygonArray(void* pPolyArray, NiUInt16 usMaxPolys);
    void SetIndexerArray(NiUInt16* pkOldIndexer, NiUInt16 usMaxPolys);
    //@}

    /// @endcond

protected:
    /// disable copy constructor.
    NiMeshScreenElements(const NiMeshScreenElements&);

    /// Convert BGRA to NiColorA
    static NiColorA BGRAToColorA(const NiRGBA& kRGBA);
    /// Convert NiColorA to BGRA
    static NiRGBA ColorAToBGRA(const NiColorA& kColor);

private:

    // lock methods. These will return the cached data pointer if
    // in a batch update, and call the appropriate
    // data stream lock if not. Always use these and not the low-level
    // DataStream::Lock calls, since these respect batch updating.
    NiPoint3* LockVertexStream(NiUInt8 uiLockMask);
    void* LockColorStream(NiUInt8 uiLockMask);
    NiPoint3* LockNormalStream(NiUInt8 uiLockMask);
    NiPoint2* LockTexCoordStream(unsigned int uiSet, NiUInt8 uiLockMask);
    NiUInt16* LockIndexStream(NiUInt8 uiLockMask);

    // read-lock versions of the above, so that they can be
    // declared as const.
    const NiPoint3* LockVertexStreamRead() const;
    const void* LockColorStreamRead() const;
    const NiPoint3* LockNormalStreamRead() const;
    const NiPoint2* LockTexCoordStreamRead(unsigned int uiSet) const;
    const NiUInt16* LockIndexStreamRead() const;

    // unlocks the specified stream if not in a batch update. Does
    // nothing if in a batch update.
    void UnlockVertexStream(NiUInt8 uiLockMask);
    void UnlockColorStream(NiUInt8 uiLockMask);
    void UnlockNormalStream(NiUInt8 uiLockMask);
    void UnlockTexCoordStream(unsigned int uiSet, NiUInt8 uiLockMask);
    void UnlockIndexStream(NiUInt8 uiLockMask);

    // read-lock versions of the above so that they can be declared const
    void UnlockVertexStreamRead() const;
    void UnlockColorStreamRead() const;
    void UnlockNormalStreamRead() const;
    void UnlockTexCoordStreamRead(unsigned int uiSet) const;
    void UnlockIndexStreamRead() const;

    void SetAttributesRegionRange(NiUInt32 uiRange);
    void SetIndicesRange(NiUInt32 uiRange);
    void ResizeStream(NiDataStream *pkStream, size_t stElementSize,
        NiUInt16 usNewMaxQuantity);

    void SetInitialColorFormat(bool bForceFixedFunctionColors);
    
    /**
        Adds a new stream to the mesh being built. The ownership of the
        pvSource pointer remains the responsibility of the caller
        and is not managed by the new stream. The data contained
        in pvSource (if not null) will be copied into the 
        newly created stream.
    */
    inline NiDataStreamRef* AddStream(
        const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex, 
        NiDataStreamElement::Format eFormat, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage, 
        const void* pvSource = NULL,
        bool bForceToolDSCreate = false);

    /// Polygon array management.  The active number of polygons is
    /// m_usPQuantity.
    class Polygon : public NiMemObject
    {
    public:
        NiUInt16 m_usNumVertices;
        NiUInt16 m_usVOffset;  // offset into the vertex arrays
        NiUInt16 m_usNumTriangles;
        NiUInt16 m_usIOffset;  // offset into the index array
    };

    /// Get a polygon from a handle.  The input handle must be valid.
    Polygon& GetPolygon(NiInt32 iPolygon);
    inline const Polygon& GetPolygon(NiInt32 iPolygon) const;

    Polygon* m_pPolygon;
    NiUInt16* m_puiPIndexer;
    NiUInt16 m_usMaxPQuantity;
    NiUInt16 m_usPGrowBy;
    NiUInt16 m_usPQuantity;

    /// Vertex array management.  The active number of vertices is
    /// m_usVertices, including those not used due to removed polygons.
    NiUInt16 m_usMaxVQuantity;
    NiUInt16 m_usVGrowBy;

    /// Index array management.  The active number of indices is
    /// 3*m_usTriangles.
    NiUInt16 m_usMaxIQuantity;
    NiUInt16 m_usIGrowBy;

    /// The format that the color data will be stored in.
    NiDataStreamElement::Format m_eColorFormat;

    /// Set to 'true' whenever vertex positions are modified or when polygons
    /// are removed.
    bool m_bBoundNeedsUpdate;

    /// Indicates if the NiMeshScreenElement has normals
    bool m_bHasNormals;
    
    /// Indicates if the NiMeshScreenElement has colors
    bool m_bHasColors;
    
    /// The number of texture coordinate sets
    NiUInt32 m_uiNumTextureSets;

    static const NiUInt16 ms_usInvalid;

    // cached raw data for batch updating. Set by the BeginBatchUpdate method,
    // and re-set to NULL in EndBatchUpdate. Checking if m_bBatchLock is set
    // to true is the official method to determine if a batch update is in
    // progress.
    NiPoint3* m_pLockedVertexData;
    void* m_pLockedColorData;
    NiPoint3* m_pLockedNormalData;
    int m_iTextureDataCount;
    NiPoint2** m_ppLockedTextureData;
    NiUInt16* m_pLockedIndexData;
    NiUInt8 m_uiLockMask;
    bool m_bBatchLock;
};

NiSmartPointer(NiMeshScreenElements);
typedef NiTObjectArray<NiMeshScreenElementsPtr> NiMeshScreenElementsArray;
#include "NiMeshScreenElements.inl"

#endif //NIMESHSCREENELEMENTS_H
