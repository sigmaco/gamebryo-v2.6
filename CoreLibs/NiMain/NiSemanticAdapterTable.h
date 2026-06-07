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

#ifndef NISEMANTICADAPTERTABLE_H
#define NISEMANTICADAPTERTABLE_H

#include "NiMainLibType.h"
#include "NiFixedString.h"
#include "NiShaderDeclaration.h"
#include "NiTArray.h"

#include <NiMemObject.h>
#include <NiUniversalTypes.h>

class NIMAIN_ENTRY NiSemanticAdapterTable : public NiMemObject
{
public:
    NiSemanticAdapterTable(NiUInt32 uiNumEntries = 4);
    virtual ~NiSemanticAdapterTable();

    NiSemanticAdapterTable& operator=(
        const NiSemanticAdapterTable& kAdapterTable);

    bool operator==(const NiSemanticAdapterTable& kSAT) const;

    void InitializeTable(NiUInt32 uiNumEntries);
    inline NiUInt32 GetNumFilledTableEntries() const;
    inline NiUInt32 GetNumAllocatedTableEntries() const;

    NiUInt32 GetFreeEntry();
    inline void ClearEntry(NiUInt32 uiEntry);

    inline NiFixedString GetGenericSemanticName(NiUInt32 uiEntry) const;
    inline NiUInt8 GetGenericSemanticIndex(NiUInt32 uiEntry) const;
    bool SetGenericSemantic(NiUInt32 uiEntry, 
        const NiFixedString& kName, NiUInt8 uiIndex);

    inline NiFixedString GetRendererSemanticName(NiUInt32 uiEntry) const;
    inline NiUInt8 GetRendererSemanticIndex(NiUInt32 uiEntry) const;
    bool SetRendererSemantic(NiUInt32 uiEntry, 
        const NiFixedString& kName, NiUInt8 uiIndex);

    inline NiUInt8 GetComponentCount(NiUInt32 uiEntry) const;
    bool SetComponentCount(NiUInt32 uiEntry, NiUInt8 uiComponentCount);

    inline NiUInt8 GetSharedRendererSemanticPosition(NiUInt32 uiEntry) const;
    bool SetSharedRendererSemanticPosition(NiUInt32 uiEntry, 
        NiUInt8 uiPosition);

    bool CreateDefaultTable(
        const NiShaderDeclaration::ShaderRegisterEntry* pkSemantics, 
        NiUInt32 uiSemanticEntryCount);

    // Streaming support
    void LoadBinary(NiBinaryStream& kStream);
    void SaveBinary(NiBinaryStream& kStream) const;
protected:

    class SemanticMapping : public NiMemObject
    {
    public:
        SemanticMapping(NiUInt32 uiValue = 0);

        // A bunch of functions that exist so we can use the class in an 
        // NiTPrimitiveArray
        SemanticMapping(const SemanticMapping& kMapping);
        SemanticMapping& operator= (const SemanticMapping& kMapping);

        bool operator== (const SemanticMapping& kMapping) const;
        bool operator!= (const SemanticMapping& kMapping) const;

        // Streaming support
        void LoadBinary(NiBinaryStream& kStream);
        void SaveBinary(NiBinaryStream& kStream) const;

        // generic <-> renderer semantic mapping
        NiFixedString m_kGenericSemantic;
        NiFixedString m_kRendererSemantic;

        NiUInt8 m_uiGenericSemanticIndex;
        NiUInt8 m_uiRendererSemanticIndex;
        NiUInt8 m_uiComponentCount;
        NiUInt8 m_uiSharedRendererSemanticPosition; 
    };

    NiUInt32 m_uiFilledEntries;
    NiTObjectArray<SemanticMapping> m_kAdapterTable;
};

#include "NiSemanticAdapterTable.inl"

#endif //#ifndef NISEMANTICADAPTERTABLE_H
