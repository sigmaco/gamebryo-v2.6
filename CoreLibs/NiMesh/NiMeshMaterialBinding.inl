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

#include "NiMesh.h"

//---------------------------------------------------------------------------
template <typename TContext>
bool NiMeshMaterialBinding::CreateBinding(
    TContext& kContext,
    const NiSemanticAdapterTable& kAdapterTable)
{
    if (!kContext.m_pkMesh)
        return false;

    const NiUInt32 uiTableEntryCount = 
        kAdapterTable.GetNumFilledTableEntries();

    // Track that all entries of adapter table are satisfied.
    bool* abAdapterTableCheckOffList =
        NiStackAlloc(bool, uiTableEntryCount);
    memset((void*)abAdapterTableCheckOffList, 0,
        sizeof(bool) * uiTableEntryCount);

    const NiUInt32 uiStreamCount = kContext.m_pkMesh->GetStreamRefCount();

    kContext.m_uiCurrentStream = 0; // we count vertex streams used
    kContext.m_uiCurrentElement = 0;

    for (NiUInt32 uiStream = 0; uiStream < uiStreamCount; uiStream++)
    {
        const NiDataStreamRef* pkStreamRef = 
            kContext.m_pkMesh->GetStreamRefAt(uiStream);
        const NiDataStream* pkDataStream = pkStreamRef->GetDataStream();

        unsigned int uiElementCount = pkStreamRef->GetElementDescCount();
        unsigned int uiElement = 0; 
        bool bElementsWereAdded = false;
        // uiElement will be incremented inside of this loop
        while (uiElement < uiElementCount)
        {
            NiDataStreamElement::Format eOriginalFormat =
                NiDataStreamElement::F_UNKNOWN;

            const NiDataStreamElement kElement = 
                pkStreamRef->GetElementDescAt(uiElement);

            const NiFixedString& kGenericSemantic = 
                pkStreamRef->GetSemanticNameAt(uiElement);
            NiUInt8 uiGenericSemanticIndex = 
                (NiUInt8)(pkStreamRef->GetSemanticIndexAt(uiElement));

            // Find adapter table entry with this generic semantic and index
            NiUInt32 uiInitialTableEntry = 0;
            for (; uiInitialTableEntry < uiTableEntryCount; 
                uiInitialTableEntry++)
            {
                const NiFixedString& kName = kAdapterTable.
                    GetGenericSemanticName(uiInitialTableEntry);
                const NiUInt32 uiIndex = kAdapterTable.
                    GetGenericSemanticIndex(uiInitialTableEntry);
                if ((kGenericSemantic == kName) &&
                    (uiGenericSemanticIndex == uiIndex))
                {
                    break;
                }
            }

            // If it is not found, skip the entry - the shader doesn't need it
            if (uiInitialTableEntry >= uiTableEntryCount)
            {
                uiElement++;
                continue;
            }

            const NiFixedString& kRendererSemantic = kAdapterTable.
                GetRendererSemanticName(uiInitialTableEntry);
            const NiUInt8 uiRendererSemanticIndex = kAdapterTable.
                GetRendererSemanticIndex(uiInitialTableEntry);

            // Check the stream validity here rather than before examining
            // the stream's elements so errors can be reported.

            // Only consider vertex streams
            if (pkDataStream->GetUsage() != NiDataStream::USAGE_VERTEX)
            {
                NiRenderer::Warning(
                    "%s> Generic semantic %s:%d requested by "
                    "semantic adapter table for mesh "
                    "(pointer: 0x%X, name: %s) "
                    "found on data stream whose usage is not USAGE_VERTEX. "
                    "This semantic may fail to be found.",
                    __FUNCTION__,
                    (const char*) kGenericSemantic, 
                    uiGenericSemanticIndex,
                    kContext.m_pkMesh, 
                    (const char*) kContext.m_pkMesh->GetName());
                uiElement++;
                continue; 
            }

            // Require GPU Read
            if ((pkDataStream->GetAccessMask() & 
                NiDataStream::ACCESS_GPU_READ) == 0)
            {
                NiRenderer::Warning(
                    "%s> Generic semantic %s:%d requested by "
                    "semantic adapter table for mesh "
                    "(pointer: 0x%X, name: %s) "
                    "found on data stream whose access mask does not include "
                    "ACCESS_GPU_READ. "
                    "This semantic may fail to be found.",
                    __FUNCTION__,
                    (const char*) kGenericSemantic, 
                    uiGenericSemanticIndex,
                    kContext.m_pkMesh, 
                    (const char*) kContext.m_pkMesh->GetName());
                uiElement++;
                continue;
            }

            // No more than 4 entries can share a semantic/index pair
            // (most you can have is one float each for x, y, z, w)
            const NiUInt32 uiMaxNumSharedEntries = 4;
            NiUInt32 auiTableEntries[uiMaxNumSharedEntries];
            memset(auiTableEntries, UINT_MAX, sizeof(auiTableEntries));

            // Find all other adapter table entries using the same renderer 
            // semantic and index. We will need to pack together with them.
            NiUInt32 uiNumSharedEntries = 0;
            for (NiUInt32 i = 0; i < uiTableEntryCount; i++)
            {
                // If we've found another with same renderer semantic
                if (kAdapterTable.GetRendererSemanticName(i) == 
                    kRendererSemantic &&
                    kAdapterTable.GetRendererSemanticIndex(i) == 
                    uiRendererSemanticIndex)
                {
                    // The position of this entry within the final
                    // semantic/index must be within the number of possible
                    // positions.
                    NIASSERT(kAdapterTable.
                        GetSharedRendererSemanticPosition(i) < 
                        uiMaxNumSharedEntries);

                    // Check off this entry as found.
                    abAdapterTableCheckOffList[i] = true;

                    // Add to table tracking where it is packed.
                    auiTableEntries[kAdapterTable.
                        GetSharedRendererSemanticPosition(i)] = i;
                    uiNumSharedEntries++;
                }
            }

            NIASSERT(uiNumSharedEntries > 0);

            // Reconcile multiple entries sharing a renderer semantic/index
            NiDataStreamElement::Type ePackedDataFormatType = 
                NiDataStreamElement::T_UNKNOWN;
            bool bPackedDataFormatTypeNormalized = false;
            NiUInt32 uiPackedDataFormatComponentCount = 0;
            NiUInt32 uiPackedOffset = 0;
            for (NiUInt32 i = 0; i < uiNumSharedEntries; i++)
            {
                // Ensure there are no holes in the positions of the shared
                // table entries
                NiUInt32 uiCurrentTableEntry = auiTableEntries[i];
                if (uiCurrentTableEntry > uiTableEntryCount)
                {
                    NiRenderer::Warning(
                        "%s> Malformed semantic adapter table for "
                        "mesh (pointer: 0x%X, name: %s): "
                        "Positions of shared table entries for renderer "
                        "semantic %s:%d don't have values "
                        "that increase monotonically starting with 0.\n"
                        "    Failing the mesh material binding.",
                        __FUNCTION__,
                        kContext.m_pkMesh, 
                        (const char*) kContext.m_pkMesh->GetName(), 
                        (const char*) kRendererSemantic, 
                        uiRendererSemanticIndex);
                    return false;
                }

                // The ordering of the shared entries in the adapter table
                // (according to the SharedRendererSemanticPosition) must 
                // match the ordering of the associated elements in the 
                // NiDataStream
                // This is incrementing the uiElement loop counter; this is
                // intentional, since the shared elements will not need to be
                // be re-evaluated in the higher-level loop.
                NiDataStreamElement kElem = 
                    pkStreamRef->GetElementDescAt(uiElement);

                const NiFixedString& kCurrentGenericSemantic = 
                    pkStreamRef->GetSemanticNameAt(uiElement);

                NiUInt32 uiCurrentGenericSemanticIndex =
                    pkStreamRef->GetSemanticIndexAt(uiElement);

                uiElement++;

                if (!kElem.IsValid() 
                    || 
                    kCurrentGenericSemantic !=  
                    kAdapterTable.GetGenericSemanticName(uiCurrentTableEntry) 
                    ||
                    uiCurrentGenericSemanticIndex !=
                    kAdapterTable.GetGenericSemanticIndex(uiCurrentTableEntry)
                    )
                {
                    NiRenderer::Warning(
                        "%s> Malformed semantic adapter table for "
                        "mesh (pointer: 0x%X, name: %s): "
                        "Positions of shared table entries for renderer "
                        "semantic %s:%d aren't ordered so "
                        "they match the order of the elements in the "
                        "NiDataStream.\n"
                        "    Failing the mesh material binding.",
                        __FUNCTION__,
                        kContext.m_pkMesh, 
                        (const char*) kContext.m_pkMesh->GetName(), 
                        (const char*) kRendererSemantic, 
                        uiRendererSemanticIndex);
                    return false;
                }

                // Obtain the data format for this element
                NiDataStreamElement::Type eElementDataFormatType = 
                    kElem.GetType();
                bool bElementDataFormatNormalized = kElem.IsNormalized();
                NiUInt32 uiAdapterTableDataFormatComponentCount = 
                    kAdapterTable.GetComponentCount(uiCurrentTableEntry);
                NiUInt32 uiElementDataFormatComponentCount = 0;

                // Ensure component count is correct - if the element is 
                // packed, GetComponentCount() will always report a component 
                // count of one. So for that case we will use 
                // GetFieldCount() instead to determine the number of 
                // components.
                if (!kElem.IsPacked())
                {
                    uiElementDataFormatComponentCount = 
                        kElem.GetComponentCount();
                }
                else
                {
                    uiElementDataFormatComponentCount = kElem.GetFieldCount();

                    if (kElem.IsLastFieldLiteral())
                        uiElementDataFormatComponentCount--;
                }

                if (uiAdapterTableDataFormatComponentCount > 
                    uiElementDataFormatComponentCount)
                {
                    NiRenderer::Warning(
                        "%s> "
                        "Shader expected values missing from the data "
                        "stream. Semantic adapter table for mesh "
                        "(pointer: 0x%X, name: %s) expected"
                        "renderer semantic %s:%d to have %d components per "
                        "element, but the corresponding NiDataStreamElement "
                        "only has %d components. ",
                        __FUNCTION__,
                        kContext.m_pkMesh,
                        (const char*) kContext.m_pkMesh->GetName(), 
                        (const char*) kRendererSemantic, 
                        uiRendererSemanticIndex,
                        uiAdapterTableDataFormatComponentCount,
                        uiElementDataFormatComponentCount);
                }
                else if (uiAdapterTableDataFormatComponentCount < 
                    uiElementDataFormatComponentCount)
                {
#if 0
                    // NOTE: This warning message can be especially verbose
                    // and could occur as the result of a desired mesh
                    // profile and shader combination. It also is not
                    // severe, indicating only potential memory waste. For 
                    // these reasons, it is compiled out by default.

                    NiRenderer::Warning(
                        "%s> "
                        "Shader is not using values from the data "
                        "stream. Should shader use these, or is there "
                        "wasted memory? Semantic adapter table for "
                        "mesh (pointer: 0x%X, name: %s) has "
                        "renderer semantic %s:%d using only %d out of %d "
                        "components provided by the corresponding "
                        "NiDataStreamElement. ",
                        __FUNCTION__,
                        kContext.m_pkMesh, 
                        (const char*) kContext.m_pkMesh->GetName(), 
                        (const char*) kRendererSemantic, 
                        uiRendererSemanticIndex,
                        uiAdapterTableDataFormatComponentCount,
                        uiElementDataFormatComponentCount);
#endif
                }

                if (i == 0)
                {
                    // First entry - simply record data format and offset
                    eOriginalFormat = kElem.GetFormat();
                    ePackedDataFormatType = eElementDataFormatType;
                    bPackedDataFormatTypeNormalized = 
                        bElementDataFormatNormalized;
                    uiPackedDataFormatComponentCount = 
                        uiAdapterTableDataFormatComponentCount;
                    uiPackedOffset = kElem.GetOffset();
                }
                else
                {
                    // Subsequent entries - ensure data format matches
                    if (ePackedDataFormatType != eElementDataFormatType ||
                        bPackedDataFormatTypeNormalized != 
                        bElementDataFormatNormalized)
                    {
                        NiRenderer::Warning(
                            "%s> "
                            "Malformed semantic adapter table for "
                            "mesh (pointer: 0x%X, name: %s): "
                            "Shared table entries for renderer "
                            "semantic %s:%d don't have a common data "
                            "format.\n"
                            "    Failing the mesh material binding.",
                            __FUNCTION__,
                            kContext.m_pkMesh, 
                            (const char*) kContext.m_pkMesh->GetName(), 
                            (const char*) kRendererSemantic, 
                            uiRendererSemanticIndex);
                        return false;
                    }

                    // Calculate new component count
                    uiPackedDataFormatComponentCount += 
                        uiAdapterTableDataFormatComponentCount;

                    if (uiPackedDataFormatComponentCount >
                        uiMaxNumSharedEntries)
                    {
                        NiRenderer::Warning(
                            "%s> "
                            "Malformed semantic adapter table for "
                            "mesh (pointer: 0x%X, name: %s): "
                            "Shared table entries for renderer "
                            "semantic %s:%d have too many combined "
                            "components (%d) to fit into a single semantic "
                            "(max %d)."
                            "\n"
                            "    Failing the mesh material binding.",
                            __FUNCTION__,
                            kContext.m_pkMesh, 
                            (const char*) kContext.m_pkMesh->GetName(), 
                            (const char*) kRendererSemantic, 
                            uiRendererSemanticIndex,
                            uiPackedDataFormatComponentCount,
                            uiMaxNumSharedEntries);
                        return false;
                    }
                }
            }

            NiDataStreamElement::Format ePackedDataFormat;
            if (uiNumSharedEntries == 1)
            {
                ePackedDataFormat = eOriginalFormat;
            }
            else
            {
                ePackedDataFormat = 
                    NiDataStreamElement::GetPredefinedFormat(
                    ePackedDataFormatType,
                    (NiUInt8)uiPackedDataFormatComponentCount, 
                    bPackedDataFormatTypeNormalized);
            }

            // Add new entry
            if (!kContext.CallBack_EndOfElementLoop(
                uiStream,
                pkStreamRef,
                kElement,
                ePackedDataFormat, 
                kRendererSemantic, 
                uiRendererSemanticIndex, 
                uiPackedDataFormatComponentCount, 
                uiPackedOffset))
            {
                return false;
            }

            // must update AFTER used to fill in element
            bElementsWereAdded = true;
            kContext.m_uiCurrentElement++;
        }

        // must update AFTER used to fill in element
        if (bElementsWereAdded)
            kContext.m_uiCurrentStream++; 
    }

    // Check that each adapter table entry was satisfied
    bool bAllEntriesSatisfied = true;
    for (unsigned int ui = 0; ui < uiTableEntryCount; ui++)
    {
        if (abAdapterTableCheckOffList[ui])
            continue;

        if (NiMeshMaterialBinding::GetCreateMissingStreams())
        {
            float fValue = (kAdapterTable.GetGenericSemanticName(ui) ==
                NiCommonSemantics::COLOR()) ? 1.0f : 0.0f;
            float afValues[4] = { fValue, fValue, fValue, fValue };

            NiDataStreamRef* pkRef = ((NiMesh*)kContext.m_pkMesh)->AddStream(
                kAdapterTable.GetGenericSemanticName(ui),
                kAdapterTable.GetGenericSemanticIndex(ui),
                NiDataStreamElement::F_FLOAT32_4,
                1, 
                NiDataStream::ACCESS_CPU_WRITE_STATIC | 
                    NiDataStream::ACCESS_GPU_READ, 
                NiDataStream::USAGE_VERTEX,
                afValues);

            pkRef->GetDataStream()->SetGPUConstantSingleEntry(true);

            if (kContext.CallBack_EndOfElementLoop(
                kContext.m_pkMesh->GetStreamRefCount() - 1,
                pkRef,
                pkRef->GetElementDescAt(0),
                NiDataStreamElement::F_FLOAT32_4, 
                kAdapterTable.GetRendererSemanticName(ui), 
                kAdapterTable.GetRendererSemanticIndex(ui), 
                4, 
                0))
            {
                kContext.m_uiCurrentStream++;
                kContext.m_uiCurrentElement++;

                NiRenderer::Warning(
                    "%s> "
                    "Semantic adapter table default data added to "
                    "mesh (pointer: 0x%X, name: %s): "
                    "for generic semantic %s:%d.",
                    __FUNCTION__,
                    kContext.m_pkMesh, 
                    (const char*) kContext.m_pkMesh->GetName(), 
                    (const char*) kAdapterTable.GetGenericSemanticName(ui), 
                    kAdapterTable.GetGenericSemanticIndex(ui));

                continue;
            }
        }

        bAllEntriesSatisfied = false;
        NiRenderer::Warning(
            "%s> "
            "Semantic adapter table required data not found on "
            "mesh (pointer: 0x%X, name: %s): "
            "Could not find generic semantic %s:%d.",
            __FUNCTION__,
            kContext.m_pkMesh, 
            (const char*) kContext.m_pkMesh->GetName(), 
            (const char*) kAdapterTable.GetGenericSemanticName(ui), 
            kAdapterTable.GetGenericSemanticIndex(ui));
    }

    if (!bAllEntriesSatisfied)
    {
        NiRenderer::Warning("    Failing the mesh material binding.");
    }

    NiStackFree(abAdapterTableCheckOffList);

    return bAllEntriesSatisfied;
}
//---------------------------------------------------------------------------
