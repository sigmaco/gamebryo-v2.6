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
    /// Summary description for TimeSliderSequence.
    /// </summary>
    public class TimeSliderSequence : TimeSliderTransitionInfo
    {
        public TimeSliderSequence(TimeSlider ts) : base(ts){}

        public override void Setup()
        {
            SrcSeq = MFramework.Instance.Animation.ActiveSequence;
            SrcSeqLength = SrcSeq.DurationDivFreq;

            SegTrk.ClearRegions();
            SegTrk.ClearMarkers();

            BeginTime = 0;
            EndTime = SrcSeq.DurationDivFreq; 

            SegTrk.MinRange = 0;
            if (BeginTime == EndTime)
                SegTrk.MaxRange = 0;
            else
                SegTrk.MaxRange = TrackRange;

            // Add one big region...
            string strText = SrcSeq.Name + "\n" + strFullSeq +
                SrcSeq.DurationDivFreq.ToString("f3");
            SegTrk.AddRegion(SegTrk.MinRange,
                SegTrk.MaxRange, ColorSrcA, ColorSrcB, strText, -1);

            TotalTime = SrcSeqLength;
            SetTrackerIncrement();
            SegTrk.UseLevelDividers = false;           

            // If the scrubber is not enabled, the sequence is playing. In
            // this case, reset the sequence's default position time to be
            // the beginning of the sequence. This prevents clamped sequences
            // from not animating when they are clicked if time has gone past
            // the end of the sequence.
            if (!m_ts.segtrkTime.Scrubber.Enabled)
            {
                SrcSeq.DefaultPositionTime = 0;
            }

            // Set position from saved values
            int iValue = (int) (SrcSeq.DefaultPositionTime
                * TrackRangeOverTotalTime);
            if (iValue >= TrackRange)
                iValue = TrackRange - 1;
            SegTrk.Scrubber.Position = iValue;
            MFramework.Instance.Animation.CurrentTime = 
                SrcSeq.DefaultPositionTime;       

            AddTextKeyMarkers();
        }

        public void AddTextKeyMarkers()
        {
            MSequence.MTextKey[] aKeys = SrcSeq.TextKeys;

            // Add markers for textkeys...
            foreach (MSequence.MTextKey tk in aKeys)
            {
                float fSrcTime = tk.Time;
                fSrcTime = GetTimeDivFreq(SrcSeq, fSrcTime);

                string strText = tk.Text + 
                    "\nOccurs at time " + fSrcTime.ToString("f3");

                MarkerItem mi;
                mi = SegTrk.AddMarker(0, strText, false, false);

                mi.Position = (int) (fSrcTime * TrackRangeOverTotalTime);
            }
        }
    }
}
