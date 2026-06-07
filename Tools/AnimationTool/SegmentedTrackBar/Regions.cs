// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace SegmentedTrackBarDLL
{
    public sealed class RegionItem: SegmentedTrackBarDLL.CollectionItem
    {
        public class RegionChangedEventArgs: System.EventArgs
        {
        }
        private Color m_ColorA = new Color();
        private Color m_ColorB = new Color();
        bool m_bUseGradient;
        private LinearGradientMode gradientStyle = LinearGradientMode.ForwardDiagonal;

        public delegate void RegionChangedEventHandler(RegionItem sender, RegionChangedEventArgs e);
        public event RegionChangedEventHandler RegionChanged;

        private int m_iLevel = 0; // Level
        private int m_iPosition = 0;
        private int m_iLastPosition = 0;
        private int m_iWidth = 0;
        private bool m_bVisible = true;
        private System.Drawing.Rectangle m_BoundRect =
            new Rectangle(0,0,0,0);
        private string m_strText;

        private SegmentedTrackBar m_TrackBar = null;

        public RegionItem()
        {
            m_bUseGradient = false;
        }
       
        public string Text
        {
            get { return m_strText; }
            set { m_strText = value; }
        }

        internal void OnRegionChanged()
        {
            if (RegionChanged != null)
            {
                RegionChanged(this, new RegionChangedEventArgs());
            }
        }
        public Color ColorA
        {
            get { return m_ColorA; }
            set { m_ColorA = value; }
        }

        public Color ColorB
        {
            get { return m_ColorB; }
            set { m_ColorB = value; }
        }

        public bool UseGradient
        {
            get { return m_bUseGradient;}
            set { m_bUseGradient = value; }
        }

        public LinearGradientMode GradientFillStyle
        {
            get { return gradientStyle; }
            set { gradientStyle = value; }
        }

        public int Level
        {
            get { return m_iLevel; }
            set { m_iLevel = value; }
        }


        public int Position
        {
            get { return m_iPosition; }
            set 
            {
                m_iPosition = value;
                
                if (m_TrackBar == null)
                {
                    m_iLastPosition = m_iPosition;
                    return;
                }

                System.Drawing.Rectangle oldRect = m_BoundRect;

                int iPixelCurr = m_TrackBar.ConvertPosToPixel(m_iPosition);
                int iPixelLast = m_TrackBar.ConvertPosToPixel(m_iLastPosition);
                int iPixelDiff = iPixelCurr - iPixelLast;

                m_BoundRect.Offset(iPixelDiff,0); // Move rectangle
                m_iLastPosition = m_iPosition;

                oldRect = System.Drawing.Rectangle.Union(oldRect, m_BoundRect);
                m_TrackBar.Invalidate(oldRect, false);

                //SegmentedTrackBarDLL.Events.RaiseRegionPosChanged(this);
            }
        }

        public int Width
        {
            get { return m_iWidth; }
            set
            {
                m_iWidth = value;
                CreateBound();
            }
        }

        public bool Visible
        {
            get { return m_bVisible; }
            set { m_bVisible = value; }
        }

        public void CreateBound()
        {
            if (m_TrackBar == null)
                return;

            CropPosition();

            int iPos = Position;
            int iPixel = m_TrackBar.ConvertPosToPixel(iPos);

            int iW = m_TrackBar.ConvertPosToPixel(iPos + Width); 
            iW = iW - iPixel;

            int iH, iYStart;

            if (Level == -1)
            {
                // Level isn't used
                iH = m_TrackBar.Thickness; 
                iYStart = m_TrackBar.HorzPadding;
            }
            else
            {
                iH = m_TrackBar.LevelHeight; 
                iYStart = m_TrackBar.HorzPadding + (iH * Level);
            }

            int iXStart = iPixel;

            // Always ensure we have minimally, a width of 1.
            if (iW <= 0)
                iW = 1;
            m_BoundRect = new Rectangle(iXStart, iYStart, iW, iH);
        }

        public void SetTrackBar(SegmentedTrackBar trackBar)
        {
            m_TrackBar = trackBar;
            CreateBound();
        }

        public bool HitTest(Point pt)
        {
            return m_BoundRect.Contains(pt);
        }

        public void Draw()
        {
            if (m_TrackBar == null)
                return;

            if (m_bVisible == false)
                return;

            if (DesignMode == true)
            {
                // not sure why, but a design time, the bounds
                // need to be created at a later stage.
                CreateBound(); 
            }

            if (UseGradient)
            {
                //HatchBrush myBrush = new HatchBrush(
                //   HatchStyle.NarrowVertical,//DarkDownwardDiagonal,
                //ColorA, ColorB);

                LinearGradientBrush myBrush = new LinearGradientBrush(
                    m_BoundRect,
                    ColorA,
                    ColorB,
                    GradientFillStyle);

                m_TrackBar.m_DBGraphics.g.FillRectangle(myBrush, m_BoundRect);

                
                // There is a bug with the LinearGradientBrush. Occassionally,
                // the first pixel of the left most edge is the color of the
                // final color in the gradient. So we handle it here by drawing
                // over that with the first color.
                Pen drawingPen1 = new Pen(ColorA, 1);
                Point pt1 = new Point(m_BoundRect.Left, m_BoundRect.Top);
                Point pt2 = new Point(m_BoundRect.Left, m_BoundRect.Bottom -2);
                m_TrackBar.m_DBGraphics.g.DrawLine(drawingPen1,pt1,pt2);

                Pen drawingPenB = new Pen(ColorB, 1);
                Point pt3 = new Point(m_BoundRect.Right, m_BoundRect.Top);
                Point pt4 = new Point(m_BoundRect.Right, m_BoundRect.Bottom -2);
                m_TrackBar.m_DBGraphics.g.DrawLine(drawingPenB,pt3,pt4);

            }
            else
            {
                SolidBrush myBrush = new SolidBrush( ColorA);
                m_TrackBar.m_DBGraphics.g.FillRectangle(myBrush, m_BoundRect);
            }   
        }

        public void CropPosition()
        {
            if (m_TrackBar == null)
                return;

            if (Position < m_TrackBar.RangeLimiterLeft.Position)
                Position = m_TrackBar.RangeLimiterLeft.Position;
            else if (Position > m_TrackBar.RangeLimiterRight.Position)                
                Position = m_TrackBar.RangeLimiterRight.Position;

            if ((Position + Width)> m_TrackBar.RangeLimiterRight.Position)
            {
                Width = m_TrackBar.RangeLimiterRight.Position - Position;
            }
        }
    }

    public class RegionCollection: SegmentedTrackBarDLL.Collection
    {
        public Int32 Add(RegionItem item)
        {
            return base.Add(item);
        }

        public void Insert(Int32 index, RegionItem item)
        {
            base.Insert(index, item);
        }

        public void Remove(RegionItem item)
        {
            base.Remove(item);
            ((IDisposable)item).Dispose();
        }

        public bool Contains(RegionItem item)
        {
            return base.Contains(item);
        }

        public Int32 IndexOf(RegionItem item)
        {
            return base.IndexOf(item);
        }

        public void CopyTo(RegionItem[] array, Int32 index)
        {
            base.CopyTo(array, index);
        }

        public new RegionItem this[Int32 index]
        {
            get
            {
                return (RegionItem)List[index];
            }
            set
            {
                List[index] = value;
            }
        }
    }
}