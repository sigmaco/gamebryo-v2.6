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

#ifndef NID3DXEFFECTATTRIBUTETABLE_H
#define NID3DXEFFECTATTRIBUTETABLE_H

#include "NiD3DXEffectShaderLibType.h"

#include <NiShaderAttributeDesc.h>
#include <NiTPointerList.h>

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectAttributeTable : public NiMemObject
{
public:
    NiD3DXEffectAttributeTable();
    ~NiD3DXEffectAttributeTable();

    bool AddAttribDesc(NiShaderAttributeDesc* pkDesc);

    bool AddAttribDesc_Bool(const char* pcName, bool bHidden, 
        bool bInitValue);
    bool AddAttribDesc_String(const char* pcName, bool bHidden, 
        char* pcInitValue, unsigned int uiMaxLen = 0);
    bool AddAttribDesc_UnsignedInt(const char* pcName, bool bHidden, 
        unsigned int uiInitValue, unsigned int uiLow = 0, 
        unsigned int uiHigh = 0);
    bool AddAttribDesc_Float(const char* pcName, bool bHidden, 
        float fInitValue, float fLow = 0.0f, float fHigh = 0.0f);
    bool AddAttribDesc_Point2(const char* pcName, bool bHidden, 
        float* pfInitValue, float* pfLow = NULL, float* pfHigh = NULL);
    bool AddAttribDesc_Point3(const char* pcName, bool bHidden, 
        float* pfInitValue, float* pfLow = NULL, float* pfHigh = NULL);
    bool AddAttribDesc_Point4(const char* pcName, bool bHidden, 
        float* pfInitValue, float* pfLow = NULL, float* pfHigh = NULL);
    bool AddAttribDesc_Matrix3(const char* pcName, bool bHidden, 
        float* pfInitValue);
    bool AddAttribDesc_Matrix4(const char* pcName, bool bHidden, 
        float* pfInitValue);
    bool AddAttribDesc_Color(const char* pcName, bool bHidden, 
        float* pfInitValue, float* pfLow = NULL, float* pfHigh = NULL);
    bool AddAttribDesc_ColorA(const char* pcName, bool bHidden, 
        float* pfInitValue, float* pfLow = NULL, float* pfHigh = NULL);
    bool AddAttribDesc_Texture(const char* pcName, bool bHidden, 
        unsigned int uiSlot, const char* pcDefault = NULL);
    bool AddAttribDesc_Array(const char* pcName, bool bHidden, 
        void* pInitValue, unsigned int uiNumElements, 
        unsigned int uiElementSize, 
        NiShaderAttributeDesc::AttributeType eSubType,
        void* pLow = NULL, void* pHigh = NULL);

    unsigned int GetAttributeCount();

    NiShaderAttributeDesc* GetFirstAttribute();
    NiShaderAttributeDesc* GetNextAttribute();
    NiShaderAttributeDesc* GetAttributeByName(const char* pcName);

protected:
    NiShaderAttributeDesc* CreateAttributeDesc(const char* pcName, 
        NiShaderAttributeDesc::AttributeType eType, bool bHidden);

    NiTListIterator m_kAttribListIter;
    NiTPointerList<NiShaderAttributeDesc*> m_kAttribList;
};

#endif  //NID3DXEFFECTATTRIBUTETABLE_H