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

#include "NSBD3D10AttributeTable.h"
#include "NSBD3D10AttributeDesc.h"
#include "NSBD3D10Utility.h"

//---------------------------------------------------------------------------
NSBD3D10AttributeTable::NSBD3D10AttributeTable()
{
    m_kAttribList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBD3D10AttributeTable::~NSBD3D10AttributeTable()
{
    // Need to walk the list and NiDelete all refs in it.
    NSBD3D10AttributeDesc* pkDesc;

    m_kAttribListIter = m_kAttribList.GetHeadPos();
    while (m_kAttribListIter)
    {
        pkDesc = m_kAttribList.GetNext(m_kAttribListIter);
        if (pkDesc)
            NiDelete pkDesc;
    }
    m_kAttribList.RemoveAll();
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc(NSBD3D10AttributeDesc* pkDesc)
{
    if (!pkDesc)
        return false;

    m_kAttribList.AddTail(pkDesc);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Bool(char* pcName, char*, 
    bool bHidden, bool bInitValue)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Bool(bInitValue);

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_String(char* pcName, char*, 
    bool bHidden, char* pcInitValue, unsigned int uiMaxLen)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_STRING, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_String(pcInitValue);
    if (uiMaxLen != 0)
    {
        pkDesc->SetRange_UnsignedInt(0, uiMaxLen);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_UnsignedInt(char* pcName, 
    char*, bool bHidden, unsigned int uiInitValue, 
    unsigned int uiLow, unsigned int uiHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_UnsignedInt(uiInitValue);
    if (uiHigh != 0)
    {
        pkDesc->SetRange_UnsignedInt(uiLow, uiHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Float(char* pcName, char*, 
    bool bHidden, float fInitValue, float fLow, float fHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Float(fInitValue);
    if (fHigh != 0.0f)
    {
        pkDesc->SetRange_Float(fLow, fHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Point2(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT2, bHidden);
    NIASSERT(pkDesc);

    NiPoint2 kPtValue(pfInitValue[0], pfInitValue[1]);
    pkDesc->SetValue_Point2(kPtValue);
    if (pfHigh[0] == 0.0f && pfHigh[1] == 0.0f)
    {
        pkDesc->SetRanged(false);
    }
    else
    {
        NiPoint2 kPtLow(pfLow[0], pfLow[1]);
        NiPoint2 kPtHigh(pfHigh[0], pfHigh[1]);
        pkDesc->SetRange_Point2(kPtLow, kPtHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Point3(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT3, bHidden);
    NIASSERT(pkDesc);

    NiPoint3 kPtValue(pfInitValue[0], pfInitValue[1], pfInitValue[2]);
    pkDesc->SetValue_Point3(kPtValue);
    if (pfHigh[0] == 0.0f && pfHigh[1] == 0.0f && pfHigh[2] == 0.0f)
    {
        pkDesc->SetRanged(false);
    }
    else
    {
        NiPoint3 kPtLow(pfLow[0], pfLow[1], pfLow[2]);
        NiPoint3 kPtHigh(pfHigh[0], pfHigh[1], pfHigh[2]);
        pkDesc->SetRange_Point3(kPtLow, kPtHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Point4(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT4, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Point4(pfInitValue);
    if (pfHigh[0] == 0.0f && pfHigh[1] == 0.0f && 
        pfHigh[2] == 0.0f && pfHigh[3] == 0.0f)
    {
        pkDesc->SetRanged(false);
    }
    else
    {
        pkDesc->SetRange_Point4(pfLow, pfHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Matrix3(char* pcName, char*, 
    bool bHidden, float* pfInitValue)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3, bHidden);
    NIASSERT(pkDesc);

    NiMatrix3 kMatValue;

    kMatValue.SetRow(0, &pfInitValue[0]);
    kMatValue.SetRow(1, &pfInitValue[3]);
    kMatValue.SetRow(2, &pfInitValue[6]);
    
    pkDesc->SetValue_Matrix3(kMatValue);
    pkDesc->SetRanged(false);

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Matrix4(char* pcName, char*,
    bool bHidden, float* pfInitValue)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Matrix4(pfInitValue);
    pkDesc->SetRanged(false);

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Color(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR, bHidden);
    NIASSERT(pkDesc);

    NiColor kClrValue;

    kClrValue.r = pfInitValue[0];
    kClrValue.g = pfInitValue[1];
    kClrValue.b = pfInitValue[2];
    
    pkDesc->SetValue_Color(kClrValue);
    if (pfHigh != 0)
    {
        NiColor kClrLow;
        NiColor kClrHigh;

        kClrLow.r = pfLow[0];
        kClrLow.g = pfLow[1];
        kClrLow.b = pfLow[2];

        kClrHigh.r = pfHigh[0];
        kClrHigh.g = pfHigh[1];
        kClrHigh.b = pfHigh[2];

        pkDesc->SetRange_Color(kClrLow, kClrHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_ColorA(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR, bHidden);
    NIASSERT(pkDesc);

    NiColorA kClrValue;

    kClrValue.r = pfInitValue[0];
    kClrValue.g = pfInitValue[1];
    kClrValue.b = pfInitValue[2];
    kClrValue.a = pfInitValue[3];
    
    pkDesc->SetValue_ColorA(kClrValue);
    if (pfHigh != 0)
    {
        NiColorA kClrLow;
        NiColorA kClrHigh;

        kClrLow.r = pfLow[0];
        kClrLow.g = pfLow[1];
        kClrLow.b = pfLow[2];
        kClrLow.a = pfLow[3];

        kClrHigh.r = pfHigh[0];
        kClrHigh.g = pfHigh[1];
        kClrHigh.b = pfHigh[2];
        kClrHigh.a = pfHigh[3];

        pkDesc->SetRange_ColorA(kClrLow, kClrHigh);
        pkDesc->SetRanged(true);
    }

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Texture(char* pcName, char*, 
    bool bHidden, unsigned int uiSlot, const char* pcDefault)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE, bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Texture(uiSlot, pcDefault);
    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::AddAttribDesc_Array(
    char* pcName,
    char*,
    bool bHidden, 
    NiShaderAttributeDesc::AttributeType eType,
    unsigned int uiArraySize,
    float* pfInitValue)
{
    NSBD3D10AttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBD3D10AttributeTable::CreateAttributeDesc(pcName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY, bHidden);
    NIASSERT(pkDesc);

    unsigned int uiElementSize = 0;
    switch (eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        uiElementSize = sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
        uiElementSize = 8*sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
        uiElementSize = 12*sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        uiElementSize = 2*sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        uiElementSize = 3*sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        uiElementSize = 4*sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        uiElementSize = 9*sizeof(float);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        uiElementSize = 16*sizeof(float);
        break;
    default:
        NIASSERT(false);
    }

    pkDesc->SetValue_Array(
        pfInitValue,
        eType,
        uiElementSize,
        uiArraySize);
    pkDesc->SetRanged(false);

    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10AttributeTable::GetAttributeCount()
{
    return m_kAttribList.GetSize();
}
//---------------------------------------------------------------------------
NSBD3D10AttributeDesc* NSBD3D10AttributeTable::GetFirstAttribute()
{
    m_kAttribListIter = m_kAttribList.GetHeadPos();
    if (m_kAttribListIter)
        return m_kAttribList.GetNext(m_kAttribListIter);
    return 0;
}
//---------------------------------------------------------------------------
NSBD3D10AttributeDesc* NSBD3D10AttributeTable::GetNextAttribute()
{
    if (m_kAttribListIter)
        return m_kAttribList.GetNext(m_kAttribListIter);
    return 0;
}
//---------------------------------------------------------------------------
NSBD3D10AttributeDesc* NSBD3D10AttributeTable::GetAttributeByName(char* pcName)
{
    NSBD3D10AttributeDesc* pkDesc = GetFirstAttribute();
    while (pkDesc)
    {
        if (NiStricmp(pkDesc->GetName(), pcName) == 0)
            return pkDesc;
        pkDesc = GetNextAttribute();
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::SaveBinary(NiBinaryStream& kStream)
{
    NSBD3D10AttributeDesc* pkAttribDesc;

    unsigned int uiCount = m_kAttribList.GetSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    m_kAttribListIter = m_kAttribList.GetHeadPos();
    while (m_kAttribListIter)
    {
        pkAttribDesc = m_kAttribList.GetNext(m_kAttribListIter);
        if (pkAttribDesc)
        {
            if (pkAttribDesc->SaveBinary(kStream))
                uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10AttributeTable::LoadBinary(NiBinaryStream& kStream)
{
    NSBD3D10AttributeDesc* pkAttribDesc;

    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkAttribDesc = NiNew NSBD3D10AttributeDesc();
        NIASSERT(pkAttribDesc);

        if (pkAttribDesc->LoadBinary(kStream))
            uiTestCount++;

        m_kAttribList.AddTail(pkAttribDesc);
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
NSBD3D10AttributeDesc* NSBD3D10AttributeTable::CreateAttributeDesc(
    char* pcName, NiShaderAttributeDesc::AttributeType eType, bool bHidden)
{
    NSBD3D10AttributeDesc* pkDesc = NiNew NSBD3D10AttributeDesc;
    if (!pkDesc)
        return 0;

    pkDesc->SetName(pcName);
    pkDesc->SetType(eType);
    pkDesc->SetHidden(bHidden);

    return pkDesc;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10AttributeTable::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "Attribute Table\n");
    NSBD3D10Utility::IndentInsert();
    NSBD3D10Utility::Dump(pf, true, "Count = %d\n", GetAttributeCount());
    NSBD3D10Utility::IndentInsert();

    NSBD3D10AttributeDesc* pkDesc = GetFirstAttribute();
    while (pkDesc)
    {
        DumpAttribDesc(pf, pkDesc);
        pkDesc = GetNextAttribute();
    }

    NSBD3D10Utility::IndentRemove();
    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
void NSBD3D10AttributeTable::DumpAttribDesc(FILE* pf, 
    NSBD3D10AttributeDesc* pkDesc)
{
    if (!pkDesc)
        return;

    NSBD3D10Utility::Dump(pf, true, "%32s - ", pkDesc->GetName());
    NiShaderAttributeDesc::AttributeType eType = pkDesc->GetType();
    switch (eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        NSBD3D10Utility::Dump(pf, false, "BOOL     ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        NSBD3D10Utility::Dump(pf, false, "STRING   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        NSBD3D10Utility::Dump(pf, false, "UINT     ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        NSBD3D10Utility::Dump(pf, false, "FLOAT    ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        NSBD3D10Utility::Dump(pf, false, "POINT2   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        NSBD3D10Utility::Dump(pf, false, "POINT3   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        NSBD3D10Utility::Dump(pf, false, "POINT4   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        NSBD3D10Utility::Dump(pf, false, "MATRIX3  ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        NSBD3D10Utility::Dump(pf, false, "MATRIX4  ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        NSBD3D10Utility::Dump(pf, false, "COLOR    ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        NSBD3D10Utility::Dump(pf, false, "TEXTURE  ");
        break;
    }

    if (pkDesc->IsHidden())
        NSBD3D10Utility::Dump(pf, false, "Hidden");
    else
        NSBD3D10Utility::Dump(pf, false, "Artist");

    if (pkDesc->IsRanged())
        NSBD3D10Utility::Dump(pf, false, "Ranged");
    NSBD3D10Utility::Dump(pf, false, "\n");
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
