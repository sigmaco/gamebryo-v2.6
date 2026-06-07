using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace SourceGrid2
{
	/// <summary>
	/// A control with a custom implementation of a scrollable area
	/// </summary>
#if !MINI
	[System.ComponentModel.ToolboxItem(false)]
#endif
	public abstract class CustomScrollControl : System.Windows.Forms.Panel
	{
		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		public CustomScrollControl()
		{
#if !MINI
			SuspendLayout();

			base.AutoScroll = false;


			//to remove flicker and use custom draw
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			//SetStyle(ControlStyles.DoubleBuffer, true);
			//SetStyle(ControlStyles.Opaque, true);
			//SetStyle(ControlStyles.ResizeRedraw, true);

			ResumeLayout(false);
#endif
		}

		#endregion

#if !MINI
		#region override AutoScroll
		/// <summary>
		/// I disabled the default AutoScroll property because I have a custom implementation
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public override bool AutoScroll
		{
			get{return false;}
			set
			{
				if (value)
					throw new SourceGridException("Auto Scroll not supported in this control");
				base.AutoScroll = false;
			}
		}
		#endregion
#endif

		#region ScrollBars and Panels
		private VScrollBar m_VScroll = null;
		private HScrollBar m_HScroll = null;
		private Panel m_BottomRightPanel = null;

		/// <summary>
		/// Represent the vertical scrollbar. Can be null.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public VScrollBar VScrollBar
		{
			get{return m_VScroll;}
		}

		/// <summary>
		/// Represent the horizontal scrollbar. Can be null.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public HScrollBar HScrollBar
		{
			get{return m_HScroll;}
		}

		/// <summary>
		/// Represent the panel at the bottom right of the control. This panel is valid only if HScrollBar and VScrollBar are valid. Otherwise is null.
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public Panel BottomRightPanel
		{
			get{return m_BottomRightPanel;}
		}

		/// <summary>
		/// Invalidate the scrollable area
		/// </summary>
		protected abstract void InvalidateScrollableArea();
		#endregion

		#region ScrollArea, ScrollPosition, DisplayRectangle
		private Size m_CustomScrollArea = new Size(0,0);

		/// <summary>
		/// Represent the logical area of the control that must be used for scrolling
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public virtual Size CustomScrollArea
		{
			get{return m_CustomScrollArea;}
			set
			{
				if (m_CustomScrollArea != value)
				{
					if (value.Height < 0)
						value.Height = 0;
					if (value.Width < 0)
						value.Width = 0;

					m_CustomScrollArea = value;
					RecalcCustomScrollBars();
				}
			}
		}

		/// <summary>
		/// Represent the current scroll position relative to the CustomScrollArea. The value must be always between 0 and - CustomScrollArea (0 or negative).
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public virtual Point CustomScrollPosition
		{
			get
			{
				int l_x = 0;
				if (m_HScroll!=null)
					l_x = -m_HScroll.Value;
				int l_y = 0;
				if (m_VScroll!=null)
					l_y = -m_VScroll.Value;
				return new Point(l_x,l_y);
			}
			set
			{
				if (m_HScroll!=null)
				{
					if (value.X < 0)
						m_HScroll.Value = -value.X;
					else
						m_HScroll.Value = 0;
				}

				if (m_VScroll!=null)
				{
					if (value.Y < 0)
						m_VScroll.Value = -value.Y;
					else
						m_VScroll.Value = 0;
				}
			}
		}

		/// <summary>
		/// Display rectangle of the control, without ScrollBars
		/// </summary>
#if !MINI
		public override Rectangle DisplayRectangle
#else
		public virtual Rectangle DisplayRectangle
#endif
		{
			get
			{
				int l_ScrollH = 0;
				if (m_HScroll!=null)
					l_ScrollH = m_HScroll.Height;

				int l_ScrollV = 0;
				if (m_VScroll!=null)
					l_ScrollV = m_VScroll.Width;

#if !MINI
				Rectangle l_Base = base.DisplayRectangle;
#else
				Rectangle l_Base = base.ClientRectangle;
#endif
				return new Rectangle(l_Base.X, l_Base.Y, l_Base.Width-l_ScrollV, l_Base.Height-l_ScrollH);
			}
		}


		#endregion

		#region Point and Rectangle Relative/Absolute conversion

		/// <summary>
		/// Convert an absolute rectangle from the total scrolling area to the current displayrectangle.
		/// </summary>
		/// <param name="p_AbsoluteRectangle"></param>
		/// <returns></returns>
		public Rectangle RectangleAbsoluteToRelative(Rectangle p_AbsoluteRectangle)
		{
			Point l_Point = PointAbsoluteToRelative(p_AbsoluteRectangle.Location);
			return new Rectangle(l_Point.X, l_Point.Y, p_AbsoluteRectangle.Size.Width, p_AbsoluteRectangle.Size.Height);
		}

		/// <summary>
		/// Convert a relative rectangle from the current displayrectangle to an absolute rectangle for the current scrolling area.
		/// </summary>
		/// <param name="p_RelativeRectangle"></param>
		/// <returns></returns>
		public Rectangle RectangleRelativeToAbsolute(Rectangle p_RelativeRectangle)
		{
			Point l_Point = PointRelativeToAbsolute(p_RelativeRectangle.Location);
			return new Rectangle(l_Point.X, l_Point.Y, p_RelativeRectangle.Size.Width, p_RelativeRectangle.Size.Height);
		}

		/// <summary>
		/// Convert a relative point from the current displayrectangle to an absolute point to the total scrolling area.
		/// </summary>
		/// <param name="p_RelativePoint"></param>
		/// <returns></returns>
		public Point PointRelativeToAbsolute(Point p_RelativePoint)
		{
			Point l_ScrollPos = CustomScrollPosition;
			return new Point(p_RelativePoint.X-l_ScrollPos.X, p_RelativePoint.Y-l_ScrollPos.Y);
		}

		/// <summary>
		/// Convert a absolute point from an absolute point to the current displayrectangle of the grid.
		/// </summary>
		/// <param name="p_AbsolutePoint"></param>
		/// <returns></returns>
		public Point PointAbsoluteToRelative(Point p_AbsolutePoint)
		{
			Point l_ScrollPos = CustomScrollPosition;
			return new Point(p_AbsolutePoint.X+l_ScrollPos.X, p_AbsolutePoint.Y+l_ScrollPos.Y);
		}

		#endregion

		#region Add/Remove ScrollBars
		/// <summary>
		/// Remove the horizontal scrollbar
		/// </summary>
		protected virtual void RemoveHScrollBar()
		{
			if (m_HScroll != null)
			{
				HScrollBar l_tmp = m_HScroll;
				m_HScroll = null;
				l_tmp.ValueChanged -= new EventHandler(HScroll_Change);
				Controls.Remove(l_tmp);
				l_tmp.Dispose();
				l_tmp = null;
			}
			m_OldHScrollValue = 0;
		}
		/// <summary>
		/// Remove the vertical scrollbar
		/// </summary>
		protected virtual void RemoveVScrollBar()
		{
			if (m_VScroll != null)
			{
				VScrollBar l_tmp = m_VScroll;
				m_VScroll = null;
				l_tmp.ValueChanged -= new EventHandler(VScroll_Change);
				Controls.Remove(l_tmp);
				l_tmp.Dispose();
				l_tmp = null;
			}
			m_OldVScrollValue = 0;
		}
		/// <summary>
		/// Insert the horizontal scroll bar
		/// </summary>
		protected virtual void InsertHScrollBar()
		{
			if (m_HScroll == null)
			{
				m_HScroll = new HScrollBar();
#if !MINI
				m_HScroll.TabStop = false;
#endif
				m_HScroll.ValueChanged += new EventHandler(HScroll_Change);
				Controls.Add(m_HScroll);
			}
		}
		/// <summary>
		/// Insert the vertical scroll bar
		/// </summary>
		protected virtual void InsertVScrollBar()
		{
			if (m_VScroll == null)
			{
				m_VScroll = new VScrollBar();
#if !MINI
				m_VScroll.TabStop = false;
#endif
				m_VScroll.ValueChanged += new EventHandler(VScroll_Change);
				Controls.Add(m_VScroll);
			}
		}

		/// <summary>
		/// recalculate the position of the horizontal scrollbar
		/// </summary>
		protected virtual void RecalcHScrollBar()
		{
			if (m_HScroll != null)
			{
				int l_WidthVScroll = 0;
				if (m_VScroll != null)
					l_WidthVScroll = m_VScroll.Width;

				m_HScroll.Location = new Point(0,ClientRectangle.Height-m_HScroll.Height);
				m_HScroll.Width = ClientRectangle.Width-l_WidthVScroll;
				m_HScroll.Minimum = 0;
				m_HScroll.Maximum = m_CustomScrollArea.Width; //Math.Max(0,m_CustomScrollArea.Width - ClientRectangle.Width) + m_VScroll.Width;
				m_HScroll.LargeChange = Math.Max( Math.Min(5, m_HScroll.Maximum),
												ClientRectangle.Width - l_WidthVScroll);
				if (m_HScroll.LargeChange < 5)
					m_HScroll.SmallChange = 1;
				else
					m_HScroll.SmallChange = m_HScroll.LargeChange / 5;

				if (m_HScroll.Value > MaximumHScroll)
					m_HScroll.Value = MaximumHScroll;

				m_HScroll.BringToFront();
			}
		}

		/// <summary>
		/// Recalculate the position of the vertical scrollbar
		/// </summary>
		protected virtual void RecalcVScrollBar()
		{
			if (m_VScroll != null)
			{
				int l_HeightHScroll = 0;
				if (m_HScroll != null)
					l_HeightHScroll = m_HScroll.Height;

				m_VScroll.Location = new Point(ClientRectangle.Width-m_VScroll.Width,0);
				m_VScroll.Height = ClientRectangle.Height-l_HeightHScroll;
				m_VScroll.Minimum = 0;
				m_VScroll.Maximum = m_CustomScrollArea.Height; //Math.Max(0,m_CustomScrollArea.Height - ClientRectangle.Height) + m_HScroll.Height;
				m_VScroll.LargeChange = Math.Max( Math.Min(5, m_VScroll.Maximum),
												ClientRectangle.Height - l_HeightHScroll);
				if (m_VScroll.LargeChange < 5)
					m_VScroll.SmallChange = 1;
				else
					m_VScroll.SmallChange = m_VScroll.LargeChange / 5;

				if (m_VScroll.Value > MaximumVScroll)
					m_VScroll.Value = MaximumVScroll;

				m_VScroll.BringToFront();
			}
		}

		/// <summary>
		/// Recalculate the scrollbars position and size
		/// </summary>
		protected virtual void RecalcCustomScrollBars()
		{
			//Check the size of the control
#if !MINI
			Size l_BaseDisplaySize = base.DisplayRectangle.Size; //il base non tiene conto delle scrollbar ed è quello che voglio
#else
			Size l_BaseDisplaySize = base.ClientRectangle.Size; //il base non tiene conto delle scrollbar ed è quello che voglio
#endif
			if (l_BaseDisplaySize.Height < m_CustomScrollArea.Height)
				InsertVScrollBar();
			else
				RemoveVScrollBar();
			if (l_BaseDisplaySize.Width < m_CustomScrollArea.Width)
				InsertHScrollBar();
			else
				RemoveHScrollBar();
			//Re-Check with the size of the ClientRectangle (that if before I added a scrollbar is smaller then the Size)
			Size l_DisplaySize = DisplayRectangle.Size;
			if (l_DisplaySize.Height < m_CustomScrollArea.Height)
				InsertVScrollBar();
			if (l_DisplaySize.Width < m_CustomScrollArea.Width)
				InsertHScrollBar();

			//if there is only one or zero scrollbar remove the BottomRightPanel
			if (m_HScroll == null || m_VScroll == null)
			{
				if (m_BottomRightPanel!=null)
				{
					Controls.Remove(m_BottomRightPanel);
					m_BottomRightPanel.Dispose();
					m_BottomRightPanel = null;
				}
			}

			RecalcVScrollBar();
			RecalcHScrollBar();

			//se sono tutti e due abilitati aggiungo il BottomRightPanel
			if (m_HScroll != null && m_VScroll != null)
			{
				if (m_BottomRightPanel==null)
				{
					m_BottomRightPanel = new Panel();
#if !MINI
					m_BottomRightPanel.TabStop = false;
					m_BottomRightPanel.BackColor = Color.FromKnownColor(KnownColor.Control);
#else
					m_BottomRightPanel.BackColor = SourceLibrary.Drawing.KnownColor.Control;
#endif
					Controls.Add(m_BottomRightPanel);
				}
				m_BottomRightPanel.Location = new Point(m_HScroll.Right,m_VScroll.Bottom);
				m_BottomRightPanel.Size = new Size(m_VScroll.Width,m_HScroll.Height);
				m_BottomRightPanel.BringToFront();
			}

			//forzo un ridisegno
			InvalidateScrollableArea();
		}

		#endregion

		#region Maximum/Minimum Scroll Position
		/// <summary>
		/// Return the maximum position that can be scrolled
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public virtual int MaximumVScroll
		{
			get
			{
				if (m_VScroll == null)
					return 0;
				else
					return m_VScroll.Maximum-m_VScroll.LargeChange;
			}
		}
		/// <summary>
		/// Return the minimum position that can be scrolled
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public virtual int MinimumVScroll
		{
			get
			{
				return 0;
			}
		}
		/// <summary>
		/// Return the minimum position that can be scrolled
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public virtual int MinimumHScroll
		{
			get
			{
				return 0;
			}
		}
		/// <summary>
		/// Return the maximum position that can be scrolled
		/// </summary>
		[Browsable(false),DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public virtual int MaximumHScroll
		{
			get
			{
				if (m_HScroll == null)
					return 0;
				else
					return m_HScroll.Maximum-m_HScroll.LargeChange;
			}
		}

		#endregion

#if !MINI
		#region Layout Events
		/// <summary>
		/// OnLayout Method
		/// </summary>
		/// <param name="e"></param>
		protected override void OnLayout(LayoutEventArgs e)
		{
			base.OnLayout(e);

			//nel caso MINI questo viene spostato nel OnResize
			RecalcCustomScrollBars();
		}
		#endregion
#else
		protected override void OnResize(EventArgs e)
		{
			base.OnResize (e);

			//nel caso di non MINI questo viene spostato nel OnLayout
			RecalcCustomScrollBars();
		}
#endif

		#region ScrollChangeEvent
		private int m_OldVScrollValue = 0;
		private void VScroll_Change(object sender, EventArgs e)
		{
			OnVScrollPositionChanged(new ScrollPositionChangedEventArgs(-m_VScroll.Value,-m_OldVScrollValue));

			InvalidateScrollableArea();
			m_OldVScrollValue = m_VScroll.Value;
		}
		private int m_OldHScrollValue = 0;
		private void HScroll_Change(object sender, EventArgs e)
		{
			OnHScrollPositionChanged(new ScrollPositionChangedEventArgs(-m_HScroll.Value,-m_OldHScrollValue));

			InvalidateScrollableArea();
			m_OldHScrollValue = m_HScroll.Value;
		}


		/// <summary>
		/// Fired when the scroll vertical posizion change
		/// </summary>
		public event ScrollPositionChangedEventHandler VScrollPositionChanged;
		/// <summary>
		/// Fired when the scroll vertical posizion change
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnVScrollPositionChanged(ScrollPositionChangedEventArgs e)
		{
			if (VScrollPositionChanged!=null)
				VScrollPositionChanged(this,e);
		}

		/// <summary>
		/// Fired when the scroll horizontal posizion change
		/// </summary>
		public event ScrollPositionChangedEventHandler HScrollPositionChanged;
		/// <summary>
		/// Fired when the scroll horizontal posizion change
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnHScrollPositionChanged(ScrollPositionChangedEventArgs e)
		{
			if (HScrollPositionChanged!=null)
				HScrollPositionChanged(this,e);
		}


		#endregion

		#region Scroll PageDown/Up/Right/Left/LineUp/Down/Right/Left
		/// <summary>
		/// Scroll the page down
		/// </summary>
		public virtual void CustomScrollPageDown()
		{
			if (m_VScroll!=null)
				m_VScroll.Value = Math.Min(m_VScroll.Value + m_VScroll.LargeChange, m_VScroll.Maximum-m_VScroll.LargeChange);
		}
		/// <summary>
		/// Scroll the page up
		/// </summary>
		public virtual void CustomScrollPageUp()
		{
			if (m_VScroll!=null)
				m_VScroll.Value = Math.Max(m_VScroll.Value - m_VScroll.LargeChange, m_VScroll.Minimum);
		}
		/// <summary>
		/// Scroll the page right
		/// </summary>
		public virtual void CustomScrollPageRight()
		{
			if (m_HScroll!=null)
				m_HScroll.Value = Math.Min(m_HScroll.Value + m_HScroll.LargeChange, m_HScroll.Maximum-m_HScroll.LargeChange);
		}
		/// <summary>
		/// Scroll the page left
		/// </summary>
		public virtual void CustomScrollPageLeft()
		{
			if (m_HScroll!=null)
				m_HScroll.Value = Math.Max(m_HScroll.Value - m_HScroll.LargeChange, m_HScroll.Minimum);
		}



		/// <summary>
		/// Scroll the page down one line
		/// </summary>
		public virtual void CustomScrollLineDown()
		{
			if (m_VScroll!=null)
				m_VScroll.Value = Math.Min(m_VScroll.Value + m_VScroll.SmallChange, m_VScroll.Maximum);
		}
		/// <summary>
		/// Scroll the page up one line
		/// </summary>
		public virtual void CustomScrollLineUp()
		{
			if (m_VScroll!=null)
				m_VScroll.Value = Math.Max(m_VScroll.Value - m_VScroll.SmallChange, m_VScroll.Minimum);
		}
		/// <summary>
		/// Scroll the page right one line
		/// </summary>
		public virtual void CustomScrollLineRight()
		{
			if (m_HScroll!=null)
				m_HScroll.Value = Math.Min(m_HScroll.Value + m_HScroll.SmallChange, m_HScroll.Maximum);
		}
		/// <summary>
		/// Scroll the page left one line
		/// </summary>
		public virtual void CustomScrollLineLeft()
		{
			if (m_HScroll!=null)
				m_HScroll.Value = Math.Max(m_HScroll.Value - m_HScroll.SmallChange, m_HScroll.Minimum);
		}

		#endregion
	}
}
