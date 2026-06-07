using System;
using System.Windows.Forms;

namespace SourceGrid2.DataModels
{
	/// <summary>
	/// Class used for editing operation, string conversion and value formatting
	/// </summary>
	public interface IDataModel : SourceLibrary.ComponentModel.Validator.IValidator
	{
		#region Editing
		/// <summary>
		/// Cell in editing, if null no cell is in editing state
		/// </summary>
		Cells.ICellVirtual EditCell
		{
			get;
		}
		/// <summary>
		/// Cell in editing, if Empty no cell is in editing state
		/// </summary>
		Position EditPosition
		{
			get;
		}

		/// <summary>
		/// Start editing the cell passed. Do not call this method for start editing a cell, you must use Cell.StartEdit. For internal use only, use Cell.StartEdit.
		/// </summary>
		/// <param name="p_Cell">Cell to start edit</param>
		/// <param name="p_Position">Editing position(Row/Col)</param>
		/// <param name="p_StartEditValue">Can be null(in this case use the p_cell.Value</param>
		void InternalStartEdit(Cells.ICellVirtual p_Cell, Position p_Position, object p_StartEditValue);

//		/// <summary>
//		/// Apply the edit action to the cell, this method doesn't finish the edit action. For internal use only.
//		/// </summary>
//		/// <returns>True if the new value is inserted in the cell</returns>
//		bool InternalApplyEdit();
		/// <summary>
		/// Terminate the edit action. For internal use only, use Cell.EndEdit.
		/// </summary>
		/// <param name="p_Cancel">True to cancel the editing and return to normal mode, false to call automatically ApplyEdit and terminate editing</param>
		/// <returns>Returns true if the cell terminate the editing mode</returns>
		bool InternalEndEdit(bool p_Cancel);


		/// <summary>
		/// Returns true if the cell is in editing state
		/// </summary>
		bool IsEditing
		{
			get;
		}

		/// <summary>
		/// Enable or disable the cell editor (if disable no edit is allowed). If false also not UI editing are blocked.
		/// </summary>
		bool EnableEdit
		{
			get;
			set;
		}

		/// <summary>
		/// Mode to edit the cell.
		/// </summary>
		EditableMode EditableMode
		{
			get;
			set;
		}

		/// <summary>
		/// Indicates if the draw of the cell when in editing mode is enabled.
		/// </summary>
		bool EnableCellDrawOnEdit
		{
			get;
		}
		#endregion

		#region Modify Functions
		/// <summary>
		/// Clear the value of the cell using the default value
		/// </summary>
		/// <param name="p_Cell"></param>
		/// <param name="p_Position">Cell position</param>
		void ClearCell(Cells.ICellVirtual p_Cell, Position p_Position);

		/// <summary>
		/// Change the value of the cell applying the rule of the current editor. Is recommend to use this method to simulate a edit operation and to validate the cell value using the current model.
		/// </summary>
		/// <param name="p_Cell">Cell to change value</param>
		/// <param name="p_Position">Current Cell Position</param>
		/// <param name="p_NewValue"></param>
		/// <returns>returns true if the value passed is valid and has been applied to the cell</returns>
		bool SetCellValue(Cells.ICellVirtual p_Cell, Position p_Position, object p_NewValue);

		#endregion

		#region Validating
		/// <summary>
		/// Fired to check if the value specified by the user is allowed
		/// this event is fired after the ValidatingValue (use ValidatingValue to check if the value is compatible with the cell)
		/// </summary>
		event ValidatingCellEventHandler Validating;
		/// <summary>
		/// Fired after the value specified by the user inserited in the cell
		/// </summary>
		event CellEventHandler Validated;
		#endregion

		#region Conversion
		/// <summary>
		/// Check if the given string is error
		/// </summary>
		/// <param name="p_str"></param>
		/// <returns></returns>
		bool IsErrorString(string p_str);
		#endregion
	}
}
