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

#ifndef NISHADERATTRIBUTEDESC_H
#define NISHADERATTRIBUTEDESC_H

// These classes and functions are used for DLL-based shader libraries
#include "NiShaderDescBase.h"

#include "NiPoint2.h"
#include "NiPoint3.h"
#include "NiMatrix3.h"
#include "NiTransform.h"
#include "NiColor.h"

// Description of an 'attribute' a shader exposes
class NIMAIN_ENTRY NiShaderAttributeDesc : public NiShaderDescBase
{
public:
    NiShaderAttributeDesc();
    virtual ~NiShaderAttributeDesc();

    enum Flags
    {
        ATTRIB_FLAGS_NONE       = 0x00000000,
        ATTRIB_FLAGS_RANGED     = 0x00000001,
        ATTRIB_FLAGS_HIDDEN     = 0x00000002
    };

    enum AttributeType
    {
        ATTRIB_TYPE_UNDEFINED   = 0x00000000,
        ATTRIB_TYPE_BOOL        = 0x00000001,
        ATTRIB_TYPE_STRING      = 0x00000002,
        ATTRIB_TYPE_UNSIGNEDINT = 0x00000003,
        ATTRIB_TYPE_FLOAT       = 0x00000004,
        ATTRIB_TYPE_POINT2      = 0x00000005,
        ATTRIB_TYPE_POINT3      = 0x00000006,
        ATTRIB_TYPE_POINT4      = 0x00000007,
        ATTRIB_TYPE_MATRIX3     = 0x00000008,
        ATTRIB_TYPE_MATRIX4     = 0x00000009,
        ATTRIB_TYPE_COLOR       = 0x0000000A,
        ATTRIB_TYPE_TEXTURE     = 0x0000000B,
        // The FLOAT8 and FLOAT12 were added to allow for those
        // arrangements in the mappings
        ATTRIB_TYPE_FLOAT8      = 0x0000000C,
        ATTRIB_TYPE_FLOAT12     = 0x0000000D,
        // ARRAY was added to allow for arrays, in our case only float arrays
        ATTRIB_TYPE_ARRAY = 0x0000000E,
        // New attribute types go above here!
        ATTRIB_TYPE_COUNT
    };

    // Code exists that depends on the order of this enumeration. New entries
    // can be added before OT_COUNT, but the existing order of entries should
    // not be changed. Additionally, NiDynamicEffect has been made to match
    // this enum for fast type checking. If you change these values, make sure
    // you update NiDynamicEffect.h
    enum ObjectType
    {
        OT_UNDEFINED = 0,

        // Dynamic effects.
        OT_EFFECT_GENERALLIGHT = 1,
        OT_EFFECT_POINTLIGHT = 2,
        OT_EFFECT_DIRECTIONALLIGHT = 3,
        OT_EFFECT_SPOTLIGHT = 4,

        OT_EFFECT_SHADOWPOINTLIGHT = 5,
        OT_EFFECT_SHADOWDIRECTIONALLIGHT = 6,
        OT_EFFECT_SHADOWSPOTLIGHT = 7,

        OT_EFFECT_ENVIRONMENTMAP = 8,
        OT_EFFECT_PROJECTEDSHADOWMAP = 9,
        OT_EFFECT_PROJECTEDLIGHTMAP = 10,
        OT_EFFECT_FOGMAP = 11,

        OT_EFFECT_POINTSHADOWMAP = 12,
        OT_EFFECT_DIRSHADOWMAP = 13,
        OT_EFFECT_SPOTSHADOWMAP = 14,


        // New object types go here!
        // When adding new objects be sure to update static object tables in
        // NiShaderConstantMapEntry::InitializeObjectTable()
        OT_COUNT
    };

    AttributeType GetType() const;
    unsigned int GetFlags() const;
    bool GetValue_Bool(bool& bValue) const;
    bool GetValue_String(const char*& pcValue) const;
    bool GetValue_UnsignedInt(unsigned int& uiValue) const;
    bool GetValue_Float(float& fValue) const;
    bool GetValue_Point2(NiPoint2& kPt2Value) const;
    bool GetValue_Point3(NiPoint3& kPtValue) const;
    bool GetValue_Point4(float*& pafValue) const;
    bool GetValue_Matrix3(NiMatrix3& kMatValue) const;
    bool GetValue_Matrix4(float*& pafValue, unsigned int uiSizeBytes) const;
    bool GetValue_Color(NiColor& kClrValue) const;
    bool GetValue_ColorA(NiColorA& kClrValue) const;
    bool GetValue_Texture(unsigned int& uiValue, const char*& pcValue) 
        const;
    bool GetValue_Array(void* pvValue, unsigned int uiValueBufferSize) const;
    bool GetArrayParams(AttributeType& eSubType, unsigned int& uiElementSize,
        unsigned int& uiNumElements) const;

    bool IsHidden() const;

    bool IsRanged() const;
    bool GetRange_UnsignedInt(unsigned int& uiLow, unsigned int& uiHigh) const;
    bool GetRange_Float(float& fLow, float& fHigh) const;
    bool GetRange_Point2(NiPoint2& kPt2Low, NiPoint2& kPt2High) const;
    bool GetRange_Point3(NiPoint3& kPtLow, NiPoint3& kPtHigh) const;
    bool GetRange_Floats(unsigned int uiCount, float* pafLow,
        float* pafHigh) const;
    bool GetRange_Color(NiColor& kClrLow, NiColor& kClrHigh) const;
    bool GetRange_ColorA(NiColorA& kClrLow, NiColorA& kClrHigh) const;
    bool GetRange_Array(void*& pvLow, void*& pvHigh, 
        unsigned int uiBufferSize) const;

    // *** begin Emergent internal use only ***
    virtual void SetName(const char* pcName);

    void SetType(AttributeType eType);
    void SetFlags(unsigned int uiFlags);

    // Value setting
    void SetValue_Bool(bool bValue);
    void SetValue_String(const char* pcValue);
    void SetValue_UnsignedInt(unsigned int uiValue);
    void SetValue_Float(float fValue);
    void SetValue_Point2(NiPoint2& kPt2Value);
    void SetValue_Point3(NiPoint3& kPt3Value);
    void SetValue_Point4(float* pafValue);
    void SetValue_Matrix3(NiMatrix3& kMatValue);
    void SetValue_Matrix4(float* pafValue);
    void SetValue_Color(NiColor& kClrValue);
    void SetValue_ColorA(NiColorA& kClrValue);
    void SetValue_Texture(unsigned int uiValue, const char* pcValue);
    void SetValue_Array(void* pvValue, AttributeType eSubType, 
        unsigned int uiElementSize, unsigned int uiNumElements);

    // Hidden or exposed
    void SetHidden(bool bHide);

    // Range setting
    void SetRanged(bool bRanged);
    bool SetRange_UnsignedInt(unsigned int uiLow, unsigned int uiHigh);
    bool SetRange_Float(float fLow, float fHigh);
    bool SetRange_Point2(NiPoint2& kPt2Low, NiPoint2& kPt2High);
    bool SetRange_Point3(NiPoint3& kPtLow, NiPoint3& kPtHigh);
    bool SetRange_Point4(float* pafLow, float* pafHigh);
    bool SetRange_Floats(unsigned int uiCount, float* pafLow, float* pafHigh);
    bool SetRange_Color(NiColor& kClrLow, NiColor& kClrHigh);
    bool SetRange_ColorA(NiColorA& kClrLow, NiColorA& kClrHigh);
    bool SetRange_Array(unsigned int uiElementSize, unsigned int uiNumElements,
        void* pvLow, void* pvHigh);

    // Assignment operator
    virtual NiShaderAttributeDesc& operator=(
        const NiShaderAttributeDesc& kSrcDesc);

    static const char* GetObjectTypeAsString(ObjectType eType);
    static size_t GetSizeOfAttributeType(AttributeType eType);
    // *** end Emergent internal use only ***

protected:
    AttributeType m_eType;
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

    // Note. This class does not derive from NiMemObject. It should
    // never be dynamically allocated.
    class ArrayData 
    {
    public:
        void* m_pvValue;
        AttributeType m_eType;
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

typedef NiPointer<NiShaderAttributeDesc> NiShaderAttributeDescPtr;

#endif  //#ifndef NISHADERATTRIBUTEDESC_H
