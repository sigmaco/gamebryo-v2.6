using System;
using System.Drawing;
using System.Windows.Forms;
using SourceGrid2.Cells;

namespace SourceGrid2.VisualModels
{
	/// <summary>
	/// A VisualModel for a header. Draw a down or up arrow when sorted.
	/// </summary>
	[Serializable]
	public class FlatHeader : Common
	{
		/// <summary>
		/// Represents a default Header
		/// </summary>
		public new readonly static FlatHeader Default;
		/// <summary>
		/// Represents a Column Header with the ability to draw an Image in the right to indicates the sort operation. You must use this model with a cell of type ICellSortableHeader.
		/// </summary>
		public readonly static FlatHeader ColumnHeader;
		/// <summary>
		/// Represents a Row Header.
		/// </summary>
		public readonly static FlatHeader RowHeader;

		#region Constructors

		static FlatHeader()
		{
			Default = new FlatHeader(true);

			ColumnHeader = new FlatHeader(true);

			RowHeader = new FlatHeader(false);
			RowHeader.TextAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleCenter;
			RowHeader.MakeReadOnly();
		}

		/// <summary>
		/// Use default setting and construct a read and write VisualProperties
		/// </summary>
		public FlatHeader():this(false)
		{
		}

		/// <summary>
		/// Use default setting
		/// </summary>
		/// <param name="p_bReadOnly"></param>
		public FlatHeader(bool p_bReadOnly)
		{
#if !MINI
			BackColor = Color.FromKnownColor(KnownColor.Control);
#else
			BackColor = SourceLibrary.Drawing.KnownColor.Control;
#endif
			Border = new RectangleBorder(new Border(Color.Black), new Border(Color.Black));

			m_bIsReadOnly = p_bReadOnly;
		}

		/// <summary>
		/// Copy constructor.  This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_Source"></param>
		/// <param name="p_bReadOnly"></param>
		public FlatHeader(FlatHeader p_Source, bool p_bReadOnly):base(p_Source, p_bReadOnly)
		{
		}
		#endregion

		/// <summary>
		/// Returns the minimum required size of the current cell, calculating using the current DisplayString, Image and Borders informations.
		/// </summary>
		/// <param name="p_Graphics"></param>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		public override SizeF Measure(Graphics p_Graphics,
			Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			SizeF maxLayoutArea)
		{
			SizeF s = base.Measure(p_Graphics, p_Cell, p_CellPosition, maxLayoutArea);
			s.Width += CommonImages.SortUp.Width; //add the width of the sort image
			return s;
		}

		#region Clone
		/// <summary>
		/// Clone this object. This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_bReadOnly">True if the new object must be read only, otherwise false.</param>
		/// <returns></returns>
		public override object Clone(bool p_bReadOnly)
		{
			return new FlatHeader(this, p_bReadOnly);
		}
		#endregion

		/// <summary>
		/// Draw the image and the displaystring of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		/// <param name="p_Status"></param>
		protected override void DrawCell_ImageAndText(SourceGrid2.Cells.ICellVirtual p_Cell, Position p_CellPosition, PaintEventArgs e, Rectangle p_ClientRectangle)
		{
			base.DrawCell_ImageAndText(p_Cell, p_CellPosition, e, p_ClientRectangle);

			if (p_Cell is ICellSortableHeader)
			{
				ICellSortableHeader l_Header = (ICellSortableHeader)p_Cell;
				SortStatus l_Status = l_Header.GetSortStatus(p_CellPosition);

				if (l_Status.Mode==GridSortMode.Ascending)
					GridPaint.DrawImageAndText(e.Graphics, p_ClientRectangle, CommonImages.SortUp, SourceLibrary.Drawing.ContentAlignment.MiddleRight, false, null, null, false, Border, Color.Black, null);
				else if (l_Status.Mode==GridSortMode.Descending)
					GridPaint.DrawImageAndText(e.Graphics, p_ClientRectangle, CommonImages.SortDown, SourceLibrary.Drawing.ContentAlignment.MiddleRight, false, null, null, false, Border, Color.Black, null);
			}
		}
	}
}
