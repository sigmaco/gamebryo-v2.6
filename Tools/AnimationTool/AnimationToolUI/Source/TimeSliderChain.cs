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
using System.Drawing;

using SegmentedTrackBarDLL;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for TimeSliderChain.
    /// </summary>
    public class TimeSliderChain : TimeSliderTransitionInfo
    {
        public TimeSliderChain(TimeSlider ts) : base(ts) {}          
        private bool m_bEven;
        private Color m_ColorA;
        private Color m_ColorB;
        private int m_iLevel;

        public void SetColor()
        {
            if (m_bEven)
            {
                m_ColorA = ColorSrcA;
                m_ColorB = ColorSrcB;
                m_iLevel = 0;
            }
            else
            {
                m_ColorA = ColorDestA;
                m_ColorB = ColorDestB;
                m_iLevel = 1;
            }
            m_bEven = !m_bEven;
        }

        public override void Setup()
        {
            // Ensure transitioning starts correctly.
            MTransition.MChainInfo[] aCI = m_ts.m_Transition.ChainInfo;

            float fTransitionTime = SrcSeq.DurationDivFreq -
                MFramework.Instance.Animation.IncrementTime;

            MFramework.Instance.Animation.TransitionStartTime =
                fTransitionTime;

            float fTransitionFrame = SrcSeq.Duration -
                MFramework.Instance.Animation.IncrementTime;

            MAnimation mAnimation = MFramework.Instance.Animation;
            MCompletionInfo ki =
                mAnimation.FindTimeForAnimationToCompleteTransition(
                Transition, 0, fTransitionFrame);

            TotalTime = ki.TimeForChainToComplete;

            BeginTime = 0;
            EndTime = TotalTime;

            // Use the chain comletion info to build regions
            MChainCompletionInfo[] aCCI = 
                ki.ChainCompletionInfo;

            m_bEven = true;
            m_iLevel = 0;

            int iStart = 0;
            int iTransStart = 0;
            int iTransEnd = 0;

            string strText;
            float fTime;

            foreach (MChainCompletionInfo cci in aCCI)
            {
                SetColor();

                iStart = (int)(cci.SeqStart * TrackRangeOverTotalTime);
                iTransStart = (int)(cci.TransStart * TrackRangeOverTotalTime);
                iTransEnd = (int)(cci.TransEnd * TrackRangeOverTotalTime);

                fTime = (cci.TransStart - cci.SeqStart);
                strText = cci.Name + " (Time in Seq began at: " + 
                    cci.InSeqBeginFrame +
                    ")" +
                    "\nTime Range: " + cci.SeqStart.ToString("f3") +
                    " to " + cci.TransStart.ToString("f3") +
                    "\nTotal Time Spent in Region: " + fTime.ToString("f3");

                if ((iTransStart - iStart) > 0)
                    SegTrk.AddRegion( 
                        iStart, iTransStart - iStart, m_ColorA, m_ColorB, 
                        strText, m_iLevel);
                
                string strTran = 
                    ProjectData.GetTypeNameForTransitionType(
                    cci.TransitionType);

                fTime = cci.TransEnd - cci.TransStart;
                strText = cci.Name + " [" + strTran + 
                    "] " + cci.NextName + 
                    "\nTime Range: " + cci.TransStart.ToString("f3") +
                    " to " + cci.TransEnd.ToString("f3") +
                    "\nTotal Time Spent in Region: " + fTime.ToString("f3");

                switch (cci.TransitionType)
                {
                    case MTransition.TransitionType.Trans_ImmediateBlend:
                    case MTransition.TransitionType.Trans_DelayedBlend:
                        SegTrk.AddRegion( iTransStart, iTransEnd - iTransStart,
                            System.Drawing.Color.Yellow,
                            System.Drawing.Color.Yellow, 
                            strText , -1);               
                        break;
                    default:
                        // crossfades and morph
                        SegTrk.AddRegion( iTransStart, iTransEnd - iTransStart,
                            m_ColorA, m_ColorB, 
                            strText , m_iLevel);               
                        SegTrk.Regions[SegTrk.Regions.Count-1].UseGradient =
                            true;
                        SetColor();

                        // Swap colors for second part...
                        Color tmpColor = m_ColorA;
                        m_ColorA = m_ColorB;
                        m_ColorB = tmpColor;


                        SegTrk.AddRegion( iTransStart, iTransEnd - iTransStart,
                            m_ColorA, m_ColorB, 
                            strText , m_iLevel);               
                        SegTrk.Regions[SegTrk.Regions.Count-1].UseGradient =
                            true;

                        m_bEven = !m_bEven;
                        break;
                }
            }
 
            SetColor();
            iStart = iTransStart + (iTransEnd - iTransStart);
            float fStart = aCCI[aCCI.Length-1].TransStart +
                (aCCI[aCCI.Length-1].TransEnd - 
                 aCCI[aCCI.Length-1].TransStart);
            fTime = (TotalTime - fStart);
            strText = Transition.Destination.Name +
                "\nTime Range: " + fStart.ToString("f3") +
                " to " + TotalTime.ToString("f3") + 
                "\nTotal Time Spent in Region: " + fTime.ToString("f3");

            SegTrk.AddRegion( iStart, TrackRange - iStart, m_ColorA, m_ColorB, 
                strText, m_iLevel);

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

            MFramework.Instance.Animation.RunUpTime(
                MFramework.Instance.Animation.CurrentTime);
        }
    }
}
