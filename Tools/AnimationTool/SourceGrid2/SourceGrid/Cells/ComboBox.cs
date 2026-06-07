using System;

namespace SourceGrid2.Cells.Virtual
{
	/// <summary>
	/// A cell with a combobox for editor. Use a model with a ICollection for standard values.
	/// </summary>
	public abstract class ComboBox : CellVirtual
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_CellType"></param>
		/// <param name="p_StandardValues"></param>
		/// <param name="p_StandardValuesExclusive"></param>
		public ComboBox(Type p_CellType, System.Collections.ICollection p_StandardValues, bool p_StandardValuesExclusive)
		{
			DataModel = new DataModels.EditorComboBox(p_CellType, p_StandardValues, p_StandardValuesExclusive);
		}
	}
}

namespace SourceGrid2.Cells.Real
{
	/// <summary>
	/// A cell with a combobox for editor. Use a model with a ICollection for standard values.
	/// </summary>
	public class ComboBox : Cell
	{
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="p_Value"></param>
		/// <param name="p_CellType"></param>
		/// <param name="p_StandardValues"></param>
		/// <param name="p_StandardValuesExclusive"></param>
		public ComboBox(object p_Value, Type p_CellType, System.Collections.ICollection p_StandardValues, bool p_StandardValuesExclusive):base(p_Value)
		{
			DataModel = new DataModels.EditorComboBox(p_CellType, p_StandardValues, p_StandardValuesExclusive);
		}
	}
}
