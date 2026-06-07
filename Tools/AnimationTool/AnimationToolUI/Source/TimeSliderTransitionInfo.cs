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
    /// Summary description for TimeSliderTransitionInfo.
    /// </summary>
    public class TimeSliderTransitionInfo
    {
        public MarkerItem TransitionMarker = null;

        protected const string strFullSeq = "Full Sequence: ";
        internal TimeSlider m_ts = null;
        private MCompletionInfo m_ciInfo = null;
        internal MCompletionInfo ciInfo
        {
            get { return m_ciInfo; }
            set
            {
                if (m_ciInfo != null)
                {
                    m_ciInfo.Dispose();
                }
                m_ciInfo = value;
            }
        }

        public void DeleteContents()
        {
            ciInfo = null;
        }

        public TimeSliderTransitionInfo(TimeSlider ts)
        {
            m_ts = ts;
        }

        #region Properties
        public float TotalTime
        {
            get { return m_ts.TotalTime; }
            set { m_ts.TotalTime = value; }
        }

        public MTransition Transition
        {
            get { return m_ts.m_Transition; }
        }

        public MSequence SrcSeq
        {
            get { return m_ts.m_SrcSeq; }
            set { m_ts.m_SrcSeq = value; }
        }

        public MSequence DestSeq
        {
            get { return m_ts.m_DestSeq; }
            set { m_ts.m_DestSeq = value; }
        }

        public float DestSeqLength
        {
            get { return m_ts.m_fDestSeqLength; }
            set { m_ts.m_fDestSeqLength = value; }
        }

        public float SrcSeqLength
        {
            get { return m_ts.m_fSrcSeqLength; }
            set { m_ts.m_fSrcSeqLength = value; }
        }

        public float Duration
        {
            get { return m_ts.m_Transition.Duration;}
        }

        public MTransition.MBlendPair[] BlendPairs
        {
            get { return m_ts.m_Transition.BlendPairs; }
        }

        public SegmentedTrackBarDLL.SegmentedTrackBar SegTrk
        {
            get { return m_ts.segtrkTime; }
        }

        public int TrackRange
        {
            get { return m_ts.m_iTrackRange; }
        }

        public float TrackRangeOverTotalTime
        {
            get { return m_ts.TrackRangeOverTotalTime; }
        }

        public float BeginTime
        {
            get { return m_ts.m_fBeginTime; }
            set { m_ts.m_fBeginTime = value; }
        }
        public float EndTime
        {
            get { return m_ts.m_fEndTime; }
            set { m_ts.m_fEndTime = value; }
        }

        public System.Drawing.Color ColorSrcA
        {
            get { return m_ts.colorSrcA; }
        }
        public System.Drawing.Color ColorSrcB
        {
            get { return m_ts.colorSrcB; }
        }
        public System.Drawing.Color ColorDurA
        {
            get { return m_ts.colorDurA; }
        }
        public System.Drawing.Color ColorDurB
        {
            get { return m_ts.colorDurB; }
        }
        public System.Drawing.Color ColorDestA
        {
            get { return m_ts.colorDestA; }
        }
        public System.Drawing.Color ColorDestB
        {
            get { return m_ts.colorDestB; }
        }
        #endregion

        public MarkerItem GetNextSrcMarker(MarkerItem marker)
        {
            return m_ts.GetNextSrcMarker(marker);
        }
        public MarkerItem GetNextDestMarker(int iFromPos)
        {
            return m_ts.GetNextDestMarker(iFromPos);
        }

        protected void AllignRegions()
        {
            m_ts.AllignRegions();
        }

        public virtual void Setup()
        {
        }

        public virtual void OnMarkerPosChanged(MarkerItem marker)
        {
        }

        public void SetTrackerIncrement()
        {
            SegTrk.Increment = 
                (int)(MFramework.Instance.Animation.IncrementTime *
                TrackRangeOverTotalTime);
        }

        public virtual void OnScrubberPosChanged()
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Animation.CurrentTime = BeginTime +
                ((float) SegTrk.Scrubber.Position) / TrackRangeOverTotalTime;

            if (Transition == null)
                SrcSeq.DefaultPositionTime = 
                    MFramework.Instance.Animation.CurrentTime;
            else
                Transition.DefaultPositionTime = 
                    MFramework.Instance.Animation.CurrentTime;

        }

        public virtual void OnPaint()
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
                return;

            if (BeginTime == EndTime)
                SegTrk.Scrubber.Position = 0;
            else
            {
                int iValue;

                iValue = (int) (MFramework.Instance.Animation.CurrentTime
                    * TrackRangeOverTotalTime);

                if (iValue >= TrackRange)
                    iValue = TrackRange - 1;

                SegTrk.Scrubber.Position = iValue;

                if (Transition == null)
                {
                    if (SrcSeq != null)
                    {
                        SrcSeq.DefaultPositionTime =
                            MFramework.Instance.Animation.CurrentTime;
                    }
                }
                else
                {
                    Transition.DefaultPositionTime =
                        MFramework.Instance.Animation.CurrentTime;
                }
            }
        }

        public float AdjustTimeForFrameRate(float fTime)
        {
            if (fTime >= MFramework.Instance.Animation.IncrementTime)
            {
                // assume a minimal frame rate
                fTime -= MFramework.Instance.Animation.IncrementTime;
            }
            else
            {
                fTime = 0.0f;
            }

            return fTime;
        }

        public void LimitScrubberToLastRegion()
        {
            // Set the range of the scrubber so it can move beyond
            // the end of the sequence.

            int iLastRegion = SegTrk.Regions.Count - 1;
            
            int iEndRange = SegTrk.Regions[iLastRegion].Position + 
                SegTrk.Regions[iLastRegion].Width;

            SegTrk.RangeLimiterRight.Position = iEndRange;
            if (SegTrk.Scrubber.Position > iEndRange)
                SegTrk.Scrubber.Position = iEndRange;
        }

        public float GetFrameFromAdjustedTime(MSequence seq, float fRealTime)
        {
            return seq.TimeMultFreq(fRealTime);
        }

        public float GetTimeDivFreq(MSequence seq, float fFrameTime)
        {
            return seq.TimeDivFreq(fFrameTime);
        }

        public float GetTextKeyTimeDivFreq(
            MSequence seq, MSequence.MTextKey tk)
        {
            return seq.TimeDivFreq(tk.Time);
        }
    }
}
