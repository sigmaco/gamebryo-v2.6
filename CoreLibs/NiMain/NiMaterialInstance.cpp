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

#include "NiRenderObject.h"
#include "NiMaterialInstance.h"

//---------------------------------------------------------------------------
NiShader* NiMaterialInstance::GetCachedShader(
    const NiRenderObject* pkGeometry,
    const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects) const
{
    if (m_spCachedShader)
    {
        if (m_eNeedsUpdate == DIRTY || (m_eNeedsUpdate == UNKNOWN && 
            pkGeometry->GetMaterialNeedsUpdateDefault()))
        {
            return NULL;
        }

        if (!m_spMaterial || m_spMaterial->IsShaderCurrent(m_spCachedShader, 
            pkGeometry, pkState, pkEffects, m_uiMaterialExtraData))
        {
            return m_spCachedShader;
        }
    }    

    return NULL;
}
//---------------------------------------------------------------------------
NiShader* NiMaterialInstance::GetCurrentShader(NiRenderObject* pkGeometry, 
    const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects)
{
    if (m_spMaterial)
    {
        bool bGetNewShader = m_eNeedsUpdate == DIRTY;

        if (m_eNeedsUpdate == UNKNOWN)
            bGetNewShader = pkGeometry->GetMaterialNeedsUpdateDefault();

        // Check if shader is still current
        if (bGetNewShader && m_spCachedShader)
        {
            bGetNewShader = !m_spMaterial->IsShaderCurrent(m_spCachedShader, 
                pkGeometry, pkState, pkEffects, m_uiMaterialExtraData);
        }

        // Get a new shader
        if (bGetNewShader) 
        {
            NiShader* pkNewShader = m_spMaterial->GetCurrentShader(
                pkGeometry, pkState, pkEffects, m_uiMaterialExtraData);

            if (pkNewShader)
            {
                NIASSERT(m_spCachedShader != pkNewShader);

                ClearCachedShader();
                m_spCachedShader = pkNewShader;

                if (!pkNewShader->SetupGeometry(pkGeometry, this))
                    ClearCachedShader();
            }
            else
            {
                ClearCachedShader();
            }
        }

        m_eNeedsUpdate = UNKNOWN;
    }

    return m_spCachedShader;
}
//---------------------------------------------------------------------------
void NiMaterialInstance::UpdateSemanticAdapterTable(NiRenderObject* pkMesh)
{
    if (!m_spCachedShader)
        return;

    // Initialize first with shader's adapter table
    m_kAdapterTable = m_spCachedShader->GetSemanticAdapterTable();

    // Check to see if table will work with mesh
    const NiUInt32 uiEntryCount = 
        m_kAdapterTable.GetNumAllocatedTableEntries();
    bool bMismatch = false;

    // Statistics from Gamebryo samples and demos suggests that the typical
    // entry count for an adapter table is 3 or 4, with a maximum of 10 seen
    // across all demos. In this application we need a map, but for the sake
    // of memory usage, and possibly even efficiency, we will use an array
    // instead.

    // This array will count the semantics present in the adapter table.
    // The i'th entry is either a positive number, indicating the count
    // for the semantic name in the i'th adapter table entry, or a negative
    // number indicating the entry to look at for the first instance of the
    // i'th semantic name in the adapter table. Later, rather than a count,
    // the entry for the first occurance of a semantic name will have the
    // sematic index to start at when searching for a new available semantic.
    NiInt16* aiMap = NiStackAlloc(NiInt16, uiEntryCount);
    NiUInt32 uiAdapterEntryIndex = 0;
    for (; uiAdapterEntryIndex < uiEntryCount; uiAdapterEntryIndex++)
    {
        NiFixedString kGenericSemantic = 
            m_kAdapterTable.GetGenericSemanticName(uiAdapterEntryIndex);

        // If entry not filled, don't look for it in the mesh.
        if (!kGenericSemantic.Exists())
        {
            // Put a reference to itself in the count array.
            aiMap[uiAdapterEntryIndex] = -(NiInt16)uiAdapterEntryIndex;
            continue;
        }
        
        // If exact match is not found, record a mismatch
        if (!pkMesh->ContainsData(kGenericSemantic, 
            m_kAdapterTable.GetGenericSemanticIndex(uiAdapterEntryIndex)))
        {
            bMismatch = true;
        }

        // Count the number of occurances of each semantic
        NiUInt32 uiIndex = 0;
        for ( ; uiIndex < uiAdapterEntryIndex; uiIndex++)
        {
            NiInt16 iCount = aiMap[uiIndex];
            if (iCount > 0 && kGenericSemantic ==
                m_kAdapterTable.GetGenericSemanticName(uiIndex))
            {
                aiMap[uiIndex]++;
                aiMap[uiAdapterEntryIndex] = -(NiInt16)uiIndex;
                break;
            }
                
        }
        if (uiIndex == uiAdapterEntryIndex)
            aiMap[uiAdapterEntryIndex] = 1;
    }

    // If no mismatch, then this adapter table is OK
    if (!bMismatch)
    {
        NiStackFree(aiMap);
        return;
    }

    // If there is a mismatch, need to reorganize table. This will involve
    // changing semantic indices, but not the semantic name itself.

    // First check to see if the mesh has the appropriate number of each
    // semantic name
    for (uiAdapterEntryIndex = 0; uiAdapterEntryIndex < uiEntryCount;
        uiAdapterEntryIndex++)
    {
        NiInt16 iCount = aiMap[uiAdapterEntryIndex];
        if (iCount > 0)
        {
            // Check if adapter table had more of a given semantic than the
            // mesh
            if ((NiUInt32)iCount > pkMesh->GetSemanticCount(
                m_kAdapterTable.GetGenericSemanticName(uiAdapterEntryIndex)))
            {
                // Can't modify the adapter table to work with the shader
                NiStackFree(aiMap);
                return;
            }
        }
    }

    // Now allocate indexes to semantics.
    for (uiAdapterEntryIndex = 0; uiAdapterEntryIndex < uiEntryCount;
        uiAdapterEntryIndex++)
    {
        NiInt16 iCount = aiMap[uiAdapterEntryIndex];
        NiUInt32 uiNewIndex = 0;
        NiUInt16 uiMapLocation = 0;
        if (iCount > 0)
        {
            // First occurance of this semantic
            uiNewIndex = 0;
            uiMapLocation = (NiUInt16)uiAdapterEntryIndex;
        }
        else
        {
            uiMapLocation = -iCount;
            uiNewIndex = aiMap[uiMapLocation];
        }
        
        NiFixedString kGenericSemantic = 
            m_kAdapterTable.GetGenericSemanticName(uiAdapterEntryIndex);

        // We know that an additional semantic match will be found because
        // of the previous verification.
        while (!pkMesh->ContainsData(kGenericSemantic, uiNewIndex))
        {
            // Check for some reasonable upper bound
            NIASSERT(uiNewIndex < 256)
            uiNewIndex++;
        }

        m_kAdapterTable.SetGenericSemantic(uiAdapterEntryIndex,
            kGenericSemantic, (NiUInt8)uiNewIndex);

        aiMap[uiMapLocation] = (NiInt16)(uiNewIndex + 1);
    }

    NiStackFree(aiMap);
}
//---------------------------------------------------------------------------
