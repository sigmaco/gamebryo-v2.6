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
#include "NiBinaryShaderLibPCH.h"

#include "NSBAttributeTable.h"
#include "NSBAttributeDesc.h"
#include "NSBUtility.h"

//---------------------------------------------------------------------------
NSBAttributeTable::NSBAttributeTable()
{
    m_kAttribList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBAttributeTable::~NSBAttributeTable()
{
    // Need to walk the list and NiDelete all refs in it.
    NSBAttributeDesc* pkDesc;

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
bool NSBAttributeTable::AddAttribDesc(NSBAttributeDesc* pkDesc)
{
    if (!pkDesc)
        return false;

    m_kAttribList.AddTail(pkDesc);

    return true;
}
//---------------------------------------------------------------------------
bool NSBAttributeTable::AddAttribDesc_Bool(char* pcName, char*, 
    bool bHidden, bool bInitValue)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_String(char* pcName, char*, 
    bool bHidden, char* pcInitValue, unsigned int uiMaxLen)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_UnsignedInt(char* pcName, 
    char*, bool bHidden, unsigned int uiInitValue, 
    unsigned int uiLow, unsigned int uiHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Float(char* pcName, char*, 
    bool bHidden, float fInitValue, float fLow, float fHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Point2(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Point3(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Point4(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Matrix3(char* pcName, char*, 
    bool bHidden, float* pfInitValue)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Matrix4(char* pcName, char*,
    bool bHidden, float* pfInitValue)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Color(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_ColorA(char* pcName, char*, 
    bool bHidden, float* pfInitValue, float* pfLow, float* pfHigh)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Texture(char* pcName, char*, 
    bool bHidden, unsigned int uiSlot, const char* pcDefault)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
bool NSBAttributeTable::AddAttribDesc_Array(
    char* pcName,
    char*,
    bool bHidden, 
    NiShaderAttributeDesc::AttributeType eType,
    unsigned int uiArraySize,
    float* pfInitValue)
{
    NSBAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(pcName);
    if (pkDesc)
        return false;

    pkDesc = NSBAttributeTable::CreateAttributeDesc(pcName, 
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
unsigned int NSBAttributeTable::GetAttributeCount()
{
    return m_kAttribList.GetSize();
}
//---------------------------------------------------------------------------
NSBAttributeDesc* NSBAttributeTable::GetFirstAttribute()
{
    m_kAttribListIter = m_kAttribList.GetHeadPos();
    if (m_kAttribListIter)
        return m_kAttribList.GetNext(m_kAttribListIter);
    return 0;
}
//---------------------------------------------------------------------------
NSBAttributeDesc* NSBAttributeTable::GetNextAttribute()
{
    if (m_kAttribListIter)
        return m_kAttribList.GetNext(m_kAttribListIter);
    return 0;
}
//---------------------------------------------------------------------------
NSBAttributeDesc* NSBAttributeTable::GetAttributeByName(char* pcName)
{
    NSBAttributeDesc* pkDesc = GetFirstAttribute();
    while (pkDesc)
    {
        if (NiStricmp(pkDesc->GetName(), pcName) == 0)
            return pkDesc;
        pkDesc = GetNextAttribute();
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NSBAttributeTable::SaveBinary(NiBinaryStream& kStream)
{
    NSBAttributeDesc* pkAttribDesc;

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
bool NSBAttributeTable::LoadBinary(NiBinaryStream& kStream)
{
    NSBAttributeDesc* pkAttribDesc;

    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkAttribDesc = NiNew NSBAttributeDesc();
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
NSBAttributeDesc* NSBAttributeTable::CreateAttributeDesc(char* pcName, 
    NiShaderAttributeDesc::AttributeType eType, bool bHidden)
{
    NSBAttributeDesc* pkDesc = NiNew NSBAttributeDesc;
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
void NSBAttributeTable::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Attribute Table\n");
    NSBUtility::IndentInsert();
    NSBUtility::Dump(pf, true, "Count = %d\n", GetAttributeCount());
    NSBUtility::IndentInsert();

    NSBAttributeDesc* pkDesc = GetFirstAttribute();
    while (pkDesc)
    {
        DumpAttribDesc(pf, pkDesc);
        pkDesc = GetNextAttribute();
    }

    NSBUtility::IndentRemove();
    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
void NSBAttributeTable::DumpAttribDesc(FILE* pf, 
    NSBAttributeDesc* pkDesc)
{
    if (!pkDesc)
        return;

    NSBUtility::Dump(pf, true, "%32s - ", pkDesc->GetName());
    NiShaderAttributeDesc::AttributeType eType = pkDesc->GetType();
    switch (eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        NSBUtility::Dump(pf, false, "BOOL     ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        NSBUtility::Dump(pf, false, "STRING   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        NSBUtility::Dump(pf, false, "UINT     ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        NSBUtility::Dump(pf, false, "FLOAT    ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        NSBUtility::Dump(pf, false, "POINT2   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        NSBUtility::Dump(pf, false, "POINT3   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        NSBUtility::Dump(pf, false, "POINT4   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        NSBUtility::Dump(pf, false, "MATRIX3  ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        NSBUtility::Dump(pf, false, "MATRIX4  ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        NSBUtility::Dump(pf, false, "COLOR    ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        NSBUtility::Dump(pf, false, "TEXTURE  ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
        NSBUtility::Dump(pf, false, "FLOAT8   ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
        NSBUtility::Dump(pf, false, "FLOAT12  ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        NSBUtility::Dump(pf, false, "ARRAY    ");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        NSBUtility::Dump(pf, false, "UNDEFINED");
        break;
    default:
        // do nothing
        break;
    }

    if (pkDesc->IsHidden())
        NSBUtility::Dump(pf, false, "Hidden");
    else
        NSBUtility::Dump(pf, false, "Artist");

    if (pkDesc->IsRanged())
        NSBUtility::Dump(pf, false, "Ranged");
    NSBUtility::Dump(pf, false, "\n");
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
