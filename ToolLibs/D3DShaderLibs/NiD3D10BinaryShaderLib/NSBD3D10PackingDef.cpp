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
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10PackingDef.h"
#include <NSBD3D10Utility.h>
#include <NiD3D10ShaderFactory.h>

//---------------------------------------------------------------------------
bool NSBD3D10PackingDef::NSBD3D10PDEntry::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiStream);
    NiStreamSaveBinary(kStream, m_uiRegister);
    NiStreamSaveBinary(kStream, m_uiInput);

    unsigned int uiValue;
    uiValue = (unsigned int)m_eType;
    NiStreamSaveBinary(kStream, uiValue);
    NiStreamSaveBinary(kStream, uiValue);
    uiValue = (unsigned int)m_eUsage;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_uiUsageIndex);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10PackingDef::NSBD3D10PDEntry::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiStream);
    NiStreamLoadBinary(kStream, m_uiRegister);
    NiStreamLoadBinary(kStream, m_uiInput);

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eType = (NiD3D10PackingDefType)uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    NiStreamLoadBinary(kStream, uiValue);
    m_eUsage = (NiShaderDeclaration::ShaderParameterUsage)uiValue;
    
    NiStreamLoadBinary(kStream, m_uiUsageIndex);

    return true;
}
//---------------------------------------------------------------------------
NSBD3D10PackingDef::NSBD3D10PackingDef() :
    m_pcName(0), 
    m_bFixedFunction(false)
{
    m_kEntryList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBD3D10PackingDef::~NSBD3D10PackingDef()
{
    NSBD3D10PDEntry* pkEntry;
    NiTListIterator kIter = m_kEntryList.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList.RemoveAll();

    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
const char* NSBD3D10PackingDef::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
void NSBD3D10PackingDef::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
bool NSBD3D10PackingDef::GetFixedFunction()
{
    return m_bFixedFunction;
}
//---------------------------------------------------------------------------
void NSBD3D10PackingDef::SetFixedFunction(bool bFixedFunction)
{
    m_bFixedFunction = bFixedFunction;
}
//---------------------------------------------------------------------------
bool NSBD3D10PackingDef::AddPackingEntry(unsigned int uiStream, 
    unsigned int uiRegister, unsigned int uiInput, NiD3D10PackingDefType eType,
    NiShaderDeclaration::ShaderParameterUsage eUsage, 
    unsigned int uiUsageIndex)
{
    bool bCreatedEntry = false;

    // Check for the entry in the list...
    NSBD3D10PDEntry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if ((pkEntry->GetStream() == uiStream) &&
            (pkEntry->GetRegister() == uiRegister))
        {
            // Exists... just overwrite
            NSBD3D10Utility::Log("WARNING> PackingDef %s - Entry stream %d "
                "register %d being written over!\n", m_pcName, 
                uiStream, uiRegister);
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* WARNING: NSBD3D10PackingDef::AddPackingEntry\n"
                "    %s - overwriting stream %d, register %d\n", 
                m_pcName, uiStream, uiRegister);
            break;
        }
        pkEntry = GetNextEntry();
    }

    if (pkEntry == 0)
    {
        pkEntry = NiNew NSBD3D10PDEntry();
        if (!pkEntry)
            return false;
        bCreatedEntry = true;
    }

    pkEntry->SetStream(uiStream);
    pkEntry->SetRegister(uiRegister);
    pkEntry->SetInput(uiInput);
    pkEntry->SetType(eType);
    pkEntry->SetUsage(eUsage);
    pkEntry->SetUsageIndex(uiUsageIndex);

    if (bCreatedEntry)
        m_kEntryList.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
const char* NSBD3D10PackingDef::GetTypeName(
    NSBD3D10PackingDef::NiD3D10PackingDefType eType)
{
    switch (eType)
    {
    case NID3DPDT_FLOAT1:       return "Float1";
    case NID3DPDT_FLOAT2:       return "Float2";
    case NID3DPDT_FLOAT3:       return "Float3";
    case NID3DPDT_FLOAT4:       return "Float4";
    case NID3DPDT_UBYTECOLOR:   return "UByteColor";
    case NID3DPDT_UBYTE4:       return "UByte4";
    case NID3DPDT_SHORT1:       return "Short1";
    case NID3DPDT_SHORT2:       return "Short2";
    case NID3DPDT_SHORT3:       return "Short3";
    case NID3DPDT_SHORT4:       return "Short4";
    case NID3DPDT_NORMSHORT1:   return "NormShort1";
    case NID3DPDT_NORMSHORT2:   return "NormShort2";
    case NID3DPDT_NORMSHORT3:   return "NormShort3";
    case NID3DPDT_NORMSHORT4:   return "NormShort4";
    case NID3DPDT_NORMPACKED3:  return "NormPacked3";
    case NID3DPDT_PBYTE1:       return "PByte1";
    case NID3DPDT_PBYTE2:       return "PByte2";
    case NID3DPDT_PBYTE3:       return "PByte3";
    case NID3DPDT_PBYTE4:       return "PByte4";
    case NID3DPDT_FLOAT2H:      return "Float2H";
    default:                    return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
const char* NSBD3D10PackingDef::GetParameterName(
    NiShaderDeclaration::ShaderParameter eParam)
{
    if ((eParam & NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK) != 0)
    {
        return "ExtraData";
    }

    switch (eParam)
    {
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION0:
        return "NiPosition";
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT:
        return "NiBlendWeight";
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES:
        return "NiBlendIndices";
    case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
        return "NiNormal";
    case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
        return "NiColor";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
        return "NiTexCoord0";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
        return "NiTexCoord1";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
        return "NiTexCoord2";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
        return "NiTexCoord3";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
        return "NiTexCoord4";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
        return "NiTexCoord5";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
        return "NiTexCoord6";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
        return "NiTexCoord7";
    case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
        return "NiTangent";
    case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
        return "NiBinormal";
    default:
        return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
NiShaderDeclarationPtr NSBD3D10PackingDef::GetVertexDescription()
{
    return GetVertexDesc_D3D10();
}
//---------------------------------------------------------------------------
NiShaderDeclaration::ShaderParameterType 
    NSBD3D10PackingDef::ConvertPackingDefType(NiD3D10PackingDefType eType)
{
    return ConvertPackingDefType_D3D10(eType);
}
//---------------------------------------------------------------------------
bool NSBD3D10PackingDef::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);

    unsigned int uiValue = m_bFixedFunction ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    unsigned int uiCount = m_kEntryList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBD3D10PDEntry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if (pkEntry->SaveBinary(kStream))
            uiTestCount++;
        pkEntry = GetNextEntry();
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10PackingDef::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_bFixedFunction = (uiValue != 0);

    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBD3D10PDEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBD3D10PDEntry();
        NIASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kEntryList.AddTail(pkEntry);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBD3D10PackingDef::NSBD3D10PDEntry* NSBD3D10PackingDef::GetFirstEntry()
{
    NSBD3D10PDEntry* pkEntry = 0;

    m_kEntryIter = m_kEntryList.GetHeadPos();
    if (m_kEntryIter)
        pkEntry = m_kEntryList.GetNext(m_kEntryIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
NSBD3D10PackingDef::NSBD3D10PDEntry* NSBD3D10PackingDef::GetNextEntry()
{
    NSBD3D10PDEntry* pkEntry = 0;

    if (m_kEntryIter)
        pkEntry = m_kEntryList.GetNext(m_kEntryIter);

    return pkEntry;
}
//---------------------------------------------------------------------------
void NSBD3D10PackingDef::GetStreamInfo(unsigned int& uiStreamCount, 
    unsigned int& uiMaxStreamEntryCount)
{
    // Should use the MAX_STREAM here, but this value will suffice for D3D10.
    const unsigned int LOCAL_MAX_STREAM = 64;
    unsigned int auiStreamEntryCount[LOCAL_MAX_STREAM];

    uiStreamCount = 0;
    uiMaxStreamEntryCount = 0;

    unsigned int ui;

    // Initialize the 'seen' array
    for (ui = 0; ui < LOCAL_MAX_STREAM; ui++)
        auiStreamEntryCount[ui] = 0;
   
    // Now cycle through and count the streams
    NSBD3D10PDEntry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        unsigned int uiStream = pkEntry->GetStream();
        // Safety catch!
        NIASSERT(uiStream < LOCAL_MAX_STREAM);

        if (auiStreamEntryCount[uiStream] == 0)
            uiStreamCount++;
        auiStreamEntryCount[uiStream] += 1;

        pkEntry = GetNextEntry();
    }

    // Now, determine the max entry count
    for (ui = 0; ui < LOCAL_MAX_STREAM; ui++)
    {
        if (auiStreamEntryCount[ui] > uiMaxStreamEntryCount)
            uiMaxStreamEntryCount = auiStreamEntryCount[ui];
    }
}
//---------------------------------------------------------------------------
