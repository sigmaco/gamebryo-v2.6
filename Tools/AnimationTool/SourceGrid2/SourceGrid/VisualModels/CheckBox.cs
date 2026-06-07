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
	public class CheckBox : Common
	{
		/// <summary>
		/// Represents a default CheckBox with the CheckBox image align to the Middle Center of the cell. You must use this VisualModel with a Cell of type ICellCheckBox.
		/// </summary>
		public new readonly static CheckBox Default = new CheckBox(true);
		/// <summary>
		/// Represents a CheckBox with the CheckBox image align to the Middle Right of the cell
		/// </summary>
		public readonly static CheckBox MiddleLeftAlign;

		#region Constructors

		static CheckBox()
		{
			MiddleLeftAlign = new CheckBox(false);
			MiddleLeftAlign.CheckBoxAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleLeft;
			MiddleLeftAlign.TextAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleLeft;
			MiddleLeftAlign.AlignTextToImage = true;
			MiddleLeftAlign.MakeReadOnly();
		}

		/// <summary>
		/// Use default setting and construct a read and write VisualProperties
		/// </summary>
		public CheckBox():this(false)
		{
		}

		/// <summary>
		/// Use default setting
		/// </summary>
		/// <param name="p_bReadOnly"></param>
		public CheckBox(bool p_bReadOnly)
		{
			m_bIsReadOnly = p_bReadOnly;
		}

		/// <summary>
		/// Copy constructor.  This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_Source"></param>
		/// <param name="p_bReadOnly"></param>
		public CheckBox(CheckBox p_Source, bool p_bReadOnly):base(p_Source, p_bReadOnly)
		{
			m_CheckBoxAlignment = p_Source.m_CheckBoxAlignment;
		}
		#endregion

		private SourceLibrary.Drawing.ContentAlignment m_CheckBoxAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleCenter;
		/// <summary>
		/// Image Alignment
		/// </summary>
		public SourceLibrary.Drawing.ContentAlignment CheckBoxAlignment
		{
			get{return m_CheckBoxAlignment;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");

				m_CheckBoxAlignment = value;

				OnChange();
			}
		}

		#region Measure
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
			ICellCheckBox l_CheckBox = (ICellCheckBox)p_Cell;
			CheckBoxStatus l_Status = l_CheckBox.GetCheckBoxStatus(p_CellPosition);

#if !MINI
			Image l_CheckImage = GetImageForState(l_Status.Checked, p_Cell.Grid.MouseCellPosition == p_CellPosition, l_Status.CheckEnable);
#else
			Image l_CheckImage = GetImageForState(l_Status.Checked, false, l_Status.CheckEnable);
#endif

			return Utility.MeasureStringAndImage(p_Graphics, l_Status.Caption, StringFormat, GetCellFont(), l_CheckImage, m_CheckBoxAlignment, AlignTextToImage, ImageStretch, Border, maxLayoutArea);
		}

		#endregion

		#region DrawCell
		/// <summary>
		/// Returns the image for the current check state
		/// </summary>
		/// <param name="p_Checked"></param>
		/// <param name="p_bIsSelected"></param>
		/// <param name="p_bIsCheckEnable"></param>
		/// <returns></returns>
		public static System.Drawing.Image GetImageForState(bool p_Checked, bool p_bIsSelected, bool p_bIsCheckEnable)
		{
			if (p_bIsCheckEnable)
			{
				if (p_Checked)
				{
#if !MINI
					if (p_bIsSelected)
						return CommonImages.CheckBoxCheckedSel;
					else
#endif
						return CommonImages.CheckBoxChecked;
				}
				else
				{
#if !MINI
					if (p_bIsSelected)
						return CommonImages.CheckBoxUnCheckedSel;
					else
#endif
						return CommonImages.CheckBoxUnChecked;
				}
			}
			else
			{
				if (p_Checked)
				{
					return CommonImages.CheckBoxCheckedDisable;
				}
				else
				{
					return CommonImages.CheckBoxUnCheckedDisable;
				}
			}
		}

		/// <summary>
		/// Draw the image and the displaystring of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		/// <param name="p_Status"></param>
		protected override void DrawCell_ImageAndText(ICellVirtual p_Cell, Position p_CellPosition, System.Windows.Forms.PaintEventArgs e, System.Drawing.Rectangle p_ClientRectangle)
		{
			//in questo caso non chiamo la classe base perchè devo disegnare una textbox
			//base.DrawCell_ImageAndText (p_Cell, p_CellPosition, e, p_ClientRectangle, p_ForeColor, p_CellBorder);

			RectangleBorder l_Border = Border;
			Color l_ForeColor = ForeColor;

			ICellCheckBox l_CheckBox = (ICellCheckBox)p_Cell;
			CheckBoxStatus l_Status = l_CheckBox.GetCheckBoxStatus(p_CellPosition);

#if !MINI
			Image l_CheckImage = GetImageForState(l_Status.Checked, p_Cell.Grid.MouseCellPosition == p_CellPosition, l_Status.CheckEnable);
#else
			Image l_CheckImage = GetImageForState(l_Status.Checked, false, l_Status.CheckEnable);
#endif

			Font l_CurrentFont = GetCellFont();

			//Image and Text
			GridPaint.DrawImageAndText(e.Graphics,
				p_ClientRectangle,
				l_CheckImage,
				m_CheckBoxAlignment, 
				ImageStretch, 
				l_Status.Caption,
				StringFormat,
				AlignTextToImage,
				l_Border,
				l_ForeColor, 
				l_CurrentFont);
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
			return new CheckBox(this, p_bReadOnly);
		}
		#endregion

#if !MINI
		#region ExportHTML
		/// <summary>
		/// Write the content of the tag specified
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		/// <param name="p_ElementTagName"></param>
		protected override void ExportHTML_Element(Cells.ICellVirtual p_Cell, Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer, string p_ElementTagName)
		{
			ICellCheckBox l_CheckBox = (ICellCheckBox)p_Cell;
			CheckBoxStatus l_Status = l_CheckBox.GetCheckBoxStatus(p_Position);

			if (p_ElementTagName == "td")
			{
				if (l_Status.Checked == true)
					p_Writer.WriteRaw("<input type=\"checkbox\" checked>");
				else
					p_Writer.WriteRaw("<input type=\"checkbox\">");

				base.ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, p_ElementTagName);
			}
			else if (p_ElementTagName == "font")
			{
				Utility.ExportHTML_Element_Font(p_Writer, l_Status.Caption, GetCellFont());

				//do not call base ExportHTML_Element because we use the caption not the display text 
				//base.ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, p_ElementTagName);
			}
			else
				base.ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, p_ElementTagName);

		}
		#endregion
#endif

	}
}
