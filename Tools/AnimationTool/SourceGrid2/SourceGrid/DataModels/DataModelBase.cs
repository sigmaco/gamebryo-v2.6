using System;
using System.Windows.Forms;

namespace SourceGrid2.DataModels
{
	/// <summary>
	/// Represents the base class of a DataModel. This DataModel support conversion but doesn't provide any user interface editor.
	/// </summary>
	public class DataModelBase : SourceLibrary.ComponentModel.Validator.ValidatorTypeConverter, IDataModel
	{
		#region Constructor
		/// <summary>
		/// Construct a Model. Based on the Type specified the constructor populate StringEditor property
		/// </summary>
		/// <param name="p_Type">The type of this model</param>
		public DataModelBase(Type p_Type):base(p_Type)
		{
		}
		#endregion
		
		#region Edit coordinates
		//Queste variabili vengono usato durante le fasi di editing
		private Cells.ICellVirtual m_EditCell;
		private Position m_EditPosition = Position.Empty;
		/// <summary>
		/// Cell in editing, if null no cell is in editing state
		/// </summary>
		public Cells.ICellVirtual EditCell
		{
			get{return m_EditCell;}
		}
		/// <summary>
		/// Cell in editing, if Empty no cell is in editing state
		/// </summary>
		public Position EditPosition
		{
			get{return m_EditPosition;}
		}

		/// <summary>
		/// Set the current editing cell, for an editor only one cell can be in editing state
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position"></param>
		protected void SetEditCell(Cells.ICellVirtual p_Cell, Position p_Position)
		{
			m_EditCell = p_Cell;
			m_EditPosition = p_Position;
		}
		#endregion

		#region ErrorString
		/// <summary>
		/// Error rappresentation
		/// </summary>
		private string m_ErrorString = "#ERROR!";

		/// <summary>
		/// Returns true if the string passed is equal to the error string rappresentation
		/// </summary>
		/// <param name="p_str"></param>
		/// <returns></returns>
		public bool IsErrorString(string p_str)
		{
			if (p_str == ErrorString)
				return true;
			else
				return false;
		}
		/// <summary>
		/// String used when error occurred
		/// </summary>
		public string ErrorString
		{
			get{return m_ErrorString;}
			set{m_ErrorString = value;}
		}
		#endregion

		#region Editable settings
		private bool m_bEnableEdit = true;
		/// <summary>
		/// Enable or disable the cell editor (if disable no visual edit is allowed)
		/// </summary>
		public bool EnableEdit
		{
			get{return m_bEnableEdit;}
			set{m_bEnableEdit = value;}
		}

		private EditableMode m_EditableMode = EditableMode.Default;
		/// <summary>
		/// Mode to edit the cell.
		/// </summary>
		public EditableMode EditableMode
		{
			get{return m_EditableMode;}
			set{m_EditableMode = value;}
		}

		private bool m_bEnableCellDrawOnEdit = true;
		/// <summary>
		/// Indicates if the draw of the cell when in editing mode is enabled.
		/// </summary>
		public virtual bool EnableCellDrawOnEdit
		{
			get{return m_bEnableCellDrawOnEdit;}
			set{m_bEnableCellDrawOnEdit = value;}
		}

		#endregion

		#region StartEdit/EndEdit/IsEditing/ApplyEdit/GetEditedValue
		/// <summary>
		/// Indicates if the current editor is in editing state
		/// </summary>
		public bool IsEditing
		{
			get{return (m_EditCell!=null);}
		}

		/// <summary>
		/// Start editing the cell passed. Do not call this method for start editing a cell, you must use Cell.StartEdit.
		/// </summary>
		/// <param name="p_Cell">Cell to start edit</param>
		/// <param name="p_Position">Editing position(Row/Col)</param>
		/// <param name="p_StartEditValue">Can be null(in this case use the p_cell.Value</param>
		public virtual void InternalStartEdit(Cells.ICellVirtual p_Cell, Position p_Position, object p_StartEditValue)
		{
			//no edit supported for this editor
		}


		/// <summary>
		/// Apply edited value
		/// </summary>
		/// <returns></returns>
		public virtual bool InternalApplyEdit()
		{
			return true;
		}
		/// <summary>
		/// Cancel the edit action
		/// </summary>
		/// <param name="p_Cancel">True to cancel the editing and return to normal mode, false to call automatically ApplyEdit and terminate editing</param>
		/// <returns>Returns true if the cell terminate the editing mode</returns>
		public virtual bool InternalEndEdit(bool p_Cancel)
		{
			return true;
		}


		/// <summary>
		/// Returns the new value edited with the custom control
		/// </summary>
		/// <returns></returns>
		public virtual object GetEditedValue()
		{
			throw new SourceGridException("No valid cell editor found");
		}
	
		#endregion

		#region ClearCell/SetCellValue
		/// <summary>
		/// Clear the value of the cell using the default value
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position">Cell position</param>
		public virtual void ClearCell(Cells.ICellVirtual p_Cell, Position p_Position)
		{
			SetCellValue(p_Cell, p_Position, DefaultValue);
		}

		/// <summary>
		/// Change the value of the cell applying the rule of the current editor. Is recommend to use this method to simulate a edit operation and to validate the cell value using the current model.
		/// </summary>
		/// <param name="p_Cell">Cell to change value</param>
		/// <param name="p_Position">Current Cell Position</param>
		/// <param name="p_NewValue"></param>
		/// <returns>returns true if the value passed is valid and has been applied to the cell</returns>
		public virtual bool SetCellValue(Cells.ICellVirtual p_Cell, Position p_Position, object p_NewValue)
		{
			if (EnableEdit)
			{
				ValidatingCellEventArgs l_cancelEvent = new ValidatingCellEventArgs(p_Cell, p_NewValue);
				OnValidating(l_cancelEvent);

				//check if cancel == true 
				if (l_cancelEvent.Cancel == false)
				{
					object l_PrevValue = p_Cell.GetValue(p_Position);
					try
					{
						p_Cell.SetValue(p_Position, ObjectToValue(l_cancelEvent.NewValue));
						OnValidated(new CellEventArgs(p_Cell));
					}
					catch(Exception err)
					{
						OnEditException(new EditExceptionEventArgs(err));
						p_Cell.SetValue(p_Position, l_PrevValue);
						l_cancelEvent.Cancel = true;//di fatto è fallita la validazione del dato
					}
				}

				return (l_cancelEvent.Cancel==false);
			}
			else
				return false;
		}

		#endregion

		#region Validating
		/// <summary>
		/// Functions used when the validating operation is finished
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnValidated(CellEventArgs e)
		{
			if (m_Validated!=null)
				m_Validated(this,e);
		}
		/// <summary>
		/// Validating the value of the cell.
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnValidating(ValidatingCellEventArgs e)
		{
			if (m_Validating!=null)
				m_Validating(this,e);
		}

		private event ValidatingCellEventHandler m_Validating;
		private event CellEventHandler m_Validated;
		/// <summary>
		/// Validating event
		/// </summary>
		public event ValidatingCellEventHandler Validating
		{
			add{m_Validating+=value;}
			remove{m_Validating-=value;}
		}
		/// <summary>
		/// Validated event
		/// </summary>
		public event CellEventHandler Validated
		{
			add{m_Validated+=value;}
			remove{m_Validated-=value;}
		}

		/// <summary>
		/// Event fired when an exception is throw in the Validated event or in an editing method
		/// </summary>
		public EditExceptionEventHandler EditException;

		/// <summary>
		/// Event fired when an exception is throw in the Validated event or in an editing method
		/// </summary>
		/// <param name="e"></param>
		protected virtual void OnEditException(EditExceptionEventArgs e)
		{
#if DEBUG
			System.Diagnostics.Debug.WriteLine("Exception on editing cell: " + e.Exception.ToString());
#endif

			if (EditException!=null)
				EditException(this, e);
		}
		#endregion
	}
}
