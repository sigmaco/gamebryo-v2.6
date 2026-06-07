using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Summary description for BehaviorModelCheckBox. This behavior can be shared between multiple cells.
	/// </summary>
	public class CheckBox : BehaviorModelGroup
	{
		/// <summary>
		/// Default behavior checkbox
		/// </summary>
		public readonly static CheckBox Default = new CheckBox();

		/// <summary>
		/// Constructor
		/// </summary>
		public CheckBox()
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_bAutoChangeValueOfSelectedCells">Indicates if this cells when checked or uncheck must change also the value of the selected cells of type CellCheckBox.</param>
		public CheckBox(bool p_bAutoChangeValueOfSelectedCells)
		{
			m_bAutoChangeValueOfSelectedCells = p_bAutoChangeValueOfSelectedCells;
		}

		#region IBehaviorModel Members
#if !MINI
		public override void OnMouseEnter(PositionEventArgs e)
		{
			base.OnMouseEnter(e);

			e.Cell.Invalidate(e.Position);//Invalidate the cell to refresh the checkbox
		}

		public override void OnMouseLeave(PositionEventArgs e)
		{
			base.OnMouseLeave(e);

			e.Cell.Invalidate(e.Position);//Invalidate the cell to refresh the checkbox
		}
#endif

		public override void OnKeyPress(PositionKeyPressEventArgs e)
		{
			base.OnKeyPress(e);

			if (e.KeyPressEventArgs.KeyChar == ' ')
				UIChangeChecked(e);
		}

		public override void OnClick(PositionEventArgs e)
		{
			base.OnClick(e);

			UIChangeChecked(e);
		}

		#endregion

		private bool m_bAutoChangeValueOfSelectedCells = false;
		/// <summary>
		/// Indicates if this cells when checked or uncheck must change also the value of the selected cells of type CellCheckBox. Default is false
		/// </summary>
		public bool AutoChangeValueOfSelectedCells
		{
			get{return m_bAutoChangeValueOfSelectedCells;}
		}

		/// <summary>
		/// Toggle the value of the current cell and if AutoChangeValueOfSelectedCells is true of all the selected cells
		/// </summary>
		/// <param name="e"></param>
		private void UIChangeChecked(PositionEventArgs e)
		{
			ICellCheckBox l_Check = (ICellCheckBox)e.Cell;
			if (l_Check.GetCheckBoxStatus(e.Position).CheckEnable)
			{
				bool l_NewVal = !l_Check.GetCheckedValue(e.Position);
				l_Check.SetCheckedValue(e.Position, l_NewVal);

				//change the status of all selected control
				if (AutoChangeValueOfSelectedCells)
				{
					foreach(Position pos in e.Cell.Grid.Selection.GetCellsPositions())
					{
						Cells.ICellVirtual c = e.Grid.GetCell(pos);
						if (c != this && c != null && c is ICellCheckBox)
						{
							ICellCheckBox cCh = (ICellCheckBox)c;
							cCh.SetCheckedValue(pos, l_NewVal);
						}
					}
				}
			}
		}

	}
}
