using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing.Design;

namespace SourceGrid2.DataModels
{
	/// <summary>
	///  A model that use a UITypeEditor to edit the cell.
	/// </summary>
	public class EditorUITypeEditor : EditorTextBoxButton
	{
		#region Constructor
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		public EditorUITypeEditor(Type p_Type):base(p_Type)
		{
			object l_Editor = System.ComponentModel.TypeDescriptor.GetEditor(p_Type,typeof(System.Drawing.Design.UITypeEditor));
			if (l_Editor == null)
				throw new SourceGridException("Type not valid, no editor associated to this type");
			m_UITypeEditor = (System.Drawing.Design.UITypeEditor)l_Editor;
		}
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate AllowNull, DefaultValue, TypeConverter, StandardValues, StandardValueExclusive
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		/// <param name="p_UITypeEditor"></param>
		public EditorUITypeEditor(Type p_Type, UITypeEditor p_UITypeEditor):base(p_Type)
		{
			m_UITypeEditor = p_UITypeEditor;
		}
		#endregion

		#region Edit Control
		public override Control CreateEditorControl()
		{
			SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor l_ComboBox = new SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor();
			l_ComboBox.TextBox.BorderStyle = BorderStyle.None;
			return l_ComboBox;
		}

		[Obsolete("Use GetEditorTextBoxButtonUITypeEditor(GridSubPanel)")]
		public virtual SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor GetEditorTextBoxButtonUITypeEditor(GridVirtual p_Grid)
		{
			return (SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor)GetEditorControl(p_Grid);
		}

		public virtual SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor GetEditorTextBoxButtonUITypeEditor(GridSubPanel p_GridPanel)
		{
			return (SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor)GetEditorControl(p_GridPanel);
		}
		#endregion

		private UITypeEditor m_UITypeEditor;
		public UITypeEditor UITypeEditor
		{
			get{return m_UITypeEditor;}
			set{m_UITypeEditor = value;}
		}

		public override void InternalStartEdit(SourceGrid2.Cells.ICellVirtual p_Cell, Position p_Position, object p_StartEditValue)
		{
			base.InternalStartEdit (p_Cell, p_Position, p_StartEditValue);
		}

		/// <summary>
		/// Fired when the control is attached to a panel. Use this method to customize the control.
		/// </summary>
		/// <param name="e"></param>
		protected override void OnEditorAttached(ControlEventArgs e)
		{
			base.OnEditorAttached(e);

			SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor control = (SourceLibrary.Windows.Forms.TextBoxButtonUITypeEditor)e.Control;
			control.UITypeEditor = m_UITypeEditor;
		}
	}
}
