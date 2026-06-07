using System;

namespace SourceGrid2.Cells
{
	/// <summary>
	/// Interface for informations about a contextmenu
	/// </summary>
	public interface ICellContextMenu
	{
		/// <summary>
		/// Get the contextmenu of the specified cell
		/// </summary>
		/// <param name="p_Position"></param>
		MenuCollection GetContextMenu(Position p_Position);
	}
}
