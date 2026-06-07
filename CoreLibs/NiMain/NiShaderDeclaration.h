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
#ifndef NISHADERDECLARATION_H
#define NISHADERDECLARATION_H
 
#include "NiObject.h"
#include "NiFixedString.h"

class NiRenderObject;
class NiGeometryData;
class NiRenderer;

//---------------------------------------------------------------------------
class NIMAIN_ENTRY NiShaderDeclaration : public NiObject
{
    NiDeclareRTTI;
public:
    // The ShaderParameter tell what data to use for the data fragment.
    // The NI_* values represent the data from the NiGeometryData object that
    // is being packed. 
    // SHADERPARAM_NI_POSITION      represents m_pkVertex
    // SHADERPARAM_NI_BLENDWEIGHT   represents SKINNED weights
    // SHADERPARAM_NI_NORMAL        represents m_pkNormal
    // SHADERPARAM_NI_COLOR         represents m_pkColor
    // SHADERPARAM_NI_TEXCOORD0     represents m_pkTexture set 0
    // SHADERPARAM_NI_TEXCOORD1     represents m_pkTexture set 1
    // SHADERPARAM_NI_TEXCOORD2     represents m_pkTexture set 2
    // SHADERPARAM_NI_TEXCOORD3     represents m_pkTexture set 3
    // SHADERPARAM_NI_TEXCOORD4     represents m_pkTexture set 4
    // SHADERPARAM_NI_TEXCOORD5     represents m_pkTexture set 5
    // SHADERPARAM_NI_TEXCOORD6     represents m_pkTexture set 6
    // SHADERPARAM_NI_TEXCOORD7     represents m_pkTexture set 7
    // SHADERPARAM_NI_TANGENT       represents m_pkNormal set 2
    // SHADERPARAM_NI_BINORMAL      represents m_pkNormal set 1
    // SHADERPARAM_DATA_ZERO        fills data w/ 0.0f (or 0, if not float)
    // SHADERPARAM_DATA_ONE         fills data w/ 1.0f (or 1, if not float)
    // SHADERPARAM_EXTRA_DATA_MASK  used to indicate 'extra data' be used
    enum ShaderParameter
    {
        SHADERPARAM_INVALID     = -1,
        SHADERPARAM_NI_POSITION =  0,       // Gamebryo Position
        SHADERPARAM_NI_POSITION0 =  0,      // Gamebryo Position
        SHADERPARAM_NI_POSITION1,           // Gamebryo Position
        SHADERPARAM_NI_POSITION2,           // Gamebryo Position
        SHADERPARAM_NI_POSITION3,           // Gamebryo Position
        SHADERPARAM_NI_POSITION4,           // Gamebryo Position
        SHADERPARAM_NI_POSITION5,           // Gamebryo Position
        SHADERPARAM_NI_POSITION6,           // Gamebryo Position
        SHADERPARAM_NI_POSITION7,           // Gamebryo Position
        SHADERPARAM_NI_BLENDWEIGHT,         // Gamebryo skinning blend weight
        SHADERPARAM_NI_BLENDINDICES,        // Gamebryo skinning blend indices
        SHADERPARAM_NI_NORMAL,              // Gamebryo normal
        SHADERPARAM_NI_COLOR,               // Gamebryo color
        SHADERPARAM_NI_TEXCOORD0,           // Gamebryo UVSet 0
        SHADERPARAM_NI_TEXCOORD1,           // Gamebryo UVSet 1
        SHADERPARAM_NI_TEXCOORD2,           // Gamebryo UVSet 2
        SHADERPARAM_NI_TEXCOORD3,           // Gamebryo UVSet 3
        SHADERPARAM_NI_TEXCOORD4,           // Gamebryo UVSet 4
        SHADERPARAM_NI_TEXCOORD5,           // Gamebryo UVSet 5
        SHADERPARAM_NI_TEXCOORD6,           // Gamebryo UVSet 6
        SHADERPARAM_NI_TEXCOORD7,           // Gamebryo UVSet 7
        SHADERPARAM_NI_TANGENT,             // Gamebryo Tangent Data
        SHADERPARAM_NI_BINORMAL,            // Gamebryo BiNormal Data
        SHADERPARAM_DATA_ZERO,              // Data set to 0
        SHADERPARAM_DATA_ONE,               // Data set to 1
        SHADERPARAM_NI_TRANSFORM0,
        SHADERPARAM_NI_TRANSFORM1,
        SHADERPARAM_NI_TRANSFORM2,

        // NOTE: SkipCount usage is indicated by ORing the skip count
        // w/ the SKIP_COUNT_MASK. For example, skip data
        // for 8 units would be used with a value of 0x40000008.
        SHADERPARAM_SKIP_COUNT_MASK = 0x40000000,
        // Insert new parameters here!
        // NOTE: ExtraData usage is indicated by ORing the extra data 
        // stream number w/ the EXTRA_DATA_MASK. For example, extra data
        // stream 1 would be used with a value of 0x80000001.
        SHADERPARAM_EXTRA_DATA_MASK = 0x80000000
    };

    //  The shader parameter type represents how to output the data
    enum ShaderParameterType
    {
        SPTYPE_FLOAT1 = 0,  // 1D float (val,0,0,1)
        SPTYPE_FLOAT2,      // 2D float (val,val,0,1)
        SPTYPE_FLOAT3,      // 3D float (val,val,val,1)
        SPTYPE_FLOAT4,      // 4D float
        SPTYPE_UBYTECOLOR,  // 4D unsigned bytes mapped to 0..1 range
        SPTYPE_UBYTE4,      // 4D unsigned byte (val,val,val,val)
        SPTYPE_SHORT2,      // 2D signed short (val,val,0,1)
        SPTYPE_SHORT4,      // 4D signed short (val,val,val,val)
        SPTYPE_NORMUBYTE4,  // 4D normalized unsigned byte (val,val,val,val)
        SPTYPE_NORMSHORT2,  // 2D normalized signed short (val,val,0,1)
        SPTYPE_NORMSHORT4,  // 4D normalized signed short (val,val,val,val)
        SPTYPE_NORMUSHORT2, // 2D normalized unsigned short (val,val,0,1)
        SPTYPE_NORMUSHORT4, // 4D normalized unigned short (val,val,val,val)
        SPTYPE_UDEC3,       // 3D unsigned 10 10 10 (val,val,val,1)
        SPTYPE_NORMDEC3,    // 3D normalized signed 10 10 10 (val,val,val,1)
        SPTYPE_FLOAT16_2,   // 2D signed 16-bit float (val,val,0,1)
        SPTYPE_FLOAT16_4,   // 4D signed 16-bit float (val,val,val,val)
        SPTYPE_NONE,        // No stream data
        SPTYPE_COUNT        // Count must appear last
    };
    
    // As of Gamebryo 2.3, usage is stored as a string. These enums are
    // converted via UsageToString. If you update these values, update the
    // SDMInit function so that the conversion tables work appropriately.
    enum ShaderParameterUsage
    {
        SPUSAGE_POSITION,
        SPUSAGE_BLENDWEIGHT,
        SPUSAGE_BLENDINDICES,
        SPUSAGE_NORMAL,
        SPUSAGE_PSIZE,
        SPUSAGE_TEXCOORD,
        SPUSAGE_TANGENT,
        SPUSAGE_BINORMAL,
        SPUSAGE_TESSFACTOR,
        SPUSAGE_POSITIONT,
        SPUSAGE_COLOR,
        SPUSAGE_FOG,
        SPUSAGE_DEPTH,
        SPUSAGE_SAMPLE,
        SPUSAGE_COUNT
    };

    enum ShaderParameterTesselator
    {
        SPTESS_DEFAULT,
        SPTESS_PARTIALU,
        SPTESS_PARTIALV,
        SPTESS_CROSSUV,
        SPTESS_UV,
        SPTESS_LOOKUP,
        SPTESS_LOOKUPPRESAMPLED,
        SPTESS_COUNT
    };

    //  Entry for a shader register
    class NIMAIN_ENTRY ShaderRegisterEntry: public NiMemObject
    {
    public:
        union
        {
            unsigned int m_uiPackingOffset;
            unsigned int m_uiPreferredStream;
        };
        ShaderParameter m_eInput;
        ShaderParameterType m_eType;
        NiFixedString m_kUsage;
        unsigned int m_uiUsageIndex;
        unsigned int m_uiExtraData;

        ShaderRegisterEntry();
    };

    //  Collection of entries for a single stream
    class NIMAIN_ENTRY ShaderRegisterStream: public NiMemObject
    {
    public:
        unsigned int m_uiEntryCount;
        ShaderRegisterEntry* m_pkEntries;
        unsigned int m_uiStride;
        bool m_bValid;
        bool m_bSPUStream;

        inline ShaderRegisterStream()
        {
            m_uiEntryCount = 0;
            m_pkEntries = 0;
            m_uiStride = 0;
            m_bValid = false;
            m_bSPUStream = false;
        };
    };

    //  Entry Access Functions
    virtual bool SetEntry(unsigned int uiStream, unsigned int uiEntry, 
        ShaderParameter eInput, ShaderParameterType eType, 
        const NiFixedString& kUsage, unsigned int uiIndex, 
        unsigned int uiExtraData);
    virtual bool SetEntry(unsigned int uiEntry,
        ShaderParameter eInput, ShaderParameterType eType, 
        unsigned int uiStream = 0);
    virtual bool RemoveEntry(unsigned int uiEntry, unsigned int uiStream = 0);
    inline const ShaderRegisterEntry* GetEntry(unsigned int uiEntry, 
        unsigned int uiStream = 0) const;
    bool ClearAllEntries(unsigned int uiStream);

    inline unsigned int GetStreamCount() const;
    inline unsigned int GetEntryCount(unsigned int uiStream) const;

    virtual void MarkAsModified();

    virtual ~NiShaderDeclaration();

    static const NiFixedString& UsageToString(ShaderParameterUsage eUsage);
    static ShaderParameterUsage StringToUsage(const NiFixedString& kUsage);

    static bool ShaderParameterToSemanticAndIndex(
        NiShaderDeclaration::ShaderParameter eParameter, 
        NiFixedString& kSemantic, NiUInt32& uiSemanticIndex,
        bool bSkinned = false);

    static unsigned int GetComponentCount(ShaderParameterType eType);

    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    static bool MergeInputSemantics(
        NiShaderDeclaration::ShaderRegisterEntry* pakGeomSemantics, 
        NiShaderDeclaration::ShaderRegisterEntry* pakMaterialSemantics, 
        unsigned int& uiGeomSemantics);
    // *** end Emergent internal use only ***

    static NiShaderDeclaration* Create(unsigned int uiMaxStreamEntryCount, 
        unsigned int uiStreamCount);
protected:
    NiShaderDeclaration();

    static NiFixedString ms_akUsageStrings[SPUSAGE_COUNT];

    unsigned int m_uiMaxStreamEntryCount;
    unsigned int m_uiStreamCount;
    ShaderRegisterStream* m_pkStreamEntries;
    bool m_bModified;

    bool Initialize(unsigned int uiMaxStreamEntryCount,
        unsigned int uiStreamCount);
};

NiSmartPointer(NiShaderDeclaration);

#include "NiShaderDeclaration.inl"

#endif  //#ifndef NISHADERDECLARATION_H

