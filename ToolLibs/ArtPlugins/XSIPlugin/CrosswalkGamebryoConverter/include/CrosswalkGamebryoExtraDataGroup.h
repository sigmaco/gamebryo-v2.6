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

#ifndef XSI_GAMEBRYO_EXTRA_DATA_GROUP_H
#define XSI_GAMEBRYO_EXTRA_DATA_GROUP_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiFixedString.h"
#include "NiExtraData.h"

#include "CustomPSet.h" 

#include <vector>

namespace epg
{
    //---------------------------------------------------------------------------
    // Structure to keep a vector of extradata.
    //---------------------------------------------------------------------------
    class ExtraDataGroup
    {
    public:
        /// Create a data group of the given name.
        ExtraDataGroup(const char* in_ShaderName, CSLCustomPSet* in_pPSet);

        /// Destroy the data group.
        ~ExtraDataGroup();

        /// Empty the data group.
        void Clear();

        /// Try to combine parameters that have names following known patterns
        /// for vectors, colors, etc, as produced by Crosswalk.
        void CombineParameters();

        /// Get the shader name associated with the data group.
        const char* GetShaderName() const;

        /// Set the shader name associated with the data group.
        void SetShaderName(const char* in_ShaderName);

        /// Get the XSI custom properties associated with the data group.
        CSLCustomPSet* GetCGFXParameters();

        /// Set the XSI custom properties associated with the data group.
        void SetCGFXParameters(CSLCustomPSet* in_pPSet);

        /// Retrieve the number of extra data entries kept in the grouo.
        int GetEntryCount() const;

        /// Retrieve the extra data at the given index.
        NiExtraData* GetEntry(int in_Index);

        /// Add a new extra data to the group.
        void AddEntry(NiExtraData* in_pExtraData);

    private:
        NiFixedString                   m_ShaderName;   // Name of associated shader.
        CSLCustomPSet*                  m_pCGFXParams;  // XSI custom property set.
        std::vector<NiExtraDataPtr>     m_ExtraDatas;   // Vector of extra data.
    };
}

#endif /* XSI_GAMEBRYO_EXTRA_DATA_GROUP_H */
