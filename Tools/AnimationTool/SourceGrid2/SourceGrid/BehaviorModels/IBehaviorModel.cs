using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Represents a behavior of a cell.
	/// </summary>
	public interface IBehaviorModel
	{
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
		/// <summary>
		/// Returns true if the current cell can receive the focus. If only one behavior return false the return value is false.
		/// </summary>
		bool CanReceiveFocus
		{
			get;
		}
	}
}
