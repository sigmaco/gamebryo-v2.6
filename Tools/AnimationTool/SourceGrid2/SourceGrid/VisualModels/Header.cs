using System;
using System.Drawing;
using System.Windows.Forms;
using SourceGrid2.Cells;

namespace SourceGrid2.VisualModels
{
	/// <summary>
	/// Summary description for a 3D Header.
	/// </summary>
	[Serializable]
	public class Header : FlatHeader
	{
		/// <summary>
		/// Represents a default Header, with a 3D border and a LightGray BackColor
		/// </summary>
		public new readonly static Header Default;
		/// <summary>
		/// Represents a Column Header with the ability to draw an Image in the right to indicates the sort operation. You must use this model with a cell of type ICellSortableHeader.
		/// </summary>
		public new readonly static Header ColumnHeader;
		/// <summary>
		/// Represents a Row Header.
		/// </summary>
		public new readonly static Header RowHeader;

		#region Constructors

		static Header()
		{
			Default = new Header(true);

			ColumnHeader = new Header(true);

			RowHeader = new Header(false);
			RowHeader.TextAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleCenter;
			RowHeader.MakeReadOnly();
		}

		/// <summary>
		/// Use default setting and construct a read and write VisualProperties
		/// </summary>
		public Header():this(false)
		{
		}

		/// <summary>
		/// Use default setting
		/// </summary>
		/// <param name="p_bReadOnly"></param>
		public Header(bool p_bReadOnly)
		{
#if !MINI
			m_HeaderShadowColor = Color.FromKnownColor(KnownColor.ControlDark);
#else
			m_HeaderShadowColor = SourceLibrary.Drawing.KnownColor.Control;
#endif

			m_HeaderLightColor = Color.White; //Color.FromKnownColor(KnownColor.ControlLight);
			m_HeaderTopLeftWidth = 4;
			m_HeaderBottomRightWidth = 3;

			SyncBorders();
			m_bIsReadOnly = p_bReadOnly;
		}

		/// <summary>
		/// Copy constructor.  This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_Source"></param>
		/// <param name="p_bReadOnly"></param>
		public Header(Header p_Source, bool p_bReadOnly):base(p_Source, p_bReadOnly)
		{
			m_HeaderTopLeftWidth = p_Source.m_HeaderTopLeftWidth;
			m_HeaderBottomRightWidth = p_Source.m_HeaderBottomRightWidth;
			m_HeaderLightColor = p_Source.m_HeaderLightColor;
			m_HeaderShadowColor = p_Source.m_HeaderShadowColor;
			//SyncBorders(); non serve perchè p_Source dovrebbe essere già sincronizzato
		}
		#endregion

		private Color m_HeaderShadowColor;
		private Color m_HeaderLightColor;
		private int m_HeaderTopLeftWidth;
		private int m_HeaderBottomRightWidth;

		/// <summary>
		/// Specifies the dark color of this cell for 3D effects (BorderStyle)
		/// </summary>
		public Color HeaderShadowColor
		{
			get{return m_HeaderShadowColor;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");

				m_HeaderShadowColor = value;
				SyncBorders();
				OnChange();
			}
		}

		/// <summary>
		/// Specifies the light color of this cell for 3D effects (BorderStyle)
		/// </summary>
		public Color HeaderLightColor
		{
			get{return m_HeaderLightColor;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");

				m_HeaderLightColor = value;
				SyncBorders();

				OnChange();
			}
		}

		/// <summary>
		/// Specified the width of the border for 3D effects (BorderStyle)
		/// </summary>
		public int HeaderLightBorderWidth
		{
			get{return m_HeaderTopLeftWidth;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");

				m_HeaderTopLeftWidth = value;
				SyncBorders();

				OnChange();
			}
		}

		/// <summary>
		/// Specified the width of the border for 3D effects (BorderStyle)
		/// </summary>
		public int HeaderShadowBorderWidth
		{
			get{return m_HeaderBottomRightWidth;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");

				m_HeaderBottomRightWidth = value;
				SyncBorders();

				OnChange();
			}
		}

		private void SyncBorders()
		{
			Border = new RectangleBorder(new Border(HeaderLightColor, HeaderLightBorderWidth),
										new Border(HeaderShadowColor, HeaderShadowBorderWidth),
										new Border(HeaderLightColor, HeaderLightBorderWidth),
										new Border(HeaderShadowColor, HeaderShadowBorderWidth));
		}

		#region Clone
		/// <summary>
		/// Clone this object. This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_bReadOnly">True if the new object must be read only, otherwise false.</param>
		/// <returns></returns>
		public override object Clone(bool p_bReadOnly)
		{
			return new Header(this, p_bReadOnly);
		}
		#endregion

		/// <summary>
		/// Draw the borders of the specified cell using DrawGradient3DBorder
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected override void DrawCell_Border(SourceGrid2.Cells.ICellVirtual p_Cell, Position p_CellPosition, PaintEventArgs e, Rectangle p_ClientRectangle)
		{
			if (p_CellPosition == p_Cell.Grid.MouseDownPosition)
				SourceLibrary.Drawing.ControlPaint.DrawGradient3DBorder(e.Graphics, p_ClientRectangle, BackColor, HeaderShadowColor, HeaderLightColor, HeaderShadowBorderWidth, HeaderLightBorderWidth, SourceLibrary.Drawing.Gradient3DBorderStyle.Sunken);
			else
				SourceLibrary.Drawing.ControlPaint.DrawGradient3DBorder(e.Graphics, p_ClientRectangle, BackColor, HeaderShadowColor, HeaderLightColor, HeaderShadowBorderWidth, HeaderLightBorderWidth, SourceLibrary.Drawing.Gradient3DBorderStyle.Raised);
		}
	}
}
