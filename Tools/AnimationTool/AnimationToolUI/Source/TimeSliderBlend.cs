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

using System;

using System.Diagnostics;

using SegmentedTrackBarDLL;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for TimeSliderBlend.
    /// </summary>
    public class TimeSliderBlend : TimeSliderTransitionInfo
    {
        internal float fSrcTimeUsingFirstKey = 0.0f;
        internal float fDestFrameUsingFirstKey = 0.0f;
        internal float fTimeLastKeyToFirstKey = 0.0f;
        internal MarkerItem m_BlendPairMarker = null;

        internal TimeSliderBlend(TimeSlider ts) : base(ts) {}

        protected void CreateBlendRegions(
            float fSrcTime, float fDuration, float fDestTime)
        {
            int pos = 0;
            int w = (int)(fSrcTime * TrackRangeOverTotalTime);

            string strText;
            float fSrcEndTime = SrcSeq.DurationDivFreq;
            strText = SrcSeq.Name + "\n" + 
                      strFullSeq + fSrcEndTime.ToString("f3");               
            SegTrk.AddRegion(pos, w, ColorSrcA, ColorSrcB, strText, 0);

            pos = w;
            w = (int)(fDuration * TrackRangeOverTotalTime);
            SegTrk.AddRegion(pos, w, ColorDurA, ColorDurB,
                "Duration: " + fDuration.ToString(),
                -1); // -1 indicates not to use a level, 
                     //but instead full thickness
            //SegTrk.Regions[1].UseGradient = true;
            
            pos += w;
            w = (int)(fDestTime * TrackRangeOverTotalTime);
            float fDestEndTime = DestSeq.DurationDivFreq;
            strText = DestSeq.Name + "\n" + 
                      strFullSeq + fDestEndTime.ToString("f3");               
            SegTrk.AddRegion(pos, w, ColorDestA, ColorDestB, strText, 1);
        }

        protected MarkerItem CreateBlendMarker(float fTime)
        {
            int pos = 0;
            int w = (int)(fTime * TrackRangeOverTotalTime);
            MarkerItem mi = SegTrk.AddMarker(pos+w,
                "Use this marker to control where the\n" +
                "transition begins in the source animation.",
                false, true);
            if (mi != null)
                mi.UseAltImageForDisabled = true;

            return mi;
        }
    }
}
