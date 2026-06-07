using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Allow to customize the cursor of a cell. The cell must implement ICellCursor. This behavior can be shared between multiple cells.
	/// </summary>
	public class Cursor : BehaviorModelGroup
	{
		public readonly static Cursor Default = new Cursor();

		#region IBehaviorModel Members

		public override void OnMouseEnter(PositionEventArgs e)
		{
			base.OnMouseEnter(e);

			ApplyCursor(e);
		}

		public override void OnMouseLeave(PositionEventArgs e)
		{
			base.OnMouseLeave(e);

			ResetCursor(e);
		}
		#endregion

		/// <summary>
		/// Change the cursor with the cursor of the cell
		/// </summary>
		/// <param name="e"></param>
		public virtual void ApplyCursor(PositionEventArgs e)
		{
			if (e.Cell is ICellCursor)
			{
				ICellCursor l_CellCursor = (ICellCursor)e.Cell;

				System.Windows.Forms.Cursor l_Cursor = l_CellCursor.GetCursor(e.Position);
				if (l_Cursor != null)
					e.Grid.GridCursor = l_Cursor;
				else
					e.Grid.GridCursor = System.Windows.Forms.Cursors.Default;
			}
		}

		/// <summary>
		/// Reset the original cursor
		/// </summary>
		/// <param name="e"></param>
		public virtual void ResetCursor(PositionEventArgs e)
		{
			if (e.Cell is ICellCursor)
			{
				e.Grid.GridCursor = System.Windows.Forms.Cursors.Default;
			}
		}
	}
}
