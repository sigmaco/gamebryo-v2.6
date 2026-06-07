using System;
using System.Drawing;

namespace SourceGrid2.Cells.Virtual
{
	/// <summary>
	/// Class that rapresent a row header with resize feature.
	/// </summary>
	public abstract class RowHeader : Header
	{
		/// <summary>
		/// Constructor using VisualModels.Header.RowHeader and BehaviorModels.RowHeader.Default
		/// </summary>
		public RowHeader():base(VisualModels.Header.RowHeader, BehaviorModels.RowHeader.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_BehaviorHeader"></param>
		public RowHeader(VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_BehaviorHeader)
			:base(p_VisualModel, p_BehaviorHeader)
		{
		}
	}

}


namespace SourceGrid2.Cells.Real
{
	/// <summary>
	/// Class that rapresent a row header with resize feature.
	/// </summary>
	public class RowHeader : Header
	{
		/// <summary>
		/// Constructor
		/// </summary>
		public RowHeader():this(null)
		{
		}

		/// <summary>
		/// Constructor using VisualModels.Header.RowHeader and BehaviorModels.RowHeader.Default
		/// </summary>
		/// <param name="p_Value"></param>
		public RowHeader(object p_Value):base(p_Value, VisualModels.Header.RowHeader, BehaviorModels.RowHeader.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_HeaderBehavior"></param>
		public RowHeader(object p_Value, VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_HeaderBehavior)
				:base(p_Value, p_VisualModel, p_HeaderBehavior)
		{
		}
	}
}
