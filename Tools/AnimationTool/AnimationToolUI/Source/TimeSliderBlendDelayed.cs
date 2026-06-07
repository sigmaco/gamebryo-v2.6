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
    /// Summary description for TimeSliderBlendDelayed.
    /// </summary>
    public class TimeSliderBlendDelayed : TimeSliderBlend
    {
        internal bool bLastKeyIsEndTime = false;
        internal bool bTimeUsingFirstKeyCausesLongerTime = true;
        internal MTransition.MBlendPair bpFirst = null;
        internal MTransition.MBlendPair bpLast = null;
        internal MTransition.MBlendPair bpLongest = null;

        public TimeSliderBlendDelayed(TimeSlider ts) : base(ts){}

        public void SetTransitionMarkerText()
        {
            if (TransitionMarker == null)
                return;

            float fTransitionTime = BeginTime +
                ((float) TransitionMarker.Position) / TrackRangeOverTotalTime;
           
            TransitionMarker.Text = "Use this marker to control where the\n" +
                "desired transition in the source animation is.\n" +
                "Note that the actual transition falls on a BlendPair.\n\n" +
                "Time of Transition: " + fTransitionTime.ToString("f3");
        }

        internal void FindLongestDurationForDelayed(
            float fDuration, MTransition.MBlendPair[] aBP)
        {
            TotalTime = 0;
            bpFirst = null;
            bpLast = null;
            bpLongest = null;
            bLastKeyIsEndTime = true;
            bTimeUsingFirstKeyCausesLongerTime = false;

            float fSrcFrameStart, fDestFrameStart, fTmpTotal;

            float fBeginFrame = 0;
            float fEndFrame = SrcSeq.Duration;

            // First, find the total length if we started from the very end
            // of the sequence.
            MAnimation animation = MFramework.Instance.Animation;
            MCompletionInfo ki =
                animation.FindTimeForAnimationToCompleteTransition(
                    Transition, fBeginFrame, fEndFrame);
            TotalTime = ki.TimeToCompleteTransition + 
                    (DestSeqLength - DestSeq.TimeDivFreq(
                    ki.FrameInDestWhenTransitionCompletes));
            ciInfo = ki;

            foreach (MTransition.MBlendPair bp in aBP)
            {
                fSrcFrameStart = SrcSeq.GetKeyTimeAt(bp.StartKey);

                fSrcFrameStart = AdjustTimeForFrameRate(fSrcFrameStart);

                fDestFrameStart = DestSeq.GetKeyTimeAt(bp.TargetKey);

                // Keep track of first and last for later.
                if (bpFirst == null)
                    bpFirst = bp;
                else if (fSrcFrameStart < 
                         SrcSeq.GetKeyTimeAt(bpFirst.StartKey))
                    bpFirst = bp;

                if (bpLast == null)
                    bpLast = bp;
                else if (fSrcFrameStart > SrcSeq.GetKeyTimeAt(bpLast.StartKey))
                    bpLast = bp;

                ki = animation.FindTimeForAnimationToCompleteTransition(
                    Transition, fBeginFrame, fSrcFrameStart);


                fTmpTotal = ki.TimeToCompleteTransition + 
                    (DestSeqLength - 
                    DestSeq.TimeDivFreq(
                        ki.FrameInDestWhenTransitionCompletes));

                if (fTmpTotal >= TotalTime)
                {
                    TotalTime = fTmpTotal;
                    bpLongest = bp;
                    ciInfo = ki;
                }
            }

            float fLastFrame = SrcSeq.GetKeyTimeAt(bpLast.StartKey);
            float fSrcSeqFrameLength = SrcSeq.Duration;

            /*
            if (fLastFrame != fSrcSeqFrameLength)
            {
                // If the last time isn't the length of the src,
                // then we may have an extra loop...
                bLastKeyIsEndTime = false;
                if (bpLongest == null || bpLongest != bpLast)
                {
                    float fDestSeqFrameLength = DestSeq.EndKeyTime -
                        DestSeq.BeginKeyTime;

                    bTimeUsingFirstKeyCausesLongerTime = true;
                    if (bpFirst != null)
                        fDestFrameUsingFirstKey = 
                            fDestSeqFrameLength - DestSeq.GetKeyTimeAt(
             * bpFirst.TargetKey);;
                }
            }*/
        }

        internal void CreateMarkersForEachBlendPair()
        {
            MTransition.MBlendPair[] aBP = BlendPairs;

            if (aBP == null)
                return;
            
            foreach (MTransition.MBlendPair bp in aBP)
            {
                string strSrcKey = bp.StartKey;
                string strDestKey = bp.TargetKey;

                // Need to find the time of startkey.
                float fSrcTimeStart = SrcSeq.GetKeyTimeAtDivFreq(strSrcKey);

                int iPosSrc = (int)(fSrcTimeStart * TrackRangeOverTotalTime);

                MarkerItem mi =
                    SegTrk.AddMarker(iPosSrc,
                    strSrcKey + ", " + fSrcTimeStart.ToString("f3"), 
                    false, false);
                mi.Tag = bp; 
            }

            m_BlendPairMarker = 
                SegTrk.AddMarker(0, "Matching Blend Pair", true, false);
        }

        public override void Setup()
        {
            float fDuration = Duration;
            float fDestTime = 0.0f;

            // The total for a delayed blend will be the total
            // of the source, duration, destination + missedSection.
            // The missedsection is 1stPair - beginnning.
            //float fTotal = fOrigSrcTime + fDuration + fOrigDestTime;
            
            MTransition.MBlendPair[] aBP = BlendPairs;           

            FindLongestDurationForDelayed(fDuration, aBP);
            CreateMarkersForEachBlendPair();

            if (ciInfo != null)
            {
                CreateBlendRegions( 
                    ciInfo.TimeToCompleteTransition - fDuration,
                    fDuration,
                    DestSeqLength - 
                    DestSeq.TimeDivFreq(
                        ciInfo.FrameInDestWhenTransitionCompletes));
            }

            TransitionMarker = CreateBlendMarker(SrcSeqLength);

            // Setup tick marks
            int w = (int)(SrcSeqLength * TrackRangeOverTotalTime);
            SegTrk.AddTicks(0, w, 0);

            w = (int)(fDestTime * TrackRangeOverTotalTime);
            int pos = 
                (int)((SrcSeqLength + fDuration) * TrackRangeOverTotalTime);
            SegTrk.AddTicks(pos, w, 1);

            BeginTime = 0.0f;
            EndTime = TotalTime;

            SetTransitionMarkerText();
            SetTrackerIncrement();
            SegTrk.UseLevelDividers = true;

            // Set position from saved values
            int iValue = (int) (Transition.DefaultPositionTime 
                * TrackRangeOverTotalTime);
            if (iValue >= TrackRange)
                iValue = TrackRange - 1;
            SegTrk.Scrubber.Position = iValue;
            MFramework.Instance.Animation.CurrentTime = 
                Transition.DefaultPositionTime;

            // set transition marker from saved values
            iValue = (int)((Transition.DefaultTransitionTime * TrackRange) / 
                           (EndTime - BeginTime));
            TransitionMarker.Position = iValue;

            if (Transition.DefaultTransitionTime >
                SrcSeq.DurationDivFreq -
                MFramework.Instance.Animation.IncrementTime)
            {
                // We only adjust the time on the far end of the sequence.
                Transition.DefaultTransitionTime =
                    AdjustTimeForFrameRate(Transition.DefaultTransitionTime);
            }

            MFramework.Instance.Animation.TransitionStartTime =
                Transition.DefaultTransitionTime;

            OnMarkerPosChanged(TransitionMarker);
            MFramework.Instance.Animation.RunUpTime(
                MFramework.Instance.Animation.CurrentTime);
        }

        public override void OnMarkerPosChanged(MarkerItem marker)
        {
            if (marker == m_BlendPairMarker)
            {
                // when we change our blendpairmarker, we get an event.
                // we want to ignore it.
                return;
            }

            float fTransitionFrameTime;

            // Regions aren't setup correctly if there arn't 3 of them.
            if (SegTrk.Regions.Count != 3)
                return;

            fTransitionFrameTime = BeginTime +
                SrcSeq.TimeMultFreq(((float) marker.Position) / 
                    (float)TrackRange * (EndTime - BeginTime));

            MAnimation animation = MFramework.Instance.Animation;
            MCompletionInfo ki =
                animation.FindTimeForAnimationToCompleteTransition(
                Transition, BeginTime, 
                fTransitionFrameTime);

            SegTrk.Regions[0].Width = (int)
                ((ki.TimeToCompleteTransition - 
                  Duration)* TrackRangeOverTotalTime);
            SegTrk.Regions[2].Width = (int)((DestSeqLength - 
                DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes))
                * TrackRangeOverTotalTime);

            // Handle special case where we blend to "end" key...
            if (ki.BlendPair != null)
            {
                if (ki.BlendPair.TargetKey == "end")
                {
                    // if this sequence isn't clamped
                    if (DestSeq.Loop == true)
                        SegTrk.Regions[2].Width += 
                            (int)(DestSeqLength * TrackRangeOverTotalTime);
                }
            }

            AllignRegions();
              
            if (ki.BlendPair != null)
            {
                float fDestTimeStart = 
                    DestSeq.GetKeyTimeAtDivFreq(ki.BlendPair.TargetKey);
                SegTrk.Ticks[1].StartPos -=
                    (int)((ki.TimeToCompleteTransition - fDestTimeStart)
                    * TrackRangeOverTotalTime);

                // No next marker... so do extra loop...
                m_BlendPairMarker.Text = ki.BlendPair.TargetKey + " (from " + 
                    ki.BlendPair.StartKey + ")";

                int iPos = (int)((ki.TimeToCompleteTransition)* 
                                 TrackRangeOverTotalTime);
                
                m_BlendPairMarker.Position = iPos;
            }

            float fAdjustedTransitionTime =
                SrcSeq.TimeDivFreq(fTransitionFrameTime);

            
            if (fAdjustedTransitionTime >
                SrcSeq.DurationDivFreq -
                MFramework.Instance.Animation.IncrementTime)
            {
                // We only adjust the time on the far end of the sequence.
                fAdjustedTransitionTime =
                    AdjustTimeForFrameRate(fAdjustedTransitionTime);
            }

            MFramework.Instance.Animation.TransitionStartTime =
                fAdjustedTransitionTime;

            Transition.DefaultTransitionTime = 
                MFramework.Instance.Animation.TransitionStartTime;

            SetTransitionMarkerText();

            // Can not call LimitScrubberToLastRegion because in some blendpair
            // scenarios, this can cause the user to not be able to move the
            // transition marker beyond a current blendpair region. 
            // Also related to this is the fact that auto-clamping is done of 
            // the scrubber regions.
            // LimitScrubberToLastRegion();
            
            SegTrk.Invalidate();
        }
    }
}
