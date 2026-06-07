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

#ifndef NSBD3D10ATTRIBUTEDESC_H
#define NSBD3D10ATTRIBUTEDESC_H

#include <NiShaderAttributeDesc.h>
#include "NiD3D10BinaryShaderLibLibType.h"

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10AttributeDesc : public NiMemObject
{
public:
    NSBD3D10AttributeDesc();
    virtual ~NSBD3D10AttributeDesc();

    NiShaderAttributeDesc* GetShaderAttributeDesc();

    const char* GetName() const;
    void SetName(const char* pcName);

    const char* GetDescription() const;
    void SetDescription(const char* pcDesc);

    NiShaderAttributeDesc::AttributeType GetType() const;
    unsigned int GetFlags() const;
    bool GetValue_Bool(bool& bValue) const;
    bool GetValue_String(const char*& pcValue) const;
    bool GetValue_UnsignedInt(unsigned int& uiValue) const;
    bool GetValue_Float(float& fValue) const;
    bool GetValue_Point2(NiPoint2& kPt2Value) const;
    bool GetValue_Point3(NiPoint3& kPtValue) const;
    bool GetValue_Point4(float*& pfValue) const;
    bool GetValue_Matrix3(NiMatrix3& kMatValue) const;
    bool GetValue_Matrix4(float*& pfValue, unsigned int uiSizeBytes) const;
    bool GetValue_Color(NiColor& kClrValue) const;
    bool GetValue_ColorA(NiColorA& kClrValue) const;
    bool GetValue_Texture(unsigned int& uiValue, const char*& pcValue) 
        const;
    bool GetValue_Array(void* pvValue, unsigned int uiValueBufferSize) const;
    bool GetArrayParams(NiShaderAttributeDesc::AttributeType& eSubType, 
        unsigned int& uiElementSize, unsigned int& uiNumElements) const;

    bool IsHidden() const;

    bool IsRanged() const;
    bool GetRange_UnsignedInt(unsigned int& uiLow, unsigned int& uiHigh) const;
    bool GetRange_Float(float& fLow, float& fHigh) const;
    bool GetRange_Point2(NiPoint2& kPt2Low, NiPoint2& kPt2High) const;
    bool GetRange_Point3(NiPoint3& kPtLow, NiPoint3& kPtHigh) const;
    bool GetRange_Floats(unsigned int uiCount, float* pfLow,
        float* pfHigh) const;
    bool GetRange_Color(NiColor& kClrLow, NiColor& kClrHigh) const;
    bool GetRange_ColorA(NiColorA& kClrLow, NiColorA& kClrHigh) const;
    bool GetRange_Array(void*& pvLow, void*& pvHigh, 
        unsigned int uiBufferSize) const;

    // *** begin Emergent internal use only ***
    void SetType(NiShaderAttributeDesc::AttributeType eType);
    void SetFlags(unsigned int uiFlags);

    // Value setting
    void SetValue_Bool(bool bValue);
    void SetValue_String(const char* pcValue);
    void SetValue_UnsignedInt(unsigned int uiValue);
    void SetValue_Float(float fValue);
    void SetValue_Point2(NiPoint2& kPt2Value);
    void SetValue_Point3(NiPoint3& kPt3Value);
    void SetValue_Point4(float* pfValue);
    void SetValue_Matrix3(NiMatrix3& kMatValue);
    void SetValue_Matrix4(float* pfValue);
    void SetValue_Color(NiColor& kClrValue);
    void SetValue_ColorA(NiColorA& kClrValue);
    void SetValue_Texture(unsigned int uiValue, const char* pcValue);
    void SetValue_Array(void* pvValue, 
        NiShaderAttributeDesc::AttributeType eSubType, 
        unsigned int uiElementSize, unsigned int uiNumElements);

    // Hidden or exposed
    void SetHidden(bool bHide);

    // Range setting
    void SetRanged(bool bRanged);
    bool SetRange_UnsignedInt(unsigned int uiLow, unsigned int uiHigh);
    bool SetRange_Float(float fLow, float fHigh);
    bool SetRange_Point2(NiPoint2& kPt2Low, NiPoint2& kPt2High);
    bool SetRange_Point3(NiPoint3& kPtLow, NiPoint3& kPtHigh);
    bool SetRange_Point4(float* pfLow, float* pfHigh);
    bool SetRange_Floats(unsigned int uiCount, float* pfLow, float* pfHigh);
    bool SetRange_Color(NiColor& kClrLow, NiColor& kClrHigh);
    bool SetRange_ColorA(NiColorA& kClrLow, NiColorA& kClrHigh);
    bool SetRange_Array(unsigned int uiElementSize, unsigned int uiNumElements,
        void* pvLow, void* pvHigh);

    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    virtual bool SaveBinary_Bool(NiBinaryStream& kStream);
    virtual bool SaveBinary_String(NiBinaryStream& kStream);
    virtual bool SaveBinary_UnsignedInt(NiBinaryStream& kStream);
    virtual bool SaveBinary_Float(NiBinaryStream& kStream);
    virtual bool SaveBinary_Point2(NiBinaryStream& kStream);
    virtual bool SaveBinary_Point3(NiBinaryStream& kStream);
    virtual bool SaveBinary_Point4(NiBinaryStream& kStream);
    virtual bool SaveBinary_Matrix3(NiBinaryStream& kStream);
    virtual bool SaveBinary_Matrix4(NiBinaryStream& kStream);
    virtual bool SaveBinary_Color(NiBinaryStream& kStream);
    virtual bool SaveBinary_Texture(NiBinaryStream& kStream);
    virtual bool SaveBinary_Array(NiBinaryStream& kStream);

    virtual bool LoadBinary_Bool(NiBinaryStream& kStream);
    virtual bool LoadBinary_String(NiBinaryStream& kStream);
    virtual bool LoadBinary_UnsignedInt(NiBinaryStream& kStream);
    virtual bool LoadBinary_Float(NiBinaryStream& kStream);
    virtual bool LoadBinary_Point2(NiBinaryStream& kStream);
    virtual bool LoadBinary_Point3(NiBinaryStream& kStream);
    virtual bool LoadBinary_Point4(NiBinaryStream& kStream);
    virtual bool LoadBinary_Matrix3(NiBinaryStream& kStream);
    virtual bool LoadBinary_Matrix4(NiBinaryStream& kStream);
    virtual bool LoadBinary_Color(NiBinaryStream& kStream);
    virtual bool LoadBinary_Texture(NiBinaryStream& kStream);
    virtual bool LoadBinary_Array(NiBinaryStream& kStream);

    char* m_pcName;
    char* m_pcDesc;

    NiShaderAttributeDesc::AttributeType m_eType;
    unsigned int m_uiFlags;

    // Data values
    // For float:
    //      m_aafValue[0][0] contains the data
    // For NiPoint#:
    //      m_aafValue[0] 'row' contains the data
    // For NiMatrix3:
    //      m_aafValue[0][0]..m_aafValue[0][2] contains row1
    //      m_aafValue[1][0]..m_aafValue[1][2] contains row2
    //      m_aafValue[2][0]..m_aafValue[2][2] contains row3
    // For Matrix4:
    //      m_aafValue[0][0]..m_aafValue[0][3] contains row1
    //      m_aafValue[1][0]..m_aafValue[1][3] contains row2
    //      m_aafValue[2][0]..m_aafValue[2][3] contains row3
    //      m_aafValue[3][0]..m_aafValue[3][3] contains row4
    // For NiColor(A)
    //      m_aafValue[0] 'row' contains the data

    struct ArrayData
    {
    public:
        void* m_pvValue;
        NiShaderAttributeDesc::AttributeType m_eType;
        unsigned int m_uiElementSize;
    };

    union
    {
        bool m_bValue;
        char* m_pcValue;
        unsigned int m_uiValue;
        float m_aafValue[4][4];
        ArrayData m_kArrayValue;
    };
    unsigned int m_uiLen;

    // Range values
    union
    {
        unsigned int m_uiLow;
        float m_aafLow[4][4];
        void* m_pvLow;
    };
    union
    {
        unsigned int m_uiHigh;
        float m_aafHigh[4][4];
        void* m_pvHigh;
    };

    char* m_pcDefault;
    unsigned int m_uiDefaultLen;
};

#endif  //NSBD3D10ATTRIBUTEDESC_H
