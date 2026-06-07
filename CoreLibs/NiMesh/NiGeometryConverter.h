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

//---------------------------------------------------------------------------
#ifndef NIGEOMETRYCONVERTER_H
#define NIGEOMETRYCONVERTER_H

#include "NiMemObject.h"
#include "NiMesh.h"
#include "NiTPointerMap.h"
#include "NiNode.h"

#include "NiGeometryData.h"
#include "NiSkinPartition.h"

class NiToolDataStream;
class NiDataStream;
class NiGeometry;
class NiTriShape;
class NiTriStrips;
class NiSkinInstance;
class NiSkinData;
class NiScreenElements;
class NiMeshScreenElements;
class NiLines;

/**
    Manages backwards compatibility by automatically converting
    NiGeometry-based assets to the NiMesh-based system.
*/
class NIMESH_ENTRY NiGeometryConverter : public NiMemObject
{
public:

    /// Type to hold the conversion result.
    enum ConversionResult
    {
        /// Successful conversion to an NiMesh based asset.
        SUCCESS,

        /// Successful conversion to an NiMesh based asset when a clone of the
        /// asset has already been successfully converted; returns a clone
        /// of the previously converted asset.
        SUCCESS_CLONED,

        /// Conversion has failed.
        FAIL
    };

    /**
        Constructor. 
        
        @param bForce32BitIndices If true, create 32-bit index buffers. If 
            false, 16-bit index buffers are used.
        @param bIsRuntime If true, NiDataStream objects will be 
            renderer-specific if a renderer has been created and its 
            NiDataStreamFactory initialized. If false, NiDataStream objects
            will be of type NiToolDataStream.
        @param bAttachModifiers If true, mesh modifiers created in the
            conversion process are both added and attached. If false, they
            are added but not attached.
    */
    NiGeometryConverter(
        bool bForce32BitIndices = true, bool bIsRuntime = false,
        bool bAttachModifiers = true);

    /// Destructor.
    ~NiGeometryConverter();

    /// Converts NiGeometry to a NiMesh.
    ConversionResult Convert(NiGeometry* pkGeometry, NiMesh* pkMesh);

    /// Sets auto-conversion of NiGeometry assets (true by default).
    static void SetAutoGeometryConversion(bool bEnable);
    
    /// Gets the state of auto-conversion of NiGeometry assets.
    static bool GetAutoGeometryConversion();

    /// Recursively converts NiGeometry objects to NiMesh objects.
    /// @return Number of objects that were converted.
    NiUInt32 RecursiveConvert(NiObject* pkNode, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects,
        NiUInt32 uiNodeIndex,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        NiUInt32 uiCurrentCount = 0);

    /// @cond EMERGENT_INTERNAL

    /// Processes materials
    static void ConvertMaterials(NiGeometry* pkGeom, NiMesh* pkMesh);

    /// Processes effect state, property state, and bound
    static void ConvertCore(NiGeometry* pkGeom, NiMesh* pkMesh);
    
    /// This method is called by the NiStream post processing
    /// system to convert a loaded NiGeometry object to
    /// an NiRenderObject object
    static void ConvertToNiMesh(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

    /// Removes all instances of NiGeometry-derived objects.
    static void RemoveNiGeometryObjects(NiAVObject* pkObject);

    /// Finalizes the process of asset conversion.
    static void Finalize(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

    /// Initializes and builds the regions for a stream reference.
    template <class T>
    static void BuildPartitions(NiDataStreamRef* pkStreamRef,
        NiUInt32 uiDestCount, const T* pkSrcData, NiUInt32 uiSrcCount, 
        NiSkinPartition::Partition* pkPartitions, NiUInt32 uiPartitionCount);

    /// @endcond

private:   

    /// Process collision.
    bool ConvertCollision();

    /// Processes index buffers.
    bool ConvertIndexBuffers();

    /// Caches the NiGeometry keep and consistency flags.
    void SetConverterStateFromGeometry();

    /// Processes positions.
    bool ConvertPositions();

    /// Processes normal, binormal, tangents.
    bool ConvertNBTs();

    /// Processes colors.
    bool ConvertColors();

    /// Processes texture coords.
    bool ConvertTexcoords();

    /// Processes additional geometry data.
    bool ConvertAGDs();

    /// Convert skin instance to attached mesh modifier.
    bool ConvertSkinInstance();

    /// Convert software skinning mesh data
    bool ConvertSoftwareSkinData();
    
    /// Converts screen elements.
    ConversionResult ConvertScreenElements();

    /// Converts NiTriShape index buffer to stream.
    bool CreateTriShapeIndexStream(NiTriShape* pkTriShape);

    /// Converts NiLines index buffer to stream.
    bool CreateLinesIndexStream(NiLines* pkLines);

    /// Converts Skinned index buffer to streams.
    bool CreateHardwareSkinnedStreams();

    /// Converts NiTriStrips index buffer to stream.
    bool CreateTriStripsIndexStream(NiTriStrips* pkTriStrips);
    
    /// Builds the index buffer for an indexed geometry asset.
    NiDataStreamRef* BuildIndexBuffer(
        NiUInt32 uiCount, 
        const NiUInt16* pusSrc, 
        NiUInt32 uiAccessMask, 
        bool bDefaultRegion = false);

    /// Builds the index buffer for list of tristrips, stitching them
    /// together into one continuous strip.
    NiDataStreamRef* BuildStriplistIndexBuffer(
        NiUInt32 uiCount, 
        const NiUInt16* pusSrc, 
        const NiUInt16 uiStripCount, 
        const NiUInt16* puiStripLengths, 
        NiUInt32 uiAccessMask, 
        bool bDefaultRegion = false);

    /// Build an index buffer for a skin partition
    bool BuildStriplistIndexBuffer(NiSkinPartition::Partition* pkPartition,
        void* pvIndexBuffer, NiUInt32 uiIndexOffset,
        NiUInt32& uiIndicesAdded);

    /// Converts consistency information to NiRenderObject format.
    NiUInt32 GetAccess(NiGeometryData::Keep eKeepMask);
    
    /// Sets the CPU write access to a unique type.
    inline void SetCPUWriteAccess(NiUInt32& uiAccess,
        NiDataStream::Access uiMask);
    
    /// Sets the CPU read access to a unique type.
    inline void SetCPUReadAccess(NiUInt32& uiAccess);

    /// Clears the shared data map. 
    /// @note This method should be called before a new conversion pass.
    inline void ClearGeometryCache();

    /// Returns true if the geometry has a NiGeomMorpherController on it.
    static bool GetIsMorphed(NiRenderObject* pkMesh);

    /// Initializes and builds the regions for a stream reference.
    template <class T>
    inline void BuildPartitions(NiDataStreamRef* pkStreamRef,
        const T* pkSrcData);
    
    /// If true, NiGeometry is converted upon loading. If false, old
    /// NiGeometry will simply be removed from the scene graph.
    static bool ms_bAutoGeometryConversion;

    typedef NiTPointerMap<NiGeometryData*, NiMesh*> SharedDataMap; 
    
    /// Map used for sharing mesh data.
    SharedDataMap m_kSharedDataMap;

    NiMesh* m_pkMesh;
    NiGeometry* m_pkGeom;
    NiSkinInstance* m_pkSkinInstance;
    NiSkinData* m_pkSkinData;
    NiSkinPartition* m_pkSkinPartition;
    NiSkinPartition::Partition* m_pkPartitions;
    NiCollisionObjectPtr m_spCollision;
    NiUInt32 m_uiKeepFlags;
    NiUInt32 m_uiConsistencyFlags;
    NiUInt32 m_uiRegionCount;
    NiUInt32 m_uiPartitionCount;
    NiUInt16 m_uiGeomVertexCount;
    NiUInt32 m_uiMeshVertexCount;
    bool m_bIsRuntime;
    bool m_bForce32BitIndices;
    bool m_bIsSkinned;
    bool m_bNiSkinInstance_HardwareSkinned;
    bool m_bNiSkinningMeshModifier_HardwareSkinned;
    bool m_bIsMorphed;
    bool m_bAttachModifiers;
    
};

#include "NiGeometryConverter.inl"
#endif
