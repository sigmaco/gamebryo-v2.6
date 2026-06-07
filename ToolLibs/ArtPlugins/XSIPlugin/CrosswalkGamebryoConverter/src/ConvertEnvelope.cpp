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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoSkinMap.h"
#include "CrosswalkGamebryoNodeMap.h"

#include "EnvelopeList.h"
#include "Envelope.h"
#include "Model.h"

#include "NiAVObject.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert all envelopes.
    //---------------------------------------------------------------------------
    bool ConvertEnvelopes(Context& io_Context, CSLEnvelopeList* in_pEnvelopes)
    {
        if (!in_pEnvelopes)
            return true;

        const SI_Int count = in_pEnvelopes->GetEnvelopeCount();
        Phaser phase(io_Context, CONVERT_ENVELOPES_PHASE, count);
        CSLEnvelope** pEnvelopeArray = in_pEnvelopes->Envelopes();
        for (SI_Int i = 0; i < count; ++i)
        {
            phase.Progress();

            CSLEnvelope* pEnv = pEnvelopeArray[i];
            if (!pEnv)
                continue;

            if (!ConvertEnvelope(io_Context, *pEnv))
                return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert an envelope.
    //---------------------------------------------------------------------------
    bool ConvertEnvelope(Context& io_Context, CSLEnvelope& in_Envelope)
    {
        // Make sure envelope contains something.
        CSLModel* pSkinned = in_Envelope.GetEnvelope();
        if (!pSkinned)
            return true;

        CSLModel* pDeformer = in_Envelope.GetDeformer();
        if (!pDeformer)
            return true;

        const SI_Int weightCount = in_Envelope.GetVertexWeightCount();
        SLVertexWeight* weightArray = in_Envelope.GetVertexWeightListPtr();
        if (!weightCount || !weightArray)
            return true;

        // Accumulate the information about this bone for each affected vertex.
        BonesDescriptor& bones = io_Context.GetSkinMap().AddBones(*pSkinned);
        for (SI_Int i = 0; i < weightCount; ++i)
        {
            const int index = (int) ::floor(weightArray[i].m_fVertexIndex + 0.001f);
            const float weight = weightArray[i].m_fWeight;
            bones.AddVertexBone(index, *pDeformer, weight);
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
