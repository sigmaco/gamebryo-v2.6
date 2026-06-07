using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// Allow to customize the contextmenu of a cell. This class read the contextmenu from the ICellContextMenu.GetContextMenu.  This behavior can be shared between multiple cells.
	/// </summary>
	public class ContextMenu : BehaviorModelGroup
	{
		/// <summary>
		/// Default tooltiptext
		/// </summary>
		public readonly static ContextMenu Default = new ContextMenu();

		#region IBehaviorModel Members
		public override void OnContextMenuPopUp(PositionContextMenuEventArgs e)
		{
			base.OnContextMenuPopUp (e);
			if (e.Cell is ICellContextMenu)
			{
				ICellContextMenu l_ContextMenu = (ICellContextMenu)e.Cell;
				MenuCollection l_Menus = l_ContextMenu.GetContextMenu(e.Position);
				if (l_Menus != null && l_Menus.Count > 0)
				{
					if (e.ContextMenu.Count>0)
					{
						System.Windows.Forms.MenuItem l_menuBreak = new System.Windows.Forms.MenuItem();
						l_menuBreak.Text = "-";
						e.ContextMenu.Add(l_menuBreak);
					}

					foreach (System.Windows.Forms.MenuItem m in l_Menus)
						e.ContextMenu.Add(m);
				}
			}
		}
		#endregion
	}
}
