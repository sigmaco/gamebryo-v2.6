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
#include "CrosswalkGamebryoAnimMap.h"

#include "Model.h"
#include "Mixer.h"
#include "Track.h"
#include "Action.h"
#include "ActionClip.h"

#include "NiAVObject.h"
#include "NiTextKey.h"
#include "NiTextKeyExtraData.h"

//---------------------------------------------------------------------------
//
// Note: in this first incarnation of the animation mixer conversion,
//       it has been agreed to leave aside all the complications related
//       to the mixer: time stretching, time cropping, transitions,
//       superposition, etc.
//
//       Instead, we use the mixer solely to enumerate a list of animation
//       clips that will be output one after the other, with NiTextKey
//       in NiTextKeyExtraData tagged on as extra data on the animated node.
//
//       That was specified inthe original Gamebryo converter request for
//       proposal as point 9.f: "f.	Multiple XSI clips will be appended
//       end-to-end in a single timeline with start frames suitably tagged
//       with the clip name."
//
//       We adopt the same nomenclature as the 3D Studio Max exporter:
//       the start of theanimation clip has "start" has a prefix and its
//       end has "end" as a prefix. For example, for a clip named "Clip1":
//       "startClip1" and "endClip1"
//
//---------------------------------------------------------------------------

namespace
{
    typedef std::map<NiString,epg::ActionTimeline> Timelines;

    //---------------------------------------------------------------------------
    // Create the necessary magic extra data on the given object to support
    // the controller extractor Gamebryo pipeline plugin.
    //---------------------------------------------------------------------------
    void CreateControllerExtractorExtraData(
        NiObjectNET& io_Object,
        const Timelines& in_Timelines)
    {
        const int keyCount = in_Timelines.size() * 2;
        NiTextKey* pDelimiterKeys = NiNew NiTextKey[keyCount];
        int keyIndex = 0;

        typedef Timelines::const_iterator iter;
        for (iter pos = in_Timelines.begin(); pos != in_Timelines.end(); ++pos, keyIndex += 2)
        {
            const char* name = pos->first;
            const epg::ActionTimeline& timeline = pos->second;

            {
                pDelimiterKeys[keyIndex].SetTime(timeline.startTime);
                NiString startText;
                startText.Format("start -name %s", name);
                pDelimiterKeys[keyIndex].SetText(NiFixedString(startText));
            }

            {
                pDelimiterKeys[keyIndex+1].SetTime(timeline.startTime + timeline.duration);
                NiString endText;
                endText.Format("end");
                pDelimiterKeys[keyIndex+1].SetText(NiFixedString(endText));
            }
        }

        NiTextKeyExtraData* pExtraData = NiNew NiTextKeyExtraData;
        pExtraData->SetKeys(pDelimiterKeys, keyCount);
        io_Object.AddExtraData(pExtraData);
    }

    //---------------------------------------------------------------------------
    // Convert an action clip.
    //---------------------------------------------------------------------------
    bool ConvertActionClip(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object,
        CSLActionClip& in_Clip,
        Timelines& io_Timelines)
    {
        epg::AnimationMap& animMap = io_Context.GetAnimationMap();
        if (!animMap.AddAnimAction(io_Context, in_Clip))
            return false;

        // Create the necesary textual description of the animation associated
        // with the action clip.
        const epg::ActionTimeline& timeline = animMap.GetActionTimeline(in_Clip);
        if (timeline.startTime != 0.0f || timeline.duration != 0.0f)
            io_Timelines[in_Clip.Name().GetText()] = timeline;

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert an animation track
    //---------------------------------------------------------------------------
    bool ConvertAnimationTrack(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object,
        CSLTrack& in_Track,
        Timelines& io_Timelines)
    {
        
        SI_Int actionClipCount = in_Track.GetActionClipCount();
        CSLActionClip** pClipArray = in_Track.GetActionClipList();
        if (!actionClipCount || !pClipArray)
            return true;

        for (SI_Int i = 0; i < actionClipCount; ++i)
        {
            CSLActionClip* pClip = pClipArray[i];
            if (!pClip)
                continue;

            if (!ConvertActionClip(io_Context, in_Model, io_Object, *pClip, io_Timelines))
                return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert all animation clips found in the specified model.
    // Also adds the necessary extra data for the controller extractor plugin.
    //---------------------------------------------------------------------------
    bool ConvertAnimationClips(
        Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object)
    {
        // Structure of mixer in XSI (with corresponding Crosswalk type in parantheses):
        //
        // - Mixer (XSI_Mixer)
        //   - Sources 
        //     - Animation
        //       - anim source 1 (XSI_Action)
        //       - ... source anim (XSI_Action)
        //     - Shape
        //       - key shape 1 (XSI_Action)
        //       - ... source shape (XSI_Action)
        //   - Tracks
        //     - Animation
        //       - anim track 1 (XSI_Track)
        //         - clip list
        //           - anim source 1 clip (XSI_ActionClip)
        //             - animated parameter (SI_FCurve)
        //           - ... clip (XSI_ActionClip)
        //             - animated parameter (SI_FCurve)
        //       - ... track (XSI_Track)
        //     - Shape
        //       - shape track 1 (XSI_Track)
        //         - clip list
        //           - key shape 1 clip (XSI_ActionClip)
        //           - ... clip (XSI_ActionClip)
        //       - shape track 2 (XSI_Track)
        //         - clip list
        //           - shape cluster clip (XSI_ActionClipContainer)
        //             - track list
        //               - mixer shape track 1 (XSI_Track)
        //                 - clip list
        //                   - ... clip (XSI_ActionClip)
        //                     - animated parameter (SI_FCurve)
        CSLMixer* pMixer = in_Model.Mixer();
        if (!pMixer)
            return true;

        Timelines timelines;

#ifdef XSI_GAMEBRYO_OLD_MIXER_SUPPORT
        SI_Int actionCount = pMixer->GetActionCount();
        CSLAction** pActionArray = pMixer->GetActionList();
        if (!actionCount || !pActionArray)
            return  true;

        for (SI_Int i = 0; i < actionCount; ++i)
        {
            CSLAction* pAction = pActionArray[i];
            if (!pAction)
                continue;

            // Add all the animation curves found in the action to the animation map.
            epg::AnimationMap& animMap = io_Context.GetAnimationMap();
            if (!animMap.AddAnimAction(io_Context, *pAction))
                return false;

            // Create the necesary textual description of the animation associated
            // with the action
            const epg::ActionTimeline& timeline = animMap.GetActionTimeline(*pAction);
            if (timeline.startTime != 0.0f || timeline.duration != 0.0f)
                timelines[pAction->Name().GetText()] = timeline;
        }
#endif /* XSI_GAMEBRYO_OLD_MIXER_SUPPORT */

        const SI_Int trackCount = pMixer->GetTrackCount();
        CSLTrack** pTrackArray = pMixer->GetTrackList();
        if (!trackCount || !pTrackArray)
            return true;

        for (SI_Int i = 0; i < trackCount; ++i)
        {
            CSLTrack* pTrack = pTrackArray[i];
            if (!pTrack)
                continue;
            if (!ConvertAnimationTrack(io_Context, in_Model, io_Object, *pTrack, timelines))
                return false;
        }

        CreateControllerExtractorExtraData(io_Object, timelines);

        return true;
    }
}
