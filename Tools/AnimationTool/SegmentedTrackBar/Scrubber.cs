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

using System.Reflection;
using System.IO;

namespace SegmentedTrackBarDLL
{
    /// <summary>
    /// Summary description for Scrubber.
    /// </summary>
    [TypeConverter(typeof(ExpandableObjectConverter))]
    [Description("Scrubber Properties")]
    public class Scrubber
    {
        private int m_iPosition;
        private int m_iLastPosition;
        private bool m_bEnabled;
        private bool m_bVisible;
        private System.Drawing.Rectangle m_BoundRect;

        private SegmentedTrackBar m_TrackBar;
        private int iScrubberWidth;
        private int iScrubberHeight;

        private bool m_bDragging;
        private bool m_bMouseOver;
        private    Point m_pntDragPoint = new Point(0,0);
        private Bitmap m_bmpScrubber = null;

        public Scrubber(SegmentedTrackBar kTrackBar)
        {
            m_TrackBar = kTrackBar;
            m_iPosition = 0;
            m_iLastPosition = m_iPosition;
            m_bEnabled = true;
            m_bVisible = true;
            
            CreateBound();

            this.m_TrackBar.MouseMove += new MouseEventHandler(OnTrackBarMouseMove);

            if (m_bmpScrubber == null)
            {
                Assembly a = Assembly.GetExecutingAssembly();
                // string [] resNames = a.GetManifestResourceNames();
                //Stream  imgStream = a.GetManifestResourceStream(
                //  "SegmentedTrackBar.Resources.SCRUBBERIMAGES.BMP");
                //Stream  imgStream = a.GetManifestResourceStream(
                //  "SegmentedTrackBar.Resources.SCRUBBERIMAGESFLAT.BMP");
                Stream  imgStream = a.GetManifestResourceStream(
                    "SegmentedTrackBar.Resources.SCRUBBERS.BMP");

                if( !(null==imgStream) )
                {                    
                    m_bmpScrubber = Bitmap.FromStream( imgStream ) as Bitmap;
                    imgStream.Close();
                    imgStream = null;
                } 
            }
            
            //if (m_bmpScrubber == null)
            //    m_bmpScrubber = new Bitmap(
            //      @"D:\GB1.2\Tools\SegmentedTrackBar\BLUE_TRACKBARDOWN13.BMP"); 


            // A little trick to get the proper pixel depth...
            // because the bitmap pixel depth may not be the same
            // as the desktop...
            // [1] Get the current desktop screen graphics.
            // [2] Make a bitmap using that color depth.
            // [3] Draw on the new bitmap
            // [4] Copy loaded bitmap to the screen bitmap
            // [5] Set our original to it...
            Graphics g = Graphics.FromHwnd(IntPtr.Zero);
            Bitmap bitmap = new Bitmap(m_bmpScrubber.Width, m_bmpScrubber.Height, g);
            g.Dispose();
            g = Graphics.FromImage(bitmap);
            g.DrawImage(m_bmpScrubber, 0, 0, bitmap.Width, bitmap.Height);
            g.Dispose();
            m_bmpScrubber = bitmap;

            m_bmpScrubber.MakeTransparent(System.Drawing.Color.Magenta);
            
            iScrubberWidth = m_bmpScrubber.Width;
            iScrubberHeight = m_bmpScrubber.Height / 5;
        }

        #region Properties
        public int Width
        {
            get { return iScrubberWidth; }
            set { iScrubberWidth = value; }
        }

        public int Height
        {
            get { return iScrubberHeight; }
            set { iScrubberHeight = value; }
        }

        public int Position
        {
            get
            {
                return m_iPosition;
            }
            set
            {
                if (m_iPosition == value)
                    return;

                System.Drawing.Rectangle oldRect = this.m_BoundRect;

                m_iPosition = value;
                
                int iPixelCurr = m_TrackBar.ConvertPosToPixel(m_iPosition);
                int iPixelLast = m_TrackBar.ConvertPosToPixel(m_iLastPosition);
                int iPixelDiff = iPixelCurr - iPixelLast;

                m_BoundRect.Offset(iPixelDiff,0); // Move rectangle
                m_iLastPosition = m_iPosition;
                Events.RaiseScrubberPosChanged();

                // assumes left to right movement...

                oldRect = System.Drawing.Rectangle.Union(oldRect, m_BoundRect);
                m_TrackBar.Invalidate(oldRect, false);
            }
        }

        public bool Enabled
        {
            get
            {
                return m_bEnabled;
            }
            set
            {
                m_bEnabled = value;
                m_TrackBar.Invalidate(m_BoundRect, false);
            }
        }
        
        public bool Visible
        {
            get
            {
                return m_bVisible;
            }
            set
            {
                m_bVisible = value;
                m_TrackBar.Invalidate(m_BoundRect, false);
            }
        }
        #endregion

        public bool HitTest(Point pt)
        {
            return m_BoundRect.Contains(pt);
        }

        public void CreateBound()
        {
            int iPos = Position;
            int iPixel = m_TrackBar.ConvertPosToPixel(iPos);

            int iW = iScrubberWidth; // ScrubberWidth;
            int iH = iScrubberHeight; //m_TrackBar.Thickness; // Scrubber Height
            int iWdiv2 = iW / 2; // ScrubberWidth;

            //int iYStart = m_TrackBar.VertPadding - 4;
            int iYStart = m_TrackBar.VertPadding + m_TrackBar.Thickness / 2 - iH/2;
            int iXStart = iPixel - iWdiv2;

            if (iW <= 0)
                iW = 1;
            m_BoundRect = new Rectangle(iXStart, iYStart, iW, iH);
        }

        public void Draw()
        {   
            if (m_bVisible == false)
                return;
           
            if (m_bEnabled == false)
            {
                // Draw as disabled
                //m_TrackBar.m_DBGraphics.g.VisibleClipBounds;
                m_TrackBar.m_DBGraphics.g.DrawImage(
                    m_bmpScrubber, 
                    m_BoundRect,
                    0, 4 * (m_bmpScrubber.Height / 5), m_bmpScrubber.Width, iScrubberHeight, 
                    GraphicsUnit.Pixel, null); 
                return;
            }

            if (m_bDragging)
            {
                m_TrackBar.m_DBGraphics.g.DrawImage(
                    m_bmpScrubber, 
                    m_BoundRect,
                    0, m_bmpScrubber.Height / 5, m_bmpScrubber.Width, iScrubberHeight, 
                    GraphicsUnit.Pixel, null); 

                return;
            }

            if (m_bMouseOver)
            {
                m_TrackBar.m_DBGraphics.g.DrawImage(
                    m_bmpScrubber, 
                    m_BoundRect,
                    0, m_bmpScrubber.Height / 5, m_bmpScrubber.Width, iScrubberHeight, 
                    GraphicsUnit.Pixel, null); 

                return;
            }

            m_TrackBar.m_DBGraphics.g.DrawImage(
                m_bmpScrubber, 
                m_BoundRect,
                0, 0, m_bmpScrubber.Width, iScrubberHeight, 
                GraphicsUnit.Pixel, null); 
        }

        public void SetTrackBar(SegmentedTrackBar trackBar)
        {
            m_TrackBar = trackBar;
        }

        public void CropPosition()
        {
            if (Position < m_TrackBar.RangeLimiterLeft.Position)
                Position = m_TrackBar.RangeLimiterLeft.Position;
            else if (Position > m_TrackBar.RangeLimiterRight.Position)
                Position = m_TrackBar.RangeLimiterRight.Position;
            CreateBound();
        }

        public void MoveLeft(int iIncrement)
        {
            Move(Position - iIncrement);
        }

        public void MoveRight(int iIncrement)
        {
            Move(Position + iIncrement);
        }

        public void Move(int iProposedPos)
        {
            if (Enabled == false)
                return;

            // Ensure that we stay in range.
            if (iProposedPos < m_TrackBar.RangeLimiterLeft.Position)
                Position = m_TrackBar.RangeLimiterLeft.Position;
            else if (iProposedPos > m_TrackBar.RangeLimiterRight.Position)
                Position = m_TrackBar.RangeLimiterRight.Position;
            else
                Position = iProposedPos;
        }

        public void RePosition(Point pt)
        {
            if (Enabled == false)
                return;

            // Ensure that we stay in range.
            int ProposedPixelPos = pt.X + (iScrubberWidth / 2);

            System.Drawing.Rectangle r = m_TrackBar.GetTrackBarRangeRect();
            if (r.Contains(ProposedPixelPos, pt.Y) == false)
                return;

            int ProposedPos = 
                m_TrackBar.ConvertPixelToPos(ProposedPixelPos);

            if (ProposedPos >= m_TrackBar.RangeLimiterLeft.Position)
            {
                if (ProposedPos <= m_TrackBar.RangeLimiterRight.Position)
                {
                    Position = ProposedPos;
                    SegmentedTrackBarDLL.Events.RaiseScrubberRepositioned();
                }
            }
        }

        public void Drag(Point pt, System.Windows.Forms.UserControl uc)
        {         
            if (Enabled == false)
                return;

            // Ensure that we stay in range.
            int ProposedPixelPos = (pt.X - m_pntDragPoint.X) + 
                (iScrubberWidth / 2);

            int ProposedPos = 
                m_TrackBar.ConvertPixelToPos(ProposedPixelPos);

            if (ProposedPos < m_TrackBar.RangeLimiterLeft.Position)
                Position = m_TrackBar.RangeLimiterLeft.Position;
            else if (ProposedPos > m_TrackBar.RangeLimiterRight.Position)
                Position = m_TrackBar.RangeLimiterRight.Position;
            else
                Position = ProposedPos;
        }

        public void BeginDrag(Point pt, System.Windows.Forms.UserControl uc)
        {
            if (Enabled == false)
                return;

            m_pntDragPoint.X = pt.X - m_BoundRect.X;          
            m_pntDragPoint.Y = pt.Y - m_BoundRect.Y;
            m_bDragging = true;
            uc.Invalidate(m_BoundRect, false);           
        }

        public void EndDrag()
        {
            if (Enabled == false)
                return;

            m_TrackBar.Invalidate(m_BoundRect, false);
            m_bDragging = false;
        }

        public bool IsDragging()
        {
            return m_bDragging;
        }

        public void OnTrackBarMouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            Point pt = new Point(e.X, e.Y);
            bool bResult = HitTest(pt);
            if (bResult != m_bMouseOver)
            {
                m_bMouseOver = bResult;
                m_TrackBar.Invalidate(m_BoundRect, false);
            }
        }
    }
}
