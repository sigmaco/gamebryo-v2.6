using System;

namespace SourceGrid2.Cells
{
	/// <summary>
	/// Interface for informations about a tooltiptext
	/// </summary>
	public interface ICellToolTipText
	{
		/// <summary>
		/// Get the tooltiptext of the specified cell
		/// </summary>
		/// <param name="p_Position"></param>
		string GetToolTipText(Position p_Position);
	}
}
