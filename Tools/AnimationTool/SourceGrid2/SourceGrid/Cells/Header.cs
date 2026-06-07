using System;
using System.Drawing;

namespace SourceGrid2.Cells.Virtual
{
	/// <summary>
	/// A cell that rappresent a header of a table, with 3D effect. This cell override IsSelectable to false. Default use VisualModels.VisualModelHeader.Style1
	/// </summary>
	public abstract class Header : CellVirtual
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public Header():this(VisualModels.Header.Default, BehaviorModels.Header.Default)
		{
		}
		/// <summary>
		/// Constructor
		/// </summary>
		public Header(VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_HeaderBehavior)
		{
			VisualModel = p_VisualModel;
			Behaviors.Add(p_HeaderBehavior);
		}
	}

}

namespace SourceGrid2.Cells.Real
{
	/// <summary>
	/// A cell that rappresent a header of a table, with 3D effect. This cell override IsSelectable to false. Default use VisualModels.VisualModelHeader.Style1
	/// </summary>
	public class Header : Cell
	{

		/// <summary>
		/// Constructor
		/// </summary>
		public Header():this(null)
		{
		}
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		public Header(object p_Value):this(p_Value, VisualModels.Header.Default, BehaviorModels.Header.Default)
		{
		}
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_HeaderBehavior"></param>
		public Header(object p_Value, VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_HeaderBehavior):base(p_Value)
		{
			VisualModel = p_VisualModel;
			Behaviors.Add(p_HeaderBehavior);
		}
	}

}
