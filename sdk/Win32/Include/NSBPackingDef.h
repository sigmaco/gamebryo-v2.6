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

#ifndef NSBPACKINGDEF_H
#define NSBPACKINGDEF_H

#include "NiBinaryShaderLibLibType.h"

#include <NiShaderDeclaration.h>
#include <NiShader.h>

#if defined(_PS3)
class NiPS3CgShaderProgram;
#endif

class NIBINARYSHADERLIB_ENTRY NSBPackingDef : public NiMemObject
{
public:
    NSBPackingDef();
    ~NSBPackingDef();

    enum NiD3DPackingDefType
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
        unsigned int uiInput, NiD3DPackingDefType eType,
        NiShaderDeclaration::ShaderParameterTesselator eTesselator, 
        NiShaderDeclaration::ShaderParameterUsage eUsage, 
        unsigned int uiUsageIndex);

    static const char* GetTypeName(NiD3DPackingDefType eType);
    static const char* GetParameterName(
        NiShaderDeclaration::ShaderParameter eParam);

    NiShaderDeclaration* GetShaderDeclaration(
        NiShader::Platform ePlatform);

    NiShaderDeclaration::ShaderParameterType 
        ConvertPackingDefType(NiD3DPackingDefType eType);

    // *** begin Emergent internal use only
#if defined(_PS3)
    void GenerateDeclarationFromVertexShader(
        NiPS3CgShaderProgram* pkVertexProgram);
#endif

    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);
    // *** end Emergent internal use only

protected:
    class NSBPDEntry : public NiMemObject
    {
    public:
        NSBPDEntry();
        ~NSBPDEntry();

        unsigned int GetStream();
        unsigned int GetRegister();
        unsigned int GetInput();
        NiD3DPackingDefType GetType();
        NiShaderDeclaration::ShaderParameterTesselator GetTesselator();
        NiShaderDeclaration::ShaderParameterUsage GetUsage();
        unsigned int GetUsageIndex();

        void SetStream(unsigned int uiStream);
        void SetRegister(unsigned int uiRegister);
        void SetInput(unsigned int uiInput);
        void SetType(NiD3DPackingDefType eType);
        void SetTesselator(
            NiShaderDeclaration::ShaderParameterTesselator eTesselator);
        void SetUsage(
            NiShaderDeclaration::ShaderParameterUsage eUsage);
        void SetUsageIndex(unsigned int uiUsageIndex);

        bool SaveBinary(NiBinaryStream& kStream);
        bool LoadBinary(NiBinaryStream& kStream);

    protected:
        unsigned int m_uiStream;
        unsigned int m_uiRegister;
        unsigned int m_uiInput;
        NiD3DPackingDefType m_eType;
        NiShaderDeclaration::ShaderParameterTesselator m_eTesselator;
        NiShaderDeclaration::ShaderParameterUsage m_eUsage;
        unsigned int m_uiUsageIndex;
    };

    NSBPDEntry* GetFirstEntry();
    NSBPDEntry* GetNextEntry();

    void GetStreamInfo(unsigned int& uiStreamCount, 
        unsigned int& uiMaxStreamEntryCount);

    char* m_pcName;
    bool m_bFixedFunction;
    NiTListIterator m_kEntryIter;
    NiTPointerList<NSBPDEntry*> m_kEntryList;
    bool m_bGenerated; // Was generated from VS
};

#include "NSBPackingDef.inl"

#endif  //NSBPACKINGDEF_H
