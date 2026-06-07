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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectAttributeTable.h"

//---------------------------------------------------------------------------
NiD3D10EffectAttributeTable::NiD3D10EffectAttributeTable()
{
    m_kAttribList.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10EffectAttributeTable::~NiD3D10EffectAttributeTable()
{
    // Need to walk the list and delete all refs in it.
    NiShaderAttributeDesc* pkDesc;

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
bool NiD3D10EffectAttributeTable::AddAttribDesc(
    NiShaderAttributeDesc* pkDesc)
{
    if (!pkDesc)
        return false;

    m_kAttribList.AddTail(pkDesc);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectAttributeTable::AddAttribDesc_Bool(
    const NiFixedString& kName, 
    bool bHidden, 
    bool bInitValue)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_String(
    const NiFixedString& kName, 
    bool bHidden, 
    const char* pcInitValue, 
    NiUInt32 uiMaxLen)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_STRING, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_UnsignedInt(
    const NiFixedString& kName, 
    bool bHidden, NiUInt32 uiInitValue, 
    NiUInt32 uiLow, 
    NiUInt32 uiHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Float(
    const NiFixedString& kName, 
    bool bHidden, 
    float fInitValue, 
    float fLow, 
    float fHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Point2(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue, 
    float* pfLow, 
    float* pfHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT2, 
        bHidden);
    NIASSERT(pkDesc);

    NiPoint2 kPtValue(pfInitValue[0], pfInitValue[1]);
    pkDesc->SetValue_Point2(kPtValue);
    if (pfHigh != 0)
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Point3(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue, 
    float* pfLow, 
    float* pfHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT3, 
        bHidden);
    NIASSERT(pkDesc);

    NiPoint3 kPtValue(pfInitValue[0], pfInitValue[1], pfInitValue[2]);
    pkDesc->SetValue_Point3(kPtValue);
    if (pfHigh != 0)
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Point4(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue, 
    float* pfLow, 
    float* pfHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT4, 
        bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Point4(pfInitValue);
    if (pfHigh != 0)
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Matrix3(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3, 
        bHidden);
    NIASSERT(pkDesc);

    NiMatrix3 kMatValue;

    kMatValue.SetRow(0, &pfInitValue[0]);
    kMatValue.SetRow(1, &pfInitValue[4]);
    kMatValue.SetRow(2, &pfInitValue[8]);
    
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Matrix4(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Color(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue, 
    float* pfLow, 
    float* pfHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_ColorA(
    const NiFixedString& kName, 
    bool bHidden, 
    float* pfInitValue, 
    float* pfLow, 
    float* pfHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR, 
        bHidden);
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
bool NiD3D10EffectAttributeTable::AddAttribDesc_Texture(
    const NiFixedString& kName, 
    bool bHidden, 
    NiUInt32 uiSlot, 
    const NiFixedString& kDefault)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE, 
        bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Texture(uiSlot, kDefault);
    if (!AddAttribDesc(pkDesc))
    {
        NiDelete pkDesc;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectAttributeTable::AddAttribDesc_Array(
    const NiFixedString& kName, 
    bool bHidden, 
    void* pvInitValue, 
    NiUInt32 uiNumElements, 
    NiUInt32 uiElementSize, 
    NiShaderAttributeDesc::AttributeType eSubType, 
    void* pvLow, 
    void* pvHigh)
{
    NiShaderAttributeDesc* pkDesc;

    // See if it's in the table first...
    pkDesc = GetAttributeByName(kName);
    if (pkDesc)
        return false;

    pkDesc = NiD3D10EffectAttributeTable::CreateAttributeDesc(
        kName, 
        NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY, 
        bHidden);
    NIASSERT(pkDesc);

    pkDesc->SetValue_Array(pvInitValue, eSubType, uiElementSize, 
        uiNumElements);

    if (pvLow == NULL || pvHigh == NULL)
    {
        pkDesc->SetRanged(false);
    }
    else
    {
        pkDesc->SetRange_Array(uiElementSize, uiNumElements, pvLow, pvHigh);
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
NiUInt32 NiD3D10EffectAttributeTable::GetAttributeCount() const
{
    return m_kAttribList.GetSize();
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc* NiD3D10EffectAttributeTable::GetFirstAttribute() const
{
    m_kAttribListIter = m_kAttribList.GetHeadPos();
    if (m_kAttribListIter)
        return m_kAttribList.GetNext(m_kAttribListIter);
    return 0;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc* NiD3D10EffectAttributeTable::GetNextAttribute() const
{
    if (m_kAttribListIter)
        return m_kAttribList.GetNext(m_kAttribListIter);
    return 0;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc* NiD3D10EffectAttributeTable::GetAttributeByName(
    const NiFixedString& kName) const
{
    NiShaderAttributeDesc* pkDesc = GetFirstAttribute();
    while (pkDesc)
    {
        if (pkDesc->GetName() == kName)
            return pkDesc;
        pkDesc = GetNextAttribute();
    }

    return 0;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc* NiD3D10EffectAttributeTable::CreateAttributeDesc(
    const NiFixedString& kName, NiShaderAttributeDesc::AttributeType eType, 
    bool bHidden)
{
    NiShaderAttributeDesc* pkDesc = NiNew NiShaderAttributeDesc();
    if (!pkDesc)
        return 0;

    pkDesc->SetName(kName);
    pkDesc->SetType(eType);
    pkDesc->SetHidden(bHidden);

    return pkDesc;
}
//---------------------------------------------------------------------------
