using System;

namespace SourceGrid2
{
	/// <summary>
	/// A class derived from ContextMenu but that is syncronized with the grid using the ContextMenuStyle property
	/// </summary>
#if !MINI
	[System.ComponentModel.ToolboxItem(false)]
#endif
	public class GridContextMenu : System.Windows.Forms.ContextMenu
	{
		private GridVirtual m_Grid;
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Grid">The grid to sync with</param>
		public GridContextMenu(GridVirtual p_Grid)
		{
			m_Grid = p_Grid;
		}

		/// <summary>
		/// Grid to sync
		/// </summary>
		public GridVirtual Grid
		{
			get{return m_Grid;}
		}

		/// <summary>
		/// Fired when the contextmenu is showed
		/// </summary>
		/// <param name="e"></param>
		protected override void OnPopup(EventArgs e)
		{
			this.MenuItems.Clear();

			base.OnPopup (e);

			MenuCollection l_Menus = m_Grid.GetGridContextMenus();
			for (int i = 0; i < l_Menus.Count; i++)
			{
				MenuItems.Add(l_Menus[i]);
			}
		}

	}
}
