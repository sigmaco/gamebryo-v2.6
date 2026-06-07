using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;

using System.Windows.Forms;


namespace SourceGrid2.DataModels
{
	/// <summary>
	/// Editor for a ComboBox (using ComboBoxEx control)
	/// </summary>
	public class EditorComboBox : EditorControlBase
	{
		#region Constructor
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		public EditorComboBox(Type p_Type):base(p_Type)
		{
		}

		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		/// <param name="p_StandardValues"></param>
		/// <param name="p_StandardValueExclusive">True to not allow custom value, only the values specified in the standardvalues collection are allowed.</param>
		public EditorComboBox(Type p_Type, ICollection p_StandardValues, bool p_StandardValueExclusive):base(p_Type)
		{
			StandardValues = p_StandardValues;
			StandardValuesExclusive = p_StandardValueExclusive;
		}
		#endregion

		#region Edit Control
		public override Control CreateEditorControl()
		{
			SourceLibrary.Windows.Forms.ComboBoxTyped l_ComboBox = new SourceLibrary.Windows.Forms.ComboBoxTyped();
			l_ComboBox.TextBox.BorderStyle = BorderStyle.None;
			return l_ComboBox;
		}

		[Obsolete("Use GetEditorComboBox(GridSubPanel)")]
		public virtual SourceLibrary.Windows.Forms.ComboBoxTyped GetEditorComboBox(GridVirtual p_Grid)
		{
			return (SourceLibrary.Windows.Forms.ComboBoxTyped)GetEditorControl(p_Grid);
		}

		public virtual SourceLibrary.Windows.Forms.ComboBoxTyped GetEditorComboBox(GridSubPanel p_GridSubPanel)
		{
			return (SourceLibrary.Windows.Forms.ComboBoxTyped)GetEditorControl(p_GridSubPanel);
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

			SourceLibrary.Windows.Forms.ComboBoxTyped l_Combo = GetEditorComboBox(p_Cell.Grid.PanelAtPosition(p_Position));

			if (p_StartEditValue is string && IsStringConversionSupported())
			{
				l_Combo.TextBox.Text = SourceLibrary.Windows.Forms.TextBoxTyped.ValidateCharactersString((string)p_StartEditValue, l_Combo.TextBox.ValidCharacters, l_Combo.TextBox.InvalidCharacters);
				if (l_Combo.TextBox.Text!=null)
					l_Combo.TextBox.SelectionStart = l_Combo.TextBox.Text.Length;
				else
					l_Combo.TextBox.SelectionStart = 0;
			}
			else
			{
				l_Combo.Value = p_Cell.GetValue(p_Position);
				l_Combo.SelectAllTextBox();
			}
		}

		/// <summary>
		/// Fired when the control is attached to a panel. Use this method to customize the control.
		/// </summary>
		/// <param name="e"></param>
		protected override void OnEditorAttached(ControlEventArgs e)
		{
			base.OnEditorAttached(e);

			SourceLibrary.Windows.Forms.ComboBoxTyped l_Combo = (SourceLibrary.Windows.Forms.ComboBoxTyped)e.Control;
			l_Combo.Validator = this;
			l_Combo.EnableEscapeKeyUndo = false;
			l_Combo.EnableEnterKeyValidate = false;
			l_Combo.EnableLastValidValue = false;
			l_Combo.EnableAutoValidation = false;
		}

		/// <summary>
		/// Returns the value inserted with the current editor control
		/// </summary>
		/// <returns></returns>
		public override object GetEditedValue()
		{
			return GetEditorComboBox(EditCell.Grid.PanelAtPosition(EditPosition)).Value;
		}
	}
}

