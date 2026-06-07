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

#ifndef NID3D10EFFECTATTRIBUTETABLE_H
#define NID3D10EFFECTATTRIBUTETABLE_H

#include "NiD3D10EffectShaderLibType.h"

#include <NiShaderAttributeDesc.h>
#include <NiTPointerList.h>

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectAttributeTable : 
    public NiMemObject
{
public:
    NiD3D10EffectAttributeTable();
    ~NiD3D10EffectAttributeTable();

    bool AddAttribDesc(NiShaderAttributeDesc* pkDesc);

    bool AddAttribDesc_Bool(
        const NiFixedString& pkName, 
        bool bHidden, 
        bool bInitValue);
    bool AddAttribDesc_String(
        const NiFixedString& pkName, 
        bool bHidden, 
        const char* pkInitValue, 
        NiUInt32 uiMaxLen = 0);
    bool AddAttribDesc_UnsignedInt(
        const NiFixedString& pkName, 
        bool bHidden, 
        NiUInt32 uiInitValue,   
        NiUInt32 uiLow = 0, 
        NiUInt32 uiHigh = 0);
    bool AddAttribDesc_Float(
        const NiFixedString& pkName, 
        bool bHidden, 
        float fInitValue, 
        float fLow = 0.0f, 
        float fHigh = 0.0f);
    bool AddAttribDesc_Point2(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue, 
        float* pfLow = NULL, 
        float* pfHigh = NULL);
    bool AddAttribDesc_Point3(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue, 
        float* pfLow = NULL, 
        float* pfHigh = NULL);
    bool AddAttribDesc_Point4(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue, 
        float* pfLow = NULL, 
        float* pfHigh = NULL);
    bool AddAttribDesc_Matrix3(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue);
    bool AddAttribDesc_Matrix4(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue);
    bool AddAttribDesc_Color(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue, 
        float* pfLow = NULL, 
        float* pfHigh = NULL);
    bool AddAttribDesc_ColorA(
        const NiFixedString& pkName, 
        bool bHidden, 
        float* pfInitValue, 
        float* pfLow = NULL, 
        float* pfHigh = NULL);
    bool AddAttribDesc_Texture(
        const NiFixedString& pkName, 
        bool bHidden, 
        NiUInt32 uiSlot, 
        const NiFixedString& pkDefault = NULL);
    bool AddAttribDesc_Array(
        const NiFixedString& pkName, 
        bool bHidden, 
        void* pvInitValue, 
        NiUInt32 uiNumElements, 
        NiUInt32 uiElementSize, 
        NiShaderAttributeDesc::AttributeType eSubType,
        void* pvLow = NULL, 
        void* pvHigh = NULL);

    NiUInt32 GetAttributeCount() const;

    NiShaderAttributeDesc* GetFirstAttribute() const;
    NiShaderAttributeDesc* GetNextAttribute() const;
    NiShaderAttributeDesc* GetAttributeByName(const NiFixedString& pkName)
        const;

protected:
    NiShaderAttributeDesc* CreateAttributeDesc(
        const NiFixedString& pkName, 
        NiShaderAttributeDesc::AttributeType eType, 
        bool bHidden);

    mutable NiTListIterator m_kAttribListIter;
    NiTPointerList<NiShaderAttributeDesc*> m_kAttribList;
};

#endif  //NID3D10EFFECTATTRIBUTETABLE_H