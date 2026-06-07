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

#include "CrosswalkGamebryoExtraDataGroup.h"

#include "NiString.h"
#include "NiBooleanExtraData.h"
#include "NiIntegerExtraData.h"
#include "NiIntegersExtraData.h"
#include "NiFloatExtraData.h"
#include "NiFloatsExtraData.h"
#include "NiStringExtraData.h"
#include "NiVectorExtraData.h"
#include "NiColorExtraData.h"
#include "NiPoint3.h"

#include <algorithm>

namespace
{
    //---------------------------------------------------------------------------
    // Maximum number of values to combine.
    //---------------------------------------------------------------------------
    enum { MAX_VALUE_COMBINATION_COUNT = 4 };

    //---------------------------------------------------------------------------
    // The type of extra data grouping that is expected.
    //---------------------------------------------------------------------------
    enum ParameterGroupType
    {
        Vector4GroupType,
        Vector3GroupType,
        Color4GroupType,
        Color3GroupType,
        InvalidGroupType
    };

    //---------------------------------------------------------------------------
    // Find a parameter with a given name.
    //---------------------------------------------------------------------------
    int FindExtraData(
        const std::vector<NiExtraDataPtr>& in_Datas,
        const NiString& in_Name,
        float& out_Value)
    {
        for (std::size_t i = 0; i < in_Datas.size(); ++i)
            if (in_Datas[i] && NiString(in_Datas[i]->GetName()) == in_Name)
            {
                if (NiFloatExtraData* pFloatData = NiDynamicCast(NiFloatExtraData, in_Datas[i]))
                {
                    out_Value = pFloatData->GetValue();
                    return i;
                }
            }

        return -1;
    }

    //---------------------------------------------------------------------------
    // Try to extract the parameter group.
    //---------------------------------------------------------------------------
    bool ExtractParameters(
        std::vector<NiExtraDataPtr>& in_Datas,
        const NiString& in_BaseName,
        const char* const* in_NameSuffixes,
        ParameterGroupType in_GroupType)
    {
        // Find the parameters in the groups and their values.
        int indexes[MAX_VALUE_COMBINATION_COUNT]  = { 0 };
        float values[MAX_VALUE_COMBINATION_COUNT] = { 0.0f };

        for (int i = 0; i < MAX_VALUE_COMBINATION_COUNT; ++i)
        {
            if (in_NameSuffixes[i])
            {
                NiString name = in_BaseName + NiString(in_NameSuffixes[i]);
                indexes[i] = FindExtraData(in_Datas, name, values[i]);
                if (-1 == indexes[i])
                    return false;
            }
            else
            {
                indexes[i] = -1;
            }
        }

        // Create the new extra data combining the parameters.
        NiExtraDataPtr pNewData = 0;
        switch (in_GroupType)
        {
            case Vector4GroupType:
            {
                pNewData = NiNew NiVectorExtraData(values);
                break;
            }
            case Vector3GroupType:
            {
                pNewData = NiNew NiVectorExtraData(NiPoint3(values[0], values[1], values[2]));
                break;
            }
            case Color4GroupType:
            {
                pNewData = NiNew NiColorExtraData(NiColorA(values[0], values[1],
                                                           values[2], values[3]));
                break;
            }
            case Color3GroupType:
            {
                pNewData = NiNew NiColorExtraData(NiColor(values[0], values[1], values[2]));
                break;
            }
            default:
                return false;
        }

        // Note: remove in revserse order to avoid messing with later indexes
        //       when erasing elements, since this is a vector.
        std::sort(indexes, indexes+MAX_VALUE_COMBINATION_COUNT);
        for (int i = MAX_VALUE_COMBINATION_COUNT-1; i >= 0; --i)
        {
            const int index = indexes[i];
            if (index != -1)
            {
                in_Datas.erase(in_Datas.begin() + index);
            }
        }

        // Add the new data and set its name to the common base name.
        if (pNewData)
        {
            pNewData->SetName(NiFixedString(in_BaseName));
            in_Datas.push_back(pNewData);
        }

        return true;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    /// Create a data group of the given name.
    //---------------------------------------------------------------------------
    ExtraDataGroup::ExtraDataGroup(const char* in_ShaderName, CSLCustomPSet* in_pPSet)
        : m_ShaderName(in_ShaderName)
        , m_pCGFXParams(in_pPSet) 
    {
    }

    //---------------------------------------------------------------------------
    /// Destroy the data group.
    //---------------------------------------------------------------------------
    ExtraDataGroup::~ExtraDataGroup()
    {
        Clear();
    }

    //---------------------------------------------------------------------------
    /// Empty the data group.
    //---------------------------------------------------------------------------
    void ExtraDataGroup::Clear()
    {
        m_ExtraDatas.clear();
    }

    //---------------------------------------------------------------------------
    /// Get the shader name associated with the data group.
    //---------------------------------------------------------------------------
    const char* ExtraDataGroup::GetShaderName() const
    {
        return m_ShaderName;
    }

    //---------------------------------------------------------------------------
    /// Set the shader name associated with the data group.
    //---------------------------------------------------------------------------
    void ExtraDataGroup::SetShaderName(const char* in_ShaderName)
    {
        m_ShaderName = in_ShaderName;
    }
                                    
    //---------------------------------------------------------------------------
    /// Get the XSI custom properties associated with the data group.
    //---------------------------------------------------------------------------
    CSLCustomPSet* ExtraDataGroup::GetCGFXParameters()
    {
        return m_pCGFXParams;
    }

    //---------------------------------------------------------------------------
    /// Set the XSI custom properties associated with the data group.
    //---------------------------------------------------------------------------
    void ExtraDataGroup::SetCGFXParameters(CSLCustomPSet* in_pPSet)
    {
        m_pCGFXParams = in_pPSet;
    } 

    //---------------------------------------------------------------------------
    /// Retrieve the number of extra data entries kept in the grouo.
    //---------------------------------------------------------------------------
    int ExtraDataGroup::GetEntryCount() const
    {
        return m_ExtraDatas.size();
    }

    //---------------------------------------------------------------------------
    /// Retrieve the extra data at the given index.
    //---------------------------------------------------------------------------
    NiExtraData* ExtraDataGroup::GetEntry(int in_Index)
    {
        if (in_Index < 0 || in_Index >= GetEntryCount())
            return 0;

        return m_ExtraDatas[in_Index];
    }

    //---------------------------------------------------------------------------
    /// Add a new extra data to the group.
    //---------------------------------------------------------------------------
    void ExtraDataGroup::AddEntry(NiExtraData* in_pExtraData)
    {
        m_ExtraDatas.push_back(in_pExtraData);
    }

    //---------------------------------------------------------------------------
    /// Try to combine parameters that have names following known patterns
    /// for vectors, colors, etc, as produced by Crosswalk.
    //---------------------------------------------------------------------------
    void ExtraDataGroup::CombineParameters()
    {
        for (std::size_t i = 0; i < m_ExtraDatas.size(); ++i)
        {
            NiExtraData* pData = m_ExtraDatas[i];
            if (!pData)
                continue;

            struct Pattern
            {
                ParameterGroupType type;
                const char* suffixes[MAX_VALUE_COMBINATION_COUNT];
                int trim;
            };
            const Pattern patterns[] =
            {
                { Color4GroupType,  { "Color_X", "Color_Y", "Color_Z", "Color_W" }, 2 },
                { Color3GroupType,  { "Color_X", "Color_Y", "Color_Z",        0  }, 2 },
                { Vector4GroupType, {      "_X",      "_Y",      "_Z",      "_W" }, 2 },
                { Vector3GroupType, {      "_X",      "_Y",      "_Z",        0  }, 2 },
                { InvalidGroupType, {        0 ,        0 ,        0 ,        0  }, 0 }
            };

            const NiString name = pData->GetName();
            for (const Pattern* pat = patterns; pat->suffixes[0]; ++pat)
            {
                NiString suffix = name.Right(strlen(pat->suffixes[0]));
                if (suffix == NiString(pat->suffixes[0]))
                {
                    NiString baseName = name.Left(name.Length() - pat->trim);
                    if (ExtractParameters(m_ExtraDatas, baseName, pat->suffixes, pat->type))
                    {
                        --i;
                        break;
                    }
                }
            }
        }
    }
}

