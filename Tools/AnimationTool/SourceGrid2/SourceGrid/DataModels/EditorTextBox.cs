using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;

using System.Windows.Forms;


namespace SourceGrid2.DataModels
{
	/// <summary>
	/// A DataModel that use a TextBoxTyped for editing support.
	/// </summary>
	public class EditorTextBox : EditorControlBase
	{
		#region Constructor
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		public EditorTextBox(Type p_Type):base(p_Type)
		{
		}
		#endregion

		#region Edit Control
		public override Control CreateEditorControl()
		{
			SourceLibrary.Windows.Forms.TextBoxTyped l_Control = new SourceLibrary.Windows.Forms.TextBoxTyped();
#if !MINI
			l_Control.BorderStyle = BorderStyle.None;
			l_Control.AutoSize = false;
#endif
			return l_Control;
		}

		[Obsolete("Use GetEditorTextBox(GridSubPanel)")]
		public virtual SourceLibrary.Windows.Forms.TextBoxTyped GetEditorTextBox(GridVirtual p_Grid)
		{
			return (SourceLibrary.Windows.Forms.TextBoxTyped)GetEditorControl(p_Grid);
		}

		public virtual SourceLibrary.Windows.Forms.TextBoxTyped GetEditorTextBox(GridSubPanel p_GridSubPanel)
		{
			return (SourceLibrary.Windows.Forms.TextBoxTyped)GetEditorControl(p_GridSubPanel);
		}
		#endregion

		/// <summary>
		/// Start editing the cell passed. Do not call this method for start editing a cell, you must use Cell.StartEdit.
		/// </summary>
		/// <param name="p_Cell">Cell to start edit</param>
		/// <param name="p_Position">Editing position(Row/Col)</param>
		/// <param name="p_StartEditValue">Can be null(in this case use the p_cell.Value</param>
		public override void InternalStartEdit(Cells.ICellVirtual p_Cell, Position p_Position, object p_StartEditValue)
		{
			base.InternalStartEdit(p_Cell, p_Position, p_StartEditValue);

			if (EnableEdit==false)
				return;

			SourceLibrary.Windows.Forms.TextBoxTyped l_TxtBox = GetEditorTextBox(p_Cell.Grid.PanelAtPosition(p_Position));
			l_TxtBox.WordWrap = p_Cell.VisualModel.WordWrap;
			l_TxtBox.TextAlign = Utility.ContentToHorizontalAlignment(p_Cell.VisualModel.TextAlignment);
			l_TxtBox.Font = p_Cell.VisualModel.Font;
			//to set the scroll of the textbox to the initial position (otherwise the textbox use the previous scroll position)
			l_TxtBox.SelectionStart = 0;
			l_TxtBox.SelectionLength = 0;

			//if (p_StartEditValue!=null && IsStringConversionSupported())
			if (p_StartEditValue is string && IsStringConversionSupported())
			{
				l_TxtBox.Text = SourceLibrary.Windows.Forms.TextBoxTyped.ValidateCharactersString((string)p_StartEditValue, m_ValidCharacters, m_InvalidCharacters);
				l_TxtBox.SelectionStart = l_TxtBox.Text.Length;
			}
			else
			{
				l_TxtBox.Value = p_Cell.GetValue(p_Position);
				l_TxtBox.SelectAll();
			}
		}

		/// <summary>
		/// Fired when the control is attached to a panel. Use this method to customize the control.
		/// </summary>
		/// <param name="e"></param>
		protected override void OnEditorAttached(ControlEventArgs e)
		{
			base.OnEditorAttached(e);

			SourceLibrary.Windows.Forms.TextBoxTyped l_TxtBox = (SourceLibrary.Windows.Forms.TextBoxTyped)e.Control;

			l_TxtBox.Validator = this;
			l_TxtBox.EnableEscapeKeyUndo = false;
			l_TxtBox.EnableEnterKeyValidate = false;
			l_TxtBox.EnableLastValidValue = false;
			l_TxtBox.EnableAutoValidation = false;

			l_TxtBox.Multiline = m_bMultiLine;
			if (m_bMultiLine==false)
				l_TxtBox.MaxLength = m_MaxLength;

			l_TxtBox.ValidCharacters = m_ValidCharacters;
			l_TxtBox.InvalidCharacters = m_InvalidCharacters;
		}

		private bool m_bMultiLine = false;
		/// <summary>
		/// Gets or sets a value indicating whether this is a multiline text box editor.
		/// </summary>
		public bool Multiline
		{
			get{return m_bMultiLine;}
			set{m_bMultiLine = value;}
		}

		private int m_MaxLength = 0;
		/// <summary>
		/// Gets or sets the maximum number of characters allowed in the text box editor.
		/// </summary>
		public int MaxLength
		{
			get{return m_MaxLength;}
			set{m_MaxLength = value;}
		}
		private char[] m_ValidCharacters;

		/// <summary>
		/// A list of characters allowed for the textbox. Used in the OnKeyPress event. If null no check is made.
		/// If not null any others charecters is not allowed. First the function check if ValidCharacters is not null then check for InvalidCharacters.
		/// </summary>
		public char[] ValidCharacters
		{
			get{return m_ValidCharacters;}
			set{m_ValidCharacters = value;}
		}

		private char[] m_InvalidCharacters;

		/// <summary>
		/// A list of characters not allowed for the textbox. Used in the OnKeyPress event. If null no check is made.
		/// If not null any characters in the list is not allowed. First the function check if ValidCharacters is not null then check for InvalidCharacters.
		/// </summary>
		public char[] InvalidCharacters
		{
			get{return m_InvalidCharacters;}
			set{m_InvalidCharacters = value;}
		}
		/// <summary>
		/// Returns the value inserted with the current editor control
		/// </summary>
		/// <returns></returns>
		public override object GetEditedValue()
		{
			return GetEditorTextBox(EditCell.Grid.PanelAtPosition(EditPosition)).Value;
		}
	}
}

