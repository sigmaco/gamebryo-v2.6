using System;
using System.Windows.Forms;

namespace SourceGrid2.DataModels
{
	/// <summary>
	/// Summary description for NumericUpDownEditor.
	/// </summary>
	public class EditorNumericUpDown : EditorControlBase
	{
		private decimal m_Maximum = 100;
		private decimal m_Minimum = 0;
		private decimal m_Increment = 1;

		/// <summary>
		/// Create a model of type Decimal
		/// </summary>
		public EditorNumericUpDown():base(typeof(decimal))
		{
		}
		
		public EditorNumericUpDown(Type p_CellType, decimal p_Maximum, decimal p_Minimum, decimal p_Increment):base(p_CellType)
		{
			if (p_CellType==null || p_CellType == typeof(int) ||
				p_CellType == typeof(long) || p_CellType == typeof(decimal))
			{
				m_Maximum = p_Maximum;
				m_Minimum = p_Minimum;
				m_Increment = p_Increment;
			}
			else
				throw new SourceGridException("Invalid CellType expected long, int or decimal");
		}

		#region Edit Control
		public override Control CreateEditorControl()
		{
			System.Windows.Forms.NumericUpDown l_Control = new System.Windows.Forms.NumericUpDown();
#if !MINI
			l_Control.BorderStyle = System.Windows.Forms.BorderStyle.None;
#endif
			return l_Control;
		}

		[Obsolete("Use GetEditorNumericUpDown(GridSubPanel)")]
		public virtual System.Windows.Forms.NumericUpDown GetEditorNumericUpDown(GridVirtual p_Grid)
		{
			return (System.Windows.Forms.NumericUpDown)GetEditorControl(p_Grid);
		}

		public virtual System.Windows.Forms.NumericUpDown GetEditorNumericUpDown(GridSubPanel p_GridSubPanel)
		{
			return (System.Windows.Forms.NumericUpDown)GetEditorControl(p_GridSubPanel);
		}
		#endregion

		/// <summary>
		/// Start editing the cell passed
		/// </summary>
		/// <param name="p_Cell">Cell to start edit</param>
		/// <param name="p_Position">Editing position(Row/Col)</param>
		/// <param name="p_StartEditValue">Can be null(in this case use the p_cell.Value</param>
		public override void InternalStartEdit(Cells.ICellVirtual p_Cell, Position p_Position, object p_StartEditValue)
		{
			base.InternalStartEdit(p_Cell, p_Position, p_StartEditValue);

			if (EnableEdit==false)
				return;

			System.Windows.Forms.NumericUpDown l_Control = GetEditorNumericUpDown(p_Cell.Grid.PanelAtPosition(p_Position));

			if (p_StartEditValue != null)
			{
				if ( IsValidObject(p_StartEditValue) )
					SetValueToControl( ObjectToValue(p_StartEditValue) );
			}
			else
			{
				SetValueToControl(p_Cell.GetValue(p_Position));
			}
		}

		/// <summary>
		/// Fired when the control is attached to a panel. Use this method to customize the control.
		/// </summary>
		/// <param name="e"></param>
		protected override void OnEditorAttached(ControlEventArgs e)
		{
			base.OnEditorAttached(e);

			System.Windows.Forms.NumericUpDown l_Control = (System.Windows.Forms.NumericUpDown)e.Control;

			l_Control.Maximum = m_Maximum;
			l_Control.Minimum = m_Minimum;
			l_Control.Increment = m_Increment;
		}

		public decimal Maximum
		{
			get{return m_Maximum;}
			set{m_Maximum = value;}
		}
		public decimal Minimum
		{
			get{return m_Minimum;}
			set{m_Minimum = value;}
		}
		public decimal Increment
		{
			get{return m_Increment;}
			set{m_Increment = value;}
		}

		/// <summary>
		/// Returns the value inserted with the current editor control
		/// </summary>
		/// <returns></returns>
		public override object GetEditedValue()
		{
			return GetValueFromControl();
		}

		private object GetValueFromControl()
		{
			if (ValueType == null)
				return GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value;
			if (ValueType == typeof(decimal))
				return GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value;
			if (ValueType == typeof(int))
				return (int)(GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value);
			if (ValueType == typeof(long))
				return (long)(GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value);

			throw new SourceGridException("Invalid type of the cell expected decimal, long or int");
		}

		private void SetValueToControl(object p_Value)
		{
			if (p_Value is decimal)
				GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value = (decimal)p_Value;
			else if (p_Value is long)
				GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value = (decimal)((long)p_Value);
			else if (p_Value is int)
				GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value = (decimal)((int)p_Value);
			else if (p_Value == null)
				GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Value = GetEditorNumericUpDown(EditCell.Grid.PanelAtPosition(EditPosition)).Minimum;
			else
				throw new SourceGridException("Invalid value, expected Decimal, Int or Long");
		}
	}
}
