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

//---------------------------------------------------------------------------
#include "NSBD3D10PackingDef.h"

#include <NiShaderDeclaration.h>

//---------------------------------------------------------------------------
NiShaderDeclarationPtr NSBD3D10PackingDef::GetVertexDesc_D3D10()
{
    // Determine the stream count, and the max entry count
    unsigned int uiStreamCount = 0;
    unsigned int uiMaxStreamEntryCount = 0;

    GetStreamInfo(uiStreamCount, uiMaxStreamEntryCount);

    NiShaderDeclarationPtr spVertexDescription =
        NiShaderDeclaration::Create(uiMaxStreamEntryCount, uiStreamCount);

    if (spVertexDescription != NULL)
    {
        unsigned int uiEntry = 0;
        unsigned int uiStream = 0xFFFFFFFF;

        NSBD3D10PDEntry* pkEntry = GetFirstEntry();
        while (pkEntry)
        {
            // Add the entry to the vertex description.
            NiShaderDeclaration::ShaderParameterType eType;

            eType = ConvertPackingDefType(pkEntry->GetType());

            NIASSERT((unsigned int)eType != 0xffffffff);

            if (uiStream != pkEntry->GetStream())
            {
                uiStream = pkEntry->GetStream();
                uiEntry = 0;
            }

            if (pkEntry->GetUsage()
                == NiShaderDeclaration::SPUSAGE_COUNT)
            {
                spVertexDescription->SetEntry(uiEntry, 
                    (NiShaderDeclaration::ShaderParameter)
                    pkEntry->GetInput(), eType, pkEntry->GetStream());
            }
            else
            {
                spVertexDescription->SetEntry(pkEntry->GetStream(), uiEntry,
                    (NiShaderDeclaration::ShaderParameter)
                    pkEntry->GetInput(), eType,
                    NiShaderDeclaration::UsageToString(pkEntry->GetUsage()), 
                    pkEntry->GetUsageIndex(), 0);
            }

            uiEntry++;
            pkEntry = GetNextEntry();
        }
    }

    return spVertexDescription;
}
//---------------------------------------------------------------------------
NiShaderDeclaration::ShaderParameterType
    NSBD3D10PackingDef::ConvertPackingDefType_D3D10(
    NiD3D10PackingDefType eType)
{
    switch (eType)
    {
    case NID3DPDT_FLOAT1:
        return NiShaderDeclaration::SPTYPE_FLOAT1;
    case NID3DPDT_FLOAT2:
        return NiShaderDeclaration::SPTYPE_FLOAT2;
    case NID3DPDT_FLOAT3:
        return NiShaderDeclaration::SPTYPE_FLOAT3;
    case NID3DPDT_FLOAT4:
        return NiShaderDeclaration::SPTYPE_FLOAT4;
    case NID3DPDT_UBYTECOLOR:
        return NiShaderDeclaration::SPTYPE_UBYTECOLOR;
    case NID3DPDT_UBYTE4:
        return NiShaderDeclaration::SPTYPE_UBYTE4;
    case NID3DPDT_SHORT2:
        return NiShaderDeclaration::SPTYPE_SHORT2;
    case NID3DPDT_SHORT4:
        return NiShaderDeclaration::SPTYPE_SHORT4;
    case NID3DPDT_NORMUBYTE4:
        return NiShaderDeclaration::SPTYPE_NORMUBYTE4;
    case NID3DPDT_NORMSHORT2:
        return NiShaderDeclaration::SPTYPE_NORMSHORT2;
    case NID3DPDT_NORMSHORT4:
        return NiShaderDeclaration::SPTYPE_NORMSHORT4;
    case NID3DPDT_NORMUSHORT2:
        return NiShaderDeclaration::SPTYPE_NORMUSHORT2;
    case NID3DPDT_NORMUSHORT4:
        return NiShaderDeclaration::SPTYPE_NORMUSHORT4;
    case NID3DPDT_UDEC3:
        return NiShaderDeclaration::SPTYPE_UDEC3;
    case NID3DPDT_NORMDEC3:
        return NiShaderDeclaration::SPTYPE_NORMDEC3;
    case NID3DPDT_FLOAT16_2:
        return NiShaderDeclaration::SPTYPE_FLOAT16_2;
    case NID3DPDT_FLOAT16_4:
        return NiShaderDeclaration::SPTYPE_FLOAT16_4;
    }

    return (NiShaderDeclaration::ShaderParameterType)0xffffffff;
}
//---------------------------------------------------------------------------
