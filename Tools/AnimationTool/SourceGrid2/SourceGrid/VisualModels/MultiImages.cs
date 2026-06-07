using System;
using System.Drawing;
using System.Windows.Forms;
using SourceGrid2.Cells;

namespace SourceGrid2.VisualModels
{
	/// <summary>
	/// Summary description for VisualModelCheckBox.
	/// </summary>
	[Serializable]
	public class MultiImages : Common
	{
		#region Constructors

		/// <summary>
		/// Use default setting and construct a read and write VisualProperties
		/// </summary>
		public MultiImages():this(false)
		{
		}

		/// <summary>
		/// Use default setting
		/// </summary>
		/// <param name="p_bReadOnly"></param>
		public MultiImages(bool p_bReadOnly):base(p_bReadOnly)
		{
		}

		/// <summary>
		/// Copy constructor.  This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_Source"></param>
		/// <param name="p_bReadOnly"></param>
		public MultiImages(MultiImages p_Source, bool p_bReadOnly):base(p_Source, p_bReadOnly)
		{
			m_Images = (PositionedImageCollection)p_Source.m_Images.Clone();
		}
		#endregion

		private PositionedImageCollection m_Images = new PositionedImageCollection();
		/// <summary>
		/// Images of the cells
		/// </summary>
		public PositionedImageCollection SubImages
		{
			//TODO questa reference anche nel caso ReadOnly = true potrebbe essere modificata, come posso fare? applicare il readonly anche alla collection
			get{return m_Images;}
		}

		#region DrawCell
		/// <summary>
		/// Draw the image and the displaystring of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected override void DrawCell_ImageAndText(Cells.ICellVirtual p_Cell, Position p_CellPosition, System.Windows.Forms.PaintEventArgs e, System.Drawing.Rectangle p_ClientRectangle)
		{
			base.DrawCell_ImageAndText (p_Cell, p_CellPosition, e, p_ClientRectangle);

			RectangleBorder l_Border = Border;
			Color l_ForeColor = ForeColor;

			for (int i = 0; i < m_Images.Count; i++)
			{
				if (m_Images[i]!=null)
				{
					GridPaint.DrawImageAndText(e.Graphics,
						p_ClientRectangle,
						m_Images[i].Image,
						m_Images[i].Alignment,
						false, 
						null, //not used
						null, //not used
						false, //not used
						l_Border,
						Color.Black, //not used
						null); //not used
				}
			}
		}
		#endregion

		#region Clone
		/// <summary>
		/// Clone this object. This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_bReadOnly">True if the new object must be read only, otherwise false.</param>
		/// <returns></returns>
		public override object Clone(bool p_bReadOnly)
		{
			return new MultiImages(this, p_bReadOnly);
		}
		#endregion
	}
}
