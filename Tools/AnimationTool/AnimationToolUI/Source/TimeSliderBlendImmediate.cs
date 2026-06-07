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
    /// Summary description for TimeSliderBlendImmediate.
    /// </summary>
    public class TimeSliderBlendImmediate : TimeSliderBlend
    {
        private float m_fTotalTimeOverTrackRange;

        public TimeSliderBlendImmediate(TimeSlider ts) : base(ts){}

        public void SetTransitionMarkerText()
        {
            if (TransitionMarker == null)
                return;

            float fTransitionTime = BeginTime +
                ((float) TransitionMarker.Position) / TrackRangeOverTotalTime;
           
            TransitionMarker.Text = "Use this marker to control where the\n" +
                "transition in the source animation is to occur.\n\n" +
                "Time of Transition: " + fTransitionTime.ToString("f3");
        }

        public override void Setup()
        {   
            float fBeginKeyFrame = 0;
            float fSrcSeqFrameLength = SrcSeq.Duration;

            MAnimation mAnimation = MFramework.Instance.Animation;
            MCompletionInfo ki =
                mAnimation.FindTimeForAnimationToCompleteTransition(
                Transition, fBeginKeyFrame, fSrcSeqFrameLength);

            float fDestTimeWhenTransitionCompletes =
                DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes);

            if (DestSeqLength > fDestTimeWhenTransitionCompletes)
            {
                TotalTime = ki.TimeToCompleteTransition +
                    (DestSeqLength - fDestTimeWhenTransitionCompletes);
            }
            else
            {
                // This is the case where 
                // DestSeqLength == fDestTimeWhenTransitionCompletes
                // this indicates that we blended to the final frame...
                Debug.Assert(
                    DestSeqLength == fDestTimeWhenTransitionCompletes);
                if (DestSeq.Loop)
                {
                    // In the loop case,
                    // the engine will run one extra cycle...
                    // so we just add DestSeqLength
                    TotalTime = ki.TimeToCompleteTransition + DestSeqLength;
                }
                else
                {
                    // In the clamp case,
                    // we will reset immediately after the transition finishes
                    TotalTime = ki.TimeToCompleteTransition;
                }
            }
            ki.Dispose();

            BeginTime = 0;
            EndTime = TotalTime;
            
            m_fTotalTimeOverTrackRange = TotalTime / TrackRange;

            CreateBlendRegions( SrcSeqLength, Duration, DestSeqLength);
            TransitionMarker = CreateBlendMarker( SrcSeqLength );

            // Setup tick marks
            int w = (int)(SrcSeqLength * TrackRangeOverTotalTime);
            SegTrk.AddTicks(0, w, 0);

            w = (int)(DestSeqLength * TrackRangeOverTotalTime);
            int pos = 
                (int)((SrcSeqLength + Duration) * TrackRangeOverTotalTime);
            SegTrk.AddTicks(pos, w, 1);

            SetTransitionMarkerText();
            SetTrackerIncrement();
            SegTrk.UseLevelDividers = true;

            // Set position from saved values
            int iValue = (int) (Transition.DefaultPositionTime 
                * TrackRangeOverTotalTime);
            if (iValue > TrackRange)
                iValue = TrackRange;
            SegTrk.Scrubber.Position = iValue;
            mAnimation.CurrentTime = Transition.DefaultPositionTime;

            // set transition marker from saved values
            iValue = (int)((Transition.DefaultTransitionTime - BeginTime) /
                 m_fTotalTimeOverTrackRange);
            TransitionMarker.Position = iValue;
            mAnimation.TransitionStartTime =
                Transition.DefaultTransitionTime;

            OnMarkerPosChanged(TransitionMarker);
            mAnimation.RunUpTime(
                mAnimation.CurrentTime);
        }

        public override void OnMarkerPosChanged(MarkerItem marker)
        {
            // A marker has changed.
            // If we are dealing with a blend... recalculate the blend...

            if (SegTrk.Regions.Count != 3)
                return;

            int iDiff = SegTrk.Regions[0].Width - marker.Position;

            SegTrk.Regions[0].Width = marker.Position;
            AllignRegions();

            // tickmarks will change for destination...
            if (SegTrk.Ticks.Count > 1)
                SegTrk.Ticks[1].StartPos -= iDiff;

            float fTransitionTime = BeginTime +
                ((float) marker.Position) * m_fTotalTimeOverTrackRange;

            MFramework.Instance.Animation.TransitionStartTime =
                fTransitionTime;

            LimitScrubberToLastRegion();
            
            SetTransitionMarkerText();

            SegTrk.Invalidate();
            
            Transition.DefaultTransitionTime = fTransitionTime;
        }
    }
}
