using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using SourceGrid2.Cells;

namespace SourceGrid2.VisualModels
{
	/// <summary>
	/// Class to manage the visual aspect of a cell. This class can be shared beetween multiple cells.
	/// </summary>
	[Serializable]
	public class Common : VisualModelBase
	{
		/// <summary>
		/// Represents a default Model
		/// </summary>
		public readonly static Common Default = new Common(true);
		/// <summary>
		/// Represents a model with a link style font and forecolor.
		/// </summary>
		public readonly static Common LinkStyle;

		static Common()
		{
			LinkStyle = new Common(false);
			LinkStyle.Font = new Font(FontFamily.GenericSerif,10,FontStyle.Underline);
			LinkStyle.ForeColor = Color.Blue;
			//LinkStyle.Cursor = Cursors.Hand;
			LinkStyle.m_bIsReadOnly = true;
		}

		#region Constructors

		/// <summary>
		/// Use default setting and construct a read and write VisualProperties
		/// </summary>
		public Common():this(false)
		{
		}

		/// <summary>
		/// Use default setting
		/// </summary>
		/// <param name="p_bReadOnly"></param>
		public Common(bool p_bReadOnly):base(p_bReadOnly)
		{
			m_ImageAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleLeft;

			m_Image = null;
			m_imgStretch = false;
			m_AlignTextToImage = true;
		}

		/// <summary>
		/// Copy constructor.  This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_Source"></param>
		/// <param name="p_bReadOnly"></param>
		public Common(Common p_Source, bool p_bReadOnly):base(p_Source, p_bReadOnly)
		{
			//anche se tecnicamente l'oggetto Image sarebbe da clonare ho scelto per risparmiare sulle risorse e per compatibilità con il compact framework (in cui non si poteva serializzare) di non clonare l'immagine
//			//Duplicate the reference fields
//			Image l_tmpImage = null;
//			if (p_Source.m_Image!=null)
//				l_tmpImage = Utility.ImageClone(p_Source.m_Image);

			//m_Image = l_tmpImage;
			m_Image = p_Source.m_Image;
			m_ImageAlignment = p_Source.m_ImageAlignment;
			m_imgStretch = p_Source.ImageStretch;
			m_AlignTextToImage = p_Source.m_AlignTextToImage;
		}
		#endregion

		#region Format
		private Image m_Image = null;

		/// <summary>
		/// Image of the cell
		/// </summary>
		public Image Image
		{
			get{return m_Image;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_Image = value;
				OnChange();
			}
		}
		private bool m_imgStretch = false;
		/// <summary>
		/// True to stretch the image otherwise false
		/// </summary>
		public bool ImageStretch
		{
			get{return m_imgStretch;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_imgStretch = value;
				OnChange();
			}
		}

		private bool m_AlignTextToImage = true;
		/// <summary>
		/// True to align the text with the image.
		/// </summary>
		public bool AlignTextToImage
		{
			get{return m_AlignTextToImage;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_AlignTextToImage = value;
				OnChange();
			}
		}

		private SourceLibrary.Drawing.ContentAlignment m_ImageAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleLeft;
		/// <summary>
		/// Image Alignment
		/// </summary>
		public SourceLibrary.Drawing.ContentAlignment ImageAlignment
		{
			get{return m_ImageAlignment;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_ImageAlignment = value;
				OnChange();
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
			return new Common(this, p_bReadOnly);
		}
		#endregion

		#region Measure (GetRequiredSize)

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
			return Utility.MeasureStringAndImage(p_Graphics, p_Cell.GetDisplayText(p_CellPosition), StringFormat, GetCellFont(), m_Image, m_ImageAlignment, m_AlignTextToImage, m_imgStretch, Border, maxLayoutArea);
		}

		#endregion

		#region DrawCell
		/// <summary>
		/// Draw the background of the specified cell. Background
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected override void DrawCell_Background(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle)
		{
#if !MINI
			using (SolidBrush br = new SolidBrush(BackColor))
			{
				e.Graphics.FillRectangle(br,p_ClientRectangle);
			}
#else
			SolidBrush br = new SolidBrush(BackColor);
			e.Graphics.FillRectangle(br,p_ClientRectangle);
#endif
		}

		/// <summary>
		/// Draw the borders of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected override void DrawCell_Border(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle)
		{
			GridPaint.DrawBorder(e.Graphics, p_ClientRectangle, Border);
		}

		/// <summary>
		/// Draw the image and the displaystring of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected override void DrawCell_ImageAndText(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle)
		{
			RectangleBorder l_Border = Border;
			Color l_ForeColor = ForeColor;

			Font l_CurrentFont = GetCellFont();

			//Image and Text
			GridPaint.DrawImageAndText(e.Graphics,
				p_ClientRectangle,
				Image,
				ImageAlignment, 
				ImageStretch, 
				p_Cell.GetDisplayText(p_CellPosition),
				StringFormat,
				AlignTextToImage,
				l_Border,
				l_ForeColor, 
				l_CurrentFont);
		}
		#endregion

#if !MINI
		#region HTML Export
		/// <summary>
		/// Write the attributes of the tag specified
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		/// <param name="p_ElementTagName"></param>
		protected override void ExportHTML_Attributes(Cells.ICellVirtual p_Cell,Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer, string p_ElementTagName)
		{
			base.ExportHTML_Attributes(p_Cell, p_Position, p_Export, p_Writer, p_ElementTagName);
			if (p_ElementTagName == "img")
			{
				p_Writer.WriteAttributeString("align", Utility.ContentToHorizontalAlignment(ImageAlignment).ToString().ToLower());
				p_Writer.WriteAttributeString("src", p_Export.ExportImage(Image));
			}		
		}

		/// <summary>
		/// Write the content of the tag specified
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		/// <param name="p_ElementTagName"></param>
		protected override void ExportHTML_Element(Cells.ICellVirtual p_Cell,Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer, string p_ElementTagName)
		{
			base.ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, p_ElementTagName);
			if (p_ElementTagName == "td")
			{
				#region Image
				//non esporto le immagini di ordinamento
				if (Image != null && CanExportHTMLImage(Image) )
				{
					p_Writer.WriteStartElement("img");

					ExportHTML_Attributes(p_Cell, p_Position, p_Export, p_Writer, "img");
					ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, "img");

					//img
					p_Writer.WriteEndElement();
				}
				#endregion
			}
			else if (p_ElementTagName == "img")
			{
			}
		}
		/// <summary>
		/// Returns true if the specified image can be exported for HTML, otherwise false. Override this method to prevent exporting certains images.
		/// </summary>
		/// <param name="p_Image"></param>
		/// <returns></returns>
		protected virtual bool CanExportHTMLImage(Image p_Image)
		{
			return true;
		}
		#endregion
#endif
	}


}
