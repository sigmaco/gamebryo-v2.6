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

#ifndef NIOUTPUTSTREAMDESCRIPTOR_H
#define NIOUTPUTSTREAMDESCRIPTOR_H

#include <NiRefObject.h>
#include <NiTArray.h>
#include <NiSmartPointer.h>
#include <NiPrimitiveType.h>
#include <NiFixedString.h>

class NiBinaryStream;

class NIMAIN_ENTRY NiOutputStreamDescriptor : public NiRefObject
{
public:
    enum DataType
    {
        DATATYPE_FLOAT = 0, 
        DATATYPE_UINT, 
        DATATYPE_INT, 
        DATATYPE_MAX
    };

    class NIMAIN_ENTRY VertexFormatEntry : public NiMemObject
    {
    public:
        VertexFormatEntry(NiUInt32 uiValue = 0);

        // A bunch of functions that exist so we can use the class in an 
        // NiTPrimitiveArray
        VertexFormatEntry(const VertexFormatEntry& kEntry);
        VertexFormatEntry& operator= (const VertexFormatEntry& kEntry);

        bool operator== (const VertexFormatEntry& kEntry) const;
        bool operator!= (const VertexFormatEntry& kEntry) const;

        // Streaming support
        bool LoadBinary(NiBinaryStream& kStream);
        bool SaveBinary(NiBinaryStream& kStream) const;

        // Data about the entry
        DataType m_eDataType;
        unsigned int m_uiComponentCount;
        NiFixedString m_kSemanticName;
        unsigned int m_uiSemanticIndex;
    };

    typedef NiTObjectArray<NiOutputStreamDescriptor::VertexFormatEntry> 
        VertexFormat;

public:
    NiOutputStreamDescriptor(NiUInt32 uiValue = 0);
    virtual ~NiOutputStreamDescriptor();

    // Stuff to enable NiTObjectArray<NiOutputStreamDescriptor>:
    NiOutputStreamDescriptor(const NiOutputStreamDescriptor& kDescriptor);
    NiOutputStreamDescriptor& operator= (
        const NiOutputStreamDescriptor& kDescriptor);
    bool operator== (const NiOutputStreamDescriptor& kDescriptor) const;
    bool operator!= (const NiOutputStreamDescriptor& kDescriptor) const;

    inline const NiFixedString& GetName() const;
    inline unsigned int GetMaxVertexCount() const;
    inline NiPrimitiveType::Type GetPrimType() const;
    inline const VertexFormat& GetVertexFormat() const;

    inline void SetName(const NiFixedString& kName);
    inline void SetMaxVertexCount(unsigned int uiMaxVertexCount);
    inline void SetPrimType(NiPrimitiveType::Type ePrimType);
    void SetVertexFormat(const VertexFormat& kVertexFormat);
    inline void AppendVertexFormat(const VertexFormatEntry& kVertexFormatEntry);

    // Streaming support
    void LoadBinary(NiBinaryStream& kStream);
    void SaveBinary(NiBinaryStream& kStream) const;

protected:
    NiFixedString m_kName;
    unsigned int m_uiMaxVertexCount;
    NiPrimitiveType::Type m_ePrimType;   // note: no strips here.
    VertexFormat m_kVertexFormat;
};

typedef NiPointer<NiOutputStreamDescriptor> NiOutputStreamDescriptorPtr;

typedef NiTObjectArray<NiOutputStreamDescriptor> NiOutputStreamDescriptorArray;

#include "NiOutputStreamDescriptor.inl"

#endif //#ifndef NIOUTPUTSTREAMDESCRIPTOR_H
