using System;
using System.Drawing;

namespace SourceGrid2.VisualModels
{
	/// <summary>
	/// A interface that represents the visual aspect of a cell. Contains the Draw method and the common properties
	/// </summary>
	public interface IVisualModel : ICloneable
	{
		#region Format
		/// <summary>
		/// If null the default font is used
		/// </summary>
		Font Font
		{
			get;
			set;
		}

		/// <summary>
		/// BackColor of the cell
		/// </summary>
		Color BackColor
		{
			get;
			set;
		}

		/// <summary>
		/// ForeColor of the cell
		/// </summary>
		Color ForeColor
		{
			get;
			set;
		}

		/// <summary>
		/// The normal border of a cell
		/// </summary>
		RectangleBorder Border
		{
			get;
			set;
		}

		/// <summary>
		/// Word Wrap.  This property is only a wrapper around StringFormat
		/// </summary>
		bool WordWrap
		{
			get;
			set;
		}

		/// <summary>
		/// Text Alignment. This property is only a wrapper around StringFormat
		/// </summary>
		SourceLibrary.Drawing.ContentAlignment TextAlignment
		{
			get;
			set;
		}
		#endregion


		/// <summary>
		/// Draw the cell specified
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="e">Paint arguments</param>
		/// <param name="p_ClientRectangle">Rectangle position where draw the current cell, relative to the current view,</param>
		void DrawCell(Cells.ICellVirtual p_Cell,
			Position p_CellPosition,
			System.Windows.Forms.PaintEventArgs e, 
			System.Drawing.Rectangle p_ClientRectangle);


		/// <summary>
		/// Returns the minimum required size of the current cell, calculating using the current DisplayString, Image and Borders informations.
		/// </summary>
		/// <param name="p_Graphics"></param>
		/// <param name="p_Cell"></param>
		/// <param name="p_CellPosition"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		SizeF Measure(Graphics p_Graphics,
									Cells.ICellVirtual p_Cell,
									Position p_CellPosition,
									SizeF maxLayoutArea);

#if !MINI
		/// <summary>
		/// Export the cell contents in html format
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		/// <param name="p_Export"></param>
		/// <param name="p_Writer"></param>
		void ExportHTML(Cells.ICellVirtual p_Cell,Position p_Position, IHTMLExport p_Export, System.Xml.XmlTextWriter p_Writer);
#endif

		/// <summary>
		/// Clone this object. This method duplicate all the reference field (Image, Font, StringFormat) creating a new instance.
		/// </summary>
		/// <param name="p_bReadOnly">True if the new object must be read only, otherwise false.</param>
		/// <returns></returns>
		object Clone(bool p_bReadOnly);


		/// <summary>
		/// True if this class is ReadOnly otherwise False.
		/// </summary>
		bool IsReadOnly
		{
			get;
		}

		/// <summary>
		/// Make the current instance readonly. Use this method to prevent unexpected changes.
		/// </summary>
		void MakeReadOnly();
        void MakeReadWrite();
	}
}
