using System;

namespace SourceGrid2.Cells
{
	/// <summary>
	/// Interface for informations about a cursor
	/// </summary>
	public interface ICellCursor
	{
		/// <summary>
		/// Get the cursor of the specified cell
		/// </summary>
		/// <param name="p_Position"></param>
		System.Windows.Forms.Cursor GetCursor(Position p_Position);
	}
}
