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

#ifndef NITERRAINSECTORFILE_H
#define NITERRAINSECTORFILE_H

#include <NiBoxBV.h>

#include "NiTerrainLibType.h"

/** 
    The class is used to iterate over and write terrain sector files.
 */
class NITERRAIN_ENTRY NiTerrainSectorFile : public NiRefObject
{
public:

    /// Version type for storing the file version
    typedef NiUInt32 FileVersion;

    /// File header type for deciphering file information.
    struct FileHeader
    {
        /// Version of the file format used.
        FileVersion m_kVersion;

        /// Width of the block in vertices.
        NiUInt32 m_uiVertsPerBlock;

        /// Load.
        void LoadBinary(NiBinaryStream& kStream);

        /// Save.
        void SaveBinary(NiBinaryStream& kStream);
    };

    /// Present Data Bitfield.
    union PresentData
    {
        /// Place holder for accessing the bitfield as a whole
        NiUInt32 m_uiBitField;
        struct Data
        {
            /// Indicates if height data is present.
            NiUInt32 m_bHeights : 1;

            /// Indicates if normal data is present.
            NiUInt32 m_bNormals : 1;

            /// Indicates if tangent data is present.
            NiUInt32 m_bTangents : 1;

            /// Indicates if morph height data is present.
            NiUInt32 m_bMorphHeights : 1;

            /// Indicates if morph normal data is present.
            NiUInt32 m_bMorphNormals : 1;

            /// Indicates if morph tangent data is present.
            NiUInt32 m_bMorphTangents : 1;
        };
    };

    /// Block Header type.
    struct BlockHeader
    {
        /// Length of the data stored in this block (including this header).
        NiUInt32 m_ulLength;
        /// Number of bytes from the start of this header to the first child.
        NiUInt32 m_ulChildOffset;
        /// Bitfield representing what data streams are present for this block.
        PresentData m_kPresentData;

        /// Bounding radius of this block.
        float m_fBoundRadius;
        /// Bounding center of this block.
        NiPoint3 m_kBoundCenter;
        /// Sum of all verts in this file.
        NiPoint3 m_kBoundSumVerts;

        /// Bounding Box Volume Center.
        NiPoint3 m_kVolumeCenter;
        /// Bounding Box Volume direction 1.
        NiPoint3 m_kVolumeDirection1;
        /// Bounding Box Volume direction 2.
        NiPoint3 m_kVolumeDirection2;
        /// Bounding Box Volume direction 3.
        NiPoint3 m_kVolumeDirection3;
        /// Bounding Box Volume extent 1.
        float m_kVolumeExtent1;
        /// Bounding Box Volume extent 2.
        float m_kVolumeExtent2;
        /// Bounding Box Volume extent 3.
        float m_kVolumeExtent3;

        /// Load.
        void LoadBinary(NiBinaryStream& kStream);
        /// Save.
        void SaveBinary(NiBinaryStream& kStream);
    };

    /** 
        Identifiers for each type of stream capable of being stored.
        
        Currently, element size is assumed to be sizeof(float) for
        endian swapping purposes.
    */
    enum StreamType
    {
        /// Invalid or unspecified stream.
        STREAM_INVALID = -1,

        /// Height stream.
        STREAM_HEIGHT = 0,

        /// Normal stream.
        STREAM_NORMAL,

        /// Tangent stream.
        STREAM_TANGENT,

        /// Morph Height stream.
        STREAM_MORPH_HEIGHT,

        /// Morph Normal stream.
        STREAM_MORPH_NORMAL,

        /// Morph Tangent stream.
        STREAM_MORPH_TANGENT,

        /// Maximum number of streams per block.
        STREAM_MAX_NUMBER 
    };

    /// Data stream header.
    struct DataStreamHeader
    {
        /**
            Number of bytes stored in this stream including this header.

            Currently, element size is assumed to be sizeof(float) for
            endian swapping purposes.
        */
        NiUInt32 m_ulLength;
        /// The type of stream this data describes.
        StreamType m_kStreamType;
        /// The number of bytes per vertex stored in this stream.
        NiUInt32 m_uiObjectSize;

        /// Load.
        void LoadBinary(NiBinaryStream& kStream);
        /// Save.
        void SaveBinary(NiBinaryStream& kStream);
    };

    /**
        Create a NiTerrainSectorFile object capable of traversing the 
        data stored in the given file. 

        @param kSectorFile the path to a sector file to open for iteration
        @param bWriteAccess set to true if we are going to output to a file. 
            can only read OR write at once, not both. 

        @return True if the file could be opened, the version has been 
            recognized, and the first block appears intact, otherwise false.
    */
    static NiTerrainSectorFile *Open(
        const char* kSectorFile,
        bool bWriteAccess = false);

    /// Destructor.
    virtual ~NiTerrainSectorFile();

    /**
        Return the version of the file being written/read.
    */
    virtual FileVersion GetFileVersion() const;

    /**
        Return true if file is ready to be written/read.
    */
    virtual bool IsReady() const;

    /**
        Return true if the file has been opened for write access.
    */
    virtual bool IsWritable() const;

    /**
        Set the number of vertices per block in this sector. 

        @note This function must be called BEFORE WriteFileHeader().
        @note Can only be called on a writable file

        @param uiVertsPerBlock the number of vertices per block in this sector.
    */
    virtual void SetBlockWidthInVerts(NiUInt32 uiVertsPerBlock);

    /**
        Write the file header.

        @note This function must only be called once, and called BEFORE
            any calls to WriteBlock().
        @note Can only be called on a writable file.
    */
    virtual void WriteFileHeader();

    /**
        Write the current block's data to the file.       

        @note This function must be called for each block to be written to the
            file, all stream data, bounding data, etc must be set before 
            this function is called. By calling this function all current block
            data is reset and must be set again before the next call to 
            WriteBlock().
        @note Can only be called on a writable file.
    */
    virtual void WriteBlock();

    /**
        Set the bounding information of the current block.

        @param kBound the bounding sphere around the block.
        @param kSumVerts the sum of all vert positions in this block.
        @param kVolume the bounding box volume of this block.

        @note Can only be called on a writable file.
    */
    virtual void SetBlockBounds(NiBound kBound, NiPoint3 kSumVerts, 
        NiBoxBV kVolume);

    /**
        Set the data for a particular stream of information in the current
        block.

        @note Can only be called on a writable file.

        @param kStreamID the ID of the stream to set the data for.
        @param uiObjectSize the size in bytes of the data for each vertex.
        @param pfData the data stream.
        @param uiDataLength the total length in bytes of the stream.
    */
    virtual void SetStreamData(StreamType kStreamID, NiUInt32 uiObjectSize,
        float* pfData, NiUInt32 uiDataLength);

    /**
        Set the data for the height stream of the current block. Assumes
        one float per vertex.

        See SetStreamData.

        @param pfData the data stream.
        @param uiDataLength the total length in bytes of the stream.
    */
    inline void SetHeightData(float* pfData, NiUInt32 uiDataLength);

    /**
        Set the data for the normal stream of the current block.
        
        Assumes two floats per vertex. (Z is assumed to be "1.0f" and the
        vector is then normalized during use).

        See SetStreamData.

        @param pfData The data stream
        @param uiDataLength The total length in bytes of the stream
    */
    inline void SetNormalData(float* pfData, NiUInt32 uiDataLength);

    /**
        Set the data for the tangent stream of the current block.
        
        Assumes two floats per vertex. (Z is assumed to be "1.0f" and the
        vector is then normalized during use)

        See SetStreamData.

        @param pfData the data stream
        @param uiDataLength the total length in bytes of the stream
    */
    inline void SetTangentData(float* pfData, NiUInt32 uiDataLength);

    /**
        Set the data for the morphing height stream of the current block. 

        Assumes one float per vertex. 

        See SetStreamData.

        @param pfData the data stream
        @param uiDataLength the total length in bytes of the stream
    */
    inline void SetMorphHeightData(float* pfData, NiUInt32 uiDataLength);

    /**
        Set the data for the morphing normal stream of the current block. 

        Assumes two floats per vertex. (Z is assumed to be "1.0f" and the 
        vector is then normalized during use)

        See SetStreamData.

        @param pfData the data stream
        @param uiDataLength the total length in bytes of the stream
    */
    inline void SetMorphNormalData(float* pfData, NiUInt32 uiDataLength);

    /**
        Set the data for the morphing tangent stream of the current block. 

        Assumes two floats per vertex. (Z is assumed to be "1.0f" and the 
        vector is then normalized during use)

        See SetStreamData.

        @param pfData The data stream.
        @param uiDataLength The total length in bytes of the stream.
    */
    inline void SetMorphTangentData(float* pfData, NiUInt32 uiDataLength);
   
    /**
        Return the number of vertices per block in this file
    */
    virtual NiUInt32 GetBlockWidthInVerts() const;

    /**
        Read in the next block of information from the file

        @note Can only be called on a file opened for read access

        @return True if the next block was successfully read. 
    */
    virtual bool NextBlock();

    /**
        Jump to the [iChildID]th child of the current block

        @note Can only be called on a file opened for read access
        @note Can be useful for jumping to a specific level of detail region 
            of the file

        @return True if the block was successfully read, false otherwise.
    */
    virtual bool PushBlock(int iChildID);

    /**
        Jump to the parent of the current block

        @note Can only be called on a file opened for read access

        @note Using this function after reading a LOD region of the file 
            (i.e. sequentially reading more than the 4 children of the parent 
            block) will cause any further GetBlockID calls to return
            incorrect information

        @return True if the block was successfully read. 
    */
    virtual bool PopBlock();

    /**
        Get the data for a particular stream of information in the current
        block.

        @note Can only be called on a readable file

        @param kStreamID The ID of the stream to set the data for.
        @param uiDataLength A reference to a variable to place the length of 
            the stream in.

        @return A pointer to the data stored in the stream.
    */
    virtual float* GetStreamData(StreamType kStreamID, NiUInt32& uiDataLength);

    /**
        Get the data for the height stream in the current
        block.

        See GetStreamData.

        @note Can only be called on a readable file.

        @return A pointer to the data stored in the stream.
    */
    inline float* GetHeightData(NiUInt32& uiDataLength);

    /**
        Get the data for the normal stream in the current block.

        See GetStreamData.

        @note Can only be called on a readable file.

        @return A pointer to the data stored in the stream.
    */
    inline float* GetNormalData(NiUInt32& uiDataLength);

    /**
        Get the data for the tangent stream in the current
        block.

        See GetStreamData.

        @note Can only be called on a readable file.

        @return A pointer to the data stored in the stream.
    */
    inline float* GetTangentData(NiUInt32& uiDataLength);

    /**
        Get the data for the morphing height stream in the current
        block.

        See GetStreamData.

        @note Can only be called on a readable file.

        @return A pointer to the data stored in the stream.
    */
    inline float* GetMorphHeightData(NiUInt32& uiDataLength);

    /**
        Get the data for the morphing normal stream in the current
        block.

        See GetStreamData.

        @note Can only be called on a readable file.

        @return A pointer to the data stored in the stream.
    */
    inline float* GetMorphNormalData(NiUInt32& uiDataLength);

    /**
        Get the data for the morphing tangent stream in the current
        block.

        See GetStreamData.

        @note Can only be called on a readable file.

        @return A pointer to the data stored in the stream.
    */
    inline float* GetMorphTangentData(NiUInt32& uiDataLength);

    /**
        Return the number of blocks that were stored before the current 
        block

        See PopBlock
    */
    inline NiUInt32 GetBlockID() const;

    /**
        Return the level of detail region that this block resides in. 
        (0 = Lowest detail)
    */
    inline NiUInt32 GetBlockLevel() const;

    /**
        Return the bounding sphere of the current block.
    */
    virtual NiBound GetBlockBound() const;

    /**
        Return the sum of the positions of each vertex in the current block.
    */
    virtual NiPoint3 GetBlockSumVerts() const;

    /**
        Return the bounding box volume of the current block.
    */
    virtual NiBoxBV GetBlockBoundVolume() const;

    /**
        function returning the file version that is currently in use
    */
    static FileVersion GetCurrentVersion();

protected:

    /**
        Constructor for this sector file iterator

        @param kSectorFile the sector file to attempt to open
        @param kAccessMode a member of NiFile::OpenMode (either read or write)
    */
    NiTerrainSectorFile(
        const char* kSectorFile,
        NiFile::OpenMode kAccessMode);

    /**
        Initialize the class by opening the file and reading the first couple
        of headers. 

        @return true if the class was successfully initialized.
    */
    virtual bool Initialize();

    /// The file version this class is capable of reading/writing
    static const FileVersion ms_kFileVersion1 = FileVersion(1);
    static const FileVersion ms_kFileVersion = FileVersion(2);

    // File data:
    /// The file object to access the file through
    NiFile *m_pkFile;
    /// The access mode used to open this file
    NiFile::OpenMode m_kAccessMode;
    /// The current position in the file
    NiUInt32 m_ulFilePosition;
    /// The header present at the beginning of this file
    FileHeader m_kFileHeader;
    /// The name of the file that was opened
    NiString m_kSectorFile;

    /// Iteration Stack/Queue - Stack in read mode, Queue in write mode 
    NiTPointerList<NiUInt32> m_kPositionStack;

    // Current block data:
    /// The number of blocks stored before this block in the file
    NiInt32 m_iCurrentBlockID;
    /// The LOD region of the file that this block resides in
    NiInt32 m_iCurrentBlockLevel;
    /// The BlockID that this LOD region began
    NiInt32 m_iCurrentBlockLevelIndex;
    /// The current block's header
    BlockHeader m_kCurrentBlockHeader;

    /// Array of data stream headers for this block
    DataStreamHeader m_akStreamHeader[STREAM_MAX_NUMBER];

    /// Array of pointers to the data for each stream in this block
    char *m_apcStreamData[STREAM_MAX_NUMBER];
};

NiSmartPointer(NiTerrainSectorFile);

#include "NiTerrainSectorFile.inl"

#endif // NITERRAINSECTORFILE_H
