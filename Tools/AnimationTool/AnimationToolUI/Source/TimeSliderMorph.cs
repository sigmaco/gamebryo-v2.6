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
    /// Summary description for TimeSliderMorph.
    /// </summary>
    public class TimeSliderMorph : TimeSliderTransitionInfo
    {
        public bool m_bMorphTagsCross;
        float m_fMaxTransitionTime;
        int m_iMaxTransitionPos;

        public TimeSliderMorph(TimeSlider ts) : base(ts)
        {
            m_bMorphTagsCross = false;
        }

        public bool IsMorphTextKey(string strText)
        {
            if (strText.Length < 6)
                return false;

            return String.Compare(strText.Substring(0, 6), "morph:") == 0;
        }

        public bool DoMorphTagsCross()
        {
            MSequence.MTextKey[] aKeys = SrcSeq.TextKeys;
            MSequence.MTextKey tkLowestInSrc = null;
            MSequence.MTextKey tkLowestInDest = null;

            // Finally, add markers for textkeys...
            foreach (MSequence.MTextKey tk in aKeys)
            {
                if (IsMorphTextKey(tk.Text) == false)
                    continue;

                if (tkLowestInSrc == null)
                {
                    tkLowestInSrc = tk;
                    tkLowestInDest = tk;
                    continue;
                }

                float fTextKeyTime = GetTextKeyTimeDivFreq(SrcSeq, tk);
                float fDestTime = 
                    DestSeq.FindCorrespondingMorphFrame(SrcSeq, fTextKeyTime);
                fDestTime = GetTimeDivFreq(DestSeq, fDestTime);

                if (fDestTime > DestSeqLength)
                    fDestTime -= DestSeqLength;

                float fLowestTime;
                
                fLowestTime = GetTimeDivFreq(SrcSeq, tkLowestInSrc.Time);
                if (fTextKeyTime < fLowestTime)
                    tkLowestInSrc = tk;

                fLowestTime = GetTimeDivFreq(DestSeq, tkLowestInDest.Time);
                if (fDestTime < tkLowestInDest.Time)
                    tkLowestInDest = tk;
            }

            if (tkLowestInSrc == tkLowestInDest)
                return false;

            return true;
        }

        public void FindMaxTotalTime()
        {
            TotalTime = 0.0f;

            MSequence.MTextKey[] aKeys = SrcSeq.TextKeys;
            MCompletionInfo ki;
            float fProposedTime;
            float fTimeInDestWhenTransitionCompletes;

            MAnimation pmAnimation = MFramework.Instance.Animation;
            foreach (MSequence.MTextKey tk in aKeys)
            {
                if (IsMorphTextKey(tk.Text) == false)
                    continue;

                float fTextKeyFrame = tk.Time;

                ki = pmAnimation.FindTimeForAnimationToCompleteTransition(
                    Transition, 0, fTextKeyFrame );

                fTimeInDestWhenTransitionCompletes = 
                    DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes);
                Debug.Assert(DestSeq.DurationDivFreq >= 
                    fTimeInDestWhenTransitionCompletes);

                fProposedTime = ki.TimeToCompleteTransition +
                    (DestSeq.DurationDivFreq - 
                     fTimeInDestWhenTransitionCompletes);

                if (fProposedTime > TotalTime)
                {
                    ciInfo = ki;    
                    TotalTime = fProposedTime;
                }
            }

            // We should also check the end of sequence...
            ki = pmAnimation.FindTimeForAnimationToCompleteTransition(
                Transition, 0, SrcSeq.Duration );

            fTimeInDestWhenTransitionCompletes = 
                DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes);
            Debug.Assert(DestSeq.DurationDivFreq >= 
                fTimeInDestWhenTransitionCompletes);

            fProposedTime = ki.TimeToCompleteTransition +
                (DestSeq.DurationDivFreq - 
                 fTimeInDestWhenTransitionCompletes);

            if (fProposedTime > TotalTime)
            {
                ciInfo = ki;    
                TotalTime = fProposedTime;
            }

            // MAX Transition Range
            float fPercent = SrcSeq.DurationDivFreq / TotalTime; 
            m_fMaxTransitionTime = SrcSeq.DurationDivFreq;
            m_iMaxTransitionPos = (int)(fPercent * TrackRange);
        }

        public int TimeSegToPosWidthAfterTransition(float fTime)
        {
            int iValue;
            float fTransitionTime;
            int iTranPos;

            if (TransitionMarker == null)
                iTranPos = m_iMaxTransitionPos;
            else
                iTranPos = TransitionMarker.Position;

            fTransitionTime =  (float) iTranPos * m_fMaxTransitionTime;
            fTransitionTime /= (float)m_iMaxTransitionPos;

            float fRangeLeft = TrackRange - iTranPos;
            float fTimeLeft = TotalTime - fTransitionTime;

            iValue = (int)( fTime * (fRangeLeft / fTimeLeft));
            
            return iValue;
        }


        public int TimeToSliderPos(float fCurrentTime)
        {
            int iValue;
            float fTransitionTime;
            int iTranPos;

            if (TransitionMarker == null)
                iTranPos = m_iMaxTransitionPos;
            else
                iTranPos = TransitionMarker.Position;
           
            fTransitionTime =  (float) iTranPos * m_fMaxTransitionTime;
            fTransitionTime /= (float)m_iMaxTransitionPos;

            float fRemaining;
            fRemaining = (fCurrentTime - fTransitionTime); 

            float fRangeLeft = TrackRange - iTranPos;
            float fTimeLeft = TotalTime - fTransitionTime;

            iValue = (int)( fRemaining * (fRangeLeft / fTimeLeft) + iTranPos);
            
            return iValue;
        }

        public int MorphTimeToScrubberPos(float fCurrentTime)
        {
            int iValue;

            float fTransitionTime;
            fTransitionTime =  
                (float) TransitionMarker.Position * m_fMaxTransitionTime;
            fTransitionTime /= (float)m_iMaxTransitionPos;

            if (fCurrentTime < fTransitionTime) 
                iValue = (int) (fCurrentTime * 
                    ( m_iMaxTransitionPos / SrcSeqLength));
            else
                iValue = TimeToSliderPos(fCurrentTime);

            return iValue;
        }

        public float ScubberPosToMorphTime()
        {
            if (SegTrk.Scrubber.Position <= TransitionMarker.Position)
            {
                float fTime = SegTrk.Scrubber.Position *
                    (m_fMaxTransitionTime / (float)m_iMaxTransitionPos);
                return fTime;
            }
            else
            {
                float fTransitionTime;
                fTransitionTime = 
                    (float) TransitionMarker.Position * m_fMaxTransitionTime;
                fTransitionTime /= (float)m_iMaxTransitionPos;

                float fTimeLeft = TotalTime - fTransitionTime;
                int iRangeLeft = TrackRange - TransitionMarker.Position;

                float fTime = 
                    (float)(SegTrk.Scrubber.Position - 
                    TransitionMarker.Position) * 
                    (fTimeLeft / (float)iRangeLeft) + fTransitionTime;

                return fTime;
            }
        }

        // SrcSeq.TextKeys
        public void UpdateTextKeyMarkers(int iTextKeys)
        {
            int iPos;

            SegmentedTrackBarDLL.MarkerCollection ami = SegTrk.Markers;

            foreach (MarkerItem mi in ami)
            {
                if (mi == TransitionMarker)
                    continue;

                MSequence.MTextKey tk = (MSequence.MTextKey)mi.Tag;
                int iLevel = (int)mi.Tag2;

                if (tk == null)
                    continue;

                float fTime; 

                float fStart = GetStartOfDestTime();

                if (mi.BelowBar)
                {
                    fTime = GetTextKeyTimeDivFreq(DestSeq, tk);

                    iPos = TimeToSliderPos(fStart);
                    iPos += TimeSegToPosWidthAfterTransition(fTime);
                    iPos += TimeSegToPosWidthAfterTransition(
                        DestSeqLength*iLevel);
                }
                else 
                {
                    // convert time to destination time...
                    fTime = tk.Time; 

                    fTime = GetTimeDivFreq(SrcSeq, fTime);
                    fTime = DestSeq.FindCorrespondingMorphFrame(SrcSeq, fTime);
                    fTime = GetTimeDivFreq(DestSeq, fTime);

                    if (fTime > DestSeq.DurationDivFreq)
                        fTime = fTime - DestSeq.DurationDivFreq;

                    iPos = TimeToSliderPos(fStart);
                    iPos += TimeSegToPosWidthAfterTransition(fTime);      
                    iPos += TimeSegToPosWidthAfterTransition(
                        DestSeqLength*iLevel);
                }

                mi.Position = iPos;

                // We only want to see tags to the right of transition as
                // these are the ones that are valid and scaled correctly.
                if (mi.Position < TransitionMarker.Position)
                    mi.Visible = false;
                else
                    mi.Visible = true;
            }
        }

        public void AddTextKeyMarkers(int iTextKeys)
        {
            MSequence.MTextKey[] aKeys;
            MarkerItem mi;
            bool bBelow;
            string strText;
            MSequence UsedSeq = null;

            if (iTextKeys == 0) // Source text keys
            {
                UsedSeq = SrcSeq;
                bBelow = false;
            }
            else
            {
                UsedSeq = DestSeq;
                bBelow = true;
            }

            aKeys = UsedSeq.TextKeys;

            // Finally, add markers for textkeys...
            foreach (MSequence.MTextKey tk in aKeys)
            {
                if (IsMorphTextKey(tk.Text) == false)
                    continue;

                float fSrcTime;
                float fDestTime;

                // The positions will be determined later...
                if (bBelow)
                {
                    fDestTime = tk.Time;
                    fDestTime = GetTimeDivFreq(DestSeq, fDestTime);
                    fSrcTime = 
                        SrcSeq.FindCorrespondingMorphFrame(DestSeq,fDestTime);

                    fSrcTime = GetTimeDivFreq(SrcSeq, fSrcTime);
                }
                else
                {
                    fSrcTime = tk.Time;
                    fSrcTime = GetTimeDivFreq(SrcSeq, fSrcTime);

                    fDestTime =
                        DestSeq.FindCorrespondingMorphFrame(SrcSeq,fSrcTime);
                    fDestTime = GetTimeDivFreq(DestSeq, fDestTime);

                }

                strText = tk.Text + 
                    "\n\nOccurs at time " + fSrcTime.ToString("f3") +
                    " in source and" +
                    "\ncorresponds to time " + fDestTime.ToString("f3") +
                    " in destination.";

                mi = SegTrk.AddMarker(0, strText, bBelow, false);
                mi.Tag = tk;
                mi.Tag2 = 0;

                // We put two identical back to back.               
                mi = SegTrk.AddMarker(0, strText, bBelow, false);
                mi.Tag = tk;
                mi.Tag2 = 1;                
            }
        }

        public override void Setup()
        {
            FindMaxTotalTime();

            BeginTime = 0; 
            EndTime = TotalTime;

            // Add markers for textkeys...
            AddTextKeyMarkers(0);
            AddTextKeyMarkers(1);

            // The source will have two regions.
            // one that represents pre-transition and one that
            // represents the duration portion.
            // The duration period is constant.
            int pos = 0;
            int w = (int)(SrcSeqLength * TrackRangeOverTotalTime);
            SegTrk.AddTicks(0, w, 0);

            string strText = SrcSeq.Name + "\n" + strFullSeq + 
                SrcSeq.DurationDivFreq.ToString("f3");               
            SegTrk.AddRegion(pos, w,ColorSrcA,ColorSrcB, strText,0);
            pos = w;
            w = TimeSegToPosWidthAfterTransition(Duration);
            strText = SrcSeq.Name + "\n" + "Transition Duration = " + 
                Duration.ToString("f3");
            SegTrk.AddRegion(pos,w,ColorSrcA,ColorSrcB,strText,0);
            SegTrk.Regions[1].UseGradient = true;

            // the marker is just above this split.
            
            MarkerItem mi = SegTrk.AddMarker(pos, null, false, true);
            mi.UseAltImageForDisabled = true;
            TransitionMarker = mi;
            SetTransitionMarkerText();

            strText = DestSeq.Name + "\n" + "Transition Duration = " + 
                Duration.ToString("f3");
            SegTrk.AddRegion(pos,w,ColorDestB,ColorDestA,strText,1);
            SegTrk.Regions[2].UseGradient = true;
            pos += w;


            // The duration period of time for the destination is constant.
            // it's position simply slides.
            w = TimeSegToPosWidthAfterTransition(DestSeqLength) - w;

            strText = DestSeq.Name + "\n" + strFullSeq + 
                      DestSeq.DurationDivFreq.ToString("f3");               
            SegTrk.AddRegion(pos,w,ColorDestA,ColorDestB,strText,1);

            // Setup tick marks
            w = TimeSegToPosWidthAfterTransition(DestSeqLength); 
            // The position will be adjusted later as it is variable
            SegTrk.AddTicks(0, w, 1);

            m_bMorphTagsCross = DoMorphTagsCross();

            SetTrackerIncrement();
            SegTrk.UseLevelDividers = true;

            // Set position from saved values
            int iValue = (int) (Transition.DefaultPositionTime 
                * ( m_iMaxTransitionPos / SrcSeqLength));
            if (iValue >= TrackRange)
                iValue = TrackRange - 1;
            SegTrk.Scrubber.Position = iValue;
            MFramework.Instance.Animation.CurrentTime = 
                Transition.DefaultPositionTime;

            // set transition marker from saved values
            iValue = (int)((Transition.DefaultTransitionTime * 
                TrackRange) / (EndTime - BeginTime));
            TransitionMarker.Position = iValue;
            MFramework.Instance.Animation.TransitionStartTime =
                Transition.DefaultTransitionTime;

            // Call OnMarkerPosChange to feed an update to 
            // ticks, markers, regions...
            OnMarkerPosChanged(TransitionMarker);

            MFramework.Instance.Animation.RunUpTime(
                MFramework.Instance.Animation.CurrentTime);
        }

        public override void OnScrubberPosChanged()
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Animation.CurrentTime =
                ScubberPosToMorphTime();

            Transition.DefaultPositionTime = 
                MFramework.Instance.Animation.CurrentTime;
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
                    MorphTimeToScrubberPos(
                        MFramework.Instance.Animation.CurrentTime);

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

        public int FindNextHighestEndOfDestSeq(int iFromPos)
        {
            int iStart = SegTrk.Ticks[1].StartPos;

            int w = (int)(DestSeqLength * TrackRangeOverTotalTime);
            while (iFromPos >= iStart)
            {
                iStart += w;                
            }
            return iStart;
        }

        public void SetTransitionMarkerText()
        {
            if (TransitionMarker == null)
                return;

            float fTransitionTime;

            // For morph, we must always refer to the src time
            fTransitionTime =  
                (float) TransitionMarker.Position * m_fMaxTransitionTime;
            fTransitionTime /= (float)m_iMaxTransitionPos;

            float fCorrTime =
                DestSeq.FindCorrespondingMorphFrame(SrcSeq, fTransitionTime);
            fCorrTime = GetTimeDivFreq(DestSeq, fCorrTime);
            
            TransitionMarker.Text = "Use this marker to control where the\n" +
                "Morph begins in the source animation.\n\n" +
                "Time of Transition: " + fTransitionTime.ToString("f3") +
                "\nCorresponding Time in Destination: " + 
                fCorrTime.ToString("f3");
        }

        public override void OnMarkerPosChanged(
            SegmentedTrackBarDLL.MarkerItem marker)
        {
            if (marker != this.TransitionMarker)
                return;

            // Calculate the new transtion time 
            float fTransitionTime;

            // For morph, we must always refer to the src time
            fTransitionTime =  (float) marker.Position * m_fMaxTransitionTime;
            fTransitionTime /= (float)m_iMaxTransitionPos;

            // For precision errors.
            if (fTransitionTime > SrcSeq.DurationDivFreq)
                fTransitionTime = SrcSeq.DurationDivFreq;

            float fTransitionFrame =
                SrcSeq.TimeMultFreq(fTransitionTime);

            // Recalculate the regions each time
            MAnimation pmAnimation = MFramework.Instance.Animation;
            MCompletionInfo ki =
                pmAnimation.FindTimeForAnimationToCompleteTransition(
                Transition, 0, fTransitionFrame );
            ciInfo = ki;    

            float fEndOfDestPos = ki.TimeToCompleteTransition +
                (DestSeq.DurationDivFreq - 
                DestSeq.TimeDivFreq(ki.FrameInDestWhenTransitionCompletes));

            int iEndOfDestPos = MorphTimeToScrubberPos(fEndOfDestPos);

            int w = (int)(Duration * TrackRangeOverTotalTime);

            // We MUST set the rangelimiter back to the full range
            // because otherwise, we'll never be able to set our
            // region past it, because of code that "crops" to
            // ensure regions don't go pass ranges...
            SegTrk.RangeLimiterRight.Position = TrackRange;
            SegTrk.Regions[3].Width = iEndOfDestPos - (marker.Position +
                SegTrk.Regions[2].Width);

            SegTrk.Regions[0].Width = marker.Position;
            SegTrk.Regions[1].Position = SegTrk.Regions[0].Width;
            SegTrk.Regions[2].Position = marker.Position;
            SegTrk.Regions[3].Position = 
                marker.Position + SegTrk.Regions[2].Width;
            SegTrk.Invalidate();
 
            pmAnimation.TransitionStartTime =
                fTransitionTime;
           
            float fStartDestTime = GetStartOfDestTime();
    
            int iValue = MorphTimeToScrubberPos(fStartDestTime);

            SegTrk.Ticks[1].StartPos = iValue;

            // Adjust ticks for source...
            float fSrcSeqEndSync = DestSeq.FindCorrespondingMorphFrame(SrcSeq,
                SrcSeqLength);
            fSrcSeqEndSync = GetTimeDivFreq(DestSeq, fSrcSeqEndSync);

            int iSrcEndWidth = 
                TimeSegToPosWidthAfterTransition(fSrcSeqEndSync);
            int iStart = iValue + iSrcEndWidth;
            int iDestEndWidth = 
                TimeSegToPosWidthAfterTransition(DestSeqLength);
            SegTrk.Ticks[0].Width = iDestEndWidth;
            SegTrk.Ticks[0].StartPos = iStart;

            Transition.DefaultTransitionTime = fTransitionTime;

            SetTransitionMarkerText();
            
            UpdateTextKeyMarkers(0);

            int iEndRange = SegTrk.Regions[3].Position + 
                SegTrk.Regions[3].Width;
            SegTrk.RangeLimiterRight.Position = iEndRange;
        }

        public float GetStartOfDestTime()
        {
            float fTransitionTime;
            fTransitionTime =  
                (float) TransitionMarker.Position * m_fMaxTransitionTime;
            fTransitionTime /= (float)m_iMaxTransitionPos;

            float fTranDestTime = 
                DestSeq.FindCorrespondingMorphFrame(SrcSeq, fTransitionTime);
            fTranDestTime = GetTimeDivFreq(DestSeq, fTranDestTime);

            if (fTranDestTime > DestSeqLength)
                return (fTransitionTime - (fTranDestTime - DestSeqLength));
            else
            {
                return (fTransitionTime - fTranDestTime); 
            }
        }
    }
}
