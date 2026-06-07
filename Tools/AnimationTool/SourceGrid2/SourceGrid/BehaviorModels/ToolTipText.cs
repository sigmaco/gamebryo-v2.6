using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Allow to customize the tooltiptext of a cell. This class read the tooltiptext from the ICellToolTipText.GetToolTipText.  This behavior can be shared between multiple cells.
	/// </summary>
	public class ToolTipText : BehaviorModelGroup
	{
		/// <summary>
		/// Default tooltiptext
		/// </summary>
		public readonly static ToolTipText Default = new ToolTipText();

		#region IBehaviorModel Members
		public override void OnMouseEnter(PositionEventArgs e)
		{
			base.OnMouseEnter(e);

			ApplyToolTipText(e);
		}

		public override void OnMouseLeave(PositionEventArgs e)
		{
			base.OnMouseLeave(e);

			ResetToolTipText(e);
		}
		#endregion

		/// <summary>
		/// Change the cursor with the cursor of the cell
		/// </summary>
		/// <param name="e"></param>
		protected virtual void ApplyToolTipText(PositionEventArgs e)
		{
			if (e.Cell is ICellToolTipText)
			{
				ICellToolTipText l_CellToolTip = (ICellToolTipText)e.Cell;
				string l_ToolTipText = l_CellToolTip.GetToolTipText(e.Position);
				if (l_ToolTipText != null && l_ToolTipText.Length > 0)
					e.Grid.GridToolTipText = l_ToolTipText;
			}
		}

		/// <summary>
		/// Reset the original cursor
		/// </summary>
		/// <param name="e"></param>
		protected virtual void ResetToolTipText(PositionEventArgs e)
		{
			e.Grid.GridToolTipText = null;
		}
	}
}
