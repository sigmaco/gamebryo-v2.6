using System;
using System.Drawing;
using System.Windows.Forms;

namespace SourceGrid2.Cells.Virtual
{
	/// <summary>
	/// A cell that contains a HTML style link. Use the click event to execute the link
	/// </summary>
	public abstract class Link : CellVirtual, ICellCursor
	{
		/// <summary>
		/// Constructor, using VisualModels.Common.LinkStyle and BehaviorModels.Cursor.Default
		/// </summary>
		public Link():
			this(VisualModels.Common.LinkStyle, 
#if !MINI
				BehaviorModels.Cursor.Default
#else
				null
#endif
			)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_LinkBehavior"></param>
		public Link(VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_LinkBehavior)
		{
			VisualModel = p_VisualModel;
			Behaviors.Add(p_LinkBehavior);
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_ExecuteLink">Event to execute when the user Click on this cell</param>
		public Link(PositionEventHandler p_ExecuteLink):this()
		{
			if (p_ExecuteLink!=null)
				Click+=p_ExecuteLink;
		}

		public event PositionEventHandler Click;

		public override void OnClick(PositionEventArgs e)
		{
			base.OnClick (e);

			if (Click!=null)
				Click(this, e);
		}

		/// <summary>
		/// Get the cursor of the specified cell
		/// </summary>
		/// <param name="p_Position"></param>
		public System.Windows.Forms.Cursor GetCursor(Position p_Position)
		{
#if !MINI
			return System.Windows.Forms.Cursors.Hand;
#else
			return System.Windows.Forms.Cursors.Default;
#endif
		}
	}
}

namespace SourceGrid2.Cells.Real
{
	/// <summary>
	/// A cell that contains a HTML style link. Use the click event to execute the link
	/// </summary>
	public class Link : Cell
	{

		/// <summary>
		/// Constructor using VisualModels.Common.LinkStyle and BehaviorModels.Cursor.Default
		/// </summary>
		/// <param name="p_Value"></param>
		public Link(object p_Value):
			this(p_Value, 
			VisualModels.Common.LinkStyle, 
#if !MINI
			BehaviorModels.Cursor.Default
#else
			null
#endif
			)
		{
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_ExecuteLink">Event to execute when the user Click on this cell</param>
		public Link(object p_Value, PositionEventHandler p_ExecuteLink):this(p_Value)
		{
			if (p_ExecuteLink!=null)
				Click+=p_ExecuteLink;
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_VisualModel"></param>
		/// <param name="p_BehaviorLink"></param>
		public Link(object p_Value, VisualModels.IVisualModel p_VisualModel, BehaviorModels.IBehaviorModel p_BehaviorLink):base(p_Value)
		{
			VisualModel = p_VisualModel;
			Behaviors.Add(p_BehaviorLink);

#if !MINI
			Cursor =  System.Windows.Forms.Cursors.Hand;
#endif
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
