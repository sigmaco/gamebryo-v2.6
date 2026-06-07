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
using System.Collections;

using System.Reflection;
using System.IO;


namespace SegmentedTrackBarDLL
{
    public sealed class MarkerItem : SegmentedTrackBarDLL.CollectionItem
    {
        //public delegate void MarkerChangedEventHandler(MarkerItem sender, MarkerEventArgs e);
        //public event MarkerChangedEventHandler MarkerChanged;

        //public delegate void MarkerEndDragEventHandler(MarkerItem sender, MarkerEventArgs e);
        //public event MarkerEndDragEventHandler MarkerDragEnded;

        private int m_iPosition = 0;
        private int m_iLastPosition;
        private bool m_bEnabled;
        private int m_iRangeLeft = 0;
        private int m_iRangeRight = 0;
        private bool m_bVisible;
        private bool m_bBelow;
        private bool m_bUseAltImageForDisabled = false;
        private string m_strText;
        private int m_iImages = 8;
        private object m_Tag = null;
        private object m_Tag2 = null;

        // For now, static because we want to share between all
        // markers used by the track... will have to change this
        // implementation if we instantiate more than one
        // SegmentedTrackbar that wants to use different sort of
        // images for the markers.
        static private Bitmap m_bmpMarker = null;
        static private int iMarkerWidth;
        static private int iMarkerHeight;
        
        private SegmentedTrackBar m_TrackBar = null;

        private bool m_bDragging = false;
        private bool m_bMouseOver = false;
        private    Point m_pntDragPoint = new Point(0,0);

        private System.Drawing.Rectangle m_BoundRect =
            new Rectangle(0,0,0,0);

        public MarkerItem()
        {
            m_iPosition = 0;
            m_iLastPosition = 0;
            m_bEnabled = true;
            m_bVisible = true;
            m_bBelow = true;

            if (m_bmpMarker == null)
            {
                Assembly a = Assembly.GetExecutingAssembly();
                // string [] resNames = a.GetManifestResourceNames();
                Stream  imgStream = a.GetManifestResourceStream(
                    "SegmentedTrackBar.Resources.MARKERS.BMP");
                
                if( !(null==imgStream) )
                {                    
                    m_bmpMarker = Bitmap.FromStream( imgStream ) as Bitmap;
                    imgStream.Close();
                    imgStream = null;
                }

                // A little trick to get the proper pixel depth...
                // because the bitmap pixel depth may not be the same
                // as the desktop...
                // [1] Get the current desktop screen graphics.
                // [2] Make a bitmap using that color depth.
                // [3] Draw on the new bitmap
                // [4] Copy loaded bitmap to the screen bitmap
                // [5] Set our original to it...
                Graphics g = Graphics.FromHwnd(IntPtr.Zero);
                Bitmap bitmap = new Bitmap(m_bmpMarker.Width, m_bmpMarker.Height, g);
                g.Dispose();
                g = Graphics.FromImage(bitmap);
                g.DrawImage(m_bmpMarker, 0, 0, bitmap.Width, bitmap.Height);
                g.Dispose();
                m_bmpMarker = bitmap;

                m_bmpMarker.MakeTransparent(System.Drawing.Color.Magenta);

            }


            iMarkerWidth = m_bmpMarker.Width;
            iMarkerHeight = m_bmpMarker.Height / m_iImages;
        }

        public object Tag
        {
            get { return m_Tag; }
            set { m_Tag = value; }
        }
        
        public object Tag2
        {
            get { return m_Tag2; }
            set { m_Tag2 = value; }
        }

        public string Text
        {
            get { return m_strText; }
            set { m_strText = value; }
        }

        public bool BelowBar
        {
            get { return m_bBelow; }
            set { m_bBelow = value; }
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

                SegmentedTrackBarDLL.Events.RaiseMarkerPosChanged(this);
            }
        }

        public bool Enabled
        {
            get { return m_bEnabled; }
            set 
            { 
                m_bEnabled = value; 
                if (m_TrackBar != null)
                    m_TrackBar.Invalidate(m_BoundRect, false);
            }
        }

        public int RangeLeft 
        {
            get { return m_iRangeLeft; }
            set { m_iRangeLeft = value; }
        }
        
        public int RangeRight 
        {
            get { return m_iRangeRight; }
            set { m_iRangeRight = value; }
        }

        public bool Visible
        {
            get { return m_bVisible; }
            set { m_bVisible = value; }
        }

        public bool UseAltImageForDisabled
        {
            get { return m_bUseAltImageForDisabled; }
            set
            {
                m_bUseAltImageForDisabled = value;
            }
        }

        public bool HitTest(Point pt)
        {
            if (Visible == false)
                return false;

            return m_BoundRect.Contains(pt);
        }

        public void SetTrackBar(SegmentedTrackBar trackBar)
        {
            m_TrackBar = trackBar;

            // NOTE 1:
            // It's possible to add the event twice (I think) if the
            // trackbar is added,removed, and added again in the designer.
            //
            // NOTE 2:
            // Removed adding an event handler per marker as it has to be
            // explicitly unregistered and with more markers, you get more
            // handlers. Instead, now we just iterate through the collection
            // and call OnTrackBarMouseMove...
            //
            //if (m_TrackBar != null)
            //    m_TrackBar.MouseMove += new MouseEventHandler(OnTrackBarMouseMove);

            CreateBound();
        }

        public void CreateBound()
        {
            if (m_TrackBar == null)
                return;

            int iPos = Position;
            int iPixel = m_TrackBar.ConvertPosToPixel(iPos);

            int iW = iMarkerWidth; // ScrubberWidth;
            int iH = iMarkerHeight; //m_TrackBar.Thickness; // Scrubber Height
            int iWdiv2 = iW / 2; // ScrubberWidth;

            int iYStart;
            
            if (m_bBelow)
                iYStart = m_TrackBar.VertPadding + m_TrackBar.Thickness + 2;
            else
                iYStart = m_TrackBar.VertPadding - iMarkerHeight - 2;


            int iXStart = iPixel - iWdiv2;

            if (iW <= 0)
                iW = 1;
            m_BoundRect = new Rectangle(iXStart, iYStart, iW, iH);
        }

        public void Draw()
        {
            if (m_bVisible == false)
                return;

            if (DesignMode == true)
            {
                // not sure why, but a design time, the bounds
                // need to be created at a later stage.
                CreateBound(); 
            }

            if (m_iPosition > m_TrackBar.MaxRange)
                return;

            if (m_iPosition < m_TrackBar.MinRange)
                return;
                
            if (m_bEnabled == false)
            {
                if (m_bUseAltImageForDisabled)
                {
                    m_TrackBar.m_DBGraphics.g.DrawImage(
                        m_bmpMarker, 
                        m_BoundRect,
                        0, 6*(m_bmpMarker.Height / m_iImages), m_bmpMarker.Width, iMarkerHeight, 
                        GraphicsUnit.Pixel, null); 
                }
                else
                {
                    if (BelowBar)
                    {
                        m_TrackBar.m_DBGraphics.g.DrawImage(
                            m_bmpMarker, 
                            m_BoundRect,
                            0, 3*(m_bmpMarker.Height / m_iImages), m_bmpMarker.Width,
                            iMarkerHeight, GraphicsUnit.Pixel, null); 
                    }
                    else
                    {
                        m_TrackBar.m_DBGraphics.g.DrawImage(
                            m_bmpMarker, 
                            m_BoundRect,
                            0, 4*(m_bmpMarker.Height / m_iImages), m_bmpMarker.Width,
                            iMarkerHeight, GraphicsUnit.Pixel, null); 
                    }
                }
    
                return;
            }

            if (m_bDragging)
            {
                m_TrackBar.m_DBGraphics.g.DrawImage(
                    m_bmpMarker, 
                    m_BoundRect,
                    0, 2*(m_bmpMarker.Height / m_iImages), m_bmpMarker.Width, iMarkerHeight, 
                    GraphicsUnit.Pixel, null); 
    
                return;
            }

            if (m_bMouseOver)
            {
                m_TrackBar.m_DBGraphics.g.DrawImage(
                    m_bmpMarker, 
                    m_BoundRect,
                    0, m_bmpMarker.Height / m_iImages, m_bmpMarker.Width, iMarkerHeight, 
                    GraphicsUnit.Pixel, null); 

                return;
            }

            m_TrackBar.m_DBGraphics.g.DrawImage(
                m_bmpMarker, 
                m_BoundRect,
                0, 0, m_bmpMarker.Width, iMarkerHeight, 
                GraphicsUnit.Pixel, null); 
        }

        public void Drag(Point pt, System.Windows.Forms.UserControl uc)
        {
            // Ensure that we stay in range.
            int ProposedPixelPos = (pt.X - m_pntDragPoint.X) + 
                (iMarkerWidth / 2);

            int ProposedPos = 
                m_TrackBar.ConvertPixelToPos(ProposedPixelPos);

            if (ProposedPos < RangeLeft)
                Position = RangeLeft;
            else if (ProposedPos > RangeRight)
                Position = RangeRight;
            else if (ProposedPos < m_TrackBar.RangeLimiterLeft.Position)
                Position = m_TrackBar.RangeLimiterLeft.Position;
            else if (ProposedPos > m_TrackBar.RangeLimiterRight.Position)
                Position = m_TrackBar.RangeLimiterRight.Position;
            else
                Position = ProposedPos;    
        }

        public void BeginDrag(Point pt, System.Windows.Forms.UserControl uc)
        {
            m_pntDragPoint.X = pt.X - m_BoundRect.X;          
            m_pntDragPoint.Y = pt.Y - m_BoundRect.Y;
            m_bDragging = true;
            uc.Invalidate(m_BoundRect, false);           
            SegmentedTrackBarDLL.Events.RaiseMarkerDragStarted(this);
        }

        public void EndDrag()
        {
            m_TrackBar.Invalidate(m_BoundRect, false);
            m_bDragging = false;
            SegmentedTrackBarDLL.Events.RaiseMarkerDragEnded(this);
        }

        public bool IsDragging()
        {
            return m_bDragging;
        }

        public void UpdateMouseOver(bool bHitTest)
        {
            if (m_bMouseOver != bHitTest)
            {
                m_bMouseOver = bHitTest;
                m_TrackBar.Invalidate(m_BoundRect, false);
            }
        }
    }

    public class MarkerCollection: SegmentedTrackBarDLL.Collection
    {
        public Int32 Add(MarkerItem item)
        {
            return base.Add(item);
        }

        public void Insert(Int32 index, MarkerItem item)
        {
            base.Insert(index, item);
        }

        public void Remove(MarkerItem item)
        {
            base.Remove(item);
            ((IDisposable)item).Dispose();
        }

        public bool Contains(MarkerItem item)
        {
            return base.Contains(item);
        }

        public Int32 IndexOf(MarkerItem item)
        {
            return base.IndexOf(item);
        }

        public void CopyTo(MarkerItem[] array, Int32 index)
        {
            base.CopyTo(array, index);
        }

        public new MarkerItem this[Int32 index]
        {
            get
            {
                return (MarkerItem)List[index];
            }
            set
            {
                List[index] = value;
            }
        }
    }
}
