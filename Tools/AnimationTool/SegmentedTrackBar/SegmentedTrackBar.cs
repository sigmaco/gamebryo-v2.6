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

using System.Windows.Forms.Design;

namespace SegmentedTrackBarDLL
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	public class SegmentedTrackBar : System.Windows.Forms.UserControl
	{
        private System.ComponentModel.IContainer components;
        private Color m_InRangeColor;
        private Color m_OutOfRangeColor;
        private int m_iThickness;
        private int m_iStart;
        private int m_iEnd;
        private int m_iHorzPadding;
        private int m_iVertPadding;
        public DBGraphics m_DBGraphics;
        public int m_iIncrement;
        public bool m_bUseLevelDividers;

        private MarkerCollection m_MarkerCollection;
        //private RangeLimiters m_RangeLimiters;
        private RangeLimiter m_RangeLimiterLeft;
        private RangeLimiter m_RangeLimiterRight;

        private RegionCollection m_RegionCollection;
        private System.Windows.Forms.ToolTip ttMarker;

        private TicksCollection m_TicksCollection;

        private int m_iLevelHeight;

        private Scrubber m_Scrubber;

		public SegmentedTrackBar()
		{
            this.m_RegionCollection = new RegionCollection();
            this.m_MarkerCollection = new MarkerCollection();
            this.m_TicksCollection = new TicksCollection();

            InitializeComponent();

            m_bUseLevelDividers = true;
            m_iIncrement = 10;
            m_iThickness = 10;
            m_iLevelHeight = m_iThickness;
            m_iHorzPadding = 16;
            m_iVertPadding = 16;
            m_InRangeColor = System.Drawing.Color.Gray;
            m_OutOfRangeColor = System.Drawing.Color.DarkGray;
            m_iStart = 0;
            m_iEnd = 100;

            m_RangeLimiterLeft = new RangeLimiter();
            m_RangeLimiterRight = new RangeLimiter();

            m_DBGraphics = new DBGraphics();
            m_Scrubber = new Scrubber(this);
		}

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.ttMarker = new System.Windows.Forms.ToolTip(this.components);
            // 
            // ttMarker
            // 
            this.ttMarker.AutoPopDelay = 5000;
            this.ttMarker.InitialDelay = 500;
            this.ttMarker.ReshowDelay = 100;
            // 
            // SegmentedTrackBar
            // 
            this.Name = "SegmentedTrackBar";
            this.Size = new System.Drawing.Size(224, 144);
            this.Load += new System.EventHandler(this.SegmentedTrackBar_Load);
            this.SizeChanged += new System.EventHandler(this.SegmentedTrackBar_SizeChanged);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.SegmentedTrackBar_MouseUp);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.SegmentedTrackBar_Paint);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.SegmentedTrackBar_KeyDown);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.SegmentedTrackBar_MouseMove);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.SegmentedTrackBar_MouseDown);

        }
        #endregion

        #region Designer Overrides
        #endregion

        #region Properties

        public bool UseLevelDividers
        {
            get { return m_bUseLevelDividers; }
            set { m_bUseLevelDividers = value; }
        }

        public int Increment
        {
            get { return m_iIncrement; }
            set { m_iIncrement = value; }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public int HorzPadding
        {
            get { return m_iHorzPadding; }
            set 
            {
                m_iHorzPadding = value;
                this.Invalidate();
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public int VertPadding
        {
            get { return m_iVertPadding; }
            set 
            {
                m_iVertPadding = value;
                this.Invalidate();
            }
        }
       
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public SegmentedTrackBarDLL.MarkerCollection Markers
        {
            get { return m_MarkerCollection; }
            set 
            {
                m_MarkerCollection = value;

                foreach (MarkerItem m in m_MarkerCollection)
                {
                    m.SetTrackBar(this);
                }
                
                this.Invalidate();
            }
        }

        public SegmentedTrackBarDLL.TicksCollection Ticks
        {
            get { return m_TicksCollection; }
            set 
            {
                m_TicksCollection = value;

                foreach (TicksItem t in m_TicksCollection)
                {
                    t.SetTrackBar(this);
                }
                
                this.Invalidate();
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public RangeLimiter RangeLimiterLeft
        {
            get { return m_RangeLimiterLeft; }
            set 
            {
                m_RangeLimiterLeft = value;
                this.Invalidate();
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public RangeLimiter RangeLimiterRight
        {
            get { return m_RangeLimiterRight; }
            set 
            {
                m_RangeLimiterRight = value;
                this.Invalidate();
            }
        }
        
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public SegmentedTrackBarDLL.RegionCollection Regions
        {
            get { return m_RegionCollection; }
            set 
            {
                m_RegionCollection = value;

                foreach (RegionItem r in m_RegionCollection)
                {
                    r.SetTrackBar(this);
                }

                this.Invalidate();
            }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
        [Category("General TrackBar Settings")]
        public Scrubber Scrubber
        {
            get { return m_Scrubber; }
            set 
            {
                m_Scrubber = value;
                m_Scrubber.SetTrackBar(this);
                this.Invalidate();
            }
        }

        [Description("The background color for the in range portion of the control")]
        [Category("General TrackBar Settings")]
        public Color BackColorInRange
        {
            get
            {
                return m_InRangeColor;
            }
            set
            {
                m_InRangeColor = value;
                this.Invalidate();
            }
        }

        [Description("The background color for the out of range portion of the control")]
        [Category("General TrackBar Settings")]
        public Color BackColorOutOfRange
        {
            get
            {
                return m_OutOfRangeColor;
            }
            set
            {
                m_OutOfRangeColor = value;
                this.Invalidate();
            }
        }

        [Description("The starting range for the trackbar")]
        [Category("General TrackBar Settings")]
        public int MinRange
        {
            get
            {
                return m_iStart;
            }
            set
            {
                m_iStart = value;
            }
        }

        [Description("The ending range for the trackbar")]
        [Category("General TrackBar Settings")]
        public int MaxRange
        {
            get
            {
                return m_iEnd;
            }
            set
            {
                m_iEnd = value;
            }
        }

        [Description("The thickness of the trackbar")]
        [Category("General TrackBar Settings")]
        public int Thickness
        {
            get
            {
                return m_iThickness;
            }
            set
            {
                m_iThickness = value;
                m_Scrubber.CreateBound();
                this.Invalidate();
            }
        }

        [Description("The height of individual tracks/regions")]
        [Category("General TrackBar Settings")]
        public int LevelHeight
        {
            get
            {
                return m_iLevelHeight;
            }
            set
            {
                m_iLevelHeight = value;
                this.Invalidate();
            }
        }
        #endregion
        
        private void SegmentedTrackBar_Load(object sender, System.EventArgs e)
        {
            // Ensure that all the components have their trackbars set
            foreach (MarkerItem m in m_MarkerCollection)
                m.SetTrackBar(this);
            foreach (RegionItem r in m_RegionCollection)
                r.SetTrackBar(this);
            Scrubber.SetTrackBar(this);

            // Ensure that the scrubber is withing the range of the limiters
            CropComponents();

            m_DBGraphics.CreateDoubleBuffer(this.CreateGraphics(), this.ClientRectangle.Width,
                this.ClientRectangle.Height);

            Invalidate();
        }

        public void SetNumberOfLevels(int iLevels)
        {
            // Creats a uniform height based on number of levels.
            if (iLevels <= 1)
            {
                LevelHeight = Thickness; 
                return;
            }

            LevelHeight = Thickness / iLevels; 
        }

        public void CropComponents()
        {
            m_Scrubber.CropPosition();

            foreach (RegionItem r in m_RegionCollection)
                r.CropPosition();
        }

        public void ClearTicks()
        {
            m_TicksCollection.Clear();
            this.Invalidate();
        }

        public void AddTicks(int iStartPos, int iWidth, int iLevel)
        {
            TicksItem item = new TicksItem();
            item.StartPos = iStartPos;
            item.Width = iWidth;
            item.Level = iLevel;
            item.SetTrackBar(this);
            this.m_TicksCollection.Add(item);
            this.Invalidate();
        }

        public void ClearMarkers()
        {
            Markers.Clear();
            this.Invalidate();
        }

        public MarkerItem AddMarker(
            int iPos, string strText, bool bBelowBar, bool bEnabled )
        {
            MarkerItem item = new MarkerItem();

            item.Enabled = bEnabled;
            item.Position = iPos;
            item.BelowBar = bBelowBar;
            item.RangeLeft = 0; //Default behavior
            item.RangeRight = iPos; //Default Behavior
            item.Text = strText;
            item.SetTrackBar(this);
            item.Visible = true;    

            // item.Tag not currently used.

            Markers.Add(item);
            item.CreateBound();
            this.Invalidate();

            return item;            
        }

        public void ClearRegions()
        {
            Regions.Clear();
            this.Invalidate();
        }

        public void AddRegion(
            int iPos, int iWidth,
            Color colorA, Color colorB, string strText, int iLevel)
        {
            RegionItem item = new RegionItem();
            item.ColorA = colorA;
            item.ColorB = colorB;
            item.GradientFillStyle = LinearGradientMode.Horizontal;
            item.Position = iPos;
            item.Width = iWidth;
            item.Text = strText;
            item.Level = iLevel;
            item.SetTrackBar(this);
            Regions.Add(item);
            this.Invalidate();
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
        }

        private void SegmentedTrackBar_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            if (m_DBGraphics.CanDoubleBuffer() == false)
                return;

			DrawTrack();

            // Render to the form
            m_DBGraphics.Render(e.Graphics);

        }

        private void DrawTrack()
        {
            FloodFillControl();
            DrawOutOfRangeBackground();
            DrawInRangeBackground();

            foreach (RegionItem r in m_RegionCollection)
                r.Draw();

            foreach (TicksItem t in m_TicksCollection)
                t.Draw();

            if (UseLevelDividers)
                DrawLevelDividers();

            foreach (MarkerItem m in m_MarkerCollection)
                m.Draw();

            DrawTrackBorders();     

            // DrawRangeLimiters();
            m_Scrubber.Draw();
        }

        private void SegmentedTrackBar_SizeChanged(object sender, System.EventArgs e)
        {           			
            m_DBGraphics.CreateDoubleBuffer(this.CreateGraphics(), this.ClientRectangle.Width,
                this.ClientRectangle.Height);
            m_Scrubber.CreateBound();
            foreach (RegionItem r in m_RegionCollection)
                r.CreateBound();

            foreach (MarkerItem m in m_MarkerCollection)
                m.CreateBound();

            Invalidate();
        }

        public void FloodFillControl()
        {
            // Temporay fill of entire control
            SolidBrush brushInner;
            brushInner = new SolidBrush(this.BackColor); 
                //Color.FromKnownColor(KnownColor.Control));
            m_DBGraphics.g.FillRectangle(brushInner, 0, 0, this.Width, this.Height);		
        }

        public System.Drawing.Rectangle GetTrackBarRangeRect()
        {
            int iMaxPixel = Size.Width - (2* m_iHorzPadding);
            int iMinPixel = m_iHorzPadding;
            return new System.Drawing.Rectangle(iMinPixel, m_iHorzPadding, 
                iMaxPixel, Thickness);
        }

        public void DrawOutOfRangeBackground()
        {
            // Draw OutOfRange background
            SolidBrush brushInner = new SolidBrush( BackColorOutOfRange );
            m_DBGraphics.g.FillRectangle(brushInner, GetTrackBarRangeRect());
        }

        public void DrawInRangeBackground()
        {
            // Layer on the InRange background
            //SolidBrush brushInner = new SolidBrush( BackColorInRange );

            // We really should expose the ability to change brushes, and styles.
            // Currently, we are using the inrangecolor AND outofrangecolor 
            // to create a cross hatch
            HatchBrush brushInner = new HatchBrush( HatchStyle.DarkDownwardDiagonal,
                this.BackColorInRange, this.BackColorOutOfRange);

            //int iLeftPos = m_RangeLimiters.Left.Position;
            //int iRightPos = m_RangeLimiters.Right.Position;
            int iLeftPos = m_RangeLimiterLeft.Position;
            int iRightPos = m_RangeLimiterRight.Position;

            int iMaxPixel = ConvertPosToPixel(iRightPos);
            int iMinPixel = ConvertPosToPixel(iLeftPos);
            m_DBGraphics.g.FillRectangle(brushInner, iMinPixel, m_iHorzPadding,
                iMaxPixel-iMinPixel, Thickness);		
        }

        public void DrawTrackBorders()
        {
            // Draw the Border edges using color cues
            int iMinX = m_iHorzPadding - 2;
            int iMinY = m_iVertPadding - 2;
            int iWidth = Size.Width - (2* (m_iHorzPadding -2) - 1);
            int iHeight = Size.Height - (2 * (m_iVertPadding -2) -1);
            
            System.Drawing.Rectangle r = new Rectangle(
                iMinX, iMinY, iWidth, iHeight);

            ControlPaint.DrawBorder3D(m_DBGraphics.g,r);
        }

        public void DrawLevelDividers()
        {
            if (LevelHeight >= Thickness)
                return;

            //Point pt1, pt2;
            int iHeight = LevelHeight;

            int iMinXPixel = ConvertPosToPixel(MinRange);
            int iMaxXPixel = ConvertPosToPixel(MaxRange);

            while (iHeight < Thickness)
            {
                int iH = VertPadding + iHeight;
                iHeight += iHeight;
                // Must subtract off for 3d border thickness
                Rectangle r = new Rectangle(iMinXPixel-2,iH-1,
                    iMaxXPixel-12,1);
                ControlPaint.DrawBorder3D(m_DBGraphics.g,r);
            }
        }

        public int ConvertPixelToPos(int iPixel)
        {
            // Position to Pixel 
            Size szPixelRange = Size;
            szPixelRange.Width -= (2 * m_iHorzPadding);
            szPixelRange.Height = Thickness;

            iPixel -= (m_iHorzPadding);

            // iPos percentage of the szTrackRange
            float fNum = Convert.ToSingle(iPixel);
            float fDenom = Convert.ToSingle(szPixelRange.Width);
            float fPosPercentage =  fNum / fDenom;  

            int iPosRange = MaxRange - MinRange;
            int iPos = (int)(fPosPercentage * iPosRange);
            iPos += MinRange;
            
            return iPos;                     
        }

        public int ConvertPosToPixel(int iPos)
        {
            // Position to Pixel 
            Size szPixelRange = Size;
            szPixelRange.Width -= (2 * m_iHorzPadding);
            szPixelRange.Height = Thickness;

            // For the following, we really should
            // ensure that the control is no smaller than twice
            // the padding..
            if (iPos < MinRange)
                iPos = MinRange;
            else if (iPos > MaxRange)
                iPos = MaxRange;

            // iPos percentage of the szTrackRange
            float fNum = Convert.ToSingle(iPos - MinRange);
            float fDenom = Convert.ToSingle(MaxRange - MinRange);
            float fPosPercentage =  fNum / fDenom;  

            int iPixel = (int)(fPosPercentage * szPixelRange.Width)
                + m_iHorzPadding;

            return iPixel;                     
        }

        public RegionItem GetScubberRegion()
        {
            int iXPixel = ConvertPosToPixel(Scrubber.Position);
            int iYPixel = HorzPadding + Thickness / 2;

            Point pt = new Point(iXPixel,iYPixel);

            foreach (RegionItem r in m_RegionCollection)
            {
                if (r.HitTest(pt) == false)
                    continue;

                return r;
            }

            return null;
        }

        private void SegmentedTrackBar_MouseMove(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            // We don't handle mousemove if either the scrubber or the
            // entire control is disabled.
            //if (Scrubber.Enabled == false || Enabled == false)
            //    return;

            Point pt = new Point(e.X, e.Y);

            if (Scrubber.Enabled == true && Enabled == true)
                if (Scrubber.IsDragging())
                {
                    Scrubber.Drag(pt, this);  
                    return;
                }

            // optimize so that we always know the marker that
            // is being dragged...
            foreach (MarkerItem m in m_MarkerCollection)
            {
                if (m.IsDragging())
                {
                    m.Drag(pt, this);
                    return;
                }
            }

            if (Scrubber.HitTest(pt))
            {
                ttMarker.Active = false;
                return;
            }

            foreach (RegionItem r in m_RegionCollection)
            {
                if (r.HitTest(pt))
                {
                    ttMarker.SetToolTip(this, r.Text);
                    ttMarker.Active = true;
                    return;
                }
            }

            foreach (MarkerItem m in m_MarkerCollection)
            {
                bool bHitTest = m.HitTest(pt);
                m.UpdateMouseOver(bHitTest);

                if (bHitTest)
                {
                    // Activate the tool tip
                    ttMarker.SetToolTip(this, m.Text);
                    ttMarker.Active = true;
                    return;
                }
            }     
            ttMarker.Active = false;
        }

        private void SegmentedTrackBar_MouseDown(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                Point pt = new Point(e.X,e.Y);
                if (Scrubber.HitTest(pt))
                {
                    this.Capture = true;
                    m_Scrubber.BeginDrag(pt, this);				
                    return;
                }
                
                // optimize to have a list of markers that can
                // be moved...
                foreach (MarkerItem m in m_MarkerCollection)
                {
                    if (m.Enabled == false)
                        continue;

                    if (m.HitTest(pt))
                    {
                        this.Capture = true;
                        m.BeginDrag(pt, this);				
                        return;
                    }
                }

                m_Scrubber.RePosition(pt);
            }
        }

        private void SegmentedTrackBar_MouseUp(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            if (Scrubber.IsDragging())
                Scrubber.EndDrag();        

            // Optimize below so that we always know the exact
            // marker that we need to question...
            foreach (MarkerItem m in m_MarkerCollection)
            {
                if (m.Enabled == false)
                    continue;

                if (m.IsDragging())
                    m.EndDrag();
            }
        }

        protected override bool IsInputKey(Keys keyData)
        {
            switch(keyData)
            {
                case Keys.Right:
                case Keys.Left:
                    return true;
            }

            return base.IsInputKey (keyData);
        }

        private void SegmentedTrackBar_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            switch(e.KeyData)
            {
                case Keys.Left:
                    Scrubber.MoveLeft(m_iIncrement);
                    break;
                case Keys.Right:
                    Scrubber.MoveRight(m_iIncrement);
                    break;
                default:
                    break;
            }      
        }
	}
}
