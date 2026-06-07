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
	public abstract class VisualModelBase : IVisualModel
	{
		#region Constructors

		/// <summary>
		/// Use default setting and construct a read and write VisualProperties
		/// </summary>
		public VisualModelBase():this(false)
		{
		}

		/// <summary>
		/// Use default setting
		/// </summary>
		/// <param name="p_bReadOnly"></param>
		public VisualModelBase(bool p_bReadOnly)
		{
#if !MINI
			m_BackColor = Color.FromKnownColor(KnownColor.Window);
			m_ForeColor = Color.FromKnownColor(KnownColor.WindowText);
#else
			m_BackColor = Color.White;
			m_ForeColor = Color.Black;
#endif
			//change the default text alignment
			TextAlignment = SourceLibrary.Drawing.ContentAlignment.MiddleLeft;

			m_Font = null; // new Font(FontFamily.GenericSansSerif,8.25f); if null the grid font is used

			m_StringFormat = (StringFormat)StringFormat.GenericDefault.Clone();
			WordWrap = false;

			//Border
			m_Border = RectangleBorder.Default;

			m_bIsReadOnly = p_bReadOnly;
		}

		/// <summary>
		/// Copy constructor.  This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_Source"></param>
		/// <param name="p_bReadOnly"></param>
		public VisualModelBase(VisualModelBase p_Source, bool p_bReadOnly)
		{
			//Duplicate the reference fields
			Font l_tmpFont = null;
			if (p_Source.m_Font!=null)
#if !MINI
				l_tmpFont = (Font)p_Source.m_Font.Clone();
#else
				l_tmpFont = new Font(p_Source.m_Font.Name, p_Source.m_Font.Size, p_Source.m_Font.Style);
#endif
			StringFormat l_tmpStringFormat = null;
			if (p_Source.m_StringFormat!=null)
				l_tmpStringFormat = (StringFormat)p_Source.m_StringFormat.Clone();

			m_bIsReadOnly = p_bReadOnly;
			m_BackColor = p_Source.m_BackColor;
			m_ForeColor = p_Source.m_ForeColor;
			m_Font = l_tmpFont;
			m_StringFormat = l_tmpStringFormat;
			m_Border = p_Source.m_Border;
		}
		#endregion

		#region ReadOnly
		/// <summary>
		/// ReadOnly variable.
		/// </summary>
		protected bool m_bIsReadOnly = false;
		/// <summary>
		/// True if this class is ReadOnly otherwise False.
		/// </summary>
		public bool IsReadOnly
		{
			get{return m_bIsReadOnly;}
		}

		/// <summary>
		/// Make the current instance readonly. Use this method to prevent unexpected changes.
		/// </summary>
		public void MakeReadOnly()
		{
			m_bIsReadOnly = true;
		}

        public void MakeReadWrite()
        {
            m_bIsReadOnly = false;
		}

		#endregion

		#region Events
		/// <summary>
		/// Fired when you change a property of this class
		/// </summary>
		public event EventHandler Change;

		/// <summary>
		/// Fired when you change a property of this class
		/// </summary>
		protected virtual void OnChange()
		{
			if (Change!=null)
				Change(this,EventArgs.Empty);
		}
		#endregion

		#region Format
		private StringFormat m_StringFormat = StringFormat.GenericDefault;

		/// <summary>
		/// StringFormat of the cell
		/// </summary>
		[System.ComponentModel.Browsable(false)]
		public StringFormat StringFormat
		{
			get{return m_StringFormat;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_StringFormat = value;
				OnChange();
			}
		}

		private Font m_Font;

		/// <summary>
		/// If null the grid font is used
		/// </summary>
		public Font Font
		{
			get{return m_Font;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_Font = value;
				OnChange();
			}
		}
		#region BackColor/ForeColor
		private Color m_BackColor; 

		/// <summary>
		/// BackColor of the cell
		/// </summary>
		public Color BackColor
		{
			get{return m_BackColor;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_BackColor = value;
				OnChange();
			}
		}
		private Color m_ForeColor; 

		/// <summary>
		/// ForeColor of the cell
		/// </summary>
		public Color ForeColor
		{
			get{return m_ForeColor;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_ForeColor = value;
				OnChange();
			}
		}


		#endregion

		#region Border

		private RectangleBorder m_Border;
		/// <summary>
		/// The normal border of a cell
		/// </summary>
		public RectangleBorder Border
		{
			get{return m_Border;}
			set
			{
				if (m_bIsReadOnly)
					throw new ObjectIsReadOnlyException("VisualProperties is readonly.");
				m_Border = value;
				OnChange();
			}
		}
		#endregion
		#region Format Properties Wrapper
		/// <summary>
		/// Word Wrap.  This property is only a wrapper around StringFormat
		/// </summary>
		public bool WordWrap
		{
			get
			{
				if ( (StringFormat.FormatFlags & StringFormatFlags.NoWrap) == StringFormatFlags.NoWrap)
					return false;
				else
					return true;
			}
			set
			{
				if (value && WordWrap == false)
					StringFormat.FormatFlags = StringFormat.FormatFlags ^ StringFormatFlags.NoWrap;
				else if (value == false)
					StringFormat.FormatFlags = StringFormat.FormatFlags | StringFormatFlags.NoWrap;
			}
		}

		/// <summary>
		/// Text Alignment. This property is only a wrapper around StringFormat
		/// </summary>
		public SourceLibrary.Drawing.ContentAlignment TextAlignment
		{
			get
			{
				return Utility.StringFormatoToContentAlignment(StringFormat);
			}

			set
			{
				if (Utility.IsBottom(value))
					StringFormat.LineAlignment = StringAlignment.Far;
				else if (Utility.IsTop(value))
					StringFormat.LineAlignment = StringAlignment.Near;
				else //if (Utility.IsMiddle(value))
					StringFormat.LineAlignment = StringAlignment.Center;

				if (Utility.IsLeft(value))
					StringFormat.Alignment = StringAlignment.Near;
				else if (Utility.IsRight(value))
					StringFormat.Alignment = StringAlignment.Far;
				else //if (Utility.IsCenter(value))
					StringFormat.Alignment = StringAlignment.Center;
			}
		}

		/// <summary>
		/// Get the font of the cell, check if the current font is null and in this case return the grid font
		/// </summary>
		/// <returns></returns>
		public virtual Font GetCellFont()
		{
			if (Font==null)
#if !MINI
				return Control.DefaultFont;
#else
				return new Font(FontFamily.GenericSansSerif, 8.25f, FontStyle.Regular);
#endif
			else
				return Font;
		}
		#endregion

		#endregion

		#region Clone
		/// <summary>
		/// Clone this object. Also ReadOnly flag is copied.
		/// </summary>
		/// <returns></returns>
		public object Clone()
		{
			return Clone(m_bIsReadOnly);
		}

		/// <summary>
		/// Clone this object. This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_bReadOnly">True if the new object must be read only, otherwise false.</param>
		/// <returns></returns>
		public abstract object Clone(bool p_bReadOnly);
		#endregion

		#region Measure
		/// <summary>
		/// Returns the minimum required size of the current cell, calculating using the current DisplayString, Image and Borders informations.
		/// </summary>
		/// <param name="p_Graphics"></param>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		public virtual SizeF Measure(Graphics p_Graphics,
			Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			SizeF maxLayoutArea)
		{
			return Utility.MeasureStringAndImage(p_Graphics, p_Cell.GetDisplayText(p_CellPosition), m_StringFormat, GetCellFont(), null, SourceLibrary.Drawing.ContentAlignment.MiddleCenter, false, false, Border, maxLayoutArea);
		}

		#endregion

		#region DrawCell
		/// <summary>
		/// Draw the cell specified
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		public virtual void DrawCell(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle)
		{
			GridVirtual l_Grid = p_Cell.Grid;

			if ( p_Cell.DataModel == null || p_Cell.DataModel.EnableCellDrawOnEdit || p_Cell.IsEditing(p_CellPosition) == false )
			{
				Graphics g = e.Graphics;

				//Set the clip region with the cell rectangle (for a bug in the PaintBorders function)
				//Region l_PreviousClip = g.Clip;
				//g.Clip = new Region(p_ClientRectangle);

				DrawCell_Background(p_Cell, p_CellPosition, e, p_ClientRectangle);

				DrawCell_Border(p_Cell, p_CellPosition, e, p_ClientRectangle);

				DrawCell_ImageAndText(p_Cell, p_CellPosition, e, p_ClientRectangle);

				//Reset the clip region
				//g.Clip = l_PreviousClip;
			}

		}

		/// <summary>
		/// Draw the background of the specified cell. Background
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected abstract void DrawCell_Background(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle);

		/// <summary>
		/// Draw the borders of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected abstract void DrawCell_Border(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle);

		/// <summary>
		/// Draw the image and the displaystring of the specified cell.
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		protected abstract void DrawCell_ImageAndText(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			PaintEventArgs e, 
			Rectangle p_ClientRectangle);
		#endregion

#if !MINI
		#region HTML Export
		/// <summary>
		/// Export the cell contents in html format
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		public virtual void ExportHTML(Cells.ICellVirtual p_Cell, Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer)
		{
			//start element td
			p_Writer.WriteStartElement("td");

			//cell attributes
			ExportHTML_Attributes(p_Cell, p_Position, p_Export, p_Writer, "td");
			
			//cell content
			ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, "td");

			//td
			p_Writer.WriteEndElement();
		}

		/// <summary>
		/// Write the attributes of the tag specified
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		/// <param name="p_ElementTagName"></param>
		protected virtual void ExportHTML_Attributes(Cells.ICellVirtual p_Cell,Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer, string p_ElementTagName)
		{
			if (p_ElementTagName == "td")
			{
				//check for rowspan and colspan 
				if (p_Cell is ICell)
				{
					//TODO non è bellissimo mettere questo if (pensare come poter fare in un altro modo)
					ICell l_Cell = (ICell)p_Cell;
					//colspan, rowspan
					p_Writer.WriteAttributeString("colspan", l_Cell.ColumnSpan.ToString() );
					p_Writer.WriteAttributeString("rowspan", l_Cell.RowSpan.ToString() );
				}

				//write backcolor
				if ( (p_Export.Mode & ExportHTMLMode.CellBackColor) == ExportHTMLMode.CellBackColor)
				{
					p_Writer.WriteAttributeString("bgcolor", HTMLExport.ColorToHTML(BackColor) );
				}

				string l_Style = "";

				//border
				l_Style = HTMLExport.CellBorderToHTMLStyle(Border);

				//style
				p_Writer.WriteAttributeString("style", l_Style);

				//alignment
				p_Writer.WriteAttributeString("align", Utility.ContentToHorizontalAlignment(TextAlignment).ToString().ToLower());
				if (Utility.IsBottom(TextAlignment))
					p_Writer.WriteAttributeString("valign", "bottom");
				else if (Utility.IsTop(TextAlignment))
					p_Writer.WriteAttributeString("valign", "top");
				else if (Utility.IsMiddle(TextAlignment))
					p_Writer.WriteAttributeString("valign", "middle");
			}
			else if (p_ElementTagName == "font")
			{
				p_Writer.WriteAttributeString("color", HTMLExport.ColorToHTML(ForeColor));
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
		protected virtual void ExportHTML_Element(Cells.ICellVirtual p_Cell,Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer, string p_ElementTagName)
		{
			if (p_ElementTagName == "td")
			{
				#region Font
				p_Writer.WriteStartElement("font");

				ExportHTML_Attributes(p_Cell, p_Position, p_Export, p_Writer, "font");
				ExportHTML_Element(p_Cell, p_Position, p_Export, p_Writer, "font");

				//font
				p_Writer.WriteEndElement();
				#endregion
			}
			else if (p_ElementTagName == "font")
			{
				Utility.ExportHTML_Element_Font(p_Writer, p_Cell.GetDisplayText(p_Position), GetCellFont());
			}
		}
		#endregion
#endif
	}
}
