using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;

using System.Windows.Forms;


namespace SourceGrid2.DataModels
{
	/// <summary>
	/// Create a DataModel editor that use a DateTimePicker as editor and a DateTimeTypeConverter for TypeConverter using a time format.
	/// </summary>
	public class EditorTime : EditorDateTime
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public EditorTime():this("T", new string[]{"T"})
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		public EditorTime( System.String p_ToStringFormat , string[] p_ParseFormats)
		{
			SourceLibrary.ComponentModel.Converter.DateTimeTypeConverter timeConverter = new SourceLibrary.ComponentModel.Converter.DateTimeTypeConverter(p_ToStringFormat, p_ParseFormats);
			TypeConverter = timeConverter;
		}

		#region Edit Control
		/// <summary>
		/// Create the editor control
		/// </summary>
		/// <returns></returns>
		public override Control CreateEditorControl()
		{
			System.Windows.Forms.DateTimePicker l_dtPicker = new DateTimePicker();
			l_dtPicker.Format = DateTimePickerFormat.Time;
			l_dtPicker.ShowUpDown = true;
			return l_dtPicker;
		}
		#endregion
	}
}

