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

// Precompiled Header
#include "NiMainPCH.h"
#include "NiRenderObject.h"
#include "NiMaterialInstance.h"
#include "NiMath.h"
#include "NiShaderDeclaration.h"
#include "NiCommonSemantics.h"
#include "NiRenderer.h"
#include "NiRenderObjectMaterialOption.h"

NiFixedString NiShaderDeclaration::ms_akUsageStrings[SPUSAGE_COUNT];

NiImplementRTTI(NiShaderDeclaration, NiObject);
//---------------------------------------------------------------------------
NiShaderDeclaration::NiShaderDeclaration() :
    m_uiMaxStreamEntryCount(0),
    m_uiStreamCount(0),
    m_pkStreamEntries(NULL),
    m_bModified(false)
{
}
//---------------------------------------------------------------------------
NiShaderDeclaration::~NiShaderDeclaration()
{
    if (m_pkStreamEntries)
    {
        for (unsigned int ui = 0; ui < m_uiStreamCount; ui++)
        {
            ShaderRegisterStream* pkStream = &(m_pkStreamEntries[ui]);
            if (pkStream)
                NiDelete [] pkStream->m_pkEntries;
        }
        NiDelete[] m_pkStreamEntries;
    }
    m_pkStreamEntries = 0;
}
//---------------------------------------------------------------------------
NiShaderDeclaration::ShaderRegisterEntry::ShaderRegisterEntry() :
    m_uiPackingOffset(0),
    m_eInput(SHADERPARAM_INVALID),
    m_eType(SPTYPE_NONE),
    m_uiUsageIndex(0),
    m_uiExtraData(0)
{
    m_kUsage = UsageToString(SPUSAGE_POSITION);
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::_SDMInit()
{
    // Establish the mapping table.
    ms_akUsageStrings[SPUSAGE_POSITION] = NiCommonSemantics::POSITION(); 
    ms_akUsageStrings[SPUSAGE_BLENDWEIGHT] = NiCommonSemantics::BLENDWEIGHT();
    ms_akUsageStrings[SPUSAGE_BLENDINDICES] = 
        NiCommonSemantics::BLENDINDICES(); 
    ms_akUsageStrings[SPUSAGE_NORMAL] = NiCommonSemantics::NORMAL(); 
    ms_akUsageStrings[SPUSAGE_PSIZE] = NiCommonSemantics::PSIZE(); 
    ms_akUsageStrings[SPUSAGE_TEXCOORD] = NiCommonSemantics::TEXCOORD(); 
    ms_akUsageStrings[SPUSAGE_TANGENT] = NiCommonSemantics::TANGENT(); 
    ms_akUsageStrings[SPUSAGE_BINORMAL] = NiCommonSemantics::BINORMAL(); 
    ms_akUsageStrings[SPUSAGE_TESSFACTOR] = NiCommonSemantics::TESSFACTOR(); 
    ms_akUsageStrings[SPUSAGE_POSITIONT] = NiCommonSemantics::POSITIONT(); 
    ms_akUsageStrings[SPUSAGE_COLOR] = NiCommonSemantics::COLOR(); 
    ms_akUsageStrings[SPUSAGE_FOG] = NiCommonSemantics::FOG(); 
    ms_akUsageStrings[SPUSAGE_DEPTH] = NiCommonSemantics::DEPTH(); 
    ms_akUsageStrings[SPUSAGE_SAMPLE] = NiCommonSemantics::SAMPLE(); 
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::_SDMShutdown()
{
    for (unsigned int ui = 0; ui < SPUSAGE_COUNT; ++ui)
    {
        ms_akUsageStrings[ui] = 0;
    }
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::SetEntry(unsigned int uiEntry, 
    ShaderParameter eInput, ShaderParameterType eType, unsigned int uiStream)
{
    ShaderParameterUsage eUsage;
    unsigned int uiIndex = 0; 

    switch (eInput)
    {
    case SHADERPARAM_NI_POSITION0:
        eUsage = SPUSAGE_POSITION;
        break;
    case SHADERPARAM_NI_POSITION1:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 1;
        break;
    case SHADERPARAM_NI_POSITION2:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 2;
        break;
    case SHADERPARAM_NI_POSITION3:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 3;
        break;
    case SHADERPARAM_NI_POSITION4:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 4;
        break;
    case SHADERPARAM_NI_POSITION5:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 5;
        break;
    case SHADERPARAM_NI_POSITION6:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 6;
        break;
    case SHADERPARAM_NI_POSITION7:
        eUsage = SPUSAGE_POSITION;
        uiIndex = 7;
        break;
    case SHADERPARAM_NI_BLENDWEIGHT:
        eUsage = SPUSAGE_BLENDWEIGHT;
        break;
    case SHADERPARAM_NI_BLENDINDICES:
        eUsage = SPUSAGE_BLENDINDICES;
        break;
    case SHADERPARAM_NI_NORMAL:
        eUsage = SPUSAGE_NORMAL;
        break;
    case SHADERPARAM_NI_COLOR:
        eUsage = SPUSAGE_COLOR;
        break;
    case SHADERPARAM_NI_TEXCOORD0:
        eUsage = SPUSAGE_TEXCOORD;
        break;
    case SHADERPARAM_NI_TEXCOORD1:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 1;
        break;
    case SHADERPARAM_NI_TEXCOORD2:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 2;
        break;
    case SHADERPARAM_NI_TEXCOORD3:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 3;
        break;
    case SHADERPARAM_NI_TEXCOORD4:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 4;
        break;
    case SHADERPARAM_NI_TEXCOORD5:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 5;
        break;
    case SHADERPARAM_NI_TEXCOORD6:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 6;
        break;
    case SHADERPARAM_NI_TEXCOORD7:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 7;
        break;
    case SHADERPARAM_NI_TANGENT:
        eUsage = SPUSAGE_TANGENT;
        break;
    case SHADERPARAM_NI_BINORMAL:
        eUsage = SPUSAGE_BINORMAL;
        break;
    case SHADERPARAM_NI_TRANSFORM0:
#ifdef _XENON
        eUsage = SPUSAGE_POSITION;
        uiIndex = 1;
#else
        eUsage = SPUSAGE_TEXCOORD;
#endif
        break;
    case SHADERPARAM_NI_TRANSFORM1:
#ifdef _XENON
        eUsage = SPUSAGE_POSITION;
        uiIndex = 2;
#else
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 1;
#endif
        break;
    case SHADERPARAM_NI_TRANSFORM2:
#ifdef _XENON
        eUsage = SPUSAGE_POSITION;
        uiIndex = 3;
#else
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 2;
#endif
        break;
    default:
        NIASSERT(!"Unsupported ShaderParameter type - "
            "try other SetEntry function");
        return false;
    }

    return SetEntry(uiStream, uiEntry, eInput, eType, UsageToString(eUsage), 
        uiIndex, 0);
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::SetEntry(unsigned int uiStream,
    unsigned int uiEntry, ShaderParameter eInput,
    ShaderParameterType eType, const NiFixedString& kUsage,
    unsigned int uiIndex, unsigned int uiExtraData)
{
    NIASSERT(m_pkStreamEntries);
    
    //  Check entry slot
    if (uiEntry >= m_uiMaxStreamEntryCount)
        return false;

    //  Check stream validity
    if (uiStream >= m_uiStreamCount)
        return false;

    //  Set it
    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    NIASSERT(pkStream);
    ShaderRegisterEntry* pkEntry = &(pkStream->m_pkEntries[uiEntry]);
    NIASSERT(pkEntry);

    if ((pkEntry->m_eInput != eInput) ||
        (pkEntry->m_eType != eType) ||
        (pkEntry->m_kUsage != kUsage) ||
        (pkEntry->m_uiUsageIndex != uiIndex) ||
        (pkEntry->m_uiExtraData != uiExtraData))
    {
        pkEntry->m_eInput = eInput;
        pkEntry->m_eType = eType;
        pkEntry->m_kUsage = kUsage;
        pkEntry->m_uiUsageIndex = uiIndex;
        pkEntry->m_uiExtraData = uiExtraData;

        pkStream->m_uiEntryCount = NiMax((int)(uiEntry + 1),
            (int)(pkStream->m_uiEntryCount));

        //  Force blend weights to output float 3
        if (eInput == SHADERPARAM_NI_BLENDWEIGHT)
            pkEntry->m_eType = SPTYPE_FLOAT3;
        MarkAsModified();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::RemoveEntry(unsigned int uiEntry, 
    unsigned int uiStream)
{
    NIASSERT(m_pkStreamEntries);
    
    //  Check entry slot
    if (uiEntry >= m_uiMaxStreamEntryCount)
        return false;

    //  Check stream validity
    if (uiStream >= m_uiStreamCount)
        return false;

    //  Set it
    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    NIASSERT(pkStream);
    ShaderRegisterEntry* pkEntry = &(pkStream->m_pkEntries[uiEntry]);
    NIASSERT(pkEntry);

    //  Set it to invalid
    pkEntry->m_eInput = SHADERPARAM_INVALID;
    pkEntry->m_eType = SPTYPE_NONE;
    MarkAsModified();

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::ClearAllEntries(unsigned int uiStream)
{
    NIASSERT(m_pkStreamEntries);
    
    //  Check stream validity
    if (uiStream >= m_uiStreamCount)
        return false;

    //  Set it
    ShaderRegisterEntry* pkEntry;

    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    NIASSERT(pkStream);
    for (unsigned int ui = 0; ui < m_uiMaxStreamEntryCount; ui++)
    {
        pkEntry = &(pkStream->m_pkEntries[ui]);
        NIASSERT(pkEntry);
        //  Set it to invalid
        pkEntry->m_eInput = SHADERPARAM_INVALID;
        pkEntry->m_eType = SPTYPE_NONE;
    }
    MarkAsModified();

    return true;
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::MarkAsModified()
{
    m_bModified = true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::MergeInputSemantics(
    NiShaderDeclaration::ShaderRegisterEntry* pakGeomSemantics, 
    NiShaderDeclaration::ShaderRegisterEntry* pakMaterialSemantics, 
    unsigned int& uiGeomSemantics)
{
    unsigned int uiMatEntry = 0;
    while (pakMaterialSemantics[uiMatEntry].m_uiUsageIndex != 0xFFFFFFFF)
    {
        if (uiGeomSemantics == NiMaterial::VS_INPUTS_MAX_NUM)
            return false;

        unsigned int uiGeomEntry = 0;
        NiShaderDeclaration::ShaderRegisterEntry* pkMatEntry = 
            pakMaterialSemantics + uiMatEntry++;
        bool bFound = false;
        while (uiGeomEntry < uiGeomSemantics)
        {
            if ((pakGeomSemantics[uiGeomEntry].m_kUsage ==
                pkMatEntry->m_kUsage) && 
                (pakGeomSemantics[uiGeomEntry].m_uiUsageIndex == 
                pkMatEntry->m_uiUsageIndex))
            {
                bFound = true;
                break;
            }
            ++uiGeomEntry;
        }
        if (!bFound)
        {
            // Add the entry.
            pakGeomSemantics[uiGeomSemantics].m_uiPreferredStream = 
                pkMatEntry->m_uiPreferredStream;
            pakGeomSemantics[uiGeomSemantics].m_eInput = pkMatEntry->m_eInput;
            pakGeomSemantics[uiGeomSemantics].m_eType = pkMatEntry->m_eType;
            pakGeomSemantics[uiGeomSemantics].m_kUsage = pkMatEntry->m_kUsage;
            pakGeomSemantics[uiGeomSemantics].m_uiUsageIndex = 
                pkMatEntry->m_uiUsageIndex; 
            pakGeomSemantics[uiGeomSemantics].m_uiExtraData = 
                pkMatEntry->m_uiExtraData;
            ++uiGeomSemantics;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::ShaderParameterToSemanticAndIndex(
    NiShaderDeclaration::ShaderParameter eParameter, NiFixedString& kSemantic, 
    NiUInt32& uiSemanticIndex, bool bSkinned)
{
    uiSemanticIndex = 0;
    kSemantic = NULL;
    switch (eParameter)
    {
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION0:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION1:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 1;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION2:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 2;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION3:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 3;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION4:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 4;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION5:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 5;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION6:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 6;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION7:
        if (bSkinned)
            kSemantic = NiCommonSemantics::POSITION_BP();
        else
            kSemantic = NiCommonSemantics::POSITION();
        uiSemanticIndex = 7;
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT:
        kSemantic = NiCommonSemantics::BLENDWEIGHT();
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES:
        kSemantic = NiCommonSemantics::BLENDINDICES();
        break;  
    case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
        if (bSkinned)
            kSemantic = NiCommonSemantics::NORMAL_BP();
        else
            kSemantic = NiCommonSemantics::NORMAL();
        break;          
    case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
        kSemantic = NiCommonSemantics::COLOR();
        break;          
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
        kSemantic = NiCommonSemantics::TEXCOORD();
        break;      
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 1;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 2;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 3;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 4;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 5;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 6;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
        kSemantic = NiCommonSemantics::TEXCOORD();
        uiSemanticIndex = 7;
        break;       
    case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
        if (bSkinned)
            kSemantic = NiCommonSemantics::TANGENT_BP();
        else
            kSemantic = NiCommonSemantics::TANGENT();
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
        if (bSkinned)
            kSemantic = NiCommonSemantics::BINORMAL_BP();
        else
            kSemantic = NiCommonSemantics::BINORMAL();
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM0:
        kSemantic = NiCommonSemantics::INSTANCETRANSFORMS();
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM1:
        kSemantic = NiCommonSemantics::INSTANCETRANSFORMS();
        uiSemanticIndex = 1;
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM2:
        kSemantic = NiCommonSemantics::INSTANCETRANSFORMS();
        uiSemanticIndex = 2;
        break;
    default:
        if (eParameter & SHADERPARAM_EXTRA_DATA_MASK)
        {
            kSemantic = NiCommonSemantics::DATASTREAM();
            uiSemanticIndex = eParameter & ~SHADERPARAM_EXTRA_DATA_MASK;
        }
        break;
    }

    return (kSemantic.Exists());
}
//---------------------------------------------------------------------------
unsigned int NiShaderDeclaration::GetComponentCount(
    NiShaderDeclaration::ShaderParameterType eType)
{
    unsigned int auiComponentCounts[] =
    {
        1, // SPTYPE_FLOAT1
        2, // SPTYPE_FLOAT2
        3, // SPTYPE_FLOAT3
        4, // SPTYPE_FLOAT4
        4, // SPTYPE_UBYTECOLOR
        4, // SPTYPE_UBYTE4
        2, // SPTYPE_SHORT2
        4, // SPTYPE_SHORT4
        4, // SPTYPE_NORMUBYTE4
        2, // SPTYPE_NORMSHORT2
        4, // SPTYPE_NORMSHORT4
        2, // SPTYPE_NORMUSHORT2
        4, // SPTYPE_NORMUSHORT4
        3, // SPTYPE_UDEC3
        3, // SPTYPE_NORMDEC3
        2, // SPTYPE_FLOAT16_2
        4, // SPTYPE_FLOAT16_4
        0, // SPTYPE_NONE
        0  // SPTYPE_COUNT
    };
    NIASSERT((SPTYPE_COUNT+1) == 19);

    return auiComponentCounts[eType];
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::Initialize(unsigned int uiMaxStreamEntryCount,
    unsigned int uiStreamCount)
{
    m_uiMaxStreamEntryCount = uiMaxStreamEntryCount;
    m_uiStreamCount = uiStreamCount;

    NIASSERT(m_uiMaxStreamEntryCount > 0);
    NIASSERT(m_uiStreamCount > 0);

    m_pkStreamEntries = NiNew ShaderRegisterStream[m_uiStreamCount];
    NIASSERT(m_pkStreamEntries);

    for (unsigned int ui = 0; ui < m_uiStreamCount; ui++)
    {
        m_pkStreamEntries[ui].m_pkEntries = 
            NiNew ShaderRegisterEntry[m_uiMaxStreamEntryCount];
        NIASSERT(m_pkStreamEntries[ui].m_pkEntries);
    }

    return true;
}
//---------------------------------------------------------------------------
NiShaderDeclaration* NiShaderDeclaration::Create(
    unsigned int uiMaxStreamEntryCount, 
    unsigned int uiStreamCount)
{
    NiShaderDeclaration* pkShaderDeclaration = NiNew NiShaderDeclaration;

    if (pkShaderDeclaration)
    {
        if (pkShaderDeclaration->Initialize(uiMaxStreamEntryCount, 
            uiStreamCount) == false)
        {
            NiDelete pkShaderDeclaration;
            pkShaderDeclaration = 0;
        }
    }

    return pkShaderDeclaration;
}
//---------------------------------------------------------------------------
