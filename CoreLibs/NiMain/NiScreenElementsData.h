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

#ifndef NISCREENELEMENTSDATA_H
#define NISCREENELEMENTSDATA_H

#include "NiTriShapeData.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiScreenElementsData : public NiTriShapeData
{
public:
    NiDeclareRTTI;
    NiDeclareStream;

public:
    /*** begin Emergent internal use only ***/
    virtual ~NiScreenElementsData();

    inline int GetMaxPQuantity() const;
    inline int GetPGrowBy() const;
    inline int GetMaxVQuantity() const;
    inline int GetVGrowBy() const;
    inline int GetMaxTQuantity() const;
    inline int GetTGrowBy() const;

    // Polygon management is transparent to the user.  All polygon operations
    // involve polygon handles, which are integer valued.  The validity of a
    // polygon handle may be tested with this function.
    bool IsValid(int iPolygon) const;

    // Polygon removal.  The return value is 'true' iff the input polygon
    // handle is valid.  After removal of one or more polygons, the polygon
    // handles that your application has stored for the remaining polygons
    // are still valid.  NOTE:  The handles are reused.  If you store a
    // handle to a polygon, remove the polygon, and then insert a new
    // polygon, the handle you stored might be valid, but it would be for a
    // polygon you are unaware of.
    bool Remove(int iPolygon);
    void RemoveAll();

    // The number of polygons in the container.
    inline int GetNumPolygons() const;

    // The number of vertices in the specified polygon.  The return value is
    // positive for a valid polygon handle; otherwise, the return value is
    // zero.
    int GetNumVertices(int iPolygon) const;

    // The number of triangles in the specified polygon.  The return value is
    // positive for a valid polygon handle; otherwise, the return value is
    // zero.
    int GetNumTriangles(int iPolygon) const;

    // Get vertices and attributes.  The return Boolean value
    // indicates the success of the operation.  It is 'false' if the input
    // polygon handle is invalid, if the input vertex index is out of range,
    // or if the input number of texture sets is invalid.

    // Set and get the polygon vertices.  The vertices (x,y) are in normalized
    // display coordinates (NDC); a point on the screen satisfies 0 <= x <= 1
    // and 0 <= y <= 1.  You may use NDC values outside the unit square;
    // for example, this is useful if you have GUI components that may be
    // translated by the user on/off the screen.  The {Set,Get}Vertices
    // functions require arrays with (at least) the same number of vertices
    // that the polygon has.  The {Set,Get}Rectangle functions are for
    // axis-aligned rectangles.
    //
    // If you have a known width w and height h, and you want to use screen
    // coordinates (x,y) relative to w and h, you may set a vertex using
    // SetVertex(polygon, vertex, NiPoint2(x/w, y/h)).  The ratios x/w and
    // y/w should be computed using floating-point arithmetic.
    bool GetVertex(int iPolygon, int iVertex, NiPoint2& kValue) const;
    bool GetVertices(int iPolygon, NiPoint2* akValue) const;
    bool GetRectangle(int iPolygon, float& fLeft, float& fTop, float& fWidth,
        float& fHeight) const;

    // Set and get the vertex normals.  The {Set,Get}Normals functions that
    // take array inputs require arrays with (at least) the same number of
    // normals as the number of polygon vertices.  The last {Set,Get}Normals
    // pair is for axis-aligned rectangles.
    bool GetNormal(int iPolygon, int iVertex, NiPoint3& kValue) const;
    bool GetNormals(int iPolygon, NiPoint3* akValue) const;
    bool GetNormals(int iPolygon, NiPoint3& kUpperLeft, NiPoint3& kLowerLeft,
        NiPoint3& kLowerRight, NiPoint3& kUpperRight) const;

    // Set and get the vertex colors.  The {Set,Get}Colors functions that
    // take array inputs require arrays with (at least) the same number of
    // colors as the number of polygon vertices.  The last {Set,Get}Colors
    // pair is for axis-aligned rectangles.
    bool GetColor(int iPolygon, int iVertex, NiColorA& kValue) const;
    bool GetColors(int iPolygon, NiColorA* akValue) const;
    bool GetColors(int iPolygon, NiColorA& kUpperLeft, NiColorA& kLowerLeft,
        NiColorA& kLowerRight, NiColorA& kUpperRight) const;

    // Set and get the texture coordinates.  The {Set,Get}Textures functions
    // that take array inputs require arrays with (at least) the same number
    // of texture coordinates as the number of polygon vertices. The
    // SetTextures function that takes four floating-point values is for
    // axis-aligned rectangles for which the texture subimage mapped to it is
    // an axis-aligned rectangle.  The last {Set,Get}Textures pair is for
    // axis-aligned rectangles.
    bool GetTexture(int iPolygon, int iVertex, unsigned short usSet,
        NiPoint2& kValue) const;
    bool GetTextures(int iPolygon, unsigned short usSet, NiPoint2* akValue)
        const;
    bool GetTextures(int iPolygon, unsigned short usSet,
        NiPoint2& kUpperLeft, NiPoint2& kLowerLeft, NiPoint2& kLowerRight,
        NiPoint2& kUpperRight) const;

    // Access to vertex arrays.  If you hold onto a pointer value, and then
    // insert and/or remove polygons, the pointer values returned from these
    // functions might no longer point to what they used to.  If you modify
    // vertices or attributes, you must call the function
    // MarkAsChanged(NiGeometryData::FOOBAR_MASK), where FOOBAR is in
    // {VERTEX, NORMAL, COLOR, TEXTURE}.
    NiPoint3* GetVertices(int iPolygon);
    const NiPoint3* GetVertices(int iPolygon) const;
    NiPoint3* GetNormals(int iPolygon);
    const NiPoint3* GetNormals(int iPolygon) const;
    NiColorA* GetColors(int iPolygon);
    const NiColorA* GetColors(int iPolygon) const;
    NiPoint2* GetTextures(int iPolygon, unsigned short usSet);
    const NiPoint2* GetTextures(int iPolygon, unsigned short usSet) const;

    // Access to index arrays.  If you modify indices, you must call
    // MarkAsChanged(NiTriBasedGeomData::TRIANGLE_INDEX_MASK).  It is not
    // possible to modify the number of indices directly.  Instead, you must
    // remove the old polygon and insert a new one.
    unsigned short* GetIndices(int iPolygon);
    const unsigned short* GetIndices(int iPolygon) const;

    // used for conversion to NiRenderObjectScreenElements
    inline void* GetPolygonArray();
    inline unsigned short* GetIndexerArray();

    /*** end Emergent internal use only ***/
private:
    // Streaming support.
    NiScreenElementsData();

    // Polygon array management.  The active number of polygons is
    // m_usPQuantity.
    class Polygon : public NiMemObject
    {
    public:
        unsigned short m_usNumVertices;
        unsigned short m_usVOffset;  // offset into the vertex arrays
        unsigned short m_usNumTriangles;
        unsigned short m_usIOffset;  // offset into the index array
    };

    // Get a polygon from a handle.  The input handle must be valid.
    Polygon& GetPolygon(int iPolygon);
    inline const Polygon& GetPolygon(int iPolygon) const;

    Polygon* m_akPolygon;
    unsigned short* m_ausPIndexer;
    unsigned short m_usMaxPQuantity, m_usPGrowBy, m_usPQuantity;

    // Vertex array management.  The active number of vertices is
    // m_usVertices, including those not used due to removed polygons.
    unsigned short m_usMaxVQuantity, m_usVGrowBy;

    // Index array management.  The active number of indices is
    // 3*m_usTriangles.
    unsigned short m_usMaxIQuantity, m_usIGrowBy;

    static const unsigned short ms_usInvalid;
};

NiSmartPointer(NiScreenElementsData);
#include "NiScreenElementsData.inl"

#endif //NISCREENELEMENTSDATA_H
