using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace SourceGrid2.Cells
{
	/// <summary>
	/// Interface to represents a cell virtual (without position or value information).
	/// </summary>
	public interface ICellVirtual
	{
		#region LinkToGrid
		/// <summary>
		/// The Grid object
		/// </summary>
		GridVirtual Grid
		{
			get;
		}

		/// <summary>
		/// Link the cell at the specified grid.
		/// </summary>
		/// <param name="p_grid"></param>
		void BindToGrid(GridVirtual p_grid);

		/// <summary>
		/// Remove the link of the cell from the previous grid.
		/// </summary>
		void UnBindToGrid();
		#endregion

		#region GetValue, SetValue, GetDisplayString

		/// <summary>
		/// Get the value of the cell at the specified position
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		object GetValue(Position p_Position);

		/// <summary>
		/// Set the value of the cell at the specified position. This method must call OnValueChanged() event.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_Value"></param>
		void SetValue(Position p_Position, object p_Value);

		/// <summary>
		/// The string representation of the Cell.GetValue method (default Value.ToString())
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		string GetDisplayText(Position p_Position);
		#endregion

		#region VisualProperties
		/// <summary>
		/// Visual properties of this cell and other cell. You can share the VisualProperties between many cell to optimize memory size.
		/// Warning Changing this property can affect many cells
		/// </summary>
		[Browsable(false)]
		VisualModels.IVisualModel VisualModel
		{
			get;
			set;
		}
		#endregion

		#region CanReceiveFocus
		/// <summary>
		/// True if the cell can have the focus otherwise false. This method simply call BehaviorModel.CanReceiveFocus.
		/// </summary>
		bool CanReceiveFocus
		{
			get;
		}
		#endregion

		#region Measure (Calculate Required Size)
		/// <summary>
		/// If the cell is not linked to a grid the result is not accurate (Font can be null). Call InternalGetRequiredSize with RowSpan and ColSpan = 1.
		/// </summary>
		/// <param name="p_Position">Position of the current cell</param>
		/// <param name="g"></param>
		/// <param name="maxLayoutArea">SizeF structure that specifies the maximum layout area for the text. If width or height are zero the value is set to a default maximum value.</param>
		/// <returns></returns>
		Size Measure(Position p_Position, Graphics g, SizeF maxLayoutArea);
		#endregion

		#region Editing

		/// <summary>
		/// Editor of this cell and others cells. If null no edit is supported. 
		///  You can share the same model between many cells to optimize memory size. Warning Changing this property can affect many cells
		/// </summary>
		DataModels.IDataModel DataModel
		{
			get;
			set;
		}

		/// <summary>
		/// Start the edit operation with the current editor specified in the Model property.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <param name="p_NewStartEditValue">The value that the editor receive. Null to use the Value of the Cell.</param>
		void StartEdit(Position p_Position, object p_NewStartEditValue);

		/// <summary>
		/// Terminate the edit operation
		/// </summary>
		/// <param name="p_bCancel">If true undo all the changes</param>
		/// <returns>Returns true if the edit operation is successfully terminated, otherwise false</returns>
		bool EndEdit(bool p_bCancel);

		/// <summary>
		/// True if this cell is currently in edit state, otherwise false.
		/// </summary>
		/// <param name="p_Position"></param>
		/// <returns></returns>
		bool IsEditing(Position p_Position);

		#endregion

		#region Events
		/// <summary>
		/// Fired when a context menu is showed
		/// </summary>
		/// <param name="e"></param>
		void OnContextMenuPopUp(PositionContextMenuEventArgs e);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnMouseDown(PositionMouseEventArgs e);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnMouseUp(PositionMouseEventArgs e);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnMouseMove(PositionMouseEventArgs e);

#if !MINI
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnMouseEnter(PositionEventArgs e);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnMouseLeave(PositionEventArgs e);
#endif
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnKeyUp ( PositionKeyEventArgs e);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnKeyDown ( PositionKeyEventArgs e);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnKeyPress ( PositionKeyPressEventArgs e);

#if !MINI
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnDoubleClick (PositionEventArgs e);
#endif

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		void OnClick (PositionEventArgs e);
		/// <summary>
		/// Fired before the cell leave the focus, you can put the e.Cancel = true to cancel the leave operation.
		/// </summary>
		/// <param name="e"></param>
		void OnFocusLeaving(PositionCancelEventArgs e);
		/// <summary>
		/// Fired when the cell has left the focus.
		/// </summary>
		/// <param name="e"></param>
		void OnFocusLeft(PositionEventArgs e);
		/// <summary>
		/// Fired when the focus is entering in the specified cell. You can put the e.Cancel = true to cancel the focus operation.
		/// </summary>
		/// <param name="e"></param>
		void OnFocusEntering(PositionCancelEventArgs e);
		/// <summary>
		/// Fired when the focus enter in the specified cell.
		/// </summary>
		/// <param name="e"></param>
		void OnFocusEntered(PositionEventArgs e);
		/// <summary>
		/// Fired when the SetValue method is called.
		/// </summary>
		/// <param name="e"></param>
		void OnValueChanged(PositionEventArgs e);

		/// <summary>
		/// Fired when the StartEdit is called. You can set the Cancel = true to stop editing.
		/// </summary>
		/// <param name="e"></param>
		void OnEditStarting(PositionCancelEventArgs e);
		/// <summary>
		/// Fired when the EndEdit is called. You can read the Cancel property to determine if the edit is completed. If you change the cancel property there is no effect.
		/// </summary>
		/// <param name="e"></param>
		void OnEditEnded(PositionCancelEventArgs e);
		#endregion

		#region BehaviorModel
		/// <summary>
		/// Collection of BehaviorModel. Represents the actions of a cell.
		/// </summary>
		BehaviorModels.BehaviorModelCollection Behaviors
		{
			get;
		}
		#endregion

		#region Invalidate
		/// <summary>
		/// Invalidate the specified position
		/// </summary>
		/// <param name="p_Position"></param>
		void Invalidate(Position p_Position);
		#endregion
	}
}
