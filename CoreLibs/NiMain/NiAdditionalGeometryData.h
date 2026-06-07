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
#ifndef NIADDITIONALGEOMETRYDATA_H
#define NIADDITIONALGEOMETRYDATA_H

#include "NiGeometryData.h"
#include "NiFlags.h"

//---------------------------------------------------------------------------
class NIMAIN_ENTRY NiAdditionalGeometryData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;
public:
    NiAdditionalGeometryData(unsigned short usVertexCount);
    NiAdditionalGeometryData(unsigned short usVertexCount, 
        unsigned int uiDataStreamCount);

    //  Data types
    enum
    {
        AGD_NITYPE_INVALID  = 0,    //  Invalid - place holder...
        AGD_NITYPE_FLOAT1   = 1,    //  Data is 1 float
        AGD_NITYPE_FLOAT2,          //          2 floats
        AGD_NITYPE_FLOAT3,          //          3
        AGD_NITYPE_FLOAT4,          //          4
        AGD_NITYPE_LONG1,           //  Data is 1 long
        AGD_NITYPE_LONG2,           //          2 longs
        AGD_NITYPE_LONG3,           //          2
        AGD_NITYPE_LONG4,           //          2
        AGD_NITYPE_ULONG1,          //  Data is 1 unsigned long
        AGD_NITYPE_ULONG2,          //          2 unsigned longs
        AGD_NITYPE_ULONG3,          //          2
        AGD_NITYPE_ULONG4,          //          2
        AGD_NITYPE_SHORT1,          //  Data is 1 short
        AGD_NITYPE_SHORT2,          //          2 shorts
        AGD_NITYPE_SHORT3,          //          3
        AGD_NITYPE_SHORT4,          //          4
        AGD_NITYPE_USHORT1,         //  Data is 1 unsigned short
        AGD_NITYPE_USHORT2,         //          2 unsigned shorts
        AGD_NITYPE_USHORT3,         //          3
        AGD_NITYPE_USHORT4,         //          4
        AGD_NITYPE_BYTE1,           //  Data is 1 byte
        AGD_NITYPE_BYTE2,           //          2 bytes
        AGD_NITYPE_BYTE3,           //          3
        AGD_NITYPE_BYTE4,           //          4
        AGD_NITYPE_UBYTE1,          //  Data is 1 unsigned byte
        AGD_NITYPE_UBYTE2,          //          2 unsigned bytes
        AGD_NITYPE_UBYTE3,          //          3
        AGD_NITYPE_UBYTE4,          //          4
        AGD_NITYPE_BLEND1,          //  Data is 1 blend weight
        AGD_NITYPE_BLEND2,          //          2 blend weights
        AGD_NITYPE_BLEND3,          //          3
        AGD_NITYPE_BLEND4,          //          4
        //  NOTE: Do NOT use the AGD_NITYPE_COUNT
        AGD_NITYPE_COUNT,           //  Count of NiTypes for data...
        AGD_NITYPE_CUSTOM 
    };

    ~NiAdditionalGeometryData();

    inline unsigned short GetVertexCount();

    // Note that NiAdditionalGeometryData will assume ownership over the
    // the input arrays and is responsible for deleting them.
    bool SetDataBlock(unsigned int uiIndex, unsigned char* pucData, 
        unsigned int uiTotalSize, bool bCopyData);
    const unsigned char* GetDataBlock(unsigned int uiIndex, 
        unsigned int& uiBlockSize);
    bool RemoveDataBlock(unsigned int uiIndex, 
        bool bRemoveAssociatedStreams = true);

    void SetDataBlockCount(unsigned int uiCount);
    unsigned int GetDataBlockCount();

    void SetDataStreamCount(unsigned int  uiCount);
    unsigned int GetDataStreamCount();

    bool SetDataStream(unsigned int uiStreamIndex, 
        unsigned int uiBlockIndex, unsigned int uiBlockOffset, 
        unsigned int uiType, unsigned short usCount, 
        unsigned int uiUnitSize, unsigned int uiStride);
    bool GetDataStream(unsigned int uiStreamIndex, 
        unsigned char*& pucData, unsigned int& uiType, 
        unsigned short& usCount, unsigned int& uiTotalSize, 
        unsigned int& uiUnitSize, unsigned int& uiStride);
    bool RemoveDataStream(unsigned int uiStreamIndex);
    
    bool SetConsistency(unsigned int uiStreamIndex, 
        NiGeometryData::Consistency eConsistency);
    bool GetConsistency(unsigned int uiStreamIndex,
        NiGeometryData::Consistency& eConsistency) const;
    bool SetKeep(unsigned int uiStreamIndex, bool bKeep);
    bool GetKeep(unsigned int uiStreamIndex, bool& bKeep) const;


    // *** begin Emergent internal use only ***
    // Bookkeeping
    inline void IncRevisionID();
    inline unsigned int GetRevisionID() const;
    inline void SetRevisionID(unsigned int uiRevID);
    // *** end Emergent internal use only ***

protected:
    NiAdditionalGeometryData();

    class NIMAIN_ENTRY NiAGDDataStream : public NiMemObject
    {
        NiDeclareFlags(unsigned char);
    public:
        NiAGDDataStream();
        
        unsigned int m_uiType;      //  Type of data
        unsigned int m_uiUnitSize;  //  Size of 'member' of data (in bytes)
        unsigned int m_uiTotalSize; //  Total size of data (in bytes)
        unsigned int m_uiStride;    //  Stride between data (in bytes)
        unsigned int m_uiBlockIndex;//  Index of NiAGDDataBlock to use
        unsigned int m_uiBlockOffset;//  Offset into NiAGDDataBlock for pointer
                                    //  to data

        inline void SetConsistency(NiGeometryData::Consistency eConsistency);
        NiGeometryData::Consistency GetConsistency() const;
        void SetKeep(bool bKeep);
        bool GetKeep() const;

        void LoadBinary(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        void GetViewerStrings(NiViewerStringsArray* pkStrings);
        bool IsEqual(const NiAGDDataStream* pkOther);

        void FillInByteSizeArray(unsigned int* pauiDataArray,
            unsigned int& uiIndex, unsigned int uiArraySize);

    protected:
        enum
        {
            AGD_KEEP_MASK         = 0x0001,
            AGD_MUTABLE           = 0x0002,
            AGD_STATIC            = 0x0003,
            AGD_VOLATILE          = 0x0004,
            AGD_CONSISTENCY_MASK  = 0x0006
        };

        inline NiGeometryData::Consistency ConvertToConsistency(
            unsigned char ucFlags) const; 
        inline unsigned char ConvertFromConsistency(
            NiGeometryData::Consistency eConsistency) const;
    };

    class NIMAIN_ENTRY NiAGDDataBlock : public NiMemObject
    {
    public:
        unsigned int m_uiDataBlockSize; //  Size of data block
        unsigned char* m_pucDataBlock;  //  Actual block of data.

        NiAGDDataBlock();
        void LoadBinary(NiStream& kStream, unsigned int uiBlockIdx,
            NiAGDDataStream* pkDataStreams, unsigned int uiNumStreams,
            unsigned short usVertexCount);
        void SaveBinary(NiStream& kStream, unsigned int uiBlockIdx,
            NiAGDDataStream* pkDataStreams, unsigned int uiNumStreams,
            unsigned short usVertexCount);

        bool IsEqual(const NiAGDDataBlock* pkOther);
        void GetViewerStrings(NiViewerStringsArray* pkStrings);
    };

    unsigned int m_uiRevID;
    unsigned short m_usVertexCount;
    unsigned int m_uiDataStreamCount;
    NiAGDDataStream* m_pkDataStreamEntries;

    NiTPrimitiveArray<NiAGDDataBlock*> m_aDataBlocks;
};

NiSmartPointer(NiAdditionalGeometryData);

//---------------------------------------------------------------------------
#include "NiAdditionalGeometryData.inl"

//---------------------------------------------------------------------------

#endif  //#ifndef NiAdditionalGeometryData_H
