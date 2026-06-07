using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Implements a behavior that cannot receive the focus. This behavior can be shared between multiple cells.
	/// </summary>
	public class Unselectable : BehaviorModelGroup
	{
		public readonly static Unselectable Default = new Unselectable();

		public override void OnFocusEntering(PositionCancelEventArgs e)
		{
			base.OnFocusEntering (e);

			e.Cancel = !CanReceiveFocus;
		}
		public override bool CanReceiveFocus
		{
			get{return false;}
		}
	}
}
