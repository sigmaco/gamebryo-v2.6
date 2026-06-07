using System;
using System.Windows.Forms;


namespace SourceGrid2.Cells.Virtual
{
	/// <summary>
	/// A cell that rappresent a button 
	/// </summary>
	public abstract class Button : CellVirtual
	{
		/// <summary>
		/// Constructor. Using VisualModels.Header.Default and BehaviorModels.Button.Default
		/// </summary>
		public Button():this(VisualModels.Header.Default, BehaviorModels.Button.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Click"></param>
		public Button(PositionEventHandler p_Click):this()
		{
			if (p_Click!=null)
				Click += p_Click;
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_BehaviorButton"></param>
		public Button(VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_BehaviorButton)
		{
			Behaviors.Add(p_BehaviorButton);
			VisualModel = p_VisualModel;
		}

		public event PositionEventHandler Click;

		public override void OnClick(PositionEventArgs e)
		{
			base.OnClick (e);

			if (Click!=null)
				Click(this, e);
		}
	}
}

namespace SourceGrid2.Cells.Real
{
	/// <summary>
	/// A cell that rappresent a button 
	/// </summary>
	public class Button : Cell
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_Click"></param>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_BehaviorButton"></param>
		public Button(object p_Value, PositionEventHandler p_Click, VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_BehaviorButton):base(p_Value)
		{
			if (p_Click!=null)
				Click += p_Click;

			Behaviors.Add(p_BehaviorButton);
			VisualModel = p_VisualModel;
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_Click"></param>
		public Button(object p_Value, PositionEventHandler p_Click):this(p_Value, p_Click, VisualModels.Header.Default, BehaviorModels.Button.Default)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		public Button(object p_Value):this(p_Value, null)
		{
		}

		/// <summary>
		/// Click event
		/// </summary>
		public event PositionEventHandler Click;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public override void OnClick(PositionEventArgs e)
		{
			base.OnClick (e);

			if (Click!=null)
				Click(this, e);
		}
	}
}
