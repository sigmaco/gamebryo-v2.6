using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// A behavior that invalidate the cell when the user click on it and make the cell unselectable. This behavior can be shared between multiple cells.
	/// </summary>
	public class Button : BehaviorModelGroup
	{
		/// <summary>
		/// Default implementation.
		/// </summary>
		public readonly static Button Default = new Button();

		/// <summary>
		/// Constructor
		/// </summary>
		public Button():this(Unselectable.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_BehaviorUnselectable"></param>
		public Button(IBehaviorModel p_BehaviorUnselectable)
		{
			SubModels.Add(p_BehaviorUnselectable);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public override void OnMouseDown(PositionMouseEventArgs e)
		{
			base.OnMouseDown (e);

			e.Grid.InvalidateCell(e.Position);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public override void OnMouseUp(PositionMouseEventArgs e)
		{
			base.OnMouseUp (e);

			e.Grid.InvalidateCell(e.Position);
		}
	}
}
