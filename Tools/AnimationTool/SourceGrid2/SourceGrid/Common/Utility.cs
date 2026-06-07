using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Runtime.Serialization;

namespace SourceGrid2
{
	/// <summary>
	/// Summary description for Utility.
	/// </summary>
	public class Utility
	{
		//TODO valutare quali metodi spostare nella SourceLibrary

		#region Constant
		/// <summary>
		/// Default Cell height
		/// </summary>
		public const int DefaultCellHeight = 20;
		/// <summary>
		/// Default cell width
		/// </summary>
		public const int DefaultCellWidth = 50;

		#endregion

		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_Align"></param>
		/// <param name="p_ClientLeft"></param>
		/// <param name="p_ClientTop"></param>
		/// <param name="p_ClientWidth"></param>
		/// <param name="p_ClientHeight"></param>
		/// <param name="p_ObjWidth"></param>
		/// <param name="p_ObjHeight"></param>
		/// <returns></returns>
		public static PointF CalculateObjAlignment(SourceLibrary.Drawing.ContentAlignment p_Align, int p_ClientLeft, int p_ClientTop, int p_ClientWidth, int p_ClientHeight, float p_ObjWidth, float p_ObjHeight)
		{
			//default X left
			PointF l_pointf = new PointF((float)p_ClientLeft,(float)p_ClientTop);

			//Y
			if (p_Align == SourceLibrary.Drawing.ContentAlignment.TopCenter ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.TopLeft ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.TopRight) //Y Top
				l_pointf.Y = (float)p_ClientTop;
			else if (p_Align == SourceLibrary.Drawing.ContentAlignment.BottomCenter ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.BottomLeft ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.BottomRight) //Y bottom
				l_pointf.Y = (float)p_ClientTop + ((float)p_ClientHeight) - p_ObjHeight;
			else //default Y middle
				l_pointf.Y = (float)p_ClientTop + ((float)p_ClientHeight)/2.0F - p_ObjHeight/2.0F;

			if ( p_Align == SourceLibrary.Drawing.ContentAlignment.BottomCenter ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.MiddleCenter ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.TopCenter)//X Center
				l_pointf.X = (float)p_ClientLeft + ((float)p_ClientWidth)/2.0F - p_ObjWidth/2.0F;
			else if (p_Align == SourceLibrary.Drawing.ContentAlignment.BottomRight ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.MiddleRight ||
				p_Align == SourceLibrary.Drawing.ContentAlignment.TopRight)//X Right
				l_pointf.X = (float)p_ClientLeft + (float)p_ClientWidth - p_ObjWidth;
			//middle default already set

			return l_pointf;
		}


		static public  int HiWord(int Number) 
		{ 
			return (Number >> 16) & 0xffff; 
		} 
 
		static public int LoWord(int Number) 
		{ 
			return Number & 0xffff; 
		} 

//		static public Image ImageClone(Image p_Image)
//		{
//			System.Runtime.Serialization.Formatters.Binary.BinaryFormatter l_BinForm = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
//			using (System.IO.MemoryStream l_Stream = new System.IO.MemoryStream())
//			{
//				l_BinForm.Serialize(l_Stream,p_Image);
//				l_Stream.Seek(0,System.IO.SeekOrigin.Begin);
//				return (Image)l_BinForm.Deserialize(l_Stream);
//			}
//		}

		#region ContentAlign Utility
		public static bool IsBottom(SourceLibrary.Drawing.ContentAlignment a)
		{
			return (a == SourceLibrary.Drawing.ContentAlignment.BottomCenter ||
				a == SourceLibrary.Drawing.ContentAlignment.BottomLeft ||
				a == SourceLibrary.Drawing.ContentAlignment.BottomRight);
		}
		public static bool IsTop(SourceLibrary.Drawing.ContentAlignment a)
		{
			return (a == SourceLibrary.Drawing.ContentAlignment.TopCenter ||
				a == SourceLibrary.Drawing.ContentAlignment.TopLeft ||
				a == SourceLibrary.Drawing.ContentAlignment.TopRight);
		}
		public static bool IsMiddle(SourceLibrary.Drawing.ContentAlignment a)
		{
			return (a == SourceLibrary.Drawing.ContentAlignment.MiddleCenter ||
				a == SourceLibrary.Drawing.ContentAlignment.MiddleLeft ||
				a == SourceLibrary.Drawing.ContentAlignment.MiddleRight);
		}
		public static bool IsCenter(SourceLibrary.Drawing.ContentAlignment a)
		{
			return (a == SourceLibrary.Drawing.ContentAlignment.BottomCenter ||
				a == SourceLibrary.Drawing.ContentAlignment.MiddleCenter ||
				a == SourceLibrary.Drawing.ContentAlignment.TopCenter);
		}
		public static bool IsLeft(SourceLibrary.Drawing.ContentAlignment a)
		{
			return (a == SourceLibrary.Drawing.ContentAlignment.BottomLeft ||
				a == SourceLibrary.Drawing.ContentAlignment.MiddleLeft ||
				a == SourceLibrary.Drawing.ContentAlignment.TopLeft);
		}
		public static bool IsRight(SourceLibrary.Drawing.ContentAlignment a)
		{
			return (a == SourceLibrary.Drawing.ContentAlignment.BottomRight ||
				a == SourceLibrary.Drawing.ContentAlignment.MiddleRight ||
				a == SourceLibrary.Drawing.ContentAlignment.TopRight);
		}
		public static HorizontalAlignment ContentToHorizontalAlignment(SourceLibrary.Drawing.ContentAlignment a)
		{
			if (IsLeft(a))
				return HorizontalAlignment.Left;
			else if (IsRight(a))
				return HorizontalAlignment.Right;
			else
				return HorizontalAlignment.Center;
		}

		public static bool IsBottom(StringFormat a)
		{
			return (a.LineAlignment == StringAlignment.Far);
		}
		public static bool IsTop(StringFormat a)
		{
			return (a.LineAlignment == StringAlignment.Near);
		}
		public static bool IsMiddle(StringFormat a)
		{
			return (a.LineAlignment == StringAlignment.Center);
		}
		public static bool IsCenter(StringFormat a)
		{
			return (a.Alignment == StringAlignment.Center);
		}
		public static bool IsLeft(StringFormat a)
		{
			return (a.Alignment == StringAlignment.Near);
		}
		public static bool IsRight(StringFormat a)
		{
			return (a.Alignment == StringAlignment.Far);
		}
		#endregion

		#region DataModel Utility
		/// <summary>
		/// Construct a DataModel for the specified type.
		/// If the Type support a UITypeEditor returns a EditorUITypeEditor else if the type has a StandardValues list return a EditorComboBox else if the type support string conversion returns a EditorTextBox otherwise returns null.
		/// </summary>
		/// <param name="p_Type">Cell Type</param>
		/// <returns></returns>
		public static DataModels.IDataModel CreateDataModel(Type p_Type)
		{
#if !MINI
			System.ComponentModel.TypeConverter l_TypeConverter = System.ComponentModel.TypeDescriptor.GetConverter(p_Type);
			ICollection l_StandardValues = null;
			bool l_StandardValuesExclusive = false;
			if (l_TypeConverter != null)
			{
				l_StandardValues = l_TypeConverter.GetStandardValues();
				if (l_StandardValues != null && l_StandardValues.Count > 0)
					l_StandardValuesExclusive = l_TypeConverter.GetStandardValuesExclusive();
				else
					l_StandardValuesExclusive = false;
			}
			object l_objUITypeEditor = System.ComponentModel.TypeDescriptor.GetEditor(p_Type,typeof(System.Drawing.Design.UITypeEditor));
			if (l_objUITypeEditor != null) //UITypeEditor founded
			{
				return new DataModels.EditorUITypeEditor(p_Type, (System.Drawing.Design.UITypeEditor)l_objUITypeEditor);
			}
			else
			{
				if (l_StandardValues != null) //combo box
				{
					return new DataModels.EditorComboBox(p_Type, l_StandardValues, l_StandardValuesExclusive);
				}
				else if (l_TypeConverter != null && l_TypeConverter.CanConvertFrom(typeof(string)) )//txtbox
				{
					return new DataModels.EditorTextBox(p_Type);
				}
				else //no editor found
					return null;
			}
#else
			SourceLibrary.ComponentModel.TypeConverter l_TypeConverter = SourceLibrary.ComponentModel.TypeDescriptor.GetConverter(p_Type);
			if (l_TypeConverter != null && l_TypeConverter.CanConvertFrom(typeof(string)) )//txtbox
			{
				return new DataModels.EditorTextBox(p_Type);
			}
			else //no editor found
				return null;
#endif
		}


#if MINI
		//UITypeEditor non supportato

		/// <summary>
		/// Construct a CellEditor for the specified type
		/// </summary>
		/// <param name="p_Type">Cell Type</param>
		/// <param name="p_DefaultValue">Default value of the editor</param>
		/// <param name="p_bAllowNull">Allow null</param>
		/// <param name="p_StandardValues">List of available values or null if there is no available values list</param>
		/// <param name="p_bStandardValueExclusive">Indicates if the p_StandardValue are the unique values supported</param>
		/// <param name="p_TypeConverter">Type converter used for conversion for the specified type</param>
		/// <returns></returns>
		public static DataModels.IDataModel CreateDataModel(Type p_Type, 
			object p_DefaultValue,
			bool p_bAllowNull,
			System.Collections.ICollection p_StandardValues,
			bool p_bStandardValueExclusive,
			SourceLibrary.ComponentModel.TypeConverter p_TypeConverter)
		{
			DataModels.DataModelBase l_Editor;
			if (p_TypeConverter != null && p_TypeConverter.CanConvertFrom(typeof(string)) )//txtbox
			{
				DataModels.EditorTextBox l_EditTextBox = new DataModels.EditorTextBox(p_Type);
				l_Editor = l_EditTextBox;
			}
			else //if no editor no edit support
			{
				l_Editor = null;
			}

			if (l_Editor!=null)
			{
				l_Editor.DefaultValue = p_DefaultValue;
				l_Editor.AllowNull = p_bAllowNull;
				//l_Editor.CellType = p_Type;
				l_Editor.StandardValues = p_StandardValues;
				l_Editor.StandardValuesExclusive = p_bStandardValueExclusive;
				l_Editor.TypeConverter = p_TypeConverter;
			}

			return l_Editor;
		}
#else
		/// <summary>
		/// Construct a CellEditor for the specified type
		/// </summary>
		/// <param name="p_Type">Cell Type</param>
		/// <param name="p_DefaultValue">Default value of the editor</param>
		/// <param name="p_bAllowNull">Allow null</param>
		/// <param name="p_StandardValues">List of available values or null if there is no available values list</param>
		/// <param name="p_bStandardValueExclusive">Indicates if the p_StandardValue are the unique values supported</param>
		/// <param name="p_TypeConverter">Type converter used for conversion for the specified type</param>
		/// <param name="p_UITypeEditor">UITypeEditor if null must be populated the TypeConverter</param>
		/// <returns></returns>
		public static DataModels.IDataModel CreateDataModel(Type p_Type, 
			object p_DefaultValue,
			bool p_bAllowNull,
			System.Collections.ICollection p_StandardValues,
			bool p_bStandardValueExclusive,
			System.ComponentModel.TypeConverter p_TypeConverter,
			System.Drawing.Design.UITypeEditor p_UITypeEditor)
		{
			DataModels.DataModelBase l_Editor;
			if (p_UITypeEditor == null)
			{
				if (p_StandardValues!=null)
				{
					DataModels.EditorComboBox l_EditCombo = new DataModels.EditorComboBox(p_Type);
					l_Editor = l_EditCombo;
				}
				else if (p_TypeConverter != null && p_TypeConverter.CanConvertFrom(typeof(string)) )//txtbox
				{
					DataModels.EditorTextBox l_EditTextBox = new DataModels.EditorTextBox(p_Type);
					l_Editor = l_EditTextBox;
				}
				else //if no editor no edit support
				{
					l_Editor = null;
				}
			}
			else //UITypeEditor supported
			{
				DataModels.EditorUITypeEditor l_UITypeEditor = new DataModels.EditorUITypeEditor(p_Type, p_UITypeEditor);
				l_Editor = l_UITypeEditor;
			}

			if (l_Editor!=null)
			{
				l_Editor.DefaultValue = p_DefaultValue;
				l_Editor.AllowNull = p_bAllowNull;
				//l_Editor.CellType = p_Type;
				l_Editor.StandardValues = p_StandardValues;
				l_Editor.StandardValuesExclusive = p_bStandardValueExclusive;
				l_Editor.TypeConverter = p_TypeConverter;
			}

			return l_Editor;
		}
#endif

		#endregion

		#region VisualModelUtility

		public static SourceLibrary.Drawing.ContentAlignment StringFormatoToContentAlignment(System.Drawing.StringFormat p_StringFormat)
		{
			if (Utility.IsBottom(p_StringFormat) && Utility.IsLeft(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.BottomLeft;
			else if (Utility.IsBottom(p_StringFormat) && Utility.IsRight(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.BottomRight;
			else if (Utility.IsBottom(p_StringFormat) && Utility.IsCenter(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.BottomCenter;

			else if (Utility.IsTop(p_StringFormat) && Utility.IsLeft(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.TopLeft;
			else if (Utility.IsTop(p_StringFormat) && Utility.IsRight(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.TopRight;
			else if (Utility.IsTop(p_StringFormat) && Utility.IsCenter(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.TopCenter;

			else if (Utility.IsMiddle(p_StringFormat) && Utility.IsLeft(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.MiddleLeft;
			else if (Utility.IsMiddle(p_StringFormat) && Utility.IsRight(p_StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.MiddleRight;
			else //if (Utility.IsMiddle(StringFormat) && Utility.IsCenter(StringFormat))
				return SourceLibrary.Drawing.ContentAlignment.MiddleCenter;
		}

		/// <summary>
		/// Export a font html element with the specified font and text
		/// </summary>
		/// <param name="p_Writer"></param>
		/// <param name="p_DisplayText"></param>
		/// <param name="p_Font"></param>
		public static void ExportHTML_Element_Font(System.Xml.XmlTextWriter p_Writer, string p_DisplayText, Font p_Font)
		{
#if !MINI
			if (p_Font.Bold)
				p_Writer.WriteStartElement("b");

			if (p_Font.Underline)
				p_Writer.WriteStartElement("u");

			if (p_Font.Italic)
				p_Writer.WriteStartElement("i");
#endif

			//displaytext
			string l_Display = p_DisplayText;
			if (l_Display == null || l_Display.Trim().Length <= 0)
				p_Writer.WriteRaw("&nbsp;");
			else
			{
				l_Display = l_Display.Replace("\r\n","<br>");
				p_Writer.WriteRaw(l_Display);
			}

#if !MINI
			//i
			if (p_Font.Italic)
				p_Writer.WriteEndElement();

			//u
			if (p_Font.Underline)
				p_Writer.WriteEndElement();

			//b
			if (p_Font.Bold)
				p_Writer.WriteEndElement();
#endif
		}

		private const int c_MaxSize = 4000;

		/// <summary>
		/// Returns the minimum required size of the current cell, calculating using the current DisplayString, Image and Borders informations.
		/// </summary>
		/// <param name="p_Graphics"></param>
		/// <param name="p_bAlignTextToImage"></param>
		/// <param name="p_bImageStretch"></param>
		/// <param name="p_Border"></param>
		/// <param name="p_DisplayText"></param>
		/// <param name="p_Font"></param>
		/// <param name="p_Image"></param>
		/// <param name="p_ImageAlignment"></param>
		/// <param name="p_StringFormat"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		public static SizeF MeasureStringAndImage(Graphics p_Graphics,
			string p_DisplayText,
			StringFormat p_StringFormat,
			Font p_Font,
			Image p_Image,
			SourceLibrary.Drawing.ContentAlignment p_ImageAlignment,
			bool p_bAlignTextToImage, 
			bool p_bImageStretch,
			RectangleBorder p_Border,
			SizeF maxLayoutArea)
		{
			SizeF l_ReqSize;

			if (maxLayoutArea.Width <= 0)
				maxLayoutArea.Width = c_MaxSize;
			if (maxLayoutArea.Height <= 0)
				maxLayoutArea.Height = c_MaxSize;

			//Calculate Text Size
			if (p_DisplayText != null && p_DisplayText.Length > 0)
			{
#if !MINI
				l_ReqSize = p_Graphics.MeasureString(p_DisplayText, p_Font, maxLayoutArea, p_StringFormat);
#else
				l_ReqSize = p_Graphics.MeasureString(p_DisplayText, p_Font);
#endif
				l_ReqSize.Width += 2; //2 extra space to always fit the text
				l_ReqSize.Height += 2; //2 extra space to always fit the text
			}
			else
				l_ReqSize = new SizeF(0,0);

			//Calculate Image Size
			if (p_Image != null)
			{
				//Check if align Text To Image
				if (p_bImageStretch == false && p_bAlignTextToImage &&
					p_DisplayText != null && p_DisplayText.Length > 0 )
				{
					if (Utility.IsBottom(p_ImageAlignment) && Utility.IsBottom(p_StringFormat))
						l_ReqSize.Height+=p_Image.Height;
					else if (Utility.IsTop(p_ImageAlignment) && Utility.IsTop(p_StringFormat))
						l_ReqSize.Height+=p_Image.Height;
					else //Max between Image and Text
					{
						if (p_Image.Height > l_ReqSize.Height)
							l_ReqSize.Height = p_Image.Height;
					}

					if (Utility.IsLeft(p_ImageAlignment) && Utility.IsLeft(p_StringFormat))
						l_ReqSize.Width+=p_Image.Width;
					else if (Utility.IsRight(p_ImageAlignment) && Utility.IsRight(p_StringFormat))
						l_ReqSize.Width+=p_Image.Width;
					else //Max between Image and Text
					{
						if (p_Image.Width > l_ReqSize.Width)
							l_ReqSize.Width = p_Image.Width;
					}
				}
				else
				{
					//Max between Image and Text
					if (p_Image.Height > l_ReqSize.Height)
						l_ReqSize.Height = p_Image.Height;
					if (p_Image.Width > l_ReqSize.Width)
						l_ReqSize.Width = p_Image.Width;
				}
			}

			//Add Border Width
			l_ReqSize.Width += p_Border.Left.Width + p_Border.Right.Width;
			l_ReqSize.Height += p_Border.Top.Width + p_Border.Bottom.Width;
	
			return l_ReqSize;
		}	
		#endregion
	}



	internal class Const
	{
		public static string c_ImagePath = @"SourceGrid2.Common.Icons.";
	}

	public class MenuCollection : CollectionBase
	{
		public MenuCollection()
		{
		}

		public int Add(MenuItem p_Item)
		{
			return List.Add(p_Item);
		}
		public void Remove(MenuItem p_Item)
		{
			List.Remove(p_Item);
		}
		public MenuItem this[int p_Index]
		{
			get{return (MenuItem)List[p_Index];}
			set{List[p_Index] = value;}
		}
	}
	
	/// <summary>
	/// A comparer for the Cell class. (Not for CellVirtual). Using the value of the cell.
	/// </summary>
	public class ValueCellComparer : IComparer
	{
		public virtual System.Int32 Compare ( System.Object x , System.Object y )
		{
			//Cell object
			if (x==null && y==null)
				return 0;
			if (x==null)
				return -1;
			if (y==null)
				return 1;

			if (x is IComparable)
				return ((IComparable)x).CompareTo(y);
			if (y is IComparable)
				return (-1* ((IComparable)y).CompareTo(x));

			//Cell.Value object
			object vx = ((Cells.Real.Cell)x).Value;
			object vy = ((Cells.Real.Cell)y).Value;
			if (vx==null && vy==null)
				return 0;
			if (vx==null)
				return -1;
			if (vy==null)
				return 1;

			if (vx is IComparable)
				return ((IComparable)vx).CompareTo(vy);
			if (vy is IComparable)
				return (-1* ((IComparable)vy).CompareTo(vx));

			throw new ArgumentException("Invalid cell object, no IComparable interface found");
		}
	}
	/// <summary>
	/// A comparer for the Cell class. (Not for CellVirtual). Using the DisplayString of the cell.
	/// </summary>
	public class DisplayStringCellComparer : IComparer
	{
		public virtual System.Int32 Compare ( System.Object x , System.Object y )
		{
			//Cell object
			if (x==null && y==null)
				return 0;
			if (x==null)
				return -1;
			if (y==null)
				return 1;

			if (x is IComparable)
				return ((IComparable)x).CompareTo(y);
			if (y is IComparable)
				return (-1* ((IComparable)y).CompareTo(x));

			//Cell.Value object
			string vx = ((Cells.Real.Cell)x).DisplayText;
			string vy = ((Cells.Real.Cell)y).DisplayText;
			if (vx==null && vy==null)
				return 0;
			if (vx==null)
				return -1;
			if (vy==null)
				return 1;

			return vx.CompareTo(vy);
		}
	}
	[Serializable]
	public class SourceGridException : ApplicationException
	{
		public SourceGridException(string p_strErrDescription):
			base(p_strErrDescription)
		{
		}
		public SourceGridException(string p_strErrDescription, Exception p_InnerException):
			base(p_strErrDescription, p_InnerException)
		{
		}
#if !MINI
		protected SourceGridException(SerializationInfo p_Info, StreamingContext p_StreamingContext): 
			base(p_Info, p_StreamingContext)
		{
		}
#endif
	}

	[Serializable]
	public class ObjectIsReadOnlyException : SourceGridException
	{
		public ObjectIsReadOnlyException(string p_strErrDescription):
			base(p_strErrDescription)
		{
		}
		public ObjectIsReadOnlyException(string p_strErrDescription, Exception p_InnerException):
			base(p_strErrDescription, p_InnerException)
		{
		}
#if !MINI
		protected ObjectIsReadOnlyException(SerializationInfo p_Info, StreamingContext p_StreamingContext): 
			base(p_Info, p_StreamingContext)
		{
		}
#endif
	}

	public class CommonImages
	{
		private CommonImages()
		{
		}
		private static Image ExtractImage(string p_Image)
		{
#if !MINI
			System.Reflection.Assembly l_as = System.Reflection.Assembly.GetExecutingAssembly();
			return Image.FromStream(l_as.GetManifestResourceStream(Const.c_ImagePath + p_Image));
#else
			System.Reflection.Assembly l_as = System.Reflection.Assembly.GetExecutingAssembly();
			Icon tmp = new Icon(l_as.GetManifestResourceStream(Const.c_ImagePath + p_Image));
			Bitmap b = new Bitmap(tmp.Width, tmp.Height);
			using (Graphics g = Graphics.FromImage(b))
			{
				g.Clear(Color.Transparent);
				g.DrawIcon(tmp,0,0);
			}
			return b;
#endif
		}

		static CommonImages()
		{
			m_SortDown = ExtractImage("SortDown.ico");
			m_SortUp = ExtractImage("SortUp.ico");
			m_CheckBoxChecked = ExtractImage("CheckBoxChecked.ico");
			m_CheckBoxCheckedDisable = ExtractImage("CheckBoxCheckedDisable.ico");
			m_CheckBoxUnChecked = ExtractImage("CheckBoxUnChecked.ico");
			m_CheckBoxUnCheckedDisable = ExtractImage("CheckBoxUnCheckedDisable.ico");
#if !MINI
			m_CheckBoxUnCheckedSel = ExtractImage("CheckBoxUnCheckedSel.ico");
			m_CheckBoxCheckedSel = ExtractImage("CheckBoxCheckedSel.ico");
			m_Clear = ExtractImage("clear.ico");
			m_Copy = ExtractImage("copy.ico");
			m_Cut = ExtractImage("cut.ico");
			m_DeleteCol = ExtractImage("DeleteCol.ico");
			m_DeleteRow = ExtractImage("DeleteRow.ico");
			m_InsertCol = ExtractImage("InsertCol.ico");
			m_InsertRow = ExtractImage("InsertRow.ico");
			m_Paste = ExtractImage("paste.ico");
			m_Properties = ExtractImage("properties.ico");
#endif
		}

		private static Image m_SortDown;
		public static Image SortDown
		{
			get{return m_SortDown;}
		}
		private static Image m_SortUp;
		public static Image SortUp
		{
			get{return m_SortUp;}
		}
		private static Image m_CheckBoxChecked;
		public static Image CheckBoxChecked
		{
			get{return m_CheckBoxChecked;}
		}
		private static Image m_CheckBoxCheckedDisable;
		public static Image CheckBoxCheckedDisable
		{
			get{return m_CheckBoxCheckedDisable;}
		}
		private static Image m_CheckBoxUnChecked;
		public static Image CheckBoxUnChecked
		{
			get{return m_CheckBoxUnChecked;}
		}
		private static Image m_CheckBoxUnCheckedDisable;
		public static Image CheckBoxUnCheckedDisable
		{
			get{return m_CheckBoxUnCheckedDisable;}
		}

#if !MINI
		private static Image m_CheckBoxUnCheckedSel;
		public static Image CheckBoxUnCheckedSel
		{
			get{return m_CheckBoxUnCheckedSel;}
		}

		private static Image m_CheckBoxCheckedSel;
		public static Image CheckBoxCheckedSel
		{
			get{return m_CheckBoxCheckedSel;}
		}

		private static Image m_Clear;
		public static Image Clear
		{
			get{return m_Clear;}
		}
		private static Image m_Copy;
		public static Image Copy
		{
			get{return m_Copy;}
		}
		private static Image m_Cut;
		public static Image Cut
		{
			get{return m_Cut;}
		}
		private static Image m_DeleteCol;
		public static Image DeleteCol
		{
			get{return m_DeleteCol;}
		}
		private static Image m_DeleteRow;
		public static Image DeleteRow
		{
			get{return m_DeleteRow;}
		}
		private static Image m_InsertCol;
		public static Image InsertCol
		{
			get{return m_InsertCol;}
		}
		private static Image m_InsertRow;
		public static Image InsertRow
		{
			get{return m_InsertRow;}
		}
		private static Image m_Paste;
		public static Image Paste
		{
			get{return m_Paste;}
		}
		private static Image m_Properties;
		public static Image Properties
		{
			get{return m_Properties;}
		}
#endif
	}

#if !MINI
	public interface IHTMLExport
	{
		System.IO.Stream Stream
		{
			get;
		}

		/// <summary>
		/// Save the Image to file and returns the file
		/// </summary>
		/// <param name="p_Image"></param>
		/// <returns>Returns the path where the image is exported valid for the HTML page</returns>
		string ExportImage(Image p_Image);

		/// <summary>
		/// Export mode
		/// </summary>
		ExportHTMLMode Mode
		{
			get;
		}
	}

	/// <summary>
	/// Setting for the export HTML of the grid
	/// </summary>
	public class HTMLExport : IHTMLExport
	{
		protected ExportHTMLMode m_Mode = ExportHTMLMode.Default;
		protected System.IO.Stream m_Stream;
		protected string m_ImageFullPath;
		protected string m_ImageRelativePath;

		/// <summary>
		/// Key:Image, Value:ImageFileName
		/// </summary>
		protected Hashtable m_EmbeddedImagesPath = new Hashtable();

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Mode"></param>
		/// <param name="p_ImageFullPath">The path to write embedded images files</param>
		/// <param name="p_ImageRelativePath">The path used in the HTML source. If you save the images in the same path of the HTML file you can leave this path empty.</param>
		/// <param name="p_HtmlStream">The stream to write</param>
		public HTMLExport(ExportHTMLMode p_Mode, string p_ImageFullPath , string p_ImageRelativePath, System.IO.Stream p_HtmlStream)
		{
			m_Mode = p_Mode;
			m_Stream = p_HtmlStream;
			m_ImageFullPath = p_ImageFullPath;
			m_ImageRelativePath = p_ImageRelativePath;
		}

		/// <summary>
		/// Export mode
		/// </summary>
		public virtual ExportHTMLMode Mode
		{
			get{return m_Mode;}
			set{m_Mode = value;}
		}

		/// <summary>
		/// List of images exported during HTML export
		/// </summary>
		public virtual string[] EmbeddedImagesPath
		{
			get
			{
				string[] l_Images = new string[m_EmbeddedImagesPath.Count];
				m_EmbeddedImagesPath.Values.CopyTo(l_Images,0);
				return l_Images;
			}
		}

		/// <summary>
		/// Clear the list of embedded images. This method don't delete the files only clear the list.
		/// </summary>
		public virtual void ClearEmbeddedImages()
		{
			m_EmbeddedImagesPath.Clear();
		}

		public virtual System.IO.Stream Stream
		{
			get{return m_Stream;}
		}

		/// <summary>
		/// Save the Image to file and returns the file
		/// </summary>
		/// <param name="p_Image"></param>
		/// <returns>Returns the path where the image is exported valid for the HTML page</returns>
		public virtual string ExportImage(Image p_Image)
		{
			string l_FileName;
			if (m_EmbeddedImagesPath.ContainsKey(p_Image))
			{
				l_FileName = (string)m_EmbeddedImagesPath[p_Image];
			}
			else
			{
				l_FileName = System.IO.Path.Combine(m_ImageFullPath, System.IO.Path.GetTempFileName() + ".jpg");
				p_Image.Save(l_FileName,System.Drawing.Imaging.ImageFormat.Jpeg);

				m_EmbeddedImagesPath.Add(p_Image, l_FileName);
			}

			//change to use relative path
			l_FileName = l_FileName.Replace(m_ImageFullPath, m_ImageRelativePath);
			return l_FileName; 
		}

		/// <summary>
		/// Convert a Color to HTML compatible string
		/// </summary>
		/// <param name="p_Color"></param>
		/// <returns></returns>
		public static string ColorToHTML(Color p_Color)
		{
			//non uso direttamente ToHtml su p_Color petchè se questo contiene dei valori tipo (Window, Control, cioè valori definiti in base al sistema) su Mozilla non funzionano
			return ColorTranslator.ToHtml(Color.FromArgb(p_Color.A,p_Color.R, p_Color.G, p_Color.B));
		}

		public static string BorderToHTMLStyle(Border p_Border)
		{
			if (p_Border.Width>0)
			{
				return p_Border.Width.ToString() + "px solid " + ColorToHTML(p_Border.Color);
			}
			else
				return "none";
		}

		public static string CellBorderToHTMLStyle(RectangleBorder p_Border)
		{
			return "border-top:" + BorderToHTMLStyle(p_Border.Top) + 
				";border-right:" + BorderToHTMLStyle(p_Border.Right) + 
				";border-bottom:" + BorderToHTMLStyle(p_Border.Bottom) + 
				";border-left:" + BorderToHTMLStyle(p_Border.Left) + 
				";";
		}
	}
#endif

	/// <summary>
	/// A collection of elements of type Position
	/// </summary>
	public class PositionCollection : System.Collections.CollectionBase
	{
		/// <summary>
		/// Initializes a new empty instance of the PositionCollection class.
		/// </summary>
		public PositionCollection()
		{
			// empty
		}

		/// <summary>
		/// Initializes a new instance of the PositionCollection class, containing elements
		/// copied from an array.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the new PositionCollection.
		/// </param>
		public PositionCollection(Position[] items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Initializes a new instance of the PositionCollection class, containing elements
		/// copied from another instance of PositionCollection
		/// </summary>
		/// <param name="items">
		/// The PositionCollection whose elements are to be added to the new PositionCollection.
		/// </param>
		public PositionCollection(PositionCollection items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Adds the elements of an array to the end of this PositionCollection.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the end of this PositionCollection.
		/// </param>
		public virtual void AddRange(Position[] items)
		{
			foreach (Position item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds the elements of another PositionCollection to the end of this PositionCollection.
		/// </summary>
		/// <param name="items">
		/// The PositionCollection whose elements are to be added to the end of this PositionCollection.
		/// </param>
		public virtual void AddRange(PositionCollection items)
		{
			foreach (Position item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds an instance of type Position to the end of this PositionCollection.
		/// </summary>
		/// <param name="value">
		/// The Position to be added to the end of this PositionCollection.
		/// </param>
		public virtual void Add(Position value)
		{
			this.List.Add(value);
		}

		/// <summary>
		/// Determines whether a specfic Position value is in this PositionCollection.
		/// </summary>
		/// <param name="value">
		/// The Position value to locate in this PositionCollection.
		/// </param>
		/// <returns>
		/// true if value is found in this PositionCollection;
		/// false otherwise.
		/// </returns>
		public virtual bool Contains(Position value)
		{
			return this.List.Contains(value);
		}

		/// <summary>
		/// Return the zero-based index of the first occurrence of a specific value
		/// in this PositionCollection
		/// </summary>
		/// <param name="value">
		/// The Position value to locate in the PositionCollection.
		/// </param>
		/// <returns>
		/// The zero-based index of the first occurrence of the _ELEMENT value if found;
		/// -1 otherwise.
		/// </returns>
		public virtual int IndexOf(Position value)
		{
			return this.List.IndexOf(value);
		}

		/// <summary>
		/// Inserts an element into the PositionCollection at the specified index
		/// </summary>
		/// <param name="index">
		/// The index at which the Position is to be inserted.
		/// </param>
		/// <param name="value">
		/// The Position to insert.
		/// </param>
		public virtual void Insert(int index, Position value)
		{
			this.List.Insert(index, value);
		}

		/// <summary>
		/// Gets or sets the Position at the given index in this PositionCollection.
		/// </summary>
		public virtual Position this[int index]
		{
			get
			{
				return (Position) this.List[index];
			}
			set
			{
				this.List[index] = value;
			}
		}

		/// <summary>
		/// Removes the first occurrence of a specific Position from this PositionCollection.
		/// </summary>
		/// <param name="value">
		/// The Position value to remove from this PositionCollection.
		/// </param>
		public virtual void Remove(Position value)
		{
			this.List.Remove(value);
		}

		/// <summary>
		/// Type-specific enumeration class, used by PositionCollection.GetEnumerator.
		/// </summary>
		public class Enumerator: System.Collections.IEnumerator
		{
			private System.Collections.IEnumerator wrapped;

			public Enumerator(PositionCollection collection)
			{
				this.wrapped = ((System.Collections.CollectionBase)collection).GetEnumerator();
			}

			public Position Current
			{
				get
				{
					return (Position) (this.wrapped.Current);
				}
			}

			object System.Collections.IEnumerator.Current
			{
				get
				{
					return (Position) (this.wrapped.Current);
				}
			}

			public bool MoveNext()
			{
				return this.wrapped.MoveNext();
			}

			public void Reset()
			{
				this.wrapped.Reset();
			}
		}

		/// <summary>
		/// Returns an enumerator that can iterate through the elements of this PositionCollection.
		/// </summary>
		/// <returns>
		/// An object that implements System.Collections.IEnumerator.
		/// </returns>        
		public new virtual PositionCollection.Enumerator GetEnumerator()
		{
			return new PositionCollection.Enumerator(this);
		}
	}

#if !MINI
	//la classe DictionaryBase non esisteva nel Compact Framework e non sono riuscia a riprodurla

	/// <summary>
	/// A dictionary with keys of type Control and values of type Position
	/// </summary>
	public class LinkedControlsList : System.Collections.DictionaryBase
	{
		/// <summary>
		/// Initializes a new empty instance of the ControlToPositionAssociation class
		/// </summary>
		public LinkedControlsList()
		{
			// empty
		}

		/// <summary>
		/// Gets or sets the Position associated with the given Control
		/// </summary>
		/// <param name="key">
		/// The Control whose value to get or set.
		/// </param>
		public virtual Position this[Control key]
		{
			get
			{
				return (Position) this.Dictionary[key];
			}
			set
			{
				this.Dictionary[key] = value;
			}
		}

		/// <summary>
		/// Adds an element with the specified key and value to this ControlToPositionAssociation.
		/// </summary>
		/// <param name="key">
		/// The Control key of the element to add.
		/// </param>
		/// <param name="value">
		/// The Position value of the element to add.
		/// </param>
		public virtual void Add(Control key, Position value)
		{
			this.Dictionary.Add(key, value);
		}

		/// <summary>
		/// Determines whether this ControlToPositionAssociation contains a specific key.
		/// </summary>
		/// <param name="key">
		/// The Control key to locate in this ControlToPositionAssociation.
		/// </param>
		/// <returns>
		/// true if this ControlToPositionAssociation contains an element with the specified key;
		/// otherwise, false.
		/// </returns>
		public virtual bool Contains(Control key)
		{
			return this.Dictionary.Contains(key);
		}

		/// <summary>
		/// Determines whether this ControlToPositionAssociation contains a specific key.
		/// </summary>
		/// <param name="key">
		/// The Control key to locate in this ControlToPositionAssociation.
		/// </param>
		/// <returns>
		/// true if this ControlToPositionAssociation contains an element with the specified key;
		/// otherwise, false.
		/// </returns>
		public virtual bool ContainsKey(Control key)
		{
			return this.Dictionary.Contains(key);
		}

		/// <summary>
		/// Determines whether this ControlToPositionAssociation contains a specific value.
		/// </summary>
		/// <param name="value">
		/// The Position value to locate in this ControlToPositionAssociation.
		/// </param>
		/// <returns>
		/// true if this ControlToPositionAssociation contains an element with the specified value;
		/// otherwise, false.
		/// </returns>
		public virtual bool ContainsValue(Position value)
		{
			foreach (Position item in this.Dictionary.Values)
			{
				if (item == value)
					return true;
			}
			return false;
		}

		/// <summary>
		/// Removes the element with the specified key from this ControlToPositionAssociation.
		/// </summary>
		/// <param name="key">
		/// The Control key of the element to remove.
		/// </param>
		public virtual void Remove(Control key)
		{
			this.Dictionary.Remove(key);
		}

		/// <summary>
		/// Gets a collection containing the keys in this ControlToPositionAssociation.
		/// </summary>
		public virtual System.Collections.ICollection Keys
		{
			get
			{
				return this.Dictionary.Keys;
			}
		}

		/// <summary>
		/// Gets a collection containing the values in this ControlToPositionAssociation.
		/// </summary>
		public virtual System.Collections.ICollection Values
		{
			get
			{
				return this.Dictionary.Values;
			}
		}

		private bool m_bUseCellBorder = true;

		/// <summary>
		/// True to insert the editor control inside the border of the cell, false to put the editor control over the entire cell. If you use true remember to set EnableCellDrawOnEdit == true.
		/// </summary>
		public bool UseCellBorder
		{
			get{return m_bUseCellBorder;}
			set{m_bUseCellBorder = value;}
		}
	}
#else
//TODO vedere se si riesce a riprodurre DictionaryBase per MINI

	/// <summary>
	/// A dictionary with keys of type Control and values of type Position
	/// </summary>
	public class LinkedControlsList : IDictionary
	{
		private Hashtable m_HashTable = new Hashtable();

		public void Add(object key, object value)
		{
			Add((Control)key, (Position)value);
		}
		public void Clear()
		{
			m_HashTable.Clear();
		}
		public bool Contains(object key)
		{
			return Contains((Control)key);
		}
		IDictionaryEnumerator IDictionary.GetEnumerator()
		{
			return m_HashTable.GetEnumerator();
		}
		public void Remove(object key)
		{
			Remove((Control)key);
		}
		public bool IsFixedSize
		{
			get{return m_HashTable.IsFixedSize;}
		}

		public bool IsReadOnly
		{
			get{return m_HashTable.IsReadOnly;}
		}
		public object this[object key]
		{
			get{return m_HashTable[key];}
			set{m_HashTable[key] = value;}
		}

		public void CopyTo(Array array, int index)
		{
			m_HashTable.CopyTo(array, index);
		}
		public int Count
		{
			get{return m_HashTable.Count;} 
		}
		public bool IsSynchronized
		{
			get{return m_HashTable.IsSynchronized;}
		}
		public object SyncRoot
		{
			get{return m_HashTable.SyncRoot;}
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return m_HashTable.GetEnumerator();
		}

		/// <summary>
		/// Initializes a new empty instance of the ControlToPositionAssociation class
		/// </summary>
		public LinkedControlsList()
		{
			// empty
		}

		/// <summary>
		/// Gets or sets the Position associated with the given Control
		/// </summary>
		/// <param name="key">
		/// The Control whose value to get or set.
		/// </param>
		public virtual Position this[Control key]
		{
			get
			{
				return (Position) m_HashTable[key];
			}
			set
			{
				m_HashTable[key] = value;
			}
		}

		/// <summary>
		/// Adds an element with the specified key and value to this ControlToPositionAssociation.
		/// </summary>
		/// <param name="key">
		/// The Control key of the element to add.
		/// </param>
		/// <param name="value">
		/// The Position value of the element to add.
		/// </param>
		public virtual void Add(Control key, Position value)
		{
			m_HashTable.Add(key, value);
		}

		/// <summary>
		/// Determines whether this ControlToPositionAssociation contains a specific key.
		/// </summary>
		/// <param name="key">
		/// The Control key to locate in this ControlToPositionAssociation.
		/// </param>
		/// <returns>
		/// true if this ControlToPositionAssociation contains an element with the specified key;
		/// otherwise, false.
		/// </returns>
		public virtual bool Contains(Control key)
		{
			return m_HashTable.Contains(key);
		}

		/// <summary>
		/// Determines whether this ControlToPositionAssociation contains a specific key.
		/// </summary>
		/// <param name="key">
		/// The Control key to locate in this ControlToPositionAssociation.
		/// </param>
		/// <returns>
		/// true if this ControlToPositionAssociation contains an element with the specified key;
		/// otherwise, false.
		/// </returns>
		public virtual bool ContainsKey(Control key)
		{
			return m_HashTable.Contains(key);
		}

		/// <summary>
		/// Determines whether this ControlToPositionAssociation contains a specific value.
		/// </summary>
		/// <param name="value">
		/// The Position value to locate in this ControlToPositionAssociation.
		/// </param>
		/// <returns>
		/// true if this ControlToPositionAssociation contains an element with the specified value;
		/// otherwise, false.
		/// </returns>
		public virtual bool ContainsValue(Position value)
		{
			foreach (Position item in m_HashTable.Values)
			{
				if (item == value)
					return true;
			}
			return false;
		}

		/// <summary>
		/// Removes the element with the specified key from this ControlToPositionAssociation.
		/// </summary>
		/// <param name="key">
		/// The Control key of the element to remove.
		/// </param>
		public virtual void Remove(Control key)
		{
			m_HashTable.Remove(key);
		}

		/// <summary>
		/// Gets a collection containing the keys in this ControlToPositionAssociation.
		/// </summary>
		public virtual System.Collections.ICollection Keys
		{
			get
			{
				return m_HashTable.Keys;
			}
		}

		/// <summary>
		/// Gets a collection containing the values in this ControlToPositionAssociation.
		/// </summary>
		public virtual System.Collections.ICollection Values
		{
			get
			{
				return m_HashTable.Values;
			}
		}

		private bool m_bUseCellBorder = true;

		/// <summary>
		/// True to insert the editor control inside the border of the cell, false to put the editor control over the entire cell. If you use true remember to set EnableCellDrawOnEdit == true.
		/// </summary>
		public bool UseCellBorder
		{
			get{return m_bUseCellBorder;}
			set{m_bUseCellBorder = value;}
		}
	}
#endif

	#region Multi Images Types
	public class PositionedImage
	{
		private Image m_Image;
		public PositionedImage(Image p_Image, 
			SourceLibrary.Drawing.ContentAlignment p_Align)
		{
			m_Image = p_Image;
			m_Alignment = p_Align;
		}
		public Image Image
		{
			get{return m_Image;}
		}

		private SourceLibrary.Drawing.ContentAlignment m_Alignment;
		public SourceLibrary.Drawing.ContentAlignment Alignment
		{
			get{return m_Alignment;}
		}
	}

	/// <summary>
	/// A collection of elements of type PositionedImage
	/// </summary>
	public class PositionedImageCollection: System.Collections.CollectionBase, ICloneable
	{
		/// <summary>
		/// Initializes a new empty instance of the PositionedImageCollection class.
		/// </summary>
		public PositionedImageCollection()
		{
			// empty
		}

		/// <summary>
		/// Initializes a new instance of the PositionedImageCollection class, containing elements
		/// copied from an array.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the new PositionedImageCollection.
		/// </param>
		public PositionedImageCollection(PositionedImage[] items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Initializes a new instance of the PositionedImageCollection class, containing elements
		/// copied from another instance of PositionedImageCollection
		/// </summary>
		/// <param name="items">
		/// The PositionedImageCollection whose elements are to be added to the new PositionedImageCollection.
		/// </param>
		public PositionedImageCollection(PositionedImageCollection items)
		{
			this.AddRange(items);
		}

		/// <summary>
		/// Adds the elements of an array to the end of this PositionedImageCollection.
		/// </summary>
		/// <param name="items">
		/// The array whose elements are to be added to the end of this PositionedImageCollection.
		/// </param>
		public virtual void AddRange(PositionedImage[] items)
		{
			foreach (PositionedImage item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds the elements of another PositionedImageCollection to the end of this PositionedImageCollection.
		/// </summary>
		/// <param name="items">
		/// The PositionedImageCollection whose elements are to be added to the end of this PositionedImageCollection.
		/// </param>
		public virtual void AddRange(PositionedImageCollection items)
		{
			foreach (PositionedImage item in items)
			{
				this.List.Add(item);
			}
		}

		/// <summary>
		/// Adds an instance of type PositionedImage to the end of this PositionedImageCollection.
		/// </summary>
		/// <param name="value">
		/// The PositionedImage to be added to the end of this PositionedImageCollection.
		/// </param>
		public virtual void Add(PositionedImage value)
		{
			this.List.Add(value);
		}

		/// <summary>
		/// Determines whether a specfic PositionedImage value is in this PositionedImageCollection.
		/// </summary>
		/// <param name="value">
		/// The PositionedImage value to locate in this PositionedImageCollection.
		/// </param>
		/// <returns>
		/// true if value is found in this PositionedImageCollection;
		/// false otherwise.
		/// </returns>
		public virtual bool Contains(PositionedImage value)
		{
			return this.List.Contains(value);
		}

		/// <summary>
		/// Return the zero-based index of the first occurrence of a specific value
		/// in this PositionedImageCollection
		/// </summary>
		/// <param name="value">
		/// The PositionedImage value to locate in the PositionedImageCollection.
		/// </param>
		/// <returns>
		/// The zero-based index of the first occurrence of the _ELEMENT value if found;
		/// -1 otherwise.
		/// </returns>
		public virtual int IndexOf(PositionedImage value)
		{
			return this.List.IndexOf(value);
		}

		/// <summary>
		/// Inserts an element into the PositionedImageCollection at the specified index
		/// </summary>
		/// <param name="index">
		/// The index at which the PositionedImage is to be inserted.
		/// </param>
		/// <param name="value">
		/// The PositionedImage to insert.
		/// </param>
		public virtual void Insert(int index, PositionedImage value)
		{
			this.List.Insert(index, value);
		}

		/// <summary>
		/// Gets or sets the PositionedImage at the given index in this PositionedImageCollection.
		/// </summary>
		public virtual PositionedImage this[int index]
		{
			get
			{
				return (PositionedImage) this.List[index];
			}
			set
			{
				this.List[index] = value;
			}
		}

		/// <summary>
		/// Removes the first occurrence of a specific PositionedImage from this PositionedImageCollection.
		/// </summary>
		/// <param name="value">
		/// The PositionedImage value to remove from this PositionedImageCollection.
		/// </param>
		public virtual void Remove(PositionedImage value)
		{
			this.List.Remove(value);
		}

		/// <summary>
		/// Type-specific enumeration class, used by PositionedImageCollection.GetEnumerator.
		/// </summary>
		public class Enumerator: System.Collections.IEnumerator
		{
			private System.Collections.IEnumerator wrapped;

			public Enumerator(PositionedImageCollection collection)
			{
				this.wrapped = ((System.Collections.CollectionBase)collection).GetEnumerator();
			}

			public PositionedImage Current
			{
				get
				{
					return (PositionedImage) (this.wrapped.Current);
				}
			}

			object System.Collections.IEnumerator.Current
			{
				get
				{
					return (PositionedImage) (this.wrapped.Current);
				}
			}

			public bool MoveNext()
			{
				return this.wrapped.MoveNext();
			}

			public void Reset()
			{
				this.wrapped.Reset();
			}
		}

		/// <summary>
		/// Returns an enumerator that can iterate through the elements of this PositionedImageCollection.
		/// </summary>
		/// <returns>
		/// An object that implements System.Collections.IEnumerator.
		/// </returns>        
		public new virtual PositionedImageCollection.Enumerator GetEnumerator()
		{
			return new PositionedImageCollection.Enumerator(this);
		}
		#region ICloneable Members

		public object Clone()
		{
			//TODO bisognerebbe clonare anche gli elementi ?
			return new PositionedImageCollection(this);
		}

		#endregion
	}

	#endregion

}
