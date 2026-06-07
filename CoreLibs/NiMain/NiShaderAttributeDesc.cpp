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

#include "NiShaderAttributeDesc.h"
#include <ctype.h>

//---------------------------------------------------------------------------
// Description of an 'attribute' a shader exposes
//---------------------------------------------------------------------------
NiShaderAttributeDesc::NiShaderAttributeDesc() :
    NiShaderDescBase(),
    m_eType(ATTRIB_TYPE_UNDEFINED),  
    m_uiFlags(0), 
    m_pcValue(NULL), 
    m_uiLen(0),
    m_uiLow(0), 
    m_uiHigh(0), 
    m_pcDefault(NULL),
    m_uiDefaultLen(0)
{                                                                    
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::~NiShaderAttributeDesc()
{
    if (m_eType == ATTRIB_TYPE_STRING)
    {
        NiFree(m_pcValue);
    }
    else if (m_eType == ATTRIB_TYPE_ARRAY)
    {
        NiFree(m_kArrayValue.m_pvValue);
        NiFree(m_pvHigh);
        NiFree(m_pvLow);
    }
    NiFree(m_pcDefault);
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType NiShaderAttributeDesc::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
unsigned int NiShaderAttributeDesc::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Bool(bool& bValue) const
{
    if (m_eType != ATTRIB_TYPE_BOOL)
        return false;

    bValue = m_bValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_String(const char*& pcValue) const
{
    if (m_eType != ATTRIB_TYPE_STRING)
        return false;

    pcValue = m_pcValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Texture(unsigned int& uiValue, 
    const char*& pcValue) const
{
    if (m_eType != ATTRIB_TYPE_TEXTURE)
        return false;

    pcValue = m_pcDefault;
    uiValue = m_uiValue;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_UnsignedInt(unsigned int& uiValue) const
{
    if (m_eType != ATTRIB_TYPE_UNSIGNEDINT)
        return false;

    uiValue = m_uiValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Float(float& fValue) const
{
    if (m_eType != ATTRIB_TYPE_FLOAT)
        return false;

    fValue = m_aafValue[0][0];
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Point2(NiPoint2& kPt2Value) const
{
    if (m_eType != ATTRIB_TYPE_POINT2)
        return false;

    kPt2Value.x = m_aafValue[0][0];
    kPt2Value.y = m_aafValue[0][1];
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Point3(NiPoint3& kPtValue) const
{
    if (m_eType != ATTRIB_TYPE_POINT3)
        return false;

    kPtValue.x = m_aafValue[0][0];
    kPtValue.y = m_aafValue[0][1];
    kPtValue.z = m_aafValue[0][2];
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Point4(float*& pafValue) const
{
    if (m_eType != ATTRIB_TYPE_POINT4)
        return false;

    pafValue[0] = m_aafValue[0][0];
    pafValue[1] = m_aafValue[0][1];
    pafValue[2] = m_aafValue[0][2];
    pafValue[3] = m_aafValue[0][3];
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Matrix3(NiMatrix3& kMatValue) const
{
    if (m_eType != ATTRIB_TYPE_MATRIX3)
        return false;

    kMatValue.SetRow(0, m_aafValue[0]);
    kMatValue.SetRow(1, m_aafValue[1]);
    kMatValue.SetRow(2, m_aafValue[2]);
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Matrix4(float*& pafValue, 
    unsigned int uiSizeBytes) 
    const
{
    NIASSERT(uiSizeBytes == (sizeof(float) * 16));

    if (m_eType != ATTRIB_TYPE_MATRIX4)
        return false;

    NiMemcpy((void*)pafValue, uiSizeBytes, (const void*)m_aafValue,
        sizeof(float) * 16);

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_Color(NiColor& kClrValue) const
{
    if (m_eType != ATTRIB_TYPE_COLOR)
        return false;

    kClrValue.r = m_aafValue[0][0];
    kClrValue.g = m_aafValue[0][1];
    kClrValue.b = m_aafValue[0][2];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetValue_ColorA(NiColorA& kClrValue) const
{
    if (m_eType != ATTRIB_TYPE_COLOR)
        return false;

    kClrValue.r = m_aafValue[0][0];
    kClrValue.g = m_aafValue[0][1];
    kClrValue.b = m_aafValue[0][2];
    kClrValue.a = m_aafValue[0][3];

    return true;
}
//---------------------------------------------------------------------------
 bool NiShaderAttributeDesc::GetValue_Array(void* pvValue,
     unsigned int uiValueBufferSize) const
{
    if (m_eType != ATTRIB_TYPE_ARRAY)
        return false;

    if (uiValueBufferSize < m_kArrayValue.m_uiElementSize * m_uiLen)
        return false;

    if (!m_kArrayValue.m_pvValue)
        return false;

    NiMemcpy(pvValue, uiValueBufferSize, m_kArrayValue.m_pvValue, 
        m_kArrayValue.m_uiElementSize * m_uiLen);

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetArrayParams(AttributeType& eSubType, 
    unsigned int& uiElementSize, unsigned int& uiNumElements) const
{
    if (m_eType != ATTRIB_TYPE_ARRAY)
        return false;

    eSubType = m_kArrayValue.m_eType;
    uiElementSize = m_kArrayValue.m_uiElementSize;
    uiNumElements = m_uiLen;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::IsRanged() const
{
    if (m_uiFlags & ATTRIB_FLAGS_RANGED)
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::IsHidden() const
{
    if ((m_uiFlags & ATTRIB_FLAGS_HIDDEN) == 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_UnsignedInt(unsigned int& uiLow, 
    unsigned int& uiHigh) const
{
    if (!IsRanged() ||
        ((m_eType != ATTRIB_TYPE_UNSIGNEDINT) && 
        (m_eType != ATTRIB_TYPE_STRING)))
    {
        return false;
    }

    uiLow = m_uiLow;
    uiHigh = m_uiHigh;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_Float(float& fLow, float& fHigh) const
{
    if (m_eType != ATTRIB_TYPE_FLOAT)
        return false;

    fLow = m_aafLow[0][0];
    fHigh = m_aafHigh[0][0];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_Point2(NiPoint2& kPt2Low,
    NiPoint2& kPt2High) const
{
    if (m_eType != ATTRIB_TYPE_POINT2)
        return false;

    kPt2Low.x = m_aafLow[0][0];
    kPt2Low.y = m_aafLow[0][1];
    kPt2High.x = m_aafHigh[0][0];
    kPt2High.y = m_aafHigh[0][1];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_Point3(NiPoint3& kPtLow, 
    NiPoint3& kPtHigh) const
{
    if (m_eType != ATTRIB_TYPE_POINT3)
        return false;

    kPtLow.x = m_aafLow[0][0];
    kPtLow.y = m_aafLow[0][1];
    kPtLow.z = m_aafLow[0][2];
    kPtHigh.x = m_aafHigh[0][0];
    kPtHigh.y = m_aafHigh[0][1];
    kPtHigh.z = m_aafHigh[0][2];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_Floats(unsigned int uiCount, 
    float* pafLow, float* pafHigh) const
{
    switch (m_eType)
    {
        case ATTRIB_TYPE_POINT4:
            if (uiCount != 4)
                return false;
            pafLow[0] = m_aafLow[0][0];
            pafLow[1] = m_aafLow[0][1];
            pafLow[2] = m_aafLow[0][2];
            pafLow[3] = m_aafLow[0][3];
            pafHigh[0] = m_aafHigh[0][0];
            pafHigh[1] = m_aafHigh[0][1];
            pafHigh[2] = m_aafHigh[0][2];
            pafHigh[3] = m_aafHigh[0][3];
            break;
        default:
            return false;
    }
    return true;
}

//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_Color(NiColor& kClrLow, 
    NiColor& kClrHigh)  const
{
    if (m_eType != ATTRIB_TYPE_COLOR)
        return false;

    kClrLow.r = m_aafLow[0][0];
    kClrLow.g = m_aafLow[0][1];
    kClrLow.b = m_aafLow[0][2];

    kClrHigh.r = m_aafHigh[0][0];
    kClrHigh.g = m_aafHigh[0][1];
    kClrHigh.b = m_aafHigh[0][2];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_ColorA(NiColorA& kClrLow, 
    NiColorA& kClrHigh) const
{
    if (m_eType != ATTRIB_TYPE_COLOR)
        return false;

    kClrLow.r = m_aafLow[0][0];
    kClrLow.g = m_aafLow[0][1];
    kClrLow.b = m_aafLow[0][2];
    kClrLow.a = m_aafLow[0][3];

    kClrHigh.r = m_aafHigh[0][0];
    kClrHigh.g = m_aafHigh[0][1];
    kClrHigh.b = m_aafHigh[0][2];
    kClrHigh.a = m_aafHigh[0][3];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::GetRange_Array(void*& pvLow, void*& pvHigh,
    unsigned int uiBufferSize) const
{
    if (m_eType != ATTRIB_TYPE_ARRAY)
        return false;

    if (uiBufferSize < m_kArrayValue.m_uiElementSize * m_uiLen)
        return false;

    if (!m_pvLow || !m_pvHigh)
        return false;

    unsigned int uiDataSize = m_kArrayValue.m_uiElementSize * 
        m_uiLen;
    NiMemcpy(pvLow, m_pvLow, uiDataSize);
    NiMemcpy(pvHigh, m_pvHigh, uiDataSize);
    return true;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetName(const char* pcName)
{
    NiShaderDescBase::SetName(pcName);

    // Replace all whitespace with an underscore
    const size_t uiLen = strlen(m_pcName) + 1;
    bool bWhitespaceFound = false;
    for (size_t i = 0; i < uiLen; i++)
    {
        if (isspace(m_pcName[i]))
        {
            bWhitespaceFound = true;
            m_pcName[i] = '_';
        }
    }

    if (bWhitespaceFound)
    {
        char acBuf[1024];
        NiSprintf(acBuf, 1024, 
            "%s Warning: Attempting to set an attribute name with spaces: "
            "\"%s\"\n"
            "    Replacing with \"%s\"\n", 
            __FUNCTION__, 
            pcName,
            m_pcName);
        NiOutputDebugString(acBuf);
    }
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetType(
    NiShaderAttributeDesc::AttributeType eType)
{
    if (m_eType != eType)
    {
        if (m_eType == ATTRIB_TYPE_STRING)
        {
            NiFree(m_pcValue);
            m_uiLen = NULL;
        }
        else if (m_eType == ATTRIB_TYPE_ARRAY)
        {
            NiFree(m_kArrayValue.m_pvValue);
            NiFree(m_pvHigh);
            NiFree(m_pvLow);
            m_uiLen = NULL;
        }

        m_eType = eType;
        if (eType == ATTRIB_TYPE_STRING)
        {
            m_pcValue = NULL;
            m_uiLen = NULL;
        }
        else if (eType == ATTRIB_TYPE_ARRAY)
        {
            m_kArrayValue.m_pvValue = NULL;
            m_pvHigh = NULL;
            m_pvLow = NULL;
            m_uiLen = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Bool(bool bValue)
{
    SetType(ATTRIB_TYPE_BOOL);
    m_bValue = bValue;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_String(const char* pcValue)
{
    SetType(ATTRIB_TYPE_STRING);

    if (pcValue && pcValue[0] != '\0')
    {
        // See if the length is OK
        if (m_pcValue && (m_uiLen <= strlen(pcValue)))
        {
            NiFree(m_pcValue);
            m_pcValue = NULL;
            m_uiLen = 0;
        }

        if (!m_pcValue)
        {
            m_uiLen = (unsigned int)(strlen(pcValue) + 1);
            m_pcValue = NiAlloc(char,m_uiLen);
        }

        NiStrcpy(m_pcValue, m_uiLen, pcValue);
    }
    else
    {
        NiFree(m_pcValue);
        m_pcValue = NULL;
        m_uiLen = 0;
    }
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_UnsignedInt(unsigned int uiValue)
{
    SetType(ATTRIB_TYPE_UNSIGNEDINT);
    m_uiValue = uiValue;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Float(float fValue)
{
    SetType(ATTRIB_TYPE_FLOAT);
    m_aafValue[0][0] = fValue;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Point2(NiPoint2& kPt2Value)
{
    SetType(ATTRIB_TYPE_POINT2);
    m_aafValue[0][0] = kPt2Value.x;
    m_aafValue[0][1] = kPt2Value.y;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Point3(NiPoint3& kPtValue)
{
    SetType(ATTRIB_TYPE_POINT3);
    m_aafValue[0][0] = kPtValue.x;
    m_aafValue[0][1] = kPtValue.y;
    m_aafValue[0][2] = kPtValue.z;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Point4(float* pafValue)
{
    NIASSERT(pafValue);

    // Assumes that there are 4 floats!!!
    SetType(ATTRIB_TYPE_POINT4);
    unsigned int uiDestSize = 16 * sizeof(float); //float [4][4];
    NiMemcpy((void*)m_aafValue, uiDestSize, (const void*)pafValue, 
        sizeof(float) * 4);
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Matrix3(NiMatrix3& kMatValue)
{
    SetType(ATTRIB_TYPE_MATRIX3);
    m_aafValue[0][0] = kMatValue.GetEntry(0,0);
    m_aafValue[0][1] = kMatValue.GetEntry(0,1);
    m_aafValue[0][2] = kMatValue.GetEntry(0,2);
    m_aafValue[1][0] = kMatValue.GetEntry(1,0);
    m_aafValue[1][1] = kMatValue.GetEntry(1,1);
    m_aafValue[1][2] = kMatValue.GetEntry(1,2);
    m_aafValue[2][0] = kMatValue.GetEntry(2,0);
    m_aafValue[2][1] = kMatValue.GetEntry(2,1);
    m_aafValue[2][2] = kMatValue.GetEntry(2,2);
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Matrix4(float* pafValue)
{
    NIASSERT(pafValue);

    // Assumes that there are 16 floats!!!
    SetType(ATTRIB_TYPE_MATRIX4);
    unsigned int uiDestSize = 16 * sizeof(float); //float [4][4];
    NiMemcpy((void*)m_aafValue, (const void*)pafValue, uiDestSize);
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Color(NiColor& kClrValue)
{
    SetType(ATTRIB_TYPE_COLOR);
    m_aafValue[0][0] = kClrValue.r;
    m_aafValue[0][1] = kClrValue.g;
    m_aafValue[0][2] = kClrValue.b;
    m_aafValue[0][3] = 1.0f;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_ColorA(NiColorA& kClrValue)
{
    SetType(ATTRIB_TYPE_COLOR);
    m_aafValue[0][0] = kClrValue.r;
    m_aafValue[0][1] = kClrValue.g;
    m_aafValue[0][2] = kClrValue.b;
    m_aafValue[0][3] = kClrValue.a;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Texture(unsigned int uiValue, 
    const char* pcValue)
{
    SetType(ATTRIB_TYPE_TEXTURE);

    if (pcValue && pcValue[0] != '\0')
    {
        // See if the length is OK
        if (m_pcDefault && (m_uiDefaultLen <= strlen(pcValue)))
        {
            NiFree(m_pcDefault);
            m_pcDefault = NULL;
            m_uiDefaultLen = 0;
        }

        if (!m_pcDefault)
        {
            m_uiDefaultLen = (unsigned int)(strlen(pcValue) + 1);
            m_pcDefault = NiAlloc(char,m_uiDefaultLen);
        }

        NiStrcpy(m_pcDefault, m_uiDefaultLen, pcValue);
    }
    else
    {
        NiFree(m_pcDefault);
        m_pcDefault = NULL;
        m_uiDefaultLen = 0;
    }

    m_uiValue = uiValue;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetValue_Array(void* pValue, 
    AttributeType eSubType, unsigned int uiElementSize, 
    unsigned int uiNumElements)
{
    NIASSERT(pValue != NULL && uiNumElements != 0);
    NIASSERT(uiElementSize == sizeof(float) || 
           uiElementSize == 2*sizeof(float) || 
           uiElementSize == 3*sizeof(float) ||
           uiElementSize == 4*sizeof(float));
    NIASSERT(eSubType == ATTRIB_TYPE_FLOAT ||
           eSubType == ATTRIB_TYPE_POINT2 || 
           eSubType == ATTRIB_TYPE_POINT3 ||
           eSubType == ATTRIB_TYPE_POINT4 ||
           eSubType == ATTRIB_TYPE_COLOR);

    SetType(ATTRIB_TYPE_ARRAY);
    unsigned int uiNewSize = uiNumElements * uiElementSize;
    if (m_kArrayValue.m_pvValue != NULL)
    {
        if (uiNewSize > (m_kArrayValue.m_uiElementSize * m_uiLen))
        {
            NiFree(m_kArrayValue.m_pvValue);
            m_uiLen = 0;
        }
    }

    m_kArrayValue.m_pvValue = NiAlloc(char,uiNewSize);
    unsigned int uiDestSize = uiNewSize * sizeof(char);
    NiMemcpy(m_kArrayValue.m_pvValue, pValue, uiDestSize);

    m_kArrayValue.m_eType = eSubType;
    m_kArrayValue.m_uiElementSize = uiElementSize;
    m_uiLen = uiNumElements;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetHidden(bool bHide)
{
    if (bHide)
        m_uiFlags |= ATTRIB_FLAGS_HIDDEN;
    else
        m_uiFlags &= ~ATTRIB_FLAGS_HIDDEN;
}
//---------------------------------------------------------------------------
void NiShaderAttributeDesc::SetRanged(bool bRanged)
{
    if (bRanged)
        m_uiFlags |= ATTRIB_FLAGS_RANGED;
    else
        m_uiFlags &= ~ATTRIB_FLAGS_RANGED;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_UnsignedInt(unsigned int uiLow, 
    unsigned int uiHigh)
{
    if ((m_eType != ATTRIB_TYPE_UNSIGNEDINT) &&
        (m_eType != ATTRIB_TYPE_STRING))
    {
        return false;
    }

    m_uiLow = uiLow;
    m_uiHigh = uiHigh;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Float(float fLow, float fHigh)
{
    if (m_eType != ATTRIB_TYPE_FLOAT)
        return false;

    m_aafLow[0][0] = fLow;
    m_aafHigh[0][0] = fHigh;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Point2(NiPoint2& kPt2Low, 
    NiPoint2& kPt2High)
{
    if (m_eType != ATTRIB_TYPE_POINT2)
        return false;

    m_aafLow[0][0] = kPt2Low.x;
    m_aafLow[0][1] = kPt2Low.y;
    m_aafHigh[0][0] = kPt2High.x;
    m_aafHigh[0][1] = kPt2High.y;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Point3(NiPoint3& kPtLow, 
    NiPoint3& kPtHigh)
{
    if (m_eType != ATTRIB_TYPE_POINT3)
        return false;

    m_aafLow[0][0] = kPtLow.x;
    m_aafLow[0][1] = kPtLow.y;
    m_aafLow[0][2] = kPtLow.z;
    m_aafHigh[0][0] = kPtHigh.x;
    m_aafHigh[0][1] = kPtHigh.y;
    m_aafHigh[0][2] = kPtHigh.z;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Point4(float* pafLow, float* pafHigh)
{
    if (m_eType != ATTRIB_TYPE_POINT4)
        return false;

    m_aafLow[0][0] = pafLow[0];
    m_aafLow[0][1] = pafLow[1];
    m_aafLow[0][2] = pafLow[2];
    m_aafLow[0][3] = pafLow[3];
    m_aafHigh[0][0] = pafHigh[0];
    m_aafHigh[0][1] = pafHigh[1];
    m_aafHigh[0][2] = pafHigh[2];
    m_aafHigh[0][3] = pafHigh[3];

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Floats(unsigned int uiCount, 
    float* pafLow, float* pafHigh)
{
    switch (m_eType)
    {
        case ATTRIB_TYPE_POINT4:
            if (uiCount != 4)
                return false;
            m_aafLow[0][0] = pafLow[0];
            m_aafLow[0][1] = pafLow[1];
            m_aafLow[0][2] = pafLow[2];
            m_aafLow[0][3] = pafLow[3];
            m_aafHigh[0][0] = pafHigh[0];
            m_aafHigh[0][1] = pafHigh[1];
            m_aafHigh[0][2] = pafHigh[2];
            m_aafHigh[0][3] = pafHigh[3];
            break;
        default:
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Color(NiColor& kClrLow, 
    NiColor& kClrHigh)
{
    if (m_eType != ATTRIB_TYPE_COLOR)
        return false;

    m_aafLow[0][0] = kClrLow.r;
    m_aafLow[0][1] = kClrLow.g;
    m_aafLow[0][2] = kClrLow.b;
    m_aafLow[0][3] = 1.0f;
    m_aafHigh[0][0] = kClrHigh.r;
    m_aafHigh[0][1] = kClrHigh.g;
    m_aafHigh[0][2] = kClrHigh.b;
    m_aafHigh[0][3] = 1.0f;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_ColorA(NiColorA& kClrLow, 
    NiColorA& kClrHigh)
{
    if (m_eType != ATTRIB_TYPE_COLOR)
        return false;

    m_aafLow[0][0] = kClrLow.r;
    m_aafLow[0][1] = kClrLow.g;
    m_aafLow[0][2] = kClrLow.b;
    m_aafLow[0][3] = kClrLow.a;
    m_aafHigh[0][0] = kClrHigh.r;
    m_aafHigh[0][1] = kClrHigh.g;
    m_aafHigh[0][2] = kClrHigh.b;
    m_aafHigh[0][3] = kClrHigh.a;

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderAttributeDesc::SetRange_Array(unsigned int uiElementSize, 
    unsigned int uiNumElements, void* pLow, void* pHigh)
{
    if (m_eType != ATTRIB_TYPE_ARRAY)
        return false;

    if (uiElementSize != m_kArrayValue.m_uiElementSize ||
        uiNumElements != m_uiLen)
    {
        return false;
    }

    unsigned int uiNewSize = uiNumElements * uiElementSize;
    if (m_pvLow != NULL || m_pvHigh != NULL)
    {
        if (uiNewSize > (m_kArrayValue.m_uiElementSize * m_uiLen))
        {
            NiFree(m_pvLow);
            NiFree(m_pvHigh);
            m_pvLow = 0;
            m_pvHigh = 0;
        }
    }
    
    if (pLow != NULL && pHigh != NULL)
    {
        m_pvLow = NiAlloc(float,uiNewSize);
        m_pvHigh = NiAlloc(float,uiNewSize);

        unsigned int uiDestSize = uiNewSize * sizeof(float);

        NiMemcpy(m_pvLow, pLow, uiDestSize);
        NiMemcpy(m_pvHigh, pHigh, uiDestSize);
    }
    else
    {
        m_pvLow = NULL;
        m_pvHigh = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc& NiShaderAttributeDesc::operator=(
    const NiShaderAttributeDesc& kSrcDesc)
{
    SetName(kSrcDesc.GetName());
    SetDescription(kSrcDesc.GetDescription());

    m_eType = kSrcDesc.m_eType;
    m_uiFlags = kSrcDesc.m_uiFlags;

    NiFree(m_pcDefault);
    m_pcDefault = NULL;
    m_uiDefaultLen = 0;

    switch (m_eType)
    {
    case ATTRIB_TYPE_BOOL:
        SetValue_Bool(kSrcDesc.m_bValue);
        break;
    case ATTRIB_TYPE_STRING:
        SetValue_String(kSrcDesc.m_pcValue);
        SetRange_UnsignedInt(kSrcDesc.m_uiLow, kSrcDesc.m_uiHigh);
        break;
    case ATTRIB_TYPE_UNSIGNEDINT:
        SetValue_UnsignedInt(kSrcDesc.m_uiValue);
        SetRange_UnsignedInt(kSrcDesc.m_uiLow, kSrcDesc.m_uiHigh);
        break;
    case ATTRIB_TYPE_FLOAT:
        SetValue_Float(kSrcDesc.m_aafValue[0][0]);
        SetRange_Float(kSrcDesc.m_aafLow[0][0], kSrcDesc.m_aafHigh[0][0]);
        break;
    case ATTRIB_TYPE_POINT2:
        {
            NiPoint2 kPt2;
            NiPoint2 kPt2_Low;
            NiPoint2 kPt2_High;
            
            kPt2.x = kSrcDesc.m_aafValue[0][0];
            kPt2.y = kSrcDesc.m_aafValue[0][1];
            SetValue_Point2(kPt2);
            kPt2_Low.x = kSrcDesc.m_aafLow[0][0];
            kPt2_Low.y = kSrcDesc.m_aafLow[0][1];
            kPt2_High.x = kSrcDesc.m_aafHigh[0][0];
            kPt2_High.y = kSrcDesc.m_aafHigh[0][1];
            SetRange_Point2(kPt2_Low, kPt2_High);
        }
        break;
    case ATTRIB_TYPE_POINT3:
        {
            NiPoint3 kPt3;
            NiPoint3 kPt3_Low;
            NiPoint3 kPt3_High;
            
            kPt3.x = kSrcDesc.m_aafValue[0][0];
            kPt3.y = kSrcDesc.m_aafValue[0][1];
            kPt3.z = kSrcDesc.m_aafValue[0][2];
            SetValue_Point3(kPt3);
            kPt3_Low.x = kSrcDesc.m_aafLow[0][0];
            kPt3_Low.y = kSrcDesc.m_aafLow[0][1];
            kPt3_Low.z = kSrcDesc.m_aafLow[0][2];
            kPt3_High.x = kSrcDesc.m_aafHigh[0][0];
            kPt3_High.y = kSrcDesc.m_aafHigh[0][1];
            kPt3_High.z = kSrcDesc.m_aafHigh[0][2];
            SetRange_Point3(kPt3_Low, kPt3_High);
        }
        break;
    case ATTRIB_TYPE_POINT4:
        {
            float* pfValue = (float*)(kSrcDesc.m_aafValue);
            float* pfLow = (float*)(kSrcDesc.m_aafLow);
            float* pfHigh = (float*)(kSrcDesc.m_aafHigh);

            SetValue_Point4(pfValue);
            SetRange_Point4(pfLow, pfHigh);
        }
        break;
    case ATTRIB_TYPE_MATRIX3:
        {
            NiMatrix3 kMat3;
            kMat3.SetRow(0, kSrcDesc.m_aafValue[0]);
            kMat3.SetRow(1, kSrcDesc.m_aafValue[1]);
            kMat3.SetRow(2, kSrcDesc.m_aafValue[2]);

            SetValue_Matrix3(kMat3);
        }
        break;
    case ATTRIB_TYPE_MATRIX4:
        {
            float* pfValue = (float*)(kSrcDesc.m_aafValue);
            SetValue_Matrix4(pfValue);
        }
        break;
    case ATTRIB_TYPE_COLOR:
        {
            NiColorA kClr;
            kClr.r = kSrcDesc.m_aafValue[0][0];
            kClr.g = kSrcDesc.m_aafValue[0][1];
            kClr.b = kSrcDesc.m_aafValue[0][2];
            kClr.a = kSrcDesc.m_aafValue[0][3];
            NiColorA kClr_Low;
            kClr_Low.r = kSrcDesc.m_aafLow[0][0];
            kClr_Low.g = kSrcDesc.m_aafLow[0][1];
            kClr_Low.b = kSrcDesc.m_aafLow[0][2];
            kClr_Low.a = kSrcDesc.m_aafLow[0][3];
            NiColorA kClr_High;
            kClr_High.r = kSrcDesc.m_aafHigh[0][0];
            kClr_High.g = kSrcDesc.m_aafHigh[0][1];
            kClr_High.b = kSrcDesc.m_aafHigh[0][2];
            kClr_High.a = kSrcDesc.m_aafHigh[0][3];

            SetValue_ColorA(kClr);
            SetRange_ColorA(kClr_Low, kClr_High);
        }
        break;
    case ATTRIB_TYPE_TEXTURE:
        SetValue_Texture(kSrcDesc.m_uiValue, kSrcDesc.m_pcDefault);
        break;
    case ATTRIB_TYPE_ARRAY:
        {
            SetValue_Array(kSrcDesc.m_kArrayValue.m_pvValue, 
                kSrcDesc.m_kArrayValue.m_eType, 
                kSrcDesc.m_kArrayValue.m_uiElementSize,
                kSrcDesc.m_uiLen);
            SetRange_Array( 
                kSrcDesc.m_kArrayValue.m_uiElementSize,
                kSrcDesc.m_uiLen,
                kSrcDesc.m_pvLow, kSrcDesc.m_pvHigh);
        }
        break;
    default:
        break;
    }
    return *this;
}
//---------------------------------------------------------------------------
const char* NiShaderAttributeDesc::GetObjectTypeAsString(ObjectType eType)
{
    switch(eType)
    {
        case OT_EFFECT_GENERALLIGHT:
            return "GeneralLight";
        case OT_EFFECT_POINTLIGHT:
            return "PointLight";
        case OT_EFFECT_DIRECTIONALLIGHT:
            return "DirectionalLight";
        case OT_EFFECT_SPOTLIGHT:
            return "SpotLight";
        case OT_EFFECT_ENVIRONMENTMAP:
            return "EnvironmentMap";
        case OT_EFFECT_PROJECTEDSHADOWMAP:
            return "ProjectedShadowMap";
        case OT_EFFECT_PROJECTEDLIGHTMAP:
            return "ProjectedLightMap";
        case OT_EFFECT_FOGMAP:
            return "FogMap";
        default:
            return NULL;
    }

}
//---------------------------------------------------------------------------
size_t NiShaderAttributeDesc::GetSizeOfAttributeType(AttributeType eType)
{
    switch (eType)
    {
    case ATTRIB_TYPE_BOOL:
        return sizeof(bool);
    case ATTRIB_TYPE_UNSIGNEDINT:
        return sizeof(unsigned int);
    case ATTRIB_TYPE_FLOAT:
        return sizeof(float);
    case ATTRIB_TYPE_POINT2:
        return 2*sizeof(float);
    case ATTRIB_TYPE_POINT3:
        return 3*sizeof(float);
    case ATTRIB_TYPE_COLOR:
    case ATTRIB_TYPE_POINT4:
        return 4*sizeof(float);
    case ATTRIB_TYPE_MATRIX3:
        return 9*sizeof(float);
    case  ATTRIB_TYPE_MATRIX4:
        return 16*sizeof(float);
    default:
        return 0;
    }
}
//---------------------------------------------------------------------------
