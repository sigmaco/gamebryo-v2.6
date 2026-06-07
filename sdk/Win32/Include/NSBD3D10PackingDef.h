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

#ifndef NSBD3D10PACKINGDEF_H
#define NSBD3D10PACKINGDEF_H

#include "NiD3D10BinaryShaderLibLibType.h"

#include <NiShaderDeclaration.h>
#include <NiShader.h>

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10PackingDef : public NiMemObject
{
public:
    NSBD3D10PackingDef();
    ~NSBD3D10PackingDef();

    enum NiD3D10PackingDefType
    {
        NID3DPDT_FLOAT1     = 0,
        NID3DPDT_FLOAT2,
        NID3DPDT_FLOAT3,
        NID3DPDT_FLOAT4,
        NID3DPDT_UBYTECOLOR,
        NID3DPDT_UBYTE4,
        NID3DPDT_SHORT1,
        NID3DPDT_SHORT2,
        NID3DPDT_SHORT3,
        NID3DPDT_SHORT4,
        NID3DPDT_NORMSHORT1,
        NID3DPDT_NORMSHORT2,
        NID3DPDT_NORMSHORT3,
        NID3DPDT_NORMSHORT4,
        NID3DPDT_NORMPACKED3,
        NID3DPDT_PBYTE1,
        NID3DPDT_PBYTE2,
        NID3DPDT_PBYTE3,
        NID3DPDT_PBYTE4,
        NID3DPDT_FLOAT2H,
        //
        NID3DPDT_NORMUBYTE4,  // 4D normalized unsigned byte (val,val,val,val)
        NID3DPDT_NORMUSHORT2, // 2D normalized unsigned short (val,val,0,1)
        NID3DPDT_NORMUSHORT4, // 4D normalized unigned short (val,val,val,val)
        NID3DPDT_UDEC3,       // 3D unsigned 10 10 10 (val,val,val,1)
        NID3DPDT_NORMDEC3,    // 3D normalized signed 10 10 10 (val,val,val,1)
        NID3DPDT_FLOAT16_2,   // 2D signed 16-bit float (val,val,0,1)
        NID3DPDT_FLOAT16_4,   // 4D signed 16-bit float (val,val,val,val)
        // 
        NID3DPDT_COUNT,
        NID3DPDT_INVALID    = 0x7fffffff
    };

    // Name
    const char* GetName();
    void SetName(const char* pcName);

    bool GetFixedFunction();
    void SetFixedFunction(bool bFixedFunction);

    bool AddPackingEntry(unsigned int uiStream, unsigned int uiRegister, 
        unsigned int uiInput, NiD3D10PackingDefType eType,
        NiShaderDeclaration::ShaderParameterUsage eUsage, 
        unsigned int uiUsageIndex);

    static const char* GetTypeName(NiD3D10PackingDefType eType);
    static const char* GetParameterName(
        NiShaderDeclaration::ShaderParameter eParam);

    NiShaderDeclarationPtr GetVertexDescription();
    NiShaderDeclarationPtr GetVertexDesc_D3D10();

    NiShaderDeclaration::ShaderParameterType 
        ConvertPackingDefType(NiD3D10PackingDefType eType);
    NiShaderDeclaration::ShaderParameterType 
        ConvertPackingDefType_D3D10(NiD3D10PackingDefType eType);

    // *** begin Emergent internal use only
    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);
    // *** end Emergent internal use only

protected:
    class NSBD3D10PDEntry : public NiMemObject
    {
    public:
        NSBD3D10PDEntry();
        ~NSBD3D10PDEntry();

        unsigned int GetStream();
        unsigned int GetRegister();
        unsigned int GetInput();
        NiD3D10PackingDefType GetType();
        NiShaderDeclaration::ShaderParameterUsage GetUsage();
        unsigned int GetUsageIndex();

        void SetStream(unsigned int uiStream);
        void SetRegister(unsigned int uiRegister);
        void SetInput(unsigned int uiInput);
        void SetType(NiD3D10PackingDefType eType);
        void SetUsage(
            NiShaderDeclaration::ShaderParameterUsage eUsage);
        void SetUsageIndex(unsigned int uiUsageIndex);

        bool SaveBinary(NiBinaryStream& kStream);
        bool LoadBinary(NiBinaryStream& kStream);

    protected:
        unsigned int m_uiStream;
        unsigned int m_uiRegister;
        unsigned int m_uiInput;
        NiD3D10PackingDefType m_eType;
        NiShaderDeclaration::ShaderParameterUsage m_eUsage;
        unsigned int m_uiUsageIndex;
    };

    NSBD3D10PDEntry* GetFirstEntry();
    NSBD3D10PDEntry* GetNextEntry();

    void GetStreamInfo(unsigned int& uiStreamCount, 
        unsigned int& uiMaxStreamEntryCount);

    char* m_pcName;
    bool m_bFixedFunction;
    NiTListIterator m_kEntryIter;
    NiTPointerList<NSBD3D10PDEntry*> m_kEntryList;
};

#include "NSBD3D10PackingDef.inl"

#endif  //NSBD3D10PACKINGDEF_H
