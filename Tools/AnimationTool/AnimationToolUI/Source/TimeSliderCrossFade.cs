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
    /// Summary description for TimeSliderCrossFade.
    /// </summary>
    public class TimeSliderCrossFade : TimeSliderTransitionInfo
    {
        public TimeSliderCrossFade(TimeSlider ts) : base(ts) {}

        public void SetTransitionMarkerText()
        {
            if (TransitionMarker == null)
                return;

            float fTransitionTime = BeginTime +
                ((float) TransitionMarker.Position) / TrackRangeOverTotalTime;
           
            TransitionMarker.Text = "Use this marker to control where the\n" +
                "Crossfade begins in the source animation.\n\n" +
                "Time of Transition: " + fTransitionTime.ToString("f3") +
                "\nCorresponding Time in Destination: 0.0";
        }

        public override void Setup()
        {
            MAnimation pmAnimation = MFramework.Instance.Animation;
            MCompletionInfo ki =
                pmAnimation.FindTimeForAnimationToCompleteTransition(
                Transition, 0, SrcSeq.Duration);
            ciInfo = ki;           
          
            float fTimeInDesWhenTransitionCompletes = 
                DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes);

            if (DestSeq.Loop)
            {
                // In the loop case,
                TotalTime = ki.TimeToCompleteTransition + 
                    (DestSeqLength - fTimeInDesWhenTransitionCompletes);

            }
            else
            {
                // In the clamp case,
                // we will reset immediately after the transition finishes
                if (Transition.Duration >= DestSeqLength)
                    TotalTime = ki.TimeToCompleteTransition;
                else
                    TotalTime = ki.TimeToCompleteTransition + 
                        (DestSeqLength - fTimeInDesWhenTransitionCompletes);
            }            
                  
            BeginTime = 0;
            EndTime = TotalTime;

            // Will need two regions for the src....
            // one for the beginning... one for the duration...
            // both we be colored as source.
            int pos = 0;
            int w = (int)(SrcSeqLength * TrackRangeOverTotalTime);
            // Setup tick marks for src
            SegTrk.AddTicks(0, w, 0);

            w = (int)((ki.TimeToCompleteTransition - Duration) 
                * TrackRangeOverTotalTime);
            string strText = SrcSeq.Name + 
                "\n" + strFullSeq + SrcSeq.DurationDivFreq.ToString("f3");
            SegTrk.AddRegion(pos,w, ColorSrcA, ColorSrcB, strText, 0);
 
            pos = w;
            w = (int)(Duration * TrackRangeOverTotalTime);
            strText = SrcSeq.Name + "\n" + "Transition Duration = " 
                + Duration.ToString("f3");

            SegTrk.AddRegion(pos, w,ColorSrcA,ColorSrcB,strText,0);
            SegTrk.Regions[1].UseGradient = true;

            // Add fade out region to dest.
            strText = DestSeq.Name + "\n" + "Transition Duration = " + 
                Duration.ToString("f3");
            SegTrk.AddRegion(pos, w,ColorDestB,ColorDestA,strText,1);
            SegTrk.Regions[2].UseGradient = true;

            // the marker is just above this split.
            MarkerItem mi = SegTrk.AddMarker(pos,null,false, true);
            TransitionMarker = mi;
            SetTransitionMarkerText();
            mi.UseAltImageForDisabled = true;

            pos += w;
            w = (int)((DestSeqLength - 
                DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes))
                * TrackRangeOverTotalTime);

            strText = DestSeq.Name + "\n" + 
                strFullSeq + DestSeq.DurationDivFreq.ToString("f3");
            SegTrk.AddRegion(pos, w,ColorDestA,ColorDestB,strText,1);
 
            // Setup tick marks for dest
            w = (int)(DestSeqLength * TrackRangeOverTotalTime);
            SegTrk.AddTicks(pos, w, 1);

            SetTrackerIncrement();
            SegTrk.UseLevelDividers = true;

            // Set position from saved values
            int iValue = (int) (Transition.DefaultPositionTime 
                * TrackRangeOverTotalTime);
            if (iValue >= TrackRange)
                iValue = TrackRange - 1;
            SegTrk.Scrubber.Position = iValue;
            pmAnimation.CurrentTime = Transition.DefaultPositionTime;

            // set transition marker from saved values
            iValue = 
                (int)((Transition.DefaultTransitionTime * TrackRange) / 
                (EndTime - BeginTime));
            TransitionMarker.Position = iValue;
            pmAnimation.TransitionStartTime =
                Transition.DefaultTransitionTime;

            OnMarkerPosChanged(TransitionMarker);
            pmAnimation.RunUpTime(
                pmAnimation.CurrentTime);
        }

        public override void OnPaint()
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            if (BeginTime == EndTime)
                SegTrk.Scrubber.Position = 0;
            else
            {
                int iValue =
                    (int)(MFramework.Instance.Animation.CurrentTime *
                    TrackRangeOverTotalTime);
                    
                if (iValue == TrackRange)
                    iValue--;

                SegTrk.Scrubber.Position = iValue;
            }

            if (Transition != null)
            {
                Transition.DefaultPositionTime =
                    MFramework.Instance.Animation.CurrentTime;
            }
        }

        public override void OnMarkerPosChanged(
            SegmentedTrackBarDLL.MarkerItem marker)
        {
            int w = (int)(Duration * TrackRangeOverTotalTime);

            SegTrk.Regions[0].Width = marker.Position;
            SegTrk.Regions[1].Position = SegTrk.Regions[0].Width;
            SegTrk.Regions[2].Position = marker.Position;
            SegTrk.Regions[3].Position = marker.Position +
                SegTrk.Regions[2].Width;

            SegTrk.Invalidate();
 
            // Calculate the new transtion time 
            float fTransitionTime = BeginTime +
                ((float) marker.Position) / TrackRangeOverTotalTime;

            MFramework.Instance.Animation.TransitionStartTime =
                fTransitionTime;

            MCompletionInfo ki =
                MFramework.Instance.Animation.
                FindTimeForAnimationToCompleteTransition(
                Transition, 0, 
                SrcSeq.TimeMultFreq(fTransitionTime));

            ciInfo = ki;           
            w = (int)((ki.TimeToCompleteTransition - Duration) 
                * TrackRangeOverTotalTime);
            SegTrk.Ticks[1].StartPos = w;

            Transition.DefaultTransitionTime = fTransitionTime;

            SetTransitionMarkerText();
            LimitScrubberToLastRegion();
        }
    }
}
