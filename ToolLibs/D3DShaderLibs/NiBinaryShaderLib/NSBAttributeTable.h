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

#ifndef NSBATTRIBUTETABLE_H
#define NSBATTRIBUTETABLE_H

#include <NiShaderAttributeDesc.h>
#include <NiTPointerList.h>
#include "NiBinaryShaderLibLibType.h"

class NSBAttributeDesc;

class NIBINARYSHADERLIB_ENTRY NSBAttributeTable : public NiMemObject
{
public:
    NSBAttributeTable();
    ~NSBAttributeTable();

    bool AddAttribDesc(NSBAttributeDesc* pkDesc);

    bool AddAttribDesc_Bool(char* pcName, char* pcDesc, bool bHidden,
        bool bInitValue);
    bool AddAttribDesc_String(char* pcName, char* pcDesc, bool bHidden,
        char* pcInitValue, unsigned int uiMaxLen = 0);
    bool AddAttribDesc_UnsignedInt(char* pcName, char* pcDesc, 
        bool bHidden, unsigned int uiInitValue, unsigned int uiLow = 0,
        unsigned int uiHigh = 0);
    bool AddAttribDesc_Float(char* pcName, char* pcDesc, bool bHidden, 
        float fInitValue, float fLow = 0.0f, float fHigh = 0.0f);
    bool AddAttribDesc_Point2(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue, float* pfLow = 0, float* pfHigh = 0);
    bool AddAttribDesc_Point3(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue, float* pfLow = 0, float* pfHigh = 0);
    bool AddAttribDesc_Point4(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue, float* pfLow = 0, float* pfHigh = 0);
    bool AddAttribDesc_Matrix3(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue);
    bool AddAttribDesc_Matrix4(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue);
    bool AddAttribDesc_Color(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue, float* pfLow = 0, float* pfHigh = 0);
    bool AddAttribDesc_ColorA(char* pcName, char* pcDesc, bool bHidden, 
        float* pfInitValue, float* pfLow = 0, float* pfHigh = 0);
    bool AddAttribDesc_Texture(char* pcName, char* pcDesc, bool bHidden, 
        unsigned int uiSlot, const char* pcDefault = 0);
    bool AddAttribDesc_Array(
        char* pcName,
        char* pcDesc,
        bool bHidden, 
        NiShaderAttributeDesc::AttributeType eType,
        unsigned int uiArraySize,
        float* pfInitValue);

    unsigned int GetAttributeCount();

    NSBAttributeDesc* GetFirstAttribute();
    NSBAttributeDesc* GetNextAttribute();
    NSBAttributeDesc* GetAttributeByName(char* pcName);

    // *** begin Emergent internal use only
    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    void Dump(FILE* pf);
    void DumpAttribDesc(FILE* pf, NSBAttributeDesc* pkDesc);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NSBAttributeDesc* CreateAttributeDesc(char* pcName, 
        NiShaderAttributeDesc::AttributeType eType, bool bHidden);

    NiTListIterator m_kAttribListIter;
    NiTPointerList<NSBAttributeDesc*> m_kAttribList;
};

#endif  //NSBATTRIBUTETABLE_H
