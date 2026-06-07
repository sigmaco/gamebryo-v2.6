using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;

using System.Windows.Forms;


namespace SourceGrid2.DataModels
{
	public class EditorTextBoxButton : EditorControlBase
	{
		#region Constructor
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		public EditorTextBoxButton(Type p_Type):base(p_Type)
		{
		}
		#endregion

		#region Edit Control
		public override Control CreateEditorControl()
		{
			SourceLibrary.Windows.Forms.TextBoxTypedButton l_ComboBox = new SourceLibrary.Windows.Forms.TextBoxTypedButton();
			l_ComboBox.TextBox.BorderStyle = BorderStyle.None;
			return l_ComboBox;
		}

		[Obsolete("Use GetEditorTextBoxTypedButton(GridSubPanel)")]
		public virtual SourceLibrary.Windows.Forms.TextBoxTypedButton GetEditorTextBoxTypedButton(GridVirtual p_Grid)
		{
			return (SourceLibrary.Windows.Forms.TextBoxTypedButton)GetEditorControl(p_Grid);
		}

		public virtual SourceLibrary.Windows.Forms.TextBoxTypedButton GetEditorTextBoxTypedButton(GridSubPanel p_GridPanel)
		{
			return (SourceLibrary.Windows.Forms.TextBoxTypedButton)GetEditorControl(p_GridPanel);
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

			SourceLibrary.Windows.Forms.TextBoxTypedButton l_TxtBox = GetEditorTextBoxTypedButton(p_Cell.Grid.PanelAtPosition(p_Position));
			//to set the scroll of the textbox to the initial position (otherwise the textbox use the previous scroll position)
			l_TxtBox.TextBox.SelectionStart = 0;
			l_TxtBox.TextBox.SelectionLength = 0;

			if (p_StartEditValue is string && IsStringConversionSupported())
			{
				l_TxtBox.TextBox.Text = SourceLibrary.Windows.Forms.TextBoxTyped.ValidateCharactersString((string)p_StartEditValue, l_TxtBox.TextBox.ValidCharacters, l_TxtBox.TextBox.InvalidCharacters);
				if (l_TxtBox.TextBox.Text!=null)
					l_TxtBox.TextBox.SelectionStart = l_TxtBox.TextBox.Text.Length;
				else
					l_TxtBox.TextBox.SelectionStart = 0;
			}
			else
			{
				l_TxtBox.Value = p_Cell.GetValue(p_Position);
				l_TxtBox.SelectAllTextBox();
			}
		}

		/// <summary>
		/// Fired when the control is attached to a panel. Use this method to customize the control.
		/// </summary>
		/// <param name="e"></param>
		protected override void OnEditorAttached(ControlEventArgs e)
		{
			base.OnEditorAttached(e);

			SourceLibrary.Windows.Forms.TextBoxTypedButton l_TxtBox = (SourceLibrary.Windows.Forms.TextBoxTypedButton)e.Control;

			l_TxtBox.Validator = this;
			l_TxtBox.EnableEscapeKeyUndo = false;
			l_TxtBox.EnableEnterKeyValidate = false;
			l_TxtBox.EnableLastValidValue = false;
			l_TxtBox.EnableAutoValidation = false;
		}

		/// <summary>
		/// Returns the value inserted with the current editor control
		/// </summary>
		/// <returns></returns>
		public override object GetEditedValue()
		{
			return GetEditorTextBoxTypedButton(EditCell.Grid.PanelAtPosition(EditPosition)).Value;
		}
	}
}

